#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "lexer.h"
#include "ast.h"

/*
 * Класс Parser реализует синтаксический анализатор языка ProbabilityScript.
 *
 * Парсер получает поток токенов от Lexer и строит абстрактное синтаксическое
 * дерево (AST), представленное структурами из ast.h.
 *
 * Используется рекурсивный нисходящий парсинг с учётом приоритетов операторов.
 */
class Parser {
public:
    /*
     * Конструктор парсера.
     *
     * @param lex ссылка на лексер, из которого будут считываться токены
     */
    explicit Parser(Lexer& lex);

    /*
     * Разобрать всю программу целиком.
     *
     * @return объект Program, содержащий список операторов верхнего уровня
     */
    Program ParseProgram();

private:
    // ================== Состояние парсера ==================

    Lexer& lexer;   // Источник токенов
    Token current;  // Текущий токен (lookahead)

    // ================== Управление токенами ==================

    /*
     * Считать следующий токен из лексера и сделать его текущим.
     */
    void Advance();

    /*
     * Проверить, совпадает ли текущий токен с заданным типом,
     * и в случае успеха продвинуться к следующему токену.
     */
    bool Match(TokenType t);

    /*
     * Проверить тип текущего токена без продвижения.
     */
    bool Check(TokenType t) const;

    /*
     * Ожидать токен заданного типа.
     * В случае несовпадения генерирует синтаксическую ошибку.
     */
    void Expect(TokenType t, const std::string& msg);

    /*
     * Сообщить об ошибке синтаксического анализа и прервать разбор.
     */
    [[noreturn]] void Error(const std::string& msg);

    // ================== Разбор операторов ==================

    /*
     * Разобрать один оператор (statement).
     */
    std::unique_ptr<Stmt> ParseStatement();

    /*
     * Разобрать блок операторов в фигурных скобках { ... }.
     */
    std::unique_ptr<BlockStmt> ParseBlock();

    // ================== Разбор выражений ==================
    // Методы организованы по уровням приоритета операторов.

    /*
     * Точка входа для разбора выражений.
     */
    std::unique_ptr<Expr> ParseExpression();

    /*
     * Операторы равенства: ==, !=
     */
    std::unique_ptr<Expr> ParseEquality();

    /*
     * Операторы сравнения: <, >, <=, >=
     */
    std::unique_ptr<Expr> ParseComparison();

    /*
     * Аддитивные операторы: +, -
     */
    std::unique_ptr<Expr> ParseAdditive();

    /*
     * Мультипликативные операторы: *, /
     */
    std::unique_ptr<Expr> ParseMultiplicative();

    /*
     * Унарные операторы (в текущей версии только унарный минус).
     */
    std::unique_ptr<Expr> ParseUnary();

    /*
     * Первичные выражения:
     * числа, идентификаторы, вызовы функций, выражения в скобках.
     */
    std::unique_ptr<Expr> ParsePrimary();

    /*
     * Разобрать аргументы вызова функции.
     */
    void ParseCallArguments(CallExpr& call);
};