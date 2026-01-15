// =====================================================
// FULL OK TEST: samples + get_stat + composition + moments
// =====================================================

// ---------- Sample A: deterministic ----------
collect A 1
collect A 2
collect A 3
collect A 4

// Basic stats
print(get_stat("count", A))        // 4
print(get_stat("mean", A))         // 2.5
print(get_stat("variance", A))     // 1.25
print(get_stat("stddev", A))       // 1.11803...
print(get_stat("median", A))       // 2.5

// Moments (raw)
print(get_stat("moment", A, 0))    // 1
print(get_stat("moment", A, 1))    // 2.5
print(get_stat("moment", A, 2))    // 7.5
print(get_stat("moment", A, 3))    // 25

// Central moments
print(get_stat("central_moment", A, 0)) // 1
print(get_stat("central_moment", A, 1)) // 0
print(get_stat("central_moment", A, 2)) // 1.25
print(get_stat("central_moment", A, 3)) // 0

// Identity checks: should be 0 (or extremely close)
print(get_stat("mean", A) - get_stat("moment", A, 1))
print(get_stat("variance", A) - get_stat("central_moment", A, 2))
print(get_stat("stddev", A) - sqrt(get_stat("central_moment", A, 2)))
print(get_stat("moment", A, 2) - pow(get_stat("mean", A), 2) - get_stat("variance", A))

// Composition tests
print(sqrt(get_stat("variance", A)))
print(pow(get_stat("mean", A), 2))
print(sqrt(pow(get_stat("stddev", A), 2)))
print(sqrt(pow(get_stat("mean", A), 2) + pow(get_stat("stddev", A), 2)))

// ---------- Sample B: deterministic ----------
collect B 10
collect B 20
collect B 30
collect B 40

print(get_stat("count", B))        // 4
print(get_stat("mean", B))         // 25
print(get_stat("variance", B))     // 125
print(get_stat("stddev", B))       // 11.1803...
print(get_stat("median", B))       // 25

// Independence of samples
print(get_stat("mean", A) - get_stat("mean", B))            // 2.5 - 25 = -22.5
print(get_stat("variance", B) / get_stat("variance", A))   // 125 / 1.25 = 100

// ---------- Sample U: uniform random ----------
repeat 5 {
    collect U uniform(0, 1)
}
print(get_stat("count", U))       // 5
print(get_stat("mean", U))
print(get_stat("variance", U))
print(get_stat("moment", U, 2))
print(get_stat("central_moment", U, 2))