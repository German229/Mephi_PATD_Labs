#pragma once

#include <string>
#include "token.h"

// Лексер ProbabilityScript.
// Вход: строка исходника.
// Выход: последовательность токенов через NextToken().
class Lexer {
public:
    explicit Lexer(const std::string& src);

    Token NextToken();

private:
    std::string source;
    std::size_t pos;

    int line;
    int column;

    char Peek() const;
    char Get();

    void SkipWhitespace();
    void SkipCommentIfAny();

    Token LexNumber(int startLine, int startCol);
    Token LexIdentifierOrKeyword(int startLine, int startCol);
    Token LexStringLiteral(int startLine, int startCol);

    static bool IsIdentStart(char c);
    static bool IsIdentChar(char c);
};