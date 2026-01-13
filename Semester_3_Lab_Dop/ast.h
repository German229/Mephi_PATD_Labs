#pragma once

#include <memory>
#include <string>
#include <vector>

/*
 * Файл ast.h описывает абстрактное синтаксическое дерево (AST)
 * языка ProbabilityScript.
 *
 * AST представляет программу в виде иерархии узлов,
 * не зависящей от конкретного синтаксиса исходного текста.
 */

// Вперёд-объявления базовых типов
struct Expr;
struct Stmt;

// ============================================================
//                         ВЫРАЖЕНИЯ
// ============================================================

/*
 * Базовый класс для всех выражений.
 */
struct Expr {
    virtual ~Expr() = default;
};

/*
 * Числовой литерал.
 */
struct NumberExpr : public Expr {
    double value;

    explicit NumberExpr(double v)
        : value(v) {}
};

/*
 * Использование переменной.
 */
struct VariableExpr : public Expr {
    std::string name;

    explicit VariableExpr(std::string n)
        : name(std::move(n)) {}
};

/*
 * Перечисление унарных операторов.
 */
enum class UnaryOp {
    Negate // унарный минус: -
};

/*
 * Унарное выражение.
 */
struct UnaryExpr : public Expr {
    UnaryOp op;
    std::unique_ptr<Expr> operand;

    UnaryExpr(UnaryOp o, std::unique_ptr<Expr> expr)
        : op(o), operand(std::move(expr)) {}
};

/*
 * Перечисление бинарных операторов.
 */
enum class BinaryOp {
    Add,           // +
    Subtract,      // -
    Multiply,      // *
    Divide,        // /
    Greater,       // >
    Less,          // <
    Equal,         // ==
    NotEqual,      // !=
    GreaterEqual,  // >=
    LessEqual      // <=
};

/*
 * Бинарное выражение.
 */
struct BinaryExpr : public Expr {
    BinaryOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    BinaryExpr(BinaryOp o,
               std::unique_ptr<Expr> lhs,
               std::unique_ptr<Expr> rhs)
        : op(o),
          left(std::move(lhs)),
          right(std::move(rhs)) {}
};

/*
 * Вызов функции.
 *
 * Используется для встроенных функций языка
 * (uniform, normal, mean, count и т.д.).
 */
struct CallExpr : public Expr {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;

    CallExpr(std::string name,
             std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(name)),
          args(std::move(arguments)) {}
};

// ============================================================
//                         ОПЕРАТОРЫ
// ============================================================

/*
 * Базовый класс для всех операторов (statements).
 */
struct Stmt {
    virtual ~Stmt() = default;
};

/*
 * Оператор присваивания:
 *   identifier = expression
 */
struct AssignStmt : public Stmt {
    std::string name;
    std::unique_ptr<Expr> expression;

    AssignStmt(std::string n, std::unique_ptr<Expr> expr)
        : name(std::move(n)),
          expression(std::move(expr)) {}
};

/*
 * Оператор вывода значения выражения:
 *   print expression
 */
struct PrintStmt : public Stmt {
    std::unique_ptr<Expr> expression;

    explicit PrintStmt(std::unique_ptr<Expr> expr)
        : expression(std::move(expr)) {}
};

/*
 * Оператор вывода статистики:
 *   print_stat("mean")
 */
struct PrintStatStmt : public Stmt {
    std::string sampleName;

    explicit PrintStatStmt(std::string name)
        : sampleName(std::move(name)) {}
};

/*
 * Оператор сбора значения в выборку:
 *   collect expression
 */
struct CollectStmt : public Stmt {
    std::unique_ptr<Expr> expression;

    explicit CollectStmt(std::unique_ptr<Expr> expr)
        : expression(std::move(expr)) {}
};

/*
 * Блок операторов, заключённый в фигурные скобки.
 */
struct BlockStmt : public Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    /*
     * Добавить оператор в блок.
     */
    void AddStatement(std::unique_ptr<Stmt> stmt) {
        statements.push_back(std::move(stmt));
    }
};

/*
 * Цикл repeat:
 *   repeat <expr> { ... }
 */
struct RepeatStmt : public Stmt {
    std::unique_ptr<Expr> countExpr;
    std::unique_ptr<BlockStmt> body;

    RepeatStmt(std::unique_ptr<Expr> count,
               std::unique_ptr<BlockStmt> block)
        : countExpr(std::move(count)),
          body(std::move(block)) {}
};

/*
 * Условный оператор if:
 *   if <expr> { ... }
 */
struct IfStmt : public Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;

    IfStmt(std::unique_ptr<Expr> cond,
           std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)),
          body(std::move(b)) {}
};

/*
 * Корневой узел AST.
 * Представляет программу целиком.
 */
struct Program {
    std::vector<std::unique_ptr<Stmt>> statements;

    /*
     * Добавить оператор верхнего уровня.
     */
    void AddStatement(std::unique_ptr<Stmt> stmt) {
        statements.push_back(std::move(stmt));
    }
};