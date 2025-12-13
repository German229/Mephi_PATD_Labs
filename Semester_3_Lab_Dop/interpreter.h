#pragma once

#include <random>
#include <ostream>

#include "ast.h"
#include "environment.h"
#include "statlib.h"
#include "value.h"

// Интерпретатор языка ProbabilityScript.
// Он обходит AST (Program), поддерживает переменные, collect, repeat,
// встроенные функции распределений и статистики.
class Interpreter {
public:
    // seed – для генератора случайных чисел (по умолчанию фиксированный, чтобы тесты были воспроизводимыми).
    explicit Interpreter(unsigned int seed = 5489u);

    // Можно переназначить поток вывода (по умолчанию std::cout).
    void SetOutputStream(std::ostream& os);

    // Выполнить программу целиком.
    void ExecuteProgram(const Program& program);

    // Доступ к окружению (переменные и выборки).
    Environment& GetEnvironment() { return env; }
    const Environment& GetEnvironment() const { return env; }

private:
    Environment env;
    std::mt19937 rng;
    std::ostream* out;  // куда печатаем print и print_stat

    // Выполнение операторов / блоков
    void ExecuteStatement(Stmt* stmt);
    void ExecuteBlock(BlockStmt* block);

    // Вычисление выражений
    Value EvaluateExpression(Expr* expr);

    // Вспомогательные методы
    void PrintValue(const Value& v);
    void ExecutePrintStat(const std::string& what);

    double GetSampleStat(const std::string& statName);
};
