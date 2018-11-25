#include "tps.h"
#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <opencv2/opencv.hpp> 
using namespace std;
using namespace cv;

//n = 68
void TPS(double target_point[68][2], double control_point[68][2], double **newx, double **newy, int height, int width)
{	
	cout<<"using TPS method to solve the problem"<<endl;
	double var_a[3][2];
	double var_w[68][2];
	double int_point[2];
	double value_point[2];
	//calculate var_w and var_a
	cout<<"begin to calculate var_a & var_w"<<endl;
	SolveVar(control_point,target_point,var_w,var_a);

	//calculate newx & newy
	cout<<"begin to calculate newx & newy"<<endl;
	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
		{
			int_point[0]=i;
			int_point[1]=j;
			// calculate the corresponding double point in original picture
			TPS_PointTransfer(int_point,value_point,control_point,var_w,var_a);
			newx[i][j]=value_point[0];
			newy[i][j]=value_point[1];
		}
}

void SolveVar(double control_point[68][2], double target_point[68][2], double var_w[68][2],double var_a[3][2])
{


	matrix L(71,71);
	cout<<"calculating matrix L"<<endl;
	for(int i=1;i<72;i++)
		for(int j=1;j<72;j++)
		{
			if(i<69&&j<69)
			{
				L(i,j)=function_U(control_point[i-1],control_point[j-1]);
			}
			else if(i>68&&j>68)
			{
				L(i,j)=0;
			}
			else if(i>68&&j<69)
			{
				if(i==69)
				{
					L(i,j)=1;
				}
				else if(i==70)
				{
					L(i,j)=control_point[j-1][0];
				}
				else if(i==71)
				{
					L(i,j)=control_point[j-1][1];
				}
				else
				{
					cout<<"Error1 in creating Matrix L"<<endl;
					return;
				}
			}
			else if(i<69&&j>68)
			{
				if(j==69)
				{
					L(i,j)=1;
				}
				else if(j==70)
				{
					L(i,j)=control_point[i-1][0];
				}
				else if(j==71)
				{
					L(i,j)=control_point[i-1][1];
				}
				else
				{
					cout<<"Error1 in creating Matrix L"<<endl;
					return;
				}
			}
			else
			{
				cout<<"Error2 in creating Matrix L"<<endl;
				return;
			}
		}

	cout<<"calculating matrix Y"<<endl;
	matrix Y(71,2);
	for(int i=1;i<72;i++)
	{
		if(i<69)
		{
			Y(i,1)=target_point[i-1][0];
			Y(i,2)=target_point[i-1][1];
		}
		else
		{
			Y(i,1)=0;
			Y(i,2)=0;
		}
	}
	matrix solution(71,2);
	cout<<"begin to solve"<<endl;
	solution=L.Inv()*Y;
	cout<<"calculating var_w & var_a"<<endl;
	for(int i=1;i<72;i++)
	{
		if(i<69)
		{
			var_w[i-1][0]=solution(i,1);
			var_w[i-1][1]=solution(i,2);
		}
		else if(i>68&&i<72)
		{
			var_a[i-69][0]=solution(i,1);
			var_a[i-69][1]=solution(i,2);
		}
		else
		{
			cout<<"convert errors"<<endl;
		}
	}

}

double function_U(double point_a[2],double point_b[2])
{
	//cout<<point_a[0]<<" "<<point_a[1]<<endl;
	double result;
	if(point_a[0]==point_b[0] && point_a[1]==point_b[1])
	{
		result=0;
	}
	else
	{
		double distance = (point_a[0]-point_b[0])*(point_a[0]-point_b[0]) + (point_a[1]-point_b[1])*(point_a[1]-point_b[1]);
		//pow(point_a[0]-point_b[0],2) + pow(point_a[1]-point_b[1],2);
		result = distance * log10(distance);
	}
	return result;
}


void TPS_PointTransfer(double int_point[2],double value_point[2],double control_point[68][2],double var_w[68][2],double var_a[3][2])
{
	double sum[2];
	sum[0]=0;
	sum[1]=0;
	for(int i=0;i<68;i++)
	{	
		sum[0] += var_w[i][0] * function_U(int_point,control_point[i]);
		sum[1] += var_w[i][1] * function_U(int_point,control_point[i]);	
	}

	value_point[0]=var_a[0][0]+var_a[1][0]*int_point[0]+var_a[2][0]*int_point[1]+sum[0];
	value_point[1]=var_a[0][1]+var_a[1][1]*int_point[0]+var_a[2][1]*int_point[1]+sum[1];
}

void FaceScale(double control_point[68][2],double target_point[68][2])
{
	double control_center[2];
	control_center[0]=0;
	control_center[1]=0;
	for(int i=0;i<68;i++)
	{
		control_center[0]+=control_point[i][0];
		control_center[1]+=control_point[i][1];
	}
	control_center[0]/=68;
	control_center[1]/=68;
	double control_distance;
	control_distance = sqrt(pow(control_point[0][0]-control_point[16][0],2)+pow(control_point[0][1]-control_point[16][1],2));


	double target_center[2];
	target_center[0]=0;
	target_center[1]=0;
	for(int i=0;i<68;i++)
	{
		target_center[0]+=target_point[i][0];
		target_center[1]+=target_point[i][1];
	}
	target_center[0]/=68;
	target_center[1]/=68;
	double target_distance;
	target_distance = sqrt(pow(target_point[0][0]-target_point[16][0],2)+pow(target_point[0][1]-target_point[16][1],2));

	double scale = control_distance / target_distance;



	for(int i=0;i<68;i++)
	{
		target_point[i][0] += control_center[0]-target_center[0];
		target_point[i][1] += control_center[1]-target_center[1];
	}

	for(int i=0;i<68;i++)
	{
		target_point[i][0] = scale*(target_point[i][0]-control_center[0])+control_center[0];
		target_point[i][1] = scale*(target_point[i][1]-control_center[1])+control_center[1];
	}
}
