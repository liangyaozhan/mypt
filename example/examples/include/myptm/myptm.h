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

#ifndef MYPTM_H
#define MYPTM_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * File: myptm_queue.h
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

#ifndef myptm_QUEUE_H
#define myptm_QUEUE_H

#include <assert.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct myptm_queue_s myptm_queue_t;

    struct myptm_queue_s
    {
        myptm_queue_t *prev;
        myptm_queue_t *next;
    };

#define myptm_queue_init(q) \
    do                      \
    {                       \
        (q)->prev = q;      \
        (q)->next = q;      \
    } while (0)

#define myptm_queue_empty(h) \
    (h == (h)->prev)

#define myptm_queue_insert_head(h, x) \
    do                                \
    {                                 \
        (x)->next = (h)->next;        \
        (x)->next->prev = x;          \
        (x)->prev = h;                \
        (h)->next = x;                \
    } while (0)

#define myptm_queue_insert_after myptm_queue_insert_head

#define myptm_queue_insert_tail(h, x)                                                     \
    do                                                                                    \
    {                                                                                     \
        assert((x)->next == (x)->prev && (x)->next == (x) && "insert twice or not init"); \
        (x)->prev = (h)->prev;                                                            \
        (x)->prev->next = x;                                                              \
        (x)->next = h;                                                                    \
        (h)->prev = x;                                                                    \
    } while (0)

#define myptm_queue_head(h) \
    (h)->next

#define myptm_queue_last(h) \
    (h)->prev

#define myptm_queue_next(q) \
    (q)->next

#define myptm_queue_prev(q) \
    (q)->prev

#define myptm_queue_remove(x)              \
    do                                     \
    {                                      \
        myptm_queue_t *p_prev = (x)->prev; \
        myptm_queue_t *p_next = (x)->next; \
        p_next->prev = p_prev;             \
        p_prev->next = p_next;             \
        myptm_queue_init(x);               \
    } while (0)

/**
 * origin:      h ==A=== q  ====B=== h
 * after list 1:      h ==A=== h
 * after list 2:      n q==B=== n
 */
#define myptm_queue_split(h, q, n) \
    (n)->prev = (h)->prev;         \
    (n)->prev->next = n;           \
    (n)->next = q;                 \
    (h)->prev = (q)->prev;         \
    (h)->prev->next = h;           \
    (q)->prev = n;

#define myptm_queue_add(h, n)    \
    (h)->prev->next = (n)->next; \
    (n)->next->prev = (h)->prev; \
    (h)->prev = (n)->prev;       \
    (h)->prev->next = h;

#define myptm_queue_move(h, _new)         \
    do                                    \
    {                                     \
        myptm_queue_insert_tail(h, _new); \
        myptm_queue_remove(h);            \
    } while (0)

#ifndef myptm_container_of
#define myptm_container_of(p, t, m) \
    ((t *)((char *)p - (char *)(&(((t *)0)->m))))
#endif

#define myptm_queue_container(q, type, m) myptm_container_of((q), type, m)

#define myptm_queue_for_each_container(pos, head, t, member)   \
    for (pos = myptm_queue_container((head)->next, t, member); \
         &pos->member != (head);                               \
         pos = myptm_queue_container(pos->member.next, t, member))

#define myptm_queue_for_each_container_safe(pos, n, head, t, member) \
    for (pos = myptm_queue_container((head)->next, t, member),       \
        n = myptm_queue_container(pos->member.next, t, member);      \
         &pos->member != (head);                                     \
         pos = n, n = myptm_queue_container(n->member.next, t, member))

#ifdef __cplusplus
}
#endif

#endif
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


#define incompatible_pointor_type_check(ptr, t) \
    extern int incompatible_pointor_type_check_array[(sizeof(*(ptr)) == sizeof(t)) ? 1 : -1];

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
    myptm_thread_t *p_thread_current;
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

#define myptm_RESTART()           \
    do                            \
    {                             \
        (__p_super_this)->lc = 0; \
        return myptm_WAITING;     \
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

#define myptm_sem_take(p_sem, timeout)                                                                            \
    do                                                                                                            \
    {                                                                                                             \
        myptm_YIELD_FLAG = 0;                                                                                     \
        myptm_LC_SET((__p_super_this)->lc);                                                                       \
        if (myptm_YIELD_FLAG == 0)                                                                                \
        {                                                                                                         \
            myptm_sem_t *__ptr = (p_sem);                                                                         \
            extern int incompatible_pointor_type_check_array[(sizeof(*(p_sem)) == sizeof(myptm_sem_t)) ? 1 : -1]; \
            myptm_err_t _local_err_pt = myptm_sem_prepare_take(__ptr, timeout, __p_super_this);                   \
            if (_local_err_pt == myptm_EEMPTY)                                                                    \
            {                                                                                                     \
                return myptm_YIELDED;                                                                             \
            }                                                                                                     \
        }                                                                                                         \
    } while (0)
#define myptm_sem_reset(p_sem)                                                                                \
    do                                                                                                        \
    {                                                                                                         \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_sem)) == sizeof(myptm_sem_t)) ? 1 : -1]; \
        myptm_sem_t *__ptr = (p_sem);                                                                         \
        __ptr->count = 0;                                                                                     \
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
void myptm_sem_give_all(myptm_sem_t *p_this);
void myptm_sem_destroy(myptm_sem_t *p_this);

void myptm_thread_init(myptm_thread_t *p_thread, int (*entry)(myptm_thread_t *));
void myptm_thread_startup(myptm_thread_t *p_thread);
void myptm_thread_prepare_delay(myptm_thread_t *p_this, int tick);
void myptm_thread_stop(myptm_thread_t *p_thread);
void myptm_thread_resume(myptm_thread_t *p_thread, int code);
void myptm_thread_join(myptm_thread_t *p_thread);

void myptm_loop_init(myptm_loop_t *p_this);
void myptm_loop_destroy(myptm_loop_t *p_this);
int myptm_loop_empty( myptm_loop_t *p_this );
int  myptm_loop_poll(myptm_loop_t *p_this);
void myptm_loop_add(myptm_loop_t *p_this, myptm_thread_t *p_thread);
void myptm_loop_timer_start(myptm_loop_t *p_this, myptm_timer_t *p_timer, int ms);
void myptm_loop_timer_remove(myptm_loop_t *p_this, myptm_timer_t *p_timer);
void myptm_loop_remove(myptm_loop_t *p_this, myptm_thread_t *p_thread);
void myptm_loop_destroy(myptm_loop_t *p_this);
int myptm_loop_next_tick(myptm_loop_t *p_this);

#define myptm_condition_variable myptm_sem_t
#define myptm_condition_variable_init(pthis) myptm_sem_init((pthis), 0)
#define myptm_condition_variable_destroy(pthis) myptm_sem_destroy((pthis))
#define myptm_condition_variable_wait(pthis) myptm_sem_take(pthis, -1)
#define myptm_condition_variable_wait_for(pthis, ms) myptm_sem_take(pthis, ms)
#define myptm_condition_variable_notify_one(pthis) myptm_sem_give(pthis, 1)
#define myptm_condition_variable_notify_all(pthis) myptm_sem_give_all(pthis)

#ifdef __cplusplus
MYPTM_CPLUSPLUS_END
#endif



#endif
/*
 * File: myptm_message_queue.h
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

#ifndef MYPTM_MESSAGE_Q_H
#define MYPTM_MESSAGE_Q_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    struct _messagequeue
    {
        uint16_t msg_size; /**< message size of each message */
        uint16_t max_msgs; /**< max number of messages */

        uint16_t entry; /**< index of messages in the queue */
        myptm_sem_t sem_read;
        myptm_sem_t sem_empty_buffer;

        void *msg_queue_head; /**< list head */
        void *msg_queue_tail; /**< list tail */
        void *msg_queue_free; /**< pointer indicated the free node of queue */

        void *msg_pool; /**< start address of message queue */
    };

    typedef struct _messagequeue message_queue;

    void _message_queue_init(message_queue *p_this, void *msgpool, int msg_size, int pool_size);
    int _message_queue_recv(message_queue *p_this, void *buffer, int size);
    int _message_queue_send(message_queue *p_this, const void *buffer, int size);

#ifndef myptm_container_of
#define myptm_container_of(p, t, m) \
    ((t *)((char *)p - (char *)(&(((t *)0)->m))))
#endif

#define message_queue_def(obj, pool_size)            \
    struct __obj_s##obj                              \
    {                                                \
        message_queue q;                             \
        uint32_t pool[pool_size / sizeof(uint32_t)]; \
    } obj
#define message_queue_ptr(obj) (&(obj).q)

#define message_queue_init_internal_pool(p, msg_size, obj)                                                \
    do                                                                                                    \
    {                                                                                                     \
        message_queue *__ptr_q = p;                                                                       \
        extern int incompatible_pointor_type_check_array[(sizeof(*p) == sizeof(message_queue)) ? 1 : -1]; \
        struct __obj_s##obj *__ptr = myptm_container_of(__ptr_q, struct __obj_s##obj, q);                 \
        _message_queue_init(&__ptr->q, __ptr->pool, msg_size, sizeof(__ptr->pool));                       \
    } while (0)

#define message_queue_init_this(obj, msg_size) message_queue_init_internal_pool(message_queue_ptr(p_this->obj), msg_size, obj)

#define message_queue_send(p_mq, buffer, size, timeout)                                                        \
    do                                                                                                         \
    {                                                                                                          \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_mq)) == sizeof(message_queue)) ? 1 : -1]; \
        myptm_sem_take(&p_mq->sem_empty_buffer, timeout);                                                      \
        if (__p_super_this->err == myptm_EOK)                                                                  \
            _message_queue_send((p_mq), (buffer), (size));                                                     \
    } while (0)

#define message_queue_recv(p_mq, buffer, size, rxsize, timeout)                                                \
    do                                                                                                         \
    {                                                                                                          \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_mq)) == sizeof(message_queue)) ? 1 : -1]; \
        myptm_sem_take(&p_mq->sem_read, timeout);                                                              \
        rxsize = 0;                                                                                            \
        if (__p_super_this->err == myptm_EOK)                                                                  \
            rxsize = _message_queue_recv((p_mq), (buffer), (size));                                            \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* header file once */

#ifdef DEF_MYPT_C_FUNCTION_BODY

#ifdef __cplusplus
extern "C"
{
#endif
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


#ifndef NULL
#define NULL (void *)0
#endif

static void myptm_thread_make_ready(myptm_thread_t *p_thread);
static void myptm_loop_make_ready(myptm_loop_t *p_this, myptm_thread_t *p_thread);

void myptm_loop_thread_delay(myptm_loop_t *p_this, myptm_thread_t *p_thread, int tick);

static void myptm_timeout_cb__(myptm_timer_t *p_timer, void *arg){
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
    while (p_thread->state != myptm_STATE_DEAD && myptm_loop_poll( p_thread->p_owner ) != -1){
    }
}

void myptm_loop_init(myptm_loop_t *p_this)
{
    myptm_queue_init(&p_this->head_state_pending);
    myptm_queue_init(&p_this->head_state_running);
    myptm_queue_init(&p_this->clip);
    myptm_tick_init( &p_this->ticker);
    p_this->systick = myptm_thread_sys_tick_get_ms();
    p_this->p_thread_current = NULL;
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
    myptm_tick_destroy( &p_this->ticker );
}

int myptm_loop_poll( myptm_loop_t *p_this )
{
    myptm_thread_t *ptr_thread;
    myptm_thread_t *ptr_thread_old;
    int32_t now = myptm_thread_sys_tick_get_ms();
    int diff;

    if (myptm_queue_empty(&p_this->clip)){
        myptm_queue_move( &p_this->head_state_running, &p_this->clip);
    }

    diff = now - p_this->systick;
    if (diff){
        p_this->systick = now;
        myptm_tick_increase( &p_this->ticker, diff );
    }
    ptr_thread_old = p_this->p_thread_current;

    while ( !myptm_queue_empty(&p_this->clip)){
        myptm_queue_t *ptr = p_this->clip.next;
        myptm_queue_remove(ptr);
        myptm_queue_insert_tail(&p_this->head_state_running, ptr);
        ptr_thread = myptm_queue_container(ptr, myptm_thread_t, node_state );
        if (ptr_thread->entry && ptr_thread->ref == 0 ){
            ptr_thread->ref++;
            p_this->p_thread_current = ptr_thread;
            int code = ptr_thread->entry(ptr_thread);
            ptr_thread->ref--;
            if (code == myptm_ENDED){
                myptm_thread_stop( ptr_thread );
            }
        }
    }
    p_this->p_thread_current = ptr_thread_old;

    myptm_queue_for_each_container( ptr_thread, &p_this->head_state_running, myptm_thread_t, node_state ){
        if (ptr_thread->ref == 0 && ptr_thread->state == myptm_STATE_READY){
            return 0;
        }
    }
    return myptm_tick_next( &p_this->ticker );
}

int myptm_loop_empty( myptm_loop_t *p_this )
{
    return myptm_queue_empty( &p_this->ticker.head ) && myptm_queue_empty( &p_this->head_state_pending ) && myptm_queue_empty( &p_this->head_state_running ) && myptm_queue_empty(&p_this->clip);
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
    p_this->err = myptm_EOK;
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
    if (!p_this){
        return ;
    }
    myptm_queue_remove( &p_thread->node_state );
    myptm_queue_remove( &p_thread->node_pending_resource );
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
    p_thread->err = (myptm_err_t)code;
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

void myptm_sem_give_all( myptm_sem_t *p_this )
{
    for (; !myptm_queue_empty(&p_this->head_pendings); ){
        myptm_thread_t *p_thread = myptm_queue_container( p_this->head_pendings.next, myptm_thread_t, node_pending_resource );
        myptm_thread_resume( p_thread, myptm_EOK );
    }
}

void myptm_sem_destroy( myptm_sem_t *p_this )
{
    while ( !myptm_queue_empty(&p_this->head_pendings) ){
        myptm_thread_t *p_thread = myptm_queue_container( p_this->head_pendings.next, myptm_thread_t, node_pending_resource );
        myptm_thread_resume( p_thread, myptm_ETERMINATED );
    }
}

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



void myptm_timer_init( myptm_timer_t *p_this, void (*cb)(myptm_timer_t *p_this, void*arg), void *arg)
{
    myptm_queue_init( &p_this->node );
    p_this->timeout_callback = cb;
    p_this->arg = arg;
    p_this->tick = 0;
    p_this->p_ticker = NULL;
}
void myptm_timer_destroy( myptm_timer_t *p_this )
{
    myptm_queue_remove( &p_this->node );
    p_this->timeout_callback = 0;
}
void myptm_timer_restart( myptm_timer_t *p_timer )
{
    myptm_tick_t *p_tick = p_timer->p_ticker;
    if (!p_tick)return ;
    myptm_tick_remove_timer( p_tick, p_timer );
    myptm_tick_add_timer(  p_tick, p_timer, p_timer->tick_origin );
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
                    pt->timeout_callback(pt, pt->arg);
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
    p_timer->tick_origin = tick;
    p_timer->p_ticker = p_tick;
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

/*
 * File: myptm_message_queue.c
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


#include <string.h>


#ifndef NULL
#define NULL (void *)0
#endif

struct mq_message
{
    struct mq_message *next;
};

#define ___ALIGN(size, align) (((size) + (align)-1) & ~((align)-1))

void _message_queue_init(message_queue *p_this, void *msgpool, int msg_size, int pool_size)
{
    int temp;
    struct mq_message *head;
    int msg_align_size = ___ALIGN(msg_size, sizeof(uint32_t));
    p_this->msg_pool = msgpool;
    p_this->msg_size = msg_size;
    p_this->max_msgs = (uint16_t)(pool_size / (msg_align_size + sizeof(struct mq_message)));
    p_this->msg_queue_head = 0;
    p_this->msg_queue_tail = 0;
    p_this->msg_queue_free = 0;

    for (temp = 0; temp < p_this->max_msgs; temp++)
    {
        head = (struct mq_message *)((uint8_t *)p_this->msg_pool +
                                     temp * (msg_align_size + sizeof(struct mq_message)));
        head->next = (struct mq_message *)p_this->msg_queue_free;
        p_this->msg_queue_free = head;
    }

    p_this->entry = 0;
    myptm_sem_init(&p_this->sem_empty_buffer, p_this->max_msgs);
    myptm_sem_init(&p_this->sem_read, 0);
}

int _message_queue_recv(message_queue *p_this, void *buffer, int size)
{
    struct mq_message *msg;
    if (p_this->entry <= 0){
        return -1;
    }
    msg = (struct mq_message *)p_this->msg_queue_head;
    p_this->msg_queue_head = msg->next;
    if (p_this->msg_queue_tail == msg)
        p_this->msg_queue_tail = NULL;
    p_this->entry--;
    size = size > p_this->msg_size ? p_this->msg_size : size;
    memcpy(buffer, msg + 1, size);
    msg->next = (struct mq_message *)p_this->msg_queue_free;
    p_this->msg_queue_free = msg;
    myptm_sem_give( &p_this->sem_empty_buffer, 1 );
    return size;
}

int _message_queue_send(message_queue *p_this, const void *buffer, int size)
{
    struct mq_message *msg;
    if (size > p_this->msg_size)
        size = p_this->msg_size;
    msg = (struct mq_message *)p_this->msg_queue_free;
    if (msg == NULL)
    {
        return -2;
    }
    if (p_this->entry == 0xffff)
    {
        return -3; /* value overflowed */
    }
    p_this->msg_queue_free = msg->next;
    msg->next = NULL;
    memcpy(msg + 1, buffer, size);
    if (p_this->msg_queue_tail != NULL)
    {
        ((struct mq_message *)p_this->msg_queue_tail)->next = msg;
    }
    p_this->msg_queue_tail = msg;
    if (p_this->msg_queue_head == NULL)
        p_this->msg_queue_head = msg;

    p_this->entry++;
    myptm_sem_give( &p_this->sem_read, 1);

    return size;
}


#ifdef __cplusplus
}
#endif
#endif /* source file def: DEF_MYPT_C_FUNCTION_BODY */

#ifdef __cplusplus

#ifndef MYPTM_HPP
#define MYPTM_HPP


/*!
    The version of C++ standard that is being used.
    The C++11 standard.
*/
#ifndef MY_HAS_CPP11
#ifdef _MSVC_LANG
#if _MSVC_LANG > 199711L
#define MY_HAS_CPP11 1
#else /* _MSVC_LANG > 199711L */
#define MY_HAS_CPP11 0
#endif /* _MSVC_LANG > 199711L */
#else  /* _MSVC_LANG */
#if __cplusplus > 199711L
#define MY_HAS_CPP11 1
#else /* __cplusplus > 199711L */
#define MY_HAS_CPP11 0
#endif /* __cplusplus > 199711L */
#endif /* _MSVC_LANG */
#endif /* MY_HAS_CPP11 */

#if MY_HAS_CPP11
#include <functional>
#include <memory>
#include <exception>
#endif

namespace myptm
{
    class loop;
    class timer;
    class noncopyable
    {
        noncopyable(const noncopyable&) = delete;
        noncopyable&operator =(const noncopyable&) = delete;
#if MY_HAS_CPP11
        noncopyable(noncopyable&&) = delete;
        noncopyable&operator =(noncopyable&&) = delete;
#endif
    public:
        noncopyable(){}
        ~noncopyable(){}
    };
    class thread: public noncopyable
    {
        myptm_thread_t th;
        loop *p_loop;

        static int entry(myptm_thread_t *p)
#if MY_HAS_CPP11
        noexcept
#endif
        {
            myptm_DEF_THIS(p, thread, th);
            int code;

            try {
                code = p_this->run();
            }catch (...){
                assert( 0 && "exception not supported in a thread");
            }
            return code;
        }
        friend class loop;

    protected:
        virtual int run() { return myptm_ENDED; }

    public:
        thread()
        {
            p_loop = NULL;
            myptm_thread_init(&this->th, entry);
        }
        virtual ~thread()
        {
            myptm_thread_stop(&this->th);
            myptm_thread_join(&this->th);
        }
        void startup()
        {
            myptm_thread_startup(&this->th);
        }

        void stop()
        {
            myptm_thread_stop(&this->th);
        }
        void join()
        {
            myptm_thread_join(&this->th);
        }
        myptm_thread_t &thread_object(){ return this->th;}
    };
    class timer:public noncopyable
    {
        friend class loop;
        loop *p_lp;
        myptm_timer_t tm;
#if MY_HAS_CPP11
        typedef std::function<void(timer&tm)> callback_t;
#else
        void (*callback_t)(timer &tm );
#endif
        callback_t ptr_callback;

        static void callback(myptm_timer_t *p_timer, void *arg)
#if MY_HAS_CPP11
        noexcept
#endif
        {
            timer *p_this = myptm_container_of(p_timer, timer, tm);
            if (p_this->ptr_callback)
            {
                try {
                    callback_t func = p_this->ptr_callback;
                    func(*p_this);
                }catch (...){
                    assert( 0 && "exception not supported in a timer callback");
                }
            }
        }

    public:
        int tick;
        void set_callback( callback_t cb )
        {
            this->ptr_callback = cb;
        }
        void set_tick( int t){ this->tick = t;}

        timer() : p_lp(NULL)
        {
            tick = 0;
            myptm_timer_init(&this->tm, callback, 0);
        }
        ~timer()
        {
            myptm_timer_destroy(&this->tm);
        }
    };
    class loop: public noncopyable
    {
        myptm_loop_t lp;

    public:
        loop()
        {
            myptm_loop_init(&this->lp);
        }
        myptm_loop_t &loop_get(){return this->lp;}

        bool empty() { return myptm_loop_empty(&this->lp); }
        int poll() { return myptm_loop_poll(&this->lp); }
        void add(thread &t) { t.p_loop = this;return myptm_loop_add(&this->lp, &t.th); }
        void remove(thread &t) { t.p_loop = NULL; return myptm_loop_remove(&this->lp, &t.th); }
        void start(timer &t ) { return myptm_loop_timer_start(&this->lp, &t.tm, t.tick); }
        void remove(timer &t) { return myptm_loop_timer_remove(&this->lp, &t.tm); }
        void stop(){ myptm_loop_destroy(&this->lp );}

#if MY_HAS_CPP11
        void timer_start( std::function<void()> funobj, int tick) {
            if (!funobj) throw std::bad_exception();
            std::shared_ptr<timer> p_timer = std::make_shared<timer>();
            if (!p_timer)throw std::bad_exception();
            p_timer->set_callback([funobj, p_timer](timer &tm ){
                funobj();
                p_timer->set_callback( nullptr );
            } );
            p_timer->set_tick( tick );
            this->start( *p_timer );
        }
#endif

        ~loop()
        {
            myptm_loop_destroy(&this->lp);
        }
    };
}

#endif
#endif /* source file def: DEF_MYPT_C_FUNCTION_BODY */
