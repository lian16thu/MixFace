#include "interpolation.h"
#include <iostream>
#include <string>
#include <cmath>
#include <opencv2/opencv.hpp> 
using namespace std;
using namespace cv;

void InterpolateNearest(double value_point[2],double value[3],cv::Mat img)
{
	if( img.empty() )
	{
		cout<<"the img is empty!"<<endl;
		return;
	}
	if(value_point[0]<0 || value_point[0]>img.rows || value_point[1]<0 || value_point[1]>img.cols)
	{
		value[0]=0;
		value[1]=0;
		value[2]=0;
		return;
	}
	double distance[4];
	int x=(int)value_point[0];
	int y=(int)value_point[1];

	distance[0]=pow(value_point[0]-x,2)+pow(value_point[1]-y,2);
	distance[1]=pow(value_point[0]-x-1,2)+pow(value_point[1]-y,2);
	distance[2]=pow(value_point[0]-x-1,2)+pow(value_point[1]-y-1,2);
	distance[3]=pow(value_point[0]-x,2)+pow(value_point[1]-y-1,2);
	int min_num=0;
	int min_distance=distance[0];
	for(int i=1;i<4;i++)
	{
		if(distance[i]<min_distance)
		{
			min_distance=distance[i];
			min_num=i;
		}
	}
	switch(min_num)
	{
	case 0:
		value[0] = (double)img.at<Vec3b>(x,y)[0];
		value[1] = (double)img.at<Vec3b>(x,y)[1];
		value[2] = (double)img.at<Vec3b>(x,y)[2];
		break;
	case 1:
		value[0] = (double)img.at<Vec3b>(x+1,y)[0];
		value[1] = (double)img.at<Vec3b>(x+1,y)[1];
		value[2] = (double)img.at<Vec3b>(x+1,y)[2];
		break;
	case 2:
		value[0] = (double)img.at<Vec3b>(x+1,y+1)[0];
		value[1] = (double)img.at<Vec3b>(x+1,y+1)[1];
		value[2] = (double)img.at<Vec3b>(x+1,y+1)[2];
		break;
	case 3:
		value[0] = (double)img.at<Vec3b>(x,y+1)[0];
		value[1] = (double)img.at<Vec3b>(x,y+1)[1];
		value[2] = (double)img.at<Vec3b>(x,y+1)[2];
		break;
	default:
		cout<<"interpolation errors!"<<endl;break;
	}

}

void InterpolateBilinear(double value_point[2],double value[3],cv::Mat img)
{
	if( img.empty() )
	{
		cout<<"the img is empty!"<<endl;
		return;
	}
	int x=(int)value_point[0];
	int y=(int)value_point[1];
	double u = value_point[0] - x;
	double v = value_point[1] - y;
	for(int i=0;i<3;i++)
	{
		value[i]=(1-u)*(1-v)*(double)img.at<Vec3b>(x,y)[i] + u*(1-v)*(double)img.at<Vec3b>(x+1,y)[i]
				+ v*(1-u)*(double)img.at<Vec3b>(x,y+1)[i] + u*v*(double)img.at<Vec3b>(x+1,y+1)[i];
	}
}

void InterpolateBicubic(double value_point[2],double value[3],cv::Mat img)
{
	if( img.empty() )
	{
		cout<<"the img is empty!"<<endl;
		return;
	}
	int x=(int)value_point[0];
	int y=(int)value_point[1];
	double u = value_point[0] - x;
	double v = value_point[1] - y;
	matrix A(1,4);
	matrix C(4,1);
	for(int i=1;i<5;i++)
	{
		A(1,i)=S_function(u+2-i);
		C(i,1)=S_function(v+2-i);
	}
	matrix B0(4,4);
	matrix B1(4,4);
	matrix B2(4,4);
	for(int i=1;i<5;i++)
		for(int j=1;j<5;j++)
		{
			B0(i,j)=img.at<Vec3b>(x-2+i,y-2+j)[0];
			B1(i,j)=img.at<Vec3b>(x-2+i,y-2+j)[1];
			B2(i,j)=img.at<Vec3b>(x-2+i,y-2+j)[2];
		}

	value[0]=(A*B0*C)(1,1);
	value[1]=(A*B1*C)(1,1);
	value[2]=(A*B2*C)(1,1);
}

double S_function(double x)
{
	double result;
	if(abs(x)<=1)
	{
		result = 1-2*x*x+abs(x)*abs(x)*abs(x);
	}
	else if(abs(x)>1&&abs(x)<2)
	{
		result = 4-8*abs(x)+5*x*x-abs(x)*abs(x)*abs(x);
	}
	else
	{
		result=0;
	}
	return result;
}