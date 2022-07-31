#include <iostream>

#include "allocator/default_alloc.h"
#include "./iterator/type_traits.h"
#include "./iterator/iterator.h"
#include "./container/vector.h"

template<typename T>
void test(T a, __true_type){
    std::cout << "a is true_type" << std::endl;
}

template<typename T>
void test(T a, __false_type){
    std::cout << "a is false_type" << std::endl;
}

int main() {
    struct a{};
    a b;
    __type_traits<a>::is_POD_type isPOD;
    test(b, isPOD);
    vector<int>arr;
    arr.push_back(1);
    std::cout << arr[0] << std::endl;
    arr.push_back(2);
    arr.push_back(3);
    for(int i = 0; i < arr.size(); ++i)std::cout << arr[i] << std::endl;
    return 0;
}
