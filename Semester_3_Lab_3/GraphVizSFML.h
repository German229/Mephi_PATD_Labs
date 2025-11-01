#pragma once
#include "Graphs.h"
#include "ConnectedComponents.h"

// Визуализация неориентированного графа в SFML 3.x.
// - Вершины раскрашены по компонентам связности.
// - Подписи вершин: номера 0..n-1 (если найден системный шрифт).
// - Подписи рёбер:
//     * при overlay = BFS/DFS — номер ребра в порядке принятия в дерево обхода;
//     * при overlay = none — подпись формата "u-v".
// - Горячие клавиши в окне:
//     * B — показать порядок обхода BFS (по всем компонентам)
//     * D — показать порядок обхода DFS (по всем компонентам, итеративно)
//     * C — очистить подсветку порядка обхода
//     * E — вкл/выкл подписи рёбер
//     * Esc — закрыть окно

// Открыть без начальной подсветки, без подписей рёбер
void VisualizeGraphSFML(const AdjListGraph& g);

// Открыть с выбранной начальной подсветкой порядка обхода:
// initialOverlay: 0 — нет, 1 — BFS, 2 — DFS
void VisualizeGraphSFML(const AdjListGraph& g, int initialOverlay);

// Полная версия: initialOverlay как выше, showEdgeLabels — показывать ли подписи рёбер сразу
void VisualizeGraphSFML(const AdjListGraph& g, int initialOverlay, bool showEdgeLabels);
