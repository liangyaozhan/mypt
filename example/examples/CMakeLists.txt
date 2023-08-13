cmake_minimum_required(VERSION 3.1)

project(examples CXX C)

add_executable(example1 example1.cpp )

target_include_directories(example1 PUBLIC include )
target_compile_options(example1 PUBLIC -g -O0 )



add_executable(example2 example2.cpp )
target_include_directories(example2 PUBLIC include )
target_compile_options(example2 PUBLIC -g -O0 )


add_executable(example3 example3.cpp )
target_include_directories(example3 PUBLIC include )
target_compile_options(example3 PUBLIC -g -O0 )

add_executable(example_driver example_driver.cpp )
target_include_directories(example_driver PUBLIC include )
target_compile_options(example_driver PUBLIC -g -O0 )


add_executable(example4 example4.cpp )
target_include_directories(example4 PUBLIC include )
target_compile_options(example4 PUBLIC -g -O0 )


add_executable(simple_shell simple_shell.cpp ringbuffer.c)
target_include_directories(simple_shell PUBLIC include )
target_compile_options(simple_shell PUBLIC -g -O0 )
target_compile_definitions(simple_shell PUBLIC USING_DEMO=1 )

if(${CMAKE_COMPILER_IS_GNUCXX})
target_link_libraries(example1 PUBLIC stdc++)
target_link_libraries(example2 PUBLIC stdc++)
target_link_libraries(example3 PUBLIC stdc++)
target_link_libraries(example_driver PUBLIC stdc++)
target_link_libraries(example4 PUBLIC stdc++)
target_link_libraries(simple_shell PUBLIC stdc++)
endif()
