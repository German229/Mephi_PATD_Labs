#pragma once
#include "Graphs.h"
#include "ConnectedComponents.h"

// Рисует неориентированный граф в окне SFML.
// Цвет вершины соответствует компоненте связности.
// Если SFML отключён (нет USE_SFML), функция выведет сообщение и вернёт управление.
void VisualizeGraphSFML(const AdjListGraph& g);
