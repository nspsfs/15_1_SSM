#if 0

#include "opencv2\opencv.hpp"
#include "opencv2\highgui\highgui.hpp"
#include <iostream>
#include <chrono>

using namespace cv;

int main()
{
	Mat img1 = imread("image/test2_1.bmp");
	Mat img2 = imread("image/test2_2.bmp");
	Mat img1g;
	Mat img2g;

	int i, j;

	cvtColor(img1, img1g, COLOR_BGR2GRAY);
	cvtColor(img2, img2g, COLOR_BGR2GRAY);

	Mat sml1(img1g.size().height, img1g.size().width, CV_8UC1);
	Mat sml2(img2g.size().height, img2g.size().width, CV_8UC1);
	Mat result = img1g.clone();

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	for (j = 1; j < img1g.size().width - 1; j++)
	{	
		for (i = 1; i < img1g.size().height - 1; i++)
		{
			sml1.at<uchar>(i, j) = abs(2 * img1g.at<uchar>(i, j) - img1g.at<uchar>(i - 1, j) - img1g.at < uchar>(i + 1, j)) + abs(2 * img1g.at<uchar>(i, j) - img1g.at<uchar>(i, j - 1) - img1g.at<uchar>(i, j + 1));
			sml2.at<uchar>(i, j) = abs(2 * img2g.at<uchar>(i, j) - img2g.at<uchar>(i - 1, j) - img2g.at < uchar>(i + 1, j)) + abs(2 * img2g.at<uchar>(i, j) - img2g.at<uchar>(i, j - 1) - img2g.at<uchar>(i, j + 1));
			if (sml1.at<uchar>(i, j) > sml2.at<uchar>(i,j))
			{
				result.at<uchar>(i, j) = img1g.at<uchar>(i, j);
			}
			else
			{
				result.at<uchar>(i, j) = img2g.at<uchar>(i, j);
			}
		}
	} 
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	imshow("img1", img1g);
	imshow("img2", img2g);
	imshow("sml1", sml1);
	imshow("sml2", sml2);
	imshow("result", result);
	imwrite("SML.jpg", result);

	cvWaitKey(0);
}

#endif

#if 0

#include "opencv\cv.h"
#include "opencv\highgui.h"
#include <iostream>
#include <chrono>

using namespace cv;

int main()
{
	IplImage* img1C = cvLoadImage("image/test2_1.bmp", 1);
	IplImage* img2C = cvLoadImage("image/test2_2.bmp", 1);
	IplImage* img1 = cvLoadImage("image/test2_1.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* img2 = cvLoadImage("image/test2_2.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	CvSize size = cvGetSize(img1);

	IplImage* sml1 = cvCloneImage(img1);
	IplImage* sml2 = cvCloneImage(img2);
	cvSetZero(sml1);
	cvSetZero(sml2);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	//#pragma omp parallel for schedule(dynamic)
	for (int i = 1; i < size.height - 1; i++)
	{
		uchar* ptrImg1 = (uchar*)(img1->imageData + (i*img1->widthStep));
		uchar* ptrImg1up = (uchar*)(img1->imageData + ((i - 1)*img1->widthStep));
		uchar* ptrImg1down = (uchar*)(img1->imageData + ((i + 1)*img1->widthStep));

		uchar* ptrImg2 = (uchar*)(img2->imageData + (i*img2->widthStep));
		uchar* ptrImg2up = (uchar*)(img2->imageData + ((i - 1)*img2->widthStep));
		uchar* ptrImg2down = (uchar*)(img2->imageData + ((i + 1)*img2->widthStep));

		uchar* ptrsml1 = (uchar*)(sml1->imageData + (i*sml1->widthStep));
		uchar* ptrsml2 = (uchar*)(sml2->imageData + (i*sml2->widthStep));
		for (int j = 1; j < size.width - 1; j++)
		{
			uchar a1 = abs(2 * ptrImg1[j] - ptrImg1up[j] - ptrImg1down[j]);
			uchar b1 = abs(2 * ptrImg1[j] - ptrImg1[j - 1] - ptrImg1[j + 1]);
			uchar a2 = abs(2 * ptrImg2[j] - ptrImg2up[j] - ptrImg2down[j]);
			uchar b2 = abs(2 * ptrImg2[j] - ptrImg2[j - 1] - ptrImg2[j + 1]);

			ptrsml1[j] = a1 + b1;
			ptrsml2[j] = a2 + b2;
		}
	}

	CvScalar sml1Avg = cvAvg(sml1);
	CvScalar sml2Avg = cvAvg(sml2);
	double dAvg1 = sml1Avg.val[0];
	double dAvg2 = sml2Avg.val[0];

	IplImage* sml1Thresh = cvCloneImage(sml1);
	IplImage* sml2Thresh = cvCloneImage(sml2);

	cvThreshold(sml1, sml1Thresh, dAvg1 + dAvg2, 255, CV_THRESH_BINARY);
	cvThreshold(sml2, sml2Thresh, dAvg1 + dAvg2, 255, CV_THRESH_BINARY);
	cvDilate(sml1Thresh, sml1Thresh, 0, 2);
	cvErode(sml1Thresh, sml1Thresh, 0, 2);
	cvDilate(sml2Thresh, sml2Thresh, 0, 2);
	cvErode(sml2Thresh, sml2Thresh, 0, 2);

	IplImage* imgAvg = cvCloneImage(img1C);
	cvSet(imgAvg, cvScalarAll(2), 0);
	cvDiv(img1C, imgAvg, img1C);
	cvDiv(img2C, imgAvg, img2C);
	cvAdd(img1C, img2C, imgAvg);

	CvMat* markers = cvCreateMat(size.height, size.width, CV_32SC1);
	cvSetZero(markers);
	int seedNum = 1;
	int seedStep = size.width / 10;
	for (int row = seedStep; row < size.height; row += seedStep) {
		int* ptr = (int*)(markers->data.ptr + row * markers->step);
		for (int col = seedStep; col < size.width; col += seedStep) {
			ptr[col] = seedNum;
			seedNum++;
		}
	}

	// watershed 알고리즘 실행
	cvWatershed(imgAvg, markers);

	long long int arrSml1[7000] = { 0, };
	long long int arrSml2[7000] = { 0, };

//#pragma omp parallel for schedule(dynamic)
	for (int i = 1; i < size.height - 1; i++)
	{
		int* ptrMarkers = (int*)(markers->data.ptr + i * markers->step);
		uchar* ptrsml1 = (uchar*)(sml1->imageData + (i*sml1->widthStep));
		uchar* ptrsml2 = (uchar*)(sml2->imageData + (i*sml2->widthStep));
		for (int j = 1; j < size.width - 1; j++)
		{
			if (ptrMarkers[j]>0)
			{
				if (ptrsml1[j] == ptrsml2[j])
				{
					arrSml1[ptrMarkers[j]]++;
					arrSml2[ptrMarkers[j]]++;
				}
				else if (ptrsml1[j]>ptrsml2[j])
				{
					arrSml1[ptrMarkers[j]]++;
				}
				else
				{
					arrSml2[ptrMarkers[j]]++;
				}
			}
		}
	}

	IplImage* fusionImage = cvCloneImage(img1);
//#pragma omp parallel for schedule(dynamic)
	for (int i = 1; i < size.height - 1; i++)
	{
		int* ptrMarkers = (int*)(markers->data.ptr + i * markers->step);
		uchar* ptrimg2 = (uchar*)(img2->imageData + (i*img2->widthStep));
		uchar* ptrFimg = (uchar*)(fusionImage->imageData + (i*fusionImage->widthStep));
		for (int j = 1; j < size.width - 1; j++)
		{
			if (ptrMarkers[j]==-1)
			{
				if (j - 1 >= 0)
				{
					ptrFimg[j] = ptrFimg[j - 1];
				}
			}
			else
			{
				if (arrSml1[ptrMarkers[j]] < arrSml2[ptrMarkers[j]])
				{
					ptrFimg[j] = ptrimg2[j];
				}
			}
		}
	}


	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	cvShowImage("SML1", sml1);
	cvShowImage("SML2", sml2);
	cvShowImage("SML1Th", sml1Thresh);
	cvShowImage("SML2Th", sml2Thresh);
	cvShowImage("FusionImage", fusionImage);


	cvWaitKey(0);
}

#endif