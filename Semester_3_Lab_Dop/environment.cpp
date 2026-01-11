#include "environment.h"

// ============================================================
//                        Переменные
// ============================================================

/*
 * Проверить, существует ли переменная с заданным именем.
 */
bool Environment::HasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

/*
 * Получить значение переменной.
 *
 * @throws std::runtime_error если переменная не определена.
 */
Value Environment::GetVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it == variables.end()) {
        throw std::runtime_error("Undefined variable: " + name);
    }
    return it->second;
}

/*
 * Создать новую переменную или обновить существующую.
 */
void Environment::SetVariable(const std::string& name, const Value& value) {
    variables[name] = value;
}

// ============================================================
//                         Выборки
// ============================================================

/*
 * Проверить, существует ли выборка с заданным именем.
 */
bool Environment::HasSample(const std::string& sampleName) const {
    return samples.find(sampleName) != samples.end();
}

/*
 * Получить выборку значений (только для чтения).
 *
 * @return указатель на выборку или nullptr, если выборка не существует.
 */
const Sequence<Value>* Environment::GetSample(const std::string& sampleName) const {
    auto it = samples.find(sampleName);
    if (it == samples.end()) {
        return nullptr;
    }
    return it->second.get();
}

/*
 * Добавить значение в выборку.
 *
 * Если выборка не существует, она создаётся автоматически.
 *
 * Важно: метод Append у Sequence<Value> не изменяет текущий объект,
 * а возвращает новый экземпляр последовательности.
 * Поэтому старая последовательность заменяется новой.
 */
void Environment::CollectSample(const std::string& sampleName, const Value& value) {
    // Найти или создать выборку
    auto it = samples.find(sampleName);
    if (it == samples.end()) {
        auto seqPtr = std::make_unique<MutableArraySequence<Value>>();
        it = samples.emplace(sampleName, std::move(seqPtr)).first;
    }

    // Текущая последовательность
    Sequence<Value>* seq = it->second.get();

    // Добавление элемента создаёт новую последовательность
    Sequence<Value>* newSeq = seq->Append(value);

    // Заменяем старую последовательность на новую
    it->second.reset(newSeq);
}