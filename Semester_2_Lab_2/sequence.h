#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "dynamic_array.h"
#include <stdexcept>
#include <iostream>

template<typename T>
class Sequence {
public:
    virtual T Get(int index) const = 0;
    virtual int GetLength() const = 0;
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;

    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt(int index, T item) = 0;

    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;
    virtual Sequence<T>* Concat(const Sequence<T>& other) const = 0;

    virtual ~Sequence() = default;
};



template<typename T>
class ArraySequence : public Sequence<T> {
private:
    DynamicArray<T>* items;

public:
    ArraySequence(T* data, int count);
    ArraySequence(int size);
    ~ArraySequence();

    T Get(int index) const override;
    int GetLength() const override;
    T GetFirst() const override;
    T GetLast() const override;

    Sequence<T>* Append(T item) override;
    Sequence<T>* Prepend(T item) override;
    Sequence<T>* InsertAt(int index, T item) override;
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    Sequence<T>* Concat(const Sequence<T>& other) const override;
};



template<typename T>
ArraySequence<T>::ArraySequence(T* data, int count) {
    items = new DynamicArray<T>(data, count);
}

template<typename T>
ArraySequence<T>::ArraySequence(int size) {
    items = new DynamicArray<T>(size);
}

template<typename T>
ArraySequence<T>::~ArraySequence() {
    delete items;
}

template<typename T>
T ArraySequence<T>::Get(int index) const {
    return items->Get(index);
}

template<typename T>
int ArraySequence<T>::GetLength() const {
    return items->GetSize();
}

template<typename T>
T ArraySequence<T>::GetFirst() const {
    return Get(0);
}

template<typename T>
T ArraySequence<T>::GetLast() const {
    return Get(GetLength() - 1);
}

template<typename T>
Sequence<T>* ArraySequence<T>::Append(T item) {
    int size = GetLength();
    T* newData = new T[size + 1];
    for (int i = 0; i < size; i++) newData[i] = Get(i);
    newData[size] = item;

    auto* result = new ArraySequence<T>(newData, size + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ArraySequence<T>::Prepend(T item) {
    int size = GetLength();
    T* newData = new T[size + 1];
    newData[0] = item;
    for (int i = 0; i < size; i++) newData[i + 1] = Get(i);

    auto* result = new ArraySequence<T>(newData, size + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ArraySequence<T>::InsertAt(int index, T item) {
    if (index < 0 || index > GetLength()) {
        throw std::out_of_range("Index out of range");
    }
    int size = GetLength();
    T* newData = new T[size + 1];
    for (int i = 0; i < index; i++) newData[i] = Get(i);
    newData[index] = item;
    for (int i = index; i < size; i++) newData[i + 1] = Get(i);

    auto* result = new ArraySequence<T>(newData, size + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ArraySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex) {
        throw std::out_of_range("Index out of range");
    }
    int size = endIndex - startIndex + 1;
    T* newData = new T[size];
    for (int i = 0; i < size; i++) newData[i] = Get(startIndex + i);

    auto* result = new ArraySequence<T>(newData, size);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ArraySequence<T>::Concat(const Sequence<T>& other) const {
    int total = GetLength() + other.GetLength();
    T* newData = new T[total];

    for (int i = 0; i < GetLength(); i++) newData[i] = Get(i);
    for (int i = 0; i < other.GetLength(); i++) newData[GetLength() + i] = other.Get(i);

    auto* result = new ArraySequence<T>(newData, total);
    delete[] newData;
    return result;
}


template<typename T>
class ImmutableArraySequence : public Sequence<T> {
private:
    DynamicArray<T>* items;

public:
    ImmutableArraySequence();
    ImmutableArraySequence(T* data, int count);
    ImmutableArraySequence(const ImmutableArraySequence<T>& other);
    ~ImmutableArraySequence();

    T Get(int index) const override;
    int GetLength() const override;
    T GetFirst() const override;
    T GetLast() const override;

    Sequence<T>* Append(T item) override;
    Sequence<T>* Prepend(T item) override;
    Sequence<T>* InsertAt(int index, T item) override;

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    Sequence<T>* Concat(const Sequence<T>& other) const override;
};

template<typename T>
ImmutableArraySequence<T>::ImmutableArraySequence() {
    items = new DynamicArray<T>(0);
}

template<typename T>
ImmutableArraySequence<T>::ImmutableArraySequence(T* data, int count) {
    items = new DynamicArray<T>(data, count);
}

template<typename T>
ImmutableArraySequence<T>::ImmutableArraySequence(const ImmutableArraySequence<T>& other) {
    items = new DynamicArray<T>(*other.items);
}

template<typename T>
ImmutableArraySequence<T>::~ImmutableArraySequence() {
    delete items;
}

template<typename T>
T ImmutableArraySequence<T>::Get(int index) const {
    return items->Get(index);
}

template<typename T>
int ImmutableArraySequence<T>::GetLength() const {
    return items->GetSize();
}

template<typename T>
T ImmutableArraySequence<T>::GetFirst() const {
    if (items->GetSize() == 0) throw std::out_of_range("Empty sequence");
    return items->Get(0);
}

template<typename T>
T ImmutableArraySequence<T>::GetLast() const {
    if (items->GetSize() == 0) throw std::out_of_range("Empty sequence");
    return items->Get(items->GetSize() - 1);
}

template<typename T>
Sequence<T>* ImmutableArraySequence<T>::Append(T item) {
    int n = items->GetSize();
    T* newData = new T[n + 1];
    for (int i = 0; i < n; i++) newData[i] = items->Get(i);
    newData[n] = item;
    auto* result = new ImmutableArraySequence<T>(newData, n + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ImmutableArraySequence<T>::Prepend(T item) {
    int n = items->GetSize();
    T* newData = new T[n + 1];
    newData[0] = item;
    for (int i = 0; i < n; i++) newData[i + 1] = items->Get(i);
    auto* result = new ImmutableArraySequence<T>(newData, n + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ImmutableArraySequence<T>::InsertAt(int index, T item) {
    int n = items->GetSize();
    if (index < 0 || index > n) throw std::out_of_range("Invalid index");
    T* newData = new T[n + 1];
    for (int i = 0; i < index; i++) newData[i] = items->Get(i);
    newData[index] = item;
    for (int i = index; i < n; i++) newData[i + 1] = items->Get(i);
    auto* result = new ImmutableArraySequence<T>(newData, n + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ImmutableArraySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= items->GetSize() || startIndex > endIndex) {
        throw std::out_of_range("Invalid range");
    }
    int len = endIndex - startIndex + 1;
    T* newData = new T[len];
    for (int i = 0; i < len; i++) newData[i] = items->Get(startIndex + i);
    auto* result = new ImmutableArraySequence<T>(newData, len);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ImmutableArraySequence<T>::Concat(const Sequence<T>& other) const {
    int n = items->GetSize();
    int m = other.GetLength();
    T* newData = new T[n + m];
    for (int i = 0; i < n; i++) newData[i] = items->Get(i);
    for (int i = 0; i < m; i++) newData[n + i] = other.Get(i);
    auto* result = new ImmutableArraySequence<T>(newData, n + m);
    delete[] newData;
    return result;
}


#endif