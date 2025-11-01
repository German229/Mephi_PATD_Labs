#pragma once
#include "Lists.h"

// Базовый интерфейс графа для обобщённых алгоритмов
class IGraph {
public:
    virtual ~IGraph() = default;
    virtual int VerticesCount() const = 0;
    // outNeighbors должен быть пуст при входе; метод заполняет его соседями v
    virtual void GetNeighbors(int v, LinkedList<int>& outNeighbors) const = 0;
};