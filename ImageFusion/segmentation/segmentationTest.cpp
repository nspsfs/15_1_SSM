#if 0
#include <iostream>
#include "opencv/cv.h"
#include "opencv2/legacy/legacy.hpp"
#include "opencv/highgui.h"
#include <time.h>

using namespace std;

int main()
{
	clock_t start_time, end_time;
	//IplImage* img = cvLoadImage("image/test4_1.jpg", 1);
	IplImage* img = cvLoadImage("image/test.bmp", 1);
	IplImage* img2 = cvCloneImage(img);
	IplImage* color_tab = cvCloneImage(img);

	start_time = clock();

	// markers 생성
	CvSize size = cvGetSize(img);
	CvMat* markers = cvCreateMat(size.height, size.width, CV_32SC1);
	cvSetZero(markers);
	int seedNum = 1;
	int seedStep = size.width/60;
	for (int row = seedStep; row < size.height; row += seedStep) {
		int* ptr = (int*)(markers->data.ptr + row * markers->step);
		for (int col = seedStep; col < size.width; col += seedStep) {
			ptr[col] = seedNum;
			seedNum++;
		}
	}

	// watershed 알고리즘 실행
	cvWatershed(img, markers);

	end_time = clock();
	cout << (double)(end_time - start_time) / 1000 << " sec" << endl;

	// wshed 이미지 생성 - 사실 뭔지 잘 모르겠다.
	for (int i = 0; i < markers->height; i++) {
		int* ptrMarkers = (int*)(markers->data.ptr + i*markers->step);
		uchar* dst = (uchar*)(img2->imageData + i*img2->widthStep);
		uchar* ptr = (uchar*)(img->imageData + i* img->widthStep);
		for (int j = 0; j < markers->width; j++) {
			int idx = ptrMarkers[j];
			if (idx == -1)
				dst[j * 3] = dst[j * 3 + 1] = dst[j * 3 + 2] = (uchar)255;
			else if (idx < -1 || idx > seedNum)
				dst[j * 3 + 0] = dst[j * 3 + 1] = dst[j * 3 + 2] = (uchar)0;
			else {
				dst[j * 3 + 0] = ptr[j * 3 + 0];
				dst[j * 3 + 1] = ptr[j * 3 + 1];
				dst[j * 3 + 2] = ptr[j * 3 + 2];
			}
		}
	}

	cvShowImage("segmentation", img2);
	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&img);

	return 0;
}
#endif

#if 0
#include <iostream>
#include <opencv/cv.h>
#include <opencv2/legacy/legacy.hpp>
#include <opencv/highgui.h>
#include <time.h>

using namespace std;

int main()
{
	clock_t start_time, end_time;
	IplImage* img = cvLoadImage("tsukuba_l.png", 1);
	IplImage* img2 = cvCloneImage(img);
	IplImage* color_tab = cvCloneImage(img);

	start_time = clock();

	// markers 생성
	CvSize size = cvGetSize(img);
	CvMat* markers = cvCreateMat(size.height, size.width, CV_32SC1);
	cvSetZero(markers);
	int seedNum = 1;
	int seedStep = 10;
	for (int row = seedStep; row < size.height; row += seedStep) {
		int* ptr = (int*)(markers->data.ptr + row * markers->step);
		for (int col = seedStep; col < size.width; col += seedStep) {
			ptr[col] = seedNum;
			seedNum++;
		}
	}

	// watershed 알고리즘 실행
	cvWatershed(img, markers);

	end_time = clock();
	cout << (double)(end_time - start_time) / 1000 << " sec" << endl;

	// wshed 이미지 생성 - 사실 뭔지 잘 모르겠다.
	for (int i = 0; i < markers->height; i++) {
		int* ptrMarkers = (int*)(markers->data.ptr + i*markers->step);
		uchar* dst = (uchar*)(img2->imageData + i*img2->widthStep);
		uchar* ptr = (uchar*)(img->imageData + i* img->widthStep);
		for (int j = 0; j < markers->width; j++) {
			int idx = ptrMarkers[j];
			if (idx == -1)
				dst[j * 3] = dst[j * 3 + 1] = dst[j * 3 + 2] = (uchar)255;
			else if (idx < -1 || idx > seedNum)
				dst[j * 3 + 0] = dst[j * 3 + 1] = dst[j * 3 + 2] = (uchar)0;
			else {
				dst[j * 3 + 0] = ptr[j * 3 + 0];
				dst[j * 3 + 1] = ptr[j * 3 + 1];
				dst[j * 3 + 2] = ptr[j * 3 + 2];
			}
		}
	}

	cvShowImage("segmentation", img2);
	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&img);

	return 0;
}
#endif
