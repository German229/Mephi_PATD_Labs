// Graphs.h
#pragma once
#include <stdexcept>
#include "dynamic_array.h"
#include "Lists.h"
#include "IGraph.h"

// --- Неориентированный граф на списках смежности ---
class AdjListGraph : public IGraph {
    int n;
    DynamicArray<LinkedList<int>> adj; // size n

    // проверка: есть ли уже ребро u--v
    bool hasNeighbor(int u, int v) const {
        const LinkedList<int>& lu = adj[u];
        for (int i = 0; i < lu.GetLength(); ++i)
            if (lu.Get(i) == v) return true;
        return false;
    }

public:
    explicit AdjListGraph(int vertices) : n(vertices), adj(vertices) {}

    int VerticesCount() const override { return n; }

    // Добавляет неориентированное ребро u <-> v
    // Запрещаем: петли и кратные рёбра
    void AddEdge(int u, int v) {
        if (u < 0 || v < 0 || u >= n || v >= n)
            throw std::out_of_range("vertex index");
        if (u == v)
            throw std::logic_error("loops are not allowed");
        if (hasNeighbor(u, v))
            throw std::logic_error("duplicate edge is not allowed");

        adj[u].Append(v);
        adj[v].Append(u);
    }

    // Выдать соседей v в outNeighbors; ожидаем, что outNeighbors пуст
    void GetNeighbors(int v, LinkedList<int>& outNeighbors) const override {
        if (v < 0 || v >= n) throw std::out_of_range("vertex index");
        const LinkedList<int>& src = adj[v];
        for (int i = 0; i < src.GetLength(); ++i)
            outNeighbors.Append(src.Get(i));
    }
};

// --- Ленивый граф (материализация по требованию) ---
// N вершин (0..N-1). Соседи v: (v-2) и (v+2), если в пределах.
class OnDemandGraph : public IGraph {
    int n;
public:
    explicit OnDemandGraph(int vertices) : n(vertices) {}
    int VerticesCount() const override { return n; }

    void GetNeighbors(int v, LinkedList<int>& outNeighbors) const override {
        if (v < 0 || v >= n) return;
        if (v - 2 >= 0) outNeighbors.Append(v - 2);
        if (v + 2 <  n) outNeighbors.Append(v + 2);
    }
};
