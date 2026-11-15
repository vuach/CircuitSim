#include "complexsolver.h"
#include <cmath>
#include <algorithm>
#include <complex>


bool CGaussForw(ComplexMatrix& augmented, double eps) {
    int n = augmented.rows;
    for (int i=0; i<n; i++) {
        int maxrow = i;
        double maxi = std::abs(augmented.data[i][i]);

        for (int j=i+1; j<n; j++) {
            if (std::abs(augmented.data[j][i])>maxi) {
                maxi = std::abs(augmented.data[j][i]);
                maxrow = j;
            }
        }
        if (maxi<eps) return false;
        CMswaprows(augmented, i, maxrow);
        for (int j=i+1; j<n; j++) {
            std::complex<double> k = augmented.data[j][i]/augmented.data[i][i];
            for (int t=i+1; t<augmented.cols; t++) {
                augmented.data[j][t]=augmented.data[j][t]-k*augmented.data[i][t];
            }
            augmented.data[j][i]=0.0;
        }
    }
    return true;
}

bool CGaussBack(const ComplexMatrix& augmented, std::complex<double>* x, double eps) {
    int n = augmented.rows;
    for (int i=n-1; i>=0; i--) {
        std::complex<double> semisum = 0;
        for (int j=n-1; j>i; j--) {
            semisum = semisum+x[j]*augmented.data[i][j];
        }
        if (std::abs(augmented.data[i][i])>eps) x[i] = (augmented.data[i][n]-semisum)/augmented.data[i][i];
        else return false;
    }
    return true;
}

bool CGaussSolve(const ComplexMatrix& M, const std::complex<double>* b, std::complex<double>* x) {
    if (M.rows!=M.cols || M.rows<=0 || M.data == nullptr || b==nullptr || x==nullptr) return false;
    int n = M.rows;
    ComplexMatrix augmented = CMcreate(n, n+1);
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            augmented.data[i][j] = M.data[i][j];
        }
        augmented.data[i][n] = b[i];
    }
    if (!CGaussForw(augmented)) {
        CMdelete(augmented);
        return false;
    }
    bool success = CGaussBack(augmented, x);
    CMdelete(augmented);
    return success;
}

double Cresidual(const ComplexMatrix& M, const std::complex<double>* b, const std::complex<double>* x) {
    double maxr = 0;
    for (int i=0; i<M.rows; i++) {
        std::complex<double> semisum = 0;
        for (int j=0; j<M.cols; j++) {
            semisum = semisum + M.data[i][j]*x[j];
        }
        maxr = std::max(maxr, std::abs(semisum-b[i]));
    }
    return maxr;
}

