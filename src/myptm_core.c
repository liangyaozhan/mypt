/*
 * File: myptm_core.c
 * Author: Liang YaoZhan <262666882@qq.com>
 * Brief:  timer
 *
 * Copyright (c) 2022 - 2023 Liang YaoZhan <262666882@qq.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2023-08-05 Liang YaoZhan <262666882@qq.com> created
 *
 */

#include <assert.h>
#include <string.h>

#include "myptm/myptm_core.h"

#ifndef NULL
#define NULL (void *)0
#endif

static void myptm_thread_make_ready(myptm_thread_t *p_thread);
static void myptm_loop_make_ready(myptm_loop_t *p_this, myptm_thread_t *p_thread);

void myptm_loop_thread_delay(myptm_loop_t *p_this, myptm_thread_t *p_thread, int tick);

static void myptm_timeout_cb__(void *arg){
    myptm_thread_t *p_thread = (myptm_thread_t*)arg;
    myptm_thread_make_ready( p_thread );
    p_thread->err = myptm_ETIMEOUT;
}

void myptm_thread_init(myptm_thread_t *p_thread, int (*entry)(myptm_thread_t *))
{
    memset(p_thread, 0x00, sizeof(*p_thread));
    myptm_thread_t *ptr = (p_thread);
    myptm_queue_init(&ptr->node_state);
    myptm_queue_init(&ptr->node_pending_resource);
    myptm_timer_init(&p_thread->timer, myptm_timeout_cb__, p_thread );
    ptr->entry = entry;
    ptr->state = myptm_STATE_INIT;
}

static void myptm_thread_make_ready(myptm_thread_t *p_thread)
{
    myptm_loop_t *p_loop = p_thread->p_owner;
    myptm_loop_make_ready(p_loop, p_thread);
}

void myptm_thread_startup(myptm_thread_t *p_thread)
{
    if ( p_thread->state == myptm_STATE_INIT){
        myptm_thread_make_ready(p_thread);
    }
}

void myptm_thread_stop(myptm_thread_t *p_thread)
{
    myptm_loop_remove(p_thread->p_owner, p_thread);
}

void myptm_thread_join(myptm_thread_t *p_thread)
{
    if (p_thread->state == myptm_STATE_DEAD){
        assert(!p_thread->p_owner);
    }
    while (p_thread->state != myptm_STATE_DEAD){
        myptm_loop_run( &p_thread->p_owner->thread );
    }
}

void myptm_loop_init(myptm_loop_t *p_this)
{
    myptm_thread_init(&p_this->thread, myptm_loop_run);
    myptm_queue_init(&p_this->head_state_pending);
    myptm_queue_init(&p_this->head_state_running);
    myptm_queue_init(&p_this->clip);
    myptm_tick_init( &p_this->ticker);
    p_this->systick = myptm_thread_sys_tick_get_ms();
}

static void __myptm_queue_item_destroy_thread( myptm_queue_t *ptr )
{
    while (!myptm_queue_empty(ptr)){
        myptm_thread_t *p_thread = myptm_queue_container( ptr->next, myptm_thread_t, node_state );
        myptm_thread_stop( p_thread );
    }
}

void myptm_loop_destroy(myptm_loop_t *p_this)
{
    __myptm_queue_item_destroy_thread( &p_this->clip );
    __myptm_queue_item_destroy_thread( &p_this->head_state_pending );
    __myptm_queue_item_destroy_thread( &p_this->head_state_running );
}

int myptm_loop_run(myptm_thread_t *p)
{
    myptm_thread_t *ptr_thread;
    int32_t now = myptm_thread_sys_tick_get_ms();
    int diff;

    myptm_DEF_THIS(p, myptm_loop_t, thread);

    if (myptm_queue_empty(&p_this->clip)){
        myptm_queue_move( &p_this->head_state_running, &p_this->clip);
    }

    diff = now - p_this->systick;
    if (diff){
        p_this->systick = now;
        myptm_tick_increase( &p_this->ticker, diff );
    }

    while ( !myptm_queue_empty(&p_this->clip)){
        myptm_queue_t *ptr = p_this->clip.next;
        myptm_queue_remove(ptr);
        myptm_queue_insert_tail(&p_this->head_state_running, ptr);
        ptr_thread = myptm_queue_container(ptr, myptm_thread_t, node_state );
        if (ptr_thread->entry && ptr_thread->ref == 0 ){
            ptr_thread->ref++;
            int code = ptr_thread->entry(ptr_thread);
            ptr_thread->ref--;
            if (code == myptm_ENDED){
                myptm_thread_stop( ptr_thread );
            }
        }
    }
    return myptm_WAITING;
}

int myptm_loop_next_tick(myptm_loop_t *p_this)
{
    if ((!myptm_queue_empty( &p_this->clip )) || (!myptm_queue_empty( &p_this->head_state_running ))){
        return 0;
    }
    return myptm_tick_next( &p_this->ticker );
}

void myptm_loop_add(myptm_loop_t *p_this, myptm_thread_t *p_thread)
{
    if (!p_thread)
    {
        return;
    }
    if (p_thread->p_owner)
    {
        return;
    }
    p_thread->p_owner = p_this;

    myptm_queue_insert_tail( &p_this->head_state_pending, &p_thread->node_state );
}
void myptm_loop_timer_start(myptm_loop_t *p_this, myptm_timer_t *p_timer, int tick )
{
    myptm_tick_add_timer( &p_this->ticker, p_timer, tick );
}

void myptm_loop_timer_remove(myptm_loop_t *p_this, myptm_timer_t *p_timer)
{
    myptm_tick_remove_timer( &p_this->ticker, p_timer );
}

void myptm_thread_suspend(myptm_thread_t *p_this)
{
    myptm_loop_t *p_loop = p_this->p_owner;
    myptm_queue_remove( &p_this->node_state );
    myptm_queue_insert_tail( &p_loop->head_state_pending, &p_this->node_state );
    p_this->state = myptm_STATE_PENDING;
    p_this->err = 0;
}

void myptm_thread_prepare_delay(myptm_thread_t *p_this, int tick)
{
    myptm_loop_t *p_loop = p_this->p_owner;
    myptm_thread_suspend(p_this);
    if ( tick != -1){
        p_this->timer.timeout_callback = myptm_timeout_cb__;
        p_this->timer.arg = p_this;
        myptm_tick_add_timer( &p_loop->ticker, &p_this->timer, tick );
    }
}
void myptm_loop_make_ready(myptm_loop_t *p_this, myptm_thread_t *p_thread)
{
    myptm_queue_remove( &p_thread->node_state );
    myptm_queue_insert_tail( &p_this->head_state_running, &p_thread->node_state );
    p_thread->state = myptm_STATE_READY;
}
void myptm_loop_remove(myptm_loop_t *p_this, myptm_thread_t *p_thread)
{
    if ((!p_this) || (p_thread->p_owner != p_this)){
        return ;
    }
    myptm_queue_remove( &p_thread->node_state );
    myptm_queue_remove( &p_thread->node_pending_resource );
    myptm_tick_remove_timer( &p_this->ticker, &p_thread->timer );
    p_thread->p_owner = NULL;
    p_thread->state = myptm_STATE_DEAD;
}

void myptm_sem_init( myptm_sem_t *p_this, int init_count )
{
    myptm_queue_init( &p_this->head_pendings );
    p_this->count = init_count;
}

myptm_err_t myptm_sem_prepare_take( myptm_sem_t *p_this, int delay_tick, myptm_thread_t *p_thread ) /* delay_tick: 0 forevery */
{
    if (p_this->count > 0){
        p_this->count--;
        p_thread->err = myptm_EOK;
        return myptm_EOK;
    }
    myptm_thread_prepare_delay( p_thread, delay_tick );
    myptm_queue_insert_tail( &p_this->head_pendings, &p_thread->node_pending_resource );
    return myptm_EEMPTY;
}

void myptm_thread_resume( myptm_thread_t *p_thread, int code)
{
    myptm_queue_remove( &p_thread->node_pending_resource );
    myptm_tick_remove_timer( &p_thread->p_owner->ticker, &p_thread->timer );
    myptm_thread_make_ready( p_thread );
    p_thread->err = code;
}

void myptm_sem_give( myptm_sem_t *p_this, int count )
{
    int i;

    for (i=0; i<count && !myptm_queue_empty(&p_this->head_pendings); i++){
        myptm_thread_t *p_thread = myptm_queue_container( p_this->head_pendings.next, myptm_thread_t, node_pending_resource );
        myptm_thread_resume( p_thread, myptm_EOK );
    }
    p_this->count += count - i;
}

void myptm_sem_destroy( myptm_sem_t *p_this, int init_count )
{
    while ( !myptm_queue_empty(&p_this->head_pendings) ){
        myptm_thread_t *p_thread = myptm_queue_container( p_this->head_pendings.next, myptm_thread_t, node_pending_resource );
        myptm_thread_resume( p_thread, myptm_ETERMINATED );
    }
}

