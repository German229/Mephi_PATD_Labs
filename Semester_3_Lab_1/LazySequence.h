#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "sequence.h"
#include <functional>
#include <stdexcept>

template<typename T>
class LazySequence : public Sequence<T> {
public:
    LazySequence();
    LazySequence(T* data, int count);
    LazySequence(const Sequence<T>& other);
    LazySequence(const LazySequence<T>& other);
    LazySequence<T>& operator=(const LazySequence<T>& other);
    ~LazySequence();
    LazySequence(std::function<T(const LazySequence<T>&, int)> generator, int length);

    T Get(int index) const override;
    int GetLength() const override;
    T GetFirst() const override;
    T GetLast() const override;

    Sequence<T>* Append(T item) override;
    Sequence<T>* Prepend(T item) override;
    Sequence<T>* InsertAt(int index, T item) override;
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    Sequence<T>* Concat(const Sequence<T>& other) const override;

    Sequence<T>* CreateFromArray(T* data, int size) const override;
    Sequence<T>* Instance() const override;
    Sequence<T>* Clone() const override;

    int GetMaterializedCount() const { return materializedCount; }

private:
    mutable DynamicArray<T>* items;
    int logicalLength;
    mutable int materializedCount;
    std::function<T(const LazySequence<T>&, int)> generator;
    bool hasGenerator;

    void ensureMaterialized(int upto) const;
};

template<typename T>
LazySequence<T>::LazySequence()
    : items(new DynamicArray<T>(0)),
      logicalLength(0),
      materializedCount(0),
      generator(),
      hasGenerator(false)
{
}

template<typename T>
LazySequence<T>::LazySequence(T* data, int count)
    : items(new DynamicArray<T>(count)),
      logicalLength(count),
      materializedCount(count),
      generator(),
      hasGenerator(false)
{
    for (int i = 0; i < count; ++i) {
        items->Set(i, data[i]);
    }
}

template<typename T>
LazySequence<T>::LazySequence(const Sequence<T>& other)
    : items(nullptr),
      logicalLength(other.GetLength()),
      materializedCount(0),
      generator(),
      hasGenerator(false)
{
    items = new DynamicArray<T>(logicalLength);
    for (int i = 0; i < logicalLength; ++i) {
        items->Set(i, other.Get(i));
    }
    materializedCount = logicalLength;
}

template<typename T>
LazySequence<T>::LazySequence(const LazySequence<T>& other)
    : items(new DynamicArray<T>(other.logicalLength)),
      logicalLength(other.logicalLength),
      materializedCount(other.materializedCount),
      generator(other.generator),
      hasGenerator(other.hasGenerator)
{
    for (int i = 0; i < other.materializedCount; ++i) {
        items->Set(i, other.items->Get(i));
    }
}

template<typename T>
LazySequence<T>& LazySequence<T>::operator=(const LazySequence<T>& other) {
    if (this == &other) return *this;
    delete items;
    logicalLength = other.logicalLength;
    materializedCount = other.materializedCount;
    generator = other.generator;
    hasGenerator = other.hasGenerator;
    items = new DynamicArray<T>(logicalLength);
    for (int i = 0; i < other.materializedCount; ++i) {
        items->Set(i, other.items->Get(i));
    }
    return *this;
}

template<typename T>
LazySequence<T>::LazySequence(std::function<T(const LazySequence<T>&, int)> gen, int length)
    : items(new DynamicArray<T>(length)),
      logicalLength(length),
      materializedCount(0),
      generator(gen),
      hasGenerator(true)
{
    if (length < 0) {
        throw std::invalid_argument("length must be non-negative");
    }
}

template<typename T>
LazySequence<T>::~LazySequence() {
    delete items;
}

template<typename T>
void LazySequence<T>::ensureMaterialized(int upto) const {
    if (upto < 0 || upto > logicalLength) {
        throw std::out_of_range("index out of range");
    }
    if (!hasGenerator) {
        if (upto > materializedCount) {
            materializedCount = upto;
        }
        return;
    }
    for (int i = materializedCount; i < upto; ++i) {
        T value = generator(*this, i);
        items->Set(i, value);
        materializedCount++;
    }
}

template<typename T>
T LazySequence<T>::Get(int index) const {
    if (index < 0 || index >= logicalLength) {
        throw std::out_of_range("index out of range");
    }
    if (index >= materializedCount) {
        ensureMaterialized(index + 1);
    }
    return items->Get(index);
}

template<typename T>
int LazySequence<T>::GetLength() const {
    return logicalLength;
}

template<typename T>
T LazySequence<T>::GetFirst() const {
    if (logicalLength == 0) {
        throw std::out_of_range("sequence is empty");
    }
    return Get(0);
}

template<typename T>
T LazySequence<T>::GetLast() const {
    if (logicalLength == 0) {
        throw std::out_of_range("sequence is empty");
    }
    return Get(logicalLength - 1);
}

template<typename T>
Sequence<T>* LazySequence<T>::Append(T item) {
    int newLen = logicalLength + 1;
    T* buffer = new T[newLen];
    for (int i = 0; i < logicalLength; ++i) {
        buffer[i] = Get(i);
    }
    buffer[logicalLength] = item;
    auto* result = new LazySequence<T>(buffer, newLen);
    delete[] buffer;
    return result;
}

template<typename T>
Sequence<T>* LazySequence<T>::Prepend(T item) {
    int newLen = logicalLength + 1;
    T* buffer = new T[newLen];
    buffer[0] = item;
    for (int i = 0; i < logicalLength; ++i) {
        buffer[i + 1] = Get(i);
    }
    auto* result = new LazySequence<T>(buffer, newLen);
    delete[] buffer;
    return result;
}

template<typename T>
Sequence<T>* LazySequence<T>::InsertAt(int index, T item) {
    if (index < 0 || index > logicalLength) {
        throw std::out_of_range("index out of range");
    }
    int newLen = logicalLength + 1;
    T* buffer = new T[newLen];
    for (int i = 0; i < index; ++i) {
        buffer[i] = Get(i);
    }
    buffer[index] = item;
    for (int i = index; i < logicalLength; ++i) {
        buffer[i + 1] = Get(i);
    }
    auto* result = new LazySequence<T>(buffer, newLen);
    delete[] buffer;
    return result;
}

template<typename T>
Sequence<T>* LazySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex < startIndex || endIndex >= logicalLength) {
        throw std::out_of_range("index out of range");
    }
    int newLen = endIndex - startIndex + 1;
    T* buffer = new T[newLen];
    for (int i = 0; i < newLen; ++i) {
        buffer[i] = Get(startIndex + i);
    }
    auto* result = new LazySequence<T>(buffer, newLen);
    delete[] buffer;
    return result;
}

template<typename T>
Sequence<T>* LazySequence<T>::Concat(const Sequence<T>& other) const {
    int otherLen = other.GetLength();
    int newLen = logicalLength + otherLen;
    T* buffer = new T[newLen];
    for (int i = 0; i < logicalLength; ++i) {
        buffer[i] = Get(i);
    }
    for (int i = 0; i < otherLen; ++i) {
        buffer[logicalLength + i] = other.Get(i);
    }
    auto* result = new LazySequence<T>(buffer, newLen);
    delete[] buffer;
    return result;
}

template<typename T>
Sequence<T>* LazySequence<T>::CreateFromArray(T* data, int size) const {
    return new LazySequence<T>(data, size);
}

template<typename T>
Sequence<T>* LazySequence<T>::Instance() const {
    return new LazySequence<T>();
}

template<typename T>
Sequence<T>* LazySequence<T>::Clone() const {
    return new LazySequence<T>(*this);
}

#endif