#ifndef MATRIX_4_H
#define MATRIX_4_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    MATRIX_INT,
    MATRIX_DOUBLE
} MatrixType;

typedef struct Matrix Matrix;

typedef struct {
    const char* name;
    size_t elemSize;
} TypeInfo;

struct Matrix {
    int rows;
    int cols;
    MatrixType type;
    void* data;
    TypeInfo* ops;
};

Matrix* create_double_matrix(int rows, int cols);
void destroyMatrix(Matrix* m);
void setValueToMatrix(Matrix* m, int row, int col, double val);
double getDoubleElement(const Matrix* m, int row, int col);
void printMatrix(const Matrix* m);

Matrix* LU_decompose(const Matrix* A, Matrix** out_L, Matrix** out_U);
Matrix* solveLU(const Matrix* L, const Matrix* U, const Matrix* b);

#endif
