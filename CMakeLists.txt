cmake_minimum_required(VERSION 3.12)
project(gbs C)

set(CMAKE_C_STANDARD 11)

include_directories(Week5)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
#so that it will get built
add_library(list Week5/list.c)

#so that it will get linked
add_executable(syncem
        Week5/syncem.c)
target_link_libraries(syncem list Threads::Threads)

