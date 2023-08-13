

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>

#define DEF_MYPT_C_FUNCTION_BODY
#include "myptm/myptm.h"

myptm_sem_t g_sem;

class my_thread_a:public myptm::thread
{
    public:
    std::string name;

    int delay;

    int32_t t0;

    int run() final
    {
        myptm_BEGIN( this->thread_object() );
        printf("# thread %s started\n", this->name.c_str() );
        while (1)
        {
            this->t0 = myptm_thread_sys_tick_get_ms();
            myptm_sem_take( &g_sem, this->delay );
            myptm_err_t err = myptm_error_code();
            int diff = myptm_thread_sys_tick_get_ms() - this->t0;
            if (err == myptm_EOK)
                printf("* thread %s take semaphore and run diff=%d\n", this->name.c_str(), diff );
            else if (err == myptm_ETIMEOUT){
                printf("* thread %s take semaphore TIMEOUT diff=%d\n", this->name.c_str(), diff );
            }
        }
        myptm_END();
    }
};

static void ticker_init();
int main(int argc, char **argv)
{
    ticker_init(); /* must init ticker first */

    myptm::timer timer;
    myptm::timer timer_500ms;
    my_thread_a a1, a2;
    myptm::loop loop;

    a1.name = "A1";
    a2.name = "A2";
    a1.delay = 100;
    a2.delay = 150;

    myptm_sem_init( &g_sem, 1);

    loop.add( a1 );
    loop.add( a2 );

    a1.startup();
    a2.startup();

    timer.set_callback( [&loop]( myptm::timer &tm){
        printf("StopALL\n");
        loop.stop();
    });
    int32_t t1 = myptm_thread_sys_tick_get_ms();
    timer_500ms.set_callback( [&loop, t1](myptm::timer &tm){
        printf("----------- 1000ms ->  %d------------ \n", int(myptm_thread_sys_tick_get_ms() - t1));
        myptm_sem_give( &g_sem, 1);
        loop.start(tm);
    });

    int32_t t0 = myptm_thread_sys_tick_get_ms();
    loop.timer_start([t0](){
        printf("-----------one time timer ------------ %d\n", int(myptm_thread_sys_tick_get_ms() - t0));
    }, 5000 );

    timer.set_tick( 15000 );
    timer_500ms.set_tick( 1000 );
    loop.start( timer );
    loop.start( timer_500ms );

    while ( !loop.empty()){
        int delay = loop.poll();
        delay = loop.poll();
        if (delay > 0){
            myptm_timer_t *p_timer;
            int sum = 0;
            myptm_queue_for_each_container(p_timer, &loop.loop_get().ticker.head, myptm_timer_t, node ){
                sum += p_timer->tick;
                printf(" [%d %d/%d] ", p_timer->tick, sum, p_timer->tick_origin );
            }
            printf(" ----------- sleep %d ---------------- \n", delay );
            std::this_thread::sleep_for( std::chrono::milliseconds( delay ) );
        }
    }
    myptm_sem_destroy( &g_sem );
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
    return (int32_t)diff_ms;
}
