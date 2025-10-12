#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
#include "Board.hpp"
#include "AI.hpp"
#include "Tests.hpp"
#include "GUI.hpp"

// -------- тесты --------
static void runTests() {
    try {
        TestBoardBasics();
        TestWinDetection();
        TestAIBlockAndWin();
        TestAIConsistency();
        std::cout << "\nAll tests passed successfully.\n\n";
    } catch (const std::exception& ex) {
        std::cerr << "Test failed: " << ex.what() << "\n";
    }
}

static void printHelp() {
    std::cout <<
      "Команды:\n"
      "  x y        — поставить X в клетку (x,y)\n"
      "  hint       — подсказка лучшего хода для X\n"
      "  bench      — сравнить Minimax vs Alpha-Beta для текущего хода (чья очередь)\n"
      "  depth N    — установить глубину поиска AI = N\n"
      "  mode ab    — включить Alpha-Beta\n"
      "  mode min   — включить чистый Minimax\n"
      "  ui         — запустить графический интерфейс (SFML)\n"
      "  print      — вывести текущее поле\n"
      "  help       — показать это сообщение\n"
      "  quit       — выход\n";
}

int main(int argc, char** argv) {
    // 1) Юнит-тесты
    runTests();

    // 2) Игра: X — человек, O — AI
    Board board;
    AI ai;
    ai.maxDepth = 8;
    ai.useAlphaBeta = true;

    // Быстрый запуск GUI, если аргумент --ui
    if (argc > 1 && std::string(argv[1]) == "--ui") {
        GuiApp app;
        bool finished = app.run(board, ai, 3);
        if (finished) {
            std::cout << "[GUI] Партия завершена. Выход.\n";
            return 0;
        }
        // если не финал — вернёмся в консольный режим:
        std::cout << "[GUI] Возврат в консольный режим.\n";
    }

    char turn = 'X';
    std::cout << "=== Крестики-нолики на бесконечном поле ===\n";
    std::cout << "Игрок X — вы. Игрок O — AI.\n";
    printHelp();
    std::cout << "\n";

    std::string line;
    while (true) {
        board.Print();

        if (turn == 'X') {
            std::cout << "[X] Ваш ход (x y | команда): ";
            if (!std::getline(std::cin, line)) { std::cout << "\nВыход.\n"; break; }
            if (line.empty()) continue;

            std::istringstream iss(line);
            int x, y;
            if (iss >> x >> y) {
                if (!board.IsCellEmpty(x, y)) {
                    std::cout << "Клетка занята. Попробуйте снова.\n";
                    continue;
                }
                board.PlaceMove(x, y, 'X');
                if (board.CheckWin(x, y)) {
                    board.Print();
                    std::cout << "Победил X!\n";
                    break;
                }
                turn = 'O';
                continue;
            }

            // команды
            if (line == "quit") break;
            if (line == "help") { printHelp(); continue; }
            if (line == "print") { board.Print(); continue; }

            if (line == "ui") {
                GuiApp app;
                bool finished = app.run(board, ai, 3); // GUI ведёт ту же партию
                std::cout << "[GUI] Возврат в консольный режим.\n";
                board.Print(); // покажем актуальное поле после GUI
                if (finished) {
                    std::cout << "Партия завершена. Выход.\n";
                    break;  // ← НИЧЕГО БОЛЬШЕ НЕ СПРАШИВАЕМ
                }
                continue;
            }

            if (line == "hint") {
                AI hintAI = ai; // те же настройки
                auto t1 = std::chrono::high_resolution_clock::now();
                auto mv = hintAI.FindBestMove(board, 'X');
                auto t2 = std::chrono::high_resolution_clock::now();
                auto dur = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
                std::cout << "[HINT] Лучший ход для X: (" << mv.x << "," << mv.y << "), score=" << mv.score
                          << ", time=" << dur << "us, "
                          << (hintAI.useAlphaBeta ? "AB nodes=" + std::to_string(hintAI.lastStatsAlpha.nodes)
                                                  : "Min nodes=" + std::to_string(hintAI.lastStatsMinimax.nodes))
                          << "\n";
                continue;
            }

            if (line == "bench") {
                AI a1 = ai; a1.useAlphaBeta = false;
                AI a2 = ai; a2.useAlphaBeta = true;

                auto t1 = std::chrono::high_resolution_clock::now();
                auto m1 = a1.FindBestMove(board, 'X');
                auto t2 = std::chrono::high_resolution_clock::now();
                auto m2 = a2.FindBestMove(board, 'X');
                auto t3 = std::chrono::high_resolution_clock::now();

                auto d1 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
                auto d2 = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();

                std::cout << "Minimax:   (" << m1.x << "," << m1.y << "), score=" << m1.score
                          << ", time=" << d1 << "us, nodes=" << a1.lastStatsMinimax.nodes << "\n";
                std::cout << "AlphaBeta: (" << m2.x << "," << m2.y << "), score=" << m2.score
                          << ", time=" << d2 << "us, nodes=" << a2.lastStatsAlpha.nodes << "\n";
                continue;
            }

            if (line.rfind("depth ", 0) == 0) {
                int d = std::max(1, std::atoi(line.substr(6).c_str()));
                ai.maxDepth = d;
                std::cout << "maxDepth = " << ai.maxDepth << "\n";
                continue;
            }

            if (line == "mode ab") {
                ai.useAlphaBeta = true;
                std::cout << "Режим: Alpha-Beta\n";
                continue;
            }
            if (line == "mode min") {
                ai.useAlphaBeta = false;
                std::cout << "Режим: Minimax\n";
                continue;
            }

            std::cout << "Неизвестная команда. Введите 'help'.\n";
        } else {
            std::cout << "[O] Ходит AI (" << (ai.useAlphaBeta ? "Alpha-Beta" : "Minimax")
                      << ", depth=" << ai.maxDepth << ")...\n";

            auto t1 = std::chrono::high_resolution_clock::now();
            auto best = ai.FindBestMove(board, 'O');
            auto t2 = std::chrono::high_resolution_clock::now();
            auto dur = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

            std::cout << "AI: (" << best.x << "," << best.y << "), score=" << best.score
                      << ", time=" << dur << "us, "
                      << (ai.useAlphaBeta ? "nodes=" + std::to_string(ai.lastStatsAlpha.nodes)
                                          : "nodes=" + std::to_string(ai.lastStatsMinimax.nodes))
                      << "\n";

            int x = best.x, y = best.y;
            if (!board.IsCellEmpty(x, y)) {
                bool placed = false;
                for (int yy = board.MinY() - 1; yy <= board.MaxY() + 1 && !placed; ++yy) {
                    for (int xx = board.MinX() - 1; xx <= board.MaxX() + 1 && !placed; ++xx) {
                        if (board.IsCellEmpty(xx, yy)) { x = xx; y = yy; placed = true; }
                    }
                }
            }

            board.PlaceMove(x, y, 'O');
            std::cout << "[O] AI сходил в (" << x << "," << y << ")\n";
            if (board.CheckWin(x, y)) {
                board.Print();
                std::cout << "Победил O!\n";
                break;
            }
            turn = 'X';
        }
    }

    return 0;
}
