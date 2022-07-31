//
// Created by Lenovo on 2022/7/31.
//

#include <iostream>
#include <algorithm>
#include "../container/vector.h"

void test_push(vector<int>&arr)
{
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    for(int i = 0; i < arr.size(); ++i)std::cout << arr[i] << std::endl;
    if(arr[0] == 1 and arr[1] == 2 and arr[2] == 3)
        std::cout << "push success" << std::endl;
}

void test_pop_and_size(vector<int>&arr)
{
    arr.pop_back();
    if(arr.size() == 2)
        std::cout << "pop and size success" << std::endl;
}

void test_insert(vector<int>&arr)
{
    arr.insert(arr.end(), 5, 2);
    if(arr.size() == 7)
        std::cout << "insert success1" << std::endl;
    arr.insert(arr.begin(), 1, 3);
    if(arr.size() == 8)
        std::cout << "insert success2" << std::endl;
}

void test_erase(vector<int>&arr)
{
    arr.erase(arr.begin());
    if(arr[0] == 1 and arr.size() == 7)
        std::cout << "erase success" << std::endl;
}

void test_front_and_back(vector<int>&arr)
{
    if(arr.front() == 1 and arr.back() == 2)
        std::cout << "front_and_back success" << std::endl;
}

void test_clear_and_empty(vector<int>&arr)
{
    arr.clear();
    if(arr.empty())
        std::cout << "clear_and_empty success" << std::endl;
}

void test_initialize(vector<int>&arr)
{
    vector<int>arr1{1, 2, 3, 1};
    if(arr1.size() != 4)
        exit(0);
    vector<int>arr2(arr1);
    if(arr1.size() != arr2.size())
        exit(0);

    std::cout << "initialize method success" << std::endl;

}

int main()
{
    vector<int>arr;
    test_push(arr);
    test_pop_and_size(arr);
    test_insert(arr);
    test_erase(arr);
    test_front_and_back(arr);
    test_clear_and_empty(arr);
    test_initialize(arr);
}

