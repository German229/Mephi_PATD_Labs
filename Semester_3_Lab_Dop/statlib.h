#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#include "sequence.h"
#include "value.h"

// Простой модуль статистики для выборок Value (сейчас только числа).
// Поддерживаем:
//  - Count   (размер выборки)
//  - Mean    (среднее, по формуле sum / n)
//  - Variance (дисперсия, по формуле sum (x - mean)^2 / n)
//  - StdDev  (стандартное отклонение = sqrt(variance))
//  - Median  (медиана — по отсортированной выборке)
class Statistics {
public:
    // Размер выборки
    static std::size_t Count(const Sequence<Value>& seq);

    // Среднее значение (мат. ожидание по выборке)
    // Если выборка пуста — бросаем std::runtime_error.
    static double Mean(const Sequence<Value>& seq);

    // Дисперсия (population variance): sum (x - mean)^2 / n
    // Если выборка пуста — бросаем std::runtime_error.
    static double Variance(const Sequence<Value>& seq);

    // Стандартное отклонение: sqrt(Variance)
    static double StdDev(const Sequence<Value>& seq);

    // Медиана (по отсортированной выборке):
    //  - для нечётного n — центральное значение;
    //  - для чётного n — среднее двух центральных.
    // Если выборка пуста — бросаем std::runtime_error.
    static double Median(const Sequence<Value>& seq);
};