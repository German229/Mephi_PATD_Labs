#pragma once
#include "Lists.h"

class IGraph {
public:
    virtual ~IGraph() = default;
    virtual int VerticesCount() const = 0;
    virtual void GetNeighbors(int v, LinkedList<int>& outNeighbors) const = 0;
};