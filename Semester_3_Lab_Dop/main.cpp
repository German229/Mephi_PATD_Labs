#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "tests.h"

static std::string ReadWholeFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open script file: " + path);
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

static void PrintUsage(const char* progName) {
    std::cout
        << "ProbabilityScript runner\n\n"
        << "Usage:\n"
        << "  " << progName << "                   Run unit tests\n"
        << "  " << progName << " <file.psc>         Run script\n"
        << "  " << progName << " --seed N <file.psc>  Run script with fixed RNG seed\n"
        << "  " << progName << " --test             Run unit tests\n"
        << "  " << progName << " --help             Show help\n\n"
        << "Examples:\n"
        << "  " << progName << " scripts/script1.psc\n"
        << "  " << progName << " --seed 42 scripts/script2.psc\n";
}

int main(int argc, char** argv) {
    try {
        unsigned int seed = 123;
        bool seedProvided = false;
        bool forceTests = false;
        std::string scriptPath;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                PrintUsage(argv[0]);
                return 0;
            }

            if (arg == "--test") {
                forceTests = true;
                continue;
            }

            if (arg == "--seed") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("Expected number after --seed");
                }
                seed = static_cast<unsigned int>(std::stoul(argv[++i]));
                seedProvided = true;
                continue;
            }

            if (arg.rfind("--seed=", 0) == 0) {
                seed = static_cast<unsigned int>(std::stoul(arg.substr(7)));
                seedProvided = true;
                continue;
            }

            // Only one positional argument allowed — script path
            if (scriptPath.empty()) {
                scriptPath = arg;
            } else {
                throw std::runtime_error("Unexpected extra argument: " + arg);
            }
        }

        // Run tests if requested explicitly or no script provided
        if (forceTests || scriptPath.empty()) {
            RunAllTests();
            return 0;
        }

        // Run script file
        std::string source = ReadWholeFile(scriptPath);
        Lexer lexer(source);
        Parser parser(lexer);
        Program program = parser.ParseProgram();

        Interpreter interp(seed);
        interp.ExecuteProgram(program);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Use --help for usage.\n";
        return 1;
    }
}
