//
// Created by Lenovo on 2022/8/16.
//

#ifndef MYTINYSTL_INITIALIZER_LIST_H
#define MYTINYSTL_INITIALIZER_LIST_H

#include <cstddef>


///在栈上直接构造一个数组 const T[N]。在栈上构造的数组会像其他变量一样，在离开作用域时自动析构，不需要手动管理内存。std::array 也是如此，它仅在其基础之上做了一层包装，使数组的行为如同其它容器一样
template<typename T>
class initializer_list
{
public:
    typedef  T          value_type;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef T*          iterator;
    typedef const T*    const_iterator;

private:
    iterator    m_array;
    size_type   m_len;

    ///编译器可以调用私有的构造函数
    constexpr initializer_list(const_iterator _a, size_t _l)
    :m_array(_a), m_len(_l){}

public:
    constexpr initializer_list() noexcept
            : m_array(0), m_len(0) { }

    // Number of elements.
    constexpr size_type
    size() const noexcept { return m_len; }

    // First element.
    constexpr const_iterator
    begin() const noexcept { return m_array; }

    // One past the last element.
    constexpr const_iterator
    end() const noexcept { return begin() + size(); }

};


#endif //MYTINYSTL_INITIALIZER_LIST_H
