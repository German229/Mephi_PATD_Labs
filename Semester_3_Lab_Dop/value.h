#pragma once

#include <stdexcept>
#include <string>
#include <ostream>

/*
 * Класс Value представляет значение времени выполнения
 * в языке ProbabilityScript.
 *
 * В текущей версии язык поддерживает только вещественные числа (double),
 * однако архитектура класса позволяет в будущем расширить его
 * до нескольких типов (например, bool, string и т.д.).
 */
class Value {
public:
    /*
     * Перечисление поддерживаемых типов значений.
     */
    enum class Type {
        Number
        // Возможные расширения: Bool, String, Array, ...
    };

private:
    Type type;     // Текущий тип значения
    double number; // Хранимое числовое значение

public:
    // ================== Конструкторы ==================

    /*
     * Конструктор по умолчанию.
     * Создаёт числовое значение, равное 0.0.
     */
    Value()
        : type(Type::Number), number(0.0) {}

    /*
     * Конструктор из вещественного числа.
     */
    explicit Value(double v)
        : type(Type::Number), number(v) {}

    /*
     * Статический фабричный метод для создания числового значения.
     */
    static Value FromNumber(double v) {
        return Value(v);
    }

    // ================== Доступ к типу ==================

    /*
     * Получить тип значения.
     */
    Type GetType() const {
        return type;
    }

    /*
     * Проверить, является ли значение числом.
     */
    bool IsNumber() const {
        return type == Type::Number;
    }

    /*
     * Получить значение как число.
     * В случае несовпадения типа генерирует исключение.
     */
    double AsNumber() const {
        if (type != Type::Number) {
            throw std::runtime_error("Value is not a number");
        }
        return number;
    }

    // ================== Арифметические операции ==================

    /*
     * Арифметические операции определены только для чисел.
     * При добавлении новых типов сюда можно встроить проверки
     * и соответствующую логику.
     */

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
        if (denom == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return Value(number / denom);
    }

    /*
     * Унарный минус.
     */
    Value operator-() const {
        return Value(-number);
    }

    // ================== Служебные методы ==================

    /*
     * Принудительно установить числовое значение.
     */
    void SetNumber(double v) {
        type = Type::Number;
        number = v;
    }

    /*
     * Получить строковое представление значения.
     * Используется для вывода и отладки.
     */
    std::string ToString() const {
        if (type == Type::Number) {
            return std::to_string(number);
        }
        return "<unknown>";
    }
};

/*
 * Перегруженный оператор вывода значения в поток.
 */
inline std::ostream& operator<<(std::ostream& os, const Value& v) {
    os << v.ToString();
    return os;
}