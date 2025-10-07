#pragma once

template<typename T>
class UnqPtr {
private:
    T* ptr;
public:
    UnqPtr(T* p = nullptr) : ptr(p) {}
    ~UnqPtr() { delete ptr; }

    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    UnqPtr(UnqPtr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
    UnqPtr& operator=(UnqPtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }

    T* release() { T* tmp = ptr; ptr = nullptr; return tmp; }

    explicit operator bool() const { return ptr != nullptr; }
};
