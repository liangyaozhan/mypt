#include <string>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#define DEF_MYPT_C_FUNCTION_BODY
#include "myptm/myptm.h"

myptm_loop_t main_loop;
int main_loop_poll()
{
    int delay = myptm_loop_poll(&main_loop);
    if (delay > 0){
        printf("     ------------ delay %d -------------\n", delay );
        std::this_thread::sleep_for( std::chrono::milliseconds( delay ) );
    }
    return !myptm_loop_empty(&main_loop);
}

////////////////////////   driver level ///////////////////////////////////////

#define driver_disable_ph_int() do{}while (0)
#define driver_enable_ph_int() do{}while (0)
#define driver_clear_ph_int() do{}while (0)

class driver 
{
public:
    myptm_sem_t sem_interrupted;
    myptm_sem_t sem_read;
    myptm_thread_t thread;
    myptm_loop_t *p_loop_env;
    int n;
    std::string name;
    std::vector<char> buffer;

    static int __recv_data_thread( myptm_thread_t *p )
    {
        myptm_DEF_THIS(p, driver, thread );

        myptm_BEGIN(p_this->thread);
        printf("# thread %s started\n", p_this->name.c_str());
        while (1)
        {
            driver_clear_ph_int();
            driver_enable_ph_int();
            myptm_sem_take( &p_this->sem_interrupted, -1 );
            myptm_err_t err = myptm_error_code();
            if (err == myptm_EOK){
                char uart_recv_char = 'A' + (rand() % ('z'-'A'));
                //printf("* driver got char %c size:%d\n", uart_recv_char, int(p_this->buffer.size()) );
                p_this->buffer.push_back(uart_recv_char);
            }
            else if (err == myptm_ETIMEOUT){
                printf("* thread %s take semaphore TIMEOUT\n", p_this->name.c_str() );
            }
        }
        myptm_END();
    }

    static void interrupt_service( driver *p_this ){
        driver_disable_ph_int();
        myptm_sem_give( &p_this->sem_interrupted, 1 );
    }

    void read( std::vector<char> &buf ){
        buf = std::move(this->buffer);
    }
    void read_block( std::vector<char> &buf ){
        while ( this->buffer.size() == 0 && main_loop_poll() ){
        }
        this->read(buf);
    }

    driver(const std::string &name, myptm_loop_t *pl)
    {
        myptm_sem_init(&this->sem_interrupted, 0);
        myptm_sem_init(&this->sem_read, 0);
        myptm_thread_init( &this->thread, __recv_data_thread);
        this->name = name;
        this->n = 0;
        this->p_loop_env = pl;
        myptm_loop_add( pl, &this->thread);
        myptm_thread_startup( &this->thread);
    }
    ~driver(){
        myptm_sem_destroy(&this->sem_interrupted);
        myptm_sem_destroy(&this->sem_read);
        myptm_thread_stop( &this->thread );
        myptm_thread_join( &this->thread );
    }
};

////////////////////////   driver level ///////////////////////////////////////
////////////////////////   ------------ ///////////////////////////////////////

////////////////////////   iosystem level ///////////////////////////////////////
class io_system
{
    driver *p_driver;
    public:
    io_system( driver *p_drv):p_driver(p_drv){
    }
    void read( std::vector<char> &buffer ){
        p_driver->read(buffer);
    }
    void read_block( std::vector<char> &buffer ){
        p_driver->read_block(buffer);
    }
};
////////////////////////   iosystem level ///////////////////////////////////////
////////////////////////   ------------ ///////////////////////////////////////


////////////////////////   application level ///////////////////////////////////////
class app{
    public:
    myptm_thread_t thread;
    io_system *dev;
    app( io_system *d):dev(d){
        myptm_thread_init( &this->thread, &_app_thread);
        myptm_loop_add( &main_loop, &this->thread );
        myptm_thread_startup( &this->thread );
    }
    ~app(){
        myptm_thread_stop(&this->thread);
        myptm_thread_join(&this->thread);
    }
    static int _app_thread(myptm_thread_t *p_thread){
        std::vector<char> buf;
        int tick;
        myptm_DEF_THIS(p_thread, app, thread );
        myptm_BEGIN( p_this->thread);
        while (1){
            tick = myptm_thread_sys_tick_get_ms();
            p_this->dev->read_block( buf );
            buf.push_back(0);
            printf("read_block:%s  waste %dms\n", buf.data(), (int)(myptm_thread_sys_tick_get_ms() - tick ));
            myptm_thread_delay( 1000  );
        }
        myptm_END();
        return 0;
    }
};
////////////////////////   application level ///////////////////////////////////////
static void __timer_callback_50ms( myptm_timer_t *p_this, void *arg)
{
    driver *p_drv = (driver *)arg;
    driver::interrupt_service( p_drv );
    myptm_loop_timer_start( &main_loop, p_this, 10 );
}
static void __timer_callback_15seconds( myptm_timer_t *p_this, void *arg)
{
    myptm_loop_t *p_loop = (myptm_loop_t*)arg;

    printf("Stop ALL!\n");
    myptm_loop_destroy(p_loop);
}

static void ticker_init();
int main(int argc, char **argv)
{
    myptm_timer_t timer_50ms;
    myptm_timer_t timer_15seconds;

    ticker_init();
    myptm_loop_init( &main_loop );
    driver thedriver("abc", &main_loop);
    io_system io(&thedriver);
    app g_app(&io);

    myptm_timer_init( &timer_50ms, __timer_callback_50ms, &thedriver );
    myptm_timer_init( &timer_15seconds, __timer_callback_15seconds, &main_loop );
    myptm_loop_timer_start( &main_loop, &timer_50ms, 10 );
    myptm_loop_timer_start( &main_loop, &timer_15seconds, 15000 );

    while (main_loop_poll()){}

    myptm_timer_destroy( &timer_50ms );
    myptm_timer_destroy( &timer_15seconds );
    printf("Done\n");
    return 0;
}

std::chrono::time_point<std::chrono::steady_clock> g_tp_last;

static void ticker_init()
{
    auto now = std::chrono::steady_clock::now();
    g_tp_last = now;
}

int32_t myptm_thread_sys_tick_get_ms()
{
    auto now = std::chrono::steady_clock::now();
    auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>( now - g_tp_last ).count();
    return diff_ms;
}
