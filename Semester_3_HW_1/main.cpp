#include <iostream>
#include <cassert>
#include "UnqPtr.hpp"
#include "ShrdPtr.hpp"

struct Base {
    virtual ~Base() = default;
    virtual const char* name() const { return "Base"; }
};
struct Derived : Base {
    const char* name() const override { return "Derived"; }
};

// для проверки утечек
struct Tracked {
    static inline int alive = 0;
    int v{0};
    Tracked() { ++alive; }
    explicit Tracked(int x): v(x) { ++alive; }
    Tracked(const Tracked& o): v(o.v) { ++alive; }
    Tracked(Tracked&& o) noexcept: v(o.v) { ++alive; o.v = 0; }
    ~Tracked(){ --alive; }
};

int main(int argc, char** argv) {
    int N = 100000; // дефолт для нагрузочного теста
    if (argc > 1) {
        try { N = std::stoi(argv[1]); } catch(...) {}
    }

    // --- Тест 1: UnqPtr базовый функционал ---
    {
        UnqPtr<int> up(new int(42));
        assert(up && *up == 42);

        int* raw = up.release();
        assert(!up && *raw == 42);
        delete raw;
    }

    // --- Тест 2: ShrdPtr копирование ---
    {
        UnqPtr<int> up(new int(99));
        ShrdPtr<int> sp(std::move(up));
        assert(sp && *sp == 99);
        assert(sp.use_count() == 1);

        ShrdPtr<int> sp2 = sp;
        assert(sp2 && *sp2 == 99);
        assert(sp.use_count() == 2);
        assert(sp2.use_count() == 2);
    }

    // --- Тест 3: ShrdPtr подтипизация ---
    {
        UnqPtr<Derived> up(new Derived());
        ShrdPtr<Derived> sd(std::move(up));
        assert(sd && sd->name() == std::string("Derived"));
        assert(sd.use_count() == 1);

        ShrdPtr<Base> sb = sd; // Derived -> Base
        assert(sb && sb->name() == std::string("Derived"));
        assert(sb.use_count() == 2);
        assert(sd.use_count() == 2);
    }

    // --- Тест 4: Нагрузочный тест и проверка утечек ---
    {
        for (int i = 0; i < N; ++i) {
            UnqPtr<Tracked> up(new Tracked(i));
            ShrdPtr<Tracked> sp(std::move(up));
            { ShrdPtr<Tracked> sp2 = sp; assert(sp2); }
        }
        assert(Tracked::alive == 0 && "Memory leak detected!");
    }

    std::cout << "All tests passed! (N=" << N << ")\n";
    return 0;
}
