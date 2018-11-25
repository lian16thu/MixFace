#ifndef INTERPOLATION_H
#define INTERPOLATION_H
#include <opencv2/opencv.hpp> 
#include "Matrix.h"
void InterpolateNearest(double value_point[2],double value[3],cv::Mat img);
void InterpolateBilinear(double value_point[2],double value[3],cv::Mat img);
void InterpolateBicubic(double value_point[2],double value[3],cv::Mat img);
double S_function(double x);
#endif