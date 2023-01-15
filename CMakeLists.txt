cmake_minimum_required(VERSION 3.16)
project(Luby_Jones)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(Luby_Jones main.cpp colours.h)
target_link_libraries(Luby_Jones Threads::Threads)
