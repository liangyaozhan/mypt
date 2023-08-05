
#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

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
}

int main(int argc, char **argv)
{
    test_timer();
}

/**
 * @brief 运行结果
 * 
t0 tick=1000
diff=OK t1 tick=6000
t0 tick=2147480647
tick=5000
diff=OK t1 tick=-2147481649
t0 tick=-3001
diff=OK t1 tick=1999
finish

 */