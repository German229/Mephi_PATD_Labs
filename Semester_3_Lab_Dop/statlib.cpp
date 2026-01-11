#include "statlib.h"

/*
 * Возвращает количество элементов в выборке.
 *
 * Делегирует вызов абстрактному интерфейсу Sequence,
 * что позволяет использовать любую конкретную реализацию
 * последовательности (массив, список и т.д.).
 */
std::size_t Statistics::Count(const Sequence<Value>& seq) {
    return seq.GetLength();
}

/*
 * Вычисляет среднее арифметическое значений в выборке.
 *
 * Формула:
 *   mean = (1 / n) * sum(x_i)
 *
 * Выборка должна быть непустой.
 */
double Statistics::Mean(const Sequence<Value>& seq) {
    std::size_t n = seq.GetLength();
    if (n == 0) {
        throw std::runtime_error("Cannot compute mean of empty sample");
    }

    double sum = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        const Value& v = seq.Get(i);
        sum += v.AsNumber();
    }

    return sum / static_cast<double>(n);
}

/*
 * Вычисляет дисперсию выборки (population variance).
 *
 * Формула:
 *   variance = (1 / n) * sum( (x_i - mean)^2 )
 *
 * Используется population variance, а не несмещённая оценка.
 */
double Statistics::Variance(const Sequence<Value>& seq) {
    std::size_t n = seq.GetLength();
    if (n == 0) {
        throw std::runtime_error("Cannot compute variance of empty sample");
    }

    double mean = Mean(seq);
    double sumSq = 0.0;

    for (std::size_t i = 0; i < n; ++i) {
        double x = seq.Get(i).AsNumber();
        double diff = x - mean;
        sumSq += diff * diff;
    }

    return sumSq / static_cast<double>(n);
}

/*
 * Вычисляет стандартное отклонение.
 *
 * Формула:
 *   stddev = sqrt(variance)
 */
double Statistics::StdDev(const Sequence<Value>& seq) {
    double var = Variance(seq);
    return std::sqrt(var);
}

/*
 * Вычисляет медиану выборки.
 *
 * Алгоритм:
 *  1. Копирование значений из Sequence в std::vector<double>
 *  2. Сортировка
 *  3. Выбор центрального элемента:
 *     - при нечётном размере — средний элемент
 *     - при чётном размере — среднее двух центральных
 */
double Statistics::Median(const Sequence<Value>& seq) {
    std::size_t n = seq.GetLength();
    if (n == 0) {
        throw std::runtime_error("Cannot compute median of empty sample");
    }

    std::vector<double> data;
    data.reserve(n);

    for (std::size_t i = 0; i < n; ++i) {
        data.push_back(seq.Get(i).AsNumber());
    }

    std::sort(data.begin(), data.end());

    if (n % 2 == 1) {
        // Нечётное число элементов — центральный элемент
        return data[n / 2];
    } else {
        // Чётное — среднее двух центральных
        std::size_t mid = n / 2;
        return (data[mid - 1] + data[mid]) / 2.0;
    }
}