#pragma once
#include <iostream>
#include <algorithm>
#include "sequence.h"

// Доска «бесконечного» размера на базе твоих Sequence.
// Хранение — динамически расширяемое окно grid[height][width], где '.','X','O'.
// (x,y) глобальные координаты. offsetX/offsetY задают смещение 0,0 внутрь окна.

class Board {
public:
    Board();

    // Основные операции
    bool IsCellEmpty(int x, int y) const;
    char GetCell   (int x, int y) const;
    void PlaceMove (int x, int y, char symbol);
    bool CheckWin  (int x, int y) const;

    // Печать занятой области (min..max)
    void Print() const;

    // Границы занятой области (если minX>maxX — ходов ещё не было)
    int MinX() const { return minX; }
    int MaxX() const { return maxX; }
    int MinY() const { return minY; }
    int MaxY() const { return maxY; }

    // Настройка правила: сколько в ряд надо для победы (по умолчанию 3 — для тестов)
    int  GetWinK() const { return winK; }
    void SetWinK(int k);

private:
    // Вспомогательные
    MutableArraySequence<char> makeRow(int w, char c);
    bool toIndex(int x, int y, int& row, int& col) const;
    void ensureContains(int x, int y);
    void expandLeft();
    void expandRight();
    void expandUp();
    void expandDown();

    int countInDirection(int x, int y, int dx, int dy) const;

private:
    // Текущее «окно»
    MutableArraySequence< MutableArraySequence<char> > grid;
    int width{1}, height{1};
    int offsetX{0}, offsetY{0}; // индекс = x+offsetX, y+offsetY

    // Занятая область
    int minX{1}, maxX{0};
    int minY{1}, maxY{0};

    // Правило победы
    int winK{3}; // по умолчанию 3 (юнит-тесты требуют «3 в ряд»)
};
