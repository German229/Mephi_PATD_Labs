#ifndef TESTS_STATISTICS_H
#define TESTS_STATISTICS_H

#include <cassert>
#include <sstream>
#include <cmath>
#include <iostream>

#include "sequence.h"
#include "LazySequence.h"
#include "Streams.h"
#include "OnlineStatistics.h"

// --------------- LazySequence tests ---------------

void TestLazySequenceBasic() {
    // generator: f(i) = 2 * i
    int generatorCalls = 0;
    auto gen = [&generatorCalls](const LazySequence<int>&, int index) -> int {
        ++generatorCalls;
        return 2 * index;
    };

    LazySequence<int> seq(gen, 10);

    // length
    assert(seq.GetLength() == 10);

    // first/last
    assert(seq.GetFirst() == 0);
    assert(seq.GetLast() == 18);

    // random access
    assert(seq.Get(0) == 0);
    assert(seq.Get(1) == 2);
    assert(seq.Get(5) == 10);

    // проверяем, что повторный доступ к уже материализованному элементу
    // не вызывает генератор ещё раз
    int prevCalls = generatorCalls;
    int v1 = seq.Get(5);
    (void)v1;
    assert(generatorCalls == prevCalls);

    // subsequence [2..5] = {4, 6, 8, 10}
    Sequence<int>* sub = seq.GetSubsequence(2, 5);
    assert(sub->GetLength() == 4);
    assert(sub->Get(0) == 4);
    assert(sub->Get(1) == 6);
    assert(sub->Get(2) == 8);
    assert(sub->Get(3) == 10);
    delete sub;

    // append
    Sequence<int>* appended = seq.Append(100);
    assert(appended->GetLength() == 11);
    assert(appended->Get(10) == 100);
    delete appended;

    // prepend
    Sequence<int>* prepended = seq.Prepend(-2);
    assert(prepended->GetLength() == 11);
    assert(prepended->Get(0) == -2);
    assert(prepended->Get(1) == 0);
    delete prepended;

    // concat
    int arr[3] = {100, 200, 300};
    ImmutableArraySequence<int> arrSeq(arr, 3);
    Sequence<int>* conc = seq.Concat(arrSeq);
    assert(conc->GetLength() == seq.GetLength() + 3);
    assert(conc->Get(seq.GetLength()) == 100);
    assert(conc->Get(seq.GetLength() + 2) == 300);
    delete conc;
}

// --------------- Streams tests ---------------

void TestReadOnlyStreamFromSequence() {
    int arr[5] = {1, 2, 3, 4, 5};
    ImmutableArraySequence<int> base(arr, 5);
    Sequence<int>* seq = &base;

    ReadOnlyStream<int> stream(seq);

    int x = 0;
    for (int expected = 1; expected <= 5; ++expected) {
        bool ok = stream.TryRead(x);
        assert(ok);
        assert(x == expected);
    }

    // now stream must be at end
    assert(stream.IsEndOfStream());
    assert(stream.GetPosition() == 5);

    bool ok2 = stream.TryRead(x);
    assert(!ok2);
}

void TestReadOnlyStreamFromIStream() {
    std::stringstream ss;
    ss << "10 20 30";

    auto deser = [](std::istream& in, int& value) -> bool {
        return static_cast<bool>(in >> value);
    };

    ReadOnlyStream<int> stream(ss, deser);

    int v = 0;
    bool ok = false;

    ok = stream.TryRead(v);
    assert(ok && v == 10);

    ok = stream.TryRead(v);
    assert(ok && v == 20);

    ok = stream.TryRead(v);
    assert(ok && v == 30);

    ok = stream.TryRead(v);
    assert(!ok);
    assert(stream.IsEndOfStream());
    assert(stream.GetPosition() == 3);
}

void TestWriteOnlyStreamToOStream() {
    std::ostringstream oss;

    auto ser = [](std::ostream& out, const int& value) {
        out << value << " ";
    };

    WriteOnlyStream<int> stream(oss, ser);

    stream.Write(7);
    stream.Write(8);
    stream.Write(9);

    assert(stream.GetPosition() == 3);

    std::string out = oss.str();
    // expected "7 8 9 " (space after each)
    assert(out == "7 8 9 ");
}

// --------------- OnlineStatistics tests ---------------

void TestOnlineStatisticsBasic() {
    OnlineStatistics<double> stats(true, true, true, true);

    double values[4] = {1.0, 2.0, 3.0, 4.0};
    for (int i = 0; i < 4; ++i) {
        stats.Add(values[i]);
    }

    assert(stats.GetCount() == 4);

    // mean
    double mean = stats.GetMean();
    assert(std::fabs(mean - 2.5) < 1e-9);

    // variance (sample): values 1,2,3,4 => mean = 2.5
    // deviations: -1.5, -0.5, 0.5, 1.5
    // squares: 2.25, 0.25, 0.25, 2.25 => sum = 5.0
    // sample variance = 5 / (4 - 1) = 5/3
    double var = stats.GetVariance();
    assert(std::fabs(var - 5.0 / 3.0) < 1e-9);

    // stddev
    double stddev = stats.GetStdDev();
    assert(std::fabs(stddev - std::sqrt(5.0 / 3.0)) < 1e-9);

    // min/max
    assert(stats.GetMin() == 1.0);
    assert(stats.GetMax() == 4.0);

    // median of even count: (2 + 3) / 2 = 2.5
    double med = stats.GetMedian();
    assert(std::fabs(med - 2.5) < 1e-9);

    // check median of odd count
    OnlineStatistics<int> statsOdd(true, true, true, true);
    int vals[5] = {1, 5, 2, 4, 3};
    for (int i = 0; i < 5; ++i) statsOdd.Add(vals[i]);
    double medOdd = statsOdd.GetMedian();
    assert(std::fabs(medOdd - 3.0) < 1e-9);
}

inline void RunAllNewTests() {
    std::cout << "Running LazySequence tests...\n";
    TestLazySequenceBasic();
    std::cout << "LazySequence tests OK\n";

    std::cout << "Running Streams tests...\n";
    TestReadOnlyStreamFromSequence();
    TestReadOnlyStreamFromIStream();
    TestWriteOnlyStreamToOStream();
    std::cout << "Streams tests OK\n";

    std::cout << "Running OnlineStatistics tests...\n";
    TestOnlineStatisticsBasic();
    std::cout << "OnlineStatistics tests OK\n";

    std::cout << "All new tests passed successfully.\n";
}

#endif