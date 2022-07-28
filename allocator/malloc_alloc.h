//
// Created by Lenovo on 2022/7/28.
//

#ifndef MYTINYSTL_MALLOC_ALLOC_H
#define MYTINYSTL_MALLOC_ALLOC_H

#include <new>
#include <cstddef>
#include <cstdlib>

#if 0
#define __THROW_BAD_ALLOC throw std::bad_alloc
#elif !(defined(__THROW_BAD_ALLOC))
#include <iostream>
# define __THROW_BAD_ALLOC std::cerr << "out of memory" << std::endl; exit(1)
#endif

/// 一级内存配置器
class malloc_alloc {
private:
    ///处理内存不足的情况
    static void *oom_malloc(size_t);
    static void *oom_realloc(void*, size_t);
    static void (* _malloc_alloc_oom_handler)();

public:
    static void* allocate(size_t n);
    static void deallocate(void *p, size_t);
    static void* reallocate(void *p, size_t, size_t new_sz);

    static void (*set_malloc_handler(void (*f)()))()
    {
        void (*old)() = _malloc_alloc_oom_handler;
        _malloc_alloc_oom_handler = f;
        return(old);
    }
};


#endif //MYTINYSTL_MALLOC_ALLOC_H
