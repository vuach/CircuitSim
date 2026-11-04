#include "circuit.h"
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

