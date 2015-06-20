#if 1
#include <time.h> 
#include <stdio.h>

#include <iostream>
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
#include <opencv2\opencv.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\features2d.hpp>

/*SIFT Parameters*/
#define nFeatures 150
#define nOctavelayers 3
#define ContrastThreshold 0.04
#define EdgeThreshold 10.0
#define SIGMA 1.6

/*Feature*/
#define FEATURE_SURF 1 //default
#define FEATURE_SIFT 2

/*Mode*/
#define MODE_HORIZONTAL 1 //defualt
#define MODE_VERTICAL 2
#define MODE_SPECIAL 3

/*Rotate Angle*/
#define ANGLE_COUNTER_CLOCKWISE_90 90
#define ANGLE_CLOCKWISE_90 -90

/*Image Path*/
#define Left_Image "image/test4_L.jpg"
#define Right_Image "image/test4_R.jpg"

using namespace std;
using namespace cv;


typedef struct MATCH_PAIR
{
	int nA;
	int nB;
} MATCH_PAIR;

int FindMatchingPoints(const CvSeq* tKeypoints, const CvSeq* tDescriptors, const CvSeq* srcKeypoints, const CvSeq* srcDescriptors, int descriptor_size, MATCH_PAIR *pMatchPair);
int FindNearestPoints(const float* pA, int laplacian, const CvSeq* srcKeypoints, const CvSeq* srcDescriptors, int descriptor_size);
//void rotateImage(IplImage* src, IplImage* dst, int direction);
//int findCropPointX(IplImage* src);
void MergeImages(IplImage* Image1, IplImage* Image2, IplImage* dstImage);

int main()
{
	cv::initModule_nonfree();

	double start, finish;
	start = clock();


	IplImage* LeftImg;
	IplImage* RightImg;

	CvPoint2D32f *pt1 = NULL;
	CvPoint2D32f *pt2 = NULL;

	int num_good_match = 0;

	LeftImg = cvLoadImage(Left_Image, CV_LOAD_IMAGE_GRAYSCALE);
	RightImg = cvLoadImage(Right_Image, CV_LOAD_IMAGE_GRAYSCALE);

#if 1 //surf mode
	//surf 특징점 찾기
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSURFParams params = cvSURFParams(3000, 0);

	//T1 -> LeftImg
	CvSeq *T1_Keypoints = NULL;
	CvSeq *T1_Descriptors = NULL;
	cvExtractSURF(LeftImg, NULL, &T1_Keypoints, &T1_Descriptors, storage, params);

	//T2 -> RightImg
	CvSeq *T2_Keypoints = NULL;
	CvSeq *T2_Descriptors = NULL;
	cvExtractSURF(RightImg, NULL, &T2_Keypoints, &T2_Descriptors, storage, params);

	//Left 특징점 뿌리기
	CvSURFPoint* surf1;
	for (int i = 0; i < (T1_Keypoints ? T1_Keypoints->total : 0); i++)
	{
		surf1 = (CvSURFPoint*)cvGetSeqElem(T1_Keypoints, i);
		cvCircle(LeftImg, cvPoint(surf1->pt.x, surf1->pt.y), 3, CV_RGB(255, 0, 255), 2, 8);
	}

	//Right 특징점 뿌리기
	CvSURFPoint* surf2;
	for (int i = 0; i < (T2_Keypoints ? T2_Keypoints->total : 0); i++)
	{
		surf2 = (CvSURFPoint*)cvGetSeqElem(T2_Keypoints, i);
		cvCircle(RightImg, cvPoint(surf2->pt.x, surf2->pt.y), 3, CV_RGB(255, 0, 255), 2, 8);
	}
	cvShowImage("LEFT", LeftImg);
	cvShowImage("RIGHT", RightImg);
	//cvWaitKey(0);

	IplImage* MergeImg = cvCreateImage(cvSize(LeftImg->width + RightImg->width, LeftImg->height), IPL_DEPTH_8U, 1);
	MergeImages(LeftImg, RightImg, MergeImg);

	MATCH_PAIR *pMatchPair = new MATCH_PAIR[T1_Keypoints->total];
	int descriptor_size = params.extended ? 128 : 64;
	num_good_match = FindMatchingPoints(T1_Keypoints, T1_Descriptors, T2_Keypoints, T2_Descriptors, descriptor_size, pMatchPair);

	pt1 = new CvPoint2D32f[num_good_match];
	pt2 = new CvPoint2D32f[num_good_match];

	int x1, y1, x2, y2;

	for (int k = 0; k < num_good_match; k++)
	{
		//매칭 k번째의 T1, T2에서의 키 포인트 정보 뽑기
		surf1 = (CvSURFPoint*)cvGetSeqElem(T1_Keypoints, pMatchPair[k].nA);
		x1 = cvRound(surf1->pt.x);
		y1 = cvRound(surf1->pt.y);
		pt1[k] = surf1->pt;

		surf2 = (CvSURFPoint*)cvGetSeqElem(T2_Keypoints, pMatchPair[k].nB);
		x2 = cvRound(surf2->pt.x) + LeftImg->width;
		y2 = cvRound(surf2->pt.y);
		pt2[k] = surf2->pt;

		//병합 영상에 라인으로 표시하기
		CvPoint r1 = cvPoint(x1, y1);
		CvPoint r2 = cvPoint(x2, y2);
		cvLine(MergeImg, r1, r2, CV_RGB(0, 0, 255));
	}

	cvShowImage("MergeImage", MergeImg);
	finish = clock();
	cout << finish - start << endl;
	cvWaitKey(0);

	cvReleaseImage(&LeftImg);
	cvReleaseImage(&RightImg);
#endif

#if 1	//호모그래피 계산
	CvMat M1, M2;
	double H[9];
	CvMat mxH = cvMat(3, 3, CV_64F, H);
	M1 = cvMat(1, num_good_match, CV_32FC2, pt1);
	M2 = cvMat(1, num_good_match, CV_32FC2, pt2);
	cvFindHomography(&M1, &M2, &mxH, CV_RANSAC, 2);

	//print Homography
	printf(" Homography matrix\n");
	for (int rows = 0; rows < 3; rows++)
	{
		for (int cols = 0; cols < 3; cols++)
		{
			printf("%lf ", cvmGet(&mxH, rows, cols));
		}
		printf("\n");
	}

	//모자이크 영상 만들기
	IplImage* WarpImg = cvCreateImage(cvSize(LeftImg->width * 2, LeftImg->height), LeftImg->depth, LeftImg->nChannels);

	cvWarpPerspective(LeftImg, WarpImg, &mxH);
	cvShowImage("warp", WarpImg);
	//cvSaveImage("result1.jpg", WarpImg);
	cvWaitKey(0);
	cvSetImageROI(WarpImg, cvRect(0, 0, RightImg->width, RightImg->height));
	cvCopy(RightImg, WarpImg);
	cvResetImageROI(WarpImg);
	cvShowImage("warp", WarpImg);
	//cvSaveImage("result2.jpg", WarpImg);
	cvWaitKey(0);



#endif
}


void MergeImages(IplImage* Image1, IplImage* Image2, IplImage* dstImage)
{
	cvSet(dstImage, CV_RGB(255, 255, 255));

	cvSetImageROI(dstImage, cvRect(0, 0, Image1->width, Image1->height));
	cvCopy(Image1, dstImage);

	cvSetImageROI(dstImage, cvRect(Image1->width, 0, Image2->width, Image2->height));
	cvCopy(Image2, dstImage);

	cvResetImageROI(dstImage);
}

int FindMatchingPoints(const CvSeq* tKeypoints, const CvSeq* tDescriptors, const CvSeq* srcKeypoints, const CvSeq* srcDescriptors, int descriptor_size, MATCH_PAIR *pMatchPair)
{
	int i;
	float* pA;
	int nMatchB;
	CvSURFPoint* surfA;
	int k = 0;
	for (i = 0; i < tDescriptors->total; i++)
	{
		pA = (float*)cvGetSeqElem(tDescriptors, i);
		surfA = (CvSURFPoint*)cvGetSeqElem(tKeypoints, i);
		nMatchB = FindNearestPoints(pA, surfA->laplacian, srcKeypoints, srcDescriptors, descriptor_size);
		if (nMatchB > 0)
		{
			pMatchPair[k].nA = i;
			pMatchPair[k].nB = nMatchB;
			k++;
		}
	}

	return k;
}

int FindNearestPoints(const float* pA, int laplacian, const CvSeq* srcKeypoints, const CvSeq* srcDescriptors, int descriptor_size)
{
	int i, k;
	float* pB;
	CvSURFPoint *surfB;
	int nMatch = -1;
	double sum2, min1 = 10000, min2 = 10000;
	for (i = 0; i < srcDescriptors->total; i++)
	{
		surfB = (CvSURFPoint*)cvGetSeqElem(srcKeypoints, i);
		pB = (float*)cvGetSeqElem(srcDescriptors, i);
		if (laplacian != surfB->laplacian)
			continue;

		sum2 = 0.0f;
		for (k = 0; k < descriptor_size; k++)   { sum2 += (pA[k] - pB[k])*(pA[k] - pB[k]); }

		if (sum2 < min1)
		{
			min2 = min1;
			min1 = sum2;
			nMatch = i;
		}
		else if (sum2 < min2)   { min2 = sum2; }
	}
	if (min1 < 0.6*min2)
		return nMatch;

	return -1;
}

#endif