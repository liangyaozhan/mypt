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
#include "myptm/myptm_core.h"

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

#define message_queue_send(p_mq, buffer, size, timeout )                                            \
    do                                                                                                         \
    {                                                                                                          \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_mq)) == sizeof(message_queue)) ? 1 : -1]; \
        myptm_sem_take(&p_mq->sem_empty_buffer, timeout);                                                      \
        if (__p_super_this->err == myptm_EOK)                                                                   \
            _message_queue_send((p_mq), (buffer), (size));                                                     \
    } while (0)

#define message_queue_recv(p_mq, buffer, size, rxsize, timeout )                                                     \
    do                                                                                                         \
    {                                                                                                          \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_mq)) == sizeof(message_queue)) ? 1 : -1]; \
        myptm_sem_take(&p_mq->sem_read, timeout);                                                              \
        if (__p_super_this->err == myptm_EOK)                                                                   \
            rxsize = _message_queue_recv((p_mq), (buffer), (size));                    \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif
