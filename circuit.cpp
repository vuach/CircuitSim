#include "circuit.h"
#include "solver.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Circuit Ccreate(int initcap) {
    Circuit C;
    if (initcap<1) initcap = 4;
    C.components = new Component[initcap];
    C.capacity = initcap;
    C.size = 0;
    C.nodeCount = 0;
    return C;
}

void Cdelete(Circuit& circuit) {
    delete[] circuit.components;
    circuit.components = nullptr;
    circuit.size = 0;
    circuit.nodeCount = 0;
    circuit.capacity = 0;
}

bool Cadd(Circuit& circuit, const Component& component) {
    if (circuit.capacity <= circuit.size) {
        int newcapacity = std::max(4, circuit.capacity*2);
        Component* newcomponents = new Component[newcapacity];
        for (int i=0; i<circuit.size; i++) {
            newcomponents[i]=circuit.components[i];
        }
        delete[] circuit.components;
        circuit.components = newcomponents;
        circuit.capacity = newcapacity;
    }
    circuit.components[circuit.size] = component;
    circuit.size = circuit.size+1;
    int maxnode = std::max(component.node1, component.node2);
    if (maxnode>circuit.nodeCount) {
        circuit.nodeCount = maxnode;
    }
    return true;
}

void Cprint(Circuit& circuit) {
    std::cout << "Components: " << std::endl;
    for (int i=0; i<circuit.size; i++) {
        switch (circuit.components[i].type) {
            case ComponentType::Resistor:
                std::cout << "Resistor" << " " << circuit.components[i].node1 << " " << circuit.components[i].node2 << " " << circuit.components[i].value;
                break;
            case ComponentType::CurrentSource:
                std::cout << "Current source" << " " << circuit.components[i].node1 << " " << circuit.components[i].node2 << " " << circuit.components[i].value;
                break;
            default:
                std::cout << "Unknown component";
                break;
        }
        std::cout << std::endl;
    }
}

bool buildDC(const Circuit& circuit, Matrix& A, double*& b) {
    int n = circuit.nodeCount;
    int Vsrccount = 0;
    int Vsrcindex = 0;
    if (n<=0 || circuit.capacity<1 || circuit.size<1 || circuit.components == nullptr || A.data!=nullptr || b != nullptr) return false;
    for (int i=0; i<circuit.size; i++) if (circuit.components[i].type == ComponentType::VoltageSource) Vsrccount+=1;
    int N=n+Vsrccount;
    A = Mcreate(N, N);
    b = new double[N]{};
    for (int i=0; i<circuit.size; i++) {
        const Component& component = circuit.components[i];
        int node1 = component.node1;
        int node2 = component.node2;
        if (node1<0 || node2<0 || node1>n || node2>n || !std::isfinite(component.value)) {
            Mdelete(A);
            delete[] b;
            b = nullptr;
            return false;
        }
        switch (component.type) {
            case ComponentType::Resistor: {
                if (component.value<=0) {
                    Mdelete(A);
                    delete[] b;
                    b = nullptr;
                    return false;
                }
                double g = 1 / component.value;
                if (node1>0) {
                    int r1 = node1-1;
                    A.data[r1][r1]+=g;
                }
                if (node2>0) {
                    int r2 = node2-1;
                    A.data[r2][r2]+=g;
                }
                if (node1>0 && node2>0) {
                    int r1 = node1-1;
                    int r2 = node2-1;
                    A.data[r1][r2]-=g;
                    A.data[r2][r1]-=g;
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
                Mdelete(A);
                delete[] b;
                b = nullptr;
                return false;
            }
        }
    }
    return true;
}


bool calcComponent(const Component& component, 
    const double* solution, int nodeCount, int voltageSrcIndex, double& voltage,
    double& current, double& power) {
    if (component.node1>nodeCount || component.node1<0 || component.node2>nodeCount || component.node2<0 || solution==nullptr || voltageSrcIndex<0) return false;
    int k=0;
    double V1=0;
    double V2=0;
    if (component.node1!=0) V1=solution[component.node1-1];
    if (component.node2!=0) V2=solution[component.node2-1];
    voltage = V1-V2;
    switch (component.type){
    case ComponentType::Resistor: {
        current = voltage/component.value;
        break;
    }
    case ComponentType::CurrentSource: {
        current = component.value;
        break;
    }
    case ComponentType::VoltageSource: {
        k = nodeCount+voltageSrcIndex;
        current = solution[k];
        break;
    }
    default: {
        return false;
    }
    }
    power = current*voltage;
    return true;
}

bool analyzeDC(const Circuit& circuit, DCResult& result) {
    int componentCount = circuit.size;
    Matrix A{};
    double* b = nullptr;
    bool build = buildDC(circuit, A, b);
    if (build==false){
        Mdelete(A);
        delete[] b;
        return false;
    }
    result.solution = new double[A.rows]{};
    result.solutionSize = A.rows;
    bool Solved = GaussSolve(A, b, result.solution);
    if (Solved==false){
        Mdelete(A);
        delete[] b;
        deleteDCResult(result);
        return false;
    }
    double Residual = residual(A, b, result.solution);
    result.residual = Residual;
    int voltageSrcIndex = 0;
    result.components = new ComponentResult[componentCount]{};
    result.componentCount = circuit.size;
    for (int i=0; i<componentCount; i++) {
        const Component& curr = circuit.components[i];
        bool Calculated = calcComponent(curr, result.solution, circuit.nodeCount, voltageSrcIndex, result.components[i].voltage, result.components[i].current, result.components[i].power); 
        if (curr.type==ComponentType::VoltageSource) {
            voltageSrcIndex+=1;
        }
        if (Calculated==false) {
            Mdelete(A);
            delete[] b;
            deleteDCResult(result);
            return false;
        }
    }
    Mdelete(A);
    delete[] b;
    return true;
}

void deleteDCResult(DCResult& result) {
    delete[] result.components;
    delete[] result.solution;
    result.components=nullptr;
    result.solution = nullptr;
    result.componentCount=0;
    result.solutionSize=0;
    result.residual=0;
}