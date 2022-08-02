//
// Created by Lenovo on 2022/8/2.
//

#include <cstddef>

#ifndef MYTINYSTL_SHARED_PTR_H
#define MYTINYSTL_SHARED_PTR_H

template<typename T>
class shared_ptr{
public:
    explicit shared_ptr(T* value): _value(value), _count(new size_t(1)){}
    shared_ptr(shared_ptr<T>& others):_value(others._value),_count(&(++*(others._count))){};
    shared_ptr& operator=(shared_ptr& others){
        if(this->_value and --*this->_count == 0)
        {
            delete _value;
            delete _count;
        }
        _count = &++(*others._count);
        _value = others._value;
        return this;
    }

    ~shared_ptr(){
        if(--*(this->_count) == 0)
        {
            delete _count;
            delete _value;
        }
    }

    T& operator*()
    {
        return *_value;
    }
    T* operator->()
    {
        return _value;
    }

    size_t getRef(){
        return *_count;
    }


private:
    size_t*  _count;
    T*      _value;
};



#endif //MYTINYSTL_SHARED_PTR_H
