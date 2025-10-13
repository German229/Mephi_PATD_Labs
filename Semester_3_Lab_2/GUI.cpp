#include "GUI.hpp"
#include <string>
#include <sstream>
#include <chrono>
#include <array>
#include <iostream>
#include <filesystem>

#ifdef USE_SFML
  #include <SFML/Graphics.hpp>
  #include <SFML/System/Angle.hpp>
#endif

// --- Проверка победы на K в ряд ---
static bool checkWinK(const Board& b, int x, int y, int K) {
    auto countDir = [&](int dx, int dy)->int {
        char s = b.GetCell(x, y);
        if (s != 'X' && s != 'O') return 0;
        int cnt = 1;
        int cx = x + dx, cy = y + dy;
        while (b.GetCell(cx, cy) == s) { cnt++; cx += dx; cy += dy; }
        cx = x - dx; cy = y - dy;
        while (b.GetCell(cx, cy) == s) { cnt++; cx -= dx; cy -= dy; }
        return cnt;
    };
    static const int dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
    for (auto& d : dirs) if (countDir(d[0], d[1]) >= K) return true;
    return false;
}

bool GuiApp::run(Board& board, AI& ai, int winK) {
#ifndef USE_SFML
    (void)board; (void)ai; (void)winK;
    std::cout << "[GUI] Соберите с USE_SFML и подключите SFML, чтобы использовать графический режим.\n";
    return false; // партия не завершена
#else
    // --- Настройки визуализации ---
    const int CELL   = 64;
    const int PAD    = 1;
    const int HUD_H  = 36;
    const int W_PIX  = 800;
    const int H_PIX  = 800 + HUD_H;

    const sf::Color gridColor(200,200,200);
    const sf::Color bgColor(245,245,245);
    const sf::Color xColor(30,30,30);
    const sf::Color oColor(20,120,220);

    const int savedDepth = ai.maxDepth;
    ai.maxDepth = std::min(savedDepth, 6);

    auto getBounds = [&](){
        int minX = board.MinX(), maxX = board.MaxX();
        int minY = board.MinY(), maxY = board.MaxY();
        if (minX > maxX) { minX = maxX = 0; minY = maxY = 0; }
        return std::tuple<int,int,int,int>(minX - PAD, maxX + PAD, minY - PAD, maxY + PAD);
    };

    int minX, maxX, minY, maxY;
    std::tie(minX, maxX, minY, maxY) = getBounds();

    const int colsFit = W_PIX / CELL;
    const int rowsFit = (H_PIX - HUD_H) / CELL;

    auto clampView = [&](){
        int needCols = (maxX - minX + 1);
        int needRows = (maxY - minY + 1);
        int cx = (minX + maxX) / 2;
        int cy = (minY + maxY) / 2;
        int vx = cx - colsFit / 2;
        int vy = cy - rowsFit / 2;
        if (needCols + 2*PAD <= colsFit) { vx = minX - (colsFit - needCols) / 2; }
        if (needRows + 2*PAD <= rowsFit) { vy = minY - (rowsFit - needRows) / 2; }
        return std::pair<int,int>(vx, vy);
    };

    int viewX, viewY;
    std::tie(viewX, viewY) = clampView();

    sf::RenderWindow window(
        sf::VideoMode({ static_cast<unsigned>(W_PIX), static_cast<unsigned>(H_PIX) }),
        "Infinite Tic-Tac-Toe"
    );
    window.setFramerateLimit(60);

    // ===== ШРИФТ и надпись победителя =====
    sf::Font font;
    bool fontOK = false;
    const std::array<const char*,4> fontCandidates = {
        "DejaVuSans.ttf",
        "/Library/Fonts/Arial Unicode.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/SFNS.ttf"
    };
    for (auto* p : fontCandidates) {
        std::error_code ec;
        if (std::filesystem::exists(p, ec) && font.openFromFile(p)) { fontOK = true; break; }
    }

    auto makeCenteredText = [&](const std::string& s, unsigned size)->sf::Text {
        sf::Text t(font, s, size);
        t.setFillColor(sf::Color::Black);
        const sf::FloatRect bounds = t.getLocalBounds();
        const sf::Vector2f center(
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
        );
        t.setOrigin(center);
        t.setPosition(sf::Vector2f(W_PIX/2.f, (rowsFit*CELL)/2.f));
        return t;
    };

    auto drawWinOverlay = [&](const std::string& msg){
        sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(W_PIX),
                                                static_cast<float>(rowsFit*CELL)));
        overlay.setFillColor(sf::Color(255,255,255,200));
        overlay.setPosition(sf::Vector2f(0.f, 0.f));
        window.draw(overlay);
        if (fontOK) {
            auto text = makeCenteredText(msg, 52);
            window.draw(text);
            auto sub = makeCenteredText("Press Esc to exit", 22);
            sub.setPosition(sf::Vector2f(W_PIX/2.f, (rowsFit*CELL)/2.f + 50.f));
            window.draw(sub);
        }
    };

    auto drawGrid = [&](){
        window.clear(bgColor);

        for (int c = 0; c <= colsFit; ++c) {
            sf::RectangleShape v(sf::Vector2f(1.f, static_cast<float>(rowsFit * CELL)));
            v.setFillColor(gridColor);
            v.setPosition(sf::Vector2f(static_cast<float>(c * CELL), 0.f));
            window.draw(v);
        }
        for (int r = 0; r <= rowsFit; ++r) {
            sf::RectangleShape h(sf::Vector2f(static_cast<float>(colsFit * CELL), 1.f));
            h.setFillColor(gridColor);
            h.setPosition(sf::Vector2f(0.f, static_cast<float>(r * CELL)));
            window.draw(h);
        }

        for (int gy = viewY; gy < viewY + rowsFit; ++gy) {
            for (int gx = viewX; gx < viewX + colsFit; ++gx) {
                char ch = board.GetCell(gx, gy);
                if (ch == 'X') {
                    sf::RectangleShape a(sf::Vector2f(static_cast<float>(CELL - 16), 3.f));
                    sf::RectangleShape b(sf::Vector2f(static_cast<float>(CELL - 16), 3.f));
                    a.setFillColor(xColor); b.setFillColor(xColor);
                    a.setOrigin(sf::Vector2f((CELL - 16) / 2.f, 1.5f));
                    b.setOrigin(sf::Vector2f((CELL - 16) / 2.f, 1.5f));
                    const sf::Vector2f center(
                        (gx - viewX) * CELL + CELL / 2.f,
                        (viewY + rowsFit - 1 - gy) * CELL + CELL / 2.f
                    );
                    a.setPosition(center);
                    b.setPosition(center);
                    a.setRotation(sf::degrees(45.f));
                    b.setRotation(sf::degrees(-45.f));
                    window.draw(a); window.draw(b);
                } else if (ch == 'O') {
                    sf::CircleShape circ((CELL - 18) / 2.f);
                    circ.setOutlineThickness(3.f);
                    circ.setOutlineColor(oColor);
                    circ.setFillColor(sf::Color::Transparent);
                    circ.setPosition(sf::Vector2f(
                        (gx - viewX) * CELL + 9.f,
                        (viewY + rowsFit - 1 - gy) * CELL + 9.f
                    ));
                    window.draw(circ);
                }
            }
        }

        sf::RectangleShape hudBg(sf::Vector2f(static_cast<float>(colsFit * CELL), static_cast<float>(HUD_H)));
        hudBg.setFillColor(sf::Color(235,235,235));
        hudBg.setPosition(sf::Vector2f(0.f, static_cast<float>(rowsFit * CELL)));
        window.draw(hudBg);
    };

    char turn = 'X';
    bool finished = false;
    bool gameFinished = false;   // <-- что вернём в консоль
    std::string winMsg;

    std::cout << "[GUI] Открыто окно. Глубина AI=" << ai.maxDepth
              << " (" << (ai.useAlphaBeta ? "Alpha-Beta" : "Minimax") << ")\n";

    while (window.isOpen()) {
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
                ai.maxDepth = savedDepth;
                std::cout << "[GUI] Окно закрыто пользователем.\n";
                return gameFinished; // если была победа — вернём true
            }
            if (auto key = ev->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    window.close();
                    ai.maxDepth = savedDepth;
                    std::cout << "[GUI] Закрыто по Esc.\n";
                    return gameFinished;
                }
                if (!finished) {
                    if (key->code == sf::Keyboard::Key::Left)  viewX -= 1;
                    if (key->code == sf::Keyboard::Key::Right) viewX += 1;
                    if (key->code == sf::Keyboard::Key::Up)    viewY += 1;
                    if (key->code == sf::Keyboard::Key::Down)  viewY -= 1;
                }
            }

            if (finished) continue;

            if (turn == 'X') {
                if (auto mb = ev->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mb->button == sf::Mouse::Button::Left) {
                        int mx = mb->position.x;
                        int my = mb->position.y;
                        if (my < rowsFit * CELL) {
                            int gx = viewX + mx / CELL;
                            int gy = viewY + (rowsFit - 1 - my / CELL);
                            if (board.IsCellEmpty(gx, gy)) {
                                board.PlaceMove(gx, gy, 'X');
                                std::cout << "[GUI] X -> (" << gx << "," << gy << ")\n";
                                if (checkWinK(board, gx, gy, winK)) {
                                    finished = true;
                                    gameFinished = true;
                                    winMsg = "X wins!";
                                    std::cout << "[GUI] X wins!\n";
                                } else {
                                    turn = 'O';
                                }
                                std::tie(minX, maxX, minY, maxY) = getBounds();
                                std::tie(viewX, viewY) = clampView();
                            }
                        }
                    }
                }
            }
        }

        drawGrid();

        if (finished) {
            drawWinOverlay(winMsg);
            window.display();
            continue;
        }

        if (turn == 'O') {
            AIMove best = ai.FindBestMove(board, 'O');

            int x = best.x, y = best.y;
            if (!board.IsCellEmpty(x, y)) {
                bool placed = false;
                for (int yy = board.MinY() - 1; yy <= board.MaxY() + 1 && !placed; ++yy)
                    for (int xx = board.MinX() - 1; xx <= board.MaxX() + 1 && !placed; ++xx)
                        if (board.IsCellEmpty(xx, yy)) { x = xx; y = yy; placed = true; }
            }
            board.PlaceMove(x, y, 'O');
            std::cout << "[GUI] O -> (" << x << "," << y << ")\n";
            if (checkWinK(board, x, y, winK)) {
                drawGrid();
                winMsg = "O wins!";
                drawWinOverlay(winMsg);
                window.display();
                std::cout << "[GUI] O wins!\n";
                // ждём закрытия / Esc
                while (window.isOpen()) {
                    if (auto e = window.pollEvent()) {
                        if (e->is<sf::Event::Closed>()) { window.close(); ai.maxDepth = savedDepth; std::cout << "[GUI] Окно закрыто.\n"; return true; }
                        if (auto k = e->getIf<sf::Event::KeyPressed>()) {
                            if (k->code == sf::Keyboard::Key::Escape) { window.close(); ai.maxDepth = savedDepth; std::cout << "[GUI] Закрыто по Esc.\n"; return true; }
                        }
                    }
                }
                ai.maxDepth = savedDepth;
                return true;
            }
            turn = 'X';
            std::tie(minX, maxX, minY, maxY) = getBounds();
            std::tie(viewX, viewY) = clampView();
        }

        window.display();
    }

    ai.maxDepth = savedDepth;
    std::cout << "[GUI] Окно закрыто (выход из цикла).\n";
    return gameFinished; // сообщим консоли, была ли победа
#endif
}
