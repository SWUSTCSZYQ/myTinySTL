//
// Created by Lenovo on 2022/8/3.
//

#include <cstddef>

#ifndef MYTINYSTL_HASH_FUN_H
#define MYTINYSTL_HASH_FUN_H

template<typename Key>
struct hash{};

inline size_t hash_string(const char* s){
    unsigned long h = 0;
    for(; *s; ++s)
        h = 5*h + *s;
    return size_t(h);
}

template<>
struct hash<char*>{
    size_t operator()(const char* s){
        return hash_string(s);
    }
};

template<>
struct hash<const char*>{
    size_t operator()(const char* s){
        return hash_string(s);
    }
};

template<>
struct hash<char>{
    size_t operator()(char x){
        return x;
    }
};

template<>
struct hash<unsigned char>{
    size_t operator()(unsigned char x){
        return x;
    }
};

template<>
struct hash<signed char>{
    size_t operator()(signed char x){
        return x;
    }
};

template<>
struct hash<short>{
    size_t operator()(short x){
        return x;
    }
};

template<>
struct hash<unsigned short>{
    size_t operator()(unsigned short x){
        return x;
    }
};

template<>
struct hash<int>{
    size_t operator()(int x){
        return x;
    }
};

template<>
struct hash<unsigned int>{
    size_t operator()(unsigned int x){
        return x;
    }
};

template<>
struct hash<long>{
    size_t operator()(long x){
        return x;
    }
};

template<>
struct hash<unsigned long>{
    size_t operator()(unsigned long x){
        return x;
    }
};

#endif //MYTINYSTL_HASH_FUN_H
