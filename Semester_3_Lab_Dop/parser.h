#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "lexer.h"
#include "ast.h"

// Парсер ProbabilityScript.
// Делает: Lexer -> AST (Program).
class Parser {
public:
    explicit Parser(Lexer& lex);

    Program ParseProgram();

private:
    Lexer& lexer;
    Token current; // lookahead

    void Advance();
    bool Match(TokenType t);
    void Expect(TokenType t, const std::string& msg);

    [[noreturn]] void Error(const std::string& msg);

    // Statements
    std::unique_ptr<Stmt> ParseStatement();
    std::unique_ptr<BlockStmt> ParseBlock();

    // Expressions (с приоритетами)
    std::unique_ptr<Expr> ParseExpression(); // +,-
    std::unique_ptr<Expr> ParseTerm();       // *,/
    std::unique_ptr<Expr> ParseUnary();
    std::unique_ptr<Expr> ParsePrimary();

    // NEW: аргументы вызова функции
    void ParseCallArguments(CallExpr& call);
};