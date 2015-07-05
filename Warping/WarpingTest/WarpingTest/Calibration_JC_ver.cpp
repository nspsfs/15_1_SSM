#if 0

#pragma warning( disable: 4996 )

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <vector>
#include <iostream>
using namespace std;

class StereoCalibrator
{
private:
	int numChessBoard;
	int numChessBoardCorner;
	int widthChessBoardCorner;
	int heightChessBoardCorner;
	void StereoCalibrator::detectChessBoard(IplImage* image, vector<CvPoint2D32f>* points, int lr)
	{
		// finding chessboard corners
		vector<CvPoint2D32f> chessBoardCorners(numChessBoardCorner);
		int cornerCount = 0;
		int isPatternFound = cvFindChessboardCorners(image, cvSize(widthChessBoardCorner, heightChessBoardCorner), &chessBoardCorners[0], &cornerCount, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		for (int i = 0; i < cornerCount; i++) {
			chessBoardCorners[i].x;
			chessBoardCorners[i].y;
		}
		cvFindCornerSubPix(image, &chessBoardCorners[0], cornerCount, cvSize(11, 11), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 0.01));

		// add finded points to points vector
		int N = points[lr].size();
		points[lr].resize(N + numChessBoardCorner, cvPoint2D32f(0, 0));
		copy(chessBoardCorners.begin(), chessBoardCorners.end(), points[lr].begin() + N);
		cout << ".";
	}

	void StereoCalibrator::detectChessBoardasStereo(IplImage* image1, IplImage* image2, vector<CvPoint2D32f>* points)
	{
		detectChessBoard(image1, points, 0);
		detectChessBoard(image2, points, 1);

		numChessBoard++;
	}

	void StereoCalibrator::makeInstrinsicMatrixAndDistortionMap(vector<CvPoint2D32f> points[2], CvSize* imgSize, CvMat* IntrisicMatrix1, CvMat*  IntrisicMatrix2, CvMat* DistortionCoeffs1, CvMat* DistortionCoeffs2, CvMat* R, CvMat* T, CvMat* E, CvMat* F)
	{
		// make objectPoint, imagePoint matrixes
		int numPoints = numChessBoard*numChessBoardCorner;
		vector<CvPoint3D32f> objectPoints;
		const float squareSize = 1.f;
		objectPoints.resize(numChessBoard*numChessBoardCorner);
		for (int i = 0; i < heightChessBoardCorner; i++) {
			for (int j = 0; j < widthChessBoardCorner; j++) {
				objectPoints[i*widthChessBoardCorner + j] = cvPoint3D32f(i*squareSize, j*squareSize, 0);
			}
		}
		for (int i = 1; i < numChessBoard; i++) {
			copy(objectPoints.begin(), objectPoints.begin() + numChessBoardCorner, objectPoints.begin() + i*numChessBoardCorner);
		}
		vector<int> npoints;
		npoints.resize(numChessBoard, numChessBoardCorner);
		CvMat _objectPoints = cvMat(1, numPoints, CV_32FC3, &objectPoints[0]);
		CvMat _imagePoints1 = cvMat(1, numPoints, CV_32FC2, &points[0][0]);
		CvMat _imagePoints2 = cvMat(1, numPoints, CV_32FC2, &points[1][0]);
		CvMat _npoints = cvMat(1, npoints.size(), CV_32S, &npoints[0]);

		// matrix initialize
		cvSetIdentity(IntrisicMatrix1);
		cvSetIdentity(IntrisicMatrix2);
		cvZero(DistortionCoeffs1);
		cvZero(DistortionCoeffs2);

		// calibration
		cout << "Running stereo calibration ..." << endl;
		cvStereoCalibrate(&_objectPoints, &_imagePoints1, &_imagePoints2, &_npoints, IntrisicMatrix1, DistortionCoeffs1, IntrisicMatrix2, DistortionCoeffs2, *imgSize, R, T, E, F, cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5), CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST + CV_CALIB_SAME_FOCAL_LENGTH);
	}
public:
	StereoCalibrator(int widthChessBoardCorner, int heightChessBoardCorner)
	{
		this->numChessBoard = 0;
		this->widthChessBoardCorner = widthChessBoardCorner;
		this->heightChessBoardCorner = heightChessBoardCorner;
		this->numChessBoardCorner = widthChessBoardCorner * heightChessBoardCorner;
	}

	void StereoCalibrator::StereoCalib(const char* imageList, CvMat* mx1, CvMat* my1, CvMat* mx2, CvMat* my2, int showUndistorted)
	{
		vector<CvPoint2D32f> points[2];
		vector<string> imageNames[2];
		CvSize imageSize = { 0, 0 };

		FILE* f = fopen(imageList, "rt");
		if (!f) {
			fprintf(stderr, "can not open file %s\n", imageList);
			return;
		}

		for (int i = 0;; i++) {
			char buf1[1024];
			char buf2[1024];
			int leftRight = i % 2;

			if (!fgets(buf1, sizeof(buf1) - 3, f)) break;
			if (!fgets(buf2, sizeof(buf2) - 3, f)) break;

			size_t len1 = strlen(buf1);
			size_t len2 = strlen(buf2);
			while (len1 > 0 && isspace(buf1[len1 - 1])) buf1[--len1] = '\0';
			while (len2 > 0 && isspace(buf2[len2 - 1])) buf2[--len2] = '\0';

			imageNames[0].push_back(buf1);
			imageNames[1].push_back(buf2);

			IplImage* img1 = cvLoadImage(buf1, 0);
			IplImage* img2 = cvLoadImage(buf2, 0);
			if (!img1 || !img2) {
				break;
			}

			imageSize = cvGetSize(img1);
			detectChessBoardasStereo(img1, img2, points);
			cvReleaseImage(&img1);
			cvReleaseImage(&img1);
		}
		fclose(f);
		cout << endl;

		// make Intrinsic Matrixes And Distortion Coefficeints Matrixes
		CvMat* M1 = cvCreateMat(3, 3, CV_64F);
		CvMat* M2 = cvCreateMat(3, 3, CV_64F);
		CvMat* D1 = cvCreateMat(1, 5, CV_64F);
		CvMat* D2 = cvCreateMat(1, 5, CV_64F);
		CvMat* R = cvCreateMat(3, 3, CV_64F);
		CvMat* T = cvCreateMat(3, 1, CV_64F);
		CvMat* E = cvCreateMat(3, 3, CV_64F);
		CvMat* F = cvCreateMat(3, 3, CV_64F);
		makeInstrinsicMatrixAndDistortionMap(points, &imageSize, M1, M2, D1, D2, R, T, E, F);
		points->clear();

		// redtification
		CvMat* R1 = cvCreateMat(3, 3, CV_64F);
		CvMat* R2 = cvCreateMat(3, 3, CV_64F);
		CvMat* P1 = cvCreateMat(3, 4, CV_64F);
		CvMat* P2 = cvCreateMat(3, 4, CV_64F);
		CvMat* Q = cvCreateMat(4, 4, CV_8UC1);
		cvStereoRectify(M1, M2, D1, D2, imageSize, R, T, R1, R2, P1, P2, Q, 0);//hartley's methud
		cout << "Q\n"<<Q << endl;
		cvReleaseMat(&R);
		cvReleaseMat(&T);
		cvReleaseMat(&E);
		cvReleaseMat(&F);

		// make Distortion Maps
		cvInitUndistortRectifyMap(M1, D1, R1, P1, mx1, my1);
		cvInitUndistortRectifyMap(M2, D2, R2, P2, mx2, my2);
		cvReleaseMat(&M1);
		cvReleaseMat(&M2);
		cvReleaseMat(&D1);
		cvReleaseMat(&D2);
		cvReleaseMat(&P1);
		cvReleaseMat(&P2);
		cvReleaseMat(&R1);
		cvReleaseMat(&R2);

		if (showUndistorted) {
			// calibration
			cvNamedWindow("rectified", 1);
			CvMat* pair = cvCreateMat(imageSize.height, imageSize.width * 2, CV_8UC3);

			CvMat* img1r = cvCreateMat(imageSize.height, imageSize.width, CV_8U);
			CvMat* img2r = cvCreateMat(imageSize.height, imageSize.width, CV_8U);
			for (int i = 0; i < numChessBoard; i++) {
				IplImage* img1 = cvLoadImage(imageNames[0][i].c_str(), 0);
				IplImage* img2 = cvLoadImage(imageNames[1][i].c_str(), 0);
				if (img1 && img2) {
					CvMat part;
					cvRemap(img1, img1r, mx1, my1);
					cvRemap(img2, img2r, mx2, my2);

					cvGetCols(pair, &part, 0, imageSize.width);
					cvCvtColor(img1r, &part, CV_GRAY2BGR);
					cvGetCols(pair, &part, imageSize.width, imageSize.width * 2);
					cvCvtColor(img2r, &part, CV_GRAY2BGR);

					for (int j = 0; j < imageSize.height; j += 16)
						cvLine(pair, cvPoint(0, j), cvPoint(imageSize.width * 2, j), CV_RGB(0, 255, 0));

					cvShowImage("rectified", pair);
					if (cvWaitKey() == 27)
						break;
				}
				cvReleaseImage(&img1);
				cvReleaseImage(&img2);
			}

			// release image
			cvReleaseMat(&img1r);
			cvReleaseMat(&img2r);
		}
	}
};


int main(void)
{
	IplImage* img1 = cvLoadImage("image/test5_L.jpg");
	IplImage* img2 = cvLoadImage("image/test5_R.jpg");
	IplImage* out1 = cvCloneImage(img1);
	IplImage* out2 = cvCloneImage(img2); 
	CvSize imageSize = cvGetSize(img1);

	CvMat* mx1 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	CvMat* my1 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	CvMat* mx2 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	CvMat* my2 = cvCreateMat(imageSize.height, imageSize.width, CV_32F);
	StereoCalibrator calibrator(21, 14);
	calibrator.StereoCalib("list2.txt", mx1, my1, mx2, my2, 1);

	cvRemap(img1, out1, mx1, my1);
	cvRemap(img2, out2, mx2, my2);
	cvNamedWindow("show1");
	cvNamedWindow("show2");
	cvShowImage("show1", out1);
	cvShowImage("show2", out2);
	cvvWaitKey(0);

	cvSaveImage("image/a.jpg", out1);
	cvSaveImage("image/b.jpg", out2);

	cvSave("mx1.xml", mx1);
	cvSave("my1.xml", my1);
	cvSave("mx2.xml", mx2);
	cvSave("my2.xml", my2);

	cvDestroyAllWindows();
	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvReleaseImage(&out1);
	cvReleaseImage(&out2);

	cvReleaseMat(&mx1);
	cvReleaseMat(&my1);
	cvReleaseMat(&mx2);
	cvReleaseMat(&my2);

	return 0;
}

#endif