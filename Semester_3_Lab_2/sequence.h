#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "dynamic_array.h"
#include <stdexcept>

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

    virtual Sequence<T>* CreateFromArray(T* data, int size) const = 0;
    virtual Sequence<T>* Instance() const = 0;
    virtual Sequence<T>* Clone() const = 0;

    virtual ~Sequence() = default;
};

template<typename T>
class ArraySequence : public Sequence<T> {
protected:
    DynamicArray<T>* items;

public:
    ArraySequence(T* data, int count);
    ArraySequence(int size);
    ArraySequence(const ArraySequence<T>& other);
    virtual ~ArraySequence();

    T Get(int index) const override;
    int GetLength() const override;
    T GetFirst() const override;
    T GetLast() const override;

    Sequence<T>* Append(T item) override;
    Sequence<T>* Prepend(T item) override;
    Sequence<T>* InsertAt(int index, T item) override;
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    Sequence<T>* Concat(const Sequence<T>& other) const override;
    void Set(int index, T value) {
        items->Set(index, value);
    }
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
ArraySequence<T>::ArraySequence(const ArraySequence<T>& other) {
    items = new DynamicArray<T>(*other.items);
}

template<typename T>
ArraySequence<T>::~ArraySequence() {
    delete items;
}

template<typename T>
T ArraySequence<T>::Get(int index) const { return items->Get(index); }

template<typename T>
int ArraySequence<T>::GetLength() const { return items->GetSize(); }

template<typename T>
T ArraySequence<T>::GetFirst() const { return Get(0); }

template<typename T>
T ArraySequence<T>::GetLast() const { return Get(GetLength() - 1); }

template<typename T>
Sequence<T>* ArraySequence<T>::Append(T item) {
    int size = GetLength();
    T* newData = new T[size + 1];
    for (int i = 0; i < size; i++) newData[i] = Get(i);
    newData[size] = item;
    auto* result = this->CreateFromArray(newData, size + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ArraySequence<T>::Prepend(T item) {
    int size = GetLength();
    T* newData = new T[size + 1];
    newData[0] = item;
    for (int i = 0; i < size; i++) newData[i + 1] = Get(i);
    auto* result = this->CreateFromArray(newData, size + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ArraySequence<T>::InsertAt(int index, T item) {
    int size = GetLength();
    T* newData = new T[size + 1];
    for (int i = 0; i < index; i++) newData[i] = Get(i);
    newData[index] = item;
    for (int i = index; i < size; i++) newData[i + 1] = Get(i);
    auto* result = this->CreateFromArray(newData, size + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ArraySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    int size = endIndex - startIndex + 1;
    T* newData = new T[size];
    for (int i = 0; i < size; i++) newData[i] = Get(startIndex + i);
    auto* result = this->CreateFromArray(newData, size);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ArraySequence<T>::Concat(const Sequence<T>& other) const {
    int size = GetLength();
    int otherSize = other.GetLength();
    T* newData = new T[size + otherSize];
    for (int i = 0; i < size; i++) newData[i] = Get(i);
    for (int i = 0; i < otherSize; i++) newData[size + i] = other.Get(i);
    auto* result = this->CreateFromArray(newData, size + otherSize);
    delete[] newData;
    return result;
}




template<typename T>
class ImmutableArraySequence : public ArraySequence<T> {
public:
    ImmutableArraySequence() : ArraySequence<T>(0) {}
    ImmutableArraySequence(T* data, int size) : ArraySequence<T>(data, size) {}
    ImmutableArraySequence(const ImmutableArraySequence<T>& other) : ArraySequence<T>(other) {}

    Sequence<T>* CreateFromArray(T* data, int size) const override {
        return new ImmutableArraySequence<T>(data, size);
    }

    Sequence<T>* Instance() const override {
        return new ImmutableArraySequence<T>();
    }

    Sequence<T>* Clone() const override {
        T* data = new T[this->GetLength()];
        for (int i = 0; i < this->GetLength(); i++) data[i] = this->Get(i);
        auto* clone = new ImmutableArraySequence<T>(data, this->GetLength());
        delete[] data;
        return clone;
    }
};

template<typename T>
class MutableArraySequence : public ArraySequence<T> {
public:
    MutableArraySequence() : ArraySequence<T>(0) {}
    MutableArraySequence(T* data, int size) : ArraySequence<T>(data, size) {}
    MutableArraySequence(const MutableArraySequence<T>& other) : ArraySequence<T>(other) {}

    Sequence<T>* CreateFromArray(T* data, int size) const override {
        return new MutableArraySequence<T>(data, size);
    }

    Sequence<T>* Instance() const override {
        return new MutableArraySequence<T>();
    }

    Sequence<T>* Clone() const override {
        return new MutableArraySequence<T>(*this);
    }
    MutableArraySequence& operator=(const MutableArraySequence& other) {
        if (this != &other) {
            delete this->items;
            this->items = new DynamicArray<T>(*other.items);
        }
        return *this;
    }

};




#endif
