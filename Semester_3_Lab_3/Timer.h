#pragma once
#include <chrono>

struct Timer {
    using clock = std::chrono::high_resolution_clock;
    clock::time_point t0;
    void start() { t0 = clock::now(); }
    long long ms() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - t0).count();
    }
};
