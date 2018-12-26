// cv.cpp : 定义控制台应用程序的入口点。
//yuechi

#include <opencv2/opencv.hpp>  
#include<string>
#include<fstream>
using namespace cv;  
using namespace std;  


string s1="C:\\Users\\jin\\pictures\\eyebrow\\";
string s2=".jpg";
//ppt表示嘴巴的12个特征点
//img原彩色图
//m唇彩的颜色
Mat ChangeMouthcolor(Point ppt[],Mat img,Scalar m){
	const Point* pts[] = {ppt};//ppt类型为Point*，pts类型为Point**，需定义成const类型
	int npt[] ={9};  //npt的类型即为int*
	//填充多边形
	int x0=4096,y0=2048,x1=0,y1=0;
	for(int i=0;i<9;i++){
		Point p=ppt[i];
		if(x0>p.x)x0=p.x;
		if(y0>p.y)y0=p.y;
		if(y1<p.y)y1=p.y;
		if(x1<p.x)x1=p.x;
	}

	
	
	Mat mask(img.size(),CV_8U,Scalar(0));
	fillPoly(mask, pts, npt,1,Scalar(255)); //嘴唇蒙版	

	Mat image_roi(Size(x1-x0,y1-y0),CV_8UC3,m);
	Rect rect2(x0,y0,x1-x0,y1-y0);	
	mask=mask(rect2);	
	Point center((int)(x0+x1)/2,(int)(y0+y1)/2);

	Mat output=img;		
	cvtColor(output, output, CV_BGR2BGRA);

	int p2=y0,p1=x0;
	for(int i=0;i<mask.rows;i++)
		for(int j=0;j<mask.cols;j++){
			if((int)mask.at<uchar>(i,j)==255){//唇彩颜色和嘴唇透明度叠加
			double alpha=0.3;
			output.at<Vec4b>(p2+i,p1+j)[0]=int(m[0]*alpha+output.at<Vec4b>(p2+i,p1+j)[0]*(1-alpha));
			output.at<Vec4b>(p2+i,p1+j)[1]=int(m[1]*alpha+output.at<Vec4b>(p2+i,p1+j)[1]*(1-alpha));
			output.at<Vec4b>(p2+i,p1+j)[2]=int(m[2]*alpha+output.at<Vec4b>(p2+i,p1+j)[2]*(1-alpha));
		
	}
}
		Mat mask2(img.size(),CV_8U,Scalar(0));

		polylines(mask2,pts,npt,1,1,Scalar(255),10);
	    Mat output1;
		medianBlur(output, output1,5);
	
	    for(int x=x0;x<x1;x++)
		for(int y=y0;y<y1;y++)
		{  if((int)mask2.at<uchar>(y,x)==255){//对嘴唇的边缘进行模糊处理
		
			output.at<Vec4b>(y,x)=output1.at<Vec4b>(y,x);
		}	
		}
	return output;
}







int main()  
{ 

	Mat  img = imread(s1+"left.jpg",1);//读取原彩色图  

	//读取嘴唇特征点.
	ifstream fin(s1+"mouth2.txt",ios::in);
	Point ppt[12];
	float a, b;char c;
	for(int i=0;i<12;i++){
		fin>>c>>a>>c>>b>>c>>c;
		ppt[i]=Point((int)a,(int)b);
		

	}
		
		Scalar m=Scalar(177,156,242);//选定唇彩颜色
		Mat output=ChangeMouthcolor(ppt,img,m);
		
		imwrite(s1+"mouth2.png",output);
	    waitKey();
	

}  