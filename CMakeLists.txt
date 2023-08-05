cmake_minimum_required(VERSION 3.1)

project(myptm C)

add_executable(myptm 
    src/myptm_core.c
    src/myptm_timer.c
    src/myptm_message_queue.c
    example/testmyptm.c
    )

target_include_directories(myptm PUBLIC include )
target_compile_options(myptm PUBLIC -g -O0 )

