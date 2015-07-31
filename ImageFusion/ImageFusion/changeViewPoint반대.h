#pragma once

#include <opencv2\core\core.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\contrib\contrib.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv2\legacy\legacy.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
/* image setting 방법
iplImage를 클래스의 이미지에 셋팅할경우.
class.imgL=cvarrToMat(iplImage);

외부 이미지를 셋팅할경우.
class.imgL=imread("imagePath.jpg");

반대로 출력되는 이미지를  iplimage로 변환할 경우
iplimage = convertImg;
--------------------------------------------------------
IplImage *oldC0 = cvCreateimage(cvSize(320,240),16,1); //cvReleaseImage(oldC0);
Mat newC = cvarrToMat(oldC0);//IplImage -> Mat
IplIamge oldC1 = newC;//Mat -> IplImage
CvMat oldC2 = newC;//Mat -> CvMat
*/
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////



class CVP{
	Mat disparity;
	Mat imgL;
	Mat copyImgL;
	Mat imgR;
	Mat dispG;
	Mat imgLG;
	Mat imgRG;
	Mat cvtR2L;
	Mat cvtR2LC;
	IplImage temp;
	Mat convertImg;

public:
	

	CVP();
	~CVP();

	void runcvt(IplImage** dst);
	void setimg(IplImage* dis, IplImage* left, IplImage* right);
};


