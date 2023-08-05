/*
 * File: myptm_core.h
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

#ifndef MYPTM_CORE_H
#define MYPTM_CORE_H

#include <stdint.h>

#include "myptm/myptm_queue.h"
#include "myptm/myptm_timer.h"

#define MYPTM_CPLUSPLUS_BEGIN \
    extern "C"                \
    {
#define MYPTM_CPLUSPLUS_END }

#ifdef __cplusplus
MYPTM_CPLUSPLUS_BEGIN
#endif

/************************************************** myptm_thread_t ***************************************/
/************************************************** myptm_thread_t ***************************************/

#define myptm_WAITING 0
#define myptm_YIELDED 1
#define myptm_EXITED 2
#define myptm_PAUSE 3
#define myptm_ENDED 4

typedef enum _mypt_state
{
    myptm_STATE_INIT, /* */
    myptm_STATE_READY,
    myptm_STATE_PENDING,
    myptm_STATE_DEAD,
} myptm_state_t;
typedef enum _mypt_err_s
{
    myptm_EOK,
    myptm_EEMPTY,
    myptm_ETERMINATED,
    myptm_ETIMEOUT,
} myptm_err_t;

struct _myptm_loop;
typedef struct _myptm_loop myptm_loop_t;
typedef struct _mypt_thread
{
    int lc;
    int (*entry)(struct _mypt_thread *);
    struct _mypt_thread *next;
    myptm_queue_t node_state;
    myptm_queue_t node_pending_resource;
    myptm_timer_t timer;
    struct _myptm_loop *p_owner;
    uint32_t ref;
    myptm_err_t err;
    myptm_state_t state;
} myptm_thread_t;
#define myptm_OBJECT(obj) myptm_thread_t obj

struct _myptm_loop
{
    myptm_thread_t thread;
    myptm_queue_t head_state_pending;
    myptm_queue_t head_state_running;
    myptm_queue_t clip;
    myptm_tick_t ticker;
    int32_t systick;
};

struct _mypt_sem
{
    int count;
    myptm_queue_t head_pendings;
};
typedef struct _mypt_sem myptm_sem_t;

/*
 绕过这个BUG:
    msvc中, __LINE__非常量
*/
#define _DBJ_CONCATENATE_(a, b) a##b
#define _DBJ_CONCATENATE(a, b) _DBJ_CONCATENATE_(a, b)
#define CONSTEXPR_LINE _DBJ_CONCATENATE(__LINE__, U)

#define myptm_LC_SET(s) \
    s = CONSTEXPR_LINE; \
    case CONSTEXPR_LINE:

#define LC_END(s)    \
    myptm_LC_SET(s); \
    }

#define myptm_DEF_THIS(ptr, type, obj) type *p_this = myptm_container_of((ptr), type, obj)
#define myptm_BEGIN(obj)                         \
    {                                            \
        myptm_thread_t *__p_super_this = &(obj); \
        char myptm_YIELD_FLAG = 1;               \
        switch ((__p_super_this)->lc)            \
        {                                        \
        case 0:

#define myptm_END()                        \
    __crt_label_end:                       \
    case -1:                               \
    LC_END((__p_super_this)->lc);          \
    myptm_YIELD_FLAG = 0;                  \
    (__p_super_this)->lc = CONSTEXPR_LINE; \
    if ((int)CONSTEXPR_LINE < 0)           \
        goto __crt_label_end;              \
    return myptm_ENDED;                    \
    }

#define myptm_WAIT_UNTIL(condition)         \
    do                                      \
    {                                       \
        myptm_LC_SET((__p_super_this)->lc); \
        if (!(condition))                   \
        {                                   \
            return myptm_WAITING;           \
        }                                   \
    } while (0)

#define myptm_WAIT_WHILE(cond) myptm_WAIT_UNTIL(!(cond))

#define myptm_RESTART()             \
    do                              \
    {                               \
        (__p_super_this)->lc = 0; \
        return myptm_WAITING;       \
    } while (0)

#define myptm_EXIT() goto __crt_label_end;

#define myptm_YIELD()                       \
    do                                      \
    {                                       \
        myptm_YIELD_FLAG = 0;               \
        myptm_LC_SET((__p_super_this)->lc); \
        if (myptm_YIELD_FLAG == 0)          \
        {                                   \
            return myptm_YIELDED;           \
        }                                   \
    } while (0)

#define myptm_sem(sem) myptm_sem_t sem

#define myptm_error_code() (__p_super_this->err)

#define myptm_sem_take(p_sem, timeout)                                                                        \
    do                                                                                                        \
    {                                                                                                         \
        myptm_sem_t *__ptr = (p_sem);                                                                         \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_sem)) == sizeof(myptm_sem_t)) ? 1 : -1]; \
        myptm_err_t _local_err_pt = myptm_sem_prepare_take(__ptr, timeout, __p_super_this);                   \
        if (_local_err_pt == myptm_EEMPTY)                                                                    \
        {                                                                                                     \
            myptm_YIELD();                                                                                    \
        }                                                                                                     \
    } while (0)

#define myptm_DO                          \
    do                                    \
    {                                     \
        myptm_LC_SET(__p_super_this->lc); \
    } while (0);

#define myptm_CONTINUE return myptm_WAITING
#define myptm_WHILE(cond)         \
    do                            \
    {                             \
        if (cond)                 \
            return myptm_WAITING; \
    } while (0)

#define myptm_thread_delay(tick)                          \
    do                                                    \
    {                                                     \
        myptm_thread_prepare_delay(__p_super_this, tick); \
        myptm_YIELD();                                    \
    } while (0)

/* you should define this function before using myptm_DELAY */
int32_t myptm_thread_sys_tick_get_ms();

void myptm_sem_init(myptm_sem_t *p_this, int init_count);
myptm_err_t myptm_sem_prepare_take(myptm_sem_t *p_this, int delay_tick, myptm_thread_t *p_thread); /* delay_tick: 0 forevery */
void myptm_sem_give(myptm_sem_t *p_this, int count);
void myptm_sem_destroy(myptm_sem_t *p_this, int init_count);

void myptm_thread_init(myptm_thread_t *p_thread, int (*entry)(myptm_thread_t *));
void myptm_thread_startup(myptm_thread_t *p_thread);
void myptm_thread_prepare_delay(myptm_thread_t *p_this, int tick);
void myptm_thread_stop(myptm_thread_t *p_thread);
void myptm_thread_resume(myptm_thread_t *p_thread, int code);
void myptm_thread_join(myptm_thread_t *p_thread);

void myptm_loop_init(myptm_loop_t *p_this);
int myptm_loop_run(myptm_thread_t *p_thread);
void myptm_loop_add(myptm_loop_t *p_this, myptm_thread_t *p_thread);
void myptm_loop_timer_start(myptm_loop_t *p_this, myptm_timer_t *p_timer, int ms );
void myptm_loop_timer_remove(myptm_loop_t *p_this, myptm_timer_t *p_timer);
void myptm_loop_remove(myptm_loop_t *p_this, myptm_thread_t *p_thread);
void myptm_loop_destroy(myptm_loop_t *p_this);
int myptm_loop_next_tick(myptm_loop_t *p_this);

#ifdef __cplusplus
MYPTM_CPLUSPLUS_END
#endif

#endif
