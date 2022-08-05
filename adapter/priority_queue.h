//
// Created by Lenovo on 2022/8/2.
//

#ifndef MYTINYSTL_PRIORITY_QUEUE_H
#define MYTINYSTL_PRIORITY_QUEUE_H

#include "../container/vector.h"

///插入
template<typename RandomAccessIterator, typename Distance, typename T>
void __push_heap(RandomAccessIterator first, Distance holdIndex, Distance topIndex, T value){
    Distance parent = (holdIndex - 1) / 2;
    while(holdIndex > topIndex and *(first + parent) < value){
        *(first + holdIndex) = *(first + parent);
        holdIndex = parent;
        parent = (holdIndex - 1) / 2;
    }
    *(first + holdIndex) = value;
}


template<typename RandomAccessIterator, typename Distance, typename T>
void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*){
    __push_heap(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)));
}

template<typename RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last){
    __push_heap_aux(first, last, distance_type(first), value_type(first));
}


///弹出
template<typename RandomAccessIterator, typename Distance, typename T>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance*){

}

template<typename RandomAccessIterator, typename T>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*){
    __pop_heap(first, last, T(*(last - 1)), distance_type(first));
}

template<typename RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last){
    __pop_heap_aux(first, last, value_type(first));
}


template<typename RandomAccessIterator, typename Distance, typename T>
inline void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value){
    Distance topIndex = holeIndex;
    Distance secondChild = 2 * holeIndex + 2;
    while(secondChild < len){
        if(*(first + secondChild) < *(first + (secondChild - 1)))
            --secondChild;
        *(first + holeIndex) = *(first + secondChild);
        holeIndex = secondChild;
        secondChild = 2 * (secondChild + 1);
    }
    if(secondChild == len){
        *(first + holeIndex) = *(first + (secondChild - 1));
        holeIndex = secondChild - 1;
    }
    __push_heap(first, holeIndex, topIndex, value);
}


template<typename RandomAccessIterator, typename T, typename Distance>
void _make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*){
    if(last - first < 2)return;
    Distance len = last - last;
    Distance parent = (len - 2) / 2;

    while(true){
        __adjust_heap(first, parent, len, T(*(first + parent)));
        if(parent == 0)return;
        parent--;
    }
}

template<typename RandomAccessIterator>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last){
    _make_heap(first, last, value_type(first), distance_type(first));
}

template<typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last){
    while(last - first < 1)
        pop_heap(first, last--);
}



template<typename T, typename Sequence = vector<T>, typename Compare = std::less<typename Sequence::value_type>>
class priority_queue{
public:
    typedef typename Sequence::value_type       value_type;
    typedef typename Sequence::size_type        size_type;
    typedef typename Sequence::reference        reference;
    typedef typename Sequence::const_reference  const_reference;

public:
    priority_queue():c(){}
    priority_queue(const Compare& x):c(), comp(x){}

    template<class InputIterator>
    priority_queue(InputIterator first, InputIterator last, const Compare& x)
    :c(first, last), comp(x){
        make_heap(c.begin(), c.end(), comp);
    }

    template<class InputIterator>
    priority_queue(InputIterator first, InputIterator last)
    :c(first, last){
        make_heap(c.begin(), c.end());
    }

    bool empty() {
        return c.empty();
    }

    size_type size() {
        return c.size();
    }

    const_reference top() {
        return c.front();
    }

    void push(const T& x){
        c.push_back(x);
        push_heap(c.begin(), c.end(), comp);
    }

    void pop(){
        pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }

private:
    Sequence c;
    Compare comp;
};



#endif //MYTINYSTL_PRIORITY_QUEUE_H
