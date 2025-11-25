#ifndef PERFORMANCE_TESTS_H
#define PERFORMANCE_TESTS_H

#include <iostream>
#include <chrono>
#include <sstream>
#include <cmath>

#include "LazySequence.h"
#include "OnlineStatistics.h"
#include "Streams.h"

inline void PerformanceTestLazySequence(std::size_t n) {
    std::cout << "\n=== Performance test: LazySequence (n = " << n << ") ===\n";

    if (n == 0) {
        std::cout << "Nothing to test (n = 0).\n";
        return;
    }
    if (n > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        n = static_cast<std::size_t>(std::numeric_limits<int>::max());
        std::cout << "n is too large, capped to INT_MAX = " << n << "\n";
    }

    int generatorCalls = 0;
    auto gen = [&generatorCalls](const LazySequence<int>&, int index) -> int {
        ++generatorCalls;
        return index;
    };

    LazySequence<int> seq(gen, static_cast<int>(n));

    auto start = std::chrono::steady_clock::now();

    long long sum = 0;
    int length = seq.GetLength();
    for (int i = 0; i < length; ++i) {
        sum += seq.Get(i);
    }

    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Total elements read: " << length << "\n";
    std::cout << "Generator calls:     " << generatorCalls << "\n";
    std::cout << "Sum of elements:     " << sum << "\n";
    std::cout << "Elapsed time:        " << ms << " ms\n";
}

inline void PerformanceTestOnlineStatistics(std::size_t n) {
    std::cout << "\n=== Performance test: OnlineStatistics (n = " << n << ") ===\n";

    if (n == 0) {
        std::cout << "Nothing to test (n = 0).\n";
        return;
    }

    OnlineStatistics<double> stats(true, true, true, true);

    auto start = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < n; ++i) {
        stats.Add(static_cast<double>(i));
    }

    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Total elements processed: " << stats.GetCount() << "\n";
    std::cout << "Elapsed time:             " << ms << " ms\n";

    // Simple correctness check for moderate n:
    try {
        double mean = stats.GetMean();
        double theoreticalMean = (static_cast<double>(n) - 1.0) * 0.5;
        std::cout << "Mean:                     " << mean << "\n";
        std::cout << "Theoretical mean:         " << theoreticalMean << "\n";
        std::cout << "Mean error:               " << std::fabs(mean - theoreticalMean) << "\n";
    } catch (const std::exception& ex) {
        std::cout << "Mean unavailable: " << ex.what() << "\n";
    }

    try {
        std::cout << "Min:                      " << stats.GetMin() << "\n";
        std::cout << "Max:                      " << stats.GetMax() << "\n";
    } catch (const std::exception& ex) {
        std::cout << "Min/Max unavailable: " << ex.what() << "\n";
    }

    try {
        std::cout << "Median:                   " << stats.GetMedian() << "\n";
    } catch (const std::exception& ex) {
        std::cout << "Median unavailable: " << ex.what() << "\n";
    }
}

inline void PerformanceTestStream(std::size_t n) {
    std::cout << "\n=== Performance test: ReadOnlyStream from stringstream (n = " << n << ") ===\n";

    if (n == 0) {
        std::cout << "Nothing to test (n = 0).\n";
        return;
    }

    std::stringstream ss;
    for (std::size_t i = 0; i < n; ++i) {
        ss << i << ' ';
    }

    auto deserializer = [](std::istream& in, long long& value) -> bool {
        return static_cast<bool>(in >> value);
    };

    ReadOnlyStream<long long> stream(ss, deserializer);

    auto start = std::chrono::steady_clock::now();

    long long sum = 0;
    std::size_t count = 0;
    long long x = 0;
    while (count < n && stream.TryRead(x)) {
        sum += x;
        ++count;
    }

    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Total elements read: " << count << "\n";
    std::cout << "Sum of elements:     " << sum << "\n";
    std::cout << "Elapsed time:        " << ms << " ms\n";
}

inline void RunPerformanceTests() {
    std::cout << "\n===== Performance tests =====\n";
    std::cout << "Enter n (number of elements, e.g. 1000000): ";
    std::size_t n = 0;
    if (!(std::cin >> n)) {
        std::cout << "Invalid input.\n";
        return;
    }

    PerformanceTestLazySequence(n);
    PerformanceTestOnlineStatistics(n);
    PerformanceTestStream(n);

    std::cout << "\nAll performance tests finished.\n";
}

#endif
