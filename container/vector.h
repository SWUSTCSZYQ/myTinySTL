//
// Created by Lenovo on 2022/7/31.
//

#ifndef MYTINYSTL_VECTOR_H
#define MYTINYSTL_VECTOR_H

#include "../allocator/allocator.h"
#include "../tools/memory_uninitialized.h"
#include "../allocator/construct.h"

template<typename T, typename Alloc = allocator<T>>
class vector {
public:
    ///嵌套型别定义
    typedef T           value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

protected:
    ///空间配置器
    typedef Alloc data_allocator;
    iterator start;             ///使用空间的头
    iterator finish;            ///使用空间的尾
    iterator end_of_storage;    ///可用空间的尾

    void insert_aux(iterator position, T x);
    void deallocate(){
        if(start)
            data_allocator ::deallocate(start, end_of_storage - start);
    }

    void fill_initialize(size_type n, const T& value){
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

public:
    iterator begin()  {return start;}
    iterator end()  {return finish;}
    size_type size() { return size_type (end() - begin());}
    size_type capacity() const{
        return size_type (end_of_storage - begin());
    }
    bool empty(){
        return begin() == end();
    }
    reference operator[](size_type n){
        return *(begin() + n);
    }
    vector():start(nullptr), finish(nullptr), end_of_storage(nullptr){}
    vector(int n, const T& value){
        fill_initialize(n, value);
    }
    vector(long n, const T& value){
        fill_initialize(n, value);
    }
    explicit vector(size_type n){
        fill_initialize(n, T());
    }

    vector(vector &others):vector(others.size()){
        uninitialized_copy(others.begin(), others.end(), start);
    }
    vector(vector &&other) noexcept
    :start(other.start),
    finish(other.finish),
    end_of_storage(other.end_of_storage)
    {
        other.start = nullptr;
        other.finish = nullptr;
        other.end_of_storage = nullptr;
    }
    vector(std::initializer_list<value_type> list)
    :vector(list.size())
    {
        uninitialized_copy(list.begin(), list.end(), start);
    }

    vector& operator=(const vector& others);
    vector& operator=(const vector&& others);

    bool operator==(vector& other){
        if(other.size() != size())return false;
        auto iter = other.start;
        auto cur = start;
        while(cur != finish)
        {
            if(*iter != *cur)return false;
            ++cur;
            ++iter;
        }
        return true;
    }

    void swap(vector& other){
        swap(start, other.start);
        swap(finish, other.finish);
        swap(end_of_storage, other.end_of_storage);
    }

    ~vector(){
        destroy(start, finish);
        deallocate();
    }

    reference front(){
        return *begin();
    }

    reference back(){
        return *(end() - 1);
    }

    void push_back(T x){
        if(finish != end_of_storage){
            construct(finish, x);
            ++finish;
        }
        else insert_aux(end(), x);
    }

    void pop_back(){
        --finish;
        destroy(finish);
    }

    iterator erase(iterator position){
        if(position + 1 != end()){
            std::copy(position + 1, finish, position); ///将position + 1 到finish的值拷贝到从finish开始的位置
        }
        --finish;
        destroy(finish);
        return position;
    }
    iterator erase(iterator first, iterator last)
    {
        iterator i = std::copy(last, finish, first);
        destroy(i, finish);
        finish -= last - first;
        return first;
    }

    void resize(size_type new_size, const T& x){
        if(new_size < size()){
            erase(begin() + new_size, end());
        }
        else insert(end(), new_size - size(), x);
    }

    void resize(size_type new_size){
        resize(new_size, T());
    }

    void insert(pointer position, size_type n, T x);

    void clear(){
        erase(begin(), end());
    }



protected:
    iterator allocate_and_fill(size_type n, const T& x){
        iterator result = data_allocator ::allocate(n);
        uninitialized_fill_n(result, n, x);
        return result;
    }
};


template<typename T, typename Alloc>
void vector<T, Alloc>::insert_aux(vector::iterator position, T x) {
    if(finish != end_of_storage){
        value_type val = (finish == begin() ? T() : *(finish - 1));
        construct(finish, val);
        ++finish;
        T x_copy = x;
        std::copy_backward(position, finish - 2, finish - 1);
        *position = x_copy;
    }
    else{
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;

        iterator new_start = data_allocator ::allocate(len);
        iterator new_finish = new_start;

        try{
            new_finish = uninitialized_copy(start, position, new_start);
            construct(new_finish, x);
            ++new_finish;
            new_finish = uninitialized_copy(position, finish, new_finish);
        }
        catch (...){
            destroy(new_start, new_finish);
            data_allocator ::deallocate(new_start, len);
            throw ;
        }

        destroy(begin(), end());
        deallocate();

        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;

    }
}

template<typename T, typename Alloc>
void vector<T, Alloc>::insert(vector::pointer position, vector::size_type n, T x) {
    if(n != 0){
        if(size_type(end_of_storage - finish) >= n){
            T x_copy = x;
            const size_type elem_after = finish - position;
            iterator old_finish = finish;
            if(elem_after > n){
                ///插入点之后的元素个数大于要插入的元素个数
                uninitialized_copy(finish - n, finish, finish);
                finish += n;
                std::copy_backward(position, old_finish - n, old_finish);
                std::fill(position, position + n, x_copy);
            }
            else{
                ///插入点之后的元素个数小于等于要插入的元素个数
                uninitialized_fill_n(finish, n - elem_after, x_copy);
                finish += n - elem_after;
                uninitialized_copy(position, old_finish, finish);
                finish += elem_after;
                std::fill(position, old_finish, x_copy);
            }
        }
        else{
            ///需要配置额外的内存
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size, n);
            iterator new_start = data_allocator ::allocate(len);
            iterator new_finish = new_start;
            new_finish = uninitialized_copy(start, position, new_start);
            new_finish = uninitialized_fill_n(new_finish, n, x);
            new_finish = uninitialized_copy(position, finish, new_finish);
            destroy(start, finish);
            deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
}

template<typename T, typename Alloc>
vector<T, Alloc> &vector<T, Alloc>::operator=(const vector &others) {
    if(this != &others)
    {
        const auto len = others.size();
        if(len > capacity())
        {
            vector<int> temp(others.begin(), others.end());
            swap(temp);
        }
        else if(size() >= len)
        {
            auto iter = std::copy(others.begin(), others.end(), begin());
            data_allocator ::destroy(iter, finish);
            finish = start + len;
        }
        else
        {
            std::copy(others.begin(), others.end(), start);
            uninitialized_copy(others.begin() + size(), others.end(), finish);
            end_of_storage = finish = start + len;
        }
    }
    return *this;
}

template<typename T, typename Alloc>
vector<T, Alloc> &vector<T, Alloc>::operator=(const vector &&others) {
    destroy(start, end_of_storage);
    start = others.start;
    finish = others.finish;
    end_of_storage = others.end_of_storage;
    others.start = nullptr;
    others.finish = nullptr;
    others.end_of_storage = nullptr;
}


#endif //MYTINYSTL_VECTOR_H
