//
// Created by Lenovo on 2022/8/7.
//

#ifndef MYTINYSTL_RBTREE_H
#define MYTINYSTL_RBTREE_H

#include "../iterator/iterator.h"
#include  "../allocator/allocator.h"
#include "../allocator/construct.h"

///红黑树节点定义
typedef bool rb_tree_color_type;
const rb_tree_color_type rb_tree_red = false;
const rb_tree_color_type rb_tree_black = true;

struct rb_tree_node_base{
    typedef rb_tree_color_type color_type;
    typedef rb_tree_node_base* base_ptr;

    color_type color;
    ///父节点以及左右孩子节点
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    static base_ptr minimum(base_ptr x){
        while(x->left)x = x->left;
        return x;
    }

    static base_ptr maximum(base_ptr x){
        while(x->right)x = x->right;
        return x;
    }

    ///构造函数, 默认是黑色节点
    rb_tree_node_base():parent(nullptr), left(nullptr), right(nullptr), color(rb_tree_black){}
    explicit rb_tree_node_base(base_ptr par):parent(par), left(nullptr), right(nullptr), color(rb_tree_black){}
    rb_tree_node_base(base_ptr par, color_type col):parent(par), left(nullptr), right(nullptr), color(col){}
    rb_tree_node_base(base_ptr par, base_ptr l, base_ptr r):parent(par), left(l), right(r), color(rb_tree_black){}
    rb_tree_node_base(base_ptr par, base_ptr l, base_ptr r,color_type col):parent(par), left(l), right(r), color(col){}

};

template <typename Value>
struct rb_tree_node: public rb_tree_node_base{
    typedef rb_tree_node<Value>* link_type;
    Value value_field;

    rb_tree_node():value_field(Value()), rb_tree_node_base(){}

};


///红黑树迭代器类型

struct rb_tree_base_iterator{
    ///基层迭代器
    typedef rb_tree_node_base::base_ptr base_ptr;
    typedef bidirectional_iterator_tag  iterator_category;
    typedef ptrdiff_t                   difference_type;

    base_ptr node;

    ///中序遍历 找下一个比他大的
    void increment()
    {
        if(node->right){
            node = node->right;
            while(node->left)
                node = node->left;
        }
        else{
            ///如果无有孩子节点，就向父节点回溯，知道该节点不是父节点的右孩子节点
            base_ptr par = node->parent;
            while(node == par->right){
                node = par->right;
                par = par->parent;
            }
            if(node->right != par)
                node = par;
        }
    }

    void decrement(){
        ///此节点为header
        if(node->color == rb_tree_red and node->parent->parent == node){
            node = node->right;
        }
        else if(node->left){
            ///有左孩子节点
            base_ptr l = node->left;
            while(l->right)
                l = l->right;
            node = l;
        }
        else{
            ///既非根节点，又没有左孩子节点就回溯
            base_ptr p = node->parent;
            while(node == p->left){
                node = p;
                p = p->parent;
            }
            node = p;
        }
    }
};


///红黑树实际的迭代器
template <typename Value, typename Ref, typename Ptr>
struct rb_tree_iterator:public rb_tree_base_iterator{
    typedef Value                                               value_type;
    typedef Ref                                                 reference;
    typedef Ptr                                                 pointer;
    typedef rb_tree_iterator<Value, Value&, Value*>             iterator;
    typedef rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
    typedef rb_tree_iterator<Value, Ref,Ptr>                    self;
    typedef rb_tree_node<Value>*                                link_type;

    rb_tree_iterator() = default;
    explicit rb_tree_iterator(link_type x) : rb_tree_base_iterator(){
        node = x;
    }
    explicit rb_tree_iterator(const iterator& iter) : rb_tree_base_iterator(){
        node = iter.node;
    }

    reference operator*(){
        return link_type (node)->value_field;
    }

    pointer operator->(){
        return &(operator*());
    }

    self& operator++(){
        increment();
        return *this;
    }

    self& operator++(int){
        self* tmp = *this;
        increment();
        return tmp;
    }

    self& operator--(){
        decrement();
        return *this;
    }

    self& operator--(int){
        self* tmp = *this;
        decrement();
        return tmp;
    }

};

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Alloc = allocator<Key>>
class rb_tree{
private:
    typedef void*                   void_pointer;
    typedef rb_tree_node_base*      base_ptr;
    typedef rb_tree_node<Value>     rb_tree_node;
    typedef allocator<rb_tree_node> rb_tree_node_allocator;
    typedef rb_tree_color_type      color_type;

public:
    typedef Key                 key_type;
    typedef Value               value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef rb_tree_node*       link_type;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;

    typedef rb_tree_iterator<value_type, reference, pointer>    iterator;

public:
    ///构造函数和析构函数
    rb_tree(const Compare& comp = Compare())
    :node_count(0), key_compare(comp){
        init();
    }

    ~rb_tree(){
        clear();
        put_node(header);
    }

    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);
public:
    ///实际调用接口
    void clear();
    Compare key_comp() {
        return key_compare;
    }

    iterator begin(){
        return leftMost();
    }

    iterator end(){
        return header;
    }

    bool empty(){
        return node_count == 0;
    }

    size_type size(){
        return node_count;
    }

    size_type max_size(){
        return size_type(-1);
    }

private:
    ///接口的实际实现函数
    iterator __insert(base_ptr x, base_ptr y, const value_type& v);
    link_type __copy(link_type x, link_type p);
    void __erase(link_type x);
    void init(){
        header = get_node();

        color(header) = rb_tree_red;
        leftMost() = header;
        rightMost() = header;
    }

private:
    link_type get_node(){
        return rb_tree_node_allocator ::allocate();
    }

    void put_node(link_type p){
        rb_tree_node_allocator ::deallocate(p);
    }

    link_type create_node(const value_type& x){
        link_type tmp = get_node();
        construct(&tmp->value_field, x);
        return tmp;
    }

    link_type clone_node(link_type x){
        ///复制一个节点
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = nullptr;
        tmp->right = nullptr;
        return tmp;
    }

    void destroy_node(link_type p){
        destroy(&p->value_field);
        put_node(p);
    }


private:
    link_type & root(){
        return (link_type&) header->parent;
    }

    link_type & leftMost(){
        return (link_type&) header->left;
    }
    link_type & rightMost(){
        return (link_type&) header->right;
    }

private:
    ///获取节点x的成员
    static link_type & left(link_type x){
        return (link_type&)(x->left);
    }
    static link_type & right(link_type x){
        return (link_type&)(x->right);
    }
    static link_type & parent(link_type x){
        return (link_type&)(x->parent);
    }
    static reference value(link_type x){
        return x->value_field;
    }
    static const Key& key(link_type x){
        return KeyOfValue()(value(x));
    }
    static color_type& color(link_type x){
        return (color_type &)(x->color);
    }

    static link_type minimum(link_type x){
        return (link_type) rb_tree_node_base::minimum(x);
    }
    static link_type maximum(link_type x){
        return (link_type) rb_tree_node_base::maximum(x);
    }
private:
    size_type   node_count; ///记录树的节点数量
    link_type   header;
    Compare     key_compare;///键值比较
};

#endif //MYTINYSTL_RBTREE_H
