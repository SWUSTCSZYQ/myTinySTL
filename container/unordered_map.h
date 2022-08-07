//
// Created by Lenovo on 2022/8/5.
//

#ifndef MYTINYSTL_UNORDERED_MAP_H
#define MYTINYSTL_UNORDERED_MAP_H

#include "../allocator/allocator.h"
#include "../iterator/iterator.h"
#include "../functor/hash_fun.h"
#include "../container/hashtable.h"
#include <algorithm>

template<typename Key,
        typename T,
        typename HashFunc = hash<Key>,
        typename EqualKey = std::equal_to<Key>,
        typename Alloc = allocator<Key>>
class unordered_map{

private:
    typedef hashtable<std::pair<const Key, T>, Key, HashFunc, std::_Select1st<std::pair<const Key, T>>, EqualKey, Alloc>    ht;

    ht rep;

public:
    typedef typename ht::key_type   key_type;
    typedef T                       data_type;
    typedef T                       mapped_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher     hasher;
    typedef typename ht::key_equal  key_equal;

    typedef typename ht::size_type          size_type;
    typedef typename ht::difference_type    difference_type;
    typedef typename ht::pointer            pointer;
    typedef typename ht::const_pointer      const_pointer;
    typedef typename ht::reference          reference;
    typedef typename ht::const_reference    const_reference;

    typedef typename ht::iterator   iterator;

    hasher hash_funct(){
        return rep.hash_funct();
    }
    key_equal key_eq(){
        return rep.hash_funct();
    }

public:
    ///构造函数
    unordered_map():rep(100, hasher(), key_equal()){}
    explicit unordered_map(size_type n): rep(n, hasher(), key_equal()){}
    unordered_map(size_type n, const hasher& hf): rep(n, hf, key_equal()){}
    unordered_map(size_type n, const hasher& hf, const key_equal& eql): rep(n, hf, eql){}

    template<typename InputIterator>
    unordered_map(InputIterator first, InputIterator last)
    :rep(100, hasher(), key_equal())
    {
        rep.insert_unique(first, last);
    }

    template<typename InputIterator>
    unordered_map(InputIterator first, InputIterator last, size_type n)
    :rep(n, hasher(), key_equal())
    {
        rep.insert_unique(first, last);
    }
    template<typename InputIterator>
    unordered_map(InputIterator first, InputIterator last, size_type n, const hasher& hf)
    :rep(n, hf, key_equal())
    {
        rep.insert_unique(first, last);
    }

    template<typename InputIterator>
    unordered_map(InputIterator first, InputIterator last, size_type n, const hasher& hf, const key_equal& eql)
    :rep(n, hf, eql)
    {
        rep.insert_unique(first, last);
    }

    bool operator==(const unordered_map& others){
        return rep == others.rep;
    }
    bool operator!=(const unordered_map& others){
        return rep != others.rep;
    }

public:
    ///调用接口
    size_type size(){
        return rep.size();
    }

    size_type max_size(){
        return rep.max_size();
    }

    bool empty(){
        return rep.empty();
    }

    void swap(unordered_map& other){
        rep.swap(other.rep);
    }

    iterator begin(){
        return rep.begin();
    }

    iterator end(){
        return rep.end();
    }

    std::pair<iterator, bool> insert(const value_type& obj){
        return rep.insert_unique(obj);
    }

    template<class InputIterator>
            void insert(InputIterator l, InputIterator r){
        rep.insert_unique(l, r);
    }

    iterator find(const key_type& key){
        return rep.find(key);
    }

    T& operator[](const key_type& key){
        return rep.find_or_insert(value_type(key, T())).second;
    }

    size_type count(const key_type& key){
        return rep.count(key);
    }

    size_type erase(const key_type& key){
        return rep.erase(key);
    }

    void erase(iterator it){
        rep.erase(it);
    }

    void erase(iterator l, iterator r){
        rep.erase(l, r);
    }

    void clear(){
        rep.clear();
    }

    void resize(size_type hint){
        rep.resize(hint);
    }

    size_type bucket_count(){
        return rep.bucket_count();
    }

    size_type max_bucket_count(){
        return rep.max_bucket_count();
    }

    size_type elems_in_bucket(size_type n){
        return rep.elems_in_bucket(n);
    }
};

#endif //MYTINYSTL_UNORDERED_MAP_H
