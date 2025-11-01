#include "ConnectedComponents.h"
#include <algorithm>
#include <queue>

namespace cc {

    static void dfsOne(const IGraph& g, int v, std::vector<bool>& used, std::vector<int>& comp) {
        used[v] = true;
        comp.push_back(v);
        LinkedList<int> neigh;
        g.GetNeighbors(v, neigh);
        for (int i = 0; i < neigh.GetLength(); ++i) {
            int u = neigh.Get(i);
            if (!used[u]) dfsOne(g, u, used, comp);
        }
    }

    std::vector<std::vector<int>> ConnectedComponentsDFS(const IGraph& g) {
        int n = g.VerticesCount();
        std::vector<bool> used(n, false);
        std::vector<std::vector<int>> comps;
        for (int v = 0; v < n; ++v) {
            if (!used[v]) {
                std::vector<int> comp;
                dfsOne(g, v, used, comp);
                std::sort(comp.begin(), comp.end());
                comps.push_back(std::move(comp));
            }
        }
        return comps;
    }

    static void bfsOne(const IGraph& g, int s, std::vector<bool>& used, std::vector<int>& comp) {
        std::queue<int> q;
        used[s] = true; q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            comp.push_back(v);
            LinkedList<int> neigh;
            g.GetNeighbors(v, neigh);
            for (int i = 0; i < neigh.GetLength(); ++i) {
                int u = neigh.Get(i);
                if (!used[u]) { used[u] = true; q.push(u); }
            }
        }
    }

    std::vector<std::vector<int>> ConnectedComponentsBFS(const IGraph& g) {
        int n = g.VerticesCount();
        std::vector<bool> used(n, false);
        std::vector<std::vector<int>> comps;
        for (int v = 0; v < n; ++v) {
            if (!used[v]) {
                std::vector<int> comp;
                bfsOne(g, v, used, comp);
                std::sort(comp.begin(), comp.end());
                comps.push_back(std::move(comp));
            }
        }
        return comps;
    }

} // namespace cc
