#ifndef LISTS_H
#define LISTS_H

#include "sequence.h"
#include <stdexcept>
#include <iostream>


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
    void Print() const;

    LinkedList<T>* GetSubList(int startIndex, int endIndex) const;
    LinkedList<T>* Concat(const LinkedList<T>* list) const;
};


template<typename T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), size(0) {}

template<typename T>
LinkedList<T>::LinkedList(T* items, int count) : head(nullptr), tail(nullptr), size(0) {
    for (int i = 0; i < count; i++) {
        Append(items[i]);
    }
}

template<typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), size(0) {
    Node* current = other.head;
    while (current) {
        Append(current->data);
        current = current->next;
    }
}

template<typename T>
LinkedList<T>::~LinkedList() {
    Node* current = head;
    while (current) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

template<typename T>
int LinkedList<T>::GetLength() const {
    return size;
}

template<typename T>
T LinkedList<T>::Get(int index) const {
    if (index < 0 || index >= size)
        throw std::out_of_range("Index out of range");
    Node* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

template<typename T>
T LinkedList<T>::GetFirst() const {
    if (size == 0) throw std::out_of_range("List is empty");
    return head->data;
}

template<typename T>
T LinkedList<T>::GetLast() const {
    if (size == 0) throw std::out_of_range("List is empty");
    return tail->data;
}

template<typename T>
void LinkedList<T>::Append(T item) {
    Node* node = new Node(item);
    if (!head) {
        head = tail = node;
    } else {
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
    if (index < 0 || index > size) {
        throw std::out_of_range("Index out of range");
    }
    if (index == 0) {
        Prepend(item);
        return;
    }
    if (index == size) {
        Append(item);
        return;
    }

    Node* current = head;
    for (int i = 0; i < index - 1; i++) {
        current = current->next;
    }

    Node* node = new Node(item);
    node->next = current->next;
    current->next = node;
    size++;
}

template<typename T>
void LinkedList<T>::Print() const {
    Node* current = head;
    while (current) {
        std::cout << current->data << ' ';
        current = current->next;
    }
    std::cout << '\n';
}

template<typename T>
LinkedList<T>* LinkedList<T>::GetSubList(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= size || startIndex > endIndex) {
        throw std::out_of_range("Index out of range");
    }

    LinkedList<T>* result = new LinkedList<T>();
    Node* current = head;
    for (int i = 0; i < startIndex; i++) {
        current = current->next;
    }

    for (int i = startIndex; i <= endIndex; i++) {
        result->Append(current->data);
        current = current->next;
    }

    return result;
}

template<typename T>
LinkedList<T>* LinkedList<T>::Concat(const LinkedList<T>* list) const {
    LinkedList<T>* result = new LinkedList<T>(*this);
    Node* current = list->head;
    while (current) {
        result->Append(current->data);
        current = current->next;
    }
    return result;
}



template<typename T>
class ListSequence : public Sequence<T> {
private:
    LinkedList<T>* items;
    ListSequence(LinkedList<T>* list) {
        items = list;
    }

public:
    ListSequence(T* items, int count);
    ListSequence();
    ~ListSequence();

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
ListSequence<T>::ListSequence(T* itemsArray, int count) {
    items = new LinkedList<T>(itemsArray, count);
}

template<typename T>
ListSequence<T>::ListSequence() {
    items = new LinkedList<T>();
}

template<typename T>
ListSequence<T>::~ListSequence() {
    delete items;
}

template<typename T>
T ListSequence<T>::Get(int index) const {
    return items->Get(index);
}

template<typename T>
int ListSequence<T>::GetLength() const {
    return items->GetLength();
}

template<typename T>
T ListSequence<T>::GetFirst() const {
    return items->GetFirst();
}

template<typename T>
T ListSequence<T>::GetLast() const {
    return items->GetLast();
}

template<typename T>
Sequence<T>* ListSequence<T>::Append(T item) {
    LinkedList<T>* newList = new LinkedList<T>(*items);
    newList->Append(item);
    return new ListSequence<T>(newList);
}

template<typename T>
Sequence<T>* ListSequence<T>::Prepend(T item) {
    LinkedList<T>* newList = new LinkedList<T>(*items);
    newList->Prepend(item);
    return new ListSequence<T>(newList);
}

template<typename T>
Sequence<T>* ListSequence<T>::InsertAt(int index, T item) {
    if (index < 0 || index > GetLength()) {
        throw std::out_of_range("Index out of range");
    }

    LinkedList<T>* newList = new LinkedList<T>(*items);
    newList->InsertAt(index, item);
    return new ListSequence<T>(newList);
}

template<typename T>
Sequence<T>* ListSequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex) {
        throw std::out_of_range("Index out of range");
    }

    LinkedList<T>* subList = items->GetSubList(startIndex, endIndex);
    return new ListSequence<T>(subList);
}

template<typename T>
Sequence<T>* ListSequence<T>::Concat(const Sequence<T>& other) const {
    LinkedList<T>* result = new LinkedList<T>(*items);
    for (int i = 0; i < other.GetLength(); i++) {
        result->Append(other.Get(i));
    }
    return new ListSequence<T>(result);
}
#endif