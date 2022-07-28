//
// Created by Lenovo on 2022/7/28.
//

#ifndef MYTINYSTL_DEFAULT_ALLOC_H
#define MYTINYSTL_DEFAULT_ALLOC_H

#include "malloc_alloc.h"

enum{__ALIGN = 8};                          //小型区块的上调边界
enum{__MAX_BYTES = 128};                    //小型区块的上界
enum{__NFREELISTS = __MAX_BYTES / __ALIGN}; //free list的个数

class default_alloc {
private:
    ///将bytes上调到8的倍数
    static size_t ROUND_UP(size_t bytes){
        return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
    }

private:
    union obj{
        union obj* free_list_link;
        char client_data[1];
    };

private:
    static obj* volatile free_list[__NFREELISTS]; ///16个free list
    ///找到要使用的区块号
    static size_t FREELIST_INDEX(size_t bytes)
    {
        return ((bytes + __ALIGN - 1) / __ALIGN - 1);
    }

    ///返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
    static void *refill(size_t n);

    ///配置一大块空间，可容纳nobjs个大小为size的区块
    static char *chunk_alloc(size_t size, int& nobjs);

    static char *start_free;
    static char *end_free;
    static size_t heap_size;

public:
    static void* allocate(size_t n);
    static void deallocate(void *p, size_t n);
    static void* reallocate(void* p, size_t old_sz, size_t new_sz);
};



#endif //MYTINYSTL_DEFAULT_ALLOC_H
