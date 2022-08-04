//
// Created by Lenovo on 2022/8/3.
//

#include "../container/hashtable.h"
#include "../functor/hash_fun.h"
#include "../allocator/allocator.h"
#include <algorithm>

template <typename T>
struct identity
{
    T operator()(T x) const { return x; }
};

int main()
{
    hashtable<int, int, hash<int>, identity<int>, std::equal_to<int>, allocator<int>>iht(50, hash<int>(), std::equal_to<int>());
    std::cout << iht.max_bucket_count() << std::endl;
    iht.insert_unique(19);
    iht.insert_unique(25);
    iht.insert_unique(63);
    iht.insert_unique(108);
    iht.insert_unique(512);
    iht.insert_unique(172);
    std::cout << iht.size() << std::endl;

    std::cout << "遍历所有值" << std::endl;
    auto iter = iht.begin();
    for(int i = 0; i < iht.size(); ++i, ++iter){
        std::cout << *iter << std::endl;
    }

    std::cout << "统计每个桶中的元素个数" << std::endl;
    for(int i = 0; i < iht.bucket_count(); ++i){
        if(iht.elems_in_bucket(i) > 0){
            std::cout << "bucket[" << i << "] has " << iht.elems_in_bucket(i) << " elems." << std:: endl;
        }
    }
}