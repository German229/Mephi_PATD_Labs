#include "interpreter.h"

#include <iostream>
#include <stdexcept>
#include <cmath>

namespace {
    const char* DEFAULT_SAMPLE_NAME = "default";
}

Interpreter::Interpreter(unsigned int seed)
    : env(),
      rng(seed),
      out(&std::cout)
{
}

void Interpreter::SetOutputStream(std::ostream& os) {
    out = &os;
}

void Interpreter::ExecuteProgram(const Program& program) {
    for (const auto& stmtPtr : program.statements) {
        ExecuteStatement(stmtPtr.get());
    }
}

void Interpreter::ExecuteStatement(Stmt* stmt) {
    if (auto* assign = dynamic_cast<AssignStmt*>(stmt)) {
        Value val = EvaluateExpression(assign->expression.get());
        env.SetVariable(assign->name, val);
        return;
    }

    if (auto* printStmt = dynamic_cast<PrintStmt*>(stmt)) {
        Value val = EvaluateExpression(printStmt->expression.get());
        PrintValue(val);
        return;
    }

    if (auto* collectStmt = dynamic_cast<CollectStmt*>(stmt)) {
        Value val = EvaluateExpression(collectStmt->expression.get());
        env.CollectSample(DEFAULT_SAMPLE_NAME, val);
        return;
    }

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

    if (auto* ps = dynamic_cast<PrintStatStmt*>(stmt)) {
        ExecutePrintStat(ps->sampleName);
        return;
    }

    if (auto* block = dynamic_cast<BlockStmt*>(stmt)) {
        ExecuteBlock(block);
        return;
    }

    throw std::runtime_error("Unknown statement type in interpreter");
}

void Interpreter::ExecuteBlock(BlockStmt* block) {
    for (const auto& stmtPtr : block->statements) {
        ExecuteStatement(stmtPtr.get());
    }
}

Value Interpreter::EvaluateExpression(Expr* expr) {
    if (auto* num = dynamic_cast<NumberExpr*>(expr)) {
        return Value::FromNumber(num->value);
    }

    if (auto* var = dynamic_cast<VariableExpr*>(expr)) {
        return env.GetVariable(var->name);
    }

    if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
        Value val = EvaluateExpression(unary->operand.get());
        switch (unary->op) {
            case UnaryOp::Negate:
                return -val;
        }
        throw std::runtime_error("Unknown unary operator");
    }

    if (auto* bin = dynamic_cast<BinaryExpr*>(expr)) {
        Value left = EvaluateExpression(bin->left.get());
        Value right = EvaluateExpression(bin->right.get());

        switch (bin->op) {
            case BinaryOp::Add:      return left + right;
            case BinaryOp::Subtract: return left - right;
            case BinaryOp::Multiply: return left * right;
            case BinaryOp::Divide:   return left / right;
        }
        throw std::runtime_error("Unknown binary operator");
    }

    if (auto* call = dynamic_cast<CallExpr*>(expr)) {
        const std::string& name = call->callee;
        const auto& args = call->args;

        // ---------------- Distributions ----------------

        // uniform(): U(0,1)
        // uniform(b): U(0,b)
        // uniform(a,b): U(a,b)
        if (name == "uniform") {
            double a = 0.0;
            double b = 1.0;

            if (args.size() == 0) {
                // ok
            } else if (args.size() == 1) {
                b = EvaluateExpression(args[0].get()).AsNumber();
            } else if (args.size() == 2) {
                a = EvaluateExpression(args[0].get()).AsNumber();
                b = EvaluateExpression(args[1].get()).AsNumber();
            } else {
                throw std::runtime_error("uniform() supports 0, 1, or 2 arguments");
            }

            if (!(a < b)) {
                throw std::runtime_error("uniform(a,b): expected a < b");
            }

            std::uniform_real_distribution<double> dist(a, b);
            return Value::FromNumber(dist(rng));
        }

        // normal(): N(0,1)
        // normal(mu): N(mu,1)
        // normal(mu,sigma): N(mu,sigma)
        if (name == "normal") {
            double mu = 0.0;
            double sigma = 1.0;

            if (args.size() == 0) {
                // ok
            } else if (args.size() == 1) {
                mu = EvaluateExpression(args[0].get()).AsNumber();
            } else if (args.size() == 2) {
                mu = EvaluateExpression(args[0].get()).AsNumber();
                sigma = EvaluateExpression(args[1].get()).AsNumber();
            } else {
                throw std::runtime_error("normal() supports 0, 1, or 2 arguments");
            }

            if (!(sigma > 0.0) || !std::isfinite(sigma)) {
                throw std::runtime_error("normal(mu,sigma): expected sigma > 0");
            }

            std::normal_distribution<double> dist(mu, sigma);
            return Value::FromNumber(dist(rng));
        }

        // ---------------- Statistics as expressions ----------------
        if (name == "mean" || name == "variance" ||
            name == "stddev" || name == "median" || name == "count") {

            if (!args.empty()) {
                throw std::runtime_error(name + "() does not accept arguments");
            }
            double res = GetSampleStat(name);
            return Value::FromNumber(res);
        }

        throw std::runtime_error("Unknown function: " + name);
    }

    throw std::runtime_error("Unknown expression type in interpreter");
}

void Interpreter::PrintValue(const Value& v) {
    if (out) {
        (*out) << v.AsNumber() << "\n";
    }
}

double Interpreter::GetSampleStat(const std::string& statName) {
    const Sequence<Value>* seq = env.GetSample(DEFAULT_SAMPLE_NAME);
    if (!seq || seq->GetLength() == 0) {
        throw std::runtime_error("No samples collected for statistics");
    }

    if (statName == "mean") {
        return Statistics::Mean(*seq);
    } else if (statName == "variance" || statName == "var") {
        return Statistics::Variance(*seq);
    } else if (statName == "stddev" || statName == "std") {
        return Statistics::StdDev(*seq);
    } else if (statName == "median") {
        return Statistics::Median(*seq);
    } else if (statName == "count") {
        return static_cast<double>(Statistics::Count(*seq));
    }

    throw std::runtime_error("Unknown statistic: " + statName);
}

// ВАЖНО: здесь меняем формат вывода на читаемый
void Interpreter::ExecutePrintStat(const std::string& what) {
    double value = GetSampleStat(what);

    if (!out) return;

    if (what == "count") {
        // печатаем как целое
        (*out) << "count = " << static_cast<std::size_t>(value) << "\n";
        return;
    }

    (*out) << what << " = " << value << "\n";
}