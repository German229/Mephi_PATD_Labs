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

double Statistics::Moment(const Sequence<Value>& seq, std::size_t k) {
    std::size_t n = seq.GetLength();
    if (n == 0) {
        throw std::runtime_error("Cannot compute moment of empty sample");
    }

    if (k == 0) {
        return 1.0;
    }

    double sum = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        double x = seq.Get(i).AsNumber();
        sum += std::pow(x, static_cast<double>(k));
    }

    return sum / static_cast<double>(n);
}

double Statistics::CentralMoment(const Sequence<Value>& seq, std::size_t k) {
    std::size_t n = seq.GetLength();
    if (n == 0) {
        throw std::runtime_error("Cannot compute central moment of empty sample");
    }

    if (k == 0) {
        return 1.0;
    }

    double mean = Moment(seq, 1);
    double sum = 0.0;

    for (std::size_t i = 0; i < n; ++i) {
        double x = seq.Get(i).AsNumber();
        double d = x - mean;
        sum += std::pow(d, static_cast<double>(k));
    }

    return sum / static_cast<double>(n);
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
    return Moment(seq, 1);
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
    return CentralMoment(seq, 2);
}

/*
 * Вычисляет стандартное отклонение.
 *
 * Формула:
 *   stddev = sqrt(variance)
 */
double Statistics::StdDev(const Sequence<Value>& seq) {
    return std::sqrt(CentralMoment(seq, 2));
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
        return data[n / 2];
    } else {
        std::size_t mid = n / 2;
        return (data[mid - 1] + data[mid]) / 2.0;
    }
}

/*
 * Ковариация двух выборок (population covariance).
 */
double Statistics::Covariance(const Sequence<Value>& x, const Sequence<Value>& y) {
    std::size_t nx = x.GetLength();
    std::size_t ny = y.GetLength();

    if (nx == 0 || ny == 0) {
        throw std::runtime_error("Cannot compute covariance of empty sample");
    }

    if (nx != ny) {
        throw std::runtime_error("Cannot compute covariance: sample sizes differ");
    }

    double meanX = Mean(x);
    double meanY = Mean(y);

    double sum = 0.0;
    for (std::size_t i = 0; i < nx; ++i) {
        double xi = x.Get(i).AsNumber();
        double yi = y.Get(i).AsNumber();
        sum += (xi - meanX) * (yi - meanY);
    }

    return sum / static_cast<double>(nx);
}

/*
 * Коэффициент корреляции Пирсона.
 */
double Statistics::Correlation(const Sequence<Value>& x, const Sequence<Value>& y) {
    std::size_t nx = x.GetLength();
    std::size_t ny = y.GetLength();

    if (nx == 0 || ny == 0) {
        throw std::runtime_error("Cannot compute correlation of empty sample");
    }

    if (nx != ny) {
        throw std::runtime_error("Cannot compute correlation: sample sizes differ");
    }

    double sx = StdDev(x);
    double sy = StdDev(y);

    if (!(sx > 0.0) || !(sy > 0.0) || !std::isfinite(sx) || !std::isfinite(sy)) {
        throw std::runtime_error("Cannot compute correlation: stddev must be finite and > 0 for both samples");
    }

    double cov = Covariance(x, y);
    double r = cov / (sx * sy);

    if (!std::isfinite(r)) {
        throw std::runtime_error("Cannot compute correlation: result is not finite");
    }

    return r;
}