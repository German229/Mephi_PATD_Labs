#ifndef LISTS_H
#define LISTS_H

#include <stdexcept>

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        explicit Node(const T& value) : data(value), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;

    void Clear() {
        Node* cur = head;
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head = tail = nullptr;
        size = 0;
    }

public:
    // --- ctors/dtor ---
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    LinkedList(T* items, int count) : LinkedList() {
        if (count < 0) throw std::invalid_argument("count < 0");
        for (int i = 0; i < count; ++i) Append(items[i]);
    }

    LinkedList(const LinkedList& other) : LinkedList() {
        for (Node* cur = other.head; cur; cur = cur->next) Append(cur->data);
    }

    LinkedList(LinkedList&& other) noexcept
        : head(other.head), tail(other.tail), size(other.size) {
        other.head = other.tail = nullptr;
        other.size = 0;
    }

    ~LinkedList() { Clear(); }

    // --- assignment ---
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            Clear();
            for (Node* cur = other.head; cur; cur = cur->next) Append(cur->data);
        }
        return *this;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            Clear();
            head = other.head;
            tail = other.tail;
            size = other.size;
            other.head = other.tail = nullptr;
            other.size = 0;
        }
        return *this;
    }

    // --- basic ops ---
    int GetLength() const { return size; }

    T Get(int index) const {
        if (index < 0 || index >= size) throw std::out_of_range("Index out of range");
        Node* cur = head;
        for (int i = 0; i < index; ++i) cur = cur->next;
        return cur->data;
    }

    T GetFirst() const {
        if (!head) throw std::out_of_range("List is empty");
        return head->data;
    }

    T GetLast() const {
        if (!tail) throw std::out_of_range("List is empty");
        return tail->data;
    }

    void Append(const T& item) {
        Node* node = new Node(item);
        if (!tail) { head = tail = node; }
        else { tail->next = node; tail = node; }
        ++size;
    }

    void Prepend(const T& item) {
        Node* node = new Node(item);
        node->next = head;
        head = node;
        if (!tail) tail = head;
        ++size;
    }

    void InsertAt(int index, const T& item) {
        if (index < 0 || index > size) throw std::out_of_range("Index out of range");
        if (index == 0) { Prepend(item); return; }
        if (index == size) { Append(item); return; }
        Node* cur = head;
        for (int i = 0; i < index - 1; ++i) cur = cur->next;
        Node* node = new Node(item);
        node->next = cur->next;
        cur->next = node;
        ++size;
    }

    // --- utility builders ---
    // Возвращает НОВЫЙ список [start..end] (включительно).
    LinkedList<T>* GetSubList(int startIndex, int endIndex) const {
        if (startIndex < 0 || endIndex < 0 || startIndex > endIndex || endIndex >= size)
            throw std::out_of_range("Index out of range");
        auto* res = new LinkedList<T>();
        Node* cur = head;
        for (int i = 0; i < startIndex; ++i) cur = cur->next;
        for (int i = startIndex; i <= endIndex; ++i) {
            res->Append(cur->data);
            cur = cur->next;
        }
        return res;
    }

    // Возвращает НОВЫЙ список = this + list (глубокая конкатенация).
    LinkedList<T>* Concat(const LinkedList<T>* list) const {
        auto* res = new LinkedList<T>();
        for (Node* cur = head; cur; cur = cur->next) res->Append(cur->data);
        if (list) {
            for (Node* cur = list->head; cur; cur = cur->next) res->Append(cur->data);
        }
        return res;
    }
};

#endif // LISTS_H