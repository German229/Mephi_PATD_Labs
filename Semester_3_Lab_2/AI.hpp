#pragma once
#include <vector>
#include <utility>
#include <cstdint>
#include "Board.hpp"

struct AIMove {
    int x;
    int y;
    int score;
};

struct AIStats {
    long long nodes = 0;
};

class AI {
public:
    int  maxDepth = 9;
    bool useAlphaBeta = true;

    int candidateMargin = 8;
    int maxCandidates   = 32;

    AIMove FindBestMove(const Board& board, char ai);

    AIMove FindBestMoveMinimax(const Board& board, char ai);
    AIMove FindBestMoveAlphaBeta(const Board& board, char ai);

    AIStats lastStatsMinimax{};
    AIStats lastStatsAlpha{};

private:
    std::vector<std::pair<int,int>> generateCandidates(const Board& board) const;
    void orderCandidates(const Board& board, std::vector<std::pair<int,int>>& cands, char sideToMove) const;

    static bool hasImmediateWin(Board board, int x, int y, char who);

    int evaluateTerminalAfterMove(const Board& b, int lastX, int lastY, char whoMoved, int depth) const;
    int evaluateStatic(const Board& b) const;

    AIMove greedyOnePly(const Board& board, char ai);

    int minimax (Board state, int depth, bool isMax, int lastX, int lastY, AIStats& stats);
    int minimaxAB(Board state, int depth, bool isMax, int lastX, int lastY, int alpha, int beta, AIStats& stats);
};
