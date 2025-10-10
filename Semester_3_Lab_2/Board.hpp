#pragma once
#include <iostream>
#include <algorithm>
#include "sequence.h"

// Бесконечное поле на базе Sequence/MutableArraySequence.
// Хранит прямоугольное окно [0..height-1] x [0..width-1] и смещения offsetX/offsetY,
// чтобы глобальные координаты (x,y) отображать в индексы массива.
// Пустая клетка — '.'; ходы — 'X' и 'O'.
class Board {
public:
    Board();

    // Разместить ход. Бросает std::runtime_error, если клетка занята.
    void PlaceMove(int x, int y, char symbol);

    // Проверка, свободна ли клетка (включая точки вне текущего окна — считаются пустыми).
    bool IsCellEmpty(int x, int y) const;

    // Получить символ в клетке (если вне текущих границ — вернёт '.').
    char GetCell(int x, int y) const;

    // Проверка победы (3 в ряд) по последнему ходу (x,y).
    bool CheckWin(int x, int y) const;

    // Печать видимого фрагмента: только прямоугольник, покрывающий все ходы (min..max по X и Y).
    void Print() const;

    // Границы занятой области (глобальные координаты). До первого хода: minX>maxX.
    int MinX() const { return minX; }
    int MaxX() const { return maxX; }
    int MinY() const { return minY; }
    int MaxY() const { return maxY; }

private:
    // Внутреннее хранилище: строки (MutableArraySequence<char>) внутри
    // MutableArraySequence<MutableArraySequence<char>>
    MutableArraySequence<MutableArraySequence<char>> grid;
    int width;   // текущее число столбцов
    int height;  // текущее число строк
    int offsetX; // глобальная x=0 находится в столбце offsetX (может меняться при расширениях)
    int offsetY; // глобальная y=0 находится в строке   offsetY

    // Границы реально занятых клеток (по глобальным координатам).
    int minX, maxX, minY, maxY;

    // Вспомогательное: создать строку длиной w, заполненную c.
    static MutableArraySequence<char> makeRow(int w, char c);

    // Координаты преобразование: глобальные -> индексы
    bool toIndex(int x, int y, int& row, int& col) const;

    // Обеспечить, что (x,y) попадает внутрь окна — расширить при необходимости.
    void ensureContains(int x, int y);

    // Разные направления расширений:
    void expandLeft();
    void expandRight();
    void expandUp();
    void expandDown();

    // Счёт подряд символов в направлении (dx,dy), включая (x,y).
    int countInDirection(int x, int y, int dx, int dy) const;
};