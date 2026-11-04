#include "matrix.h"
#include <iostream>

Matrix Mcreate(int rows, int cols) {
    Matrix M;
    M.rows = rows;
    M.cols = cols;
    M.data = new double*[rows];
    for (int i=0; i<rows; i++) {
        M.data[i] = new double[cols]{};
    }
    return M;
}

Matrix Mcopy(const Matrix& A) {
    Matrix B = Mcreate(A.rows, A.cols);
    for (int i=0; i<A.rows; i++) {
        for (int j=0; j<A.cols; j++) {
            B.data[i][j]=A.data[i][j];
        }
    }
    return B;
}

void Mdelete(Matrix& M)  {
    for (int i=0; i<M.rows; i++) delete[] M.data[i];
    delete[] M.data;
    M.data = nullptr;
    M.rows = 0;
    M.cols = 0;
}

void Mswaprows(Matrix& M, int swap1, int swap2) {
    double* tmp = M.data[swap1];
    M.data[swap1]=M.data[swap2];
    M.data[swap2]=tmp;
}

void Mprint(const Matrix& M, bool augmented) {
    for (int i=0; i<M.rows; i++) {
        int j = 0;
        for (j; j<M.cols; j++) {
            if (augmented && j == M.cols-1) std::cout << "| ";
            std::cout << M.data[i][j] << " ";
        }
        std::cout << std::endl;
    }
}