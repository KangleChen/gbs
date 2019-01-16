project(gbs C)

set(CMAKE_C_STANDARD 11)

include_directories(Week5)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#Week7
add_library(list Week7/list.c Week7/parser.c)
add_executable(mysh Week7/mysh.c)
target_link_libraries(mysh list Threads::Threads)

#Week8
add_library(list8 Week8/list.c Week8/hexdump.c)
add_executable(server Week8/server.c)
target_link_libraries(server list8)

#Week9
add_executable(mem Week9/mem.c)
add_library(lib_week9 Week9/list.c Week9/memalloc.c)
target_link_libraries(mem lib_week9 m)

#Week10
#Week11

add_executable(week11 Week11_J/mysh.c)
add_library(lib_week11 Week11_J/list.c Week11_J/parser.c)
target_link_libraries(week11 lib_week11)

