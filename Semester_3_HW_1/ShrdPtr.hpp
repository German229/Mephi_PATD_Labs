#pragma once
#include "UnqPtr.hpp"
#include <cstddef>
#include <type_traits>

template<typename T>
class ShrdPtr {
    T* ptr;
    int* refCount;

    void release() {
        if (refCount) {
            (*refCount)--;
            if (*refCount == 0) {
                delete ptr;
                delete refCount;
            }
        }
    }
public:
    ShrdPtr() : ptr(nullptr), refCount(nullptr) {}

    explicit ShrdPtr(UnqPtr<T>&& u) {
        ptr = u.release();
        if (ptr) {
            refCount = new int(1);
        } else {
            refCount = nullptr;
        }
    }

    ShrdPtr(const ShrdPtr& other) : ptr(other.ptr), refCount(other.refCount) {
        if (refCount) (*refCount)++;
    }
    ShrdPtr& operator=(const ShrdPtr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            refCount = other.refCount;
            if (refCount) (*refCount)++;
        }
        return *this;
    }

    template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    ShrdPtr(const ShrdPtr<U>& other) {
        ptr = static_cast<T*>(other.ptr);
        refCount = other.refCount;
        if (refCount) (*refCount)++;
    }

    ~ShrdPtr() { release(); }

    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }

    int use_count() const { return refCount ? *refCount : 0; }

    explicit operator bool() const { return ptr != nullptr; }

    template<typename U> friend class ShrdPtr;
};
