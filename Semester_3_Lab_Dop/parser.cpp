#include "parser.h"
#include <iostream>
#include <cstdlib>

Parser::Parser(Lexer& lex)
    : lexer(lex),
      current(TokenType::EndOfFile, "", 1, 1)
{
    // std::cerr << "PARSER_BUILD_MARK=ADD_EQ_CMP\n";
    Advance();
}

void Parser::Advance() {
    current = lexer.NextToken();
}

bool Parser::Match(TokenType t) {
    if (current.type == t) {
        Advance();
        return true;
    }
    return false;
}

bool Parser::Check(TokenType t) const {
    return current.type == t;
}

void Parser::Expect(TokenType t, const std::string& msg) {
    if (current.type != t) {
        Error(msg);
    }
    Advance();
}

[[noreturn]] void Parser::Error(const std::string& msg) {
    throw std::runtime_error(
        "Parse error at line " + std::to_string(current.line) +
        ", column " + std::to_string(current.column) +
        ": " + msg + " (got '" + current.text + "')"
    );
}

Program Parser::ParseProgram() {
    Program prog;
    while (!Check(TokenType::EndOfFile)) {
        prog.AddStatement(ParseStatement());
    }
    return prog;
}

std::unique_ptr<Stmt> Parser::ParseStatement() {
    if (Match(TokenType::KeywordRepeat)) {
        auto countExpr = ParseExpression();
        auto body = ParseBlock();
        return std::make_unique<RepeatStmt>(std::move(countExpr), std::move(body));
    }

    if (Match(TokenType::KeywordCollect)) {
        auto expr = ParseExpression();
        return std::make_unique<CollectStmt>(std::move(expr));
    }

    if (Match(TokenType::KeywordPrint)) {
        auto expr = ParseExpression();
        return std::make_unique<PrintStmt>(std::move(expr));
    }

    if (Match(TokenType::KeywordPrintStat)) {
        Expect(TokenType::LParen, "Expected '(' after print_stat");
        if (!Check(TokenType::StringLiteral)) {
            Error("Expected string literal inside print_stat(\"...\")");
        }
        std::string name = current.text;
        Advance();
        Expect(TokenType::RParen, "Expected ')' after print_stat(\"...\")");
        return std::make_unique<PrintStatStmt>(name);
    }

    if (Match(TokenType::If)) {
        auto cond = ParseExpression();
        auto body = ParseBlock();
        return std::make_unique<IfStmt>(std::move(cond), std::move(body));
    }

    if (Check(TokenType::Identifier)) {
        std::string name = current.text;
        Advance();
        Expect(TokenType::Assign, "Expected '=' after identifier in assignment");
        auto expr = ParseExpression();
        return std::make_unique<AssignStmt>(name, std::move(expr));
    }

    if (Check(TokenType::LBrace)) {
        return ParseBlock();
    }

    Error("Expected statement");
}

std::unique_ptr<BlockStmt> Parser::ParseBlock() {
    Expect(TokenType::LBrace, "Expected '{' to start block");
    auto block = std::make_unique<BlockStmt>();

    while (!Check(TokenType::RBrace)) {
        if (Check(TokenType::EndOfFile)) {
            Error("Unterminated block: expected '}'");
        }
        block->AddStatement(ParseStatement());
    }

    Expect(TokenType::RBrace, "Expected '}' to close block");
    return block;
}

// ===== Expressions with precedence =====

std::unique_ptr<Expr> Parser::ParseExpression() {
    return ParseEquality();
}

std::unique_ptr<Expr> Parser::ParseEquality() {
    auto left = ParseComparison();

    while (true) {
        if (Match(TokenType::EqualEqual)) {
            auto right = ParseComparison();
            left = std::make_unique<BinaryExpr>(BinaryOp::Equal, std::move(left), std::move(right));
        } else if (Match(TokenType::BangEqual)) {
            auto right = ParseComparison();
            left = std::make_unique<BinaryExpr>(BinaryOp::NotEqual, std::move(left), std::move(right));
        } else {
            break;
        }
    }

    return left;
}

std::unique_ptr<Expr> Parser::ParseComparison() {
    auto left = ParseAdditive();

    while (true) {
        if (Match(TokenType::Greater)) {
            auto right = ParseAdditive();
            left = std::make_unique<BinaryExpr>(BinaryOp::Greater, std::move(left), std::move(right));

        } else if (Match(TokenType::Less)) {
            auto right = ParseAdditive();
            left = std::make_unique<BinaryExpr>(BinaryOp::Less, std::move(left), std::move(right));

        } else if (Match(TokenType::GreaterEqual)) {
            auto right = ParseAdditive();
            left = std::make_unique<BinaryExpr>(BinaryOp::GreaterEqual, std::move(left), std::move(right));

        } else if (Match(TokenType::LessEqual)) {
            auto right = ParseAdditive();
            left = std::make_unique<BinaryExpr>(BinaryOp::LessEqual, std::move(left), std::move(right));

        } else {
            break;
        }
    }

    return left;
}


std::unique_ptr<Expr> Parser::ParseAdditive() {
    auto left = ParseMultiplicative();

    while (true) {
        if (Match(TokenType::Plus)) {
            auto right = ParseMultiplicative();
            left = std::make_unique<BinaryExpr>(BinaryOp::Add, std::move(left), std::move(right));
        } else if (Match(TokenType::Minus)) {
            auto right = ParseMultiplicative();
            left = std::make_unique<BinaryExpr>(BinaryOp::Subtract, std::move(left), std::move(right));
        } else {
            break;
        }
    }

    return left;
}

std::unique_ptr<Expr> Parser::ParseMultiplicative() {
    auto left = ParseUnary();

    while (true) {
        if (Match(TokenType::Star)) {
            auto right = ParseUnary();
            left = std::make_unique<BinaryExpr>(BinaryOp::Multiply, std::move(left), std::move(right));
        } else if (Match(TokenType::Slash)) {
            auto right = ParseUnary();
            left = std::make_unique<BinaryExpr>(BinaryOp::Divide, std::move(left), std::move(right));
        } else {
            break;
        }
    }

    return left;
}

std::unique_ptr<Expr> Parser::ParseUnary() {
    if (Match(TokenType::Minus)) {
        auto operand = ParseUnary();
        return std::make_unique<UnaryExpr>(UnaryOp::Negate, std::move(operand));
    }
    return ParsePrimary();
}

void Parser::ParseCallArguments(CallExpr& call) {
    Expect(TokenType::LParen, "Expected '(' after function name");

    if (Check(TokenType::RParen)) {
        Advance();
        return;
    }

    call.args.push_back(ParseExpression());

    while (Match(TokenType::Comma)) {
        call.args.push_back(ParseExpression());
    }

    Expect(TokenType::RParen, "Expected ')' after function arguments");
}

std::unique_ptr<Expr> Parser::ParsePrimary() {
    if (Check(TokenType::Number)) {
        double v = std::strtod(current.text.c_str(), nullptr);
        Advance();
        return std::make_unique<NumberExpr>(v);
    }

    if (Check(TokenType::Identifier)) {
        std::string name = current.text;
        Advance();

        if (Check(TokenType::LParen)) {
            auto call = std::make_unique<CallExpr>(name, std::vector<std::unique_ptr<Expr>>{});
            ParseCallArguments(*call);
            return call;
        }

        return std::make_unique<VariableExpr>(name);
    }

    if (Match(TokenType::LParen)) {
        auto e = ParseExpression();
        Expect(TokenType::RParen, "Expected ')'");
        return e;
    }

    Error("Expected primary expression");
}