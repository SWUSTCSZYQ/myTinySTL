cmake_minimum_required(VERSION 3.23)
project(myTinySTL)

set(CMAKE_CXX_STANDARD 20)

include_directories(allocator, iterator, tools, test)

add_executable(myTinySTL test/vector_test.cpp allocator/malloc_alloc.cpp allocator/malloc_alloc.h
        allocator/default_alloc.cpp allocator/default_alloc.h tools/memory_uninitialized.cpp
        tools/memory_uninitialized.h iterator/iterator.cpp iterator/iterator.h iterator/type_traits.h
        allocator/construct.h container/vector.cpp container/vector.h allocator/allocator.cpp allocator/allocator.h )
