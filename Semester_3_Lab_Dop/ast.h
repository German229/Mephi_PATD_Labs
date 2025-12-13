#pragma once

#include <memory>
#include <string>
#include <vector>

// Вперёд-объявления базовых классов
struct Expr;
struct Stmt;

// ========================= ВЫРАЖЕНИЯ =========================

// Базовый класс для всех выражений
struct Expr {
    virtual ~Expr() = default;
};

// Числовой литерал, например: 3.14
struct NumberExpr : public Expr {
    double value;

    explicit NumberExpr(double v)
        : value(v) {}
};

// Переменная, например: x, sum
struct VariableExpr : public Expr {
    std::string name;

    explicit VariableExpr(std::string n)
        : name(std::move(n)) {}
};

// Унарная операция, например: -x
enum class UnaryOp {
    Negate // -
};

struct UnaryExpr : public Expr {
    UnaryOp op;
    std::unique_ptr<Expr> operand;

    UnaryExpr(UnaryOp o, std::unique_ptr<Expr> expr)
        : op(o), operand(std::move(expr)) {}
};

// Бинарная операция: +, -, *, /
enum class BinaryOp {
    Add,       // +
    Subtract,  // -
    Multiply,  // *
    Divide     // /
};

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

// Вызов функции: f(arg1, arg2, ...)
struct CallExpr : public Expr {
    std::string callee; // имя функции: uniform, normal, mean и т.п.
    std::vector<std::unique_ptr<Expr>> args;

    CallExpr(std::string name,
             std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(name)),
          args(std::move(arguments)) {}
};

// ========================= ОПЕРАТОРЫ (STATEMENTS) =========================

// Базовый класс для всех операторов
struct Stmt {
    virtual ~Stmt() = default;
};

// Присваивание: x = expr
struct AssignStmt : public Stmt {
    std::string name;                 // имя переменной
    std::unique_ptr<Expr> expression; // выражение справа

    AssignStmt(std::string n, std::unique_ptr<Expr> expr)
        : name(std::move(n)),
          expression(std::move(expr)) {}
};

// Оператор print expr
struct PrintStmt : public Stmt {
    std::unique_ptr<Expr> expression;

    explicit PrintStmt(std::unique_ptr<Expr> expr)
        : expression(std::move(expr)) {}
};

// Оператор print_stat("name")
struct PrintStatStmt : public Stmt {
    std::string sampleName; // имя выборки / статистики

    explicit PrintStatStmt(std::string name)
        : sampleName(std::move(name)) {}
};

// Оператор collect expr
struct CollectStmt : public Stmt {
    std::unique_ptr<Expr> expression;

    explicit CollectStmt(std::unique_ptr<Expr> expr)
        : expression(std::move(expr)) {}
};

// Блок { stmt1; stmt2; ... } — в нашем языке это тело repeat
struct BlockStmt : public Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    BlockStmt() = default;

    // Удобный метод для добавления оператора в блок
    void AddStatement(std::unique_ptr<Stmt> stmt) {
        statements.push_back(std::move(stmt));
    }
};

// Оператор repeat N { ... }
struct RepeatStmt : public Stmt {
    std::unique_ptr<Expr> countExpr;   // выражение, задающее число повторений
    std::unique_ptr<BlockStmt> body;   // тело цикла

    RepeatStmt(std::unique_ptr<Expr> count,
               std::unique_ptr<BlockStmt> block)
        : countExpr(std::move(count)),
          body(std::move(block)) {}
};

// Корень программы: последовательность операторов верхнего уровня
struct Program {
    std::vector<std::unique_ptr<Stmt>> statements;

    Program() = default;

    void AddStatement(std::unique_ptr<Stmt> stmt) {
        statements.push_back(std::move(stmt));
    }
};