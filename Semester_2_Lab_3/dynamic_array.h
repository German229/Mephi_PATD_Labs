#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdexcept>
#include <iostream>

template<typename T>
class DynamicArray {
private:
    T* data;
    int size;
public:
    DynamicArray(T* items, int count);
    DynamicArray(int size);
    DynamicArray(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(const DynamicArray<T>& other);
    ~DynamicArray();

    T Get(int index) const;
    int GetSize() const;
    void Set(int index, T value);
    void Resize(int newSize);
    void Print() const;
    T& operator[](int index);
};


template<typename T>
DynamicArray<T>::DynamicArray(T* items, int count) : size(count) {
    data = new T[count];
    for (int i = 0; i < count; i++) {
        data[i] = items[i];
    }
}

template<typename T>
DynamicArray<T>::DynamicArray(int size) : size(size) {
    data = new T[size];
}

template<typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other) : size(other.size) {
    data = new T[size];
    for (int i = 0; i < size; i++) {
        data[i] = other.data[i];
    }
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this == &other) return *this;
    delete[] data;
    size = other.size;
    data = new T[size];
    for (int i = 0; i < size; i++) {
        data[i] = other.data[i];
    }
    return *this;
}

template<typename T>
DynamicArray<T>::~DynamicArray() {
    delete[] data;
}

template<typename T>
T DynamicArray<T>::Get(int index) const {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index out of range");
    return data[index];
}

template<typename T>
int DynamicArray<T>::GetSize() const {
    return size;
}

template<typename T>
void DynamicArray<T>::Set(int index, T value) {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index out of range");
    data[index] = value;
}

template<typename T>
void DynamicArray<T>::Resize(int newSize) {
    if (newSize < 0)
        throw std::invalid_argument("new size cannot be negative");

    T* newData = new T[newSize];
    int elementsToCopy = (newSize < size) ? newSize : size;

    for (int i = 0; i < elementsToCopy; i++) {
        newData[i] = data[i];
    }

    delete[] data;
    data = newData;
    size = newSize;
}

template<typename T>
void DynamicArray<T>::Print() const {
    for (int i = 0; i < size; i++) {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;
}




#endif