
N = 10000
repeat N {
y = normal(0, 1)
collect y
}

print_stat("count")
print_stat("mean")
print_stat("variance")
print_stat("stddev")
print_stat("median")

// print(get_stat("count",l))
// l >> get_count >> print