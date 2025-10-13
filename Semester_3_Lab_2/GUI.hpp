#pragma once
#include "Board.hpp"
#include "AI.hpp"

class GuiApp {
public:
    // Возвращает:
    //   true  — партия завершена (X или O победил в GUI)
    //   false — окно закрыто без завершения партии (возврат в консоль)
    bool run(Board& board, AI& ai, int winK = 3);
};
