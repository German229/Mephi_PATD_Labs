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
    // Параметры
    int  maxDepth = 9;
    bool useAlphaBeta = true; // true -> Alpha-Beta, false -> чистый Minimax

    // Единый интерфейс: найти лучший ход за символ ai ('O' обычно, но можно и 'X' для подсказки).
    AIMove FindBestMove(const Board& board, char ai);

    // СТАРЫЕ ИМЕНА (для совместимости с тестами) — обёртки:
    AIMove FindBestMoveMinimax(const Board& board, char ai);     // вызывает FindBestMove с useAlphaBeta=false
    AIMove FindBestMoveAlphaBeta(const Board& board, char ai);   // вызывает FindBestMove с useAlphaBeta=true

    // Последняя статистика (узлы) для каждого режима
    AIStats lastStatsMinimax{};
    AIStats lastStatsAlpha{};

private:
    // Генерация кандидатов в окне [minX-1..maxX+1] x [minY-1..maxY+1] + сортировка (move ordering).
    std::vector<std::pair<int,int>> generateCandidates(const Board& board) const;
    void orderCandidates(const Board& board, std::vector<std::pair<int,int>>& cands, char sideToMove) const;

    // Тактический слой: «выиграть сейчас / заблокировать сейчас»
    static bool hasImmediateWin(Board board, int x, int y, char who);

    // Терминальная оценка после последнего хода whoMoved в (lastX, lastY)
    int evaluateTerminalAfterMove(const Board& b, int lastX, int lastY, char whoMoved, int depth) const;

    // Minimax / Alpha-Beta (возвращают оценку с т. зр. X)
    int minimax(Board state, int depth, bool isMax, int lastX, int lastY, AIStats& stats);
    int minimaxAB(Board state, int depth, bool isMax, int lastX, int lastY, int alpha, int beta, AIStats& stats);
};
