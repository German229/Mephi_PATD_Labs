// GraphVizSFML.cpp  (для SFML 3.x)
#include "GraphVizSFML.h"
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>

#ifdef USE_SFML
  #include <SFML/Graphics.hpp>
  #include <queue>
#endif

// ---------- палитра цветов для компонент ----------
static
#ifdef USE_SFML
sf::Color
#else
struct { unsigned char r,g,b,a; }
#endif
pickColor(int k) {
#ifdef USE_SFML
    static const sf::Color pal[] = {
        sf::Color(0xE6,0x39,0x46), sf::Color(0xA1,0xD9,0xCE),
        sf::Color(0xFF,0xBE,0x0B), sf::Color(0x43,0xAA,0x8B),
        sf::Color(0x3A,0x86,0xFF), sf::Color(0xFB,0x56,0x0A),
        sf::Color(0x9B,0x5D,0xE5), sf::Color(0x2A,0x9D,0x8F)
    };
    return pal[k % (int)(sizeof(pal)/sizeof(pal[0]))];
#else
    return {0,0,0,255};
#endif
}

#ifdef USE_SFML
// ---------- тихая загрузка шрифта (без логов) ----------
static bool LoadAnyFont(sf::Font& font) {
    // Стабильный системный шрифт на macOS. Можно заменить на локальный .ttf рядом с бинарником.
    const char* p = "/System/Library/Fonts/Supplemental/Arial Unicode.ttf";
    return font.openFromFile(p);
}

static inline long long edgeKey(int u, int v) {
    if (u > v) std::swap(u, v);
    return (static_cast<long long>(u) << 32) | static_cast<unsigned>(v);
}

// ---------- helper: получить отсортированный список соседей вершины ----------
static std::vector<int> NeighSorted(const AdjListGraph& g, int u) {
    LinkedList<int> L; g.GetNeighbors(u, L);
    std::vector<int> v; v.reserve(L.GetLength());
    for (int i = 0; i < L.GetLength(); ++i) v.push_back(L.Get(i));
    std::sort(v.begin(), v.end());
    return v;
}

// ---------- построить overlay для BFS: порядок вершин и нумерация "деревянных" рёбер ----------
static void BuildBFSOverlay(const AdjListGraph& g,
                            std::vector<int>& vertexOrder,
                            std::unordered_map<long long,int>& treeEdgeNumber)
{
    const int n = g.VerticesCount();
    vertexOrder.clear(); vertexOrder.reserve(n);
    treeEdgeNumber.clear();
    std::vector<char> used(n, false);
    std::queue<int> q;
    int eid = 1;

    for (int s = 0; s < n; ++s) {
        if (used[s]) continue;
        used[s] = true;
        q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            vertexOrder.push_back(u);

            auto neigh = NeighSorted(g, u);
            for (int v : neigh) {
                if (!used[v]) {
                    used[v] = true;
                    q.push(v);
                    treeEdgeNumber[edgeKey(u, v)] = eid++; // ребро, по которому впервые пришли в v
                }
            }
        }
    }
}

// ---------- построить overlay для DFS (итеративно) ----------
static void BuildDFSOverlay(const AdjListGraph& g,
                            std::vector<int>& vertexOrder,
                            std::unordered_map<long long,int>& treeEdgeNumber)
{
    const int n = g.VerticesCount();
    vertexOrder.clear(); vertexOrder.reserve(n);
    treeEdgeNumber.clear();
    std::vector<char> used(n, false);
    std::vector<int> it(n, 0);           // индекс следующего соседа
    int eid = 1;

    for (int s = 0; s < n; ++s) {
        if (used[s]) continue;

        // для стабильности заранее возьмём отсорт. списки соседей
        std::vector<std::vector<int>> adj(n);
        // (лишь раз на компоненту строить дороговато; упростим — строим на всю верш. область)
        // но это O(n * deg) приемлемо при учебных размерах
        for (int u = 0; u < n; ++u) adj[u] = NeighSorted(g, u);

        std::vector<int> st;
        st.push_back(s);

        while (!st.empty()) {
            int u = st.back();

            if (!used[u]) {
                used[u] = true;
                vertexOrder.push_back(u);
            }

            // найти первого непосещённого соседа
            while (it[u] < (int)adj[u].size() && used[adj[u][it[u]]]) ++it[u];

            if (it[u] == (int)adj[u].size()) {
                st.pop_back();
                continue;
            }

            int v = adj[u][it[u]++]; // забираем соседа
            if (!used[v]) {
                treeEdgeNumber[edgeKey(u, v)] = eid++; // дерево DFS
                st.push_back(v);
            }
        }
    }
}
#endif // USE_SFML

// ---------- внутренняя реализация ----------
#ifdef USE_SFML
static void VisualizeImpl(const AdjListGraph& g, int initialOverlay, bool showEdgeLabelsInit) {
    const int n = g.VerticesCount();
    if (n <= 0) { std::cout << "Nothing to visualize.\n"; return; }

    // 1) Компоненты связности → цвет вершины
    auto comps = cc::ConnectedComponentsDFS(g);
    std::vector<int> colorOf(n, 0);
    for (int cid = 0; cid < (int)comps.size(); ++cid)
        for (int v : comps[cid]) colorOf[v] = cid;

    // 2) Геометрия по окружности
    const float W = 1000.f, H = 800.f;
    const sf::Vector2f center(W * 0.5f, H * 0.5f);
    const float R = std::min(W, H) * 0.38f;
    const float PI = 3.14159265358979323846f;

    std::vector<sf::Vector2f> pos(n);
    for (int v = 0; v < n; ++v) {
        float ang = 2.f * PI * (float)v / std::max(1, n);
        pos[v] = { center.x + R * std::cos(ang), center.y + R * std::sin(ang) };
    }

    // 3) Рёбра (u < v)
    std::vector<std::pair<int,int>> edges;
    edges.reserve(n * 2);
    for (int u = 0; u < n; ++u) {
        LinkedList<int> neigh; g.GetNeighbors(u, neigh);
        for (int i = 0; i < neigh.GetLength(); ++i) {
            int v = neigh.Get(i);
            if (v > u) edges.emplace_back(u, v);
        }
    }

    // 4) Окно и шрифт
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{ (unsigned)W, (unsigned)H }),
        "Lab3 Graph Viewer (SFML)"
    );
    window.setFramerateLimit(60);

    sf::Font font;
    const bool hasFont = LoadAnyFont(font);

    // 5) Состояние overlay
    bool showOrder = false;
    std::vector<int> vertexOrder;               // последовательность посещённых вершин
    std::unordered_map<long long,int> edgeOrd;  // edgeKey -> порядковый номер (1..)
    // состояние подписи рёбер
    bool showEdgeLabels = showEdgeLabelsInit;

    auto applyOverlay = [&](int mode){
        if (mode == 1) {
            BuildBFSOverlay(g, vertexOrder, edgeOrd);
            showOrder = true;
        } else if (mode == 2) {
            BuildDFSOverlay(g, vertexOrder, edgeOrd);
            showOrder = true;
        } else {
            vertexOrder.clear();
            edgeOrd.clear();
            showOrder = false;
        }
    };
    if (initialOverlay < 0 || initialOverlay > 2) initialOverlay = 0;
    applyOverlay(initialOverlay);

    // 6) Главный цикл
    while (window.isOpen()) {
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            } else if (auto key = ev->getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                } else if (key->scancode == sf::Keyboard::Scancode::B) {
                    applyOverlay(1);
                } else if (key->scancode == sf::Keyboard::Scancode::D) {
                    applyOverlay(2);
                } else if (key->scancode == sf::Keyboard::Scancode::C) {
                    applyOverlay(0);
                } else if (key->scancode == sf::Keyboard::Scancode::E) {
                    showEdgeLabels = !showEdgeLabels;
                }
            }
        }

        window.clear(sf::Color(0x12,0x12,0x12));

        // 6.1) Рёбра (+ подписи)
        for (auto [u, v] : edges) {
            const bool isTreeEdge = showOrder && (edgeOrd.find(edgeKey(u, v)) != edgeOrd.end());

            sf::Vertex seg[2];
            seg[0].position = pos[u]; seg[1].position = pos[v];
            // дерево — поярче, небазовые — серые
            seg[0].color = seg[1].color = isTreeEdge ? sf::Color(230,230,230)
                                                     : sf::Color(160,160,160);
            window.draw(seg, 2, sf::PrimitiveType::Lines);

            if (hasFont && showEdgeLabels) {
                // позиция подписи: середина + смещение по нормали
                sf::Vector2f mid{ (pos[u].x + pos[v].x) * 0.5f, (pos[u].y + pos[v].y) * 0.5f };
                sf::Vector2f d{ pos[v].x - pos[u].x, pos[v].y - pos[u].y };
                float len = std::sqrt(d.x*d.x + d.y*d.y);
                sf::Vector2f nrm{ 0.f, 0.f };
                if (len > 1e-6f) nrm = sf::Vector2f{ -d.y / len, d.x / len };
                float offset = 10.f;
                sf::Vector2f labelPos{ mid.x + nrm.x * offset, mid.y + nrm.y * offset };

                std::string text;
                if (showOrder) {
                    auto it = edgeOrd.find(edgeKey(u, v));
                    if (it != edgeOrd.end()) text = std::to_string(it->second); // номер ребра по порядку
                    else text.clear(); // не дерево — без подписи
                } else {
                    text = std::to_string(u) + "-" + std::to_string(v); // overlay=none
                }

                if (!text.empty()) {
                    sf::Text eText(font, text, 14u);
                    eText.setFillColor(sf::Color(220,220,220));
                    eText.setPosition(labelPos);
                    window.draw(eText);
                }
            }
        }

        // 6.2) Вершины + номера и (если overlay) ранги
        const float rad = (n <= 50) ? 18.f : std::max(6.f, 18.f - 0.15f*(n-50));

        // подготовим ранги (vertex -> порядковый номер посещения), если надо
        std::vector<int> rank;
        if (showOrder) {
            rank.assign(n, -1);
            for (int i = 0; i < (int)vertexOrder.size(); ++i) rank[vertexOrder[i]] = i;
        }

        for (int v = 0; v < n; ++v) {
            // кружок
            sf::CircleShape circle(rad);
            circle.setOrigin(sf::Vector2f{ rad, rad });
            circle.setPosition(pos[v]);
            circle.setFillColor(pickColor(colorOf[v]));
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(1.5f);
            window.draw(circle);

            if (hasFont) {
                // основной номер вершины (по центру)
                sf::Text t(font, std::to_string(v), (unsigned)std::round(rad * 1.1f));
                t.setFillColor(sf::Color::Black);
                t.setPosition(sf::Vector2f{
                    pos[v].x - rad * 0.5f,
                    pos[v].y - rad * 0.65f
                });
                window.draw(t);

                // маленький номер "ранга" (порядок посещения) при overlay
                if (showOrder && !rank.empty() && rank[v] != -1) {
                    sf::Text small(font, std::to_string(rank[v]), (unsigned)std::round(rad * 0.8f));
                    small.setFillColor(sf::Color::White);
                    small.setPosition(sf::Vector2f{
                        pos[v].x + rad * 0.55f,
                        pos[v].y + rad * 0.20f
                    });
                    window.draw(small);
                }
            }
        }

        window.display();
    }
}
#endif // USE_SFML

// ---------- публичные обёртки ----------
void VisualizeGraphSFML(const AdjListGraph& g) {
#ifndef USE_SFML
    std::cout << "SFML is not enabled (no USE_SFML). Visualization skipped.\n";
#else
    VisualizeImpl(g, 0, false);
#endif
}

void VisualizeGraphSFML(const AdjListGraph& g, int initialOverlay) {
#ifndef USE_SFML
    std::cout << "SFML is not enabled (no USE_SFML). Visualization skipped.\n";
#else
    if (initialOverlay < 0 || initialOverlay > 2) initialOverlay = 0;
    VisualizeImpl(g, initialOverlay, false);
#endif
}

void VisualizeGraphSFML(const AdjListGraph& g, int initialOverlay, bool showEdgeLabels) {
#ifndef USE_SFML
    std::cout << "SFML is not enabled (no USE_SFML). Visualization skipped.\n";
#else
    if (initialOverlay < 0 || initialOverlay > 2) initialOverlay = 0;
    VisualizeImpl(g, initialOverlay, showEdgeLabels);
#endif
}
