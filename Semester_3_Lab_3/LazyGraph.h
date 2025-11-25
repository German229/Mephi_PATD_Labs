#ifndef LAZY_GRAPH_H
#define LAZY_GRAPH_H

#include <functional>
#include <stdexcept>
#include "IGraph.h"
#include "Lists.h"

class LazyGraph : public IGraph {
public:
    using NeighborFn = std::function<void(int /*u*/, LinkedList<int>& /*out*/)>;

    LazyGraph(int n, NeighborFn gen) : n_(n), gen_(std::move(gen)) {
        if (n_ < 0) throw std::invalid_argument("n must be >= 0");
        if (!gen_) throw std::invalid_argument("generator is null");
    }

    int VerticesCount() const override { return n_; }

    void GetNeighbors(int v, LinkedList<int>& out) const override {
        if (v < 0 || v >= n_) throw std::out_of_range("vertex index");
        out = LinkedList<int>();
        gen_(v, out);
    }

private:
    int n_;
    NeighborFn gen_;
};

#endif