#include "parser.h"

#include <cstdlib>
#include <vector>

Parser::Parser(Lexer& lex)
    : lexer(lex),
      current(TokenType::EndOfFile, "", 1, 1)
{
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
    while (current.type != TokenType::EndOfFile) {
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
        if (current.type != TokenType::StringLiteral) {
            Error("Expected string literal inside print_stat(\"...\")");
        }
        std::string statName = current.text;
        Advance();
        Expect(TokenType::RParen, "Expected ')' after print_stat(\"...\")");
        return std::make_unique<PrintStatStmt>(statName);
    }

    if (current.type == TokenType::Identifier) {
        std::string name = current.text;
        Advance();
        Expect(TokenType::Assign, "Expected '=' after identifier in assignment");
        auto expr = ParseExpression();
        return std::make_unique<AssignStmt>(name, std::move(expr));
    }

    if (current.type == TokenType::LBrace) {
        auto b = ParseBlock();
        return b;
    }

    Error("Expected statement");
}

std::unique_ptr<BlockStmt> Parser::ParseBlock() {
    Expect(TokenType::LBrace, "Expected '{' to start block");
    auto block = std::make_unique<BlockStmt>();

    while (current.type != TokenType::RBrace) {
        if (current.type == TokenType::EndOfFile) {
            Error("Unterminated block: expected '}'");
        }
        block->AddStatement(ParseStatement());
    }

    Expect(TokenType::RBrace, "Expected '}' to close block");
    return block;
}

std::unique_ptr<Expr> Parser::ParseExpression() {
    auto left = ParseTerm();

    while (current.type == TokenType::Plus || current.type == TokenType::Minus) {
        TokenType opTok = current.type;
        Advance();

        auto right = ParseTerm();
        if (opTok == TokenType::Plus) {
            left = std::make_unique<BinaryExpr>(BinaryOp::Add, std::move(left), std::move(right));
        } else {
            left = std::make_unique<BinaryExpr>(BinaryOp::Subtract, std::move(left), std::move(right));
        }
    }

    return left;
}

std::unique_ptr<Expr> Parser::ParseTerm() {
    auto left = ParseUnary();

    while (current.type == TokenType::Star || current.type == TokenType::Slash) {
        TokenType opTok = current.type;
        Advance();

        auto right = ParseUnary();
        if (opTok == TokenType::Star) {
            left = std::make_unique<BinaryExpr>(BinaryOp::Multiply, std::move(left), std::move(right));
        } else {
            left = std::make_unique<BinaryExpr>(BinaryOp::Divide, std::move(left), std::move(right));
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

// разбор аргументов: ( [expr (',' expr)*] )
void Parser::ParseCallArguments(CallExpr& call) {
    Expect(TokenType::LParen, "Expected '(' after function name");

    if (current.type == TokenType::RParen) {
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
    if (current.type == TokenType::Number) {
        double v = std::strtod(current.text.c_str(), nullptr);
        Advance();
        return std::make_unique<NumberExpr>(v);
    }

    if (current.type == TokenType::Identifier) {
        std::string name = current.text;
        Advance();

        if (current.type == TokenType::LParen) {
            // ВАЖНО: у тебя CallExpr требует (name, arguments)
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