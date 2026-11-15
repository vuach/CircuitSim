#include "circuit.h"
#include "complexsolver.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <complex>
#include "ac.h"

constexpr double pi = 3.14159265358979323846;

bool buildAC(const Circuit& circuit, double frequency, ComplexMatrix& A, std::complex<double>*& b) {
    int n = circuit.nodeCount;
    int Vsrccount = 0;
    int Vsrcindex = 0;
    double omega = 2.0 * pi * frequency;
    if (n<=0 || circuit.capacity<1 || circuit.size<1 || circuit.components == nullptr || A.data!=nullptr || b != nullptr || !std::isfinite(frequency) || frequency <= 0) return false;
    for (int i=0; i<circuit.size; i++) if (circuit.components[i].type == ComponentType::VoltageSource) Vsrccount+=1;
    int N=n+Vsrccount;
    A = CMcreate(N, N);
    b = new std::complex<double>[N]{};
    for (int i=0; i<circuit.size; i++) {
        const Component& component = circuit.components[i];
        int node1 = component.node1;
        int node2 = component.node2;
        if (node1<0 || node2<0 || node1>n || node2>n || !std::isfinite(component.value)) {
            CMdelete(A);
            delete[] b;
            b = nullptr;
            return false;
        }
        switch (component.type) {
            case ComponentType::Resistor:
            case ComponentType::Capacitor:
            case ComponentType::Inductor: {
                if (component.value<=0) {
                    CMdelete(A);
                    delete[] b;
                    b = nullptr;
                    return false;
                }
                std::complex<double> Y;
                if (component.type==ComponentType::Capacitor) Y = std::complex<double>(0.0, omega*component.value);
                if (component.type==ComponentType::Resistor) Y = 1 / component.value;
                if (component.type==ComponentType::Inductor) Y = std::complex<double>(0.0, -1.0 / (omega*component.value));
                if (node1>0) {
                    int r1 = node1-1;
                    A.data[r1][r1]+=Y;
                }
                if (node2>0) {
                    int r2 = node2-1;
                    A.data[r2][r2]+=Y;
                }
                if (node1>0 && node2>0) {
                    int r1 = node1-1;
                    int r2 = node2-1;
                    A.data[r1][r2]-=Y;
                    A.data[r2][r1]-=Y;
                }
                break;
            }
            case ComponentType::CurrentSource: {
                if (node1 > 0) b[node1 - 1] -= component.value;
                if (node2 > 0) b[node2 - 1] += component.value;
                break;
            }
            case ComponentType::VoltageSource: {
                int k=n+Vsrcindex;
                if(node1>0){
                    A.data[node1-1][k]+=1;
                    A.data[k][node1-1]+=1;
                }
                if (node2>0){
                    A.data[node2-1][k]-=1;
                    A.data[k][node2-1]-=1;
                }
                b[k] = component.value;
                Vsrcindex++;
                break;
            }
            default: {
                CMdelete(A);
                delete[] b;
                b = nullptr;
                return false;
            }
        }
    }
    return true;
}

bool calcACComponent(const Component& component, const std::complex<double>* solution, int nodeCount, int voltageSrcIndex, double frequency, std::complex<double>& voltage, std::complex<double>& current, std::complex<double>& power) {
    if (component.node1>nodeCount || component.node1<0 || component.node2>nodeCount || component.node2<0 || solution==nullptr || voltageSrcIndex<0 || !std::isfinite(frequency) || frequency<=0) return false;
    std::complex<double> V1 = 0.0;
    std::complex<double> V2 = 0.0;
    if (component.node1 > 0) V1 = solution[component.node1 - 1];
    if (component.node2 > 0) V2 = solution[component.node2 - 1];
    voltage = V1-V2;
    double omega = 2.0 * pi * frequency;
    if (!std::isfinite(component.value)) return false;
    if (component.type == ComponentType::Resistor ||
        component.type == ComponentType::Capacitor ||
        component.type == ComponentType::Inductor) {
        if (component.value <= 0) return false;
    }
    switch (component.type){
    case ComponentType::Resistor: {
        current = voltage/component.value;
        break;
    }
    case ComponentType::CurrentSource: {
        current = component.value;
        break;
    }
    case ComponentType::Capacitor: {
        std::complex<double> Y(0.0, omega*component.value);
        current = Y*voltage;
        break;
    }
    case ComponentType::Inductor: {
        std::complex<double> Y(0.0, -1.0 / (omega*component.value));
        current = Y*voltage;
        break;
    }
    case ComponentType::VoltageSource: {
        int k=0;
        k = nodeCount+voltageSrcIndex;
        current = solution[k];
        break;
    }
    default: {
        return false;
    }
    }
    power = 0.5 * voltage * std::conj(current);
    return true;
}

bool analyzeAC(const Circuit& circuit, double frequency, ACResult& result) {
    int componentCount = circuit.size;
    ComplexMatrix A{};
    std::complex<double>* b = nullptr;
    bool build = buildAC(circuit, frequency, A, b);
    if (build==false){
        CMdelete(A);
        delete[] b;
        return false;
    }
    result.solution = new std::complex<double>[A.rows]{};
    result.solutionSize = A.rows;
    result.frequency = frequency;
    bool Solved = CGaussSolve(A, b, result.solution);
    if (Solved==false){
        CMdelete(A);
        delete[] b;
        deleteACResult(result);
        return false;
    }
    double Residual = Cresidual(A, b, result.solution);
    result.residual = Residual;
    int voltageSrcIndex = 0;
    result.components = new ACComponentResult[componentCount]{};
    result.componentCount = circuit.size;
    for (int i=0; i<componentCount; i++) {
        const Component& curr = circuit.components[i];
        bool Calculated = calcACComponent(curr, result.solution, circuit.nodeCount, voltageSrcIndex, frequency, result.components[i].voltage, result.components[i].current, result.components[i].power); 
        if (curr.type==ComponentType::VoltageSource) {
            voltageSrcIndex+=1;
        }
        if (Calculated==false) {
            CMdelete(A);
            delete[] b;
            deleteACResult(result);
            return false;
        }
    }
    CMdelete(A);
    delete[] b;
    return true;
}

void deleteACResult(ACResult& result) {
    delete[] result.components;
    delete[] result.solution;
    result.components=nullptr;
    result.solution = nullptr;
    result.componentCount=0;
    result.solutionSize=0;
    result.residual=0;
}