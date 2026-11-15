#include "complexmatrix.h"
#include <iostream>
#include <complex>

ComplexMatrix CMcreate(int rows, int cols) {
    ComplexMatrix M;
    M.rows = rows;
    M.cols = cols;
    M.data = new std::complex<double>*[rows]{};
    for (int i=0; i<rows; i++) {
        M.data[i] = new std::complex<double>[cols]{};
    }
    return M;
}

ComplexMatrix CMcopy(const ComplexMatrix& A) {
    ComplexMatrix B = CMcreate(A.rows, A.cols);
    for (int i=0; i<A.rows; i++) {
        for (int j=0; j<A.cols; j++) {
            B.data[i][j]=A.data[i][j];
        }
    }
    return B;
}

void CMdelete(ComplexMatrix& M)  {
    for (int i=0; i<M.rows; i++) delete[] M.data[i];
    delete[] M.data;
    M.data = nullptr;
    M.rows = 0;
    M.cols = 0;
}

void CMswaprows(ComplexMatrix& M, int swap1, int swap2) {
    std::complex<double>* tmp = M.data[swap1];
    M.data[swap1]=M.data[swap2];
    M.data[swap2]=tmp;
}

void CMprint(const ComplexMatrix& M, bool augmented) {
    for (int i=0; i<M.rows; i++) {
        for (int j=0; j<M.cols; j++) {
            if (augmented && j == M.cols-1) std::cout << "| ";
            std::cout << M.data[i][j] << " ";
        }
        std::cout << std::endl;
    }
}