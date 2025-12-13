#pragma once

#include <string>

enum class TokenType {
    EndOfFile,

    Identifier,
    Number,
    StringLiteral,

    // Keywords
    KeywordRepeat,
    KeywordCollect,
    KeywordPrint,
    KeywordPrintStat,

    // Symbols
    Assign,      // =
    Plus,        // +
    Minus,       // -
    Star,        // *
    Slash,       // /
    LParen,      // (
    RParen,      // )
    LBrace,      // {
    RBrace,      // }
    Comma        // ,   <-- NEW
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;

    Token(TokenType t, std::string s, int l, int c)
        : type(t), text(std::move(s)), line(l), column(c) {}
};