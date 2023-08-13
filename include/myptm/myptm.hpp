
#ifndef MYPTM_HPP
#define MYPTM_HPP

#include "myptm/myptm.h"

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
