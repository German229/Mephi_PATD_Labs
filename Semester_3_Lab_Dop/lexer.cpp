#include "lexer.h"

#include <cctype>
#include <stdexcept>

/*
 * Конструктор лексера.
 * Инициализирует исходный текст и начальное положение курсора.
 */
Lexer::Lexer(const std::string& src)
    : source(src), pos(0), line(1), column(1) {}

/*
 * Возвращает текущий символ без продвижения позиции.
 * Если достигнут конец входа, возвращает '\0'.
 */
char Lexer::Peek() const {
    if (pos >= source.size()) return '\0';
    return source[pos];
}

/*
 * Возвращает следующий символ (pos + 1) без продвижения позиции.
 * Используется для распознавания двухсимвольных токенов (//, >=, <=, ==).
 */
char Lexer::PeekNext() const {
    if (pos + 1 >= source.size()) return '\0';
    return source[pos + 1];
}

/*
 * Считывает текущий символ и продвигает позицию вперёд.
 * Корректно обновляет номер строки и столбца.
 */
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

/*
 * Пропускает все пробельные символы:
 * пробелы, табуляции, переводы строк.
 */
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

/*
 * Пропускает однострочный комментарий вида // ...
 * Останавливается на конце строки или конце файла.
 */
void Lexer::SkipCommentIfAny() {
    if (Peek() == '/' && PeekNext() == '/') {
        while (Peek() != '\0' && Peek() != '\n') {
            Get();
        }
    }
}

/*
 * Проверка: может ли символ быть первым в идентификаторе.
 */
bool Lexer::IsIdentStart(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

/*
 * Проверка: может ли символ входить в тело идентификатора.
 */
bool Lexer::IsIdentChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

/*
 * Лексический разбор числового литерала.
 * Поддерживаются целые и вещественные числа с одной точкой.
 */
Token Lexer::LexNumber(int startLine, int startCol) {
    std::string buf;
    bool seenDot = false;

    while (true) {
        char c = Peek();
        if (std::isdigit(static_cast<unsigned char>(c))) {
            buf.push_back(Get());
        } else if (c == '.' && !seenDot) {
            seenDot = true;
            buf.push_back(Get());
        } else {
            break;
        }
    }

    return Token(TokenType::Number, buf, startLine, startCol);
}

/*
 * Лексический разбор строкового литерала в двойных кавычках.
 * Строки не могут содержать перевод строки.
 */
Token Lexer::LexStringLiteral(int startLine, int startCol) {
    Get(); // пропускаем открывающую кавычку
    std::string buf;

    while (true) {
        char c = Peek();

        if (c == '\0') {
            throw std::runtime_error(
                "Unterminated string literal at line " + std::to_string(startLine)
            );
        }

        if (c == '"') {
            Get(); // закрывающая кавычка
            break;
        }

        if (c == '\n') {
            throw std::runtime_error(
                "String literal cannot contain newline at line " + std::to_string(startLine)
            );
        }

        buf.push_back(Get());
    }

    return Token(TokenType::StringLiteral, buf, startLine, startCol);
}

/*
 * Лексический разбор идентификатора или ключевого слова.
 * После чтения идентификатора производится проверка на ключевые слова языка.
 */
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
    if (buf == "if")         return Token(TokenType::If, buf, startLine, startCol);

    return Token(TokenType::Identifier, buf, startLine, startCol);
}

/*
 * Основной метод лексера.
 * Последовательно возвращает токены из входного текста.
 */
Token Lexer::NextToken() {
    while (true) {
        SkipWhitespace();
        SkipCommentIfAny();
        SkipWhitespace();

        char c = Peek();
        if (c == '\0') {
            return Token(TokenType::EndOfFile, "", line, column);
        }

        int startLine = line;
        int startCol = column;

        if (std::isdigit(static_cast<unsigned char>(c))) {
            return LexNumber(startLine, startCol);
        }

        if (IsIdentStart(c)) {
            return LexIdentifierOrKeyword(startLine, startCol);
        }

        if (c == '"') {
            return LexStringLiteral(startLine, startCol);
        }

        switch (c) {
            case '=':
                Get();
                if (Peek() == '=') {
                    Get();
                    return Token(TokenType::EqualEqual, "==", startLine, startCol);
                }
                return Token(TokenType::Assign, "=", startLine, startCol);

            case '!':
                Get();
                if (Peek() == '=') {
                    Get();
                    return Token(TokenType::BangEqual, "!=", startLine, startCol);
                }
                throw std::runtime_error(
                    "Unexpected character '!' at line " + std::to_string(line)
                );

            case '>':
                Get();
                if (Peek() == '=') {
                    Get();
                    return Token(TokenType::GreaterEqual, ">=", startLine, startCol);
                }
                return Token(TokenType::Greater, ">", startLine, startCol);

            case '<':
                Get();
                if (Peek() == '=') {
                    Get();
                    return Token(TokenType::LessEqual, "<=", startLine, startCol);
                }
                return Token(TokenType::Less, "<", startLine, startCol);

            case '+': Get(); return Token(TokenType::Plus, "+", startLine, startCol);
            case '-': Get(); return Token(TokenType::Minus, "-", startLine, startCol);
            case '*': Get(); return Token(TokenType::Star, "*", startLine, startCol);

            case '/':
                if (PeekNext() == '/') {
                    SkipCommentIfAny();
                    continue;
                }
                Get();
                return Token(TokenType::Slash, "/", startLine, startCol);

            case '(': Get(); return Token(TokenType::LParen, "(", startLine, startCol);
            case ')': Get(); return Token(TokenType::RParen, ")", startLine, startCol);
            case '{': Get(); return Token(TokenType::LBrace, "{", startLine, startCol);
            case '}': Get(); return Token(TokenType::RBrace, "}", startLine, startCol);
            case ',': Get(); return Token(TokenType::Comma, ",", startLine, startCol);
        }

        throw std::runtime_error(
            "Unexpected character '" + std::string(1, c) +
            "' at line " + std::to_string(line) +
            ", column " + std::to_string(column)
        );
    }
}