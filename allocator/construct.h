//
// Created by Lenovo on 2022/7/30.
//

#ifndef MYTINYSTL_CONSTRUCT_H
#define MYTINYSTL_CONSTRUCT_H

#include <new>
#include "../iterator/iterator.h"
#include "../iterator/type_traits.h"

///使用emplacement new
template<typename T1, typename  T2>
inline void construct(T1* p, const T2& value){
    new (p) T1(value);
}

template <typename T>
inline void destroy(T* pointer){
    pointer->~T();
}


template <typename Ty, typename... Args>
void construct(Ty* ptr, Args&&... args)
{
    ::new ((void*)ptr) Ty(forward<Args>(args)...);
}

inline void destroy(char*, char*){}
inline void destroy(wchar_t*, wchar_t*){}

template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type){}

template<typename ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type){
    for(; first < last; ++first)
        destroy(&*first);
}

template<typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*){
    typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
    __destroy_aux(first, last, trivial_destructor());
}

template<class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last){
    __destroy(first, last, value_type(first));
}


#endif //MYTINYSTL_CONSTRUCT_H
