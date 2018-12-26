// cv.cpp : 定义控制台应用程序的入口点。


#include <opencv2/opencv.hpp>  
#include<string>
#include<fstream>
using namespace cv;  
using namespace std;  


string s1="C:\\Users\\jin\\pictures\\eyebrow\\";
string s2=".jpg";
//RemoveEyebrow擦除眉毛的函数
//ppt 眉毛的9个特征点
//img整张图
//f=0左眉;f=1右眉，以我的角度
//image_roi skin.jpg那张图
//x0,y0,x1,y1分别表示眉毛的外接矩形的最左边，最右边，最上，最下
Mat RemoveEyebrow(Point ppt[],Mat img,int f,Mat image_roi,int &x0,int &y0,int &x1,int &y1){
	const Point* pts[] = {ppt};//ppt类型为Point*，pts类型为Point**，需定义成const类型
	int npt[] ={9};  //npt的类型即为int*
	//填充多边形
	
	for(int i=0;i<9;i++){
		Point p=ppt[i];
		if(x0>p.x)x0=p.x;
		if(y0>p.y)y0=p.y;
		if(y1<p.y)y1=p.y;
		if(x1<p.x)x1=p.x;
	}

	
	
	Mat mask(img.size(),CV_8U,Scalar(0));
	fillPoly(mask, pts, npt,1,Scalar(255)); 
	Vec3b up,down;int flag=1,u,d;
	for(int x=x0+1;x<x1;x++){

		for(int y=y0;y<=y1;y++)
		{  if((int)mask.at<uchar>(y,x)==255){
			if(flag){

				up=img.at<Vec3b>(y,x);
				flag=0;
				u=y;

			}}
		else if(!flag){
			down=img.at<Vec3b>(y-1,x);
			flag=1;
			d=y-1;




		}	}
		for(int y=0;y<=1*(d-u)/5;y++){

			img.at<Vec3b>(d-y,x)=img.at<Vec3b>(d+y,x);



		}
		for(int y=0;y<=1*(d-u)/5;y++){
			img.at<Vec3b>(u+y,x)=img.at<Vec3b>(u-y,x);
		}
		int u1=u+(d-u)/5;
		for(int y=0;y<=1*(d-u)/5;y++){
			img.at<Vec3b>(u1+y,x)=img.at<Vec3b>(u1-y,x);
		}



		int s=(d-u)/5;
		int l=(d-u)/5+(d-u)/5;
		int m=(d-u)/5+(d-u)/5+3;
		for(int y=0;y<=(d-u)/5+(d-u)/5+3;y++){

			img.at<Vec3b>(y+u+l,x)[0]=(1.0-1.0*y/m)*img.at<Vec3b>(u+l-y,x)[0]+1.0*y/m*img.at<Vec3b>(d-s+m-y,x)[0];
			img.at<Vec3b>(y+u+l,x)[1]=(1.0-1.0*y/m)*img.at<Vec3b>(u+l-y,x)[1]+1.0*y/m*img.at<Vec3b>(d-s+m-y,x)[1];
			img.at<Vec3b>(y+u+l,x)[2]=(1.0-1.0*y/m)*img.at<Vec3b>(u+l-y,x)[2]+1.0*y/m*img.at<Vec3b>(d-s+m-y,x)[2];

		}


	}
	
	Rect rt(0,0,x1-x0,y1-y0);
	
	image_roi=image_roi(rt);
	Rect rect2(x0,y0,x1-x0,y1-y0);
	if(f==1){
	for(int x=x0;x<(x1+x0)/2;x++){
		for(int y=y0;y<y1;y++){
			if((int)mask.at<uchar>(y,x)==0){
			mask.at<uchar>(y,x)=255;
			}
			else break;
		}
	}}
	else{
	for(int x=x1;x>=(x1+x0)/2;x--){
		for(int y=y0;y<y1;y++){
			if((int)mask.at<uchar>(y,x)==0){
			mask.at<uchar>(y,x)=255;
			}
			else break;
		}
	}
	
	
	
	}
	mask=mask(rect2);


	
	Point center((int)(x0+x1)/2,(int)(y0+y1)/2);

	// Seamlessly clone src into dst and put the results in output
	Mat output;
	seamlessClone(image_roi, img, mask, center, output, NORMAL_CLONE);
    Mat mask2(img.size(),CV_8U,Scalar(0));
	Mat output1;
		medianBlur(output, output1,5);
	
	for(int x=x0;x<x1;x++)
		for(int y=y0;y<y1;y++)
		{  if((int)mask.at<uchar>(y-y0,x-x0)==255){
		
			output.at<Vec3b>(y,x)=output1.at<Vec3b>(y,x);
		}
		
		}
		
		
	

	

		return output;
		



}


//ChangeEyebrow 贴上假眉毛函数
//output 擦除了眉毛的整张图
//f=0左眉;f=1右眉，以我的角度
//eyebrow 假眉毛矩阵
//p1,p2 假眉毛外接矩形的左上角贴在原图的（p1,p2）位置。
//s表示眉毛外接矩形的长度，控制眉毛的大小
//angle控制眉毛的角度，用15度之类的表示
Mat ChangeEyebrow(Mat output,int f,Mat eyebrow,int p1,int p2,int s,double angle){
	
	if(f) flip(  eyebrow,eyebrow,1);
	resize(eyebrow, eyebrow, Size(s,int(1.0*eyebrow.rows/eyebrow.cols*(s))));
    cvtColor(output, output, CV_BGR2BGRA);
	Mat dst;
   int top,bottom,left,right;
   top=bottom=eyebrow.cols/2;
   left=right=eyebrow.rows/2;
   copyMakeBorder(eyebrow, eyebrow, eyebrow.cols/2, eyebrow.cols/2, eyebrow.rows/2, eyebrow.rows/2, 0);

	Size src_sz = eyebrow.size();
	Size dst_sz(eyebrow.cols,eyebrow.rows);
	int len = std::max(eyebrow.cols, eyebrow.rows);
 
	//指定旋转中心
	cv::Point2f center(eyebrow.cols/2,eyebrow.rows/2);
   
	
	//获取旋转矩阵（2x3矩阵）
	cv::Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);
 
	//根据旋转矩阵进行仿射变换
	cv::warpAffine(eyebrow,eyebrow, rot_mat, dst_sz);

	for(int i= 0;i<eyebrow.rows;i++)
		for(int j=0;j<eyebrow.cols;j++){
			if(eyebrow.at<Vec4b>(i,j)[3]!=0){
			double alpha=1.0*eyebrow.at<Vec4b>(i,j)[3]/255;
			int x=p1+j-left;
			int y=p2+i-top;
			output.at<Vec4b>(y,x)[0]=int(eyebrow.at<Vec4b>(i,j)[0]*alpha+output.at<Vec4b>(y,x)[0]*(1-alpha));
			output.at<Vec4b>(y,x)[1]=int(eyebrow.at<Vec4b>(i,j)[1]*alpha+output.at<Vec4b>(y,x)[1]*(1-alpha));
			output.at<Vec4b>(y,x)[2]=int(eyebrow.at<Vec4b>(i,j)[2]*alpha+output.at<Vec4b>(y,x)[2]*(1-alpha));
		
	}
		}

		return output;
		



}



int main()  
{ 

	Mat  img = imread(s1+"left.jpg",1);//读取原彩色图  

	//读取眉毛特征点.
	ifstream fin(s1+"test3.txt",ios::in);
	Point ppt[9];
	float a, b;char c;
	for(int i=0;i<9;i++){
		fin>>c>>a>>c>>b>>c>>c;
		ppt[i]=Point((int)a,(int)b);
		

	}
		for(int i=0;i<11;i++){
			cout<<i<<endl;
		int a=(i>=9?'A'-9:'1');
		Mat eyebrow=imread(s1+"data\\ZHF\\"+char(i+a)+"\\"+char(i+a)+".png",-1);
		Mat  image_roi=imread(s1+"skin2.jpg");
		int x0=4096,y0=2048,x1=0,y1=0;
		Mat remove=RemoveEyebrow(ppt,img,1,image_roi,x0,y0,x1,y1);
		imwrite(s1+"myfdfdr"+char(i+'1')+".png",remove);
	    waitKey();
		int p1=x0,p2=y0,s=x1-x0;
		Mat output;
		output=ChangeEyebrow(remove,1,eyebrow,p1,p2,s,0);
		imwrite(s1+"myr"+char(i+'1')+".png",output);
		output=ChangeEyebrow(remove,0,eyebrow,p1,p2,s,15);
		imwrite(s1+"myrr"+char(i+'1')+".png",output);
		}
	    waitKey();

}  