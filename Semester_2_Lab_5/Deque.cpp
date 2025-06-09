#include "Deque.h"
#include <stdexcept>

template<typename T>
Deque<T>::Deque() {
    block_count = INIT_BLOCKS;
    blocks = new T*[block_count];
    for (int i = 0; i < block_count; i++)
        blocks[i] = new T[BLOCK_SIZE];
    start_block = block_count / 2;
    start_index = 0;
    total_size = 0;
}

template<typename T>
Deque<T>::~Deque() {
    for (int i = 0; i < block_count; ++i)
        delete[] blocks[i];
    delete[] blocks;
}

template<typename T>
void Deque<T>::ensure_left_space() {
    if (start_block == 0) {
        int new_block_count = block_count * 2;
        T** new_blocks = new T*[new_block_count];
        int offset = block_count;
        for (int i = 0; i < block_count; ++i)
            new_blocks[i + offset] = blocks[i];
        for (int i = 0; i < offset; ++i)
            new_blocks[i] = new T[BLOCK_SIZE];
        delete[] blocks;
        blocks = new_blocks;
        start_block += offset;
        block_count = new_block_count;
    }
}

template<typename T>
void Deque<T>::ensure_right_space() {
    int end_index = start_index + total_size;
    int block = start_block + end_index / BLOCK_SIZE;
    if (block >= block_count) {
        int new_block_count = block_count * 2;
        T** new_blocks = new T*[new_block_count];
        for (int i = 0; i < block_count; ++i)
            new_blocks[i] = blocks[i];
        for (int i = block_count; i < new_block_count; ++i)
            new_blocks[i] = new T[BLOCK_SIZE];
        delete[] blocks;
        blocks = new_blocks;
        block_count = new_block_count;
    }
}

template<typename T>
void Deque<T>::push_back(T val) {
    ensure_right_space();
    int index = start_index + total_size;
    int block = start_block + index / BLOCK_SIZE;
    int offset = index % BLOCK_SIZE;
    blocks[block][offset] = val;
    total_size++;
}

template<typename T>
void Deque<T>::push_front(T val) {
    if (start_index == 0) {
        ensure_left_space();
        start_block--;
        start_index = BLOCK_SIZE;
    }
    start_index--;
    blocks[start_block][start_index] = val;
    total_size++;
}

template<typename T>
void Deque<T>::pop_back() {
    if (total_size > 0)
        total_size--;
}

template<typename T>
void Deque<T>::pop_front() {
    if (total_size > 0) {
        start_index++;
        if (start_index == BLOCK_SIZE) {
            start_index = 0;
            start_block++;
        }
        total_size--;
    }
}

template<typename T>
T& Deque<T>::operator[](int i) {
    int index = start_index + i;
    int block = start_block + index / BLOCK_SIZE;
    int offset = index % BLOCK_SIZE;
    return blocks[block][offset];
}

template<typename T>
T& Deque<T>::front() {
    return (*this)[0];
}

template<typename T>
T& Deque<T>::back() {
    return (*this)[total_size - 1];
}

template<typename T>
int Deque<T>::size() const {
    return total_size;
}

template<typename T>
bool Deque<T>::empty() const {
    return total_size == 0;
}

template<typename T>
void Deque<T>::rotate_left(int k) {
    if (total_size == 0 || k % total_size == 0) return;
    k %= total_size;
    for (int i = 0; i < k; ++i) {
        T temp = front();
        pop_front();
        push_back(temp);
    }
}

template<typename T>
void Deque<T>::rotate_right(int k) {
    if (total_size == 0 || k % total_size == 0) return;
    k %= total_size;
    for (int i = 0; i < k; ++i) {
        T temp = back();
        pop_back();
        push_front(temp);
    }
}

template<typename T>
bool Deque<T>::is_sorted(bool ascending) {
    if (total_size <= 1) return true;
    for (int i = 0; i < total_size - 1; ++i) {
        if (ascending) {
            if ((*this)[i] > (*this)[i + 1]) return false;
        } else {
            if ((*this)[i] < (*this)[i + 1]) return false;
        }
    }
    return true;
}

template class Deque<int>;