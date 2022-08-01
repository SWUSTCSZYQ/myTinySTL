//
// Created by Lenovo on 2022/7/31.
//

#ifndef MYTINYSTL_LIST_H
#define MYTINYSTL_LIST_H

#include <algorithm>
#include <initializer_list>
#include "../iterator/iterator.h"
#include "../allocator/allocator.h"
#include "../allocator/construct.h"

///list节点采用双向链表
template<typename T>
struct __list_node{
    typedef void* void_pointer;
    void_pointer prev;
    void_pointer next;
    T data;

    __list_node() = default;

    explicit __list_node(const T& x)
    :prev(nullptr),
    next(nullptr),
    data(x)
    {}

    __list_node(const __list_node& others)
    :prev(others.prev),
    next(others.next),
    data(others.data)
    {}

    explicit __list_node(T&& val)
    :data(std::move(val)),
    prev(nullptr),
    next(nullptr)
    {}

};

template <typename T, typename Ref, typename Ptr>
struct __list_iterator : public iterator<bidirectional_iterator_tag, T>{
    typedef __list_iterator<T, T*, T&>      iterator;
    typedef __list_iterator<T, Ref, Ptr>    self;

    typedef bidirectional_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef Ptr                         pointer;
    typedef Ref                         reference;
    typedef __list_node<T>*             link_type;
    typedef size_t                      size_type;
    typedef ptrdiff_t                   difference_type;

    link_type node;

    ///构造函数
    __list_iterator(const link_type& x): node(x){}
    __list_iterator()= default;
    __list_iterator(const iterator& x):node(x.node){}

    reference operator*(){
        return node->data;
    }

    bool operator==(const self& x)const{
        return node == x.node;
    }
    bool operator!=(const self& x)const{
        return node != x.node;
    }

    reference operator*()const{
        return (*node).data;
    }

    pointer operator->()const{
        return &(operator*());
    }

    self& operator++(){
        node = (link_type)((*node).next);
        return *this;
    }

    self operator++(int){
        self temp = *this;
        ++*this;
        return temp;
    }

    self& operator--(){
        node = (link_type)((*node).prev);
        return *this;
    }

    self operator--(int){
        self temp = *this;
        --*this;
        return temp;
    }
};

template<typename T, typename Alloc = allocator<T>>
class list {
public:
    typedef __list_node<T>              list_node;
    typedef __list_iterator<T, T&, T*>  iterator;
    typedef list_node*                  link_type;

    typedef Alloc                       data_allocator;
    typedef allocator<__list_node<T>>   node_allocator;

    typedef typename iterator ::value_type         value_type;
    typedef typename iterator ::pointer            pointer;
    typedef typename iterator ::reference          reference;
    typedef typename iterator ::size_type          size_type;
    typedef typename iterator ::difference_type    difference_type;

private:
    link_type node;
    size_type size_;

    ///构造函数
public:
    list()
    :node(nullptr), size_(0){
        empty_initialize();
    }

    explicit list(size_type n)
    :node(nullptr), size_(n){
        fill_init(n, value_type());
    }

    list(size_type n, const T& val)
    :node(nullptr), size_(n){
        fill_init(n, val);
    }

    list(std::initializer_list<value_type> init):size_(0){
        empty_initialize();
        for(auto iter = init.begin(); iter != init.end(); ++iter)
        {
            insert(end(), *iter);
            ++size_;
        }
    }

    list(list& rhs){
        size_ = rhs.size_;
        copy_init(rhs.begin(), rhs.end());
    }

    list(const list&& rhs)noexcept
    :node(rhs.node), size_(rhs.size_){
        rhs.node = nullptr;
        rhs.size_ = 0;
    }

    list& operator=(const list& rhs)
    {
        if(this != &rhs){
            ///深拷贝元素值
            copy_init(rhs.begin(), rhs.end());
            size_ = rhs.size_;
        }
        return *this;
    }

    list& operator=(list&& rhs) noexcept {
        clear();
        size_ = rhs.size_;
        rhs.size_ = 0;
        splice(end(), rhs);
        return *this;
    }

public:
    iterator begin(){
        return (link_type)((*node).next);
    }
    iterator end(){
        return (link_type)(node);
    }
    bool empty() const{
        return node->next == node;
    }

    size_type size(){
        return size_;
    }
    reference front(){
        return *begin();
    }
    reference back(){
        return *(--end());
    }
    void clear();
    iterator insert(iterator position, const T& x);
    void push_front(const T& x){
        insert(begin(), x);
    }
    void push_back(const T& x){
        insert(end(), x);
    }

    iterator erase(iterator position);
    void pop_front(){
        erase(begin());
    }
    void pop_back(){
        iterator tmp = end();
        erase(--tmp);
    }

    ///移除所有值为value的节点
    void remove(const T& value);

private:
    ///配置一个节点空间，并返回地址
    link_type get_node(){
        return node_allocator ::allocate();
    }

    ///释放一个节点
    void put_node(link_type p){
        node_allocator ::deallocate(p);
    }

    ///产生一个带元素值得节点
    link_type create_node(const T& x){
        link_type p = get_node();
        construct(p, x);
        return p;
    }

    void destroy_node(link_type p){
        destroy(&p->data);
        put_node(p);
    }

    void empty_initialize(){
        node = get_node();
        node->prev = node;
        node->next = node;
    }

    ///初始化n个值为val的元素
    void fill_init(size_type n, const T& val);
    void copy_init(iterator begin, iterator end);
    void splice(iterator position, list& rhs);

};

template<typename T, typename Alloc>
void list<T, Alloc>::fill_init(size_type n, const T& val) {
    empty_initialize();
    for(int i = 0; i < n; ++i)
    {
        link_type cur = create_node(val);
        ///插入到list尾部
        cur->next = node;
        cur->prev = node->prev;
        (link_type(node->prev))->next = cur;
        node->prev = cur;
    }
}

template<typename T, typename Alloc>
void list<T, Alloc>::copy_init(iterator begin, iterator end) {

    node = get_node();
    for(auto iter = begin; iter != end; ++iter)
    {
        link_type cur = create_node(*iter);
        cur->next = node;
        cur->prev = node->prev;
        (link_type(node->prev))->next = cur;
        node->prev = cur;
    }
}

template<typename T, typename Alloc>
void list<T, Alloc>::splice(list::iterator position, list &rhs) {

    while(rhs.node->next != rhs.node)
    {
        ///取出节点
        iterator cur = rhs.node->next;
        rhs.node->next->next->prev = rhs.node;
        rhs.node->next = rhs.node->next->next;

        ///插入到position前面
        cur->next = position;
        cur->prev = position->prev;
        position->prev->next = cur;
        position->prev = cur;
    }
}

template<typename T, typename Alloc>
void list<T, Alloc>::clear() {
    link_type cur = (link_type)node->next;
    while(cur != node)
    {
        auto tmp = cur;
        cur = (link_type)cur->next;
        destroy_node(tmp);
    }
    node->next = node;
    node->prev = node;
    size_ = 0;
}

template<typename T, typename Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator position, const T& x) {
    link_type tmp = create_node(x);
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    (link_type(position.node->prev))->next = tmp;
    position.node->prev = tmp;
    return tmp;
}

template<typename T, typename Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator position) {
    link_type next_node = link_type (position.node->next);
    link_type prev_node = link_type (position.node->prev);
    prev_node->next = next_node;
    next_node->prev = prev_node;
    destroy_node(position.node);
    return iterator(next_node);
}

template<typename T, typename Alloc>
void list<T, Alloc>::remove(const T &value) {
    iterator first = begin();
    iterator last = end();
    while(first != last)
    {
        iterator next = first;
        ++next;
        if(*first == value)erase(first);
        first = next;
    }
}



#endif //MYTINYSTL_LIST_H
