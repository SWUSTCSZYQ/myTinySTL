//
// Created by Lenovo on 2022/8/4.
//

#ifndef MYTINYSTL_UNORDERED_SET_H
#define MYTINYSTL_UNORDERED_SET_H

#include "../allocator/allocator.h"
#include "../iterator/iterator.h"
#include "../functor/hash_fun.h"
#include "../container/hashtable.h"

template<typename Value,
        typename HashFunc = hash<Value>,
        typename EqualKey = std::equal_to<Value>,
        typename Alloc = allocator<Value>>
class unordered_set{
private:
    template <typename T>
    struct identity
    {
        T operator()(T x) const { return x; }
    };

    typedef hashtable<Value, Value, HashFunc, identity<Value>, EqualKey, Alloc> ht;

public:
    typedef typename ht::key_type   key_type;
    typedef typename ht::value_type value_type;
    typedef typename ht::hasher     hasher;
    typedef typename ht::key_equal  key_equal;

    typedef typename ht::size_type          size_type;
    typedef typename ht::difference_type    difference_type;
    typedef typename ht::const_pointer      const_pointer;
    typedef typename ht::pointer            pointer;
    typedef typename ht::reference          reference;
    typedef typename ht::const_reference    const_reference;

    typedef typename ht::iterator   iterator;

    hasher hash_funct(){
        return rep.hash_funct();
    }

    key_equal key_eq(){
        return rep.key_eq();
    }

public:
    ///构造函数
    unordered_set(): rep(100, hasher(), key_equal()){}
    explicit unordered_set(size_type n): rep(n, hasher(), key_equal()){}
    unordered_set(size_type n, const hasher& hf): rep(n, hf, key_equal()){}
    unordered_set(size_type n, const hasher& hf, const key_equal& eql): rep(n, hf, eql){}

    template<typename InputIterator>
    unordered_set(InputIterator first, InputIterator last)
    :rep(100, hasher(), key_equal())
    {
        rep.insert_unique(first, last);
    }

    template<typename InputIterator>
    unordered_set(InputIterator first, InputIterator last, size_type n)
            :rep(n, hasher(), key_equal())
    {
        rep.insert_unique(first, last);
    }
    template<typename InputIterator>
    unordered_set(InputIterator first, InputIterator last, size_type n, const hasher& hf)
            :rep(n, hf, key_equal())
    {
        rep.insert_unique(first, last);
    }

    template<typename InputIterator>
    unordered_set(InputIterator first, InputIterator last, size_type n, const hasher& hf, const key_equal& eql)
            :rep(n, hf, eql)
    {
        rep.insert_unique(first, last);
    }

    bool operator==(const unordered_set& others){
        return rep == others.rep;
    }
    bool operator!=(const unordered_set& others){
        return rep != others.rep;
    }

public:
    ///公共调用接口
    size_type size(){
        return rep.size();
    }
    size_type max_size(){
        return rep.max_size();
    }
    bool empty(){
        return rep.empty();
    }

    void swap(unordered_set& hs){
        rep.swap(hs.rep);
    }

    iterator begin(){
        return rep.begin();
    }

    iterator end(){
        return rep.end();
    }

    std::pair<iterator, bool>insert(const value_type& obj){
        auto p = rep.insert_unique(obj);
        return std::pair<iterator, bool>(p.first, p.second);
    }

    template <typename InputIterator>
    void insert(InputIterator first, InputIterator last){
        rep.insert_unique(first, last);
    }

    iterator find(const key_type& key){
        return rep.find(key);
    }

    size_type count(const key_type& key){
        return rep.count(key);
    }

    size_type erase(const key_type& key){
        return rep.erase(key);
    }
    void erase(iterator iter){
        return rep.erase(iter);
    }
    void erase(iterator l, iterator r){
        return rep.erase(l, r);
    }

    void clear(){
        return rep.clear();
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

private:
    ht rep;

};


#endif //MYTINYSTL_UNORDERED_SET_H
