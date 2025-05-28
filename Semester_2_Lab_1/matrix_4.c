#include "matrix_4.h"

Matrix* create_double_matrix(int rows, int cols) {
    Matrix* m = (Matrix*) malloc(sizeof(Matrix));
    if (!m) return NULL;
    m->rows = rows;
    m->cols = cols;
    m->type = MATRIX_DOUBLE;
    m->data = calloc(rows * cols, sizeof(double));
    m->ops = (TypeInfo*) malloc(sizeof(TypeInfo));
    m->ops->name = "double";
    m->ops->elemSize = sizeof(double);
    return m;
}

void destroyMatrix(Matrix* m) {
    if (m) {
        free(m->data);
        free(m->ops);
        free(m);
    }
}

void setValueToMatrix(Matrix* m, int row, int col, double val) {
    if (m->type != MATRIX_DOUBLE) return;
    double* data = (double*)m->data;
    data[row * m->cols + col] = val;
}

double getDoubleElement(const Matrix* m, int row, int col) {
    double* data = (double*)m->data;
    return data[row * m->cols + col];
}

void printMatrix(const Matrix* m) {
    double* data = (double*)m->data;
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            printf("%.4lf ", data[i * m->cols + j]);
        }
        printf("\n");
    }
}

Matrix* LU_decompose(const Matrix* A, Matrix** out_L, Matrix** out_U) {
    int n = A->rows;
    *out_L = create_double_matrix(n, n);
    *out_U = create_double_matrix(n, n);
    Matrix* L = *out_L;
    Matrix* U = *out_U;

    for (int i = 0; i < n; i++) {
        for (int k = i; k < n; k++) {
            double sum = 0;
            for (int j = 0; j < i; j++)
                sum += getDoubleElement(L, i, j) * getDoubleElement(U, j, k);
            double val = getDoubleElement(A, i, k) - sum;
            setValueToMatrix(U, i, k, val);
        }

        for (int k = i; k < n; k++) {
            if (i == k)
                setValueToMatrix(L, i, i, 1.0);
            else {
                double sum = 0;
                for (int j = 0; j < i; j++)
                    sum += getDoubleElement(L, k, j) * getDoubleElement(U, j, i);
                double val = (getDoubleElement(A, k, i) - sum) / getDoubleElement(U, i, i);
                setValueToMatrix(L, k, i, val);
            }
        }
    }
    return NULL;
}

Matrix* solveLU(const Matrix* L, const Matrix* U, const Matrix* b) {
    int n = L->rows;
    Matrix* y = create_double_matrix(n, 1);
    Matrix* x = create_double_matrix(n, 1);

    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int j = 0; j < i; j++)
            sum += getDoubleElement(L, i, j) * getDoubleElement(y, j, 0);
        double val = (getDoubleElement(b, i, 0) - sum);
        setValueToMatrix(y, i, 0, val);
    }

    for (int i = n - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < n; j++)
            sum += getDoubleElement(U, i, j) * getDoubleElement(x, j, 0);
        double val = (getDoubleElement(y, i, 0) - sum) / getDoubleElement(U, i, i);
        setValueToMatrix(x, i, 0, val);
    }

    destroyMatrix(y);
    return x;
}
