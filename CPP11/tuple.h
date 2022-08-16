//
// Created by Lenovo on 2022/8/16.
//

#ifndef MYTINYSTL_TUPLE_H
#define MYTINYSTL_TUPLE_H

#include<iostream>

template<typename ... Value>class tuple;

///递归出口
template<>class tuple<>{};

template<typename Head, typename... Tail>
class tuple<Head, Tail...> :private tuple<Tail...>
{
    using self = tuple<Head, Tail...>;
    using inherited = tuple<Tail...>; ///基类类型


public:
    tuple()= default;
    explicit tuple(Head v, Tail... vTail)
    :m_head(v),inherited(vTail...){}

     bool operator==(self& others){
        return m_head == others.m_head and tail() == others.tail();
    }
    bool operator!=(self& others){
        return m_head != others.m_head or tail() != others.tail();
    }

    Head head()
    {
        return m_head;
    }

    inherited& tail(){
        return *this;
    }

protected:
    Head m_head;

};

/// 当元素个数大于1时调用该方法
template<typename Head, typename... Tail>
std::ostream& operator<<(std::ostream& os, tuple<Head, Tail...>& tpe)
{
    os <<  tpe.head() << ",";
    /// 将tpe的基类作为输出的对象 递归调用输出函数
    operator<<(os, tpe.tail());
    return os;
}

/// 当元素个数为 1 是调用该方法
template<typename T>
std::ostream& operator<<(std::ostream& os, tuple<T>& tpe)
{
    os << tpe.head();
    return os;
}

#endif //MYTINYSTL_TUPLE_H
