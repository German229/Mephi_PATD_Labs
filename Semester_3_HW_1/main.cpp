#include <iostream>
#include <sstream>
#include <string>
#include <cassert>
#include "UnqPtr.hpp"
#include "ShrdPtr.hpp"
#include "Sequence.hpp"
#include "tests.hpp"

static void run_repl() {
    std::cout << "Interactive mode. Type 'help' for commands.\n";

    Sequence<TestBase> seq;
    ShrdPtr<TestBase> last_ptr;

    // для демонстрации массивов
    ShrdPtr<int[]> last_arr;

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string cmd; iss >> cmd;
        if (cmd.empty()) continue;

        if (cmd == "help") {
            std::cout <<
                "Commands:\n"
                "  help                     - show this help\n"
                "  mkint X                  - create UnqPtr<int>(X), show value, release()\n"
                "  mkderived                - create Derived -> ShrdPtr<Derived> -> ShrdPtr<Base>, show name & use_count\n"
                "  seq_add N                - push N Derived objects into Sequence<TestBase>\n"
                "  seq_show                 - print sequence size and element names\n"
                "  seq_clear                - clear sequence by reassign (drop refs)\n"
                "  bench N                  - run raw vs shared benchmarks with N and show time/rss\n"
                "  leak N                   - create/destroy N TestTracked via ShrdPtr; ensure alive==0\n"
                "  mkarr N                  - create ShrdPtr<int[]> of size N, fill, show sample & use_count\n"
                "  arr_copy                 - copy last array to test use_count\n"
                "  arr_show I               - show last array element at index I\n"
                "  auto [N]                 - run full automatic tests (like default)\n"
                "  quit / exit              - leave\n";
        } else if (cmd == "quit" || cmd == "exit") {
            break;

        } else if (cmd == "mkint") {
            int x;
            if (!(iss >> x)) { std::cout << "usage: mkint X\n"; continue; }
            UnqPtr<int> up(new int(x));
            std::cout << "UnqPtr<int> -> *up=" << *up << "\n";
            int* raw = up.release();
            std::cout << "release() => raw=" << (raw ? std::to_string(*raw) : std::string("<null>")) << ", up is null\n";
            delete raw;

        } else if (cmd == "mkderived") {
            UnqPtr<TestDerived> u(new TestDerived());
            ShrdPtr<TestDerived> sd(std::move(u));
            ShrdPtr<TestBase> sb = sd;
            last_ptr = sb;
            std::cout << "name=" << sb->name() << " use_count=" << sb.use_count() << "\n";

        } else if (cmd == "seq_add") {
            int n; if (!(iss >> n) || n < 0) { std::cout << "usage: seq_add N\n"; continue; }
            for (int i=0;i<n;++i) {
                UnqPtr<TestDerived> u(new TestDerived());
                seq.push_unq(std::move(u));
            }
            std::cout << "Sequence size=" << seq.size() << "\n";

        } else if (cmd == "seq_show") {
            std::cout << "Sequence size=" << seq.size() << "\n";
            for (std::size_t i=0;i<seq.size();++i) {
                const auto& p = seq.at(i);
                std::cout << "  [" << i << "] " << (p ? p->name() : "<null>") << "\n";
            }

        } else if (cmd == "seq_clear") {
            seq = Sequence<TestBase>();
            std::cout << "Sequence cleared. size=" << seq.size() << "\n";

        } else if (cmd == "bench") {
            int N; if (!(iss >> N) || N <= 0) { std::cout << "usage: bench N\n"; continue; }
            auto r1 = bench_raw_header_only(N);
            auto r2 = bench_shared_header_only(N);
            std::cout << "raw:    N=" << r1.N << " ms=" << r1.ms << " rss=" << r1.rss << "\n";
            std::cout << "shared: N=" << r2.N << " ms=" << r2.ms << " rss=" << r2.rss << "\n";
            write_csv_header_only(r1, r2);

        } else if (cmd == "leak") {
            int N; if (!(iss >> N) || N <= 0) { std::cout << "usage: leak N\n"; continue; }
            TestTracked::alive = 0;
            for (int i=0;i<N;++i) {
                UnqPtr<TestTracked> u(new TestTracked(i));
                ShrdPtr<TestTracked> s(std::move(u));
                { ShrdPtr<TestTracked> s2 = s; (void)s2; }
            }
            std::cout << "alive=" << TestTracked::alive << (TestTracked::alive==0 ? " (OK)\n" : " (LEAK!)\n");

        // ===== массивы =====
        } else if (cmd == "mkarr") {
            int n; if (!(iss >> n) || n <= 0) { std::cout << "usage: mkarr N\n"; continue; }
            UnqPtr<int[]> ua(new int[n]);
            for (int i=0;i<n;++i) ua[i] = i * 10;

            // без промежуточного shared ‘sa’, сразу в last_arr (move)
            last_arr = ShrdPtr<int[]>(std::move(ua));  // use_count = 1
            std::cout << "arr size~" << n
                      << " sample: [0]=" << last_arr[0]
                      << (n>1 ? (std::string(" [1]=") + std::to_string(last_arr[1])) : "")
                      << " use_count=" << last_arr.use_count() << "\n";
        } else if (cmd == "arr_copy") {
            if (!last_arr) { std::cout << "no array yet. use mkarr N\n"; continue; }
            ShrdPtr<int[]> copy = last_arr;
            std::cout << "array copied. use_count=" << last_arr.use_count() << "\n";

        } else if (cmd == "arr_show") {
            if (!last_arr) { std::cout << "no array yet. use mkarr N\n"; continue; }
            int idx; if (!(iss >> idx) || idx < 0) { std::cout << "usage: arr_show I\n"; continue; }
            std::cout << "arr[" << idx << "]=" << last_arr[idx] << "\n";

        } else if (cmd == "auto") {
            int N = 100000;
            if (iss >> N) { if (N <= 0) { std::cout << "N must be > 0\n"; continue; } }
            run_all_tests_header_only(N);

        } else {
            std::cout << "unknown command. Type 'help'.\n";
        }
    }
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--repl") {
        run_repl();
        return 0;
    }
    int N = 100000;
    if (argc > 1 && std::string(argv[1]) == "--auto") {
        if (argc > 2) { try { N = std::stoi(argv[2]); } catch(...) {} }
    } else if (argc > 1) {
        try { N = std::stoi(argv[1]); } catch(...) {}
    }

    run_all_tests_header_only(N);
    return 0;
}
