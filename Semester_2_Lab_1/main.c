#include "matrix_4.h"

int main() {
    int n = 3;
    Matrix* A = create_double_matrix(n, n);
    Matrix* b = create_double_matrix(n, 1);

    double valsA[9] = {
        2, -1, -2,
        -4, 6, 3,
        -4, -2, 8
    };
    double valsB[3] = {-2, 9, -6};

    for (int i = 0; i < n * n; i++)
        setValueToMatrix(A, i / n, i % n, valsA[i]);

    for (int i = 0; i < n; i++)
        setValueToMatrix(b, i, 0, valsB[i]);

    printf("Matrix A:\n");
    printMatrix(A);
    printf("Vector b:\n");
    printMatrix(b);

    Matrix *L, *U;
    LU_decompose(A, &L, &U);

    printf("L matrix:\n");
    printMatrix(L);
    printf("U matrix:\n");
    printMatrix(U);

    Matrix* x = solveLU(L, U, b);
    printf("Solution x:\n");
    printMatrix(x);

    destroyMatrix(A);
    destroyMatrix(b);
    destroyMatrix(L);
    destroyMatrix(U);
    destroyMatrix(x);

    return 0;
}
