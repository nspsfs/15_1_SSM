#pragma once

#include <iostream>
#include <omp.h>
#include "opencv2\opencv.hpp"
#include "opencv2\highgui\highgui.hpp"

using namespace cv;
using namespace std;

class Fusion
{
	Mat img1;
	Mat img2;
	Mat img1G;
	Mat img2G;
	Mat sml1;
	Mat sml2;
	Mat result;

	IplImage temp;
	int i, j, k;

public:
	Fusion();
	~Fusion();
	void setimg(IplImage* image1, IplImage* image2);
	void sml(IplImage** dst);
	void mmsml(IplImage** dst);
	void segsml(IplImage** dst);
};