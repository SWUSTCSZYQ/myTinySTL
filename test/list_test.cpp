//
// Created by Lenovo on 2022/8/1.
//

#include <iostream>
#include <algorithm>
#include "../container/list.h"

void test_push(list<int>&arr)
{
    int sz = arr.size();
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    if(arr.size() == sz + 3)
        std::cout << "push success" << std::endl;
}

void test_pop_and_size(list<int>&arr)
{
    int sz = arr.size();
    arr.pop_back();
    if(arr.size() == sz - 1)
        std::cout << "pop and size success" << std::endl;
}

void test_insert(list<int>&arr)
{
    int sz = arr.size();
    arr.insert(arr.end(), 5);
    if(arr.size() == sz + 1)
        std::cout << "insert success1" << std::endl;
    arr.insert(arr.begin(), 1);
    if(arr.size() == sz)
        std::cout << "insert success2" << std::endl;
}

void test_erase(list<int>&arr)
{
    int sz = arr.size();
    arr.erase(arr.begin());
    if(arr.size() == sz)
        std::cout << "erase success" << std::endl;
}

void test_front_and_back(list<int>&arr)
{
    std::cout << "front is" << arr.front() << "  back is " << arr.back() << std::endl;
    std::cout << "front_and_back success" << std::endl;
}

void test_clear_and_empty(list<int>&arr)
{
    arr.clear();
    if(arr.empty())
        std::cout << "clear_and_empty success" << std::endl;
}

void test_initialize(list<int>&arr)
{
    list<int>arr1{1, 2, 3, 1};
    if(arr1.size() != 4)
        exit(0);
    list<int>arr2(arr1);
    if(arr1.size() != arr2.size())
        exit(0);

    std::cout << "initialize method success" << std::endl;

}

int main()
{
    list<int>test(3, 1);
    test.insert(test.begin(), 0);
    test.push_front(-1);
    test.push_back(-2);
    std::cout << test.back() << std::endl;
    test_push(test);
    test_pop_and_size(test);
    test_insert(test);
    test_erase(test);
    test_front_and_back(test);
    test_clear_and_empty(test);
    test_initialize(test);
}