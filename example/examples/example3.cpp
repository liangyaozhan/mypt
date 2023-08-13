

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>

#define DEF_MYPT_C_FUNCTION_BODY
#include "myptm/myptm.h"

myptm_sem_t g_sem;

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
        myptm_sem_take( &g_sem, 1000 );
        myptm_err_t err = myptm_error_code();
        if (err == myptm_EOK)
            printf("* thread %s take semaphore and run\n", p_this->name );
        else if (err == myptm_ETIMEOUT){
            printf("* thread %s take semaphore TIMEOUT\n", p_this->name );
        }
    }
    myptm_END();
}

void my_thread_a_init(struct my_thread_a *p_this, const char *name)
{
    myptm_thread_init(&p_this->thread, my_thread_entry_a);
    p_this->name = name;
    p_this->n = 0;
}


static void __stop_everything( myptm_timer_t *p_timer, void *arg)
{
    myptm_loop_t *p_loop = (myptm_loop_t *)arg;
    printf("# stop all\n");
    myptm_loop_destroy( p_loop );
}

static void __timer_callback_500ms( myptm_timer_t *p_timer, void *arg)
{
    myptm_loop_t *p_loop = (myptm_loop_t *)arg;

    myptm_sem_give( &g_sem, 1);
    myptm_loop_timer_start( p_loop, p_timer, 500 );
}

static void ticker_init();
int main(int argc, char **argv)
{
    myptm_loop_t loop;
    myptm_timer_t timer;
    myptm_timer_t timer_500ms;
    struct my_thread_a a1, a2;

    myptm_sem_init( &g_sem, 1);

    ticker_init();

    my_thread_a_init(&a1, "a1");
    my_thread_a_init(&a2, "a2");

    myptm_loop_init( &loop );
    myptm_loop_add( &loop, &a1.thread );
    myptm_loop_add( &loop, &a2.thread );

    myptm_thread_startup( &a1.thread );
    myptm_thread_startup( &a2.thread );

    myptm_timer_init( &timer, __stop_everything, &loop );
    myptm_timer_init( &timer_500ms, __timer_callback_500ms, &loop);
    myptm_loop_timer_start( &loop, &timer_500ms, 500 );
    myptm_loop_timer_start( &loop, &timer, 15000 );

    while ( !myptm_loop_empty(&loop)){
        int delay = myptm_loop_poll(&loop);
        if (delay > 0){
            std::this_thread::sleep_for( std::chrono::milliseconds( delay ) );
        }
    }
    
    myptm_sem_destroy( &g_sem );
    myptm_loop_destroy( &loop );
    myptm_thread_stop( &a1.thread );
    myptm_thread_stop( &a2.thread );
    myptm_thread_join( &a1.thread );
    myptm_thread_join( &a2.thread );
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
