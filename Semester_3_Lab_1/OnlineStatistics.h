#ifndef ONLINE_STATISTICS_H
#define ONLINE_STATISTICS_H

#include "dynamic_array.h"
#include <stdexcept>
#include <limits>
#include <cmath>

// ------------------------ Base interface ------------------------

template<typename T>
class IStatistic {
public:
    virtual ~IStatistic() {}
    virtual void Add(const T& value) = 0;
};

// ------------------------ Mean ------------------------

template<typename T>
class MeanStatistic : public IStatistic<T> {
public:
    MeanStatistic() : sum(0.0L), count(0) {}

    void Add(const T& value) override {
        sum += static_cast<long double>(value);
        ++count;
    }

    long long GetCount() const {
        return count;
    }

    double GetMean() const {
        if (count == 0) {
            throw std::runtime_error("no data for mean");
        }
        return static_cast<double>(sum / static_cast<long double>(count));
    }

private:
    long double sum;
    long long count;
};

// ------------------------ Variance (Welford) ------------------------

template<typename T>
class VarianceStatistic : public IStatistic<T> {
public:
    VarianceStatistic() : mean(0.0L), m2(0.0L), count(0) {}

    void Add(const T& value) override {
        long double x = static_cast<long double>(value);
        ++count;
        long double delta = x - mean;
        mean += delta / static_cast<long double>(count);
        long double delta2 = x - mean;
        m2 += delta * delta2;
    }

    long long GetCount() const {
        return count;
    }

    double GetVariance() const {
        if (count < 2) {
            throw std::runtime_error("not enough data for variance");
        }
        return static_cast<double>(m2 / static_cast<long double>(count - 1));
    }

    double GetStdDev() const {
        return std::sqrt(GetVariance());
    }

    double GetMean() const {
        if (count == 0) {
            throw std::runtime_error("no data for mean");
        }
        return static_cast<double>(mean);
    }

private:
    long double mean;
    long double m2;
    long long count;
};

// ------------------------ Min / Max ------------------------

template<typename T>
class MinMaxStatistic : public IStatistic<T> {
public:
    MinMaxStatistic()
        : hasValue(false),
          minValue(),
          maxValue() {}

    void Add(const T& value) override {
        if (!hasValue) {
            minValue = value;
            maxValue = value;
            hasValue = true;
        } else {
            if (value < minValue) minValue = value;
            if (value > maxValue) maxValue = value;
        }
    }

    bool HasValue() const {
        return hasValue;
    }

    T GetMin() const {
        if (!hasValue) {
            throw std::runtime_error("no data for min");
        }
        return minValue;
    }

    T GetMax() const {
        if (!hasValue) {
            throw std::runtime_error("no data for max");
        }
        return maxValue;
    }

private:
    bool hasValue;
    T minValue;
    T maxValue;
};

// ------------------------ BinaryHeap for median ------------------------

template<typename T, bool MinHeap>
class BinaryHeap {
public:
    BinaryHeap()
        : data(16),
          count(0)
    {
    }

    bool Empty() const {
        return count == 0;
    }

    int Size() const {
        return count;
    }

    T Top() const {
        if (count == 0) {
            throw std::runtime_error("heap is empty");
        }
        return data.Get(0);
    }

    void Push(const T& value) {
        if (count == data.GetSize()) {
            int newCap = data.GetSize() == 0 ? 1 : data.GetSize() * 2;
            data.Resize(newCap);
        }
        data.Set(count, value);
        siftUp(count);
        ++count;
    }

    void Pop() {
        if (count == 0) {
            throw std::runtime_error("heap is empty");
        }
        --count;
        if (count > 0) {
            T last = data.Get(count);
            data.Set(0, last);
            siftDown(0);
        }
    }

private:
    DynamicArray<T> data;
    int count;

    static bool better(const T& a, const T& b) {
        if (MinHeap) {
            return a < b;
        } else {
            return a > b;
        }
    }

    void siftUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            T cur = data.Get(idx);
            T par = data.Get(parent);
            if (!better(cur, par)) break;
            // swap data[idx], data[parent]
            data.Set(idx, par);
            data.Set(parent, cur);
            idx = parent;
        }
    }

    void siftDown(int idx) {
        while (true) {
            int left = idx * 2 + 1;
            int right = idx * 2 + 2;
            int best = idx;

            if (left < count) {
                T leftVal = data.Get(left);
                T bestVal = data.Get(best);
                if (better(leftVal, bestVal)) {
                    best = left;
                }
            }
            if (right < count) {
                T rightVal = data.Get(right);
                T bestVal = data.Get(best);
                if (better(rightVal, bestVal)) {
                    best = right;
                }
            }
            if (best == idx) break;

            T vIdx = data.Get(idx);
            T vBest = data.Get(best);
            data.Set(idx, vBest);
            data.Set(best, vIdx);
            idx = best;
        }
    }
};

// ------------------------ Median ------------------------

template<typename T>
class MedianStatistic : public IStatistic<T> {
public:
    MedianStatistic() : left(), right(), totalCount(0) {}

    void Add(const T& value) override {
        if (left.Empty() || value <= left.Top()) {
            left.Push(value);
        } else {
            right.Push(value);
        }

        // rebalance: left can exceed right by at most 1
        if (left.Size() > right.Size() + 1) {
            T moved = left.Top();
            left.Pop();
            right.Push(moved);
        } else if (right.Size() > left.Size()) {
            T moved = right.Top();
            right.Pop();
            left.Push(moved);
        }
        ++totalCount;
    }

    long long GetCount() const {
        return totalCount;
    }

    double GetMedian() const {
        if (totalCount == 0) {
            throw std::runtime_error("no data for median");
        }
        if (left.Size() > right.Size()) {
            return static_cast<double>(left.Top());
        } else {
            // even number of elements
            double a = static_cast<double>(left.Top());
            double b = static_cast<double>(right.Top());
            return (a + b) * 0.5;
        }
    }

private:
    BinaryHeap<T, false> left;  // max-heap for lower half
    BinaryHeap<T, true> right;  // min-heap for upper half
    long long totalCount;
};

// ------------------------ OnlineStatistics aggregator ------------------------

template<typename T>
class OnlineStatistics {
public:
    OnlineStatistics(bool withMean,
                     bool withVariance,
                     bool withMinMax,
                     bool withMedian)
        : useMean(withMean),
          useVariance(withVariance),
          useMinMax(withMinMax),
          useMedian(withMedian),
          count(0)
    {
    }

    void Add(const T& value) {
        if (useMean) {
            meanStat.Add(value);
        }
        if (useVariance) {
            varStat.Add(value);
        }
        if (useMinMax) {
            minmaxStat.Add(value);
        }
        if (useMedian) {
            medianStat.Add(value);
        }
        ++count;
    }

    long long GetCount() const {
        return count;
    }

    bool HasMean() const { return useMean; }
    bool HasVariance() const { return useVariance; }
    bool HasMinMax() const { return useMinMax; }
    bool HasMedian() const { return useMedian; }

    double GetMean() const {
        if (!useMean) throw std::runtime_error("mean is disabled");
        return meanStat.GetMean();
    }

    double GetVariance() const {
        if (!useVariance) throw std::runtime_error("variance is disabled");
        return varStat.GetVariance();
    }

    double GetStdDev() const {
        if (!useVariance) throw std::runtime_error("variance is disabled");
        return varStat.GetStdDev();
    }

    T GetMin() const {
        if (!useMinMax) throw std::runtime_error("min/max is disabled");
        return minmaxStat.GetMin();
    }

    T GetMax() const {
        if (!useMinMax) throw std::runtime_error("min/max is disabled");
        return minmaxStat.GetMax();
    }

    double GetMedian() const {
        if (!useMedian) throw std::runtime_error("median is disabled");
        return medianStat.GetMedian();
    }

private:
    bool useMean;
    bool useVariance;
    bool useMinMax;
    bool useMedian;

    long long count;

    MeanStatistic<T> meanStat;
    VarianceStatistic<T> varStat;
    MinMaxStatistic<T> minmaxStat;
    MedianStatistic<T> medianStat;
};

#endif
