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
    If,

    // Symbols
    Assign,       // =
    Plus,         // +
    Minus,        // -
    Star,         // *
    Slash,        // /
    LParen,       // (
    RParen,       // )
    LBrace,       // {
    RBrace,       // }
    Comma,        // ,
    Greater,      // >
    Less,         // <
    EqualEqual,   // ==
    BangEqual,     // !=
    GreaterEqual, // >=   <-- добавить
    LessEqual    // <=   <-- добавить
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;

    Token(TokenType t, std::string s, int l, int c)
        : type(t), text(std::move(s)), line(l), column(c) {}
};

inline const char* TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile:        return "EndOfFile";
        case TokenType::Identifier:       return "Identifier";
        case TokenType::Number:           return "Number";
        case TokenType::StringLiteral:    return "StringLiteral";

        case TokenType::KeywordRepeat:    return "KeywordRepeat";
        case TokenType::KeywordCollect:   return "KeywordCollect";
        case TokenType::KeywordPrint:     return "KeywordPrint";
        case TokenType::KeywordPrintStat: return "KeywordPrintStat";
        case TokenType::If:               return "If";

        case TokenType::Assign:           return "Assign";
        case TokenType::Plus:             return "Plus";
        case TokenType::Minus:            return "Minus";
        case TokenType::Star:             return "Star";
        case TokenType::Slash:            return "Slash";
        case TokenType::LParen:           return "LParen";
        case TokenType::RParen:           return "RParen";
        case TokenType::LBrace:           return "LBrace";
        case TokenType::RBrace:           return "RBrace";
        case TokenType::Comma:            return "Comma";
        case TokenType::Greater:          return "Greater";
        case TokenType::Less:             return "Less";
        case TokenType::EqualEqual:       return "EqualEqual";
        case TokenType::BangEqual:        return "BangEqual";

        default:                          return "Unknown";
    }
}