#include "AI.hpp"
#include <algorithm>
#include <limits>
#include <cmath>

static inline int posInf() { return std::numeric_limits<int>::max() / 4; }
static inline int negInf() { return -std::numeric_limits<int>::max() / 4; }

// ===== Тактика «выиграть сейчас / заблокировать сейчас» =====
bool AI::hasImmediateWin(Board board, int x, int y, char who) {
    if (!board.IsCellEmpty(x, y)) return false;
    board.PlaceMove(x, y, who);
    return board.CheckWin(x, y);
}

// ===== Терминальная оценка =====
int AI::evaluateTerminalAfterMove(const Board& b, int lastX, int lastY, char whoMoved, int depth) const {
    if (b.CheckWin(lastX, lastY)) {
        int base = 100 - depth;
        return (whoMoved == 'X') ? base : -base;
    }
    return 0;
}

// ===== Генерация кандидатов (рамка ±candidateMargin) =====
std::vector<std::pair<int,int>> AI::generateCandidates(const Board& board) const {
    std::vector<std::pair<int,int>> cands;

    int minX = board.MinX(), maxX = board.MaxX();
    int minY = board.MinY(), maxY = board.MaxY();

    if (minX > maxX) {
        cands.push_back({0,0});
        return cands;
    }

    const int m = std::max(1, candidateMargin);
    for (int y = minY - m; y <= maxY + m; ++y)
        for (int x = minX - m; x <= maxX + m; ++x)
            if (board.IsCellEmpty(x, y)) cands.push_back({x, y});

    return cands;
}

// ===== Сортировка кандидатов (БЕЗ обрезки!) =====
void AI::orderCandidates(const Board& board, std::vector<std::pair<int,int>>& cands, char /*sideToMove*/) const {
    if (cands.empty()) return;

    int minX = board.MinX(), maxX = board.MaxX();
    int minY = board.MinY(), maxY = board.MaxY();
    double cx = 0.5 * (minX + maxX);
    double cy = 0.5 * (minY + maxY);

    auto neighborScore = [&](int x, int y)->int {
        int sc = 0;
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                char ch = board.GetCell(x + dx, y + dy);
                if (ch == 'X' || ch == 'O') sc += 2;
            }
        double d = std::hypot(x - cx, y - cy);
        int centerBonus = (d < 1.5 ? 2 : (d < 2.5 ? 1 : 0));
        return sc + centerBonus;
    };

    std::stable_sort(cands.begin(), cands.end(), [&](auto a, auto b){
        int sa = neighborScore(a.first, a.second);
        int sb = neighborScore(b.first, b.second);
        if (sa != sb) return sa > sb;
        if (a.second != b.second) return a.second < b.second;
        return a.first < b.first;
    });
}

// ===== Статическая оценка (для не-терминалов), счёт с т. зр. X =====
int AI::evaluateStatic(const Board& b) const {
    const int K = b.GetWinK();

    auto patternScore = [&](int len, int openEnds)->int {
        if (len >= K) return 100000;
        int base = 0;
        if      (len == K-1) base = 6000;
        else if (len == K-2) base = 900;
        else if (len == K-3) base = 120;
        else if (len == 1)   base = 5;
        else                 base = 25 * len;

        if (openEnds == 2) base = base * 3 / 2;
        else if (openEnds == 0) base = base / 3;
        return base;
    };

    const int dirs[4][2] = { {1,0},{0,1},{1,1},{1,-1} };

    long long scoreX = 0, scoreO = 0;

    int minX = b.MinX(), maxX = b.MaxX();
    int minY = b.MinY(), maxY = b.MaxY();
    if (minX > maxX) return 0;

    for (int y = minY; y <= maxY; ++y)
        for (int x = minX; x <= maxX; ++x) {
            char s = b.GetCell(x,y);
            if (s != 'X' && s != 'O') continue;

            for (auto& d : dirs) {
                int dx = d[0], dy = d[1];
                if (b.GetCell(x-dx, y-dy) == s) continue;

                int len = 0, cx = x, cy = y;
                while (b.GetCell(cx, cy) == s) { ++len; cx += dx; cy += dy; }

                int openEnds = 0;
                if (b.GetCell(x-dx, y-dy) == '.') ++openEnds;
                if (b.GetCell(cx,   cy)   == '.') ++openEnds;

                int ps = patternScore(len, openEnds);
                if (s == 'X') scoreX += ps; else scoreO += ps;
            }
        }

    long long score = scoreX - scoreO;
    if (score > 9000)  score += 600;
    if (score < -9000) score -= 600;
    if (score > 40000) score = 40000;
    if (score < -40000) score = -40000;
    return static_cast<int>(score);
}

// ===== Быстрый «умный 1-плай» =====
AIMove AI::greedyOnePly(const Board& board, char ai) {
    auto cands = generateCandidates(board);
    if (cands.empty()) return {0,0,0};
    orderCandidates(board, cands, ai);
    if ((int)cands.size() > maxCandidates) cands.resize(maxCandidates);

    // Мгновенная победа / блок — ЗДЕСЬ уже список сокращён, но это ок:
    // перед этим FindBestMove делает полный проход по всем пустым.
    char opp = (ai == 'X' ? 'O' : 'X');
    for (auto& mv : cands) if (hasImmediateWin(board, mv.first, mv.second, ai))
        return {mv.first, mv.second, (ai=='X'?+100:-100)};
    for (auto& mv : cands) if (hasImmediateWin(board, mv.first, mv.second, opp))
        return {mv.first, mv.second, 0};

    AIMove best{cands[0].first, cands[0].second, 0};
    int bestScore = (ai=='O') ? posInf() : negInf();

    for (auto& mv : cands) {
        if (!board.IsCellEmpty(mv.first, mv.second)) continue;
        Board copy = board;
        copy.PlaceMove(mv.first, mv.second, ai);
        int sc = evaluateStatic(copy);
        if (ai == 'O') { if (sc < bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; } }
        else           { if (sc > bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; } }
    }
    return best;
}

// ===== Minimax (без отсечений) =====
int AI::minimax(Board state, int depth, bool isMax, int lastX, int lastY, AIStats& stats) {
    ++stats.nodes;

    if (lastX <= 100000000 && lastY <= 100000000) {
        char whoMoved = isMax ? 'O' : 'X';
        int term = evaluateTerminalAfterMove(state, lastX, lastY, whoMoved, depth);
        if (term != 0) return term;
    }
    if (depth >= maxDepth) return evaluateStatic(state);

    auto cands = generateCandidates(state);
    if (cands.empty()) return evaluateStatic(state);
    orderCandidates(state, cands, isMax ? 'X' : 'O');
    if ((int)cands.size() > maxCandidates) cands.resize(maxCandidates);

    if (isMax) {
        int best = negInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'X');
            int val = minimax(state, depth + 1, false, mv.first, mv.second, stats);
            best = std::max(best, val);
            if (best >= 100 - (depth + 1)) break;
        }
        return best;
    } else {
        int best = posInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'O');
            int val = minimax(state, depth + 1, true, mv.first, mv.second, stats);
            best = std::min(best, val);
            if (best <= -100 + (depth + 1)) break;
        }
        return best;
    }
}

// ===== Alpha-Beta =====
int AI::minimaxAB(Board state, int depth, bool isMax, int lastX, int lastY, int alpha, int beta, AIStats& stats) {
    ++stats.nodes;

    if (lastX <= 100000000 && lastY <= 100000000) {
        char whoMoved = isMax ? 'O' : 'X';
        int term = evaluateTerminalAfterMove(state, lastX, lastY, whoMoved, depth);
        if (term != 0) return term;
    }
    if (depth >= maxDepth) return evaluateStatic(state);

    auto cands = generateCandidates(state);
    if (cands.empty()) return evaluateStatic(state);
    orderCandidates(state, cands, isMax ? 'X' : 'O');
    if ((int)cands.size() > maxCandidates) cands.resize(maxCandidates);

    if (isMax) { // X
        int best = negInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'X');
            int val = minimaxAB(state, depth + 1, false, mv.first, mv.second, alpha, beta, stats);
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
            int val = minimaxAB(state, depth + 1, true, mv.first, mv.second, alpha, beta, stats);
            best = std::min(best, val);
            beta = std::min(beta, best);
            if (beta <= alpha) break;
        }
        return best;
    }
}

// ===== Публичный интерфейс =====
AIMove AI::FindBestMove(const Board& board, char ai) {
    // ПОЛНЫЙ список пустых клеток в рамке — для «win now / block now»
    auto full = generateCandidates(board);
    if (full.empty()) return {0,0,0};

    // 1) Мгновенная победа
    for (auto& mv : full) {
        if (hasImmediateWin(board, mv.first, mv.second, ai)) {
            int sc = (ai == 'X' ? +100 : -100);
            if (useAlphaBeta) { lastStatsAlpha.nodes = 1; }
            else              { lastStatsMinimax.nodes = 1; }
            return {mv.first, mv.second, sc};
        }
    }
    // 2) Мгновенный блок победы соперника — тоже по ПОЛНОМУ списку
    char opp = (ai == 'X' ? 'O' : 'X');
    for (auto& mv : full) {
        if (hasImmediateWin(board, mv.first, mv.second, opp)) {
            if (useAlphaBeta) { lastStatsAlpha.nodes = 1; }
            else              { lastStatsMinimax.nodes = 1; }
            return {mv.first, mv.second, 0};
        }
    }

    // Дальше сортируем и ОТРЕЗАЕМ до maxCandidates
    auto cands = full;
    orderCandidates(board, cands, ai);
    if ((int)cands.size() > maxCandidates) cands.resize(maxCandidates);

    // 2.5) Быстрый 1-плай при winK>=4
    if (board.GetWinK() >= 4) {
        return greedyOnePly(board, ai);
    }

    // 3) Классический поиск для winK==3
    AIMove best{cands[0].first, cands[0].second, 0};
    if (useAlphaBeta) {
        lastStatsAlpha.nodes = 0;
        int bestScore = (ai == 'O') ? posInf() : negInf();
        for (auto& mv : cands) {
            if (!board.IsCellEmpty(mv.first, mv.second)) continue;
            Board copy = board;
            copy.PlaceMove(mv.first, mv.second, ai);
            bool nextIsMax = (ai == 'O');
            int sc = minimaxAB(copy, 0, nextIsMax, mv.first, mv.second, negInf(), posInf(), lastStatsAlpha);
            if (ai == 'O') { if (sc < bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; } }
            else           { if (sc > bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; } }
        }
    } else {
        lastStatsMinimax.nodes = 0;
        int bestScore = (ai == 'O') ? posInf() : negInf();
        for (auto& mv : cands) {
            if (!board.IsCellEmpty(mv.first, mv.second)) continue;
            Board copy = board;
            copy.PlaceMove(mv.first, mv.second, ai);
            bool nextIsMax = (ai == 'O');
            int sc = minimax(copy, 0, nextIsMax, mv.first, mv.second, lastStatsMinimax);
            if (ai == 'O') { if (sc < bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; } }
            else           { if (sc > bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; } }
        }
    }
    return best;
}

// ===== Обёртки для тестов =====
AIMove AI::FindBestMoveMinimax(const Board& board, char ai) {
    AI tmp = *this; tmp.useAlphaBeta = false; return tmp.FindBestMove(board, ai);
}
AIMove AI::FindBestMoveAlphaBeta(const Board& board, char ai) {
    AI tmp = *this; tmp.useAlphaBeta = true;  return tmp.FindBestMove(board, ai);
}
