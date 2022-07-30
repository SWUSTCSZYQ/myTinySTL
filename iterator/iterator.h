//
// Created by Lenovo on 2022/7/30.
//

#include <cstddef>

#ifndef MYTINYSTL_ITERATOR_H
#define MYTINYSTL_ITERATOR_H

/// 五种迭代器类型
struct input_iterator_tag{};
struct output_iterator_tag{};
struct forward_iterator_tag:public input_iterator_tag{};
struct bidirectional_iterator_tag: public forward_iterator_tag{};
struct random_access_iterator_tag:public  bidirectional_iterator_tag{};

template<typename Category, class T, class Distance = ptrdiff_t,
        class Pointer = T*, class Reference = T&>
                struct iterator{
                    typedef Category    iterator_category;
                    typedef T           value_type;
                    typedef Distance    difference_type;
                    typedef Pointer     pointer;
                    typedef Reference   reference;
                };

/// iterator 模板
template<typename Iterator>
struct iterator_traits{
    typedef typename Iterator::iterator_category    iterator_category;
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::difference_type      difference_type;
    typedef typename Iterator::pointer              pointer;
    typedef typename Iterator::reference            reference;
};

///针对指针的特化版本
template<typename T>
struct iterator_traits<T*>{
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};

template<typename T>
struct iterator_traits<const T*>{
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};

template<typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&){
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return category();
}

template<typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

template<typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator){
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}


/// distance 的 input_iterator_tag 的版本
template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag){
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last){
        ++first;
        ++n;
    }
    return n;
}

/// distance 的 random_access_iterator_tag 的版本
template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, random_access_iterator_tag){
    return last - first;
}

template<typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last){
    typedef typename iterator_traits<InputIterator>::iterator_category category;
    return __distance(first, last, category());
}

template<class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag){
    while(n--)++i;
}

template<class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, bidirectional_iterator_tag){
    if(n >= 0)
    {
        while(n--)++i;
    }
    else
        while(n++)--i;
}

template<class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, random_access_iterator_tag){
    i += n;
}

template<class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n){
    __advance(i, n, iterator_category(i));
}


// 模板类 : reverse_iterator
// 代表反向迭代器，使前进为后退，后退为前进
template <class Iterator>
class reverse_iterator
{
private:
    Iterator current;  // 记录对应的正向迭代器

public:
    // 反向迭代器的五种相应型别
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
    typedef typename iterator_traits<Iterator>::value_type        value_type;
    typedef typename iterator_traits<Iterator>::difference_type   difference_type;
    typedef typename iterator_traits<Iterator>::pointer           pointer;
    typedef typename iterator_traits<Iterator>::reference         reference;

    typedef Iterator                                              iterator_type;
    typedef reverse_iterator<Iterator>                            self;

public:
    // 构造函数
    reverse_iterator() {}
    explicit reverse_iterator(iterator_type i) :current(i) {}
    reverse_iterator(const self& rhs) :current(rhs.current) {}

public:
    // 取出对应的正向迭代器
    iterator_type base() const
    { return current; }

    // 重载操作符
    reference operator*() const
    { // 实际对应正向迭代器的前一个位置
        auto tmp = current;
        return *--tmp;
    }
    pointer operator->() const
    {
        return &(operator*());
    }

    // 前进(++)变为后退(--)
    self& operator++()
    {
        --current;
        return *this;
    }
    self operator++(int)
    {
        self tmp = *this;
        --current;
        return tmp;
    }
    // 后退(--)变为前进(++)
    self& operator--()
    {
        ++current;
        return *this;
    }
    self operator--(int)
    {
        self tmp = *this;
        ++current;
        return tmp;
    }

    self& operator+=(difference_type n)
    {
        current -= n;
        return *this;
    }
    self operator+(difference_type n) const
    {
        return self(current - n);
    }
    self& operator-=(difference_type n)
    {
        current += n;
        return *this;
    }
    self operator-(difference_type n) const
    {
        return self(current + n);
    }

    reference operator[](difference_type n) const
    {
        return *(*this + n);
    }
};

// 重载 operator-
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs,
          const reverse_iterator<Iterator>& rhs)
{
    return rhs.base() - lhs.base();
}

// 重载比较操作符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
    return lhs.base() == rhs.base();
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs)
{
    return rhs.base() < lhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
    return !(lhs == rhs);
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs)
{
    return rhs < lhs;
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
    return !(rhs < lhs);
}

template <class Iterator>
bool operator>=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
    return !(lhs < rhs);
}

#endif //MYTINYSTL_ITERATOR_H
