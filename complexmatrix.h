#pragma once
#include <complex>

struct ComplexMatrix {
    std::complex<double>** data;
    int rows;
    int cols;
};

ComplexMatrix CMcreate(int rows, int cols);
ComplexMatrix CMcopy(const ComplexMatrix& matrix);
void CMdelete(ComplexMatrix& matrix);
void CMswaprows(ComplexMatrix& matrix, int r1, int r2);
void CMprint(const ComplexMatrix& matrix, bool augmented = false);