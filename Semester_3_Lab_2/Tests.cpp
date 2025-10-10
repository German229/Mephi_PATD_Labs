#include <cassert>
#include <stdexcept>
#include <iostream>
#include "Board.hpp"
#include "AI.hpp"

void TestBoardBasics() {
    Board b;
    // изначально пусто
    assert(b.MinX() > b.MaxX());
    assert(b.GetCell(0,0) == '.');
    // ставим X в (0,0)
    b.PlaceMove(0,0,'X');
    assert(b.GetCell(0,0) == 'X');
    assert(b.MinX() == 0 && b.MaxX() == 0);
    assert(b.MinY() == 0 && b.MaxY() == 0);
    // ставим O вправо (2,0) -> расширение вправо
    b.PlaceMove(2,0,'O');
    assert(b.GetCell(2,0) == 'O');
    assert(b.MinX() == 0 && b.MaxX() == 2);
    // ставим X влево/вверх (-1, -1) -> расширение влево и вверх
    b.PlaceMove(-1,-1,'X');
    assert(b.GetCell(-1,-1) == 'X');
    assert(b.MinX() == -1 && b.MaxX() == 2);
    assert(b.MinY() == -1 && b.MaxY() == 0);
    std::cout << "TestBoardBasics OK\n";
}

void TestWinDetection() {
    Board b;
    b.PlaceMove(0,0,'X');
    b.PlaceMove(1,0,'X');
    b.PlaceMove(2,0,'X');
    assert(b.CheckWin(2,0) == true);
    Board c;
    c.PlaceMove(2,2,'O');
    c.PlaceMove(2,1,'O');
    c.PlaceMove(2,0,'O');
    assert(c.CheckWin(2,0) == true);
    Board d;
    d.PlaceMove(0,0,'X');
    d.PlaceMove(1,1,'X');
    d.PlaceMove(2,2,'X');
    assert(d.CheckWin(2,2) == true);
    std::cout << "TestWinDetection OK\n";
}

static inline bool isBlockAtEitherEnd(const AIMove& mv) {
    // допустимые блоки для X на (0,0) и (1,0): (-1,0) ИЛИ (2,0)
    return (mv.y == 0) && (mv.x == -1 || mv.x == 2);
}

static inline bool isWinAtEitherEnd(const AIMove& mv) {
    // допустимые выигрыши для O на (0,0) и (1,0): (-1,0) ИЛИ (2,0)
    return (mv.y == 0) && (mv.x == -1 || mv.x == 2);
}

void TestAIBlockAndWin() {
    AI ai;
    ai.maxDepth = 6;

    // AI = 'O' должен блокировать X: X на (0,0) и (1,0) -> O ставит (-1,0) или (2,0)
    Board b;
    b.PlaceMove(0,0,'X');
    b.PlaceMove(1,0,'X');
    auto mv = ai.FindBestMoveAlphaBeta(b, 'O');
    assert(isBlockAtEitherEnd(mv));

    // AI = 'O' должен выигрывать: O на (0,0) и (1,0) -> ход (-1,0) или (2,0)
    Board c;
    c.PlaceMove(0,0,'O');
    c.PlaceMove(1,0,'O');
    auto mv2 = ai.FindBestMoveAlphaBeta(c, 'O');
    assert(isWinAtEitherEnd(mv2));

    std::cout << "TestAIBlockAndWin OK\n";
}

void TestAIConsistency() {
    AI ai;
    ai.maxDepth = 7;
    Board b;
    // Немного позиция
    b.PlaceMove(0,0,'X');
    b.PlaceMove(1,0,'O');
    b.PlaceMove(0,1,'X');

    auto a = ai.FindBestMoveMinimax(b, 'O');
    auto c = ai.FindBestMoveAlphaBeta(b, 'O');
    // Результат (оценка) должен совпасть
    assert(a.score == c.score);
    std::cout << "TestAIConsistency OK\n";
}