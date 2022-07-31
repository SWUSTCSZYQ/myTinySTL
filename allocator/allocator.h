//
// Created by Lenovo on 2022/7/31.
//

#ifndef MYTINYSTL_ALLOCATOR_H
#define MYTINYSTL_ALLOCATOR_H

#include <cstddef>
#include "default_alloc.h"

template<typename T, typename Alloc = default_alloc>
class allocator {
public:
    static T* allocate(size_t n){
        return n == 0 ? nullptr: (T*)Alloc::allocate(n * sizeof (T));
    }

    static T* allocate(){
        return (T*)Alloc::allocate(sizeof (T));
    }

    static void deallocate(T* p, size_t n){
        if(n != 0)Alloc::deallocate(p, n * sizeof (T));
    }

    static void deallocate(T* p){
        Alloc::deallocate(p, sizeof (T));
    }
};


#endif //MYTINYSTL_ALLOCATOR_H
