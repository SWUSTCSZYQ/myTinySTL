//
// Created by Lenovo on 2022/8/2.
//

#ifndef MYTINYSTL_UNIQUE_PTR_H
#define MYTINYSTL_UNIQUE_PTR_H

template<typename T>
class unique_ptr
{
public:
    explicit unique_ptr(T* ptr):_ptr(ptr){}
    ~unique_ptr(){delete _ptr;}
    T& operator*(){return *_ptr;}
    T operator->(){return &(this->operator*());}
    T* release()
    {
        T* tmp = _ptr;
        _ptr = nullptr;
        return tmp;
    }
    void reset(T* ptr = nullptr)
    {
        T* temp = _ptr;
        _ptr = ptr;
        if(temp)delete temp;
    }
private:
    unique_ptr(const unique_ptr<T>&)=delete;
    unique_ptr<T>& operator=(const unique_ptr<T>&) = delete;
    T* _ptr;
}


#endif //MYTINYSTL_UNIQUE_PTR_H
