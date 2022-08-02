//
// Created by Lenovo on 2022/8/1.
//

#ifndef MYTINYSTL_DEQUE_H
#define MYTINYSTL_DEQUE_H


#include <cstddef>
#include "../iterator/iterator.h"
#include "../allocator/allocator.h"
#include "../tools/memory_uninitialized.h"

///如果n为0，buffer size使用默认值，如果sz小于512 传回512/sz，否则传回1；
inline size_t deque_buf_size(size_t n, size_t sz){
    return n ? n : (sz < 512 ?  size_t(512 / sz) : size_t(1));
}

///deque迭代器
template<typename T, typename Ref, typename Ptr, size_t BufSize>
struct deque_iterator: public iterator<random_access_iterator_tag, T>{
    typedef deque_iterator<T, T&, T*, BufSize>              iterator;
    typedef deque_iterator<T, const T&, const T*, BufSize>  const_iterator;
    static size_t buffer_size(){
        return deque_buf_size(BufSize, sizeof(T));
    }

    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef Ptr                         pointer;
    typedef Ref                         reference;
    typedef size_t                      size_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          value_pointer;
    typedef T**                         map_pointer;

    typedef deque_iterator  self;

    T*          cur;    //缓冲区中现行元素
    T*          first;  //指向缓冲区的头
    T*          last;   //指向缓冲区的尾
    map_pointer node;   //指向管控中心

    ///拷贝、赋值、移动构造函数
    deque_iterator()
    :cur(nullptr), first(nullptr), last(nullptr), node(nullptr)
    {}

    deque_iterator(value_pointer v, map_pointer n)
    :cur(v), first(*n), last(*n + buffer_size()), node(n)
    {}

    deque_iterator(const iterator& rhs)
    :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node)
    {}

    deque_iterator(const iterator&& rhs)
    :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node){
        rhs.cur = nullptr;
        rhs.first = nullptr;
        rhs.last = nullptr;
        rhs.node = nullptr;
    }

    deque_iterator(const const_iterator& rhs)
            :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node)
    {
    }

    self& operator=(const iterator& rhs){
        if(this != &rhs)
        {
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }




    void set_node(map_pointer new_node)
    {
        node = new_node;
        first = *new_node;
        last = first + difference_type (buffer_size());
    }

    reference operator*() const{
        return *cur;
    }
    pointer operator->() const{
        return &(operator*());
    }

    difference_type operator-(const self& x){
        return difference_type (buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
    }

    self& operator++(){
        ++cur;
        if(cur == last){
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    self operator++(int){
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--(){
        if(cur == first){
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int){
        self tmp = *this;
        --*this;
        return tmp;
    }

    ///随机存取
    self& operator+=(difference_type n){
        difference_type offset = n + (cur - first);
        if(offset >= 0 and offset < difference_type(buffer_size())){
            ///目标位置在同一缓冲区
            cur += n;
        }
        else
        {
            difference_type node_offset = offset > 0 ? offset / difference_type (buffer_size()) : -difference_type ((-offset - 1) / buffer_size()) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }

    self operator+(difference_type n) const{
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(difference_type n){
        return  *this += -n;
    }

    self operator-(difference_type n){
        self tmp = *this;
        return tmp -= n;
    }

    reference operator[](difference_type n)const{
        return *(*this + n);
    }

    bool operator==(const self& x)const{
        return cur == x.cur;
    }
    bool operator!=(const self& x)const{
        return cur != x.cur;
    }
    bool operator<(const self& x)const{
        return (node == x.node) ? (cur < x.cur) : (node < x.node);
    }

    bool operator> (const self& rhs) const { return rhs < *this; }
    bool operator<=(const self& rhs) const { return !(rhs < *this); }
    bool operator>=(const self& rhs) const { return !(*this < rhs); }

};

template <typename T, typename Alloc = allocator<T>, size_t BufSiz = 0>
class deque {
public:
    typedef T               value_type;
    typedef value_type*     pointer;
    typedef T&              reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;

    typedef deque_iterator<T, T&, T*, BufSiz>   iterator;

private:
    typedef pointer* map_pointer;

    ///分别用来元素和指针
    typedef Alloc               data_allocator;
    typedef allocator<pointer>  map_allocator;

    ///构造函数
public:
    deque()
    :start(), finish(), map(0), map_size(0)
    {}
    explicit deque(int n)
    :start(), finish(), map(0), map_size(0){
        fill_initialize(n, value_type());
    }
    /*
    deque(deque& others)
    :start(), finish(), map(0), map_size(0){
        copy_initialize(others.begin(), others.end());
    }*/

    deque(const deque& others)
            :start(), finish(), map(0), map_size(0){
        copy_initialize(others.begin(), others.end());
    }

    deque(const deque&& others)noexcept
    :start(others.start), finish(others.finish), map(others.map), map_size(others.map_size){
        others.start = nullptr;
        others.finish = nullptr;
        others.map = nullptr;
        others.map_size = 0;
    }

    deque(int n, const value_type& value)
    :start(), finish(), map(0), map_size(0)
    {
        fill_initialize(n, value);
    }

    deque& operator=(const deque& others);
    deque& operator==(const deque& others);
public:
    ///基本操作,以及用户调用接口
    iterator begin(){
        return start;
    }
    iterator end(){
        return finish;
    }

    ///调用 deque_iterator::operator[]
    reference operator[](size_type n){
        return start[difference_type(n)];
    }

    ///调用 deque_iterator::operator*
    reference front(){
        return *start;
    }

    reference back(){
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    size_type size() {
        return finish - start;;
    }

    bool empty() const{
        return finish == start;
    }

    void push_back(const value_type& value);
    void push_front(const value_type& value);
    void pop_back();
    void pop_front();
    void clear();
    iterator erase(iterator pos);
    ///清除[first, last)之间多的元素
    iterator erase(iterator first, iterator last);
    iterator insert(iterator position, const value_type& x);

private:
    void fill_initialize(size_type n, const value_type& value);
    void copy_initialize(iterator begin, iterator end);
    void create_map_and_nodes(size_type num_elements);
    void push_back_aux(const value_type& val);
    void push_front_aux(const value_type& val);
    void pop_back_aux();
    void pop_front_aux();
    iterator insert_aux(iterator pos, const value_type& x);

    ///在后面或者前面新增节点
    void reserve_map_at_back(size_type nodes_to_add = 1){
        reallocate_map(nodes_to_add, false);
    }
    void reserve_map_at_front(size_type nodes_to_add = 1){
        reallocate_map(nodes_to_add, true);
    }
    void reallocate_map(size_type nodes_to_add, bool add_at_front);
    size_type max(size_type a, size_type b){
        if(a > b)return a;
        else return b;
    }
    void deallocate_node(pointer node){
        map_allocator ::deallocate(node);
    }

private:
    iterator    start;
    iterator    finish;
    map_pointer map;
    ///map内指针数
    size_type   map_size;
    static size_type min_node_num;

    size_type init_map_size();

    pointer allocator_node();
};

///一个map所要管理的最少节点数
template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::size_type deque<T, Alloc, BufSiz>::init_map_size() {
    return min_node_num;
}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::size_type deque<T, Alloc, BufSiz>::min_node_num = 8;


template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::fill_initialize(deque::size_type n, const value_type& value) {
    create_map_and_nodes(n);
    for(map_pointer cur = start.node; cur < finish.node; ++cur){
        uninitialized_fill(*cur, *cur + iterator::buffer_size(), value);
    }
    uninitialized_fill(finish.first, finish.cur, value);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::create_map_and_nodes(deque::size_type num_elements) {
    ///需要节点数 = 元素个数 / 每个缓冲区可容纳的元素个数 + 1
    size_type num_nodes = num_elements / iterator::buffer_size() + 1;
    ///一个map需要管理的节点数， 最多为所需节点数 + 2
    map_size = max(init_map_size(), num_nodes + 2);
    map = map_allocator ::allocate(map_size);

    ///保持在最中间，使其可以向两头扩充
    map_pointer n_start = map + (map_size - num_nodes) / 2;
    map_pointer n_finish = n_start + num_nodes - 1;
    for(map_pointer cur = n_start; cur <= n_finish; ++cur){
        *cur = allocator_node();
    }
    start.set_node(n_start);
    finish.set_node(n_finish);
    start.cur = start.first;
    finish.cur = finish.first + num_elements % iterator::buffer_size();

}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::pointer deque<T, Alloc, BufSiz>::allocator_node() {
    return data_allocator ::allocate(iterator::buffer_size());
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::push_back(const value_type &value) {
    if(finish.cur != finish.last - 1){
        construct(finish.cur, value);
        ++finish.cur;
    }
    else{
        push_back_aux(value);
    }
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::push_back_aux(const value_type &val) {
    value_type val_copy = val;
    reserve_map_at_back();
    *(finish.node + 1) = allocator_node();
    construct(finish.cur, val_copy);
    finish.set_node(finish.node + 1);
    finish.cur = finish.first;
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + nodes_to_add;

    map_pointer new_n_start;
    if(map_size > 2 * new_num_nodes){
        ///map中有剩余空间
        new_n_start = map + (map_size - new_num_nodes / 2)
                + (add_at_front ? nodes_to_add : 0);
        if(new_n_start < start.node)
            std::copy(start.node, finish.node + 1, new_n_start);
        else
            std::copy_backward(start.node, finish.node + 1, new_n_start + old_num_nodes);

    }
    else{
        size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
        ///配置新的空间给map使用
        map_pointer new_map = map_allocator ::allocate(new_map_size);
        new_n_start = new_map + (new_map_size - new_num_nodes) / 2
                + (add_at_front ? nodes_to_add : 0);
        std::copy(start.node, finish.node + 1, new_n_start);
        map_allocator ::deallocate(map, map_size);
        map = new_map;
        map_size = new_map_size;
    }
    start.set_node(new_n_start);
    finish.set_node(new_n_start + old_num_nodes - 1);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::push_front(const value_type &value) {
    if(start.cur != start.first){
        construct(start.cur - 1, value);
        --start.cur;
    }
    else{
        push_front_aux(value);
    }
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::push_front_aux(const value_type &val) {
    value_type val_copy = val;
    reserve_map_at_front();
    *(start.node - 1) = allocator_node();
    start.set_node(start.node - 1);
    start.cur = start.last - 1;
    construct(start.cur, val_copy);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::pop_back() {
    if(finish.cur != finish.first){
        --finish.cur;
        data_allocator ::destory(finish.cur);
    }
    else
        pop_back_aux();
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::pop_front() {
    if(start.cur != start.last - 1){
        destroy(start.cur);
        ++start.cur;
    }
    else
        pop_front_aux();
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::pop_back_aux() {
    deallocate_node(finish.first);
    finish.set_node(finish.node - 1);
    finish.cur = finish.last - 1;
    destroy(finish.cur);
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::pop_front_aux() {
    destroy(start.cur);
    deallocate_node(start.first);
    start.set_node(start.node + 1);
    start.cur = start.first;
}

template<typename T, typename Alloc, size_t BufSiz>
void deque<T, Alloc, BufSiz>::clear() {
    for(map_pointer node = start.node + 1; node < finish.node; ++node){
        destroy(*node, *node + iterator::buffer_size());
    }
    ///至少有头尾两个缓冲区
    if(start.node != finish.node){
        destroy(start.cur, start.last);
        destroy(finish.first, finish.last);
        data_allocator ::deallocate(finish.first, iterator::buffer_size());
    }
    else{
        destroy(start.cur, finish.cur);
    }
    finish = start;
}


template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::iterator deque<T, Alloc, BufSiz>::erase(deque::iterator pos) {
    iterator next = pos + 1;
    difference_type index = pos - start;
    if(index < size() >> 1){
        std::copy_backward(start, pos, next);
        pop_front();
    }
    else{
        std::copy(next, finish, pos);
        pop_back();
    }
    return start + index;
}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::iterator deque<T, Alloc, BufSiz>::erase(deque::iterator first, iterator last) {
    if(first == start and last == finish){
        clear();
        return finish;
    }
    difference_type n = last - first;
    difference_type elems_before = first - start;
    if(elems_before < (size() - n) / 2){
        std::copy_backward(start, first, last);
        iterator new_start = start + n;
        destroy(start, new_start);
        ///释放冗余缓冲区
        for(map_pointer cur = start.node; cur < new_start.node; ++cur)
            data_allocator ::deallocate(*cur, iterator::buffer_size());
        start = new_start;
    }
    else{
        std::copy(last, finish, first);
        iterator new_finish = finish - n;
        destroy(new_finish, finish);
        for(map_pointer cur = new_finish.node; cur < finish.node; ++cur)
            data_allocator ::deallocate(*cur, iterator::buffer_size());
        finish = new_finish;
    }
    return start + elems_before;
}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::iterator deque<T, Alloc, BufSiz>::insert(iterator position, const value_type& x) {
    if(position.cur == start.cur){
        pop_front(x);
        return start;
    }
    else if(position.cur == finish.cur){
        pop_back(x);
        iterator tmp = finish;
        --tmp;
        return tmp;
    }
    return insert_aux(position, x);
}

template<typename T, typename Alloc, size_t BufSiz>
typename deque<T, Alloc, BufSiz>::iterator deque<T, Alloc, BufSiz>::insert_aux(iterator pos, const value_type& x) {
    difference_type index = pos - start;
    value_type x_copy = x;
    if(index < size() < 2){
        push_front(front());
        iterator front1 = start + 1;
        iterator front2 = front1 + 1;
        pos = start + index;
        iterator pos1 = pos + 1;
        std::copy(front2, pos1, front1);
    }
    else{
        push_back(back());
        iterator back1 = finish - 1;
        iterator back2 = back1 - 1;
        pos = start + index;
        std::copy_backward(pos, back2, back1);
    }
    *pos = x_copy;
    return pos;
}




#endif //MYTINYSTL_DEQUE_H
