#pragma once

#include <string>

/*
 * Перечисление TokenType описывает все возможные типы токенов,
 * которые может порождать лексический анализатор языка ProbabilityScript.
 *
 * Каждый токен представляет собой атомарный элемент исходного текста,
 * используемый синтаксическим анализатором при построении AST.
 */
enum class TokenType {
    // Специальные токены
    EndOfFile,        // Конец входного файла

    // Базовые типы токенов
    Identifier,       // Идентификатор (имя переменной или функции)
    Number,           // Числовой литерал
    StringLiteral,    // Строковый литерал

    // Ключевые слова языка
    KeywordRepeat,    // repeat
    KeywordCollect,   // collect
    KeywordPrint,     // print
    KeywordPrintStat, // print_stat
    If,               // if

    // Операторы и разделители
    Assign,           // =
    Plus,             // +
    Minus,            // -
    Star,             // *
    Slash,            // /
    LParen,           // (
    RParen,           // )
    LBrace,           // {
    RBrace,           // }
    Comma,            // ,

    // Операторы сравнения
    Greater,          // >
    Less,             // <
    EqualEqual,       // ==
    BangEqual,        // !=
    GreaterEqual,     // >=
    LessEqual         // <=
};

/*
 * Структура Token представляет один лексический токен.
 *
 * Используется парсером для анализа синтаксической структуры программы.
 */
struct Token {
    TokenType type;   // Тип токена
    std::string text; // Текстовое представление токена
    int line;         // Номер строки, где начинается токен
    int column;       // Номер столбца, где начинается токен

    /*
     * Конструктор токена.
     *
     * @param t  тип токена
     * @param s  текст токена
     * @param l  номер строки
     * @param c  номер столбца
     */
    Token(TokenType t, std::string s, int l, int c)
        : type(t), text(std::move(s)), line(l), column(c) {}
};

/*
 * Вспомогательная функция для преобразования TokenType в строку.
 *
 * Используется для отладки и диагностических сообщений.
 */
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
        case TokenType::GreaterEqual:     return "GreaterEqual";
        case TokenType::LessEqual:        return "LessEqual";

        default:                          return "Unknown";
    }
}
