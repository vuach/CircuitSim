#pragma once
#include "complexmatrix.h"

bool CGaussForw(ComplexMatrix& augmented, double eps = 1e-12);
bool CGaussBack(const ComplexMatrix& augmented, std::complex<double>* x, double eps = 1e-12);
bool CGaussSolve(const ComplexMatrix& augmented, const std::complex<double>* b, std::complex<double>* x);
double Cresidual(const ComplexMatrix& augmented, const std::complex<double>* b, std::complex<double>* x);