#ifndef TPS_H
#define TPS_H

void SolveVar(double control_point[68][2], double target_point[68][2], double var_w[68][2],double var_a[3][2]);
double function_U(double point_a[2],double point_b[2]);
void FaceScale(double control_point[68][2],double target_point[68][2]);
void TPS_PointTransfer(double int_point[2],double value_point[2],double control_point[68][2],double var_w[68][2],double var_a[3][2]);
void TPS(double target_point[68][2], double control_point[68][2], double **newx, double **newy, int height, int width);
#endif
