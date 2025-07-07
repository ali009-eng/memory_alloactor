cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

add_executable(memory
        src/memory_allocator.cpp
        src/main.cpp
)