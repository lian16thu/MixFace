#ifndef BSPLINES_H
#define BSPLINES_H
#include "Matrix.h"
void test(double control_point[68][2]);
double W_function(double x,double y);
double B_function(double x);
void B_Splines(double oldpoint[68][2], double changed_newpoint[68][2], double **newx, double **newy, int row, int col);

#endif