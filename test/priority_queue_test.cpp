//
// Created by Lenovo on 2022/8/3.
//


#include "../adapter/priority_queue.h"
#include "../container/vector.h"
#include "iostream"

int main()
{
    vector<int>ia{0,1,2,3,4,5,6,12,34};
    priority_queue<int>test;
    for(int i : ia){
        test.push(i);
    }
    std::cout << "size = " << test.size() << std::endl;

    while(!test.empty())
    {
        std::cout << test.top() << " ";
        test.pop();
    }
    std::cout << std::endl;
}