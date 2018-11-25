#include "bsplines.h"
#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <opencv2/opencv.hpp> 
using namespace std;
using namespace cv;

void test(double control_point[68][2])
{
	Mat img = imread("../picture/8.jpg");
	if( img.empty() )
		{
			cout<<"the img is empty!"<<endl;
			return;
		}
	cout<<"height="<<img.rows<<endl;
	cout<<"width="<<img.cols<<endl;


	for(int k=0;k<68;k++)
		for(int i=0;i<2;i++)
			for(int j=0;j<2;j++)
			{
				img.at<Vec3b>((int)control_point[k][0]+i,(int)control_point[k][1]+j)=0;
			}

	namedWindow("Example1",WINDOW_AUTOSIZE);
	imshow("Example1",img);
	waitKey(100000);
}
double W_function(double x,double y)
{
	return B_function(x)*B_function(y);
}


double B_function(double x)
{
	double result;
	double x_abs = abs(x);
	if (x_abs <= 1)
		result=(3*x_abs*x_abs*x_abs-6*x_abs*x_abs+4)/6;
	else if (x_abs <= 2)
		result=(-x_abs*x_abs*x_abs+6*x_abs*x_abs-12*x_abs+8)/6;
	else 
		result = 0;
	return result;
}




void B_Splines(double target_point[68][2], double control_point[68][2], double **newx, double **newy, int height, int width)
{
	cout<<"using B_Splines method to solve the problem"<<endl;
	double zc[68];
	double **phi_old = NULL,**phi_new;

	//if p=0 calculate for newx; if p=1 calculate for newy
	for(int xy = 0;xy < 2;xy++)
	{
		//initialize zc
		cout<<"initializing zc"<<endl;
		for(int i = 0;i < 68;i++)
			zc[i] = target_point[i][xy] - control_point[i][xy];
		for(int k = 0; ;k++)
		{
			cout<<endl;
			cout<<"turn "<<k<<endl;
			//calculate the size of the lattices
			int size = int(pow(2,k)) + 3;
			cout<<"the size of the lattices is "<<size<<endl;

			phi_new = new double *[size];
			for(int i = 0;i < size;i++)
				phi_new[i] = new double [size];

			//calculate wab
			cout<<"calculating wab"<<endl;
			double b_control_point[68][2];
			double wab[68];
			for(int i = 0;i < 68;i++)
			{
				b_control_point[i][0] = control_point[i][0]/((double)height/(size-3))+1;
				b_control_point[i][1] = control_point[i][1]/((double)width/(size-3))+1;
				wab[i] = 0;
				int int_x = (int)b_control_point[i][0];
				int int_y = (int)b_control_point[i][1];
				for(int s = int_x-1;s < int_x+3;s++)
					for(int t = int_y-1;t < int_y+3;t++)
						wab[i] += W_function(b_control_point[i][0]-s,b_control_point[i][1]-t)*W_function(b_control_point[i][0]-s,b_control_point[i][1]-t);
			}


			//calculate phi_new
			cout<<"calculating phi_new"<<endl;
			for(int i = 0; i < size;i++)
				for(int j = 0;j < size;j++)
				{
					double wc,phi_c;
					double delta = 0,omg = 0;
					for(int s = 0;s < 68;s++)
					{
						wc = W_function(b_control_point[s][0]-i,b_control_point[s][1]-j);
						phi_c = wc*zc[s]/wab[s];
						delta += wc*wc*phi_c;
						omg += wc*wc;
					}
					phi_new[i][j] = 0;
					if(omg != 0)
						phi_new[i][j] = delta/omg;
					else
						phi_new[i][j] = 0;
				}
			//update zc and calculate the error
			cout<<"updating zc and calculating error"<<endl;
			double error = 0;
			for(int i = 0;i < 68;i++)
			{
				int int_x = (int)b_control_point[i][0];
				int int_y = (int)b_control_point[i][1];
				for(int s = int_x-1;s < int_x+3;s++)
					for(int t = int_y-1;t < int_y+3;t++)
					{
						zc[i] -= W_function(b_control_point[i][0]-s,b_control_point[i][1]-t)*phi_new[s][t];

					}
				error += abs(zc[i]);

			}
			cout<<"the error is "<<error<<endl;

			//update phi_old
			cout<<"updating phi_old"<<endl;
			if(size == 4)
				phi_old = phi_new;
			else
			{
				double **phi_old_temp = new double *[size];
				for (int i = 0; i < size; i++)
					phi_old_temp[i] = new double[size];

				for(int i = 0;i < (size-3)/2+3;i++)
					for(int j = 0;j < (size-3)/2+3;j++)
					{
						if(2*i-1>=0 && 2*i-1<size && 2*j-1>=0 && 2*j-1<size)
							phi_old_temp[2*i-1][2*j-1] = (phi_old[i-1][j-1]+phi_old[i-1][j+1]+phi_old[i+1][j-1]+phi_old[i+1][j+1]+6*(phi_old[i-1][j]+phi_old[i][j-1]+phi_old[i][j+1]+phi_old[i+1][j])+36*phi_old[i][j])/64;
						if(2*i-1>=0 && 2*i-1<size && 2*j>=0 && 2*j<size)
							phi_old_temp[2*i-1][2*j] = (phi_old[i-1][j]+phi_old[i-1][j+1]+phi_old[i+1][j]+phi_old[i+1][j+1]+6*(phi_old[i][j]+phi_old[i][j+1]))/16;
						if(2*i>=0 && 2*i<size && 2*j-1>=0 && 2*j-1<size)
							phi_old_temp[2*i][2*j-1] = (phi_old[i][j-1]+phi_old[i][j+1]+phi_old[i+1][j-1]+phi_old[i+1][j+1]+6*(phi_old[i][j]+phi_old[i+1][j]))/16;
						if(2*i>=0 && 2*i<size && 2*j>=0 && 2*j<size)
							phi_old_temp[2*i][2*j] = (phi_old[i][j]+phi_old[i][j+1]+phi_old[i+1][j]+phi_old[i+1][j+1])/4;
					}

				for(int i = 0;i < size;i++)
					for(int j = 0;j < size;j++)
						phi_new[i][j] += phi_old_temp[i][j];
				phi_old = phi_new;
				for (int i = 0; i < size; i++)
					delete[] phi_old_temp[i];
				delete[] phi_old_temp;
			}	


			double threshold=0.1;

			if(error < threshold)
			{
				//calculate newx/newy
				if(xy==0)
					cout<<"calculating newx"<<endl;
				else
					cout<<"calculating newy"<<endl;
				for(int i = 0;i < height;i++)
					for(int j = 0;j < width;j++)
					{
						double u,v;
						u = i/((double)height/(size-3))+1;
						v = j/((double)width/(size-3))+1;
						if(xy == 0)
						{
							newx[i][j] = (double)i;
						}
						else
							newy[i][j] = (double)j;
						int int_u = (int)u;
						int int_v = (int)v;
						for(int s = int_u-1;s < int_u+3;s++)
							for(int t = int_v-1;t < int_v+3;t++)
								if(xy == 0)
									newx[i][j] += W_function(u-s,v-t)*phi_old[s][t];
								else
									newy[i][j] += W_function(u-s,v-t)*phi_old[s][t];
					}
				break;
			}
		}
	}
	cout<<"B_Splines finished"<<endl;
	
}


