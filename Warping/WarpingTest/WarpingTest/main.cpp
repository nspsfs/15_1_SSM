#if 0
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

using namespace std;
using namespace cv;


typedef struct MATCH_PAIR
{
	int nA;
	int nB;
} MATCH_PAIR;


int FindMatchingPoints(const CvSeq* tKeypoints, const CvSeq* tDescriptors, const CvSeq* srcKeypoints, const CvSeq* srcDescriptors, int descriptor_size, MATCH_PAIR *pMatchPair);
int FindNearestPoints(const float* pA, int laplacian, const CvSeq* srcKeypoints, const CvSeq* srcDescriptors, int descriptor_size);
void rotateImage(IplImage* src, IplImage* dst, int direction);
int findCropPointX(IplImage* src);
void MergeImages(IplImage* Image1, IplImage* Image2, IplImage* dstImage);

int yw_panorama(int panorama_mode = 1, int feature = 1, int num_img = 0);
int test();
int main(){
	//test();
	yw_panorama(MODE_HORIZONTAL, FEATURE_SURF, 9);
	return 1;
}
int yw_panorama(int _panorama_mode, int _feature, int _num_img)
{

	cv::initModule_nonfree();
	///////////////////////////////////////////////////////////////////////////////////////////
	double start, finish;
	start = clock();

	int panorama_mode = _panorama_mode; // Horizontal == 1 Vertical == 2 Special == 3
	int feature = _feature; // SURF == 1  SIFT == 2 
	int num_img = _num_img;

	char* folder_path = "Images";

	char* file_path_Side = (char*)malloc(strlen(folder_path) + 20);
	char* file_path_Center = (char*)malloc(strlen(folder_path) + 20);

	char* save_path = (char*)malloc(strlen(folder_path) + 20);

	//char filePath_SideImg[128] = "Images\\test2.jpg"; // 휘어질 영상
	//char filePath_CenterImg[128] = "Images\\test1.jpg"; // 

	IplImage* SideImg;
	IplImage* CenterImg;

	///////////////////////////////////////////////////////////////////////////////////////////

	CvPoint2D32f *pt1 = NULL;
	CvPoint2D32f *pt2 = NULL;

	int num_good_match = 0;


	SideImg = cvLoadImage("test4_L.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	CenterImg = cvLoadImage("test4_R.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	if (feature == FEATURE_SURF){
		//surf 특징점 찾기
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSURFParams params = cvSURFParams(3000, 0);

		//T1 Img
		CvSeq *T1_Keypoints = NULL;
		CvSeq *T1_Descriptors = NULL;
		cvExtractSURF(SideImg, NULL, &T1_Keypoints, &T1_Descriptors, storage, params);

		//T2 Img
		CvSeq *T2_Keypoints = NULL;
		CvSeq *T2_Descriptors = NULL;
		cvExtractSURF(CenterImg, NULL, &T2_Keypoints, &T2_Descriptors, storage, params);

		//특징점 뿌리기 1
		CvSURFPoint* surf1;
		for (int i = 0; i < (T1_Keypoints ? T1_Keypoints->total : 0); i++)
		{
			surf1 = (CvSURFPoint*)cvGetSeqElem(T1_Keypoints, i);
			//cvCircle(SideImg, cvPoint(surf1->pt.x, surf1->pt.y), 3, CV_RGB(255, 0, 255), 2, 8);
		}

		//특징점 뿌리기 2
		CvSURFPoint* surf2;
		for (int i = 0; i < (T2_Keypoints ? T2_Keypoints->total : 0); i++)
		{
			surf2 = (CvSURFPoint*)cvGetSeqElem(T2_Keypoints, i);
			//cvCircle(CenterImg, cvPoint(surf2->pt.x, surf2->pt.y), 3, CV_RGB(255, 0, 255), 2, 8);
		}
		//cvShowImage("Center", CenterImg);
		//cvShowImage("Side", SideImg);
		//cvWaitKey(0);
		///////////////////////////////////////////////////////////////////////////////////////////
		IplImage* MergeImg = cvCreateImage(cvSize(SideImg->width + CenterImg->width, SideImg->height), IPL_DEPTH_8U, 1);
		MergeImages(SideImg, CenterImg, MergeImg);
		//매칭
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
			x2 = cvRound(surf2->pt.x) + SideImg->width;
			y2 = cvRound(surf2->pt.y);
			pt2[k] = surf2->pt;

			//병합 영상에 라인으로 표시하기
			CvPoint r1 = cvPoint(x1, y1);
			CvPoint r2 = cvPoint(x2, y2);
			cvLine(MergeImg, r1, r2, CV_RGB(0, 0, 255));
		}
		cvShowImage("merge", MergeImg);
		cvSaveImage("matching.jpg", MergeImg);
		cvWaitKey(0);
	}
	else if (feature == FEATURE_SIFT){

		num_good_match = 0;

		Mat img_1(SideImg);
		Mat img_2(CenterImg);
		SiftFeatureDetector detector(nFeatures, nOctavelayers, ContrastThreshold, EdgeThreshold, SIGMA);

		std::vector<KeyPoint> keypoints_1, keypoints_2;


		detector.detect(img_1, keypoints_1);
		detector.detect(img_2, keypoints_2);

		//-- Step 2: Calculate descriptors (feature vectors)
		//SurfDescriptorExtractor extractor;
		SiftDescriptorExtractor extractor(nFeatures, nOctavelayers, ContrastThreshold, EdgeThreshold, SIGMA);

		Mat descriptors_1, descriptors_2;

		extractor.compute(img_1, keypoints_1, descriptors_1);
		extractor.compute(img_2, keypoints_2, descriptors_2);

		//-- Step 3: Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;
		matcher.match(descriptors_1, descriptors_2, matches);

		double max_dist = 0; double min_dist = 100;

		//-- Quick calculation of max and min distances between keypoints
		for (int i = 0; i < descriptors_1.rows; i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}
		std::vector< DMatch > good_matches;
		for (int i = 0; i < descriptors_1.rows; i++)
		{
			if (matches[i].distance <= max(2 * min_dist, 0.02))
				//if (matches[i].distance <= 2 * min_dist)
			{
				good_matches.push_back(matches[i]);
				num_good_match++;
			}
		}
		if (num_good_match < 4){
			// handle exception
		}

		//-- Draw only "good" matches
		Mat img_matches;
		drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		pt1 = new CvPoint2D32f[num_good_match];
		pt2 = new CvPoint2D32f[num_good_match];

		for (int i = 0; i < num_good_match; i++){
			pt1[i] = keypoints_1[good_matches[i].queryIdx].pt;
			pt2[i] = keypoints_2[good_matches[i].trainIdx].pt;
		}
	}
	else{
		/*handle error : select feature*/
	}

	printf("feature matching complete\n");
	///////////////////////////////////////////////////////////////////////////////////////////
	//호모그래피 계산
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

	///////////////////////////////////////////////////////////////////////////////////////////
	//모자이크 영상 만들기
	IplImage* WarpImg = cvCreateImage(cvSize(SideImg->width * 2, SideImg->height), SideImg->depth, SideImg->nChannels);

	cvWarpPerspective(SideImg, WarpImg, &mxH);
	cvShowImage("warp", WarpImg);
	cvSaveImage("result1.jpg", WarpImg);
	cvWaitKey(0);
	cvSetImageROI(WarpImg, cvRect(0, 0, CenterImg->width, CenterImg->height));
	cvCopy(CenterImg, WarpImg);
	cvResetImageROI(WarpImg);
	cvShowImage("warp", WarpImg);
	cvSaveImage("result2.jpg", WarpImg);
	cvWaitKey(0);

	//crop invalid area
	//int x = findCropPointX(WarpImg);
	//IplImage* PanoImg = cvCreateImage(cvSize(x, WarpImg->height), WarpImg->depth, WarpImg->nChannels);
	//cvSetImageROI(WarpImg, cvRect(0, 0, x, WarpImg->height));
	//cvShowImage("warp1", WarpImg);
	//cvWaitKey(0);
	//cvCopy(WarpImg, PanoImg);
	//cvShowImage("pano", PanoImg);
	//cvWaitKey(0);

	//cvNamedWindow("PanoImg", CV_WINDOW_NORMAL);
	printf("mosaic image complete\n");

	printf("save image complete\n");
	///////////////////////////////////////////////////////////////////////////////////////////   


	cvReleaseImage(&SideImg);
	cvReleaseImage(&CenterImg);
	cvReleaseImage(&WarpImg);


	finish = clock();
	printf("time = %f", finish - start);

	return (finish - start) / 1000;
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
void rotateImage(IplImage* src, IplImage* dst, int direction){

	if (src == NULL || dst == NULL){
		/*handle error*/
	}
	else if ((direction != ANGLE_CLOCKWISE_90) && (direction != ANGLE_COUNTER_CLOCKWISE_90)){
		/*handle error*/
	}
	else if ((src->width != dst->height) || (src->height != dst->width)){
		/*handle error*/
	}


	IplImage *srcB, *srcG, *srcR,
		*dstB, *dstG, *dstR;
	CvPoint2D32f centerPoint;
	CvMat* rotationMatrix = cvCreateMat(2, 3, CV_32FC1);

	if (src->nChannels == 3){
		srcB = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		srcG = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		srcR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

		dstB = cvCreateImage(cvGetSize(dst), IPL_DEPTH_8U, 1);
		dstG = cvCreateImage(cvGetSize(dst), IPL_DEPTH_8U, 1);
		dstR = cvCreateImage(cvGetSize(dst), IPL_DEPTH_8U, 1);

		cvSplit(src, srcB, srcG, srcR, NULL);
	}

	/*calculate rotation matrix*/
	if (direction == ANGLE_COUNTER_CLOCKWISE_90){
		if (src->width > src->height) // wide image Okay
		{
			centerPoint = cvPoint2D32f(src->width / 2, src->width / 2);
			cv2DRotationMatrix(centerPoint, ANGLE_COUNTER_CLOCKWISE_90, 1, rotationMatrix);
		}
		else if (src->width < src->height) // narrow image Okay
		{
			centerPoint = cvPoint2D32f(src->width / 2, src->width / 2);
			cv2DRotationMatrix(centerPoint, ANGLE_COUNTER_CLOCKWISE_90, 1, rotationMatrix);
		}
		else // width == height  square image
		{
			centerPoint = cvPoint2D32f(src->width / 2, src->width / 2);
			cv2DRotationMatrix(centerPoint, ANGLE_COUNTER_CLOCKWISE_90, 1, rotationMatrix);
		}
	}
	else if (direction == ANGLE_CLOCKWISE_90){
		if (src->height < src->width) // wide image Okay
		{
			centerPoint = cvPoint2D32f(src->height / 2, src->height / 2);
			cv2DRotationMatrix(centerPoint, ANGLE_CLOCKWISE_90, 1, rotationMatrix);
		}
		else if (src->height > src->width) // narrow image
		{
			centerPoint = cvPoint2D32f(src->height / 2, src->height / 2);
			cv2DRotationMatrix(centerPoint, ANGLE_CLOCKWISE_90, 1, rotationMatrix);
		}
		else // height == width  square image
		{
			centerPoint = cvPoint2D32f(src->height / 2, src->height / 2);
			cv2DRotationMatrix(centerPoint, ANGLE_CLOCKWISE_90, 1, rotationMatrix);
		}
	}

	/*rotate*/
	if (src->nChannels == 3){
		cvWarpAffine(srcB, dstB, rotationMatrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
		cvWarpAffine(srcG, dstG, rotationMatrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
		cvWarpAffine(srcR, dstR, rotationMatrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);

		cvMerge(dstB, dstG, dstR, NULL, dst);

		cvReleaseImage(&srcB);
		cvReleaseImage(&srcG);
		cvReleaseImage(&srcR);
		cvReleaseImage(&dstB);
		cvReleaseImage(&dstG);
		cvReleaseImage(&dstR);
	}
	else if (src->nChannels == 1)
	{
		cvWarpAffine(src, dst, rotationMatrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	}

	cvReleaseMat(&rotationMatrix);
}
int findCropPointX(IplImage* src){

	//계산 시간을 줄이기 위해서 영상크기 줄임
	IplImage* down = cvCreateImage(cvSize(src->width / 2, src->height / 2), src->depth, src->nChannels);
	cvResize(src, down);

	int i = 0;
	double min_val = 0;
	double max_val = 0;
	int line_thickness = src->width / 40;
	CvPoint max_loc, min_loc;
	int x = 0;

	IplImage* correlationImage = cvCreateImage(cvSize(1, 1), IPL_DEPTH_32F, 1);
	IplImage* rightLineImage = cvCreateImage(cvSize(line_thickness, down->height), down->depth, down->nChannels);

	cvSetZero(rightLineImage);
	cvResetImageROI(down);

	for (int i = line_thickness + 1; i < down->width / 2; i += line_thickness)
	{
		cvSetImageROI(down, cvRect(down->width - i, 0, line_thickness, down->height));
		cvMatchTemplate(rightLineImage, down, correlationImage, CV_TM_CCOEFF_NORMED);
		cvMinMaxLoc(correlationImage, &min_val, &max_val, &min_loc, &max_loc);
		cvResetImageROI(down);

		//printf("when x = %d, max = %6.5lf\n", down->width - i, max_val);

		if (max_val < 0.5){
			x = down->width - i;
			break;
		}
	}

	//cvLine(down, cvPoint(x, 0), cvPoint(x, down->height), CV_RGB(255, 0, 0), 2);
	//cvShowImage("down", down);
	//cvShowImage("croped", rightLineImage);
	//cvShowImage("corr", correlationImage);
	//cvWaitKey(0);

	cvReleaseImage(&down);
	cvReleaseImage(&correlationImage);
	cvReleaseImage(&rightLineImage);
	return x * 2;
}

int test(){
	CvSize down;
	IplImage* src = cvLoadImage("Images\\test.jpg", CV_LOAD_IMAGE_UNCHANGED);
	IplImage* dst = cvCreateImage(cvSize(512, 288), IPL_DEPTH_8U, 3);
	cvResize(cvLoadImage("Images\\test.jpg", CV_LOAD_IMAGE_UNCHANGED), dst);


	int i = 0;
	return i;
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
#endif
