cmake_minimum_required(VERSION 3.12)
project(gbs C)

set(CMAKE_C_STANDARD 11)

include_directories(Week5)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
#so that it will get built
add_library(list Week7/list.c Week7/parser.c)
add_library(list8 Week8/list.c)

#so that it will get linked
add_executable(mysh
        Week7/mysh.c)
add_executable(server Week8/server.c)
target_link_libraries(server list8)
target_link_libraries(mysh list Threads::Threads)

