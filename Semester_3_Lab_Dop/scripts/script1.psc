// ==============================
// Comprehensive test: samples + get_stat + moments + composition
// ==============================

// ---------- Sample A: deterministic 1 2 3 4 ----------
collect A 1
collect A 2
collect A 3
collect A 4

// базовые статистики A
print(get_stat("count", A))      // expect 4
print(get_stat("mean", A))       // expect 2.5
print(get_stat("variance", A))   // expect 1.25
print(get_stat("stddev", A))     // expect ~1.1180339887
print(get_stat("median", A))     // expect 2.5

// моменты A
print(get_stat("moment", A, 0))           // expect 1
print(get_stat("moment", A, 1))           // expect 2.5  (== mean)
print(get_stat("moment", A, 2))           // expect 7.5  (E[X^2] for 1..4)
print(get_stat("moment", A, 3))           // expect 25   (E[X^3] for 1..4)

print(get_stat("central_moment", A, 0))   // expect 1
print(get_stat("central_moment", A, 1))   // expect 0
print(get_stat("central_moment", A, 2))   // expect 1.25 (== variance)
print(get_stat("central_moment", A, 3))   // expect 0    (симметрия относительно mean для 1..4)

// тождества на A (печатаем разности — должны быть 0 или около 0)
print(get_stat("mean", A) - get_stat("moment", A, 1))
print(get_stat("variance", A) - get_stat("central_moment", A, 2))
print(get_stat("stddev", A) - sqrt(get_stat("central_moment", A, 2)))

// variance = E[X^2] - (E[X])^2
print(get_stat("moment", A, 2) - pow(get_stat("mean", A), 2) - get_stat("variance", A))

// композиция функций на A
print(sqrt(get_stat("variance", A))) // == stddev
print(pow(get_stat("stddev", A), 2)) // == variance
print(sqrt(pow(get_stat("stddev", A), 2))) // == stddev
print(sqrt(pow(get_stat("mean", A), 2) + pow(get_stat("stddev", A), 2))) // just composition check

// ---------- Sample B: deterministic 10 20 30 40 ----------
collect B 10
collect B 20
collect B 30
collect B 40

print(get_stat("count", B))      // 4
print(get_stat("mean", B))       // 25
print(get_stat("variance", B))   // 125
print(get_stat("stddev", B))     // ~11.180339887
print(get_stat("median", B))     // 25

// моменты B + тождества
print(get_stat("moment", B, 1) - get_stat("mean", B))              // 0
print(get_stat("central_moment", B, 2) - get_stat("variance", B))  // 0
print(get_stat("moment", B, 2) - pow(get_stat("mean", B), 2) - get_stat("variance", B)) // 0

// ---------- Mixed: ensure A and B are distinct ----------
print(get_stat("mean", A) - get_stat("mean", B))
print(get_stat("variance", B) / get_stat("variance", A))

// ---------- Nested deep example from requirement ----------
print(get_stat("moment", A, 2) - pow(get_stat("mean", A), 2)) // should equal variance(A)
print(sqrt(get_stat("variance", A)))                          // should equal stddev(A)
