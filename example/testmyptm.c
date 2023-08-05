
#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <limits.h>
#include "myptm/myptm.h"
#include "myptm/myptm_message_queue.h"

#include <Windows.h>

int64_t _myptm_thread_sys_tick_get_ms()
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
int32_t myptm_thread_sys_tick_get_ms()
{
    int32_t xx = (int32_t)(_myptm_thread_sys_tick_get_ms() - g_ms_base) + g_ms_offset;
    return xx;
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
    myptm_OBJECT(thread);
    int n;
    const char *name;
};

int my_thread_entry_a( myptm_thread_t *p )
{
    printf("entering my_thread_entry_a p=%p\n", p);
    myptm_DEF_THIS(p, struct my_thread_a, thread );

    myptm_BEGIN(p_this->thread);
    printf("thread %s started\n", p_this->name);
    while (1)
    {
        msg_t msg;
        int tx_size;
        msg.from_thread = p_this->name;
        msg.value = p_this->n;
        message_queue_send( message_queue_ptr(g_the_msg), &msg, sizeof(msg), tx_size);
        printf("-- thread %s txsize=%d\n", p_this->name, tx_size );
        myptm_thread_delay((1+rand()%3) * 1000 );
    }
    myptm_END();
}

void my_thread_a_init(struct my_thread_a *p_this, const char *name)
{
    myptm_thread_init(&p_this->thread, my_thread_entry_a);
    p_this->name = name;
    p_this->n = 0;
}

struct my_thread_b
{
    myptm_OBJECT(thread);
    int n;
    const char *name;
    struct my_thread_a a1, a2;
    char name_a1[32];
    char name_a2[32];
    int i;
    message_queue_def( msgq, 1024 );
};



int my_thread_entry_b( myptm_thread_t *p)
{
    myptm_DEF_THIS(p, struct my_thread_b, thread );

    myptm_BEGIN(p_this->thread);
    printf("thread %s started\n", p_this->name);
    my_thread_a_init(&p_this->a1, "b.a1");
    my_thread_a_init(&p_this->a2, "b.a2");
    myptm_loop_add( p_this->thread.p_owner, &p_this->a1.thread);
    myptm_loop_add( p_this->thread.p_owner, &p_this->a2.thread);
    myptm_thread_startup( &p_this->a1.thread);
    myptm_thread_startup( &p_this->a2.thread);
    while (1)
    {
        for (p_this->i = 0; p_this->i < 10; p_this->i++)
        {
            printf("thread %s delay 500ms for-loop %d\n", p_this->name, p_this->i);
            myptm_thread_delay(50);
        }
        myptm_thread_delay(5000);
        printf("######## thread %s exit n=%d\n\n", p_this->name, p_this->n++);
        while (1){
            int rx_size;
            msg_t msg;
            message_queue_recv( message_queue_ptr(g_the_msg), &msg, sizeof(msg), rx_size, -1 );
            printf("#########  rx_size=%d\n", rx_size );
            if (rx_size == sizeof(msg)){
                printf(" ########    message queue  msg recv from thread %s value=%d\n", msg.from_thread, msg.value );
            }
            p_this->n++;
            if (p_this->n == 10){
                myptm_thread_stop( &p_this->a1.thread );
            } else if (p_this->n == 20){
                myptm_thread_stop( &p_this->a2.thread );
            }
        }
    }
    myptm_END();
}

void my_thread_b_init(struct my_thread_b *p_this, const char *name)
{
    myptm_thread_init(&p_this->thread, my_thread_entry_b);
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
    myptm_loop_t loop;

    struct my_thread_a a1, a2;
    struct my_thread_b b1;

    message_queue_init_internal_pool( message_queue_ptr(g_the_msg), sizeof(msg_t), g_the_msg );
    //message_queue_init_internal_pool( &(g_the_msg), sizeof(msg_t), g_the_msg );

    g_ms_base = _myptm_thread_sys_tick_get_ms();

    my_thread_a_init(&a1, "a1");
    my_thread_a_init(&a2, "a2");
    my_thread_b_init(&b1, "b1");

    myptm_loop_init( &loop );
    myptm_loop_add( &loop, &a1.thread );
    myptm_loop_add( &loop, &a2.thread );
    myptm_loop_add( &loop, &b1.thread );

    myptm_thread_startup( &a1.thread );
    myptm_thread_startup( &a2.thread );
    myptm_thread_startup( &b1.thread );

    while (1)
    {
        int delay;
        myptm_loop_run( &loop.thread );
        delay = myptm_loop_next_tick( &loop );
        if (delay){
            printf(" ------------ sleep %dms ------------------\n", delay);
            Sleep(delay);
        }
    }
}
