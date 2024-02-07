#include <stdio.h>
#include <math.h>
#include "glfem.h"


double integrate(double x[3], double y[3], double (*f) (double, double)) {
    double I = 0;
    double xLoc[3];
    double yLoc[3];

    double xi[3] = {1.0/6, 1.0/6, 2.0/3};
    double eta[3] = {1.0/6, 2.0/3, 1.0/6};
    double omega[3] = {1.0/6, 1.0/6, 1.0/6};

    for (int k = 0; k < 3; k++) {
        xLoc[k] = x[0]*(1-xi[k]-eta[k]) + x[1]*xi[k] + x[2]*eta[k];
        yLoc[k] = y[0]*(1-xi[k]-eta[k]) + y[1]*xi[k] + y[2]*eta[k];

        I += omega[k] * f(xLoc[k], yLoc[k]);
    }

    glfemSetColor(GLFEM_BLACK); glfemDrawElement(x,y,3);
    glfemSetColor(GLFEM_BLUE);  glfemDrawNodes(x,y,3);
    glfemSetColor(GLFEM_RED);   glfemDrawNodes(xLoc,yLoc,3);
    
    return I;
}

double integrateRecursive(double x[3], double y[3], double (*f)(double,double), int n) {
    if (n == 0) return integrate(x, y, f);

    double middleX[3]; double middleY[3];

    for (int i = 0; i < 3; i++) {
        middleX[i] = (x[i]+x[(i+1)%3])/2;
        middleY[i] = (y[i]+y[(i+1)%3])/2;
    }

    double I = integrateRecursive(middleX, middleY, f, n-1);

    for (int i = 0; i < 3; i++) {
        double locX[3] = {x[i], middleX[i], middleX[(i-1)%3 < 0 ? 3+(i-1) : (i-1)%3]};
        double locY[3] = {y[i], middleY[i], middleY[(i-1)%3 < 0 ? 3+(i-1) : (i-1)%3]};

        I += integrateRecursive(locX, locY, f, n-1);
    }

    return I;
}
