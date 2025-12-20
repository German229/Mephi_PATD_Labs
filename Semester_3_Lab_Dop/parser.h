#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "lexer.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(Lexer& lex);

    Program ParseProgram();

private:
    Lexer& lexer;
    Token current;

    void Advance();
    bool Match(TokenType t);
    bool Check(TokenType t) const;
    void Expect(TokenType t, const std::string& msg);

    [[noreturn]] void Error(const std::string& msg);

    // Statements
    std::unique_ptr<Stmt> ParseStatement();
    std::unique_ptr<BlockStmt> ParseBlock();

    // Expressions (precedence levels)
    std::unique_ptr<Expr> ParseExpression();        // entry
    std::unique_ptr<Expr> ParseEquality();          // == !=
    std::unique_ptr<Expr> ParseComparison();        // < > <= >=
    std::unique_ptr<Expr> ParseAdditive();          // + -
    std::unique_ptr<Expr> ParseMultiplicative();    // * /
    std::unique_ptr<Expr> ParseUnary();             // unary -
    std::unique_ptr<Expr> ParsePrimary();           // numbers, identifiers, calls, (...)

    void ParseCallArguments(CallExpr& call);
};