#pragma once
#include <type_traits>
#include <cstddef>
#include <cassert>

template<typename T>
class UnqPtr {
private:
    // Если T = U[], то Elem = U, иначе Elem = T
    using Elem = std::conditional_t<std::is_array_v<T>, std::remove_extent_t<T>, T>;

    Elem* ptr = nullptr;

    static void destroy(Elem* p) {
        if (!p) return;
        if constexpr (std::is_array_v<T>)
            delete[] p;   // T — массив => delete[]
        else
            delete p;     // T — одиночный => delete
    }

public:
    // Конструктор от сырого указателя (и для одиночного, и для массива)
    explicit UnqPtr(Elem* p = nullptr) : ptr(p) {}

    ~UnqPtr() { destroy(ptr); }

    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    UnqPtr(UnqPtr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
    UnqPtr& operator=(UnqPtr&& other) noexcept {
        if (this != &other) {
            destroy(ptr);
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // Доступ к указателю
    Elem* get() const { return ptr; }

    // Разыменование — только для одиночного объекта
    Elem& operator*()  const { assert(!std::is_array_v<T> && "operator* only for single object"); return *ptr; }
    Elem* operator->() const { assert(!std::is_array_v<T> && "operator-> only for single object"); return  ptr; }

    // Индексация — только для массива
    Elem& operator[](std::size_t i) const { assert(std::is_array_v<T> && "operator[] only for array"); return ptr[i]; }

    // Отдать владение (без удаления)
    Elem* release() { Elem* tmp = ptr; ptr = nullptr; return tmp; }

    explicit operator bool() const { return ptr != nullptr; }
};
