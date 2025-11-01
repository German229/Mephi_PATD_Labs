#pragma once
#include "Graphs.h"
#include "ConnectedComponents.h"

// Визуализация графа с подписями вершин их номерами (0..n-1).
// Без вопросов в консоли; тихая загрузка шрифта из стандартных путей.
void VisualizeGraphSFML(const AdjListGraph& g);
