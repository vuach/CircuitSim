#include "sweep.h"
#include <iostream>
#include <cmath>
#include <complex>

bool frequencySweep(const Circuit& circuit, int inputNode, int outputNode, double startFrequency, double endFrequency, int pointCount, SweepResult& result) {
    if (!std::isfinite(startFrequency) || !std::isfinite(endFrequency) || startFrequency<=0 || startFrequency>endFrequency || pointCount<2 || inputNode<=0 || outputNode<=0 || inputNode>circuit.nodeCount || outputNode>circuit.nodeCount || result.points!=nullptr) return false;
    result.points = new FrequencyPoint[pointCount]{};
    result.count=pointCount;
    for (int i=0; i<pointCount; i++) {
        double t = static_cast<double>(i) / (pointCount-1);
        double frequency = startFrequency*std::pow(endFrequency / startFrequency, t);
        ACResult ac{};
        if(!analyzeAC(circuit, frequency, ac)) { deleteSweepResult(result); return false; }
        std::complex<double> vin=ac.solution[inputNode-1], vout=ac.solution[outputNode-1];
        if(std::abs(vin)<=1e-15) { deleteACResult(ac); deleteSweepResult(result); return false; }
        result.points[i]={frequency,vout/vin};
        deleteACResult(ac);
    }
    return true;
}

void deleteSweepResult(SweepResult& result) {
    delete[] result.points;
    result.points=nullptr;
    result.count=0;
}