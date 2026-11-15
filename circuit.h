#pragma once
#include "matrix.h"

enum class ComponentType {
    Resistor,
    CurrentSource
};

struct Component {
    ComponentType type;
    int node1;
    int node2;
    double value;
};

struct Circuit {
    Component* components;
    int size;
    int capacity;
    int nodeCount;
};


Circuit Ccreate(int initialCapacity);
void Cdelete(Circuit& circuit);
bool Cadd(Circuit& circuit, const Component& component);
void Cprint(Circuit& circuit);
bool buildDC(const Circuit& circuit, Matrix& A, double*& b);