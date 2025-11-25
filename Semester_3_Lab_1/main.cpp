#include <iostream>
#include <limits>
#include <string>
#include <fstream>

#include "sequence.h"
#include "Lists.h"
#include "LazySequence.h"
#include "Streams.h"
#include "OnlineStatistics.h"
#include "TestsStatistics.h"
#include "PerformanceTests.h"

// Demo for online statistics
void RunStatisticsDemo() {
    std::cout << "===== Online statistics demo =====\n";
    std::cout << "Select data source:\n";
    std::cout << "  1 - manual input from console\n";
    std::cout << "  2 - generated sequence (0, 1, 2, ...)\n";
    std::cout << "  3 - read from file\n";
    std::cout << "Choice: ";

    int source = 0;
    if (!(std::cin >> source)) {
        return;
    }

    std::cout << "How many values to process? (<=0 = no limit for source)\n";
    std::cout << "N = ";
    long long limit = 0;
    if (!(std::cin >> limit)) {
        return;
    }

    bool useMean = true;
    bool useVar = true;
    bool useMinMax = true;
    bool useMedian = true;

    OnlineStatistics<double> stats(useMean, useVar, useMinMax, useMedian);

    if (source == 1) {
        std::cout << "Enter numbers (non-numeric input to stop):\n";
        long long processed = 0;
        while (true) {
            double x;
            if (!(std::cin >> x)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }
            stats.Add(x);
            ++processed;
            if (limit > 0 && processed >= limit) break;
        }
    } else if (source == 2) {
        std::cout << "Generated sequence: 0, 1, 2, ...\n";
        if (limit <= 0) {
            std::cout << "Limit must be positive for generator, using 1000000\n";
            limit = 1000000;
        }
        for (long long i = 0; i < limit; ++i) {
            stats.Add(static_cast<double>(i));
        }
    } else if (source == 3) {
        std::cout << "File name (inside 'files' directory): ";
        std::string fileName;
        std::cin >> fileName;

        // Adjust this path according to your build setup
        std::string fullPath = "../Semester_3_Lab_1/files/" + fileName;

        auto deserializer = [](std::istream& in, double& value) -> bool {
            return static_cast<bool>(in >> value);
        };

        std::ifstream fin(fullPath);
        if (!fin.is_open()) {
            std::cerr << "Error opening file: " << fullPath << "\n";
            return;
        }

        try {
            ReadOnlyStream<double> stream(fin, deserializer);
            long long processed = 0;
            double x;
            while ((limit <= 0 || processed < limit) && stream.TryRead(x)) {
                stats.Add(x);
                ++processed;
            }
        } catch (const std::exception& ex) {
            std::cerr << "Error while reading stream: " << ex.what() << "\n";
            return;
        }
    } else {
        std::cout << "Unknown source.\n";
        return;
    }

    std::cout << "\n===== Results =====\n";
    std::cout << "Count: " << stats.GetCount() << "\n";

    try {
        if (stats.HasMean()) {
            std::cout << "Mean: " << stats.GetMean() << "\n";
        }
    } catch (const std::exception& ex) {
        std::cout << "Mean: error (" << ex.what() << ")\n";
    }

    try {
        if (stats.HasVariance()) {
            std::cout << "Variance: " << stats.GetVariance() << "\n";
            std::cout << "StdDev:   " << stats.GetStdDev() << "\n";
        }
    } catch (const std::exception& ex) {
        std::cout << "Variance/StdDev: error (" << ex.what() << ")\n";
    }

    try {
        if (stats.HasMinMax()) {
            std::cout << "Min: " << stats.GetMin() << "\n";
            std::cout << "Max: " << stats.GetMax() << "\n";
        }
    } catch (const std::exception& ex) {
        std::cout << "Min/Max: error (" << ex.what() << ")\n";
    }

    try {
        if (stats.HasMedian()) {
            std::cout << "Median: " << stats.GetMedian() << "\n";
        }
    } catch (const std::exception& ex) {
        std::cout << "Median: error (" << ex.what() << ")\n";
    }

    std::cout << "=============================\n";
}

int main() {
    while (true) {
        std::cout << "\n===== Main menu =====\n";
        std::cout << "1 - Online statistics demo\n";
        std::cout << "2 - Run new unit tests (LazySequence, Streams, Statistics)\n";
        std::cout << "3 - Run performance tests\n";
        std::cout << "0 - Exit\n";
        std::cout << "Choice: ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            return 0;
        }

        if (choice == 0) {
            return 0;
        } else if (choice == 1) {
            RunStatisticsDemo();
        } else if (choice == 2) {
            RunAllNewTests();
        } else if (choice == 3) {
            RunPerformanceTests();
        } else {
            std::cout << "Unknown choice.\n";
        }
    }
}
