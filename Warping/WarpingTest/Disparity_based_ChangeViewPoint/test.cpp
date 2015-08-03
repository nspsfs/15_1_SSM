#include <iostream>
#include <chrono>
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include <omp.h>

using namespace std;

int main()
{
	IplImage* disparity = cvLoadImage("image/test_disparity.jpg");
	IplImage* imgR = cvLoadImage("image/test_R.jpg");
	IplImage* imgL = cvLoadImage("image/test_L.jpg");
	//IplImage* disparity = cvLoadImage("image/disp.png");
	//IplImage* imgR = cvLoadImage("image/right.png");
	//IplImage* imgL = cvLoadImage("image/left.png");
	CvSize size = cvGetSize(imgR);

	int pixelDiff = 16;

	IplImage* copyImgR = cvCloneImage(imgR);
	IplImage* disparityGray = cvCreateImage(size,IPL_DEPTH_8U,1);
	IplImage* imgRGray = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* imgLGray = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvCvtColor(imgR, imgRGray, CV_RGB2GRAY);
	cvCvtColor(imgL, imgLGray, CV_RGB2GRAY);
	cvCvtColor(disparity, disparityGray, CV_RGB2GRAY);

	IplImage* cvtR2LGray = cvCloneImage(imgLGray);
	//IplImage* cvtR2L = cvCloneImage(imgL);
	IplImage* cvtR2L = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvSetZero(cvtR2L);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < size.height; i++)
	{
		uchar* pDispG = (uchar*)(disparityGray->imageData + i*disparityGray->widthStep);
		uchar* prtCvtR2L = (uchar*)(cvtR2L->imageData + i*cvtR2L->widthStep);
		uchar* prtCopyImgR = (uchar*)(copyImgR->imageData + i*copyImgR->widthStep);
		for (int j = 0; j < size.width; j++)
		{
			//int tempVal = j + 1;// pDispG[j] / pixelDiff - j + 1;
			//tempVal -= pDispG[j] / pixelDiff;
			int tempVal = j + (pDispG[j] / pixelDiff);
			//if (tempVal < size.width)
			if (tempVal <size.width)
			{
				prtCvtR2L[3 * j] = prtCopyImgR[3 * tempVal];
				prtCvtR2L[3 * j + 1] = prtCopyImgR[3 * tempVal + 1];
				prtCvtR2L[3 * j + 2] = prtCopyImgR[3 * tempVal + 2];

				prtCopyImgR[3 * tempVal] = 0;
				prtCopyImgR[3 * tempVal + 1] = 0;
				prtCopyImgR[3 * tempVal + 2] = 0;
			}
		}
	}

	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "시차변환 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	cvShowImage("LeftImage", imgL);
	cvShowImage("RightImage", imgR);
	cvShowImage("ConvertImage", cvtR2L);

	cvWaitKey(0);

}