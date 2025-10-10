#include <iostream>
#include <chrono>
#include "Board.hpp"
#include "AI.hpp"
#include "Tests.hpp"

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

int main() {
    // 1) Юнит-тесты
    runTests();

    // 2) Игровой цикл: X — человек, O — AI
    Board board;
    AI ai;
    ai.maxDepth = 8;

    char turn = 'X';
    std::cout << "=== Крестики-нолики на бесконечном поле ===\n";
    std::cout << "Игрок X — вы. Игрок O — AI.\n";
    std::cout << "Введите координаты хода: x y (например: 0 0). Ctrl+D/Ctrl+Z — выйти.\n\n";

    while (true) {
        board.Print();

        if (turn == 'X') {
            std::cout << "[X] Ваш ход (x y): ";
            int x, y;
            if (!(std::cin >> x >> y)) {
                std::cout << "\nВыход.\n";
                break;
            }
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
        } else {
            std::cout << "[O] Ходит AI... (сравниваем Minimax vs Alpha-Beta)\n";

            // Рекомендация/ход AI c замером времени
            auto t1 = std::chrono::high_resolution_clock::now();
            auto best1 = ai.FindBestMoveMinimax(board, 'O');
            auto t2 = std::chrono::high_resolution_clock::now();
            auto best2 = ai.FindBestMoveAlphaBeta(board, 'O');
            auto t3 = std::chrono::high_resolution_clock::now();

            auto dur1 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            auto dur2 = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();

            std::cout << "Minimax: (" << best1.x << "," << best1.y << "), score=" << best1.score
                      << ", time=" << dur1 << "us\n";
            std::cout << "Alpha-Beta: (" << best2.x << "," << best2.y << "), score=" << best2.score
                      << ", time=" << dur2 << "us\n";

            // используем ход альфа-бета
            int x = best2.x, y = best2.y;
            if (!board.IsCellEmpty(x, y)) {
                // на всякий случай fallback: возьмём minimax
                x = best1.x; y = best1.y;
            }
            if (!board.IsCellEmpty(x, y)) {
                // если вдруг и это занято (не должно), ищем первое пустое место в окне
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