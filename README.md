
# 多线程风格编写单线程代码.

# 例子即是最好的文档

已经试验环境: msvc, msys里的gcc

```C

#include <stdio.h>
#include "mypt.h"

#include <Windows.h>

int32_t mypt_thread_sys_tick_get_ms()
{
    LARGE_INTEGER	SystemFrequency, StartTime;
    double          ms;
    QueryPerformanceFrequency(&SystemFrequency);
    QueryPerformanceCounter(&StartTime);
    ms = 1000.0 * (double)(StartTime.QuadPart) / (double)SystemFrequency.QuadPart;
    return (int32_t)ms;
}

struct my_thread_a
{
    MYPT_OBJECT(namea);
    int n;
    const char *name;
};

void my_thread_a_init( struct my_thread_a *p_this, const char *name)
{
    MYPT_INIT( &p_this->namea );
    p_this->name = name;
    p_this->n = 0;
}

int my_thread_entry_a( struct my_thread_a *p_this )
{
    MYPT_BEGIN(p_this->namea);
    printf("thread %s started\n", p_this->name );
    while (1){
        printf("thread %s delay1 1000ms n=%d\n", p_this->name, p_this->n++);
        MYPT_DELAY_MS(1000);
        printf("    -> thread %s delay1 1000ms done\n\n", p_this->name );
        printf("thread %s delay2 1000ms\n", p_this->name);
        MYPT_DELAY_MS(1000);
        printf("    -> thread %s delay2 1000ms done\n\n", p_this->name);
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
};

void my_thread_b_init( struct my_thread_b *p_this, const char *name)
{
    MYPT_INIT(&p_this->nameb);
    p_this->n = 0;
    p_this->name = name;
    sprintf(p_this->name_a1, "%s's a1", name );
    sprintf(p_this->name_a2, "%s's a2", name );
    my_thread_a_init( &p_this->a1, p_this->name_a1 );
    my_thread_a_init( &p_this->a2, p_this->name_a2 );
}

int my_thread_entry_b( struct my_thread_b *p_this )
{
    MYPT_BEGIN(p_this->nameb);
    printf("thread %s started\n", p_this->name );
    while (1){
        my_thread_entry_a( &p_this->a1 );
        my_thread_entry_a( &p_this->a2 );
        printf("thread %s delay 5000ms done %d\n\n", p_this->name, p_this->n++ );
        MYPT_DELAY_MS(5000);
    }
    MYPT_END();
}

int main(int argc, char **argv)
{
    struct my_thread_a a1, a2;
    struct my_thread_b b1;

    my_thread_a_init( &a1, "thread a1" );
    my_thread_a_init( &a2, "thread a2" );

    my_thread_b_init( &b1, "thread b1" );

    while (1){
        my_thread_entry_a(&a1);
        my_thread_entry_a(&a2);
        my_thread_entry_b(&b1);
    }
}


```



输出:

```bash
/usr/bin/bash $ ./a
thread thread a1 started
thread thread a1 delay1 1000ms n=0
thread thread a2 started
thread thread a2 delay1 1000ms n=0
thread thread b1 started
thread thread b1's a1 started
thread thread b1's a1 delay1 1000ms n=0
thread thread b1's a2 started
thread thread b1's a2 delay1 1000ms n=0
thread thread b1 delay 5000ms done 0

    -> thread thread a1 delay1 1000ms done

thread thread a1 delay2 1000ms
    -> thread thread a2 delay1 1000ms done

thread thread a2 delay2 1000ms
    -> thread thread a1 delay2 1000ms done

thread thread a1 delay1 1000ms n=1
    -> thread thread a2 delay2 1000ms done

thread thread a2 delay1 1000ms n=1
    -> thread thread a1 delay1 1000ms done

thread thread a1 delay2 1000ms
    -> thread thread a2 delay1 1000ms done

thread thread a2 delay2 1000ms
    -> thread thread a1 delay2 1000ms done

thread thread a1 delay1 1000ms n=2
    -> thread thread a2 delay2 1000ms done

thread thread a2 delay1 1000ms n=2
    -> thread thread a1 delay1 1000ms done

thread thread a1 delay2 1000ms
    -> thread thread a2 delay1 1000ms done

thread thread a2 delay2 1000ms
    -> thread thread b1's a1 delay1 1000ms done

thread thread b1's a1 delay2 1000ms
    -> thread thread b1's a2 delay1 1000ms done

thread thread b1's a2 delay2 1000ms
thread thread b1 delay 5000ms done 1

    -> thread thread a1 delay2 1000ms done

thread thread a1 delay1 1000ms n=3
    -> thread thread a2 delay2 1000ms done

thread thread a2 delay1 1000ms n=3
    -> thread thread a2 delay1 1000ms done

thread thread a2 delay2 1000ms
    -> thread thread a1 delay1 1000ms done

thread thread a1 delay2 1000ms
    -> thread thread a1 delay2 1000ms done

thread thread a1 delay1 1000ms n=4
    -> thread thread a2 delay2 1000ms done

thread thread a2 delay1 1000ms n=4
    -> thread thread a1 delay1 1000ms done

thread thread a1 delay2 1000ms
    -> thread thread a2 delay1 1000ms done

thread thread a2 delay2 1000ms
    -> thread thread a2 delay2 1000ms done

thread thread a2 delay1 1000ms n=5
    -> thread thread b1's a1 delay2 1000ms done

thread thread b1's a1 delay1 1000ms n=1
    -> thread thread b1's a2 delay2 1000ms done

thread thread b1's a2 delay1 1000ms n=1
thread thread b1 delay 5000ms done 2

    -> thread thread a1 delay2 1000ms done

thread thread a1 delay1 1000ms n=5
    -> thread thread a2 delay1 1000ms done

thread thread a2 delay2 1000ms
    -> thread thread a1 delay1 1000ms done

thread thread a1 delay2 1000ms
    -> thread thread a1 delay2 1000ms done

thread thread a1 delay1 1000ms n=6
    -> thread thread a2 delay2 1000ms done

thread thread a2 delay1 1000ms n=6
    -> thread thread a1 delay1 1000ms done

thread thread a1 delay2 1000ms
    -> thread thread a2 delay1 1000ms done

thread thread a2 delay2 1000ms



```



