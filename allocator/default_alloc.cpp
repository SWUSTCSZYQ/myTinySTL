//
// Created by Lenovo on 2022/7/28.
//

#include "default_alloc.h"

char* default_alloc::start_free = nullptr;
char* default_alloc::end_free = nullptr;
size_t default_alloc::heap_size = 0;
default_alloc::obj* volatile default_alloc::free_list[__NFREELISTS] =
        {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
         nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
         nullptr, nullptr};

void *default_alloc::allocate(size_t n) {
    ///大于128调用一级配置器
    if(n > (size_t) __MAX_BYTES)
        return malloc_alloc::allocate(n);

    ///寻找16个free lists中适当的一个
    obj* volatile *my_free_list = free_list + FREELIST_INDEX(n);
    obj* result = *my_free_list;
    ///没有可用的free list， 重新填充
    if(!result)
        return refill(ROUND_UP(n));

    ///调整free list
    *my_free_list = result->free_list_link;
    return result;
}

void default_alloc::deallocate(void *p, size_t n) {
    if(n > (size_t)__MAX_BYTES)
        return malloc_alloc::deallocate(p, n);

    obj* volatile *my_free_list = free_list + FREELIST_INDEX(n);
    obj *q = (obj*)p;
    ///回收区块
    q->free_list_link = *my_free_list;
    *my_free_list = q;
}

void *default_alloc::reallocate(void *p, size_t old_sz, size_t new_sz) {
    if(old_sz > (size_t)__MAX_BYTES)
        return malloc_alloc::reallocate(p, old_sz, new_sz);
    deallocate(p, old_sz);
    return allocate(new_sz);
}

char *default_alloc::chunk_alloc(size_t size, int& nobjs) {
    char *result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free; ///内存池剩余空间

    if(bytes_left >= total_bytes)
    {
        ///内存池剩余空间满足需求量
        result = start_free;
        start_free += total_bytes;
        return result;
    }
    else if(bytes_left >= size)
    {
        ///内存池剩余空间不能完全满足需求，但能提供一个以上的区块
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;
    }
    else
    {
        ///内存池剩余空间连一个区块的大小都无法提供
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        if(bytes_left > 0)
        {
            ///内存池中还有剩余空间，将其分给合适的free list
            obj* volatile *my_free_list = free_list + FREELIST_INDEX(bytes_left);
            ///将内存池中的残余空间编入
            ((obj*)start_free)->free_list_link = *my_free_list;
            *my_free_list = (obj*)start_free;
        }

        ///配置堆空间，用来补充内存池
        start_free = (char*)malloc(bytes_to_get);
        if(start_free == nullptr)
        {
            ///堆空间不足，malloc失败
            obj* volatile *my_free_list;
            for(size_t i = size; i <= __MAX_BYTES; i += __ALIGN)
            {
                my_free_list = free_list + FREELIST_INDEX(i);
                obj* p = *my_free_list;
                if(p)
                {
                    *my_free_list = p->free_list_link;
                    start_free = (char*)p;
                    end_free = start_free + i;
                    return (chunk_alloc(size, nobjs));
                }
            }
            end_free = nullptr;
            start_free = (char*)malloc_alloc::allocate(bytes_to_get);
        }
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        return chunk_alloc(size, nobjs);
    }
}

void *default_alloc::refill(size_t n) {
    int nobjs = 20;
    char* chunk = chunk_alloc(n, nobjs);
    obj* volatile *my_free_list;
    obj *result, *current_obj, *next_obj;

    //如果只有一个区块的空间，就直接分配给调用者
    if(nobjs == 1)return chunk;
    my_free_list = free_list + FREELIST_INDEX(n);

    result = (obj*)chunk;
    *my_free_list = next_obj = (obj*)(chunk + n);
    for(int i = 1;;++i)
    {
        current_obj = next_obj;
        next_obj = (obj*)((char*)next_obj + n);
        if(nobjs -1 == i)
        {
            current_obj->free_list_link = nullptr;
            break;
        }else
            current_obj->free_list_link = next_obj;
    }
    return result;
}
