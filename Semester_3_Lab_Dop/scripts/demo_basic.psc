N = 100

repeat N {
    x = uniform(0, 1)
    collect x
}

print_stat("count")
print_stat("mean")
print_stat("variance")
print_stat("stddev")
print_stat("median")