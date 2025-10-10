#include "AI.hpp"
#include <algorithm>
#include <limits>

static inline int posInf() { return std::numeric_limits<int>::max() / 4; }
static inline int negInf() { return -std::numeric_limits<int>::max() / 4; }

// --------- тактика «выиграть сейчас / заблокировать сейчас» ---------
static bool hasImmediateWin(Board board, int x, int y, char who) {
    if (!board.IsCellEmpty(x, y)) return false;
    board.PlaceMove(x, y, who);
    return board.CheckWin(x, y);
}

// Оценка терминала: +100 - depth, если победил X; -100 + depth, если победил O; 0 — иначе.
int AI::evaluateTerminalAfterMove(const Board& b, int lastX, int lastY, char whoMoved, int depth) const {
    if (b.CheckWin(lastX, lastY)) {
        int base = 100 - depth; // быстрее — лучше
        return (whoMoved == 'X') ? base : -base;
    }
    return 0;
}

std::vector<std::pair<int,int>> AI::generateCandidates(const Board& board) const {
    std::vector<std::pair<int,int>> cands;

    int minX = board.MinX(), maxX = board.MaxX();
    int minY = board.MinY(), maxY = board.MaxY();

    if (minX > maxX) { // старт
        cands.push_back({0,0});
        return cands;
    }

    for (int y = minY - 1; y <= maxY + 1; ++y) {
        for (int x = minX - 1; x <= maxX + 1; ++x) {
            if (board.IsCellEmpty(x, y)) cands.push_back({x, y});
        }
    }
    return cands;
}

// ---------------- Minimax (без отсечений) ----------------
int AI::minimax(Board state, int depth, bool isMax, char /*ai*/, int lastX, int lastY) {
    if (lastX <= 100000000 && lastY <= 100000000) {
        char whoMoved = isMax ? 'O' : 'X'; // перед входом ход сделал противоположный текущему
        int term = evaluateTerminalAfterMove(state, lastX, lastY, whoMoved, depth);
        if (term != 0) return term;
    }
    if (depth >= maxDepth) return 0;

    auto cands = generateCandidates(state);
    if (cands.empty()) return 0;

    if (isMax) { // X
        int best = negInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'X');
            int val = minimax(state, depth + 1, false, 'O', mv.first, mv.second);
            best = std::max(best, val);
            if (best == 100 - (depth + 1)) break;
        }
        return best;
    } else {     // O
        int best = posInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'O');
            int val = minimax(state, depth + 1, true, 'O', mv.first, mv.second);
            best = std::min(best, val);
            if (best == -100 + (depth + 1)) break;
        }
        return best;
    }
}

// ---------------- Minimax с альфа-бета ----------------
int AI::minimaxAB(Board state, int depth, bool isMax, char /*ai*/, int lastX, int lastY, int alpha, int beta) {
    if (lastX <= 100000000 && lastY <= 100000000) {
        char whoMoved = isMax ? 'O' : 'X';
        int term = evaluateTerminalAfterMove(state, lastX, lastY, whoMoved, depth);
        if (term != 0) return term;
    }
    if (depth >= maxDepth) return 0;

    auto cands = generateCandidates(state);
    if (cands.empty()) return 0;

    if (isMax) { // X
        int best = negInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'X');
            int val = minimaxAB(state, depth + 1, false, 'O', mv.first, mv.second, alpha, beta);
            best = std::max(best, val);
            alpha = std::max(alpha, best);
            if (beta <= alpha) break;
        }
        return best;
    } else {     // O
        int best = posInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'O');
            int val = minimaxAB(state, depth + 1, true, 'O', mv.first, mv.second, alpha, beta);
            best = std::min(best, val);
            beta = std::min(beta, best);
            if (beta <= alpha) break;
        }
        return best;
    }
}

// ---------------- Публичные методы ----------------
AIMove AI::FindBestMoveMinimax(const Board& board, char ai) {
    auto cands = generateCandidates(board);
    if (cands.empty()) return {0,0,0};

    char opp = (ai == 'X' ? 'O' : 'X');

    // 1) моментальная победа
    for (auto& mv : cands)
        if (hasImmediateWin(board, mv.first, mv.second, ai))
            return {mv.first, mv.second, (ai=='X'? +100 : -100)};

    // 2) моментальный блок победы оппонента
    for (auto& mv : cands)
        if (hasImmediateWin(board, mv.first, mv.second, opp))
            return {mv.first, mv.second, 0};

    // 3) minimax
    int bestScore = (ai == 'O') ? posInf() : negInf();
    AIMove best{cands[0].first, cands[0].second, 0};

    for (auto& mv : cands) {
        if (!board.IsCellEmpty(mv.first, mv.second)) continue;
        Board copy = board;
        copy.PlaceMove(mv.first, mv.second, ai);
        int sc = minimax(copy, 0, /*next is X*/ (ai == 'O'), ai, mv.first, mv.second);
        if (ai == 'O') {
            if (sc < bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; }
        } else {
            if (sc > bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; }
        }
    }
    return best;
}

AIMove AI::FindBestMoveAlphaBeta(const Board& board, char ai) {
    auto cands = generateCandidates(board);
    if (cands.empty()) return {0,0,0};

    char opp = (ai == 'X' ? 'O' : 'X');

    // 1) моментальная победа
    for (auto& mv : cands)
        if (hasImmediateWin(board, mv.first, mv.second, ai))
            return {mv.first, mv.second, (ai=='X'? +100 : -100)};

    // 2) моментальный блок победы оппонента
    for (auto& mv : cands)
        if (hasImmediateWin(board, mv.first, mv.second, opp))
            return {mv.first, mv.second, 0};

    // 3) alpha-beta
    int bestScore = (ai == 'O') ? posInf() : negInf();
    AIMove best{cands[0].first, cands[0].second, 0};

    for (auto& mv : cands) {
        if (!board.IsCellEmpty(mv.first, mv.second)) continue;
        Board copy = board;
        copy.PlaceMove(mv.first, mv.second, ai);
        int sc = minimaxAB(copy, 0, /*next is X*/ (ai == 'O'), ai, mv.first, mv.second, negInf(), posInf());
        if (ai == 'O') {
            if (sc < bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; }
        } else {
            if (sc > bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; }
        }
    }
    return best;
}
