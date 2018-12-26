#pragma once
#include<opencv2/opencv.hpp>

#define FACE_NUM 2//检测斑点的区域有左右两百年
#define FACE_CONTOUR 5//每个检测斑点的区域由5个点围成的区域
#define NOSE_NUM 1
#define NOSE_CONTOUR 11//检测毛孔的区域


using namespace std;
using namespace cv;


/*
	the base class to detect the blob on the face
*/
class BlobDetector {
public:
	BlobDetector() {};
	virtual ~BlobDetector() {};

	virtual void spotDetect(Mat& src, vector<Point> poi, float&) = 0;

protected:

	virtual void imageProcess(Mat& src, Mat& result) = 0;
	/*
		automatically calcuate the thread value that we used to binarize the source image
		@param src the input image
	*/

	/*
		get the bounding box of the detecting area
		@param poi the feature points on the face
		@param contourIndex the detecting area contours' index list in the poi
		@param length the length of the input contourIndex
	*/
	
	Rect detectArea(vector<Point> poi, int* contourIndex, int length, Mat &mask);
	/*
		according to the contours we found, we draw the blob on the input image
		@param src the input image
		@param contours the contours we found
	*/
	void drawBlob(Mat& src, vector <vector<Point>> contours);

	/*
		according the contourType,we detect the contours we are interested in
		@param src the input image
		@param contourType the type of the contour we interested in 
	*/
	vector<vector<Point>> contourDetect(Mat& src);

protected:
	/*
		the feature points' indexs in the uvs of the faces
	*/
	int faceContour[FACE_NUM][5] = {
		53,55,109,69,27,
		52,54,107,68,26
	};
};

/*
	the class to detect the stain on the face
*/
class StainDetector :public BlobDetector {
public:
	StainDetector() {};
	~StainDetector() {};
	

	/*
	detect the stain on the faces,
	this function will draw the contours detected on the source image,the input image.
	And get the ratio of the stain's area
	@param src the input image
	@param poi the feature points on the face
	@param ratio the ratio of the total stain's area
	@param contourType the contour's type we are insterested in
	*/
	void spotDetect(Mat& src, vector<Point> poi, float& ratio);
	void removeStain(Mat& src, vector<Point> poi,Mat skin);
protected:
	/*
	processing the image before we recognise the stain on the face.
	Firstly, convert the input image from rgb to gray.
	Secondly, use the OPEN operator to reduce the nosie.
	Thirdly, use the equalize operator to enhance the contrast.
	Then use the gaussian blur to reduce the nosie.
	Finally, use the ostu to find the threshold value and binarize the input image.
	@param src the input image
	@param result the result of the processing
	*/
	void imageProcess(Mat& src, Mat& reslut);

private:

	/*
	calculate the area of the stain we found
	@param contours the contours we found
	*/
	double areaCalculate(vector<vector<Point>> contours);
	vector<vector<Point>> contourFilter(vector<vector<Point>> contours, double filterArea =50.0);
private:
	float _ratio;
};

/*
the class to detect the pore on the face
*/
class PoreDetector :public BlobDetector {
public:
	PoreDetector() {};
	~PoreDetector() {};
	
	/*
	detect the pore on the faces,
	this function will draw the contours detected on the source image,the input image.
	And get the average diameter of the pores.
	@param src the input image
	@param poi the feature points on the face
	@param avgDia the average diameter of the pore we found on the face
	@param contourType the contour's type we are insterested in
	*/
	void spotDetect(Mat& src, vector<Point> poi, float& avgDia);

protected:
	/*
	processing the image before we recognise the pore on the face.
	histogram equalize the input image immediately,
	and binarize it with the threshold value found by the ostu.
	@param src the input image
	@param result the result of the processing
	*/
	void imageProcess(Mat& src, Mat& result);


	

private:
	/*
	the feature points' indexs in the uvs of the faces.
	*/
	int noseContour[11] = { 19,64,70,108,28,22,29,110,71,65,19};

	float _avgDia;
	int* histogram;
};
