#ifndef LISTS_H
#define LISTS_H

#include "sequence.h"
#include <stdexcept>

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(T value) : data(value), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;

public:
    LinkedList();
    LinkedList(T* items, int count);
    LinkedList(const LinkedList<T>& other);
    ~LinkedList();

    T Get(int index) const;
    void Append(T item);
    void Prepend(T item);
    void InsertAt(int index, T item);
    int GetLength() const;

    T GetFirst() const;
    T GetLast() const;
    LinkedList<T>* GetSubList(int startIndex, int endIndex) const;
    LinkedList<T>* Concat(const LinkedList<T>* list) const;
};

template<typename T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), size(0) {}

template<typename T>
LinkedList<T>::LinkedList(T* items, int count) : LinkedList() {
    for (int i = 0; i < count; i++) Append(items[i]);
}

template<typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& other) : LinkedList() {
    for (Node* cur = other.head; cur != nullptr; cur = cur->next)
        Append(cur->data);
}

template<typename T>
LinkedList<T>::~LinkedList() {
    Node* cur = head;
    while (cur) {
        Node* next = cur->next;
        delete cur;
        cur = next;
    }
}

template<typename T>
int LinkedList<T>::GetLength() const { return size; }

template<typename T>
T LinkedList<T>::Get(int index) const {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index out of range");
    Node* cur = head;
    for (int i = 0; i < index; i++) cur = cur->next;
    return cur->data;
}

template<typename T>
T LinkedList<T>::GetFirst() const {
    if (!head) throw std::out_of_range("List is empty");
    return head->data;
}

template<typename T>
T LinkedList<T>::GetLast() const {
    if (!tail) throw std::out_of_range("List is empty");
    return tail->data;
}

template<typename T>
void LinkedList<T>::Append(T item) {
    Node* node = new Node(item);
    if (!tail) head = tail = node;
    else {
        tail->next = node;
        tail = node;
    }
    size++;
}

template<typename T>
void LinkedList<T>::Prepend(T item) {
    Node* node = new Node(item);
    node->next = head;
    head = node;
    if (!tail) tail = head;
    size++;
}

template<typename T>
void LinkedList<T>::InsertAt(int index, T item) {
    if (index < 0 || index > size)
        throw std::out_of_range("Index out of range");
    if (index == 0) return Prepend(item);
    if (index == size) return Append(item);
    Node* cur = head;
    for (int i = 0; i < index - 1; i++) cur = cur->next;
    Node* node = new Node(item);
    node->next = cur->next;
    cur->next = node;
    size++;
}

template<typename T>
LinkedList<T>* LinkedList<T>::GetSubList(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= size || startIndex > endIndex)
        throw std::out_of_range("Index out of range");
    LinkedList<T>* sublist = new LinkedList<T>();
    Node* cur = head;
    for (int i = 0; i < startIndex; i++) cur = cur->next;
    for (int i = startIndex; i <= endIndex; i++) {
        sublist->Append(cur->data);
        cur = cur->next;
    }
    return sublist;
}

template<typename T>
LinkedList<T>* LinkedList<T>::Concat(const LinkedList<T>* list) const {
    LinkedList<T>* result = new LinkedList<T>(*this);
    for (Node* cur = list->head; cur; cur = cur->next)
        result->Append(cur->data);
    return result;
}

// ------------------------- ListSequence -------------------------

template<typename T>
class ListSequence : public Sequence<T> {
protected:
    LinkedList<T>* items;

public:
    ListSequence();
    ListSequence(T* data, int count);
    ListSequence(const ListSequence<T>& other);
    virtual ~ListSequence();

    T Get(int index) const override;
    int GetLength() const override;
    T GetFirst() const override;
    T GetLast() const override;

    Sequence<T>* Append(T item) override;
    Sequence<T>* Prepend(T item) override;
    Sequence<T>* InsertAt(int index, T item) override;
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    Sequence<T>* Concat(const Sequence<T>& other) const override;

    virtual Sequence<T>* CreateFromArray(T* data, int size) const = 0;
    virtual Sequence<T>* Instance() const = 0;
    virtual Sequence<T>* Clone() const = 0;
};

template<typename T>
ListSequence<T>::ListSequence() : items(new LinkedList<T>()) {}

template<typename T>
ListSequence<T>::ListSequence(T* data, int count)
    : items(new LinkedList<T>(data, count)) {}

template<typename T>
ListSequence<T>::ListSequence(const ListSequence<T>& other)
    : items(new LinkedList<T>(*other.items)) {}

template<typename T>
ListSequence<T>::~ListSequence() { delete items; }

template<typename T>
T ListSequence<T>::Get(int index) const { return items->Get(index); }

template<typename T>
int ListSequence<T>::GetLength() const { return items->GetLength(); }

template<typename T>
T ListSequence<T>::GetFirst() const { return items->GetFirst(); }

template<typename T>
T ListSequence<T>::GetLast() const { return items->GetLast(); }

template<typename T>
Sequence<T>* ListSequence<T>::Append(T item) {
    int size = GetLength();
    T* newData = new T[size + 1];
    for (int i = 0; i < size; i++) newData[i] = Get(i);
    newData[size] = item;
    auto* result = CreateFromArray(newData, size + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ListSequence<T>::Prepend(T item) {
    int size = GetLength();
    T* newData = new T[size + 1];
    newData[0] = item;
    for (int i = 0; i < size; i++) newData[i + 1] = Get(i);
    auto* result = CreateFromArray(newData, size + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ListSequence<T>::InsertAt(int index, T item) {
    int size = GetLength();
    T* newData = new T[size + 1];
    for (int i = 0; i < index; i++) newData[i] = Get(i);
    newData[index] = item;
    for (int i = index; i < size; i++) newData[i + 1] = Get(i);
    auto* result = CreateFromArray(newData, size + 1);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ListSequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    int size = endIndex - startIndex + 1;
    T* newData = new T[size];
    for (int i = 0; i < size; i++) newData[i] = Get(startIndex + i);
    auto* result = CreateFromArray(newData, size);
    delete[] newData;
    return result;
}

template<typename T>
Sequence<T>* ListSequence<T>::Concat(const Sequence<T>& other) const {
    int size = GetLength() + other.GetLength();
    T* newData = new T[size];
    for (int i = 0; i < GetLength(); i++) newData[i] = Get(i);
    for (int i = 0; i < other.GetLength(); i++) newData[GetLength() + i] = other.Get(i);
    auto* result = CreateFromArray(newData, size);
    delete[] newData;
    return result;
}

// ------------------------- Immutable -------------------------

template<typename T>
class ImmutableListSequence : public ListSequence<T> {
public:
    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(T* data, int count) : ListSequence<T>(data, count) {}
    ImmutableListSequence(const ImmutableListSequence<T>& other) : ListSequence<T>(other) {}

    Sequence<T>* CreateFromArray(T* data, int size) const override {
        return new ImmutableListSequence<T>(data, size);
    }

    Sequence<T>* Instance() const override {
        return new ImmutableListSequence<T>();
    }

    Sequence<T>* Clone() const override {
        int size = this->GetLength();
        T* newData = new T[size];
        for (int i = 0; i < size; i++) newData[i] = this->Get(i);
        auto* result = new ImmutableListSequence<T>(newData, size);
        delete[] newData;
        return result;
    }
};

// ------------------------- Mutable -------------------------

template<typename T>
class MutableListSequence : public ListSequence<T> {
public:
    MutableListSequence() : ListSequence<T>() {}
    MutableListSequence(T* data, int count) : ListSequence<T>(data, count) {}
    MutableListSequence(const MutableListSequence<T>& other) : ListSequence<T>(other) {}

    Sequence<T>* CreateFromArray(T* data, int size) const override {
        return new MutableListSequence<T>(data, size);
    }

    Sequence<T>* Instance() const override {
        return new MutableListSequence<T>();
    }

    Sequence<T>* Clone() const override {
        return new MutableListSequence<T>(*this);
    }
};

#endif
