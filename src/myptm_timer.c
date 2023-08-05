/*
 * File: myptm_timer.c
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

#include "myptm/myptm_timer.h"


void myptm_timer_init( myptm_timer_t *p_this, void (*cb)(void*), void *arg)
{
    myptm_queue_init( &p_this->node );
    p_this->timeout_callback = cb;
    p_this->arg = arg;
    p_this->tick = 0;
}
void myptm_timer_destroy( myptm_timer_t *p_this )
{
    myptm_queue_remove( &p_this->node );
    p_this->timeout_callback = 0;
}

void myptm_tick_init(  myptm_tick_t *p_tick)
{
    myptm_queue_init(&p_tick->head);
}

void myptm_tick_increase(  myptm_tick_t *p_tick, unsigned int tick )
{
    while (tick > 0 && (!myptm_queue_empty(&p_tick->head)) ){
        myptm_timer_t *p_timer;
        p_timer = myptm_queue_container(p_tick->head.next, myptm_timer_t, node);
        if (p_timer->tick < tick){
            tick -= p_timer->tick;
            p_timer->tick = 0;
        } else {
            p_timer->tick -= tick;
            tick = 0;
            if ( p_timer->tick > 0){
                return ;
            }
        }
        while ( !myptm_queue_empty(&p_tick->head)){
            myptm_timer_t *pt = myptm_queue_container(p_tick->head.next, myptm_timer_t, node);
            if (pt->tick == 0){
                myptm_queue_remove( &pt->node );
                if (pt->timeout_callback){
                    pt->timeout_callback(pt->arg);
                }
            }else{
                break;
            }
        }
    }
}

void myptm_tick_destroy(  myptm_tick_t *p_this)
{
    while (!myptm_queue_empty(&p_this->head)){
        myptm_queue_remove( p_this->head.next );
    }
}

void myptm_tick_add_timer(  myptm_tick_t *p_tick, myptm_timer_t *p_timer, int tick)
{
    myptm_timer_t *iter;
    myptm_queue_for_each_container(iter, &p_tick->head, myptm_timer_t, node){
        if (tick > iter->tick){
            tick -= iter->tick;
        } else {
            break;
        }
    }
    p_timer->tick = tick;
    myptm_queue_insert_tail( &iter->node, &p_timer->node );
    if ( &iter->node != &p_tick->head){
        iter->tick -= tick;
    }
}

void myptm_tick_remove_timer(  myptm_tick_t *p_tick, myptm_timer_t *p_timer)
{
    if ( myptm_queue_empty(&p_timer->node )){
        return ;
    }
    if ( p_timer->node.next != &p_tick->head ){
        myptm_timer_t *p_timer_next = myptm_queue_container(p_timer->node.next, myptm_timer_t, node);
        p_timer_next->tick += p_timer->tick;
    }
    myptm_queue_remove( &p_timer->node );
}

int  myptm_tick_next(myptm_tick_t *p_this)
{
    if (myptm_queue_empty(&p_this->head)){
        return -1;
    }
    myptm_timer_t *pt = myptm_queue_container(p_this->head.next, myptm_timer_t, node);
    return pt->tick;
}

