
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern "C"{
#include "ringbuffer.h"
}

/*
cmd-line format:
token1 token2 ... tokenN\n

遇到\n即执行命令

*/
#define is_token_seperator(ch) (ch == ' ' || ch == '\t' || ch=='\r')
#define array_size(arr) (sizeof(arr)/sizeof(arr[0]))

struct token
{
    uint8_t len;
    char tk[12];
};
typedef struct token token_t;
void token_init( token_t *p_this)
{
    memset(p_this, 0x00, sizeof(*p_this));
}
void token_input( token_t *p_this, char ch)
{
    if (p_this->len < sizeof(p_this->tk)-1 ){
        p_this->tk[p_this->len++] = ch;
        p_this->tk[p_this->len] = 0;
    }
}

struct cmd_line
{
    int8_t n;
    token_t tokens[ 6 ];
};
typedef struct cmd_line cmd_line_t;

void cmd_line_init(cmd_line_t *p_this)
{
    memset(p_this, 0x00, sizeof(*p_this));
    p_this->n = 0;
}

void cmd_line_on_cmd( cmd_line_t *p_this)
{
    int i;
    int n = p_this->n;
    int32_t args[8] = {0};
    const char *cmd_name = NULL;

    if (p_this->n < 1){
        return ;
    }

    cmd_name = p_this->tokens[0].tk;

    for (i=1; i<n; i++){
        const char *p = p_this->tokens[i].tk;
        if ( p[0] == '0' && p[1] == 'x' ){
            sscanf( p+2, "%x", &args[i-1]);
            printf("sscanf for [16] %s data=0x%x\n", p, args[i-1] );
        } else {
            sscanf( p, "%d", &args[i-1]);
            printf("sscanf for [10] %s  data=%d\n", p, args[i-1] );
        }
    }

    if ( strcmp(cmd_name, "write") == 0){
        printf("calling write cmd :");
        // write( args1, arg2 );
    } else if ( 0 == strcmp( cmd_name, "read") ){
        printf("calling read cmd :");
        // read( args1, arg2 );
    } else{
        printf("calling cmd '%s' is not supported, args:", cmd_name );
    }
    for (i=0; i<n-1; i++){
        printf( " [0x%x] ", args[i]);
    }

    printf("\n");

#if 0
    printf("cmd_line_on_cmd:");
    for (i=0; i<p_this->n; i++){
        printf(" [%s] ", p_this->tokens[i].tk);
    }
    printf("\n");
#endif
}
void cmd_line_input(cmd_line_t *p_this, char ch)
{
    token_t *p_current;
    int8_t index;
    if (p_this->n == 0){
        if ( p_this->n < array_size(p_this->tokens) ){
            token_init( &p_this->tokens[p_this->n]);
            p_this->n++;
        }
    }
    if (is_token_seperator(ch) ){
        if (p_this->tokens[p_this->n-1].len == 0){
            return ;
        }
        /* allocate next token */
        if ( p_this->n < array_size(p_this->tokens) ){
            token_init( &p_this->tokens[p_this->n]);
            p_this->n++;
        }
        return;
    }
    if (ch == '\n'){
        cmd_line_on_cmd(p_this);
        cmd_line_init( p_this );
        return ;
    }
    index = p_this->n-1;
    p_current = &p_this->tokens[index];
    token_input( p_current, ch );
}

ring_buffer_t g_ringbuffer;

#define hw_disable_interrupt() g_lock.lock()
#define hw_enable_interrupt()  g_lock.unlock()


#include <mutex>
#include <thread>
#include <chrono>

std::mutex g_lock;

/*
使用另外一个线程模拟中段
*/
void uart_interrupt_handler( void )
{
    const char *g_cmds[] = {
        "write 0x1235 2222 3333 444 555\n",
        "read  0x1235 2222 3333 444 555\n",
        "read  0x1235 22235 3333 444 595\n",
        "peek     0x12350 22220 33330 4440 5550\n",
        "\n",
        "    \t\t\r\n",
    };

    printf("uart interrupt handler simulator thread started.\n");

    while (1){
        printf("interrupt loop\n");
        for (int i=0; i<array_size(g_cmds); i++){
            const char *strcmd = g_cmds[i];
            int len = strlen(strcmd);
            int j;
            for (j=0; j<len; j++){
                hw_disable_interrupt();
                ring_buffer_queue( &g_ringbuffer, *strcmd );
                hw_enable_interrupt();
                strcmd++;
                std::this_thread::sleep_for( std::chrono::milliseconds(50) );
            }
        }
    }
}

#define USING_DEMO
#ifdef USING_DEMO
int main(int argc, char **argv)
{
    cmd_line_t cmdline;

    ring_buffer_init( &g_ringbuffer );

    cmd_line_init(&cmdline);

    std::thread thread(uart_interrupt_handler);

    while (1){
        char chr;
        hw_disable_interrupt();
        if (ring_buffer_dequeue(&g_ringbuffer, &chr)){
            hw_enable_interrupt();
            cmd_line_input( &cmdline, chr );
        } else {
            hw_enable_interrupt();
        }
        std::this_thread::sleep_for( std::chrono::milliseconds(1));
    }

    thread.join();

    printf("Done\n");
    return 0;
}
#endif


/**
 *  
  ./simple_shell.exe
cmd:
sscanf for [16] 0x1235 data=0x1235
sscanf for [10] 2222  data=2222
sscanf for [10] 3333  data=3333
sscanf for [10] 444555  data=444555
cmd: write  [0x1235]  [0x8ae]  [0xd05]  [0x6c88b]
sscanf for [16] 0x1235 data=0x1235
sscanf for [10] 2222  data=2222
sscanf for [10] 3333  data=3333
sscanf for [10] 444555  data=444555
cmd: read  [0x1235]  [0x8ae]  [0xd05]  [0x6c88b]
sscanf for [16] 0x12350 data=0x12350
sscanf for [10] 22220  data=22220
sscanf for [10] 33330  data=33330
sscanf for [10] 44405550  data=44405550
cmd: peek  [0x12350]  [0x56cc]  [0x8232]  [0x2a5932e]
Done

 * 
 */

