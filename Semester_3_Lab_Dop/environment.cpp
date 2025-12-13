#include "environment.h"

// ================== Переменные ==================

bool Environment::HasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

Value Environment::GetVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it == variables.end()) {
        throw std::runtime_error("Undefined variable: " + name);
    }
    return it->second;
}

void Environment::SetVariable(const std::string& name, const Value& value) {
    variables[name] = value;
}

// ================== Выборки ==================

bool Environment::HasSample(const std::string& sampleName) const {
    return samples.find(sampleName) != samples.end();
}

const Sequence<Value>* Environment::GetSample(const std::string& sampleName) const {
    auto it = samples.find(sampleName);
    if (it == samples.end()) {
        return nullptr;
    }
    return it->second.get();
}

void Environment::CollectSample(const std::string& sampleName, const Value& value) {
    // Находим или создаём выборку
    auto it = samples.find(sampleName);
    if (it == samples.end()) {
        // Создаём пустую MutableArraySequence<Value>
        auto seqPtr = std::make_unique<MutableArraySequence<Value>>();
        it = samples.emplace(sampleName, std::move(seqPtr)).first;
    }

    // Текущая последовательность
    Sequence<Value>* seq = it->second.get();

    // Append в нашей модели НЕ модифицирует текущее Sequence,
    // а создаёт НОВУЮ последовательность и возвращает её указатель.
    Sequence<Value>* newSeq = seq->Append(value);

    // Заменяем старую последовательность на новую.
    it->second.reset(newSeq);
}