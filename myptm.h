

#ifndef MYPTM_HPP_H
#define MYPTM_HPP_H

#include <stdint.h>

#define MYPTM_CPLUSPLUS_BEGIN \
    extern "C"                \
    {
#define MYPTM_CPLUSPLUS_END }

#ifdef __cplusplus
MYPTM_CPLUSPLUS_BEGIN
#endif

#define MYPT_WAITING 0
#define MYPT_YIELDED 1
#define MYPT_EXITED 2
#define MYPT_PAUSE 3
#define MYPT_ENDED 4

struct _myptm_loop;
typedef struct _mypt_thread
{
    int lc;
    int32_t tick;
    int (*entry)(struct _mypt_thread *);
    struct _mypt_thread *next;
    struct _myptm_loop *p_owner;
    uint8_t mark_to_delete;
} mypt_thread;

struct _mypt_sem
{
    int count;
};
typedef struct _mypt_sem mypt_sem;

/*
 绕过这个BUG:
    msvc中, __LINE__非常量
*/
#define _DBJ_CONCATENATE_(a, b) a##b
#define _DBJ_CONCATENATE(a, b) _DBJ_CONCATENATE_(a, b)
#define CONSTEXPR_LINE _DBJ_CONCATENATE(__LINE__, U)

#define LC_INIT(s) (s) = 0;
#define LC_RESUME(s) \
    switch (s)       \
    {                \
    case 0:
#define LC_SET(s)       \
    s = CONSTEXPR_LINE; \
    case CONSTEXPR_LINE:
#define LC_END(s) \
    LC_SET(s);    \
    }

#define MYPT_INIT(ptr) LC_INIT((ptr)->lc)

#define MYPT_DEF_THIS(ptr, type, obj)  type *p_this = myptm_container_of((ptr), type, obj)
#define MYPT_BEGIN(obj)                       \
    {                                         \
        mypt_thread *__p_super_this = &(obj); \
        char MYPT_YIELD_FLAG = 1;             \
        LC_RESUME((__p_super_this)->lc)

#define MYPT_END()                         \
    __crt_label_end:                       \
    case -1:                               \
    LC_END((__p_super_this)->lc);          \
    MYPT_YIELD_FLAG = 0;                   \
    (__p_super_this)->lc = CONSTEXPR_LINE; \
    if ((int)CONSTEXPR_LINE < 0)           \
        goto __crt_label_end;              \
    return MYPT_ENDED;                     \
    }

#define MYPT_WAIT_UNTIL(condition)    \
    do                                \
    {                                 \
        LC_SET((__p_super_this)->lc); \
        if (!(condition))             \
        {                             \
            return MYPT_WAITING;      \
        }                             \
    } while (0)

#define MYPT_WAIT_WHILE(cond) MYPT_WAIT_UNTIL(!(cond))

#define MYPT_RESTART()             \
    do                             \
    {                              \
        MYPT_INIT(__p_super_this); \
        return MYPT_WAITING;       \
    } while (0)

#define MYPT_EXIT() goto __crt_label_end;

#define MYPT_YIELD()                  \
    do                                \
    {                                 \
        MYPT_YIELD_FLAG = 0;          \
        LC_SET((__p_super_this)->lc); \
        if (MYPT_YIELD_FLAG == 0)     \
        {                             \
            return MYPT_YIELDED;      \
        }                             \
    } while (0)

#define MYPT_SEM(sem) mypt_sem sem

#define mypt_sem_take(p_sem, timeout)                                                                      \
    do                                                                                                     \
    {                                                                                                      \
        mypt_sem *__ptr = (p_sem);                                                                         \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_sem)) == sizeof(mypt_sem)) ? 1 : -1]; \
        LC_SET(__p_super_this->lc);                                                                        \
        if (__ptr->count <= 0)                                                                             \
        {                                                                                                  \
            return MYPT_PAUSE;                                                                             \
        }                                                                                                  \
    } while (0)

#define mypt_sem_give(p_sem)                                                                               \
    do                                                                                                     \
    {                                                                                                      \
        mypt_sem *__ptr = (p_sem);                                                                         \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_sem)) == sizeof(mypt_sem)) ? 1 : -1]; \
        __ptr->count++;                                                                                    \
    } while (0)

#define MYPT_DO                     \
    do                              \
    {                               \
        LC_SET(__p_super_this->lc); \
    } while (0);

#define MYPT_CONTINUE return MYPT_WAITING
#define MYPT_WHILE(cond)         \
    do                           \
    {                            \
        if (cond)                \
            return MYPT_WAITING; \
    } while (0)

#define MYPT_DELAY_MS(ms)                                                          \
    do                                                                             \
    {                                                                              \
        __p_super_this->tick = mypt_thread_sys_tick_get_ms() + (ms);               \
        MYPT_WAIT_UNTIL(mypt_thread_sys_tick_get_ms() - __p_super_this->tick > 0); \
    } while (0)

#define MYPT_OBJECT(obj) mypt_thread obj

#define mypt_thread_init(p_thread, func)                                                                         \
    do                                                                                                           \
    {                                                                                                            \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_thread)) == sizeof(mypt_thread)) ? 1 : -1]; \
        mypt_thread *ptr = (p_thread);                                                                           \
        MYPT_INIT(ptr);                                                                                          \
        ptr->tick = 0;                                                                                           \
        ptr->entry = (func);                                                                                     \
        ptr->next = 0;                                                                                           \
        ptr->mark_to_delete = 0;                                                                                 \
        ptr->p_owner = 0;                                                                                        \
    } while (0)

#define mypt_thread_startup(p_main_loop, p_thread) myptm_loop_add((p_main_loop), (p_thread))
#define mypt_thread_stop(p_thread)                                                                               \
    do                                                                                                           \
    {                                                                                                            \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_thread)) == sizeof(mypt_thread)) ? 1 : -1]; \
        mypt_thread *ptr = (p_thread);                                                                           \
        ptr->mark_to_delete = 1;                                                                                 \
        if (ptr == __p_super_this)                                                                               \
        {                                                                                                        \
            MYPT_EXIT();                                                                                         \
        }                                                                                                        \
        else                                                                                                     \
        {                                                                                                        \
            MYPT_WAIT_WHILE(ptr->mark_to_delete == 1);                                                           \
        }                                                                                                        \
    } while (0)

/* you should define this function before using MYPT_DELAY */
int32_t mypt_thread_sys_tick_get_ms();

struct _myptm_loop
{
    mypt_thread thread;
    mypt_thread *running;
    mypt_thread *pending;
};

typedef struct _myptm_loop myptm_loop;

void myptm_loop_init(myptm_loop *p_this);
int myptm_loop_run(myptm_loop *p_this);
void myptm_loop_add(myptm_loop *p_this, mypt_thread *p_thread);

struct _messagequeue
{
    uint16_t msg_size; /**< message size of each message */
    uint16_t max_msgs; /**< max number of messages */

    uint16_t entry; /**< index of messages in the queue */

    void *msg_queue_head; /**< list head */
    void *msg_queue_tail; /**< list tail */
    void *msg_queue_free; /**< pointer indicated the free node of queue */

    void *msg_pool; /**< start address of message queue */
};

typedef struct _messagequeue message_queue;

void _message_queue_init(message_queue *p_this, void *msgpool, int msg_size, int pool_size);
int _message_queue_recv(message_queue *p_this, void *buffer, int size);
int _message_queue_send(message_queue *p_this, const void *buffer, int size);

#ifndef myptm_container_of
#define myptm_container_of(p, t, m) \
    ((t *)((char *)p - (char *)(&(((t *)0)->m))))
#endif

#define message_queue_def(obj, pool_size)            \
    struct __obj_s##obj                              \
    {                                                \
        message_queue q;                             \
        uint32_t pool[pool_size / sizeof(uint32_t)]; \
    } obj
#define message_queue_ptr(obj) (&(obj).q)

#define message_queue_init_internal_pool(p, msg_size, obj)                                                \
    do                                                                                                    \
    {                                                                                                     \
        message_queue *__ptr_q = p;                                                                       \
        extern int incompatible_pointor_type_check_array[(sizeof(*p) == sizeof(message_queue)) ? 1 : -1]; \
        struct __obj_s##obj *__ptr = myptm_container_of(__ptr_q, struct __obj_s##obj, q);                 \
        _message_queue_init(&__ptr->q, __ptr->pool, msg_size, sizeof(__ptr->pool));                       \
    } while (0)

#define message_queue_init_this(obj, msg_size) message_queue_init_internal_pool(message_queue_ptr(p_this->obj), msg_size, obj)

#define message_queue_send(p_mq, buffer, size, error_code)                                                     \
    do                                                                                                         \
    {                                                                                                          \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_mq)) == sizeof(message_queue)) ? 1 : -1]; \
        MYPT_WAIT_UNTIL((error_code = _message_queue_send((p_mq), (buffer), (size))) >= 0);                    \
    } while (0)
#define message_queue_send_timeout(p_mq, buffer, size, timeout, error_code)                                    \
    do                                                                                                         \
    {                                                                                                          \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_mq)) == sizeof(message_queue)) ? 1 : -1]; \
        __p_super_this->tick = mypt_thread_sys_tick_get_ms() + (timeout);                                      \
        MYPT_WAIT_WHILE((error_code = _message_queue_send((p_mq), (buffer), (size))) < 0 &&                    \
                        (mypt_thread_sys_tick_get_ms() - __p_super_this->tick > 0));                           \
    } while (0)

#define message_queue_recv(p_mq, buffer, size, error_code)                                                     \
    do                                                                                                         \
    {                                                                                                          \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_mq)) == sizeof(message_queue)) ? 1 : -1]; \
        MYPT_WAIT_UNTIL((error_code = _message_queue_recv((p_mq), (buffer), (size))) >= 0);                    \
    } while (0)
#define message_queue_recv_timeout(p_mq, buffer, size, timeout, error_code)                                    \
    do                                                                                                         \
    {                                                                                                          \
        extern int incompatible_pointor_type_check_array[(sizeof(*(p_mq)) == sizeof(message_queue)) ? 1 : -1]; \
        __p_super_this->tick = mypt_thread_sys_tick_get_ms() + (timeout);                                      \
        MYPT_WAIT_WHILE((error_code = _message_queue_recv((p_mq), (buffer), (size))) < 0 &&                    \
                        (mypt_thread_sys_tick_get_ms() - __p_super_this->tick > 0));                           \
    } while (0)

#ifdef __cplusplus
MYPTM_CPLUSPLUS_END
#endif

#endif
