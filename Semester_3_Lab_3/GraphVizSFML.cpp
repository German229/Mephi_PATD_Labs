// GraphVizSFML.cpp  (SFML 3.x)
#include "GraphVizSFML.h"
#include <cmath>
#include <vector>
#include <string>
#include <iostream>

#ifdef USE_SFML
    #include <SFML/Graphics.hpp>
#endif

// Палитра цветов для компонентов
static
#ifdef USE_SFML
sf::Color
#else
struct { unsigned char r,g,b,a; }  // заглушка, не используется без SFML
#endif
pickColor(int k) {
#ifdef USE_SFML
    static const sf::Color pal[] = {
        sf::Color(0xE6,0x39,0x46), // red
        sf::Color(0xA1,0xD9,0xCE), // mint
        sf::Color(0xFF,0xBE,0x0B), // yellow
        sf::Color(0x43,0xAA,0x8B), // teal
        sf::Color(0x3A,0x86,0xFF), // blue
        sf::Color(0xFB,0x56,0x0A), // orange
        sf::Color(0x9B,0x5D,0xE5), // purple
        sf::Color(0x2A,0x9D,0x8F)  // green
    };
    return pal[k % (int)(sizeof(pal)/sizeof(pal[0]))];
#else
    return {0,0,0,255};
#endif
}

void VisualizeGraphSFML(const AdjListGraph& g) {
#ifndef USE_SFML
    std::cout << "SFML is not enabled (no USE_SFML). Visualization skipped.\n";
    return;
#else
    const int n = g.VerticesCount();
    if (n <= 0) {
        std::cout << "Nothing to visualize: graph has no vertices.\n";
        return;
    }

    // 1) Компоненты связности для раскраски
    auto comps = cc::ConnectedComponentsDFS(g);
    std::vector<int> colorOf(n, 0);
    for (int cid = 0; cid < (int)comps.size(); ++cid) {
        for (int v : comps[cid]) colorOf[v] = cid;
    }

    // 2) Позиции вершин по окружности
    const float W = 1000.f, H = 800.f;
    const sf::Vector2f center(W * 0.5f, H * 0.5f);
    const float R = std::min(W, H) * 0.38f;
    const float PI = 3.14159265358979323846f;

    std::vector<sf::Vector2f> pos(n);
    for (int v = 0; v < n; ++v) {
        const float ang = 2.f * PI * (float)v / std::max(1, n);
        pos[v] = { center.x + R * std::cos(ang), center.y + R * std::sin(ang) };
    }

    // 3) Собираем список рёбер (каждое рисуем один раз)
    std::vector<std::pair<int,int>> edges;
    edges.reserve(n * 4);
    for (int u = 0; u < n; ++u) {
        LinkedList<int> neigh;
        g.GetNeighbors(u, neigh);
        for (int i = 0; i < neigh.GetLength(); ++i) {
            int v = neigh.Get(i);
            if (v > u) edges.emplace_back(u, v);
        }
    }

    // 4) Окно (SFML 3: VideoMode принимает Vector2u)
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u{ (unsigned)W, (unsigned)H }),
        "Lab3 Graph Viewer (SFML)"
    );
    window.setFramerateLimit(60);

    // 5) Шрифт (опционально). Положи DejaVuSans.ttf рядом с бинарником
    sf::Font font;
    const bool hasFont = font.openFromFile("DejaVuSans.ttf");

    // 6) Главный цикл (SFML 3: pollEvent() -> std::optional<Event>)
    while (window.isOpen()) {
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            } else if (auto key = ev->getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                }
            }
        }

        window.clear(sf::Color(0x12,0x12,0x12)); // фон

        // 6.1) Рёбра
        for (auto [u, v] : edges) {
            sf::Vertex seg[2];
            seg[0].position = pos[u];
            seg[1].position = pos[v];
            seg[0].color = sf::Color(180, 180, 180);
            seg[1].color = sf::Color(180, 180, 180);
            window.draw(seg, 2, sf::PrimitiveType::Lines);
        }

        // 6.2) Вершины
        const float rad = (n <= 50) ? 18.f : std::max(6.f, 18.f - 0.15f*(n-50));
        for (int v = 0; v < n; ++v) {
            sf::CircleShape circle(rad);
            circle.setOrigin(sf::Vector2f{ rad, rad });
            circle.setPosition(pos[v]);
            circle.setFillColor(pickColor(colorOf[v]));
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(1.5f);
            window.draw(circle);

            if (hasFont) {
                // SFML 3: конструктор Text(font, string, size)
                sf::Text t(font, std::to_string(v), (unsigned)std::round(rad * 1.3f));
                t.setFillColor(sf::Color::Black);
                // В SFML 3 setPosition принимает один Vector2f
                t.setPosition(sf::Vector2f{
                    pos[v].x - rad * 0.5f,
                    pos[v].y - rad * 0.65f
                });
                window.draw(t);
            }
        }

        window.display();
    }
#endif
}