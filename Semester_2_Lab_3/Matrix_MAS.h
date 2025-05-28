#ifndef MATRIX_MAS_H
#define MATRIX_MAS_H

#include "sequence.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <vector>

template<typename T>
class Matrix {
public:
    Matrix(int n, int m);
    Matrix(int n, int m, const T* items);
    Matrix(const Matrix<T>& other);
    Matrix<T>& operator=(const Matrix<T>& other);

    Matrix<T> operator+(const Matrix<T>& other) const;
    Matrix<T> operator*(const Matrix<T>& other) const;
    Matrix<T> operator*(T k) const;
    Matrix<T> Transpose() const;

    void SwapColumns(int a, int b);
    void SwapRows(int a, int b);
    void ScaleColumn(int ind, T k);
    void ScaleRow(int ind, T k);
    void multiplyRow(int row, T k);
    void addLinearCombination(int sourceRow, int targetRow, T k);

    void print() const;
    T determinant() const;
    Matrix<T> solveSLAE(const Matrix<T>& b) const;
    int findBiggestInColumn(int col, int startRow = 0) const;
    Matrix<T> getInverse() const;
    T getKNorm() const;
    T getCondKNorm() const;

private:
    int n, m;
    MutableArraySequence<MutableArraySequence<T>> data;
};

template<typename T>
Matrix<T>::Matrix(int n_, int m_)
    : n(n_), m(m_) {
    auto* rowsArr = new MutableArraySequence<T>[n_];
    for (int i = 0; i < n_; ++i) {
        T* buf = new T[m_];
        for (int j = 0; j < m_; ++j) buf[j] = T(0);
        rowsArr[i] = MutableArraySequence<T>(buf, m_);
        delete[] buf;
    }
    data = MutableArraySequence<MutableArraySequence<T>>(rowsArr, n_);
    delete[] rowsArr;
}

template<typename T>
Matrix<T>::Matrix(int n_, int m_, const T* items_)
    : n(n_), m(m_) {
    auto* rowsArr = new MutableArraySequence<T>[n_];
    for (int i = 0; i < n_; ++i) {
        T* buf = new T[m_];
        for (int j = 0; j < m_; ++j)
            buf[j] = items_[i * m_ + j];
        rowsArr[i] = MutableArraySequence<T>(buf, m_);
        delete[] buf;
    }
    data = MutableArraySequence<MutableArraySequence<T>>(rowsArr, n_);
    delete[] rowsArr;
}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& other)
    : n(other.n), m(other.m), data(other.data) {}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other) {
    if (this != &other) {
        n = other.n;
        m = other.m;
        data = other.data;
    }
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& o) const {
    if (n != o.n || m != o.m) throw std::invalid_argument("Matrix dimensions must match");
    Matrix<T> r(n, m);
    for (int i = 0; i < n; ++i) {
        T* buf = new T[m];
        for (int j = 0; j < m; ++j)
            buf[j] = data.Get(i).Get(j) + o.data.Get(i).Get(j);
        r.data.Set(i, MutableArraySequence<T>(buf, m));
        delete[] buf;
    }
    return r;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& o) const {
    if (m != o.n) throw std::invalid_argument("Inner dimensions must match");
    Matrix<T> r(n, o.m);
    for (int i = 0; i < n; ++i) {
        T* buf = new T[o.m];
        for (int j = 0; j < o.m; ++j) {
            T sum = T(0);
            for (int k = 0; k < m; ++k)
                sum += data.Get(i).Get(k) * o.data.Get(k).Get(j);
            buf[j] = sum;
        }
        r.data.Set(i, MutableArraySequence<T>(buf, o.m));
        delete[] buf;
    }
    return r;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(T k) const {
    Matrix<T> r(n, m);
    for (int i = 0; i < n; ++i) {
        T* buf = new T[m];
        for (int j = 0; j < m; ++j)
            buf[j] = data.Get(i).Get(j) * k;
        r.data.Set(i, MutableArraySequence<T>(buf, m));
        delete[] buf;
    }
    return r;
}

template<typename T>
Matrix<T> Matrix<T>::Transpose() const {
    Matrix<T> r(m, n);
    for (int i = 0; i < m; ++i) {
        T* buf = new T[n];
        for (int j = 0; j < n; ++j)
            buf[j] = data.Get(j).Get(i);
        r.data.Set(i, MutableArraySequence<T>(buf, n));
        delete[] buf;
    }
    return r;
}

template<typename T>
void Matrix<T>::SwapRows(int a, int b) {
    auto ra = data.Get(a);
    auto rb = data.Get(b);
    data.Set(a, rb);
    data.Set(b, ra);
}

template<typename T>
void Matrix<T>::SwapColumns(int a, int b) {
    for (int i = 0; i < n; ++i) {
        auto row = data.Get(i);
        T t = row.Get(a);
        row.Set(a, row.Get(b));
        row.Set(b, t);
        data.Set(i, row);
    }
}

template<typename T>
void Matrix<T>::ScaleRow(int ind, T k) {
    auto row = data.Get(ind);
    for (int j = 0; j < m; ++j)
        row.Set(j, row.Get(j) * k);
    data.Set(ind, row);
}

template<typename T>
void Matrix<T>::ScaleColumn(int ind, T k) {
    for (int i = 0; i < n; ++i) {
        auto row = data.Get(i);
        row.Set(ind, row.Get(ind) * k);
        data.Set(i, row);
    }
}

template<typename T>
void Matrix<T>::multiplyRow(int row, T k) {
    ScaleRow(row, k);
}

template<typename T>
void Matrix<T>::addLinearCombination(int s, int t, T k) {
    auto rs = data.Get(s);
    auto rt = data.Get(t);
    for (int j = 0; j < m; ++j)
        rt.Set(j, rt.Get(j) + rs.Get(j) * k);
    data.Set(t, rt);
}

template<typename T>
void Matrix<T>::print() const {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j)
            std::cout << data.Get(i).Get(j) << " ";
        std::cout << "\n";
    }
}

template<typename T>
int Matrix<T>::findBiggestInColumn(int col, int startRow) const {
    int idx = startRow;
    T maxv = std::abs(data.Get(startRow).Get(col));
    for (int i = startRow + 1; i < n; ++i) {
        T v = std::abs(data.Get(i).Get(col));
        if (v > maxv) { maxv = v; idx = i; }
    }
    return idx;
}

template<typename T>
T Matrix<T>::determinant() const {
    if (n != m) throw std::domain_error("Determinant defined only for square matrices");
    std::vector<std::vector<T>> tmp(n, std::vector<T>(m));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            tmp[i][j] = data.Get(i).Get(j);
    T det = T(1);
    for (int i = 0; i < n; ++i) {
        int p = i;
        for (int r = i; r < n; ++r)
            if (std::abs(tmp[r][i]) > std::abs(tmp[p][i])) p = r;
        if (std::abs(tmp[p][i]) < T(1e-12)) return T(0);
        if (p != i) {
            std::swap(tmp[p], tmp[i]);
            det = -det;
        }
        det *= tmp[i][i];
        for (int r = i + 1; r < n; ++r) {
            T f = tmp[r][i] / tmp[i][i];
            for (int c = i; c < n; ++c)
                tmp[r][c] -= f * tmp[i][c];
        }
    }
    return det;
}

template<typename T>
Matrix<T> Matrix<T>::solveSLAE(const Matrix<T>& b) const {
    if (n != m || b.n != n) throw std::invalid_argument("Invalid dimensions for SLAE");
    int cols = m + b.m;
    std::vector<std::vector<T>> aug(n, std::vector<T>(cols));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j)
            aug[i][j] = data.Get(i).Get(j);
        for (int j = 0; j < b.m; ++j)
            aug[i][m + j] = b.data.Get(i).Get(j);
    }
    for (int i = 0; i < n; ++i) {
        int p = i;
        for (int r = i; r < n; ++r)
            if (std::abs(aug[r][i]) > std::abs(aug[p][i])) p = r;
        if (std::abs(aug[p][i]) < T(1e-12)) throw std::runtime_error("Matrix is singular");
        std::swap(aug[p], aug[i]);
        T d = aug[i][i];
        for (int c = i; c < cols; ++c) aug[i][c] /= d;
        for (int r = 0; r < n; ++r) {
            if (r == i) continue;
            T f = aug[r][i];
            for (int c = i; c < cols; ++c)
                aug[r][c] -= f * aug[i][c];
        }
    }
    T* sol = new T[n * b.m];
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < b.m; ++j)
            sol[i * b.m + j] = aug[i][m + j];
    Matrix<T> x(n, b.m, sol);
    delete[] sol;
    return x;
}

template<typename T>
Matrix<T> Matrix<T>::getInverse() const {
    Matrix<T> I(n, n);
    for (int i = 0; i < n; ++i) {
        auto r = I.data.Get(i);
        for (int j = 0; j < n; ++j)
            r.Set(j, (i == j ? T(1) : T(0)));
        I.data.Set(i, r);
    }
    return solveSLAE(I);
}

template<typename T>
T Matrix<T>::getKNorm() const {
    T best = T(0);
    for (int j = 0; j < m; ++j) {
        T s = T(0);
        for (int i = 0; i < n; ++i)
            s += std::abs(data.Get(i).Get(j));
        if (s > best) best = s;
    }
    return best;
}

template<typename T>
T Matrix<T>::getCondKNorm() const {
    auto inv = getInverse();
    return getKNorm() * inv.getKNorm();
}

#endif
