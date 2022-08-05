//
// Created by Lenovo on 2022/8/3.
//

#ifndef MYTINYSTL_HASHTABLE_H
#define MYTINYSTL_HASHTABLE_H

#include "../allocator/allocator.h"
#include "../iterator/iterator.h"
#include "../container/vector.h"
#include <iostream>

template<typename Value, typename Key, typename HashFunc,
        typename ExtractKey, typename EqualKey, typename Alloc = allocator<Value>>
class hashtable;

///hash节点
template<typename Value>
struct hashtable_node{
    hashtable_node* next;
    Value           val;

    hashtable_node() = default;
    explicit hashtable_node(const Value& n):next(nullptr), val(n){}
    hashtable_node(const hashtable_node& node):next(node.next), val(node.val){}
    hashtable_node(const hashtable_node&& node) noexcept :next(node.next), val(std::move(node.val)){
        node.next = nullptr;
    }
};


///哈希表迭代器
template <typename Value, typename Key, typename HashFunc,
        typename ExtractKey, typename EqualKey, typename Alloc >
struct hashtable_iterator: public iterator<forward_iterator_tag, Value>{
    typedef hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>            hashtable;
    typedef hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>   iterator;
    typedef hashtable_node<Value>                                                   node;

    typedef forward_iterator_tag    iterator_category;
    typedef Value                   value_type;
    typedef ptrdiff_t               difference_type;
    typedef size_t                  size_type;
    typedef Value*                  pointer;
    typedef Value&                  reference;
    typedef const Value*            const_pointer;
    typedef const Value&            const_reference;

    node* cur;          ///迭代器目前所指向的节点
    hashtable* ht;      ///保持对迭代器的连接

    hashtable_iterator() = default;
    hashtable_iterator(node* n, hashtable* table)
    :cur(n), ht(table){}
    hashtable_iterator(const iterator& others)
    :cur(others.cur), ht(others.ht)
    {}

    iterator& operator=(const iterator& others){
        if(this != &others){
            cur = others.cur;
            ht = others.ht;
        }
        return *this;
    }



    reference operator*() const{
        return cur->val;
    };

    pointer operator->() const{
        return &(operator*());
    }

    iterator operator++();
    iterator operator++(int);
    bool operator==(const iterator& other);
    bool operator!=(const iterator& other);

};

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator++() {
    const node* old = cur;
    cur = cur->next;
    if(!cur){
        size_type bucket = ht->btk_num(old->val);
        while(!cur and ++bucket < ht->buckets.size()){
            cur = ht->buckets[bucket];
        }
        return *this;
    }
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
bool hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator==(
        const hashtable_iterator::iterator &other) {
    return cur == other.cur;
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
bool hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::operator!=(
        const hashtable_iterator::iterator &other) {
    return cur != other.cur;
}

static const int stl_num_primes = 28;
static const unsigned long long stl_prime_list[stl_num_primes] = {
        53,           97,          193,        389,         769,
        1543,         3079,        6151,       12289,       24593,
        49157,      98317,      1996613,    393241,     786433,
        1572869,    3145739,    6291467,    12582917,   25165843,
        50331653,   100663319,  201326611,  402653189,  805306457,
        1610612741, 32212254731,42949672711
};

///找到最接近并大于n的质数
inline unsigned long long stl_next_prime(unsigned long long n){
    const unsigned long long* first = stl_prime_list;
    const unsigned long long* last = stl_prime_list + stl_num_primes;
    const unsigned long long* pos = std::lower_bound(first, last, n);
    return pos == last ? (*last - 1) : *pos;
}



///哈希表实现
template<typename Value, typename Key, typename HashFunc,
        typename ExtractKey, typename EqualKey, typename Alloc>
class hashtable{
    /*
         * Value:节点的实际值类型
         * Key:节点的键值类型
         * HashFunc:哈希函数
         * ExtractKey:从节点中提取键值的方法
         * EqualKey:判断键值是否相同
         * Alloc:空间配置
         * */
public:
    typedef HashFunc    hasher;
    typedef EqualKey    key_equal;

    typedef hashtable_node<Value>   node;
    typedef allocator<node>         node_allocator;

    typedef hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>   iterator;

    typedef Value           value_type;
    typedef Value&          reference;
    typedef const Value&    const_reference;
    typedef Value*          pointer;
    typedef const Value*    const_pointer;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef Key             key_type;
friend hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>;

private:
    hasher      hash;
    key_equal   equals;
    ExtractKey  get_key;

    ///以vector作为底层容器
    vector<node*>   buckets;
    size_type       num_elements;

public:
    ///构造函数
    hashtable(size_type n, const HashFunc& hf, const EqualKey& eql)
    :hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0)
    {
        initialize_buckets(n);
    }

    bool operator==(const hashtable& others){
        return buckets == others.buckets;
    }
    bool operator!=(const hashtable& others){
        return buckets != others.buckets;
    }
public:
    hasher hash_funct(){
        return hash;
    }

    key_equal key_eq(){
        return equals;
    }

    ///调用接口
    size_type bucket_count(){
        return buckets.size();
    }

    std::pair<iterator, bool> insert_unique(const value_type& obj){
        resize(num_elements + 1);
        return insert_unique_no_resize(obj);
    }

    template<typename InputIterator>
    std::pair<iterator, bool> insert_unique(InputIterator first, InputIterator last){

    }

    iterator insert_equal(const value_type& obj){
        resize(num_elements + 1);
        return insert_equal_no_resize(obj);
    }

    void clear();
    void copy_from(const hashtable& ht);

    iterator find(const key_type& key);
    size_type count(const key_type& key);
    size_type size(){
        return num_elements;
    }

    size_type erase(const key_type& key);
    void erase(iterator iter);
    void erase(iterator first, iterator last);

    size_type max_size(){
        return buckets.size();
    }

    unsigned long long max_bucket_count(){
        return stl_prime_list[stl_num_primes - 1];
    }

    iterator begin(){
        return m_begin();
    }

    iterator end(){
        return m_end();
    }

    bool empty(){
        return size() == 0;
    }

    void swap(hashtable& ht){
        buckets.swap(ht.buckets);
    }

    size_type elems_in_bucket(const size_type& obj);

private:
    ///供迭代器使用
    ///判断元素的落脚处
    size_type btk_num(const value_type& obj, size_t n){
        return btk_num_key(get_key(obj), n);
    }
    size_type btk_num(const value_type& obj){
        return btk_num_key(get_key(obj));
    }

    size_type btk_num_key(const key_type& key){
        return btk_num_key(key, buckets.size());
    }
    size_type btk_num_key(const key_type& key, size_t n){
        return hash(key) % n;
    }

    ///判断是否需要扩充
    void resize(size_type num_elements_hint);
private:
    iterator m_end();
    iterator m_begin();
    void initialize_buckets(size_type n);
    size_type next_size(size_type n)const{
        return stl_next_prime(n);
    }

    node* new_node(const value_type& obj);
    void delete_node(node*);
    ///不允许键值重复
    std::pair<iterator, bool> insert_unique_no_resize(const value_type& obj);
    ///允许键值重复
    iterator insert_equal_no_resize(const value_type& obj);

};




template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::node *hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::new_node(const value_type &obj) {
    node* n = node_allocator ::allocate();
    n->next = nullptr;
    construct(&n->val, obj);
    return n;
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::delete_node(hashtable::node *n) {
    destroy(&n->val);
    node_allocator ::deallocate(n);
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::initialize_buckets(hashtable::size_type n) {
    const size_type n_buckets = next_size(n);
    buckets.reserve(n_buckets);
    buckets.insert(buckets.end(), n_buckets, (node*) 0);
    num_elements = 0;
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::resize(hashtable::size_type num_elements_hint) {
    const size_type old_n = buckets.size();
    if(num_elements_hint > old_n){
        ///需要重新配置
        const size_type n = next_size(num_elements_hint);
        if(n > old_n){
            vector<node*>tmp(n, (node*)0);
            for(size_type bucket = 0; bucket < old_n; ++bucket){
                node* first = buckets[bucket];
                while(first){
                    size_type new_bucket = btk_num(first->val, n);
                    ///让旧的指向其所对应的串行的下一个节点
                    ///将当前节点插入到新的buckets中，成为其对应串行的第一个节点
                    ///回到旧的buckets所指的处理串行，准备处理下一个节点
                    buckets[bucket] = first->next;
                    first->next = tmp[new_bucket];
                    tmp[new_bucket] = first;
                    first = buckets[bucket];
                }
            }
            buckets.swap(tmp);
        }
    }
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
std::pair<typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator, bool>
hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::insert_unique_no_resize(const value_type &obj) {
    const size_type n = btk_num(obj);
    node* first = buckets[n];

    ///buckets[n]已被占用，first不为0,
    for(node* cur = first; cur; cur = cur->next)
        if(equals(get_key(cur->val), get_key(obj)))
            return std::pair<iterator, bool>(iterator(cur, this), false);

    ///first指向bucket所指链表的头部节点
    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return std::pair<iterator, bool>(iterator(tmp, this), true);
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator
hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::insert_equal_no_resize(const value_type &obj) {
    const size_type n = btk_num(obj);
    node* first = buckets[n];

    for(node* cur = first; cur; cur = cur->first){
        if(equals(get_key(cur->val), get_key(obj))){
            node* tmp = new_node(obj);
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elements;
            return iterator(tmp, this);
        }
    }

    ///没有发现重复的键值
    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return iterator(tmp, this);

}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::clear() {
    for(size_type i = 0; i < buckets.size(); ++i)
    {
        ///删除每一个bucket的每一个节点都删除
        node *cur = buckets[i];
        while(cur != 0){
            node* next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = 0;
    }
    num_elements = 0;
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::copy_from(const hashtable &ht) {
    buckets.clear();
    if(buckets.size() < ht.buckets.size())
        buckets.reserve(ht.buckets.size());
    buckets.insert(buckets.end(), ht.buckets.size(), (node*)0);

    for(size_type i = 0; i < ht.buckets.size(); ++i)
    {
        if(const node* cur = ht.buckets[i]){
            node* copy = new_node(cur->v);
            buckets[i] = copy;

            ///针对bucket list 复制每一个节点
            for(node* next = cur->next; next; cur = next, next = cur->next){
                copy->next = new_node(next->val);
                copy = copy->next;
            }
        }
    }
    num_elements = ht.num_elements;
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::find(const key_type &key) {
    size_type n = btk_num_key(key);
    node* first;
    for(first = buckets[n]; first and !equals(get_key(first->val), key); first = first->next){}
    return iterator(first, this);
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::size_type hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::count(const key_type& key) {
    const size_type n = btk_num_key(key);
    size_type result = 0;
    for(node* cur = buckets[n]; cur; cur = cur->next)
        if(equals(get_key(cur->val), key))
            ++result;
    return result;
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::m_end() {
    return hashtable::iterator(nullptr, this);
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::iterator hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::m_begin() {
    for(size_type n = 0; n < buckets.size(); ++n){
        if(buckets[n]){
            return iterator(buckets[n], this);
        }

    }
    return iterator(nullptr, this);
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::size_type
hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::elems_in_bucket(const size_type &obj) {
    size_type result = 0;
    node* cur = buckets[obj];
    while(cur)
    {
        ++result;
        cur = cur->next;
    }
    return result;
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::size_type hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(const key_type &key) {
    const size_type n = btk_num_key(key);
    node* cur = buckets[n];
    size_type count = 0;
    while(cur and equals(get_key(cur->val), key)){
        ++count;
        cur = cur->next;
    }
    buckets[n] = cur;
    while(cur and cur->next)
    {
        ///此时 !equals(get_key(cur->val), key)
        node* next = cur->next;
        if(equals(get_key(next->val), key))
        {
            cur->next = next->next;
            delete_node(next);
            ++count;
        }
        else{
            cur = cur->next;
        }
    }
    return count;
}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(hashtable::iterator iter) {

}

template<typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey, typename Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>::erase(hashtable::iterator first,
                                                                         hashtable::iterator last) {

}


#endif //MYTINYSTL_HASHTABLE_H
