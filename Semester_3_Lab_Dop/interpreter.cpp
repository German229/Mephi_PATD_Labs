// interpreter.cpp
#include "interpreter.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

/*
 * Конструктор интерпретатора.
 *
 * Инициализирует:
 *  - окружение исполнения,
 *  - генератор случайных чисел с заданным seed,
 *  - поток вывода (std::cout по умолчанию).
 */
Interpreter::Interpreter(unsigned int seed)
    : env(),
      rng(seed),
      out(&std::cout)
{
}

/*
 * Установить альтернативный поток вывода.
 */
void Interpreter::SetOutputStream(std::ostream& os) {
    out = &os;
}

/*
 * Выполнить программу целиком.
 *
 * Программа представляет собой список операторов верхнего уровня.
 */
void Interpreter::ExecuteProgram(const Program& program) {
    for (const auto& stmtPtr : program.statements) {
        ExecuteStatement(stmtPtr.get());
    }
}

/*
 * Выполнение одного оператора AST.
 *
 * Тип оператора определяется с помощью dynamic_cast.
 */
void Interpreter::ExecuteStatement(Stmt* stmt) {

    // ---------- Присваивание ----------
    if (auto* assign = dynamic_cast<AssignStmt*>(stmt)) {
        Value val = EvaluateExpression(assign->expression.get());
        env.SetVariable(assign->name, val);
        return;
    }

    // ---------- print ----------
    if (auto* printStmt = dynamic_cast<PrintStmt*>(stmt)) {
        Value val = EvaluateExpression(printStmt->expression.get());
        PrintValue(val);
        return;
    }

    // ---------- collect ----------
    if (auto* collectStmt = dynamic_cast<CollectStmt*>(stmt)) {
        if (collectStmt->sampleName.empty()) {
            throw std::runtime_error("collect requires an explicit sample name");
        }

        Value val = EvaluateExpression(collectStmt->expression.get());
        env.CollectSample(collectStmt->sampleName, val);
        return;
    }

    // ---------- if ----------
    if (auto* ifStmt = dynamic_cast<const IfStmt*>(stmt)) {
        Value condVal = EvaluateExpression(ifStmt->condition.get());
        if (condVal.AsNumber() != 0.0) {
            for (const auto& subStmt : ifStmt->body->statements) {
                ExecuteStatement(subStmt.get());
            }
        }
        return;
    }

    // ---------- repeat ----------
    if (auto* repeatStmt = dynamic_cast<RepeatStmt*>(stmt)) {
        Value countVal = EvaluateExpression(repeatStmt->countExpr.get());
        double d = countVal.AsNumber();

        if (d < 0) {
            throw std::runtime_error("repeat count cannot be negative");
        }

        std::size_t n = static_cast<std::size_t>(d);
        for (std::size_t i = 0; i < n; ++i) {
            ExecuteBlock(repeatStmt->body.get());
        }
        return;
    }

    // ---------- print_stat ----------
    if (auto* ps = dynamic_cast<PrintStatStmt*>(stmt)) {
        ExecutePrintStat(ps->sampleName);
        return;
    }

    // ---------- блок ----------
    if (auto* block = dynamic_cast<BlockStmt*>(stmt)) {
        ExecuteBlock(block);
        return;
    }

    throw std::runtime_error("Unknown statement type in interpreter");
}

/*
 * Выполнение блока операторов.
 */
void Interpreter::ExecuteBlock(BlockStmt* block) {
    for (const auto& stmtPtr : block->statements) {
        ExecuteStatement(stmtPtr.get());
    }
}

/*
 * Вычисление выражения.
 *
 * Рекурсивно обходит AST выражения и возвращает Value.
 */
Value Interpreter::EvaluateExpression(Expr* expr) {

    // ---------- число ----------
    if (auto* num = dynamic_cast<NumberExpr*>(expr)) {
        return Value::FromNumber(num->value);
    }

    // ---------- переменная ----------
    if (auto* var = dynamic_cast<VariableExpr*>(expr)) {
        return env.GetVariable(var->name);
    }

    // ---------- унарное выражение ----------
    if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
        Value val = EvaluateExpression(unary->operand.get());
        switch (unary->op) {
            case UnaryOp::Negate:
                return -val;
        }
        throw std::runtime_error("Unknown unary operator");
    }

    // ---------- бинарное выражение ----------
    if (auto* bin = dynamic_cast<BinaryExpr*>(expr)) {
        Value left = EvaluateExpression(bin->left.get());
        Value right = EvaluateExpression(bin->right.get());

        switch (bin->op) {
            case BinaryOp::Add:           return left + right;
            case BinaryOp::Subtract:      return left - right;
            case BinaryOp::Multiply:      return left * right;
            case BinaryOp::Divide:        return left / right;

            case BinaryOp::Greater:
                return Value::FromNumber(left.AsNumber() > right.AsNumber());
            case BinaryOp::Less:
                return Value::FromNumber(left.AsNumber() < right.AsNumber());
            case BinaryOp::Equal:
                return Value::FromNumber(left.AsNumber() == right.AsNumber());
            case BinaryOp::NotEqual:
                return Value::FromNumber(left.AsNumber() != right.AsNumber());
            case BinaryOp::GreaterEqual:
                return Value::FromNumber(left.AsNumber() >= right.AsNumber());
            case BinaryOp::LessEqual:
                return Value::FromNumber(left.AsNumber() <= right.AsNumber());
        }

        throw std::runtime_error("Unknown binary operator");
    }

    // ---------- вызов функции ----------
    if (auto* call = dynamic_cast<CallExpr*>(expr)) {
        const std::string& name = call->callee;
        const auto& args = call->args;

        // ===== Явная статистика (требование ЛР) =====
        //
        // 2 аргумента:
        //   get_stat("mean", A)
        //
        // 3 аргумента:
        //   get_stat("moment", A, 2)
        //   get_stat("central_moment", A, 2)
        //
        if (name == "get_stat") {
            if (args.size() != 2 && args.size() != 3) {
                throw std::runtime_error(
                    "get_stat() expects 2 or 3 arguments: "
                    "get_stat(\"mean\", A) or get_stat(\"moment\", A, 2)"
                );
            }

            auto* stat = dynamic_cast<StringExpr*>(args[0].get());
            if (!stat) {
                throw std::runtime_error(
                    "get_stat(): first argument must be a string literal, e.g. \"mean\""
                );
            }

            auto* sample = dynamic_cast<SampleRefExpr*>(args[1].get());
            if (!sample) {
                throw std::runtime_error(
                    "get_stat(): second argument must be a sample identifier, e.g. get_stat(\"mean\", A)"
                );
            }

            if (args.size() == 2) {
                return Value::FromNumber(GetSampleStat(stat->value, sample->name));
            }

            double kd = EvaluateExpression(args[2].get()).AsNumber();
            if (!std::isfinite(kd) || kd < 0.0 || std::floor(kd) != kd) {
                throw std::runtime_error(
                    "get_stat(): k must be a non-negative integer, e.g. 2"
                );
            }

            std::size_t k = static_cast<std::size_t>(kd);
            return Value::FromNumber(GetSampleStat(stat->value, sample->name, k));
        }

        // ===== Математические функции (для композиции) =====

        if (name == "sqrt") {
            if (args.size() != 1) {
                throw std::runtime_error("sqrt(x) expects exactly 1 argument");
            }
            double x = EvaluateExpression(args[0].get()).AsNumber();
            if (x < 0.0 || !std::isfinite(x)) {
                throw std::runtime_error("sqrt(x): x must be finite and >= 0");
            }
            return Value::FromNumber(std::sqrt(x));
        }

        if (name == "pow") {
            if (args.size() != 2) {
                throw std::runtime_error("pow(x, y) expects exactly 2 arguments");
            }
            double x = EvaluateExpression(args[0].get()).AsNumber();
            double y = EvaluateExpression(args[1].get()).AsNumber();
            if (!std::isfinite(x) || !std::isfinite(y)) {
                throw std::runtime_error("pow(x, y): x and y must be finite");
            }
            double r = std::pow(x, y);
            if (!std::isfinite(r)) {
                throw std::runtime_error("pow(x, y): result is not finite");
            }
            return Value::FromNumber(r);
        }

        // ===== Распределения =====

        if (name == "uniform") {
            double a = 0.0;
            double b = 1.0;

            if (args.size() == 1) {
                b = EvaluateExpression(args[0].get()).AsNumber();
            } else if (args.size() == 2) {
                a = EvaluateExpression(args[0].get()).AsNumber();
                b = EvaluateExpression(args[1].get()).AsNumber();
            } else if (!args.empty()) {
                throw std::runtime_error("uniform() supports 0–2 arguments");
            }

            if (!(a < b)) {
                throw std::runtime_error("uniform(a,b): expected a < b");
            }

            std::uniform_real_distribution<double> dist(a, b);
            return Value::FromNumber(dist(rng));
        }

        if (name == "normal") {
            double mu = 0.0;
            double sigma = 1.0;

            if (args.size() == 1) {
                mu = EvaluateExpression(args[0].get()).AsNumber();
            } else if (args.size() == 2) {
                mu = EvaluateExpression(args[0].get()).AsNumber();
                sigma = EvaluateExpression(args[1].get()).AsNumber();
            } else if (!args.empty()) {
                throw std::runtime_error("normal() supports 0–2 arguments");
            }

            if (sigma <= 0.0 || !std::isfinite(sigma)) {
                throw std::runtime_error("normal(mu,sigma): sigma must be > 0");
            }

            std::normal_distribution<double> dist(mu, sigma);
            return Value::FromNumber(dist(rng));
        }

        // ===== Запрет неявной статистики =====

        if (name == "mean" || name == "variance" ||
            name == "stddev" || name == "median" || name == "count") {
            throw std::runtime_error(
                "Implicit statistics are disabled. Use get_stat(\"" + name + "\", <sample>)"
            );
        }

        throw std::runtime_error("Unknown function: " + name);
    }

    throw std::runtime_error("Unknown expression type in interpreter");
}

/*
 * Вывести числовое значение.
 */
void Interpreter::PrintValue(const Value& v) {
    if (out) {
        (*out) << v.AsNumber() << "\n";
    }
}

/*
 * Получить статистическую характеристику выборки (2-аргументная форма).
 */
double Interpreter::GetSampleStat(const std::string& statName, const std::string& sampleName) {
    if (statName == "moment" || statName == "central_moment") {
        throw std::runtime_error(
            "Use get_stat(\"" + statName + "\", <sample>, k) with 3 arguments"
        );
    }

    const Sequence<Value>* seq = env.GetSample(sampleName);
    if (!seq || seq->GetLength() == 0) {
        throw std::runtime_error("Sample '" + sampleName + "' is empty or does not exist");
    }

    if (statName == "mean")      return Statistics::Mean(*seq);
    if (statName == "variance")  return Statistics::Variance(*seq);
    if (statName == "stddev")    return Statistics::StdDev(*seq);
    if (statName == "median")    return Statistics::Median(*seq);
    if (statName == "count")     return static_cast<double>(Statistics::Count(*seq));

    throw std::runtime_error("Unknown statistic: " + statName);
}

/*
 * Получить статистическую характеристику выборки с параметром k (3-аргументная форма).
 */
double Interpreter::GetSampleStat(const std::string& statName, const std::string& sampleName, std::size_t k) {
    const Sequence<Value>* seq = env.GetSample(sampleName);
    if (!seq || seq->GetLength() == 0) {
        throw std::runtime_error("Sample '" + sampleName + "' is empty or does not exist");
    }

    if (statName == "moment") {
        return Statistics::Moment(*seq, k);
    }
    if (statName == "central_moment") {
        return Statistics::CentralMoment(*seq, k);
    }

    throw std::runtime_error(
        "Statistic '" + statName + "' does not accept k. "
        "Use get_stat(\"mean\", A) or get_stat(\"moment\", A, 2)"
    );
}

/*
 * print_stat отключён.
 */
void Interpreter::ExecutePrintStat(const std::string&) {
    throw std::runtime_error(
        "print_stat is disabled. Use print(get_stat(\"mean\", <sample>))"
    );
}