cmake_minimum_required(VERSION 3.5)

project(os_lab C)

set(CMAKE_C_STANDARD 11)

set(SOURCES
    src/fork_demo.c
    src/exec_demo.c
    src/pipe_demo.c
    src/pipeline_demo.c
    src/zombie_demo.c
)

add_executable(fork_demo src/fork_demo.c)
add_executable(exec_demo src/exec_demo.c)
add_executable(pipe_demo src/pipe_demo.c)
add_executable(pipeline_demo src/pipeline_demo.c)
add_executable(zombie_demo src/zombie_demo.c)
