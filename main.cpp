#include <iostream>

#include "allocator/default_alloc.h"

int main() {
    char test = 'a';
    char *a = (char*)default_alloc::allocate(sizeof test);
    *a = 'b';
    std::cout << *a << std::endl;
    return 0;
}
