//
// Created by Lenovo on 2022/8/1.
//

#ifndef MYTINYSTL_ALGORITHM_H
#define MYTINYSTL_ALGORITHM_H

#include "../iterator/iterator.h"

///数值算法

///求和
template<typename InputIterator, typename T>
T accumulate(InputIterator first, InputIterator last, T init){
    while(first++ != last)
    {
        init += *first;
    }
    return init;
}

template<typename InputIterator, typename T, typename BinaryOperator>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperator op)
{
    while(first++ != last)
        init += op(init, *first);
    return init;
}

///计算 first 和 last 中相邻元素的差额
template <typename InputIterator, typename OutputIterator, typename T>
OutputIterator __adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, T*){
    T value = *first;
    while(++first != last){
        T tmp = *first;
        *++result = tmp - value;
        value = tmp;
    }
    return ++result;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result)
{
    if(first == last)return result;
    return __adjacent_difference(first, last, result, value_type(first));
}

template <typename InputIterator, typename OutputIterator, typename T, typename BinaryOperation>
OutputIterator __adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, T*, BinaryOperation op){
    T value = *first;
    while(++first != last){
        T tmp = *first;
        *++result = op(tmp, value);
        value = tmp;
    }
    return ++result;
}

template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, BinaryOperation op)
{
    if(first == last)return result;
    *result = first;
    return __adjacent_difference(first, last, result, value_type(first), op);
}


template<typename ForwardIterator, typename T>
void iota(ForwardIterator first, ForwardIterator last, T value)
{
    while(first != last) *first++ = value++;
}



#endif //MYTINYSTL_ALGORITHM_H
