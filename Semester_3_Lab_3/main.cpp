// main.cpp
#include <iostream>
#include <fstream>
#include <cstring>
#include "Timer.h"
#include "Graphs.h"
#include "ConnectedComponents.h"
#include "GraphVizSFML.h"  // VisualizeGraphSFML(AdjListGraph&)

// объявление из Tests_Graph.cpp
extern "C" void RunGraphTests();

// --- чтение графа с жёсткой валидацией; ok=false => ввод отменён/ошибка ---
static AdjListGraph readGraphInteractiveOrAbort(bool& ok) {
    ok = false;
    int n, m;
    std::cout << "Enter number of vertices (0-based): ";
    if (!(std::cin >> n)) return AdjListGraph(0);
    if (n < 0) { std::cout << "Error: n must be >= 0\n"; return AdjListGraph(0); }

    AdjListGraph g(n);

    std::cout << "Enter number of edges: ";
    if (!(std::cin >> m)) return AdjListGraph(0);
    if (m < 0) { std::cout << "Error: m must be >= 0\n"; return AdjListGraph(0); }

    long long maxSimple = 1LL * n * (n - 1) / 2;
    if (m > maxSimple) {
        std::cout << "Error: for a simple undirected graph with n=" << n
                  << " max edges is " << maxSimple << ". Aborting.\n";
        return AdjListGraph(0);
    }

    std::cout << "Enter " << m << " edges as pairs 'u v' (0-based, no loops/duplicates):\n";
    int added = 0;
    while (added < m) {
        int u, v;
        if (!(std::cin >> u >> v)) return AdjListGraph(0); // EOF/ошибка ввода
        try {
            g.AddEdge(u, v);  // бросит исключение для петли/дубликата/индексов вне диапазона
            ++added;
        } catch (const std::out_of_range& e) {
            std::cout << "Invalid edge (" << u << "," << v << "): " << e.what() << ". Re-enter:\n";
        } catch (const std::logic_error& e) {
            std::cout << "Invalid edge (" << u << "," << v << "): " << e.what() << ". Re-enter:\n";
        }
    }
    ok = true;
    return g;
}

// --- режим ручного запуска алгоритма (DFS/BFS) с выводом компонент и времени ---
static void runManual() {
    bool ok = false;
    AdjListGraph g = readGraphInteractiveOrAbort(ok);
    if (!ok) return;

    std::cout << "Choose algorithm: 1) DFS  2) BFS  3) Compare & save CSV\nChoice: ";
    int choice; if (!(std::cin >> choice)) return;

    if (choice == 1) {
        Timer t; t.start();
        auto comps = cc::ConnectedComponentsDFS(g);
        long long ms = t.ms();
        std::cout << "Components: " << comps.size() << "\n";
        for (size_t i = 0; i < comps.size(); ++i) {
            std::cout << "Component " << (i+1) << ": ";
            for (int v : comps[i]) std::cout << v << " ";
            std::cout << "\n";
        }
        std::cout << "Time (DFS): " << ms << " ms\n";
    } else if (choice == 2) {
        Timer t; t.start();
        auto comps = cc::ConnectedComponentsBFS(g);
        long long ms = t.ms();
        std::cout << "Components: " << comps.size() << "\n";
        for (size_t i = 0; i < comps.size(); ++i) {
            std::cout << "Component " << (i+1) << ": ";
            for (int v : comps[i]) std::cout << v << " ";
            std::cout << "\n";
        }
        std::cout << "Time (BFS): " << ms << " ms\n";
    } else {
        Timer t1; t1.start(); auto a = cc::ConnectedComponentsDFS(g); long long ms1 = t1.ms();
        Timer t2; t2.start(); auto b = cc::ConnectedComponentsBFS(g); long long ms2 = t2.ms();
        std::cout << "DFS comps: " << a.size() << ", time: " << ms1 << " ms\n";
        std::cout << "BFS comps: " << b.size() << ", time: " << ms2 << " ms\n";

        std::ofstream out("lab3_times.csv");
        out << "algo,time_ms\n";
        out << "DFS," << ms1 << "\n";
        out << "BFS," << ms2 << "\n";
        std::cout << "Saved: lab3_times.csv\n";
    }
}

int main(int argc, char** argv) {
    // быстрый запуск тестов: ./lab3 --test
    if (argc > 1 && (std::strcmp(argv[1], "--test") == 0)) {
        RunGraphTests();
        return 0;
    }

    std::cout << "Lab3: Undirected Graph + Connected Components + Lazy Graph\n";
    for (;;) {
        std::cout << "\nMenu:\n"
                  << "1) Manual graph input & components\n"
                  << "2) Run tests\n"
                  << "3) Compare & save CSV\n"
                  << "4) Visualize (SFML)\n"
                  << "0) Exit\n"
                  << "Choice: ";
        int c; if (!(std::cin >> c)) return 0;

        if (c == 0) return 0;
        else if (c == 1) runManual();
        else if (c == 2) RunGraphTests();
        else if (c == 3) {
            bool ok = false;
            AdjListGraph g = readGraphInteractiveOrAbort(ok);
            if (!ok) continue;
            Timer t1; t1.start(); auto a = cc::ConnectedComponentsDFS(g); long long ms1 = t1.ms();
            Timer t2; t2.start(); auto b = cc::ConnectedComponentsBFS(g); long long ms2 = t2.ms();
            std::cout << "DFS comps: " << a.size() << ", time: " << ms1 << " ms\n";
            std::cout << "BFS comps: " << b.size() << ", time: " << ms2 << " ms\n";

            std::ofstream out("lab3_times.csv");
            out << "algo,time_ms\n";
            out << "DFS," << ms1 << "\n";
            out << "BFS," << ms2 << "\n";
            std::cout << "Saved: lab3_times.csv\n";
        }
        else if (c == 4) {
            bool ok = false;
            AdjListGraph g = readGraphInteractiveOrAbort(ok);
            if (ok) VisualizeGraphSFML(g);
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}
