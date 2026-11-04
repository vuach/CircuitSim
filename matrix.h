#pragma once

struct Matrix {
    double** data;
    int rows;
    int cols;
};

Matrix Mcreate(int rows, int cols);
Matrix Mcopy(const Matrix& A);
void Mdelete(Matrix& M);
void Mswaprows(Matrix& M, int swap1, int swap2);
void Mprint(const Matrix& M, bool ampl = false);