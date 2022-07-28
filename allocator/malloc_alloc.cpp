//
// Created by Lenovo on 2022/7/28.
//


#include "malloc_alloc.h"

void (*malloc_alloc::_malloc_alloc_oom_handler)() = nullptr;

void malloc_alloc::deallocate(void *p, size_t) {
    free(p);
}

void *malloc_alloc::allocate(size_t n) {
    void *result = malloc(n);
    if(!result)
        result = oom_malloc(n);
    return result;
}

void *malloc_alloc::reallocate(void *p, size_t, size_t new_sz) {
    void *result = realloc(p, new_sz);
    if(!result)
        result = oom_realloc(p, new_sz);
    return result;
}

void *malloc_alloc::oom_malloc(size_t n) {
    void (* my_alloc_handler)();
    void *result;
    while(true) ///尝试释放、配置、再释放、再配置
    {
        my_alloc_handler = _malloc_alloc_oom_handler;
        if (!my_alloc_handler) {
            __THROW_BAD_ALLOC;
        }
        (*my_alloc_handler)();
        result = malloc(n);
        if(result)
            return result;
    }
}

void *malloc_alloc::oom_realloc(void *p, size_t n) {
    void (* my_alloc_handler)();
    void *result;

    while(true)
    {
        my_alloc_handler = _malloc_alloc_oom_handler;
        if (!my_alloc_handler) {
            __THROW_BAD_ALLOC;
        }
        (*my_alloc_handler)();
        result = realloc(p, n);
        if(result)return result;
    }
}
