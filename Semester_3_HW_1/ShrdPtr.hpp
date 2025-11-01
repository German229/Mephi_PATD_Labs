#pragma once
#include "UnqPtr.hpp"
#include <type_traits>
#include <cassert>

template<typename T>
class ShrdPtr {
private:
    using Elem = std::conditional_t<std::is_array_v<T>, std::remove_extent_t<T>, T>;

    Elem* ptr = nullptr;
    int*  refCount = nullptr;

    static void destroy(Elem* p) {
        if (!p) return;
        if constexpr (std::is_array_v<T>)
            delete[] p;
        else
            delete p;
    }

    void release() {
        if (refCount) {
            if (--(*refCount) == 0) {
                destroy(ptr);
                delete refCount;
            }
        }
    }

public:
    ShrdPtr() = default;

    explicit ShrdPtr(UnqPtr<T>&& u) {
        ptr = u.release();
        if (ptr) refCount = new int(1);
    }

    ShrdPtr(const ShrdPtr& other) : ptr(other.ptr), refCount(other.refCount) {
        if (refCount) (*refCount)++;
    }
    ShrdPtr& operator=(const ShrdPtr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr; refCount = other.refCount;
            if (refCount) (*refCount)++;
        }
        return *this;
    }

    template<typename U,
             typename = std::enable_if_t<!std::is_array_v<T> && !std::is_array_v<U> && std::is_base_of_v<T, U>>>
    ShrdPtr(const ShrdPtr<U>& other) {
        ptr = static_cast<T*>(other.ptr);
        refCount = other.refCount;
        if (refCount) (*refCount)++;
    }

    ~ShrdPtr() { release(); }

    Elem* get() const { return ptr; }

    Elem& operator*()  const { assert(!std::is_array_v<T> && "operator* only for single object"); return *ptr; }
    Elem* operator->() const { assert(!std::is_array_v<T> && "operator-> only for single object"); return  ptr; }

    Elem& operator[](std::size_t i) const { assert(std::is_array_v<T> && "operator[] only for array"); return ptr[i]; }

    int  use_count() const { return refCount ? *refCount : 0; }
    explicit operator bool() const { return ptr != nullptr; }

    template<typename U> friend class ShrdPtr;
};
