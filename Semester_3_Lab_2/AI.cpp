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
// +100 - depth, если победил X; -100 + depth, если победил O; иначе 0
int AI::evaluateTerminalAfterMove(const Board& b, int lastX, int lastY, char whoMoved, int depth) const {
    if (b.CheckWin(lastX, lastY)) {
        int base = 100 - depth; // быстрее — лучше
        return (whoMoved == 'X') ? base : -base;
    }
    return 0;
}

// ===== Генерация и сортировка кандидатов (move ordering) =====
std::vector<std::pair<int,int>> AI::generateCandidates(const Board& board) const {
    std::vector<std::pair<int,int>> cands;

    int minX = board.MinX(), maxX = board.MaxX();
    int minY = board.MinY(), maxY = board.MaxY();

    if (minX > maxX) { // стартовая позиция
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

// Эвристика упорядочивания: больше соседей (X/O) в 8-окрестности + ближе к центру занятой области
void AI::orderCandidates(const Board& board, std::vector<std::pair<int,int>>& cands, char /*sideToMove*/) const {
    if (cands.empty()) return;

    int minX = board.MinX(), maxX = board.MaxX();
    int minY = board.MinY(), maxY = board.MaxY();
    double cx = 0.5 * (minX + maxX);
    double cy = 0.5 * (minY + maxY);

    auto neighborScore = [&](int x, int y)->int {
        int sc = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                char ch = board.GetCell(x + dx, y + dy);
                if (ch == 'X' || ch == 'O') sc += 2; // сосед-камень поблизости — хорошо
            }
        }
        // чуть стимулируем ходы ближе к центру текущего окна:
        double d = std::hypot(x - cx, y - cy);
        int centerBonus = (d < 1.5 ? 2 : (d < 2.5 ? 1 : 0));
        return sc + centerBonus;
    };

    std::stable_sort(cands.begin(), cands.end(), [&](auto a, auto b){
        int sa = neighborScore(a.first, a.second);
        int sb = neighborScore(b.first, b.second);
        if (sa != sb) return sa > sb;
        // вторичный ключ: x,y по возрастанию (стабильность)
        if (a.second != b.second) return a.second < b.second;
        return a.first < b.first;
    });
}

// ===== Minimax (без отсечений). Оценка — для X (maximize). =====
int AI::minimax(Board state, int depth, bool isMax, int lastX, int lastY, AIStats& stats) {
    ++stats.nodes;

    if (lastX <= 100000000 && lastY <= 100000000) {
        char whoMoved = isMax ? 'O' : 'X'; // перед этим ход сделал противоположный
        int term = evaluateTerminalAfterMove(state, lastX, lastY, whoMoved, depth);
        if (term != 0) return term;
    }
    if (depth >= maxDepth) return 0;

    auto cands = generateCandidates(state);
    if (cands.empty()) return 0;
    orderCandidates(state, cands, isMax ? 'X' : 'O');

    if (isMax) { // X
        int best = negInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'X');
            int val = minimax(state, depth + 1, false, mv.first, mv.second, stats);
            best = std::max(best, val);
            if (best == 100 - (depth + 1)) break; // уже оптимум
        }
        return best;
    } else {     // O
        int best = posInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'O');
            int val = minimax(state, depth + 1, true, mv.first, mv.second, stats);
            best = std::min(best, val);
            if (best == -100 + (depth + 1)) break;
        }
        return best;
    }
}

// ===== Minimax с альфа-бета. Оценка — для X. =====
int AI::minimaxAB(Board state, int depth, bool isMax, int lastX, int lastY, int alpha, int beta, AIStats& stats) {
    ++stats.nodes;

    if (lastX <= 100000000 && lastY <= 100000000) {
        char whoMoved = isMax ? 'O' : 'X';
        int term = evaluateTerminalAfterMove(state, lastX, lastY, whoMoved, depth);
        if (term != 0) return term;
    }
    if (depth >= maxDepth) return 0;

    auto cands = generateCandidates(state);
    if (cands.empty()) return 0;
    orderCandidates(state, cands, isMax ? 'X' : 'O');

    if (isMax) { // X
        int best = negInf();
        for (auto& mv : cands) {
            if (!state.IsCellEmpty(mv.first, mv.second)) continue;
            state.PlaceMove(mv.first, mv.second, 'X');
            int val = minimaxAB(state, depth + 1, false, mv.first, mv.second, alpha, beta, stats);
            best = std::max(best, val);
            alpha = std::max(alpha, best);
            if (beta <= alpha) break; // отсечение
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
            if (beta <= alpha) break; // отсечение
        }
        return best;
    }
}

// ===== Публичный интерфейс =====
AIMove AI::FindBestMove(const Board& board, char ai) {
    auto cands = generateCandidates(board);
    if (cands.empty()) return {0,0,0};
    orderCandidates(board, cands, ai);

    char opp = (ai == 'X' ? 'O' : 'X');

    // 1) Моментальная победа
    for (auto& mv : cands) {
        if (hasImmediateWin(board, mv.first, mv.second, ai)) {
            int sc = (ai == 'X' ? +100 : -100);
            // обновим статистику, чтобы в выводе не висели старые числа
            if (useAlphaBeta) { lastStatsAlpha.nodes = 1; }
            else              { lastStatsMinimax.nodes = 1; }
            return {mv.first, mv.second, sc};
        }
    }
    // 2) Моментальный блок
    for (auto& mv : cands) {
        if (hasImmediateWin(board, mv.first, mv.second, opp)) {
            if (useAlphaBeta) { lastStatsAlpha.nodes = 1; }
            else              { lastStatsMinimax.nodes = 1; }
            return {mv.first, mv.second, 0};
        }
    }

    // 3) Поиск
    AIMove best{cands[0].first, cands[0].second, 0};
    if (useAlphaBeta) {
        lastStatsAlpha.nodes = 0;
        int bestScore = (ai == 'O') ? posInf() : negInf();
        for (auto& mv : cands) {
            if (!board.IsCellEmpty(mv.first, mv.second)) continue;
            Board copy = board;
            copy.PlaceMove(mv.first, mv.second, ai);
            // после хода ai, ходит противоположный: если ai=O, то MAX (X); если ai=X, то MIN (O)
            bool nextIsMax = (ai == 'O');
            int sc = minimaxAB(copy, 0, nextIsMax, mv.first, mv.second, negInf(), posInf(), lastStatsAlpha);
            if (ai == 'O') {
                if (sc < bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; }
            } else {
                if (sc > bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; }
            }
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
            if (ai == 'O') {
                if (sc < bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; }
            } else {
                if (sc > bestScore) { bestScore = sc; best = {mv.first, mv.second, sc}; }
            }
        }
    }
    return best;
}

// ===== Обёртки для совместимости с тестами =====
AIMove AI::FindBestMoveMinimax(const Board& board, char ai) {
    AI tmp = *this;
    tmp.useAlphaBeta = false;
    return tmp.FindBestMove(board, ai);
}

AIMove AI::FindBestMoveAlphaBeta(const Board& board, char ai) {
    AI tmp = *this;
    tmp.useAlphaBeta = true;
    return tmp.FindBestMove(board, ai);
}
