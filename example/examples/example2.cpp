

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>

#define DEF_MYPT_C_FUNCTION_BODY
#include "myptm/myptm.h"

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
    printf("# thread %s started\n", p_this->name);
    while (1)
    {
        msg_t msg;
        int tx_size;
        msg.from_thread = p_this->name;
        msg.value = p_this->n;
        message_queue_send( message_queue_ptr(g_the_msg), &msg, sizeof(msg), tx_size);
        printf("* thread %s txsize=%d\n", p_this->name, tx_size );
        myptm_thread_delay( 5000 );
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
    int i;
};
int my_thread_entry_b( myptm_thread_t *p)
{
    int rx_size = -2;
    msg_t msg;

    myptm_DEF_THIS(p, struct my_thread_b, thread );

    myptm_BEGIN(p_this->thread);
    printf("# thread %s started\n", p_this->name);
    while (1)
    {
        message_queue_recv( message_queue_ptr(g_the_msg), &msg, sizeof(msg), rx_size, 1000 );
        printf("### rx_size=%d\n", rx_size );
        {
            myptm_err_t err = myptm_error_code();
            switch (err){
                case myptm_EOK:
                printf("error-code:EOK\n");
                if (rx_size != sizeof(msg)){
                    printf("* BUG! invalid message queue size\n");
                }
                printf("* message queue  msg recv from thread %s value=%d\n", msg.from_thread, msg.value );
                break;
                case myptm_EEMPTY:
                printf("error-code:EEMPTY\n");
                break;
                case myptm_ETERMINATED:
                printf("error-code:ETERMINATED\n");
                break;
                case myptm_ETIMEOUT:
                printf("error-code:ETIMEOUT\n");
                break;
            }
        }
        p_this->n++;
    }
    myptm_END();
}

void my_thread_b_init(struct my_thread_b *p_this, const char *name)
{
    myptm_thread_init(&p_this->thread, my_thread_entry_b);
    p_this->n = 0;
    p_this->name = name;
}

static void __stop_everything( myptm_timer_t *p_timer, void *arg)
{
    myptm_loop_t *p_loop = (myptm_loop_t *)arg;
    printf("# stop all\n");
    myptm_loop_destroy( p_loop );
}

static void __select4delay( myptm_loop_t *p_this, int delay, void *arg)
{
    //printf("     ------------ delay %d -------------\n", delay );
    if (delay > 0)
        std::this_thread::sleep_for( std::chrono::milliseconds( delay ) );
}

static void ticker_init();
int main(int argc, char **argv)
{
    myptm_loop_t loop;
    myptm_timer_t timer;
    struct my_thread_a a1, a2;
    struct my_thread_b b1;

    ticker_init();

    message_queue_init_internal_pool( message_queue_ptr(g_the_msg), sizeof(msg_t), g_the_msg );
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

    myptm_timer_init( &timer, __stop_everything, &loop );
    myptm_loop_timer_start( &loop, &timer, 15000 );

    while ( !myptm_loop_empty(&loop)){
        int delay = myptm_loop_poll(&loop);
        if (delay > 0){
            std::this_thread::sleep_for( std::chrono::milliseconds( delay ) );
        }
    }

    myptm_timer_destroy( &timer );
    
    return 0;
}

std::chrono::time_point<std::chrono::steady_clock> g_tp_last;

static void ticker_init()
{
    auto now = std::chrono::steady_clock::now();
    g_tp_last = now;
}

int32_t g_ms_offset = 0;
int32_t myptm_thread_sys_tick_get_ms()
{
    auto now = std::chrono::steady_clock::now();
    auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>( now - g_tp_last ).count();
    return diff_ms;
}
