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

#include "myptm/myptm_message_queue.h"

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

