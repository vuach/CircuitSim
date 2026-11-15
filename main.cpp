#include "matrix.h"
#include "solver.h"
#include "circuit.h"
#include <iostream>

/*int main() {

    int cols;
    int rows;
    std::cout << "Cols: Rows: " << std::endl;
    std::cin >> cols >> rows;
    Matrix A = Mcreate(cols, rows);
    double data = 0;
    std::cout << "Data: " << std::endl;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            std::cin >> data;
            A.data[i][j] = data;
        }
    }
    double eq = 0;
    double* b = new double[rows];
    std::cout << "eq " << std::endl;
    for (int i=0; i<rows; i++) {
        std::cin >> eq;
        b[i] = eq;
    }
    double* x = new double[rows];
    if (GaussSolve(A, b, x)) {
        std::cout << "x0 = " << x[0] << "\n";
        std::cout << "x1 = " << x[1] << "\n";
        std::cout << "Residual = "
                  << residual(A, b, x) << "\n";
    }
    else {
        std::cout << "No unique solution\n";
    }

    Mdelete(A);
}
*/
int main() {
    Circuit circuit = Ccreate(2);
    Cadd(circuit, {ComponentType::Resistor, 1, 2, 1000.0});
    Cadd(circuit, {ComponentType::Resistor, 2, 0, 2000.0});
    Cadd(circuit, {ComponentType::CurrentSource, 0, 1, 0.003});
    Cprint(circuit);
    DCResult result{};
    bool made = analyzeDC(circuit, result);
    if (made==false) std::cout << "compute failed\n";
    else {
        for (int i=0; i<result.solutionSize; i++) {
            std::cout << result.solution[i] << " ";
        }
        std::cout << std::endl;
        for (int i=0; i<result.componentCount; i++) {
            std::cout << "I = " << result.components[i].current
          << " A, U = " << result.components[i].voltage
          << " V, P = " << result.components[i].power
          << " W\n";
        }
        std::cout << std::endl << result.residual << std::endl;
    }
    deleteDCResult(result);
    Cdelete(circuit);
    /*Matrix A{};
    double* b = nullptr;
    if (!buildDC(circuit, A, b)) {
        std::cout << "buildDC failed\n";
        Cdelete(circuit);
        return 1;
    }
    std::cout << "\nMatrix A:\n";
    Mprint(A);
    std::cout << "\nVector b:\n";
    for (int i = 0; i < A.rows; i++) std::cout << b[i] << "\n";

    double* x = new double[A.rows]{};
    if (GaussSolve(A, b, x)) {
        std::cout << "\nNode voltages:\n";
        for (int i = 0; i < A.rows; i++) std::cout << "V" << i + 1 << " = " << x[i] << " V\n";
        std::cout << "\nResidual = " << residual(A, b, x) << "\n";
    }
    else std::cout << "GaussSolve failed\n";

    delete[] x;
    delete[] b;
    Mdelete(A);
    Cdelete(circuit);*/
    return 0;
}