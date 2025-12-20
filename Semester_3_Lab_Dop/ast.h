#pragma once

#include <memory>
#include <string>
#include <vector>

// Вперёд-объявления
struct Expr;
struct Stmt;

// ========================= ВЫРАЖЕНИЯ =========================

struct Expr {
    virtual ~Expr() = default;
};

struct NumberExpr : public Expr {
    double value;
    explicit NumberExpr(double v) : value(v) {}
};

struct VariableExpr : public Expr {
    std::string name;
    explicit VariableExpr(std::string n) : name(std::move(n)) {}
};

enum class UnaryOp {
    Negate // -
};

struct UnaryExpr : public Expr {
    UnaryOp op;
    std::unique_ptr<Expr> operand;

    UnaryExpr(UnaryOp o, std::unique_ptr<Expr> expr)
        : op(o), operand(std::move(expr)) {}
};

enum class BinaryOp {
    Add, Subtract, Multiply, Divide,
    Greater, Less, Equal, NotEqual, GreaterEqual, LessEqual
};

struct BinaryExpr : public Expr {
    BinaryOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    BinaryExpr(BinaryOp o, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : op(o), left(std::move(lhs)), right(std::move(rhs)) {}
};

struct CallExpr : public Expr {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;

    CallExpr(std::string name, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(name)), args(std::move(arguments)) {}
};

// ========================= ОПЕРАТОРЫ =========================

struct Stmt {
    virtual ~Stmt() = default;
};

struct AssignStmt : public Stmt {
    std::string name;
    std::unique_ptr<Expr> expression;

    AssignStmt(std::string n, std::unique_ptr<Expr> expr)
        : name(std::move(n)), expression(std::move(expr)) {}
};

struct PrintStmt : public Stmt {
    std::unique_ptr<Expr> expression;
    explicit PrintStmt(std::unique_ptr<Expr> expr) : expression(std::move(expr)) {}
};

struct PrintStatStmt : public Stmt {
    std::string sampleName;
    explicit PrintStatStmt(std::string name) : sampleName(std::move(name)) {}
};

struct CollectStmt : public Stmt {
    std::unique_ptr<Expr> expression;
    explicit CollectStmt(std::unique_ptr<Expr> expr) : expression(std::move(expr)) {}
};

struct BlockStmt : public Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    void AddStatement(std::unique_ptr<Stmt> stmt) {
        statements.push_back(std::move(stmt));
    }
};

struct RepeatStmt : public Stmt {
    std::unique_ptr<Expr> countExpr;
    std::unique_ptr<BlockStmt> body;

    RepeatStmt(std::unique_ptr<Expr> count, std::unique_ptr<BlockStmt> block)
        : countExpr(std::move(count)), body(std::move(block)) {}
};

struct IfStmt : public Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;

    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct Program {
    std::vector<std::unique_ptr<Stmt>> statements;

    void AddStatement(std::unique_ptr<Stmt> stmt) {
        statements.push_back(std::move(stmt));
    }
};
