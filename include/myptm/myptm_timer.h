/*
 * File: myptm_timer.h
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

#ifndef myptm_TIMER_H
#define myptm_TIMER_H

#include "myptm/myptm_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct myptm_timer_s myptm_timer_t;
struct myptm_tick_s;
typedef struct myptm_tick_s myptm_tick_t;

struct myptm_timer_s
{
/* private: */
    myptm_queue_t       node;
    unsigned int      tick_origin;
    unsigned int      tick;
    myptm_tick_t     *p_ticker;

/* public: */
    void (*timeout_callback)( myptm_timer_t *p_this, void *arg); /*!< timeout callback function */
    void             *arg;
};

void myptm_timer_init( myptm_timer_t *p_this, void (*cb)(myptm_timer_t *p_this, void*arg), void *arg);
void myptm_timer_destroy( myptm_timer_t *p_this );
void myptm_timer_restart(  myptm_timer_t *p_timer );

struct myptm_tick_s
{
    myptm_queue_t head;
};

void myptm_tick_init(  myptm_tick_t *p_this);
void myptm_tick_increase(  myptm_tick_t *p_this, unsigned int tick );
void myptm_tick_destroy(  myptm_tick_t *p_this);

void myptm_tick_add_timer(  myptm_tick_t *p_this, myptm_timer_t *p_timer, int tick);
void myptm_tick_remove_timer(  myptm_tick_t *p_this, myptm_timer_t *p_timer);
int  myptm_tick_next(myptm_tick_t *p_this);

#ifdef __cplusplus
}
#endif


#endif

