#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "value.h"
#include "sequence.h"   // Sequence<Value>, MutableArraySequence<Value>

/*
 * Класс Environment представляет окружение исполнения программы
 * на языке ProbabilityScript.
 *
 * Окружение хранит:
 *  - значения переменных,
 *  - выборки значений, используемые для статистических вычислений.
 */
class Environment {
public:
    /*
     * Конструктор окружения.
     * Создаёт пустое окружение без переменных и выборок.
     */
    Environment() = default;

    // =========================================================
    //                     Переменные
    // =========================================================

    /*
     * Проверить, существует ли переменная с заданным именем.
     */
    bool HasVariable(const std::string& name) const;

    /*
     * Получить значение переменной.
     *
     * @throws std::runtime_error если переменная не определена.
     */
    Value GetVariable(const std::string& name) const;

    /*
     * Создать или обновить значение переменной.
     */
    void SetVariable(const std::string& name, const Value& value);

    // =========================================================
    //                   Выборки (collect)
    // =========================================================

    /*
     * Добавить значение в выборку с указанным именем.
     *
     * Если выборка ещё не существует, она создаётся автоматически.
     */
    void CollectSample(const std::string& sampleName, const Value& value);

    /*
     * Проверить, существует ли выборка с заданным именем.
     */
    bool HasSample(const std::string& sampleName) const;

    /*
     * Получить выборку значений (только для чтения).
     *
     * @return указатель на выборку или nullptr, если выборка не существует.
     */
    const Sequence<Value>* GetSample(const std::string& sampleName) const;

private:
    // =========================================================
    //                 Внутренние структуры
    // =========================================================

    /*
     * Таблица переменных:
     * имя переменной → значение.
     */
    std::unordered_map<std::string, Value> variables;

    /*
     * Таблица выборок:
     * имя выборки → последовательность значений.
     *
     * Выборки хранятся через абстрактный интерфейс Sequence<Value>,
     * что позволяет менять конкретную реализацию без изменения интерфейса.
     */
    std::unordered_map<std::string, std::unique_ptr<Sequence<Value>>> samples;
};