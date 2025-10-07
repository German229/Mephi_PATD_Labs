#pragma once
#include <cassert>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "UnqPtr.hpp"
#include "ShrdPtr.hpp"
#include "Sequence.hpp"

// ==== Платформенные инклюды (на верхнем уровне!) ====
#if defined(__APPLE__)
  #include <mach/mach.h>
#elif defined(__linux__)
  #include <sys/resource.h>
#endif

// ==== Тестовые типы ====
struct TestBase {
    virtual ~TestBase() = default;
    virtual const char* name() const { return "Base"; }
};
struct TestDerived : TestBase {
    const char* name() const override { return "Derived"; }
};

// Для проверки утечек
struct TestTracked {
    static inline int alive = 0;
    int v{0};
    TestTracked() { ++alive; }
    explicit TestTracked(int x): v(x) { ++alive; }
    TestTracked(const TestTracked& o): v(o.v) { ++alive; }
    TestTracked(TestTracked&& o) noexcept: v(o.v) { ++alive; o.v = 0; }
    ~TestTracked(){ --alive; }
};

// ==== RSS (память процесса) ====
inline std::uint64_t tests_rss_bytes() {
#if defined(__APPLE__)
    mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
        return static_cast<std::uint64_t>(info.resident_size);
    return 0;
#elif defined(__linux__)
    rusage ru{};
    if (getrusage(RUSAGE_SELF, &ru) == 0)
        return static_cast<std::uint64_t>(ru.ru_maxrss) * 1024ULL;
    return 0;
#else
    return 0;
#endif
}

// ==== Функциональные тесты ====
inline void run_functional_tests_header_only() {
    // UnqPtr
    {
        UnqPtr<int> up(new int(42));
        assert(up && *up == 42);
        int* raw = up.release();
        assert(!up && *raw == 42);
        delete raw;
    }
    // ShrdPtr copy
    {
        UnqPtr<int> up(new int(99));
        ShrdPtr<int> sp(std::move(up));
        assert(sp && *sp == 99 && sp.use_count() == 1);
        ShrdPtr<int> sp2 = sp;
        assert(sp2 && *sp2 == 99 && sp.use_count() == 2 && sp2.use_count() == 2);
    }
    // Подтипизация
    {
        UnqPtr<TestDerived> up(new TestDerived());
        ShrdPtr<TestDerived> sd(std::move(up));
        assert(sd && std::string(sd->name()) == "Derived");
        ShrdPtr<TestBase> sb = sd;
        assert(sb && std::string(sb->name()) == "Derived");
    }
    // Контейнер Sequence<T>
    {
        Sequence<TestBase> seq;
        { UnqPtr<TestDerived> u(new TestDerived()); seq.push_unq(std::move(u)); }
        { UnqPtr<TestDerived> u(new TestDerived()); ShrdPtr<TestDerived> sd(std::move(u)); ShrdPtr<TestBase> sb = sd; seq.push(sb); }
        assert(seq.size() == 2);
        for (std::size_t i=0;i<seq.size();++i) {
            auto& p = seq.at(i);
            assert(p && std::string(p->name()) == "Derived");
        }
    }
}

// ==== Бенчмарки (время + RSS) ====
struct BenchResult { const char* variant; int N; long long ms; std::uint64_t rss; };

inline BenchResult bench_raw_header_only(int N) {
    using clock = std::chrono::high_resolution_clock;
    auto t0 = clock::now();
    for (int i=0;i<N;++i) {
        TestTracked* p = new TestTracked(i);
        if (!p || p->v < -1) std::abort();
        delete p;
    }
    auto t1 = clock::now();
    return {"raw", N, std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count(), tests_rss_bytes()};
}

inline BenchResult bench_shared_header_only(int N) {
    using clock = std::chrono::high_resolution_clock;
    auto t0 = clock::now();
    for (int i=0;i<N;++i) {
        UnqPtr<TestTracked> u(new TestTracked(i));
        ShrdPtr<TestTracked> s(std::move(u));
        { ShrdPtr<TestTracked> s2 = s; if (!s2) std::abort(); }
    }
    auto t1 = clock::now();
    return {"shared", N, std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count(), tests_rss_bytes()};
}

inline void write_csv_header_only(const BenchResult& a, const BenchResult& b, const std::string& path = "bench.csv") {
    std::ofstream f(path, std::ios::trunc);
    if (!f) return; // молча выходим, если не удалось открыть
    f << "variant,N,ms,rss_bytes\n";
    f << a.variant << "," << a.N << "," << a.ms << "," << a.rss << "\n";
    f << b.variant << "," << b.N << "," << b.ms << "," << b.rss << "\n";
}


// ==== Запуск всего комплекта ====
inline void run_all_tests_header_only(int N) {
    run_functional_tests_header_only();

    // Проверка утечек на больших N
    {
        for (int i=0;i<N;++i) {
            UnqPtr<TestTracked> u(new TestTracked(i));
            ShrdPtr<TestTracked> s(std::move(u));
            { ShrdPtr<TestTracked> s2 = s; assert(s2); }
        }
        assert(TestTracked::alive == 0 && "Memory leak detected!");
    }

    // Бенч
    BenchResult r1 = bench_raw_header_only(N);
    BenchResult r2 = bench_shared_header_only(N);
    std::cout << "raw:    N=" << r1.N << " ms=" << r1.ms << " rss=" << r1.rss << "\n";
    std::cout << "shared: N=" << r2.N << " ms=" << r2.ms << " rss=" << r2.rss << "\n";
    write_csv_header_only(r1, r2);

    std::cout << "All tests passed! (N=" << N << ")\n";
}
