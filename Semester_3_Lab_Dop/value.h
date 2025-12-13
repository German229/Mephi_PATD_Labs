#pragma once

#include <stdexcept>
#include <string>
#include <ostream>

// Представление значения в языке ProbabilityScript.
// Сейчас язык оперирует только вещественными числами (double),
// но класс Value сделан так, чтобы в будущем его можно было
// расширить до нескольких типов (через enum + union / std::variant).
class Value {
public:
    enum class Type {
        Number
        // В будущем можно добавить: Bool, String, Array, ...
    };

private:
    Type type;
    double number; // единственный поддерживаемый тип на данный момент

public:
    // Конструктор по умолчанию: 0.0
    Value()
        : type(Type::Number), number(0.0) {}

    // Явный конструктор из double
    explicit Value(double v)
        : type(Type::Number), number(v) {}

    // Статический конструктор для удобства
    static Value FromNumber(double v) {
        return Value(v);
    }

    // Получить тип значения
    Type GetType() const {
        return type;
    }

    // Проверить, что значение — число
    bool IsNumber() const {
        return type == Type::Number;
    }

    // Получить значение как число (с проверкой типа)
    double AsNumber() const {
        if (type != Type::Number) {
            throw std::runtime_error("Value is not a number");
        }
        return number;
    }

    // ================== Арифметические операции ==================

    // Все операции определены только для чисел.
    // Если в будущем появятся другие типы, сюда можно добавить проверки
    // и перегрузки для них.

    Value operator+(const Value& other) const {
        return Value(number + other.AsNumber());
    }

    Value operator-(const Value& other) const {
        return Value(number - other.AsNumber());
    }

    Value operator*(const Value& other) const {
        return Value(number * other.AsNumber());
    }

    Value operator/(const Value& other) const {
        double denom = other.AsNumber();
        // В данном проекте считаем, что деление на ноль — ошибка исполнения.
        if (denom == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return Value(number / denom);
    }

    Value operator-() const {
        return Value(-number);
    }

    // ================== Служебные методы ==================

    // Установить числовое значение
    void SetNumber(double v) {
        type = Type::Number;
        number = v;
    }

    // Строковое представление (для отладки / print)
    std::string ToString() const {
        if (type == Type::Number) {
            // Для простоты используем стандартное преобразование double → string.
            return std::to_string(number);
        }
        return "<unknown>";
    }
};

// Вспомогательный оператор вывода для std::ostream (например, std::cout << value)
inline std::ostream& operator<<(std::ostream& os, const Value& v) {
    os << v.ToString();
    return os;
}