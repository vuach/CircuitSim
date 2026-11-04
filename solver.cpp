#include "Solver.h"
#include <cmath>
#include <algorithm>


bool GaussForw(Matrix& augmented) {
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
        if (maxi==0.0) return false;
        Mswaprows(augmented, i, maxrow);
        for (int j=i+1; j<n; j++) {
            double k = augmented.data[j][i]/augmented.data[i][i];
            for (int t=i; t<augmented.cols; t++) {
                augmented.data[j][t]=augmented.data[j][t]-k*augmented.data[i][t];
            }
            augmented.data[j][i]=0.0;
        }
    }
    return true;
}

bool GaussBack(const Matrix& augmented, double* x) {
    int n = augmented.rows;
    for (int i=n-1; i>=0; i--) {
        double semisum = 0;
        for (int j=n-1; j>i; j--) {
            semisum = semisum+x[j]*augmented.data[i][j];
        }
        if (augmented.data[i][i]!=0.0) x[i] = (augmented.data[i][n]-semisum)/augmented.data[i][i];
        else return false;
    }
    return true;
}

bool GaussSolve(const Matrix& M, const double* b, double* x) {
    if (M.rows!=M.cols || M.rows<=0 || M.data == nullptr || b==nullptr || x==nullptr) return false;
    int n = M.rows;
    Matrix augmented = Mcreate(n, n+1);
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            augmented.data[i][j] = M.data[i][j];
        }
        augmented.data[i][n] = b[i];
    }
    if (!GaussForw(augmented)) {
        Mdelete(augmented);
        return false;
    }
    bool success = GaussBack(augmented, x);
    Mdelete(augmented);
    return success;
}