// ===============================
// TEST: Named samples + get_stat()
// ===============================

// ---------- 0) Deterministic sample for sanity checks ----------
collect D 1
collect D 2
collect D 3
collect D 4

print(get_stat("count", D))     // expected 4
print(get_stat("mean", D))      // expected 2.5
print(get_stat("variance", D))  // population variance for [1,2,3,4] = 1.25 (если у тебя population)
print(get_stat("stddev", D))    // sqrt(variance) ~ 1.118...
print(get_stat("median", D))    // for [1,2,3,4] expected 2.5 (если median как среднее двух серед.)

// Проверка if + сравнения (должно напечатать 111 и 222, а 333 не печатать)
if get_stat("count", D) == 4 {
print 111
}
if get_stat("mean", D) > 2 {
print 222
}
if get_stat("variance", D) < 1 {
print 333
}

// ---------- 1) uniform() : 0 args ----------
N = 200
repeat N {
x = uniform()
collect U0 x
}

print(get_stat("count", U0))
print(get_stat("mean", U0))
print(get_stat("variance", U0))
print(get_stat("stddev", U0))
print(get_stat("median", U0))

// Композиция через выражения (пример формулы)
print(get_stat("mean", U0) + 2 * get_stat("stddev", U0))

// ---------- 2) uniform(b) : 1 arg ----------
B = 5
repeat N {
y = uniform(B)          // U(0, B)
collect U1 y
}

print(get_stat("count", U1))
print(get_stat("mean", U1))     // близко к B/2
print(get_stat("variance", U1)) // близко к B^2/12
print(get_stat("stddev", U1))
print(get_stat("median", U1))

// ---------- 3) uniform(a,b) : 2 args ----------
A = -2
C = 3
repeat N {
z = uniform(A, C)       // U(A, C)
collect U2 z
}

print(get_stat("count", U2))
print(get_stat("mean", U2))     // близко к (A+C)/2
print(get_stat("variance", U2)) // близко к (C-A)^2/12
print(get_stat("stddev", U2))
print(get_stat("median", U2))

// ---------- 4) normal() : 0 args ----------
repeat N {
n0 = normal()           // N(0,1)
collect N0 n0
}

print(get_stat("count", N0))
print(get_stat("mean", N0))     // близко к 0
print(get_stat("variance", N0)) // близко к 1
print(get_stat("stddev", N0))
print(get_stat("median", N0))

// ---------- 5) normal(mu) : 1 arg ----------
MU = 10
repeat N {
n1 = normal(MU)         // N(MU,1)
collect N1 n1
}

print(get_stat("count", N1))
print(get_stat("mean", N1))     // близко к MU
print(get_stat("variance", N1)) // близко к 1
print(get_stat("stddev", N1))
print(get_stat("median", N1))

// ---------- 6) normal(mu, sigma) : 2 args ----------
MU2 = -3
SIG = 2
repeat N {
n2 = normal(MU2, SIG)   // N(MU2, SIG)
collect N2 n2
}

print(get_stat("count", N2))
print(get_stat("mean", N2))      // близко к MU2
print(get_stat("variance", N2))  // близко к SIG^2
print(get_stat("stddev", N2))    // близко к SIG
print(get_stat("median", N2))

// ---------- 7) Cross-sample check (independence) ----------
// Простая проверка: средние у выборок должны отличаться (как правило)
if get_stat("mean", N1) > get_stat("mean", N0) {
print 9001
}
if get_stat("mean", U2) > get_stat("mean", U0) {
print 9002
}

// ---------- 8) Nested repeats + arithmetic stress ----------
K = 5
repeat K {
repeat 20 {
t = uniform(-1, 1)
collect STRESS t
}
}

// Проверка, что реально собрали K*20 элементов
print(get_stat("count", STRESS))
print(get_stat("mean", STRESS))
print(get_stat("variance", STRESS))