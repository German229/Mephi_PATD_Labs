#include <iostream>
#include <cassert>
#include <queue>
#include <algorithm>

#include "Graphs.h"
#include "ConnectedComponents.h"
#include "LazyGraph.h"

static void expectCount(const char* name, int got, int exp) {
    std::cout << name << ": expected " << exp << ", got " << got
              << " -> " << (got == exp ? "PASS" : "FAIL") << "\n";
    assert(got == exp);
}

extern "C" void RunGraphTests() {
    std::cout << "Running graph tests...\n";

    { // T1: 0 вершин
        AdjListGraph g(0);
        auto a = cc::ConnectedComponentsDFS(g);
        auto b = cc::ConnectedComponentsBFS(g);
        expectCount("T1 empty (DFS)", (int)a.size(), 0);
        expectCount("T1 empty (BFS)", (int)b.size(), 0);
    }

    { // T2: 1 вершина
        AdjListGraph g(1);
        auto a = cc::ConnectedComponentsDFS(g);
        auto b = cc::ConnectedComponentsBFS(g);
        expectCount("T2 single (DFS)", (int)a.size(), 1);
        expectCount("T2 single (BFS)", (int)b.size(), 1);
        assert((int)a[0].size() == 1 && a[0][0] == 0);
    }

    { // T3: 5 изолированных
        AdjListGraph g(5);
        auto a = cc::ConnectedComponentsDFS(g);
        expectCount("T3 isolated (DFS)", (int)a.size(), 5);
    }

    { // T4: цепочка 0-1-2-3-4
        AdjListGraph g(5);
        g.AddEdge(0,1); g.AddEdge(1,2); g.AddEdge(2,3); g.AddEdge(3,4);
        auto a = cc::ConnectedComponentsDFS(g);
        auto b = cc::ConnectedComponentsBFS(g);
        expectCount("T4 chain (DFS)", (int)a.size(), 1);
        expectCount("T4 chain (BFS)", (int)b.size(), 1);
        assert((int)a[0].size() == 5);
    }

    { // T5: две компоненты: {0,1,2} и {3,4}
        AdjListGraph g(5);
        g.AddEdge(0,1); g.AddEdge(1,2);
        g.AddEdge(3,4);
        auto a = cc::ConnectedComponentsDFS(g);
        expectCount("T5 two comps (DFS)", (int)a.size(), 2);
        int s0 = (int)a[0].size(), s1 = (int)a[1].size();
        assert((s0 == 3 && s1 == 2) || (s0 == 2 && s1 == 3));
    }

    { // T6: on-demand граф (если у тебя есть OnDemandGraph)
      // Оставляю как в твоей версии.
        OnDemandGraph g(10);
        auto a = cc::ConnectedComponentsDFS(g);
        expectCount("T6 on-demand (DFS)", (int)a.size(), 2);
        int s0 = (int)a[0].size(), s1 = (int)a[1].size();
        assert(s0 + s1 == 10);
    }

    { // T7: LazyGraph — решётка 3x2 (W=3, H=2) с 4-связностью. Должен быть связный (1 компонент).
        const int W = 3, H = 2, N = W * H;
        LazyGraph g(N, [W, H](int u, LinkedList<int>& out) {
            int x = u % W, y = u / W;
            auto add = [&](int nx, int ny) {
                if (0 <= nx && nx < W && 0 <= ny && ny < H) {
                    int v = ny * W + nx;
                    if (v != u) out.Append(v);
                }
            };
            add(x - 1, y);
            add(x + 1, y);
            add(x, y - 1);
            add(x, y + 1);
        });

        auto a = cc::ConnectedComponentsDFS(g);
        auto b = cc::ConnectedComponentsBFS(g);
        expectCount("T7 lazy-grid (DFS)", (int)a.size(), 1);
        expectCount("T7 lazy-grid (BFS)", (int)b.size(), 1);
        assert((int)a[0].size() == N);
    }

    std::cout << "All graph tests passed.\n";
}
