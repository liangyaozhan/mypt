#ifndef MYPT_HPP_
#define MYPT_HPP_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FTK_END_DECLS }

#define MYPT_WAITING 0
#define MYPT_YIELDED 1
#define MYPT_EXITED  2
#define MYPT_ENDED   3

/*
 绕过这个BUG: 
    msvc中, __LINE__非常量
*/
#define _DBJ_CONCATENATE_(a, b) a ## b
#define _DBJ_CONCATENATE(a, b)  _DBJ_CONCATENATE_(a, b)
#define CONSTEXPR_LINE _DBJ_CONCATENATE(__LINE__,U)

#define LC_INIT(s) (s) = 0;
#define LC_RESUME(s) switch(s) { case 0:
#define LC_SET(s) s = CONSTEXPR_LINE; case CONSTEXPR_LINE:
#define LC_END(s) LC_SET(s);}

#define MYPT_INIT(ptr)   LC_INIT((ptr)->lc)

#define MYPT_BEGIN( obj ) { \
    mypt_thread *__p_super_this = &(obj);\
    char MYPT_YIELD_FLAG = 1;\
    LC_RESUME((__p_super_this)->lc)

#define MYPT_END() __crt_label_end:case -1:LC_END((__p_super_this)->lc); \
    MYPT_YIELD_FLAG = 0;(__p_super_this)->lc = CONSTEXPR_LINE;if ((int)CONSTEXPR_LINE < 0)goto __crt_label_end; \
    return MYPT_ENDED;\
    }

#define MYPT_WAIT_UNTIL(condition)	        \
        do {						\
            LC_SET((__p_super_this)->lc);				\
            if(!(condition)) {				\
                return MYPT_WAITING;			\
            }						\
        } while(0)

#define MYPT_WAIT_WHILE(cond)  MYPT_WAIT_UNTIL(!(cond))

#define MYPT_RESTART()				\
        do {						\
            MYPT_INIT(__p_super_this);				\
            return MYPT_WAITING;			\
        } while(0)

#define MYPT_EXIT() goto __crt_label_end;


#define MYPT_YIELD()				\
        do {						\
            MYPT_YIELD_FLAG = 0;				\
            LC_SET((__p_super_this)->lc);				\
            if(MYPT_YIELD_FLAG == 0) {			\
                return MYPT_YIELDED;			\
            }						\
        } while(0)

#define MYPT_YIELD_UNTIL(cond)		\
        do {						\
            MYPT_YIELD_FLAG = 0;				\
            LC_SET((__p_super_this)->lc);				\
            if((MYPT_YIELD_FLAG == 0) || !(cond)) {	\
                return MYPT_YIELDED;			\
            }						\
        } while(0)



#define MYPT_DELAY_MS(ms)	        do{\
        __p_super_this->tick = mypt_thread_sys_tick_get_ms()+(ms);\
        MYPT_WAIT_UNTIL(mypt_thread_sys_tick_get_ms() - __p_super_this->tick > 0);}while (0)

typedef struct _mypt_thread
{
    int lc;
    int32_t tick;
}mypt_thread;

#define MYPT_OBJECT(obj) mypt_thread obj


/* you should define this function before using MYPT_DELAY */
int32_t mypt_thread_sys_tick_get_ms();

#ifdef __cplusplus
}
#endif

#endif /* MYPT_H_ */

/** @} */
