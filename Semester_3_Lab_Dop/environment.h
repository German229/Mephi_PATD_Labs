#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "value.h"
#include "sequence.h"   // MutableArraySequence, Sequence

// Окружение исполнения:
//  - таблица переменных (имя -> Value)
//  - таблица выборок для статистики (имя -> Sequence<Value>)
class Environment {
public:
    Environment() = default;

    // ================== Переменные ==================

    // Есть ли переменная с таким именем
    bool HasVariable(const std::string& name) const;

    // Получить значение переменной.
    // Если переменная не определена — бросает std::runtime_error.
    Value GetVariable(const std::string& name) const;

    // Установить / создать переменную.
    void SetVariable(const std::string& name, const Value& value);

    // ================== Выборки (для collect) ==================

    // Добавить значение в выборку с указанным именем.
    // Если выборки ещё нет — создаётся пустая и затем в неё добавляется элемент.
    void CollectSample(const std::string& sampleName, const Value& value);

    // Есть ли выборка с таким именем
    bool HasSample(const std::string& sampleName) const;

    // Получить выборку (только для чтения).
    // Если выборки нет — возвращает nullptr.
    const Sequence<Value>* GetSample(const std::string& sampleName) const;

private:
    // Таблица переменных: имя -> значение
    std::unordered_map<std::string, Value> variables;

    // Таблица выборок: имя -> последовательность значений.
    // Храним через абстрактный Sequence<Value>, а конкретная реализация — MutableArraySequence<Value>.
    std::unordered_map<std::string, std::unique_ptr<Sequence<Value>>> samples;
};