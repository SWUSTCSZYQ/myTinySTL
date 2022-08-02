//
// Created by Lenovo on 2022/8/2.
//

#ifndef MYTINYSTL_QUEUE_H
#define MYTINYSTL_QUEUE_H

#include "../container/deque.h"

template<typename T, class Sequence = deque<T>>
class queue{
    friend bool operator==(const queue&, const queue&);
    friend bool operator!=(const queue&, const queue&);
    friend bool operator<(const queue&, const queue&);
    friend bool operator>(const queue&, const queue&);
    friend bool operator<=(const queue&, const queue&);
    friend bool operator>=(const queue&, const queue&);

public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;
public:
    ///构造函数
    queue():c(){}
public:
    ///基本操作
    bool empty() const{
        return c.empty();
    }

    size_type size(){
        return c.size();
    }

    reference front(){
        return c.front();
    }

    reference back(){
        return c.back();
    }

    void push(const value_type& x){
        c.push_back(x);
    }
    void pop(){
        c.pop_front();
    }

private:
    Sequence c;
};

template<typename T, typename Sequence>
bool operator==(const queue<T, Sequence>&x, const queue<T, Sequence>&y){
    return x.c == y.c;
}
template<typename T, typename Sequence>
bool operator!=(const queue<T, Sequence>&x, const queue<T, Sequence>&y){
    return x.c != y.c;
}

template<typename T, typename Sequence>
bool operator<(const queue<T, Sequence>&x, const queue<T, Sequence>&y){
    return x.c < y.c;
}

template<typename T, typename Sequence>
bool operator>(const queue<T, Sequence>&x, const queue<T, Sequence>&y){
    return x.c > y.c;
}

template<typename T, typename Sequence>
bool operator<=(const queue<T, Sequence>&x, const queue<T, Sequence>&y){
    return x.c <= y.c;
}

template<typename T, typename Sequence>
bool operator>=(const queue<T, Sequence>&x, const queue<T, Sequence>&y){
    return x.c >= y.c;
}

#endif //MYTINYSTL_QUEUE_H
