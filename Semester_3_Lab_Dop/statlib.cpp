#include "statlib.h"

std::size_t Statistics::Count(const Sequence<Value>& seq) {
    return seq.GetLength();
}

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

    // Population variance: делим на n
    return sumSq / static_cast<double>(n);
}

double Statistics::StdDev(const Sequence<Value>& seq) {
    double var = Variance(seq);
    return std::sqrt(var);
}

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
        // нечётное число элементов
        return data[n / 2];
    } else {
        // чётное — среднее двух центральных
        std::size_t mid = n / 2;
        return (data[mid - 1] + data[mid]) / 2.0;
    }
}