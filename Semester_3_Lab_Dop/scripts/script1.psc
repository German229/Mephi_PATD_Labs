// ============================================================
// FULL OK TEST (should run without errors)
// Seed is taken from CLI: --seed=42
// ============================================================

// -------------------------------
// 0) Arithmetic + unary + precedence + comparisons
// -------------------------------
a = 10
b = 3

print(a + b)          // 13
print(a - b)          // 7
print(a * b)          // 30
print(a / b)          // 3.33333...
print(-a)             // -10

print((a + b) * 2)    // 26
print(a + b * 2)      // 16

print(a > b)          // 1
print(a < b)          // 0
print(a == b)         // 0
print(a != b)         // 1
print(a >= 10)        // 1
print(a <= 9)         // 0

if (a > b) {
    print(999)
}

// -------------------------------
// 1) repeat + assignment inside loop
// -------------------------------
N = 7
x = 0
repeat N {
    x = x + 1
}
print(x)              // 7

// -------------------------------
// 2) Deterministic named sample A
// A = [1,2,3,4]
// -------------------------------
collect A 1
collect A 2
collect A 3
collect A 4

print(get_stat("count", A))       // 4
print(get_stat("mean", A))        // 2.5
print(get_stat("variance", A))    // 1.25
print(get_stat("stddev", A))      // sqrt(1.25)=1.11803...
print(get_stat("median", A))      // 2.5

// moments
print(get_stat("moment", A, 0))           // 1
print(get_stat("moment", A, 1))           // 2.5
print(get_stat("moment", A, 2))           // 7.5
print(get_stat("moment", A, 3))           // 25

// central moments
print(get_stat("central_moment", A, 0))   // 1
print(get_stat("central_moment", A, 2))   // 1.25
print(get_stat("central_moment", A, 3))   // 0
print(get_stat("central_moment", A, 4))   // 2.5625

// composition: sqrt/pow/nesting
print(sqrt(get_stat("variance", A)))                          // == stddev
print(pow(get_stat("mean", A), 2))                            // 6.25
print(sqrt(pow(get_stat("stddev", A), 2)))                    // == stddev
print(sqrt(pow(get_stat("mean", A), 2) + pow(get_stat("stddev", A), 2)))  // sqrt(7.5)=2.73861...

// -------------------------------
// 3) Covariance / Correlation (2 samples, same length)
// X = [1,2,3,4]
// Y = [2,4,6,8]     => corr = +1, cov = 2.5  (population version)
// Yn = [8,6,4,2]    => corr = -1, cov = -2.5
// -------------------------------
collect X 1
collect X 2
collect X 3
collect X 4

collect Y 2
collect Y 4
collect Y 6
collect Y 8

collect Yn 8
collect Yn 6
collect Yn 4
collect Yn 2

print(get_stat("covariance", X, Y))      // 2.5
print(get_stat("correlation", X, Y))     // 1

print(get_stat("covariance", X, Yn))     // -2.5
print(get_stat("correlation", X, Yn))    // -1

// -------------------------------
// 4) Distributions: uniform/normal (just to ensure no runtime errors)
// -------------------------------
print(uniform())
print(uniform(10))
print(uniform(-1, 1))

print(normal())
print(normal(5))
print(normal(0, 2))

// -------------------------------
// 5) Random sample U: collect, then stats (values are seed-dependent)
// -------------------------------
repeat 200 {
    collect U uniform(0, 1)
}

print(get_stat("count", U))        // 200
print(get_stat("mean", U))         // ~0.5
print(get_stat("variance", U))     // ~0.083
print(get_stat("stddev", U))
print(get_stat("median", U))

// moments on random sample
print(get_stat("moment", U, 2))
print(get_stat("central_moment", U, 2))