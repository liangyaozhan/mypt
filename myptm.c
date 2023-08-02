
#include <string.h>

#include "myptm.h"

#ifndef NULL
#define NULL (void *)0
#endif

void myptm_loop_init(myptm_loop *p_this)
{
    mypt_thread_init(&p_this->thread, myptm_loop_run);
    p_this->pending = NULL;
    p_this->running = NULL;
}

int myptm_loop_run(myptm_loop *p_this)
{
    MYPT_BEGIN(p_this->thread)

    while (1)
    {
        mypt_thread *ptr = p_this->running;
        mypt_thread **pp = &p_this->running;
        while (ptr && ptr->entry)
        {
            int code;
            if (!ptr->mark_to_delete)
            {
                code = (*ptr->entry)(ptr);
            }
            else
            {
                code = MYPT_ENDED;
            }
            if (code == MYPT_ENDED)
            {
                *pp = ptr->next;
                ptr->next = NULL;
                ptr->p_owner = NULL;
                ptr->mark_to_delete++;
                ptr = *pp;
            }
            else
            {
                pp = &ptr->next;
                ptr = ptr->next;
            }
        }
        if (p_this->pending)
        {
            mypt_thread *p_end = p_this->running;
            if (p_end == NULL)
            {
                p_this->running = p_this->pending;
            }
            else
            {
                while (p_end->next)
                {
                    p_end = p_end->next;
                }
                p_end->next = p_this->pending;
            }
            p_this->pending = NULL;
        }
        /* mark deleted */
        MYPT_YIELD();
    }
    MYPT_END();
}

void myptm_loop_add(myptm_loop *p_this, mypt_thread *p_thread)
{
    mypt_thread *p_end = p_this->pending;
    if (!p_thread)
    {
        return;
    }
    if (p_thread->p_owner)
    {
        return;
    }
    p_thread->p_owner = p_this;
    if (p_end == 0)
    {
        p_this->pending = p_thread;
    }
    else
    {
        while (p_end->next)
        {
            p_end = p_end->next;
        }
        p_end->next = p_thread;
    }
}

#if 0
struct sgl_list_node;
struct sgl_list_node
{
    struct sgl_list_node *next;
};

static mypt_thread **simple_list_del( mypt_thread **head, mypt_thread *p )
{
    while ( *head ){
        if (p == *head ){
            *head = p->next;
            p->next = 0;
            return head;
        }
        head = &(*head)->next;
    }
    return NULL;
}
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
}

int _message_queue_recv(message_queue *p_this, void *buffer, int size)
{
    struct mq_message *msg;
    if (p_this->entry <= 0)
        return -1;
    msg = (struct mq_message *)p_this->msg_queue_head;
    p_this->msg_queue_head = msg->next;
    if (p_this->msg_queue_tail == msg)
        p_this->msg_queue_tail = NULL;
    p_this->entry--;
    size = size > p_this->msg_size ? p_this->msg_size : size;
    memcpy(buffer, msg + 1, size);
    msg->next = (struct mq_message *)p_this->msg_queue_free;
    p_this->msg_queue_free = msg;
    return size;
}

int _message_queue_send(message_queue *p_this, const void *buffer, int size )
{
    struct mq_message *msg;
    if (size > p_this->msg_size)
        size = p_this->msg_size;
    msg = (struct mq_message *)p_this->msg_queue_free;
    if (msg == NULL)
    {
        return -2;
    }
    if(p_this->entry == 0xffff )
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

    p_this->entry ++;

    return size;
}
