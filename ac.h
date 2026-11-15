#include "circuit.h"
#include "complexmatrix.h"
#pragma once

struct ACComponentResult {
    std::complex<double> voltage;
    std::complex<double> current;
    std::complex<double> power;
};

struct ACResult {
    std::complex<double>* solution;
    int solutionSize;
    ACComponentResult* components;
    int componentCount;
    double frequency;
    double residual;
};

bool buildAC(const Circuit& circuit, double frequency, ComplexMatrix& A, std::complex<double>*& b);
bool calcACComponent(
    const Component& component,
    const std::complex<double>* solution,
    int nodeCount, int voltageSrcIndex, double frequency,
    std::complex<double>& voltage,
    std::complex<double>& current,
    std::complex<double>& power
);