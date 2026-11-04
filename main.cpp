#include "matrix.h"
#include "solver.h"
#include <iostream>

int main() {
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