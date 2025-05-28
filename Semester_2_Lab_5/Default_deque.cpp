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


    void push_back(T val) {
        int index = start_index + total_size;
        int block = start_block + index / BLOCK_SIZE;
        int offset = index % BLOCK_SIZE;
        blocks[block][offset] = val;
        total_size++;
    }

    void push_front(T val) {
        if (start_index == 0) {
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

    // Вставим несколько элементов в конец
    dq.push_back(10);
    dq.push_back(20);
    dq.push_back(30);

    // Вставим несколько элементов в начало
    dq.push_front(5);
    dq.push_front(2);

    // Теперь очередь выглядит как: 2, 5, 10, 20, 30
    cout << "Deque contents after push operations:\n";
    for (int i = 0; i < dq.size(); ++i)
        cout << dq[i] << " ";  // должно вывести: 2 5 10 20 30
    cout << "\n";

    // Удалим элемент с конца
    dq.pop_back(); // удалит 30
    // Удалим элемент с начала
    dq.pop_front(); // удалит 2

    // Теперь очередь: 5, 10, 20
    cout << "Deque contents after pop operations:\n";
    for (int i = 0; i < dq.size(); ++i)
        cout << dq[i] << " ";  // должно вывести: 5 10 20
    cout << "\n";

    // Проверим размер
    cout << "Current size: " << dq.size() << "\n";

    // Получим доступ к конкретным элементам
    cout << "First element (dq[0]): " << dq[0] << "\n";
    cout << "Last element (dq[dq.size()-1]): " << dq[dq.size() - 1] << "\n";

    return 0;
}

