#include "parser.h"

#include <iostream>
#include <cstdlib>

/*
 * Конструктор парсера.
 * Инициализирует начальное состояние и считывает первый токен.
 */
Parser::Parser(Lexer& lex)
    : lexer(lex),
      current(TokenType::EndOfFile, "", 1, 1)
{
    Advance();
}

/*
 * Считать следующий токен из лексера.
 */
void Parser::Advance() {
    current = lexer.NextToken();
}

/*
 * Проверить, совпадает ли текущий токен с заданным типом.
 * В случае совпадения — продвинуться к следующему токену.
 */
bool Parser::Match(TokenType t) {
    if (current.type == t) {
        Advance();
        return true;
    }
    return false;
}

/*
 * Проверить тип текущего токена без продвижения.
 */
bool Parser::Check(TokenType t) const {
    return current.type == t;
}

/*
 * Ожидать токен определённого типа.
 * В случае ошибки генерируется исключение.
 */
void Parser::Expect(TokenType t, const std::string& msg) {
    if (current.type != t) {
        Error(msg);
    }
    Advance();
}

/*
 * Сообщить об ошибке синтаксического анализа с указанием позиции.
 */
[[noreturn]] void Parser::Error(const std::string& msg) {
    throw std::runtime_error(
        "Parse error at line " + std::to_string(current.line) +
        ", column " + std::to_string(current.column) +
        ": " + msg + " (got '" + current.text + "')"
    );
}

/*
 * Разобрать программу целиком.
 * Программа представляет собой последовательность операторов.
 */
Program Parser::ParseProgram() {
    Program prog;
    while (!Check(TokenType::EndOfFile)) {
        prog.AddStatement(ParseStatement());
    }
    return prog;
}

/*
 * Разобрать один оператор языка.
 */
std::unique_ptr<Stmt> Parser::ParseStatement() {
    if (Match(TokenType::KeywordRepeat)) {
        auto countExpr = ParseExpression();
        auto body = ParseBlock();
        return std::make_unique<RepeatStmt>(std::move(countExpr), std::move(body));
    }

    if (Match(TokenType::KeywordCollect)) {
        if (!Check(TokenType::Identifier)) {
            Error("Expected sample identifier after collect, e.g. collect A x");
        }

        std::string sampleName = current.text;
        Advance();

        auto expr = ParseExpression();
        return std::make_unique<CollectStmt>(std::move(sampleName), std::move(expr));
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

/*
 * Разобрать блок операторов в фигурных скобках.
 */
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

// ================== Выражения с приоритетами ==================

/*
 * Точка входа для разбора выражений.
 */
std::unique_ptr<Expr> Parser::ParseExpression() {
    return ParseEquality();
}

/*
 * Операторы равенства: ==, !=
 */
std::unique_ptr<Expr> Parser::ParseEquality() {
    auto left = ParseComparison();

    while (true) {
        if (Match(TokenType::EqualEqual)) {
            auto right = ParseComparison();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::Equal, std::move(left), std::move(right)
            );
        } else if (Match(TokenType::BangEqual)) {
            auto right = ParseComparison();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::NotEqual, std::move(left), std::move(right)
            );
        } else {
            break;
        }
    }

    return left;
}

/*
 * Операторы сравнения: <, >, <=, >=
 */
std::unique_ptr<Expr> Parser::ParseComparison() {
    auto left = ParseAdditive();

    while (true) {
        if (Match(TokenType::Greater)) {
            auto right = ParseAdditive();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::Greater, std::move(left), std::move(right)
            );
        } else if (Match(TokenType::Less)) {
            auto right = ParseAdditive();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::Less, std::move(left), std::move(right)
            );
        } else if (Match(TokenType::GreaterEqual)) {
            auto right = ParseAdditive();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::GreaterEqual, std::move(left), std::move(right)
            );
        } else if (Match(TokenType::LessEqual)) {
            auto right = ParseAdditive();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::LessEqual, std::move(left), std::move(right)
            );
        } else {
            break;
        }
    }

    return left;
}

/*
 * Аддитивные операторы: +, -
 */
std::unique_ptr<Expr> Parser::ParseAdditive() {
    auto left = ParseMultiplicative();

    while (true) {
        if (Match(TokenType::Plus)) {
            auto right = ParseMultiplicative();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::Add, std::move(left), std::move(right)
            );
        } else if (Match(TokenType::Minus)) {
            auto right = ParseMultiplicative();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::Subtract, std::move(left), std::move(right)
            );
        } else {
            break;
        }
    }

    return left;
}

/*
 * Мультипликативные операторы: *, /
 */
std::unique_ptr<Expr> Parser::ParseMultiplicative() {
    auto left = ParseUnary();

    while (true) {
        if (Match(TokenType::Star)) {
            auto right = ParseUnary();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::Multiply, std::move(left), std::move(right)
            );
        } else if (Match(TokenType::Slash)) {
            auto right = ParseUnary();
            left = std::make_unique<BinaryExpr>(
                BinaryOp::Divide, std::move(left), std::move(right)
            );
        } else {
            break;
        }
    }

    return left;
}

/*
 * Унарные выражения (в текущей версии — унарный минус).
 */
std::unique_ptr<Expr> Parser::ParseUnary() {
    if (Match(TokenType::Minus)) {
        auto operand = ParseUnary();
        return std::make_unique<UnaryExpr>(
            UnaryOp::Negate, std::move(operand)
        );
    }
    return ParsePrimary();
}

/*
 * Разбор аргументов вызова функции.
 */
void Parser::ParseCallArguments(CallExpr& call) {
    Expect(TokenType::LParen, "Expected '(' after function name");

    if (Check(TokenType::RParen)) {
        Advance();
        return;
    }

    // 1) Первый аргумент всегда как обычное выражение
    call.args.push_back(ParseExpression());

    // 2) Остальные аргументы
    while (Match(TokenType::Comma)) {
        // Спец-правило для get_stat: второй аргумент — это ИМЯ ВЫБОРКИ, а не числовая переменная
        if (call.callee == "get_stat" && call.args.size() == 1) {
            if (!Check(TokenType::Identifier)) {
                Error("Expected sample identifier as 2nd argument of get_stat, e.g. get_stat(\"mean\", A)");
            }
            std::string sampleName = current.text;
            Advance();
            call.args.push_back(std::make_unique<SampleRefExpr>(std::move(sampleName)));
            continue;
        }

        // По умолчанию — обычное выражение
        call.args.push_back(ParseExpression());
    }

    Expect(TokenType::RParen, "Expected ')' after function arguments");
}

/*
 * Первичные выражения: числа, переменные, вызовы функций, скобки.
 */
std::unique_ptr<Expr> Parser::ParsePrimary() {
    if (Check(TokenType::Number)) {
        double v = std::strtod(current.text.c_str(), nullptr);
        Advance();
        return std::make_unique<NumberExpr>(v);
    }

    if (Check(TokenType::StringLiteral)) {
        std::string s = current.text;
        Advance();
        return std::make_unique<StringExpr>(std::move(s));
    }

    if (Check(TokenType::Identifier)) {
        std::string name = current.text;
        Advance();

        if (Check(TokenType::LParen)) {
            auto call = std::make_unique<CallExpr>(
                name, std::vector<std::unique_ptr<Expr>>{}
            );
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