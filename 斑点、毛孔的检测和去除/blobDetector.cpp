#include"blobDetector.h"

//返回检测区域的外接矩形
//poi表示所有uv特征点集合
//contourIndex检测区域的特征点索引集合
//length 检测区域轮廓上特征点个数
//mask 返回检测区域的蒙版
Rect BlobDetector::detectArea(vector<Point> poi, int * contourIndex, int length,Mat &mask)
{
	Point contour[13];
	for (int p_index = 0; p_index < length; p_index++)
	{
		contour[p_index]=poi[contourIndex[p_index]];
	}
	
	const Point* pts[] = {contour };//ppt类型为Point*，pts类型为Point**，需定义成const类型
		int npt[] = { length };  //npt的类型即为int*
		//填充多边形
		
	fillPoly(mask, pts, npt, 1, Scalar(255));//检测区域蒙版
	polylines(mask, pts, npt, 1, 1, Scalar(120), 1);//绘制检测区域的轮廓
	int left = contour[0].x;
	int top = contour[0].y;
	int right = contour[0].x;
	int down = contour[0].y;

	for (int i = 1; i < length; i++)
	{
		int now_x = contour[i].x;
		int now_y = contour[i].y;

		left = left > now_x ? now_x : left;
		top = top > now_y ? now_y : top;
		right = right < now_x ? now_x : right;
		down = down < now_y ? now_y : down;

	}

	Rect roiRect = Rect(left, top, right - left, down - top);
	return roiRect;//返回检测区域的外包矩形
}
//画出斑点的轮廓
void BlobDetector::drawBlob(Mat& src, vector<vector<Point>> contours)//绘制斑点的轮廓
{
	for (int i = 0; i < contours.size(); i++)
	{
		drawContours(src, contours, i, Scalar(255),1,8);
	}
}

vector<vector<Point>> BlobDetector::contourDetect(Mat & src)//检测斑点的轮廓
{
	vector<vector<Point>> contours;
	vector<Vec4i>hierarchy;
	findContours(src, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point());	
	vector<vector<Point>> in_contours;

	return contours;
	
	
}
//src 原检测区域彩色图
//result 黑白二值图，白的代表斑点
void StainDetector::imageProcess(Mat & src, Mat& result)
{
	Mat image_gaussian;
	Mat image_equalize;
	Mat image_gray;

	cvtColor(src, image_gray, CV_BGR2GRAY);
	
	int blockSize = 49;
	int constValue =10;
	adaptiveThreshold(image_gray, result, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
	return;
}
//removeStain去除斑点函数
//src表示原图
//poi表示原图的所有特征点集合
//image_roi skin2.jpg的皮肤蒙版
void StainDetector::removeStain(Mat & src, vector<Point> poi,Mat image_roi)
{
	
	for (int c_index = 0; c_index < FACE_NUM; c_index++)//这里去除斑点的区域就是检测斑点的那个区域
	{
		Mat mask(src.size(), CV_8U, Scalar(0));
		Rect roiRect = this->detectArea(poi, this->faceContour[c_index], FACE_CONTOUR, mask);
		Mat roi(src, roiRect);
		Mat roi_mask(mask, roiRect);
		Mat result;
		this->imageProcess(roi, result);
		result.setTo(255, roi_mask != 0 & result != 0);
		result.setTo(0, roi_mask == 0 | result == 0);
		vector<vector<Point>> contours = this->contourDetect(result);
		contours = this->contourFilter(contours);//首先检测到斑点，并把斑点的轮廓集合装在contours里
		int x0 = 4096, x1 = 0, y0 = 2048, y1 = 0;
		for (int i = 0; i < contours.size(); i++) {//对每一个轮廓里的斑点去除
			x0 = 4096; x1 = 0; y0 = 2048; y1 = 0;
			for (int j = 0; j < contours[i].size(); j++) {
				Point p = contours[i][j];
				if (x0 > p.x)x0 = p.x;
				if (y0 > p.y)y0 = p.y;
				if (y1 < p.y)y1 = p.y;
				if (x1 < p.x)x1 = p.x;
			}


			Vec3b up, down; int flag = 1, u, d;

			for (int x = x0; x <= x1; x++) {//首先对称，把外面的皮肤折进来

				u = y0; d = y1;
				for (int y = 0; y <= 1 * (d - u) / 5; y++) {

					roi.at<Vec3b>(d - y, x) = roi.at<Vec3b>(d + y, x);



				}
				for (int y = 0; y <= 1 * (d - u) / 5; y++) {
					roi.at<Vec3b>(u + y, x) = roi.at<Vec3b>(u - y, x);
				}
				int u1 = u + (d - u) / 5;
				for (int y = 0; y <= 1 * (d - u) / 5; y++) {
					roi.at<Vec3b>(u1 + y, x) = roi.at<Vec3b>(u1 - y, x);
				}



				int s = (d - u) / 5;
				int l = (d - u) / 5 + (d - u) / 5;
				int m = (d - u) / 5 + (d - u) / 5 + 3;
				for (int y = 0; y <= (d - u) / 5 + (d - u) / 5 + 3; y++) {//中间区域采用线性过渡

					roi.at<Vec3b>(y + u + l, x)[0] = (1.0 - 1.0*y / m)*roi.at<Vec3b>(u + l - y, x)[0] + 1.0*y / m * roi.at<Vec3b>(d - s + m - y, x)[0];
					roi.at<Vec3b>(y + u + l, x)[1] = (1.0 - 1.0*y / m)*roi.at<Vec3b>(u + l - y, x)[1] + 1.0*y / m * roi.at<Vec3b>(d - s + m - y, x)[1];
					roi.at<Vec3b>(y + u + l, x)[2] = (1.0 - 1.0*y / m)*roi.at<Vec3b>(u + l - y, x)[2] + 1.0*y / m * roi.at<Vec3b>(d - s + m - y, x)[2];

				}			}
		}
		
		resize(image_roi, image_roi, result.size());//将皮肤蒙版改成需要的尺寸
		Point center((int)(result.cols / 2), (int)(result.rows / 2));
		seamlessClone(image_roi, roi, result, center, roi, NORMAL_CLONE);//皮肤蒙版和原来斑点区域进行泊松融合
		roi.setTo(Scalar(0, 255, 0), roi_mask == 120);//用绿色圈出检测斑点的区域
		roi.copyTo(src(roiRect));//修改后的区域拷贝到原图上

	}

	
}
//StainDetector::spotDetect检测色斑的函数
//src表示原图
//poi表示原图的所有特征点集合
//ratio 斑点区域占所有皮肤的比重
void StainDetector::spotDetect(Mat & src, vector<Point> poi, float& ratio)
{

	double spotArea = 0.0;
	double faceArea = 0.0;
	for (int c_index = 0; c_index < FACE_NUM; c_index++)
	{
		Mat mask(src.size(), CV_8U, Scalar(0));//检测区域蒙版
		Rect roiRect = this->detectArea(poi, this->faceContour[c_index], FACE_CONTOUR,mask);//检测区域的外接矩形
		Mat roi(src, roiRect);//裁剪区域
		Mat roi_mask(mask, roiRect);//裁剪蒙版
		Mat result;	
		this->imageProcess(roi, result);//result中白色的点是斑点
		
		result.setTo(255, roi_mask != 0&result!=0);//与蒙版交的区域
		result.setTo(0, roi_mask == 0 | result == 0);
		
		vector<vector<Point>> contours = this->contourDetect(result);
		contours = this->contourFilter(contours);
		this->drawBlob(roi,contours);
		
     	spotArea += this->areaCalculate(contours);
		faceArea += roi.cols*roi.rows;
		roi.setTo(Scalar(0,255,0), roi_mask ==120);//绘制检测区域为绿色边
		roi.copyTo(src(roiRect));

	}

	ratio = spotArea / faceArea;//斑点占检测的矩形的比例
}
//累加斑点的面积
double StainDetector::areaCalculate(vector<vector<Point>> contours)
{
	double totalArea = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		totalArea += area;
		
	}

	return totalArea;
}
//src 原检测区域的彩色图
//result 黑白二值图 白的代表毛孔

void PoreDetector::imageProcess(Mat& src, Mat& result)
{
	Mat image_gary;
	cvtColor(src, image_gary, CV_BGR2GRAY);
	int blockSize = 5;
	int constValue = 5;
	adaptiveThreshold(image_gary, result, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
	Mat labels, stats, centroids;
	int nccomps=connectedComponentsWithStats(result, labels, stats, centroids);
	//毛孔检测里面去掉面积大于20的连通域
	vector<int>colors(nccomps+1);
	for (int i = 1; i < nccomps; i++) {
		
		if (stats.at<int>(i, cv::CC_STAT_AREA) > 20)
			colors[i]=0;
		else colors[i] = 255;
	}
	
	for (int y = 0; y < result.rows; y++)
		for (int x = 0; x < result.cols; x++)
		{
			int label = labels.at<int>(y, x);
			CV_Assert(0 <= label && label <= nccomps);
			result.at<uchar>(y, x) = colors[label];
		}
	
	return;

}
//PoreDetector::spotDetect检测毛孔的函数
//src表示原图
//poi表示原图的所有特征点集合
//avgDia 毛孔的数量

void PoreDetector::spotDetect(Mat & src, vector<Point> poi, float& avgDia)
{
	int spotNum = 0;
	double totalDia = 0.0;

	Mat mask(src.size(), CV_8U, Scalar(0));
	Rect roiRect = this->detectArea(poi, this->noseContour, NOSE_CONTOUR,mask);
	Mat roi(src, roiRect);
	
	Mat roi_mask(mask, roiRect);
	Mat result;
	this->imageProcess(roi, result);
	roi.setTo(Scalar(255, 0, 0), result == 255 & roi_mask == 255);//取检测出的毛孔区域和蒙版的交集
	roi.setTo(Scalar(0, 255, 0), roi_mask == 120);//描绘出毛孔检测区域的轮廓
	for(int x=0;x<roi.cols;x++)
		for (int y = 0; y < roi.rows; y++) {
			if (roi.at<Vec3b>(y, x)[0] ==255&& roi.at<Vec3b>(y, x)[1] == 0&& roi.at<Vec3b>(y, x)[2] == 0)
				spotNum++;
		}
	avgDia = spotNum ;
	roi.copyTo(src(roiRect));
}

//斑点检测的时候过滤掉小的毛孔
vector<vector<Point>> StainDetector::contourFilter(vector<vector<Point>> contours, double filterArea)
{
	vector<vector<Point>> filterContours;

	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);

		if (area > filterArea)
		{
			filterContours.push_back(contours[i]);
		}
	}

	return filterContours;
}