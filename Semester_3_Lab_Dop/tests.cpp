#include "tests.h"

#include <iostream>
#include <cassert>
#include <sstream>
#include <cmath>

#include "lexer.h"
#include "token.h"
#include "value.h"
#include "ast.h"
#include "parser.h"
#include "environment.h"
#include "statlib.h"
#include "interpreter.h"
#include <limits>

// ========================= Тесты для Value =========================

void TestValueBasic() {
    Value a = Value::FromNumber(2.0);
    Value b = Value::FromNumber(4.0);

    // Проверка типа
    assert(a.IsNumber());
    assert(b.IsNumber());
    assert(a.AsNumber() == 2.0);
    assert(b.AsNumber() == 4.0);

    // Арифметика
    Value c = a + b;
    assert(c.AsNumber() == 6.0);

    Value d = b - a;
    assert(d.AsNumber() == 2.0);

    Value e = a * b;
    assert(e.AsNumber() == 8.0);

    Value f = b / a;
    assert(f.AsNumber() == 2.0);

    Value g = -a;
    assert(g.AsNumber() == -2.0);

    // Простой smoke-тест ToString (строка не нулевая по длине)
    std::string s = c.ToString();
    assert(!s.empty());
}

// ========================= Вспомогательная функция для лексера =========================

void AssertToken(const Token& tok, TokenType expectedType, const std::string& expectedText) {
    assert(tok.type == expectedType);
    assert(tok.text == expectedText);
}

// ========================= Тесты для Lexer =========================

void TestLexerSimpleExpression() {
    std::string src = "x = 1 + 2\n";
    Lexer lexer(src);

    Token t1 = lexer.NextToken();
    AssertToken(t1, TokenType::Identifier, "x");

    Token t2 = lexer.NextToken();
    AssertToken(t2, TokenType::Assign, "=");

    Token t3 = lexer.NextToken();
    AssertToken(t3, TokenType::Number, "1");

    Token t4 = lexer.NextToken();
    AssertToken(t4, TokenType::Plus, "+");

    Token t5 = lexer.NextToken();
    AssertToken(t5, TokenType::Number, "2");

    Token t6 = lexer.NextToken();
    assert(t6.type == TokenType::EndOfFile);
}

void TestLexerKeywordsAndBlocks() {
    std::string src =
        "repeat 10 {\n"
        "  collect x\n"
        "  print x\n"
        "}\n";

    Lexer lexer(src);

    Token t1 = lexer.NextToken();
    AssertToken(t1, TokenType::KeywordRepeat, "repeat");

    Token t2 = lexer.NextToken();
    AssertToken(t2, TokenType::Number, "10");

    Token t3 = lexer.NextToken();
    AssertToken(t3, TokenType::LBrace, "{");

    Token t4 = lexer.NextToken();
    AssertToken(t4, TokenType::KeywordCollect, "collect");

    Token t5 = lexer.NextToken();
    AssertToken(t5, TokenType::Identifier, "x");

    Token t6 = lexer.NextToken();
    AssertToken(t6, TokenType::KeywordPrint, "print");

    Token t7 = lexer.NextToken();
    AssertToken(t7, TokenType::Identifier, "x");

    Token t8 = lexer.NextToken();
    AssertToken(t8, TokenType::RBrace, "}");

    Token t9 = lexer.NextToken();
    assert(t9.type == TokenType::EndOfFile);
}

void TestLexerStringLiteralAndPrintStat() {
    std::string src = "print_stat(\"mean\")\n";
    Lexer lexer(src);

    Token t1 = lexer.NextToken();
    AssertToken(t1, TokenType::KeywordPrintStat, "print_stat");

    Token t2 = lexer.NextToken();
    AssertToken(t2, TokenType::LParen, "(");

    Token t3 = lexer.NextToken();
    // В строковом литерале мы сохраняем текст БЕЗ кавычек
    AssertToken(t3, TokenType::StringLiteral, "mean");

    Token t4 = lexer.NextToken();
    AssertToken(t4, TokenType::RParen, ")");

    Token t5 = lexer.NextToken();
    assert(t5.type == TokenType::EndOfFile);
}

void TestLexerComments() {
    std::string src =
        "// это комментарий\n"
        "x = 1 // ещё комментарий\n";

    Lexer lexer(src);

    Token t1 = lexer.NextToken();
    AssertToken(t1, TokenType::Identifier, "x");

    Token t2 = lexer.NextToken();
    AssertToken(t2, TokenType::Assign, "=");

    Token t3 = lexer.NextToken();
    AssertToken(t3, TokenType::Number, "1");

    Token t4 = lexer.NextToken();
    assert(t4.type == TokenType::EndOfFile);
}

// ========================= Тесты для AST (ручная сборка) =========================

// Простейшая программа:
// x = 1
// print x
void TestASTSimpleProgram() {
    Program prog;

    // x = 1
    auto assign = std::make_unique<AssignStmt>(
        "x",
        std::make_unique<NumberExpr>(1.0)
    );

    // print x
    auto print = std::make_unique<PrintStmt>(
        std::make_unique<VariableExpr>("x")
    );

    prog.AddStatement(std::move(assign));
    prog.AddStatement(std::move(print));

    // Проверяем структуру
    assert(prog.statements.size() == 2);

    // 1-й оператор — AssignStmt
    Stmt* s1 = prog.statements[0].get();
    auto* assignPtr = dynamic_cast<AssignStmt*>(s1);
    assert(assignPtr != nullptr);
    assert(assignPtr->name == "x");

    auto* numExpr = dynamic_cast<NumberExpr*>(assignPtr->expression.get());
    assert(numExpr != nullptr);
    assert(numExpr->value == 1.0);

    // 2-й оператор — PrintStmt
    Stmt* s2 = prog.statements[1].get();
    auto* printPtr = dynamic_cast<PrintStmt*>(s2);
    assert(printPtr != nullptr);

    auto* varExpr = dynamic_cast<VariableExpr*>(printPtr->expression.get());
    assert(varExpr != nullptr);
    assert(varExpr->name == "x");
}

// Программа с repeat:
// repeat 3 {
//   collect x
//   print x
// }
void TestASTRepeatBlock() {
    Program prog;

    // Тело блока: collect x; print x;
    auto block = std::make_unique<BlockStmt>();

    block->AddStatement(
        std::make_unique<CollectStmt>(
            std::make_unique<VariableExpr>("x")
        )
    );

    block->AddStatement(
        std::make_unique<PrintStmt>(
            std::make_unique<VariableExpr>("x")
        )
    );

    // repeat 3 { ... }
    auto repeat = std::make_unique<RepeatStmt>(
        std::make_unique<NumberExpr>(3.0),
        std::move(block)
    );

    prog.AddStatement(std::move(repeat));

    // Проверяем структуру
    assert(prog.statements.size() == 1);

    Stmt* s = prog.statements[0].get();
    auto* repeatPtr = dynamic_cast<RepeatStmt*>(s);
    assert(repeatPtr != nullptr);

    auto* countNum = dynamic_cast<NumberExpr*>(repeatPtr->countExpr.get());
    assert(countNum != nullptr);
    assert(countNum->value == 3.0);

    auto* bodyBlock = repeatPtr->body.get();
    assert(bodyBlock != nullptr);
    assert(bodyBlock->statements.size() == 2);

    auto* collectPtr = dynamic_cast<CollectStmt*>(bodyBlock->statements[0].get());
    assert(collectPtr != nullptr);

    auto* printPtr = dynamic_cast<PrintStmt*>(bodyBlock->statements[1].get());
    assert(printPtr != nullptr);
}

// ========================= Тесты для Parser =========================

// x = 1 + 2
// print x
void TestParserSimpleProgram() {
    std::string src =
        "x = 1 + 2\n"
        "print x\n";

    Lexer lexer(src);
    Parser parser(lexer);

    Program prog = parser.ParseProgram();

    assert(prog.statements.size() == 2);

    // Проверяем первый оператор: x = 1 + 2
    auto* assign = dynamic_cast<AssignStmt*>(prog.statements[0].get());
    assert(assign != nullptr);
    assert(assign->name == "x");

    auto* binExpr = dynamic_cast<BinaryExpr*>(assign->expression.get());
    assert(binExpr != nullptr);
    assert(binExpr->op == BinaryOp::Add);

    auto* leftNum = dynamic_cast<NumberExpr*>(binExpr->left.get());
    auto* rightNum = dynamic_cast<NumberExpr*>(binExpr->right.get());
    assert(leftNum != nullptr);
    assert(rightNum != nullptr);
    assert(leftNum->value == 1.0);
    assert(rightNum->value == 2.0);

    // Проверяем второй оператор: print x
    auto* print = dynamic_cast<PrintStmt*>(prog.statements[1].get());
    assert(print != nullptr);

    auto* varExpr = dynamic_cast<VariableExpr*>(print->expression.get());
    assert(varExpr != nullptr);
    assert(varExpr->name == "x");
}

// repeat 3 {
//   collect x
//   print x
// }
void TestParserRepeatBlock() {
    std::string src =
        "repeat 3 {\n"
        "  collect x\n"
        "  print x\n"
        "}\n";

    Lexer lexer(src);
    Parser parser(lexer);

    Program prog = parser.ParseProgram();

    assert(prog.statements.size() == 1);

    auto* repeat = dynamic_cast<RepeatStmt*>(prog.statements[0].get());
    assert(repeat != nullptr);

    auto* countNum = dynamic_cast<NumberExpr*>(repeat->countExpr.get());
    assert(countNum != nullptr);
    assert(countNum->value == 3.0);

    auto* bodyBlock = repeat->body.get();
    assert(bodyBlock != nullptr);
    assert(bodyBlock->statements.size() == 2);

    auto* collectStmt = dynamic_cast<CollectStmt*>(bodyBlock->statements[0].get());
    assert(collectStmt != nullptr);

    auto* printStmt = dynamic_cast<PrintStmt*>(bodyBlock->statements[1].get());
    assert(printStmt != nullptr);
}

// print_stat("x")
void TestParserPrintStat() {
    std::string src =
        "print_stat(\"x\")\n";

    Lexer lexer(src);
    Parser parser(lexer);

    Program prog = parser.ParseProgram();

    assert(prog.statements.size() == 1);

    auto* ps = dynamic_cast<PrintStatStmt*>(prog.statements[0].get());
    assert(ps != nullptr);
    assert(ps->sampleName == "x");
}

// ========================= Тесты для Environment =========================

void TestEnvironmentVariables() {
    Environment env;

    // В начале переменных нет
    assert(!env.HasVariable("x"));

    // Устанавливаем переменную
    env.SetVariable("x", Value::FromNumber(42.0));
    assert(env.HasVariable("x"));

    Value vx = env.GetVariable("x");
    assert(vx.IsNumber());
    assert(vx.AsNumber() == 42.0);

    // Проверка, что при запросе несуществующей переменной бросается исключение
    bool threw = false;
    try {
        (void)env.GetVariable("y"); // y не существует
    } catch (const std::runtime_error&) {
        threw = true;
    }
    assert(threw);
}

void TestEnvironmentSamples() {
    Environment env;

    // Сначала выборок нет
    assert(!env.HasSample("x"));
    assert(env.GetSample("x") == nullptr);

    // Собираем значения в выборку "x"
    env.CollectSample("x", Value::FromNumber(1.0));
    env.CollectSample("x", Value::FromNumber(2.0));
    env.CollectSample("x", Value::FromNumber(3.5));

    assert(env.HasSample("x"));

    const Sequence<Value>* seq = env.GetSample("x");
    assert(seq != nullptr);
    assert(seq->GetLength() == 3);

    Value v0 = seq->Get(0);
    Value v1 = seq->Get(1);
    Value v2 = seq->Get(2);

    assert(v0.AsNumber() == 1.0);
    assert(v1.AsNumber() == 2.0);
    assert(v2.AsNumber() == 3.5);
}

// ========================= Тесты для Statistics =========================

void TestStatisticsBasic() {
    Environment env;
    // Выборка: 1.0, 2.0, 3.0, 4.0
    env.CollectSample("s", Value::FromNumber(1.0));
    env.CollectSample("s", Value::FromNumber(2.0));
    env.CollectSample("s", Value::FromNumber(3.0));
    env.CollectSample("s", Value::FromNumber(4.0));

    const Sequence<Value>* seq = env.GetSample("s");
    assert(seq != nullptr);

    std::size_t n = Statistics::Count(*seq);
    assert(n == 4);

    double mean = Statistics::Mean(*seq);
    // (1 + 2 + 3 + 4) / 4 = 2.5
    assert(std::fabs(mean - 2.5) < 1e-9);

    double var = Statistics::Variance(*seq);
    // mean = 2.5; дисперсия = 1.25
    assert(std::fabs(var - 1.25) < 1e-9);

    double sd = Statistics::StdDev(*seq);
    assert(std::fabs(sd - std::sqrt(1.25)) < 1e-9);

    double med = Statistics::Median(*seq);
    // медиана для чётной выборки: (2 + 3) / 2 = 2.5
    assert(std::fabs(med - 2.5) < 1e-9);

    // Проверим медиану на нечётной выборке
    env.CollectSample("t", Value::FromNumber(1.0));
    env.CollectSample("t", Value::FromNumber(2.0));
    env.CollectSample("t", Value::FromNumber(3.0));
    env.CollectSample("t", Value::FromNumber(4.0));
    env.CollectSample("t", Value::FromNumber(10.0));

    const Sequence<Value>* seq2 = env.GetSample("t");
    assert(seq2 != nullptr);
    double med2 = Statistics::Median(*seq2);
    assert(std::fabs(med2 - 3.0) < 1e-9);
}

// ========================= Тесты для Interpreter =========================

// 1) Базовые присваивания и выражения:
// x = 1 + 2
// y = x * 3
void TestInterpreterAssignAndExpr() {
    std::string src =
        "x = 1 + 2\n"
        "y = x * 3\n";

    Lexer lexer(src);
    Parser parser(lexer);
    Program prog = parser.ParseProgram();

    Interpreter interp(/*seed=*/123);
    interp.ExecuteProgram(prog);

    const Environment& env = interp.GetEnvironment();
    double x = env.GetVariable("x").AsNumber();
    double y = env.GetVariable("y").AsNumber();

    assert(std::fabs(x - 3.0) < 1e-9);
    assert(std::fabs(y - 9.0) < 1e-9);
}

// 2) repeat + collect:
// repeat 3 {
//   collect 1
// }
void TestInterpreterRepeatAndCollect() {
    std::string src =
        "repeat 3 {\n"
        "  collect 1\n"
        "}\n";

    Lexer lexer(src);
    Parser parser(lexer);
    Program prog = parser.ParseProgram();

    Interpreter interp(/*seed=*/123);
    interp.ExecuteProgram(prog);

    const Environment& env = interp.GetEnvironment();
    const Sequence<Value>* seq = env.GetSample("default");
    assert(seq != nullptr);
    assert(seq->GetLength() == 3);
    for (int i = 0; i < 3; ++i) {
        assert(std::fabs(seq->Get(i).AsNumber() - 1.0) < 1e-9);
    }
}

// 3) mean() как функция-выражение:
// collect 1
// collect 2
// collect 3
// x = mean()
void TestInterpreterMeanFunction() {
    std::string src =
        "collect 1\n"
        "collect 2\n"
        "collect 3\n"
        "x = mean()\n";

    Lexer lexer(src);
    Parser parser(lexer);
    Program prog = parser.ParseProgram();

    Interpreter interp(/*seed=*/123);
    interp.ExecuteProgram(prog);

    const Environment& env = interp.GetEnvironment();
    double x = env.GetVariable("x").AsNumber();
    // (1 + 2 + 3) / 3 = 2
    assert(std::fabs(x - 2.0) < 1e-9);
}

// 1) Лексер: запятая в вызове
void TestLexerComma() {
    std::string src = "x = uniform(1, 2)\n";
    Lexer lexer(src);

    Token t1 = lexer.NextToken(); assert(t1.type == TokenType::Identifier);
    Token t2 = lexer.NextToken(); assert(t2.type == TokenType::Assign);
    Token t3 = lexer.NextToken(); assert(t3.type == TokenType::Identifier); // uniform
    Token t4 = lexer.NextToken(); assert(t4.type == TokenType::LParen);
    Token t5 = lexer.NextToken(); assert(t5.type == TokenType::Number);
    Token t6 = lexer.NextToken(); assert(t6.type == TokenType::Comma);      // <-- NEW
    Token t7 = lexer.NextToken(); assert(t7.type == TokenType::Number);
    Token t8 = lexer.NextToken(); assert(t8.type == TokenType::RParen);
    Token t9 = lexer.NextToken(); assert(t9.type == TokenType::EndOfFile);
}

// 2) Парсер: два аргумента в CallExpr
void TestParserCallTwoArgs() {
    std::string src = "x = uniform(1, 2)\n";
    Lexer lexer(src);
    Parser parser(lexer);

    Program prog = parser.ParseProgram();
    assert(prog.statements.size() == 1);

    auto* assign = dynamic_cast<AssignStmt*>(prog.statements[0].get());
    assert(assign != nullptr);
    assert(assign->name == "x");

    auto* call = dynamic_cast<CallExpr*>(assign->expression.get());
    assert(call != nullptr);
    assert(call->callee == "uniform");
    assert(call->args.size() == 2);
}

// 3) Интерпретатор: uniform(a,b) даёт значение в диапазоне
void TestInterpreterUniformTwoArgsRange() {
    std::string src =
        "x = uniform(5, 7)\n"
        "collect x\n";

    Lexer lexer(src);
    Parser parser(lexer);
    Program prog = parser.ParseProgram();

    Interpreter interp(/*seed=*/123);
    interp.ExecuteProgram(prog);

    const Environment& env = interp.GetEnvironment();
    double x = env.GetVariable("x").AsNumber();
    assert(x >= 5.0 && x < 7.0);

    const Sequence<Value>* seq = env.GetSample("default");
    assert(seq && seq->GetLength() == 1);
    double s0 = seq->Get(0).AsNumber();
    assert(s0 >= 5.0 && s0 < 7.0);
}

// 4) Интерпретатор: normal(mu,sigma) возвращает конечное число
void TestInterpreterNormalTwoArgsFinite() {
    std::string src = "x = normal(0, 2)\n";
    Lexer lexer(src);
    Parser parser(lexer);
    Program prog = parser.ParseProgram();

    Interpreter interp(/*seed=*/123);
    interp.ExecuteProgram(prog);

    double x = interp.GetEnvironment().GetVariable("x").AsNumber();
    assert(std::isfinite(x));
}

// ========================= RunAllTests =========================

void RunAllTests() {
    // Value
    TestValueBasic();

    // Lexer
    TestLexerSimpleExpression();
    TestLexerKeywordsAndBlocks();
    TestLexerStringLiteralAndPrintStat();
    TestLexerComments();

    // AST (ручная сборка)
    TestASTSimpleProgram();
    TestASTRepeatBlock();

    // Parser (Lexer -> Parser -> AST)
    TestParserSimpleProgram();
    TestParserRepeatBlock();
    TestParserPrintStat();

    // Environment
    TestEnvironmentVariables();
    TestEnvironmentSamples();

    // Statistics
    TestStatisticsBasic();

    // Interpreter
    TestInterpreterAssignAndExpr();
    TestInterpreterRepeatAndCollect();
    TestInterpreterMeanFunction();

    TestLexerComma();
    TestParserCallTwoArgs();
    TestInterpreterUniformTwoArgsRange();
    TestInterpreterNormalTwoArgsFinite();

    std::cout << "All tests for front-end, environment, statistics, and interpreter passed successfully.\n";
}