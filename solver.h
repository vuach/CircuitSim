#pragma once
#include "matrix.h"

bool GaussForw(Matrix& augmented);
bool GaussBack(const Matrix& augmented, double* x);
bool GaussSolve(const Matrix& A, const double* b, double* x);
double residual(const Matrix& A, const double* b, const double* x);