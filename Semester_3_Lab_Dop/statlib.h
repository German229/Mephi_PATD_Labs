#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#include "sequence.h"
#include "value.h"

/*
 * Модуль Statistics
 *
 * Предоставляет набор базовых статистических функций
 * для выборок значений типа Value.
 *
 * На текущий момент предполагается, что все значения —
 * вещественные числа (double).
 *
 * Поддерживаемые статистики:
 *  - Count    — размер выборки
 *  - Mean     — среднее арифметическое
 *  - Variance — дисперсия (population variance)
 *  - StdDev   — стандартное отклонение
 *  - Median   — медиана
 */
class Statistics {
public:
    /*
     * Получить количество элементов в выборке.
     *
     * @param seq  Последовательность значений.
     * @return     Число элементов в выборке.
     */
    static std::size_t Count(const Sequence<Value>& seq);

    /*
     * Вычислить среднее арифметическое выборки.
     *
     * Формула:
     *   mean = (1 / n) * sum(x_i)
     *
     * @param seq  Последовательность значений.
     * @return     Среднее значение.
     *
     * @throws std::runtime_error если выборка пуста.
     */
    static double Mean(const Sequence<Value>& seq);

    /*
     * Вычислить дисперсию выборки (population variance).
     *
     * Формула:
     *   variance = (1 / n) * sum( (x_i - mean)^2 )
     *
     * Используется population variance, а не unbiased estimator.
     *
     * @param seq  Последовательность значений.
     * @return     Дисперсия.
     *
     * @throws std::runtime_error если выборка пуста.
     */
    static double Variance(const Sequence<Value>& seq);


    /*
     * Начальный момент k-го порядка:
     *   moment_k = (1 / n) * sum(x_i^k)
     *
     * @param seq  Последовательность значений.
     * @param k    Порядок момента (k >= 0).
     * @return     Значение момента.
     *
     * @throws std::runtime_error если выборка пуста.
     */
    static double Moment(const Sequence<Value>& seq, std::size_t k);

    /*
     * Центральный момент k-го порядка:
     *   cmoment_k = (1 / n) * sum((x_i - mean)^k)
     *
     * @param seq  Последовательность значений.
     * @param k    Порядок момента (k >= 0).
     * @return     Значение центрального момента.
     *
     * @throws std::runtime_error если выборка пуста.
     */
    static double CentralMoment(const Sequence<Value>& seq, std::size_t k);

    /*
     * Вычислить стандартное отклонение.
     *
     * Формула:
     *   stddev = sqrt(variance)
     *
     * @param seq  Последовательность значений.
     * @return     Стандартное отклонение.
     *
     * @throws std::runtime_error если выборка пуста.
     */
    static double StdDev(const Sequence<Value>& seq);

    /*
     * Вычислить медиану выборки.
     *
     * Алгоритм:
     *  1. Копирование значений в std::vector<double>
     *  2. Сортировка
     *  3. Выбор центрального элемента:
     *     - при нечётном n — значение с индексом n / 2
     *     - при чётном n — среднее двух центральных значений
     *
     * @param seq  Последовательность значений.
     * @return     Медиана.
     *
     * @throws std::runtime_error если выборка пуста.
     */
    static double Median(const Sequence<Value>& seq);
};