#pragma once
#include <vector>
#include <type_traits>
#include "UnqPtr.hpp"
#include "ShrdPtr.hpp"

template<typename T>
class Sequence {
    std::vector<ShrdPtr<T>> data_;
public:
    Sequence() = default;
    std::size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }

    void push(const ShrdPtr<T>& s) { data_.push_back(s); }
    void push(UnqPtr<T>&& u) { data_.emplace_back(std::move(u)); }

    template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    void push_unq(UnqPtr<U>&& u) {
        ShrdPtr<U> su(std::move(u));
        ShrdPtr<T> st = su;
        data_.push_back(st);
    }

    const ShrdPtr<T>& at(std::size_t i) const { return data_.at(i); }
    ShrdPtr<T>& at(std::size_t i) { return data_.at(i); }
};
