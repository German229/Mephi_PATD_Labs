#include "lexer.h"

#include <cctype>
#include <stdexcept>
#include <string>

/*
 * Локальная утилита: сформировать сообщение об ошибке с позицией.
 */
static std::runtime_error LexError(int line, int col, const std::string& msg) {
    return std::runtime_error(
        "Lex error at line " + std::to_string(line) +
        ", column " + std::to_string(col) +
        ": " + msg
    );
}

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
 *
 * Примечание: преподаватель просил Read() вместо Get().
 */
char Lexer::Read() {
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
            Read();
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
            Read();
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
 *
 * Поддержка:
 *   DIGITS
 *   DIGITS '.' DIGITS
 *
 * Запрещено (ошибка):
 *   12.      (нет дробной части)
 *   12..3    (две точки)
 *   123abc   (число + буквы)
 */
Token Lexer::LexNumber(int startLine, int startCol) {
    std::string buf;
    bool seenDot = false;

    // 1) Целая часть (минимум одна цифра гарантирована вызовом)
    while (std::isdigit(static_cast<unsigned char>(Peek()))) {
        buf.push_back(Read());
    }

    // 2) Дробная часть (если есть точка)
    if (Peek() == '.') {
        seenDot = true;
        buf.push_back(Read()); // '.'

        // Требуем хотя бы одну цифру после точки
        if (!std::isdigit(static_cast<unsigned char>(Peek()))) {
            throw LexError(startLine, startCol, "Invalid number literal: expected digits after '.'");
        }

        while (std::isdigit(static_cast<unsigned char>(Peek()))) {
            buf.push_back(Read());
        }
    }

    // 3) Явные запреты: вторая точка или буквы/подчёркивание сразу после числа
    if (Peek() == '.') {
        throw LexError(startLine, startCol, "Invalid number literal: multiple '.' in number");
    }
    if (IsIdentStart(Peek())) {
        throw LexError(startLine, startCol, "Invalid number literal: letters after number (e.g. 123abc)");
    }

    (void)seenDot;
    return Token(TokenType::Number, buf, startLine, startCol);
}

/*
 * Лексический разбор строкового литерала в двойных кавычках.
 *
 * Поддерживаемые escape-последовательности:
 *   \\  \"  \n  \t  \r
 *
 * Любая неизвестная escape-последовательность — ошибка.
 */
Token Lexer::LexStringLiteral(int startLine, int startCol) {
    Read(); // пропускаем открывающую кавычку
    std::string buf;

    while (true) {
        char c = Peek();

        if (c == '\0') {
            throw LexError(startLine, startCol, "Unterminated string literal");
        }

        if (c == '\n') {
            throw LexError(startLine, startCol, "String literal cannot contain newline");
        }

        if (c == '"') {
            Read(); // закрывающая кавычка
            break;
        }

        if (c == '\\') {
            int escLine = line;
            int escCol = column;

            Read(); // '\'
            char e = Peek();
            if (e == '\0') {
                throw LexError(escLine, escCol, "Unterminated escape sequence in string literal");
            }

            switch (e) {
                case '\\': Read(); buf.push_back('\\'); break;
                case '"':  Read(); buf.push_back('"');  break;
                case 'n':  Read(); buf.push_back('\n'); break;
                case 't':  Read(); buf.push_back('\t'); break;
                case 'r':  Read(); buf.push_back('\r'); break;
                default:
                    throw LexError(escLine, escCol, std::string("Invalid escape sequence: \\") + e);
            }
            continue;
        }

        buf.push_back(Read());
    }

    return Token(TokenType::StringLiteral, buf, startLine, startCol);
}

/*
 * Лексический разбор идентификатора или ключевого слова.
 * После чтения идентификатора производится проверка на ключевые слова языка.
 */
Token Lexer::LexIdentifierOrKeyword(int startLine, int startCol) {
    std::string buf;
    buf.push_back(Read());

    while (IsIdentChar(Peek())) {
        buf.push_back(Read());
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
        // Пропуск пробелов и комментариев
        SkipWhitespace();
        if (Peek() == '/' && PeekNext() == '/') {
            SkipCommentIfAny();
            continue;
        }
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
                Read();
                if (Peek() == '=') {
                    Read();
                    return Token(TokenType::EqualEqual, "==", startLine, startCol);
                }
                return Token(TokenType::Assign, "=", startLine, startCol);

            case '!':
                Read();
                if (Peek() == '=') {
                    Read();
                    return Token(TokenType::BangEqual, "!=", startLine, startCol);
                }
                throw LexError(startLine, startCol, "Unexpected character '!': expected '!='");

            case '>':
                Read();
                if (Peek() == '=') {
                    Read();
                    return Token(TokenType::GreaterEqual, ">=", startLine, startCol);
                }
                return Token(TokenType::Greater, ">", startLine, startCol);

            case '<':
                Read();
                if (Peek() == '=') {
                    Read();
                    return Token(TokenType::LessEqual, "<=", startLine, startCol);
                }
                return Token(TokenType::Less, "<", startLine, startCol);

            case '+': Read(); return Token(TokenType::Plus, "+", startLine, startCol);
            case '-': Read(); return Token(TokenType::Minus, "-", startLine, startCol);
            case '*': Read(); return Token(TokenType::Star, "*", startLine, startCol);

            case '/':
                if (PeekNext() == '/') {
                    SkipCommentIfAny();
                    continue;
                }
                Read();
                return Token(TokenType::Slash, "/", startLine, startCol);

            case '(': Read(); return Token(TokenType::LParen, "(", startLine, startCol);
            case ')': Read(); return Token(TokenType::RParen, ")", startLine, startCol);
            case '{': Read(); return Token(TokenType::LBrace, "{", startLine, startCol);
            case '}': Read(); return Token(TokenType::RBrace, "}", startLine, startCol);
            case ',': Read(); return Token(TokenType::Comma, ",", startLine, startCol);
        }

        throw LexError(startLine, startCol, std::string("Unexpected character '") + c + "'");
    }
}