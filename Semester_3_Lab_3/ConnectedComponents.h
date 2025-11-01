#pragma once
#include <vector>
#include "IGraph.h"

namespace cc {
    // DFS
    std::vector<std::vector<int>> ConnectedComponentsDFS(const IGraph& g);
    // BFS
    std::vector<std::vector<int>> ConnectedComponentsBFS(const IGraph& g);
}
