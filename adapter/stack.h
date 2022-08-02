//
// Created by Lenovo on 2022/8/2.
//

#ifndef MYTINYSTL_STACK_H
#define MYTINYSTL_STACK_H

#include "../container/deque.h"

template<typename T, class Sequence = deque<T>>
class stack{
    friend bool operator==(const stack&, const stack&);
    friend bool operator!=(const stack&, const stack&);
    friend bool operator<(const stack&, const stack&);
    friend bool operator>(const stack&, const stack&);
    friend bool operator<=(const stack&, const stack&);
    friend bool operator>=(const stack&, const stack&);
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;

public:
    ///构造函数
    stack():c(){}
public:
    ///基本操作
    bool empty() const{
        return c.empty();
    }

    size_type size(){
        return c.size();
    }

    reference top(){
        return c.back();
    }

    void push(const value_type& x){
        c.push_back(x);
    }
    void pop(){
        c.pop_back();
    }

private:
    Sequence c;
};

template<typename T, typename Sequence>
bool operator==(const stack<T, Sequence>&x, const stack<T, Sequence>&y){
    return x.c == y.c;
}
template<typename T, typename Sequence>
bool operator!=(const stack<T, Sequence>&x, const stack<T, Sequence>&y){
    return x.c != y.c;
}

template<typename T, typename Sequence>
bool operator<(const stack<T, Sequence>&x, const stack<T, Sequence>&y){
    return x.c < y.c;
}

template<typename T, typename Sequence>
bool operator>(const stack<T, Sequence>&x, const stack<T, Sequence>&y){
    return x.c > y.c;
}

template<typename T, typename Sequence>
bool operator<=(const stack<T, Sequence>&x, const stack<T, Sequence>&y){
    return x.c <= y.c;
}

template<typename T, typename Sequence>
bool operator>=(const stack<T, Sequence>&x, const stack<T, Sequence>&y){
    return x.c >= y.c;
}

#endif //MYTINYSTL_STACK_H
