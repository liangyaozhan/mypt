
#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <limits.h>
#include "myptm.h"

#include <Windows.h>

int64_t _mypt_thread_sys_tick_get_ms()
{
    LARGE_INTEGER SystemFrequency, StartTime;
    double ms;
    QueryPerformanceFrequency(&SystemFrequency);
    QueryPerformanceCounter(&StartTime);
    ms = 1000.0 * (double)(StartTime.QuadPart) / (double)SystemFrequency.QuadPart;
    return (int64_t)ms;
}

int64_t g_ms_base;
int32_t g_ms_offset = 0;
int32_t mypt_thread_sys_tick_get_ms()
{
    return (int32_t)(_mypt_thread_sys_tick_get_ms() - g_ms_base) + g_ms_offset;
}

struct _msg
{
    const char *from_thread;
    int value;
};
typedef struct _msg msg_t;

message_queue_def( g_the_msg, 1024 );

struct my_thread_a
{
    MYPT_OBJECT(namea);
    int n;
    const char *name;
};

int my_thread_entry_a( mypt_thread *p )
{
    MYPT_DEF_THIS(p, struct my_thread_a, namea );

    MYPT_BEGIN(p_this->namea);
    printf("thread %s started\n", p_this->name);
    while (1)
    {
        msg_t msg;
        int tx_size;
        printf(" *** thread %s delay1 1000ms n=%d tick=%d\n", p_this->name, p_this->n++, mypt_thread_sys_tick_get_ms() );
        MYPT_DELAY_MS(1000);
        msg.from_thread = p_this->name;
        msg.value = p_this->n;
        message_queue_send( message_queue_ptr(g_the_msg), &msg, sizeof(msg), tx_size);
        printf("-- thread %s txsize=%d\n", p_this->name, tx_size );
        MYPT_DELAY_MS(100);
        //printf("thread %s delay2 1000ms\n", p_this->name);
        //MYPT_DELAY_MS(1000);
        //printf("    -> thread %s delay2 1000ms done\n\n", p_this->name);
    }
    MYPT_END();
}

void my_thread_a_init(struct my_thread_a *p_this, const char *name)
{
    mypt_thread_init(&p_this->namea, my_thread_entry_a);
    p_this->name = name;
    p_this->n = 0;
}

struct my_thread_b
{
    MYPT_OBJECT(nameb);
    int n;
    const char *name;
    struct my_thread_a a1, a2;
    char name_a1[32];
    char name_a2[32];
    int i;
    message_queue_def( msgq, 1024 );
};



int my_thread_entry_b( mypt_thread *p)
{
    MYPT_DEF_THIS(p, struct my_thread_b, nameb );

    MYPT_BEGIN(p_this->nameb);
    printf("thread %s started\n", p_this->name);
    my_thread_a_init(&p_this->a1, "b.a1");
    my_thread_a_init(&p_this->a2, "b.a2");
    myptm_loop_add( p_this->nameb.p_owner, &p_this->a1.namea);
    myptm_loop_add( p_this->nameb.p_owner, &p_this->a2.namea);
    while (1)
    {
        for (p_this->i = 0; p_this->i < 10; p_this->i++)
        {
            printf("thread %s delay 500ms for-loop %d\n", p_this->name, p_this->i);
            MYPT_DELAY_MS(50);
        }
        printf("thread %s delay 5000ms done %d\n\n", p_this->name, p_this->n++);
        MYPT_DELAY_MS(5000);
        printf("thread %s exit n=%d\n\n", p_this->name, p_this->n++);
        while (1){
            int rx_size;
            msg_t msg;
            message_queue_recv( message_queue_ptr(g_the_msg), &msg, sizeof(msg), rx_size );
            if (rx_size == sizeof(msg)){
                printf("    *** msg recv from thread %s value=%d\n", msg.from_thread, msg.value );
            }
        }
    }
    MYPT_END();
}

void my_thread_b_init(struct my_thread_b *p_this, const char *name)
{
    mypt_thread_init(&p_this->nameb, my_thread_entry_b);
    p_this->n = 0;
    p_this->name = name;
    sprintf(p_this->name_a1, "%s's a1", name);
    sprintf(p_this->name_a2, "%s's a2", name);
    my_thread_a_init(&p_this->a1, p_this->name_a1);
    my_thread_a_init(&p_this->a2, p_this->name_a2);
    //message_queue_init_internal_pool( message_queue_ptr( p_this->msgq ), sizeof(msg_t), msgq );
    message_queue_init_this( msgq, sizeof(msg_t) );
    //message_queue_init_internal_pool( p_this, sizeof(msg_t), msgq );
}

int main(int argc, char **argv)
{
    myptm_loop loop;

    struct my_thread_a a1, a2;
    struct my_thread_b b1;

    message_queue_init_internal_pool( message_queue_ptr(g_the_msg), sizeof(msg_t), g_the_msg );
    //message_queue_init_internal_pool( &(g_the_msg), sizeof(msg_t), g_the_msg );

    g_ms_base = _mypt_thread_sys_tick_get_ms();

    my_thread_a_init(&a1, "a1");
    my_thread_a_init(&a2, "a2");
    my_thread_b_init(&b1, "b1");

    myptm_loop_init( &loop );
    myptm_loop_add( &loop, &a1.namea );
    myptm_loop_add( &loop, &a2.namea );
    myptm_loop_add( &loop, &b1.nameb );

    while (1)
    {
        myptm_loop_run( &loop.thread );
    }
}
