#include "Board.hpp"
#include <stdexcept>

// --- ВСПОМОГАТЕЛЬНЫЕ ---

MutableArraySequence<char> Board::makeRow(int w, char c) {
    char* buf = new char[w];
    for (int i = 0; i < w; ++i) buf[i] = c;
    MutableArraySequence<char> row(buf, w);
    delete[] buf;
    return row;
}

bool Board::toIndex(int x, int y, int& row, int& col) const {
    col = x + offsetX;
    row = y + offsetY;
    if (row < 0 || row >= height || col < 0 || col >= width) return false;
    return true;
}

// --- КОНСТРУКТОР ---

Board::Board()
: grid(), width(1), height(1), offsetX(0), offsetY(0),
  minX(1), maxX(0), minY(1), maxY(0) // так min>max означает «нет ни одного хода»
{
    // Инициализируем сетку 1x1 c '.'
    auto* rows = new MutableArraySequence<char>[1];
    rows[0] = makeRow(1, '.');
    grid = MutableArraySequence<MutableArraySequence<char>>(rows, 1);
    delete[] rows;
}

// --- РАСШИРЕНИЯ ---

void Board::expandLeft() {
    int newW = width + 1;
    // Для каждой строки создаём буфер на 1 больше, вставляем '.' слева, копируем остальное
    for (int r = 0; r < height; ++r) {
        MutableArraySequence<char> oldRow = grid.Get(r);
        char* buf = new char[newW];
        buf[0] = '.';
        for (int c = 0; c < width; ++c) buf[c + 1] = oldRow.Get(c);
        grid.Set(r, MutableArraySequence<char>(buf, newW));
        delete[] buf;
    }
    width = newW;
    offsetX += 1; // глобальный x=0 сдвинулся вправо на один индекс
}

void Board::expandRight() {
    int newW = width + 1;
    for (int r = 0; r < height; ++r) {
        MutableArraySequence<char> oldRow = grid.Get(r);
        char* buf = new char[newW];
        for (int c = 0; c < width; ++c) buf[c] = oldRow.Get(c);
        buf[newW - 1] = '.';
        grid.Set(r, MutableArraySequence<char>(buf, newW));
        delete[] buf;
    }
    width = newW;
}

void Board::expandUp() {
    int newH = height + 1;
    auto* rows = new MutableArraySequence<char>[newH];
    // Новая верхняя строка — вся '.'
    rows[0] = makeRow(width, '.');
    // Остальные копируем
    for (int r = 0; r < height; ++r) rows[r + 1] = grid.Get(r);
    grid = MutableArraySequence<MutableArraySequence<char>>(rows, newH);
    delete[] rows;
    height = newH;
    offsetY += 1; // глобальный y=0 сдвинулся вниз на один индекс
}

void Board::expandDown() {
    int newH = height + 1;
    auto* rows = new MutableArraySequence<char>[newH];
    for (int r = 0; r < height; ++r) rows[r] = grid.Get(r);
    rows[newH - 1] = makeRow(width, '.');
    grid = MutableArraySequence<MutableArraySequence<char>>(rows, newH);
    delete[] rows;
    height = newH;
}

// Гарантируем, что (x,y) будет внутри окна
void Board::ensureContains(int x, int y) {
    // Расширяем по горизонтали
    int col = x + offsetX;
    while (col < 0)  { expandLeft();  col = x + offsetX; }
    while (col >= width)  { expandRight(); col = x + offsetX; }

    // Расширяем по вертикали
    int row = y + offsetY;
    while (row < 0)  { expandUp();    row = y + offsetY; }
    while (row >= height) { expandDown(); row = y + offsetY; }
}

// --- ОСНОВНЫЕ МЕТОДЫ ---

bool Board::IsCellEmpty(int x, int y) const {
    int r, c;
    if (!toIndex(x, y, r, c)) return true; // вне окна считаем пустым
    return grid.Get(r).Get(c) == '.';
}

char Board::GetCell(int x, int y) const {
    int r, c;
    if (!toIndex(x, y, r, c)) return '.';
    return grid.Get(r).Get(c);
}

void Board::PlaceMove(int x, int y, char symbol) {
    if (symbol != 'X' && symbol != 'O') throw std::invalid_argument("symbol must be 'X' or 'O'");
    if (!IsCellEmpty(x, y)) throw std::runtime_error("Cell is not empty");

    ensureContains(x, y);
    int r = y + offsetY;
    int c = x + offsetX;

    auto row = grid.Get(r);
    row.Set(c, symbol);
    grid.Set(r, row);

    // обновляем границы занятой области
    if (minX > maxX) { // первый ход
        minX = maxX = x;
        minY = maxY = y;
    } else {
        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }
}

int Board::countInDirection(int x, int y, int dx, int dy) const {
    char s = GetCell(x, y);
    if (s != 'X' && s != 'O') return 0;
    int cnt = 1; // включаем (x,y)

    // вперёд
    int cx = x + dx, cy = y + dy;
    while (GetCell(cx, cy) == s) { cnt++; cx += dx; cy += dy; }

    // назад
    cx = x - dx; cy = y - dy;
    while (GetCell(cx, cy) == s) { cnt++; cx -= dx; cy -= dy; }

    return cnt;
}

bool Board::CheckWin(int x, int y) const {
    // 4 направления: горизонталь, вертикаль, диагонали
    static const int dirs[4][2] = {
        {1, 0}, {0, 1}, {1, 1}, {1, -1}
    };
    for (auto& d : dirs) {
        if (countInDirection(x, y, d[0], d[1]) >= 3) return true;
    }
    return false;
}

void Board::Print() const {
    if (minX > maxX) { // пока нет ходов
        std::cout << "(пусто)\n";
        return;
    }
    for (int y = maxY; y >= minY; --y) {
        std::cout << "y=" << y << " | ";
        for (int x = minX; x <= maxX; ++x) {
            std::cout << GetCell(x, y) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "     ";
    for (int x = minX; x <= maxX; ++x) std::cout << "--";
    std::cout << "\n      ";
    for (int x = minX; x <= maxX; ++x) std::cout << x % 10 << ' ';
    std::cout << "\n";
}
