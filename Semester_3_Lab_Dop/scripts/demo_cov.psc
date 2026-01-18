repeat 200 {
    x = normal(0, 1)
    y = 2 * x + normal(0, 0.5)

    collect A x
    collect B y
    collect P (x * y)
}

ex = get_stat("mean", A)
ey = get_stat("mean", B)
exy = get_stat("mean", P)

cov = exy - ex * ey
sx = get_stat("stddev", A)
sy = get_stat("stddev", B)

print(cov)
print(cov / (sx * sy))