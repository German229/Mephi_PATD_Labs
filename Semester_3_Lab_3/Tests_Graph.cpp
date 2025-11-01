#include <iostream>
#include <cassert>
#include "Graphs.h"
#include "ConnectedComponents.h"

static void expectCount(const char* name, int got, int exp) {
    std::cout << name << ": expected " << exp << ", got " << got
              << " -> " << (got == exp ? "PASS" : "FAIL") << "\n";
    assert(got == exp);
}

extern "C" void RunGraphTests() {
    std::cout << "Running graph tests...\n";

    { // 0 вершин
        AdjListGraph g(0);
        auto a = cc::ConnectedComponentsDFS(g);
        auto b = cc::ConnectedComponentsBFS(g);
        expectCount("T1 empty (DFS)", (int)a.size(), 0);
        expectCount("T1 empty (BFS)", (int)b.size(), 0);
    }
    { // 1 вершина
        AdjListGraph g(1);
        auto a = cc::ConnectedComponentsDFS(g);
        auto b = cc::ConnectedComponentsBFS(g);
        expectCount("T2 single (DFS)", (int)a.size(), 1);
        expectCount("T2 single (BFS)", (int)b.size(), 1);
        assert((int)a[0].size() == 1 && a[0][0] == 0);
    }
    { // 5 изолированных
        AdjListGraph g(5);
        auto a = cc::ConnectedComponentsDFS(g);
        expectCount("T3 isolated (DFS)", (int)a.size(), 5);
    }
    { // цепочка 0-1-2-3-4
        AdjListGraph g(5);
        g.AddEdge(0,1); g.AddEdge(1,2); g.AddEdge(2,3); g.AddEdge(3,4);
        auto a = cc::ConnectedComponentsDFS(g);
        auto b = cc::ConnectedComponentsBFS(g);
        expectCount("T4 chain (DFS)", (int)a.size(), 1);
        expectCount("T4 chain (BFS)", (int)b.size(), 1);
        assert((int)a[0].size() == 5);
    }
    { // две компоненты: {0,1,2} и {3,4}
        AdjListGraph g(5);
        g.AddEdge(0,1); g.AddEdge(1,2);
        g.AddEdge(3,4);
        auto a = cc::ConnectedComponentsDFS(g);
        expectCount("T5 two comps (DFS)", (int)a.size(), 2);
        int s0 = (int)a[0].size(), s1 = (int)a[1].size();
        assert((s0 == 3 && s1 == 2) || (s0 == 2 && s1 == 3));
    }
    { // ленивый граф N=10 -> 2 компоненты
        OnDemandGraph g(10);
        auto a = cc::ConnectedComponentsDFS(g);
        expectCount("T6 on-demand (DFS)", (int)a.size(), 2);
        int s0 = (int)a[0].size(), s1 = (int)a[1].size();
        assert(s0 + s1 == 10);
    }

    std::cout << "All graph tests passed.\n";
}
