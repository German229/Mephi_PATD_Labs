// main.cpp
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <string>
#include <queue>
#include <filesystem>

#include "Timer.h"
#include "Graphs.h"
#include "ConnectedComponents.h"
#include "GraphVizSFML.h"

// из Tests_Graph.cpp
extern "C" void RunGraphTests();

// --------- путь к каталогу исходников лабы (где лежит этот main.cpp) ---------
static std::filesystem::path Lab3SourceDir() {
    return std::filesystem::path(__FILE__).parent_path();
}

// ---------------------- CSV: только список рёбер ----------------------
static void SaveEdgesCSV(const AdjListGraph& g, const std::filesystem::path& filepath) {
    std::ofstream out(filepath);
    out << "u,v\n";
    const int n = g.VerticesCount();
    for (int u = 0; u < n; ++u) {
        LinkedList<int> L; g.GetNeighbors(u, L);
        for (int i = 0; i < L.GetLength(); ++i) {
            int v = L.Get(i);
            if (v > u) out << u << "," << v << "\n"; // только u<v, без дублей
        }
    }
}

// ---------------------- ввод графа ----------------------
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
            g.AddEdge(u, v);  // кидает исключение для петли/дубликата/индексов вне диапазона
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

// ---------------------- режимы ----------------------
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
        // сравнение времени и сохранение только times в graphs_csv
        Timer t1; t1.start(); auto a = cc::ConnectedComponentsDFS(g); long long ms1 = t1.ms();
        Timer t2; t2.start(); auto b = cc::ConnectedComponentsBFS(g); long long ms2 = t2.ms();

        std::filesystem::path outdir = Lab3SourceDir() / "graphs_csv";
        std::error_code ec; std::filesystem::create_directories(outdir, ec);

        std::ofstream out(outdir / "lab3_times.csv");
        out << "algo,time_ms\n";
        out << "DFS," << ms1 << "\n";
        out << "BFS," << ms2 << "\n";

        std::cout << "DFS comps: " << a.size() << ", time: " << ms1 << " ms\n";
        std::cout << "BFS comps: " << b.size() << ", time: " << ms2 << " ms\n";
        std::cout << "Saved: " << std::filesystem::absolute(outdir / "lab3_times.csv") << "\n";
    }
}

static void runExportEdgesOnly() {
    bool ok = false;
    AdjListGraph g = readGraphInteractiveOrAbort(ok);
    if (!ok) return;

    // каталог graphs_csv рядом с исходниками лабы
    std::filesystem::path outdir = Lab3SourceDir() / "graphs_csv";
    std::error_code ec;
    std::filesystem::create_directories(outdir, ec);

    auto path = outdir / "lab3_graph.csv";
    SaveEdgesCSV(g, path);

    std::cout << "Saved edges CSV: " << std::filesystem::absolute(path) << "\n";
}

int main(int argc, char** argv) {
    if (argc > 1 && (std::strcmp(argv[1], "--test") == 0)) {
        RunGraphTests();
        return 0;
    }

    std::cout << "Lab3: Undirected Graph + Connected Components + Lazy Graph\n";
    for (;;) {
        std::cout << "\nMenu:\n"
                  << "1) Manual graph input & components\n"
                  << "2) Run tests\n"
                  << "3) Compare\n"
                  << "4) Visualize (SFML)\n"
                  << "5) Export graph (edges CSV -> Semester_3_Lab_3/graphs_csv)\n"
                  << "0) Exit\n"
                  << "Choice: ";
        int c; if (!(std::cin >> c)) return 0;

        if (c == 0) return 0;
        else if (c == 1) runManual();
        else if (c == 2) RunGraphTests();
        else if (c == 3) {
            runManual(); // внутри 3-й ветки сохранит только lab3_times.csv при выборе "3"
        }
        else if (c == 4) {
            bool ok = false;
            AdjListGraph g = readGraphInteractiveOrAbort(ok);
            if (!ok) continue;

            std::cout << "Initial overlay (0/none/1/BFS/2/DFS): ";
            std::string s; std::cin >> s;
            for (auto& ch : s) ch = (char)std::tolower((unsigned char)ch);
            int overlay = (s == "1" || s == "bfs") ? 1 : (s == "2" || s == "dfs") ? 2 : 0;

            std::cout << "Edge labels? (y/n): ";
            char yn; std::cin >> yn;
            bool showEdges = (yn == 'y' || yn == 'Y');

            VisualizeGraphSFML(g, overlay, showEdges);
        }
        else if (c == 5) {
            runExportEdgesOnly(); // только один CSV с рёбрами
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}
