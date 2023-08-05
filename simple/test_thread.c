
#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <limits.h>
#include "mypt.h"

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
int32_t g_ms_offset = INT_MAX - 3000;
int32_t mypt_thread_sys_tick_get_ms()
{
    return (int32_t)(_mypt_thread_sys_tick_get_ms() - g_ms_base) + g_ms_offset;
}


/* 使用定时器g_timer_0计时,并用值0表示定时器未开启 */
static uint32_t g_timer_0[4] = {0, 0, 0, 0};

uint8_t timer_is_running(uint8_t idx)
{
    return g_timer_0[idx] != 0;
}

//设置定时时间timeout
void timer_start(uint8_t idx, int32_t timeout)
{
    g_timer_0[idx] = mypt_thread_sys_tick_get_ms() + timeout;
    if (g_timer_0[idx] == 0 ){
        g_timer_0[idx] = 1;
    }
}

//停止计时，计时数组清0
void timer_stop(uint8_t idx)
{
    g_timer_0[idx] = 0;
}

//判断是否超时，单位1ms,返回1定时时间到超时，返回0时间没到。
uint8_t timer_is_timeout(uint8_t idx)
{
    int32_t diff = (int32_t)mypt_thread_sys_tick_get_ms() - (int32_t)g_timer_0[idx];
    return diff >= 0;
}

struct my_thread_a
{
    MYPT_OBJECT(namea);
    int n;
    const char *name;
};

void my_thread_a_init(struct my_thread_a *p_this, const char *name)
{
    MYPT_INIT(&p_this->namea);
    p_this->name = name;
    p_this->n = 0;
}

int my_thread_entry_a(struct my_thread_a *p_this)
{
    MYPT_BEGIN(p_this->namea);
    printf("thread %s started\n", p_this->name);
    while (1)
    {
        printf(" *** thread %s delay1 1000ms n=%d tick=%d\n", p_this->name, p_this->n++, mypt_thread_sys_tick_get_ms() );
        MYPT_DELAY_MS(1000);
        //printf("thread %s delay2 1000ms\n", p_this->name);
        //MYPT_DELAY_MS(1000);
        //printf("    -> thread %s delay2 1000ms done\n\n", p_this->name);
    }
    MYPT_END();
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
};

void my_thread_b_init(struct my_thread_b *p_this, const char *name)
{
    MYPT_INIT(&p_this->nameb);
    p_this->n = 0;
    p_this->name = name;
    sprintf(p_this->name_a1, "%s's a1", name);
    sprintf(p_this->name_a2, "%s's a2", name);
    my_thread_a_init(&p_this->a1, p_this->name_a1);
    my_thread_a_init(&p_this->a2, p_this->name_a2);
}

int my_thread_entry_b(struct my_thread_b *p_this)
{
    MYPT_BEGIN(p_this->nameb);
    printf("thread %s started\n", p_this->name);
    while (1)
    {
        my_thread_entry_a(&p_this->a1);
        my_thread_entry_a(&p_this->a2);
        for (p_this->i = 0; p_this->i < 10; p_this->i++)
        {
            printf("thread %s delay 500ms for-loop %d\n", p_this->name, p_this->i);
            MYPT_DELAY_MS(50);
        }
        printf("thread %s delay 5000ms done %d\n\n", p_this->name, p_this->n++);
        MYPT_DELAY_MS(5000);
    }
    MYPT_END();
}

void test_timer()
{
    int64_t t0, t1;

    g_ms_base = _mypt_thread_sys_tick_get_ms();
    g_ms_offset = 1000;
    printf("t0 tick=%d\n", mypt_thread_sys_tick_get_ms());
    t0 = _mypt_thread_sys_tick_get_ms();
    timer_start( 0, 5000 );
    while (!timer_is_timeout(0)){
    }
    t1 = _mypt_thread_sys_tick_get_ms();
    printf("diff=%s t1 tick=%d\n", ((t1 - t0) == 5000?"OK":"Failed"), mypt_thread_sys_tick_get_ms() );

    g_ms_base = _mypt_thread_sys_tick_get_ms();
    g_ms_offset = INT_MAX - 3000;
    printf("t0 tick=%d\n", mypt_thread_sys_tick_get_ms());
    t0 = _mypt_thread_sys_tick_get_ms();
    timer_start( 0, 5000 );
    while (!timer_is_timeout(0)){
    }
    t1 = _mypt_thread_sys_tick_get_ms();
    printf("tick=%ld\n", t1 - t0);
    printf("diff=%s t1 tick=%d\n", ((t1 - t0) == 5000?"OK":"Failed"), mypt_thread_sys_tick_get_ms() );
 
    g_ms_base = _mypt_thread_sys_tick_get_ms();
    g_ms_offset = UINT_MAX - 3000;
    printf("t0 tick=%d\n", mypt_thread_sys_tick_get_ms());
    t0 = _mypt_thread_sys_tick_get_ms();
    timer_start( 0, 5000 );
    while (!timer_is_timeout(0)){
    }
    t1 = _mypt_thread_sys_tick_get_ms();
    printf("diff=%s t1 tick=%d\n", ((t1 - t0) == 5000?"OK":"Failed"), mypt_thread_sys_tick_get_ms() );
 
    printf("finish\n");
    while (1);
}

int main(int argc, char **argv)
{
    struct my_thread_a a1, a2;
    struct my_thread_b b1;

    test_timer();

    g_ms_base = _mypt_thread_sys_tick_get_ms();

    my_thread_a_init(&a1, "a1");
    my_thread_a_init(&a2, "a2");

    //my_thread_b_init(&b1, "b1");

    while (1)
    {
        my_thread_entry_a(&a1);
        my_thread_entry_a(&a2);
        //my_thread_entry_b(&b1);
    }
}
