#include <iostream>
using namespace std;

template<typename T>
class Deque {
    static const int BLOCK_SIZE = 4;
    static const int INIT_BLOCKS = 8;

    T** blocks;
    int start_block;
    int start_index;
    int total_size;
    int block_count;

public:
    Deque() {
        block_count = INIT_BLOCKS;
        blocks = new T*[block_count];
        for (int i = 0; i < block_count; i++) {
            blocks[i] = new T[BLOCK_SIZE];
        }
        start_block = block_count / 2;
        start_index = 0;
        total_size = 0;
    }

    ~Deque() {
        for (int i = 0; i < block_count; ++i)
            delete[] blocks[i];
        delete[] blocks;
    }

private:
    void ensure_left_space() {
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

    void ensure_right_space() {
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

public:
    void push_back(T val) {
        ensure_right_space();
        int index = start_index + total_size;
        int block = start_block + index / BLOCK_SIZE;
        int offset = index % BLOCK_SIZE;
        blocks[block][offset] = val;
        total_size++;
    }

    void push_front(T val) {
        if (start_index == 0) {
            ensure_left_space();
            start_block--;
            start_index = BLOCK_SIZE;
        }
        start_index--;
        blocks[start_block][start_index] = val;
        total_size++;
    }

    void pop_back() {
        if (total_size > 0)
            total_size--;
    }

    void pop_front() {
        if (total_size > 0) {
            start_index++;
            if (start_index == BLOCK_SIZE) {
                start_index = 0;
                start_block++;
            }
            total_size--;
        }
    }

    T& operator[](int i) {
        int index = start_index + i;
        int block = start_block + index / BLOCK_SIZE;
        int offset = index % BLOCK_SIZE;
        return blocks[block][offset];
    }

    int size() const {
        return total_size;
    }
};


int main() {
    Deque<int> dq;

    for (int i = 0; i < 10; i++)
        dq.push_back(i * 10);

    for (int i = 0; i < 5; i++)
        dq.push_front(-i);

    cout << "Contents of deque:\n";
    for (int i = 0; i < dq.size(); ++i)
        cout << dq[i] << " ";
    cout << "\n";

    dq.pop_back();
    dq.pop_front();

    cout << "After pops:\n";
    for (int i = 0; i < dq.size(); ++i)
        cout << dq[i] << " ";
    cout << "\n";

    cout << "Size: " << dq.size() << "\n";
}
