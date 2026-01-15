// interpreter.h
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
 *  - скрытой выборки по умолчанию нет.
 */
class Interpreter {
public:
    /*
     * Конструктор интерпретатора.
     *
     * @param seed начальное значение для генератора случайных чисел.
     *             По умолчанию используется фиксированное значение,
     *             что обеспечивает воспроизводимость результатов.
     */
    explicit Interpreter(unsigned int seed = 5489u);

    /*
     * Установить поток вывода для оператора print.
     *
     * По умолчанию используется std::cout.
     */
    void SetOutputStream(std::ostream& os);

    /*
     * Выполнить программу целиком.
     *
     * @param program корневой узел AST, представляющий программу.
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

    Environment env;      // Окружение исполнения (переменные и выборки)
    std::mt19937 rng;     // Генератор случайных чисел
    std::ostream* out;    // Поток вывода для print

    // =========================================================
    //               Исполнение операторов
    // =========================================================

    /*
     * Выполнить один оператор AST.
     */
    void ExecuteStatement(Stmt* stmt);

    /*
     * Выполнить блок операторов.
     */
    void ExecuteBlock(BlockStmt* block);

    // =========================================================
    //               Вычисление выражений
    // =========================================================

    /*
     * Вычислить значение выражения.
     */
    Value EvaluateExpression(Expr* expr);

    // =========================================================
    //               Вспомогательные методы
    // =========================================================

    /*
     * Вывести значение в поток вывода.
     */
    void PrintValue(const Value& v);

    /*
     * print_stat отключён (в рамках отказа от неявного контекста).
     * Оставлен для совместимости AST/Parser и явного сообщения об ошибке.
     */
    void ExecutePrintStat(const std::string& what);

    /*
     * Получить значение статистической функции по выборке:
     *   get_stat("mean", A)
     *   get_stat("variance", A)
     *   get_stat("stddev", A)
     *   get_stat("median", A)
     *   get_stat("count", A)
     */
    double GetSampleStat(const std::string& statName, const std::string& sampleName);

    /*
     * Получить значение статистической функции по выборке с параметром k:
     *   get_stat("moment", A, k)
     *   get_stat("central_moment", A, k)
     */
    double GetSampleStat(const std::string& statName, const std::string& sampleName, std::size_t k);
};