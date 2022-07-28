cmake_minimum_required(VERSION 3.23)
project(myTinySTL)

set(CMAKE_CXX_STANDARD 20)

include_directories(allocator)

add_executable(myTinySTL main.cpp allocator/malloc_alloc.cpp allocator/malloc_alloc.h allocator/default_alloc.cpp allocator/default_alloc.h)
