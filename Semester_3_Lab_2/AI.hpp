#pragma once
#include <vector>
#include <utility>
#include <cstdint>
#include "Board.hpp"

struct AIMove {
    int x;
    int y;
    int score; // оценка с точки зрения X (X — maximize, O — minimize)
};

struct AIStats {
    long long nodes = 0; // число посещённых позиций при последнем вызове
};

class AI {
public:
    // Параметры поиска
    int  maxDepth = 9;
    bool useAlphaBeta = true; // true -> Alpha-Beta, false -> чистый Minimax

    // Параметры производительности
    int candidateMargin = 1;   // рамка генерации вокруг занятой области (±margin)
    int maxCandidates   = 16;  // брать только топ-N кандидатов после упорядочивания

    // Единый интерфейс: найти лучший ход за символ ai ('O' обычно, но можно и 'X' для подсказки).
    AIMove FindBestMove(const Board& board, char ai);

    // Для тестов — обёртки:
    AIMove FindBestMoveMinimax(const Board& board, char ai);
    AIMove FindBestMoveAlphaBeta(const Board& board, char ai);

    // Последняя статистика
    AIStats lastStatsMinimax{};
    AIStats lastStatsAlpha{};

private:
    // Генерация кандидатов и упорядочивание
    std::vector<std::pair<int,int>> generateCandidates(const Board& board) const;
    void orderCandidates(const Board& board, std::vector<std::pair<int,int>>& cands, char sideToMove) const;

    // Тактика: «выиграть сейчас / заблокировать сейчас»
    static bool hasImmediateWin(Board board, int x, int y, char who);

    // Терминальная и статическая оценки (возвращают счёт с т. зр. X)
    int evaluateTerminalAfterMove(const Board& b, int lastX, int lastY, char whoMoved, int depth) const;
    int evaluateStatic(const Board& b) const;

    // Быстрый «умный 1-плай» (без рекурсии) — выбирает ход по статической оценке
    AIMove greedyOnePly(const Board& board, char ai);

    // Minimax / AlphaBeta
    int minimax (Board state, int depth, bool isMax, int lastX, int lastY, AIStats& stats);
    int minimaxAB(Board state, int depth, bool isMax, int lastX, int lastY, int alpha, int beta, AIStats& stats);
};
