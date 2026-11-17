#pragma once
#include "ac.h"
#include <complex>

struct FrequencyPoint {
    double frequency;
    std::complex<double> transfer;
};

struct SweepResult {
    FrequencyPoint* points;
    int count;
};

bool frequencySweep(const Circuit& circuit, int inputNode, int outputNode, double startFrequency,
    double endFrequency, int pointCount, SweepResult& result);
void deleteSweepResult(SweepResult& result);