#pragma once

#include <cstddef>
#include <ostream>
#include <random>
#include <string>

#include "ast.h"
#include "environment.h"
#include "statlib.h"
#include "value.h"

/*
 * Класс Interpreter реализует интерпретатор языка ProbabilityScript.
 *
 * Интерпретатор:
 *  - обходит абстрактное синтаксическое дерево (AST),
 *  - вычисляет выражения,
 *  - исполняет операторы,
 *  - управляет окружением исполнения (переменные и выборки),
 *  - предоставляет встроенные функции распределений и статистики.
 *
 * Важно:
 *  - статистика в языке работает только через явную выборку:
 *      get_stat("mean", A)
 *      get_stat("moment", A, 2)
 *      get_stat("covariance", A, B)
 *      get_stat("correlation", A, B)
 *  - скрытой выборки по умолчанию нет.
 */
class Interpreter {
public:
    /*
     * Конструктор интерпретатора.
     *
     * @param seed начальное значение для генератора случайных чисел.
     */
    explicit Interpreter(unsigned int seed = 5489u);

    /*
     * Установить поток вывода для оператора print.
     */
    void SetOutputStream(std::ostream& os);

    /*
     * Выполнить программу целиком.
     */
    void ExecuteProgram(const Program& program);

    /*
     * Получить доступ к окружению исполнения.
     */
    Environment& GetEnvironment() { return env; }
    const Environment& GetEnvironment() const { return env; }

private:
    // =========================================================
    //                 Внутреннее состояние
    // =========================================================

    Environment env;
    std::mt19937 rng;
    std::ostream* out;

    // =========================================================
    //               Исполнение операторов
    // =========================================================

    void ExecuteStatement(Stmt* stmt);
    void ExecuteBlock(BlockStmt* block);

    // =========================================================
    //               Вычисление выражений
    // =========================================================

    Value EvaluateExpression(Expr* expr);

    // =========================================================
    //               Вспомогательные методы
    // =========================================================

    void PrintValue(const Value& v);

    /*
     * print_stat отключён.
     */
    void ExecutePrintStat(const std::string& what);

    /*
     * 2-аргументная статистика:
     *   get_stat("mean", A)
     *   get_stat("variance", A)
     *   get_stat("stddev", A)
     *   get_stat("median", A)
     *   get_stat("count", A)
     */
    double GetSampleStat(const std::string& statName, const std::string& sampleName);

    /*
     * 3-аргументная статистика с k:
     *   get_stat("moment", A, k)
     *   get_stat("central_moment", A, k)
     */
    double GetSampleStat(const std::string& statName, const std::string& sampleName, std::size_t k);

    /*
     * 3-аргументная статистика по двум выборкам:
     *   get_stat("covariance", A, B)
     *   get_stat("correlation", A, B)
     *   get_stat("corr", A, B)
     */
    double GetSampleStat(const std::string& statName, const std::string& sampleX, const std::string& sampleY);
};