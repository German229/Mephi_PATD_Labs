#include "lexer.h"

#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& src)
    : source(src), pos(0), line(1), column(1) {}

char Lexer::Peek() const {
    if (pos >= source.size()) return '\0';
    return source[pos];
}

char Lexer::Get() {
    if (pos >= source.size()) return '\0';
    char c = source[pos++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

void Lexer::SkipWhitespace() {
    while (true) {
        char c = Peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            Get();
            continue;
        }
        break;
    }
}

void Lexer::SkipCommentIfAny() {
    // // ... до конца строки
    if (Peek() == '/' && pos + 1 < source.size() && source[pos + 1] == '/') {
        while (Peek() != '\0' && Peek() != '\n') {
            Get();
        }
    }
}

bool Lexer::IsIdentStart(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool Lexer::IsIdentChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

Token Lexer::LexNumber(int startLine, int startCol) {
    std::string buf;
    bool seenDot = false;

    while (true) {
        char c = Peek();
        if (std::isdigit(static_cast<unsigned char>(c))) {
            buf.push_back(Get());
            continue;
        }
        if (c == '.' && !seenDot) {
            seenDot = true;
            buf.push_back(Get());
            continue;
        }
        break;
    }

    return Token(TokenType::Number, buf, startLine, startCol);
}

Token Lexer::LexIdentifierOrKeyword(int startLine, int startCol) {
    std::string buf;
    buf.push_back(Get());

    while (IsIdentChar(Peek())) {
        buf.push_back(Get());
    }

    if (buf == "repeat")     return Token(TokenType::KeywordRepeat, buf, startLine, startCol);
    if (buf == "collect")    return Token(TokenType::KeywordCollect, buf, startLine, startCol);
    if (buf == "print")      return Token(TokenType::KeywordPrint, buf, startLine, startCol);
    if (buf == "print_stat") return Token(TokenType::KeywordPrintStat, buf, startLine, startCol);

    return Token(TokenType::Identifier, buf, startLine, startCol);
}

Token Lexer::LexStringLiteral(int startLine, int startCol) {
    // Ожидаем, что текущий символ — "
    Get(); // пропускаем "

    std::string buf;
    while (true) {
        char c = Peek();
        if (c == '\0') {
            throw std::runtime_error("Unterminated string literal at line " + std::to_string(startLine));
        }
        if (c == '"') {
            Get(); // закрывающая "
            break;
        }
        // минимальная поддержка escape не нужна, но запретим перевод строки в строковом литерале
        if (c == '\n') {
            throw std::runtime_error("String literal cannot contain newline at line " + std::to_string(startLine));
        }
        buf.push_back(Get());
    }

    // text без кавычек
    return Token(TokenType::StringLiteral, buf, startLine, startCol);
}

Token Lexer::NextToken() {
    while (true) {
        SkipWhitespace();
        SkipCommentIfAny();
        // после комментария может быть перевод строки/пробелы
        SkipWhitespace();

        char c = Peek();
        if (c == '\0') {
            return Token(TokenType::EndOfFile, "", line, column);
        }

        int startLine = line;
        int startCol = column;

        // Number
        if (std::isdigit(static_cast<unsigned char>(c))) {
            return LexNumber(startLine, startCol);
        }

        // Identifier / keyword
        if (IsIdentStart(c)) {
            return LexIdentifierOrKeyword(startLine, startCol);
        }

        // String literal
        if (c == '"') {
            return LexStringLiteral(startLine, startCol);
        }

        // One-char tokens
        switch (c) {
            case '=': Get(); return Token(TokenType::Assign, "=", startLine, startCol);
            case '+': Get(); return Token(TokenType::Plus, "+", startLine, startCol);
            case '-': Get(); return Token(TokenType::Minus, "-", startLine, startCol);
            case '*': Get(); return Token(TokenType::Star, "*", startLine, startCol);
            case '/': Get(); return Token(TokenType::Slash, "/", startLine, startCol);
            case '(': Get(); return Token(TokenType::LParen, "(", startLine, startCol);
            case ')': Get(); return Token(TokenType::RParen, ")", startLine, startCol);
            case '{': Get(); return Token(TokenType::LBrace, "{", startLine, startCol);
            case '}': Get(); return Token(TokenType::RBrace, "}", startLine, startCol);
            case ',': Get(); return Token(TokenType::Comma, ",", startLine, startCol); // <-- NEW
            default:
                break;
        }

        throw std::runtime_error(
            "Unexpected character '" + std::string(1, c) +
            "' at line " + std::to_string(line) +
            ", column " + std::to_string(column)
        );
    }
}
