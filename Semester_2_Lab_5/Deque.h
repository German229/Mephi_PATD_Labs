#pragma once

#include <iostream>

template<typename T>
class Deque {
    static const int BLOCK_SIZE = 4;
    static const int INIT_BLOCKS = 8;

    T** blocks;
    int start_block;
    int start_index;
    int total_size;
    int block_count;

    void ensure_left_space();
    void ensure_right_space();

public:
    Deque();
    ~Deque();

    void push_back(T val);
    void push_front(T val);
    void pop_back();
    void pop_front();

    T& operator[](int i);
    T& front();
    T& back();

    int size() const;
    bool empty() const;

    void rotate_left(int k);
    void rotate_right(int k);
    bool is_sorted(bool ascending = true);
};
