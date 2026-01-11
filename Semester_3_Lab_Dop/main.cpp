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

/*
 * Читает файл целиком в строку.
 *
 * Используется для загрузки исходного кода скрипта ProbabilityScript
 * перед лексическим анализом.
 */
static std::string ReadWholeFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open script file: " + path);
    }

    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

/*
 * Печатает справку по использованию программы.
 *
 * Используется при запуске с флагом --help
 * или при ошибках в аргументах командной строки.
 */
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

/*
 * Точка входа программы.
 *
 * Отвечает за:
 *  - разбор аргументов командной строки;
 *  - запуск юнит-тестов;
 *  - загрузку и выполнение скрипта ProbabilityScript.
 */
int main(int argc, char** argv) {
    try {
        // Параметры запуска
        unsigned int seed = 123;     // seed по умолчанию
        bool seedProvided = false;
        bool forceTests = false;
        std::string scriptPath;

        /*
         * Разбор аргументов командной строки.
         */
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

            // Позиционный аргумент — путь к скрипту
            if (scriptPath.empty()) {
                scriptPath = arg;
            } else {
                throw std::runtime_error("Unexpected extra argument: " + arg);
            }
        }

        /*
         * Если скрипт не задан или явно указан --test,
         * запускаем юнит-тесты.
         */
        if (forceTests || scriptPath.empty()) {
            RunAllTests();
            return 0;
        }

        /*
         * Загрузка и выполнение скрипта.
         *
         * Последовательность строго следующая:
         *   1. Чтение файла
         *   2. Лексический анализ
         *   3. Синтаксический анализ (AST)
         *   4. Интерпретация AST
         */
        std::string source = ReadWholeFile(scriptPath);

        Lexer lexer(source);
        Parser parser(lexer);
        Program program = parser.ParseProgram();

        Interpreter interp(seed);
        interp.ExecuteProgram(program);

        return 0;
    }
    catch (const std::exception& e) {
        // Единая точка обработки ошибок
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Use --help for usage.\n";
        return 1;
    }
}