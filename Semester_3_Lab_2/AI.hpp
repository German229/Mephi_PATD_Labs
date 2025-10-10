#pragma once
#include <vector>
#include <limits>
#include <utility>
#include "Board.hpp"

struct AIMove {
    int x;
    int y;
    int score;
};

class AI {
public:
    // Найти лучший ход за символ ai ('O' обычно). Использует Minimax (без отсечения).
    AIMove FindBestMoveMinimax(const Board& board, char ai);

    // Найти лучший ход за символ ai с альфа-бета отсечением.
    AIMove FindBestMoveAlphaBeta(const Board& board, char ai);

    // Ограничение глубины (опционально). По умолчанию 9 (для 3-в-ряд достаточно).
    int maxDepth = 9;

private:
    // Сгенерировать кандидатов: все пустые клетки в окне [minX-1..maxX+1] x [minY-1..maxY+1].
    std::vector<std::pair<int,int>> generateCandidates(const Board& board) const;

    // Оценка терминальных исходов относительно 'X' (положительно для X).
    // Если хотим оценку относительно ai, можно инвертировать знак при необходимости.
    int evaluateTerminalAfterMove(const Board& b, int lastX, int lastY, char whoMoved, int depth) const;

    // Minimax (без отсечений)
    int minimax(Board state, int depth, bool isMax, char ai, int lastX, int lastY);

    // Minimax с альфа-бета
    int minimaxAB(Board state, int depth, bool isMax, char ai, int lastX, int lastY, int alpha, int beta);
};
