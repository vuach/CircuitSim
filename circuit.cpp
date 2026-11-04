#include "circuit.h"
#include "solver.h"
#include <iostream>
#include <cmath>

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
    }
}

bool buildDC(const Circuit& circuit, Matrix& A, double*& b) {
    int n = circuit.nodeCount;
    if (n<=0 || circuit.capacity<1 || circuit.size<1 || circuit.components == nullptr || A.data!=nullptr || b != nullptr) return false;
    A = Mcreate(n, n);
    b = new double[n]{};
    for (int i=0; i<n; i++) {
        b[i]=0;
    }
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