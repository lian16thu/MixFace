#include "src/hello.h"
#include "src/tps.h"
#include "src/interpolation.h"
#include "src/Matrix.h"
#include "src/bsplines.h"
#include <iostream>
#include <opencv2/opencv.hpp> 
#include <fstream>
#include <string>
#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing/render_face_detections.h"
#include "dlib/image_processing.h"
#include "dlib/gui_widgets.h"
#include "dlib/image_io.h"

using namespace dlib;
using namespace std;
using namespace cv;
Mat Clip(Mat original_img);
void landmark_detect(string img_path,string file_path);

int main( int argc, char **argv)
{
	//HelloFunc();

	// input the basic info of the problem
	string target_num;
	cout<<"please input target_num, which means you will transform this picture"<<endl;
	while(1)
	{
		cin>>target_num;
		if(target_num>"0"&&target_num<":"&&target_num.length()==1)
		{
			break;
		}
		else
		{
			cout<<"no picture found, please choose again"<<endl;
			cout<<"you can choose 1 ~ 9"<<endl;
		}
	}

	string control_num;
	cout<<"please input control_num, which means you will use the structure of this picture"<<endl;
	while(1)
	{
		cin>>control_num;
		if(control_num>"0"&&control_num<":"&&control_num.length()==1)
		{
			break;
		}
		else
		{
			cout<<"no picture found, please choose again"<<endl;
			cout<<"you can choose 1 ~ 9"<<endl;
		}
	}

	string method_name;
	cout<<"please input the method to mix the faces"<<endl;
	cout<<"you can choose TPS or B_Splines"<<endl;
	while(1)
	{
		cin>>method_name;
		if(method_name=="TPS"||method_name=="B_Splines")
		{
			break;
		}
		else
		{
			cout<<"no method found, please choose again"<<endl;
			cout<<"you can choose TPS or B_Splines"<<endl;
		}
	}

	string interpotation_name;
	cout<<"please input the interpotation method"<<endl;
	cout<<"you can choose nearest, bilinear or bicubic"<<endl;
	while(1)
	{
		cin>>interpotation_name;
		if(interpotation_name=="nearest"||interpotation_name=="bilinear"||interpotation_name=="bicubic")
		{
			break;
		}
		else
		{
			cout<<"no method found, please choose again"<<endl;
			cout<<"you can choose nearest, bilinear or bicubic"<<endl;
		}
	}
	
	//string target_num="6";
	//string control_num="8";
	//string method_name="B_Splines";
	string control_file_path="../picture/"+control_num+".txt";
	string control_img_path="../picture/"+control_num+".jpg";
	string target_file_path="../picture/"+target_num+".txt";
	string target_img_path="../picture/"+target_num+".jpg";

	landmark_detect(control_img_path,control_file_path);
	landmark_detect(target_img_path,target_file_path);


	// input control info
	cout<<"opening control_file from "<<control_file_path<<endl;
	FILE *control_file;
	control_file=fopen(control_file_path.c_str(),"rt+");
	double control_point[68][2];
	for(int i=0;i<68;i++)
	{
		
		fscanf(control_file,"%lf",&control_point[i][1]);
		fscanf(control_file,"%lf",&control_point[i][0]);
	}
	fclose(control_file);
	cout<<"opening control_img from "<<control_img_path<<endl;
	Mat img_control = imread(control_img_path.c_str());
	
	if( img_control.empty() )
		{
			cout<<"control img is empty!"<<endl;
			return 0;
		}


	// input target info
	cout<<"opening target_file from "<<target_file_path<<endl;
	FILE *target_file;
	target_file=fopen(target_file_path.c_str(),"rt+");
	double target_point[68][2];
	for(int i=0;i<68;i++)
	{
		
		fscanf(target_file,"%lf",&target_point[i][1]);
		fscanf(target_file,"%lf",&target_point[i][0]);
	}
	fclose(target_file);
	cout<<"opening target_img from "<<target_img_path<<endl;
	Mat img = imread(target_img_path.c_str());
	
	if( img.empty() )
		{
			cout<<"target img is empty!"<<endl;
			return 0;
		}

	
	// regulate the control_point with the target_point
	double control_point_regulate[68][2];
	for(int i=0;i<68;i++)
	{
		control_point_regulate[i][0] = control_point[i][0];
		control_point_regulate[i][1] = control_point[i][1];
	}

	FaceScale(target_point,control_point_regulate);


	// using TPS / B_Splines method to calculate the coresponding coordinates of each point in converted pictures
	int width=img.cols;
	int height=img.rows;
	double **newx = new double*[height];
	for (int i = 0; i < height; i++)
		newx[i] = new double[width];
	double **newy = new double*[height];
	for (int i = 0; i < height; i++)
		newy[i] = new double[width];

	if(method_name=="B_Splines")
	{
		//using B_Splines method
		B_Splines(target_point,control_point_regulate, newx, newy,height,width);
	}
	else
	{
		// using TPS method
		TPS(target_point,control_point_regulate, newx, newy,height,width);
	}


	// create the converted picture
	cout<<"begin to create the converted picture"<<endl;
	Mat img_convert(img.rows,img.cols,CV_8UC3,Scalar::all(1));
	double value_point[2];
	double value[3];
	for(int i=0;i<img.rows;i++)
		for(int j=0;j<img.cols;j++)
		{
			// calculate the corresponding double point in original picture
			value_point[0]=newx[i][j];
			value_point[1]=newy[i][j];
			// calculate the value of the double point
			if(interpotation_name=="nearest")
				InterpolateNearest(value_point,value,img);
			else if(interpotation_name=="bicubic")
				InterpolateBicubic(value_point,value,img);
			else
				InterpolateBilinear(value_point,value,img);

			// build the converted picture
			img_convert.at<Vec3b>(i,j)[0]=value[0];
			img_convert.at<Vec3b>(i,j)[1]=value[1];
			img_convert.at<Vec3b>(i,j)[2]=value[2];
		}
	
	
	// mark the control points in pictures
	for(int i=0;i<68;i++)
	{
		for(int m=-1;m<1;m++)
			for(int n=-1;n<1;n++)
				img.at<Vec3b>((int)target_point[i][0]+n,(int)target_point[i][1]+m)=0;

	}
	for(int i=0;i<68;i++)
	{
		for(int m=-1;m<1;m++)
			for(int n=-1;n<1;n++)
				img_control.at<Vec3b>((int)control_point[i][0]+n,(int)control_point[i][1]+m)=255;

	}

	// clip the converted picture
	Mat img_new=Clip(img_convert);


	// display the pictures
	cout<<"display the original picture"<<endl;
	imshow("Original",img);
	cout<<"display the controling picture"<<endl;
	imshow("Controling",img_control);
	cout<<"display the mixed picture"<<endl;
	imshow("Mixed",img_convert);
	cout<<"display the clipped picture"<<endl;
	imshow("Clipped",img_new);
	waitKey();
	return 0;
}

Mat Clip(Mat img_convert)
{
	int width=img_convert.cols;
	int height=img_convert.rows;
	int up,down,left,right;
	up=1;
	down=height-2;
	left=1;
	right=width-2;
	double threshold=0.5;

	//calculating the border
	cout<<"begin to clip"<<endl;
	for(int i=0;i<height;i++)
	{
		int num=0;
		for(int j=0;j<width;j++)
		{
			double add=img_convert.at<Vec3b>(i,j)[0]+img_convert.at<Vec3b>(i,j)[1]+img_convert.at<Vec3b>(i,j)[2];
			if(add==0)
			{
				num++;
			}
		}
		if(num>(int)(width*threshold))
		{
			up=i;
		}
		else
		{
			break;
		}
	}
	cout<<"up = "<<up<<endl;

	for(int i=height-1;i>-1;i--)
	{
		int num=0;
		for(int j=0;j<width;j++)
		{
			double add=img_convert.at<Vec3b>(i,j)[0]+img_convert.at<Vec3b>(i,j)[1]+img_convert.at<Vec3b>(i,j)[2];
			if(add==0)
			{
				num++;
			}
		}
		if(num>(int)(width*threshold))
		{
			down=i;
		}
		else
		{
			break;
		}
	}
	cout<<"down = "<<down<<endl;

	for(int j=0;j<width;j++)
	{
		int num=0;
		for(int i=0;i<height;i++)
		{
			double add=img_convert.at<Vec3b>(i,j)[0]+img_convert.at<Vec3b>(i,j)[1]+img_convert.at<Vec3b>(i,j)[2];
			if(add==0)
			{
				num++;
			}
		}
		if(num>(int)(height*threshold))
		{
			left=j;
		}
		else
		{
			break;
		}
	}
	cout<<"left = "<<left<<endl;


	for(int j=width-1;j>-1;j--)
	{
		int num=0;
		for(int i=0;i<height;i++)
		{
			double add=img_convert.at<Vec3b>(i,j)[0]+img_convert.at<Vec3b>(i,j)[1]+img_convert.at<Vec3b>(i,j)[2];
			if(add==0)
			{
				num++;
			}
		}
		if(num>(int)(height*threshold))
		{
			right=j;
		}
		else
		{
			break;
		}
	}
	cout<<"right = "<<right<<endl;

	// draw the border
	for(int i=1;i<height-1;i++)
	{
		for(int m=-1;m<1;m++)
			for(int n=-1;n<1;n++)
			{
				img_convert.at<Vec3b>(i+n,left+m)=255;
				img_convert.at<Vec3b>(i+n,right+m)=255;
			}
	}
	for(int j=1;j<width-1;j++)
	{
		for(int m=-1;m<1;m++)
			for(int n=-1;n<1;n++)
			{
				img_convert.at<Vec3b>(up+n,j+m)=255;
				img_convert.at<Vec3b>(down+n,j+m)=255;
			}
	}

	// create a new clipped picture
	int new_height=down - up-2;
	int new_width = right -left-2;
	Mat new_img(new_height,new_width,CV_8UC3,Scalar::all(1));
	for(int i=0;i<new_height;i++)
		for(int j=0;j<new_width;j++)
		{
			new_img.at<Vec3b>(i,j)=img_convert.at<Vec3b>(i+up+1,j+left+1);
		}

	cout<<"clip finished"<<endl;
	return new_img;
}


void landmark_detect(string img_path,string file_path)
{  
try
    {
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor sp;
        deserialize("../shape_predictor_68_face_landmarks.dat") >> sp;


        image_window win, win_faces;
     

            cout << "processing image " << img_path << endl;
            array2d<rgb_pixel> img;
            load_image(img, img_path);
          
            std::vector<dlib::rectangle> dets = detector(img);
            cout << "Number of faces detected: " << dets.size() << endl;
            std::vector<full_object_detection> shapes;
            if(dets.size()!=1)
            {
            	cout<<"detecting no or more than one face!!!"<<endl;
            	return;
            }
            for (unsigned long j = 0; j < dets.size(); ++j)
            {
                full_object_detection shape = sp(img, dets[j]);
                
                ofstream fout;
				fout.open(file_path);
                if (fout) { // 如果创建成功
                for (int i = 0; i < shape.num_parts(); i++)
                {
                    fout <<shape.part(i).x()<<" "<<shape.part(i).y()<< endl; // 使用与cout同样的方式进行写入

                }
  
                fout.close();  // 执行完操作后关闭文件句柄
                }

                shapes.push_back(shape);
            }
            cout<<"detectiong finished"<<endl;
        }
    
    catch (exception& e)
    {
        cout << "\nexception thrown!" << endl;
        cout << e.what() << endl;
    }

}
// ----------------------------------------------------------------------------------------

