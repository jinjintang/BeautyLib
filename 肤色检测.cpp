// cv.cpp : 定义控制台应用程序的入口点。
//yuechi

#include <opencv2/opencv.hpp>  
#include<string>
#include<fstream>
using namespace cv;
using namespace std;
string s1 = "C:/Users/jin/Documents/wpic/head/";
//读取所有点的uv坐标
vector<Point> setPoint(string uvsPath)
{
	ifstream infile;
	infile.open(uvsPath);

	string s;
	getline(infile, s);

	vector<Point> poi;
	string coordinate;
	float x = 0;
	float y = 0;
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == '(')
		{
			coordinate.clear();
			continue;
		}
		if (s[i] == ',')
		{
			x = atof(coordinate.c_str());
			continue;
		}
		if (s[i] == ' ')
		{
			coordinate.clear();
			continue;
		}
		if (s[i] == ')')
		{
			y = atof(coordinate.c_str());

			Point point = Point((int)(x * 4096), (int)(y * 2048));
			poi.push_back(point);
			continue;
		}
		coordinate.push_back(s[i]);
	}

	return poi;


}
string DetectSkinColor(Mat img, vector<Point> poi,Scalar s[][11]) {
	int faceContour[20] = { 73,113,112,111,54,52,13,11,9,3,0,4,10,12,14,53,55,114,115,116 };//检测皮肤的区域
	Point contour[20];
	for (int p_index = 0; p_index < 20; p_index++)
	{
		contour[p_index] = poi[faceContour[p_index]];
	}

	const Point* pts[] = { contour };//ppt类型为Point*，pts类型为Point**，需定义成const类型
	int npt[] = { 20 };  //npt的类型即为int*
	//填充多边形
	Mat mask(img.size(), CV_8U, Scalar(0));
	fillPoly(mask, pts, npt, 1, Scalar(255));
	Mat img_gray;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	int blockSize = 49;
	int constValue = 10;
	Mat result;
	adaptiveThreshold(img_gray, result, 120, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
	//肤色计算平均值时要去掉五官
	mask.setTo(0, result == 120);

	int left = contour[0].x;
	int top = contour[0].y;
	int right = contour[0].x;
	int down = contour[0].y;

	for (int i = 1; i < 5; i++)
	{
		int now_x = contour[i].x;
		int now_y = contour[i].y;

		left = left > now_x ? now_x : left;
		top = top > now_y ? now_y : top;
		right = right < now_x ? now_x : right;
		down = down < now_y ? now_y : down;

	}
	Scalar color = (0, 0, 0);
	int n = 0;
	for (int y = top; y <= down; y++)
		for (int x = left; x <= right; x++) {
			if ((int)mask.at<uchar>(y, x) == 255) {
				color[0] = color[0] + img.at<Vec3b>(y, x)[0];
				color[1] = color[1] + img.at<Vec3b>(y, x)[1];
				color[2] = color[2] + img.at<Vec3b>(y, x)[2];
				n++;
			}
		}


	for (int i = 0; i < 3; i++)
		color[i] = (int)(1.0*color[i] / n);//计算肤色RGB的平均值
	Scalar sk = s[0][0];
	int xc = 0, yc = 0;
	int l = 255 * 255 * 3;
	for (int y = 0; y < 6; y++)
		for (int x = 0; x < 11; x++) {
			int dis = 0;
			for (int j = 0; j < 3; j++) {
				dis += (s[y][x][j] - color[j])*(s[y][x][j] - color[j]);//判断RGB离哪个肤色最近
			}
			if (dis < l) {
				l = dis;
				sk = s[y][x];
				xc = x;
				yc = y;
			}
		}
	string str = "";//str表示横向编号
	if (xc + 1 < 9)str = char(xc + 1) + '0';
	else str = "1"+char((xc+1-10)+'0');
	return  char(yc + 'A') + str ;//返回肤色编号

}











int main()
{

	Mat  img = imread("C:/Users/jin/Pictures/skincolor.png", 1);//读取肤色卡 
	Scalar s[6][11];//肤色数组
	for(int y=0;y<6;y++)
	for (int x = 0; x < 11; x++) {
		s[y][x]=img.at<Vec3b>(35+50*y, 90+40*x);	
	}
	for (int i = 0; i < 6; i++) {
		img = imread(s1 + char(i + '1') + ".jpg", 1);//读取原彩色图  
		vector<Point> poi = setPoint(s1 + char('1' + i) + ".txt");//读取坐标点
		cout << DetectSkinColor(img, poi, s) << endl;//检测肤色
	}
		waitKey();


}