#include <iostream>
#include "imageFusion.h"
#include "changeViewPoint.h"

#define MFimage1 "image/MFtest1_1.jpg"
#define MFimage2 "image/MFtest1_2.jpg"

#define CVPdisp "image/venus/disp2.pgm"
#define CVPleft "image/venus/im2.ppm"
#define CVPright "image/venus/im6.ppm"


using namespace std;

void getDisparityMapAndMultifocusVideos(const char* path1, const char* path2, int delayFrame)
{
	clock_t start_time, end_time;
	CvMat* mx1 = (CvMat*)cvLoad("mx1.xml");
	CvMat* my1 = (CvMat*)cvLoad("my1.xml");
	CvMat* mx2 = (CvMat*)cvLoad("mx2.xml");
	CvMat* my2 = (CvMat*)cvLoad("my2.xml");

	CvCapture* capture1 = cvCreateFileCapture(path1);
	CvCapture* capture2 = cvCreateFileCapture(path2);
	IplImage* leftImage = cvQueryFrame(capture1);
	IplImage* rightImage = cvQueryFrame(capture2);
	CvSize size = cvGetSize(leftImage);
	CvSize RotateSize = cvSize(size.height, size.width);
	IplImage* leftImageRotation = cvCreateImage(RotateSize, IPL_DEPTH_8U, 3);
	IplImage* rightImageRotation = cvCreateImage(RotateSize, IPL_DEPTH_8U, 3);
	cvFlip(leftImage, leftImage, 0);
	cvFlip(leftImage, leftImage, 1);
	cvTranspose(leftImage, leftImageRotation);
	cvTranspose(rightImage, rightImageRotation);

	//DisparityMapMaker maker;
	Fusion fusion;
	CVP cvp;
	/*maker.initialSetting(8, 1, 60);
	maker.setImage(leftImageRotation, rightImageRotation);

	IplImage* disparityMap = maker.getSegmentBasedDisparityMap();
	CvSize disparityMapSize = cvGetSize(disparityMap);
	IplImage* preDisparityMap1 = cvCloneImage(disparityMap);
	IplImage* preDisparityMap2 = cvCloneImage(disparityMap);
	IplImage* preDisparityMap3 = cvCloneImage(disparityMap);
	IplImage* filtered1 = cvCreateImage(cvGetSize(disparityMap), IPL_DEPTH_8U, 1);
	IplImage* filtered2 = cvCreateImage(cvGetSize(disparityMap), IPL_DEPTH_8U, 1);
	IplImage* filteredDisaprityMap = cvCreateImage(cvGetSize(disparityMap), IPL_DEPTH_8U, 1);
	IplImage* filteredDisaprityMapRotation = cvCreateImage(cvSize(disparityMapSize.height, disparityMapSize.width), IPL_DEPTH_8U, 1);

	IplImage* resizedDisparityMap = cvCreateImage(RotateSize, IPL_DEPTH_8U, 1);*/
	IplImage* convertImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
	IplImage* multiFocusImage = cvCreateImage(size, IPL_DEPTH_8U, 3);

	IplImage* tempOut1 = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* tempOut2 = cvCreateImage(size, IPL_DEPTH_8U, 3);

	int fourcc = CV_FOURCC('M', 'J', 'P', 'G');
	CvVideoWriter* disparityMapVideoOut = cvCreateVideoWriter("dspMap.avi", fourcc, 24, size, 0);
	CvVideoWriter* multifocusVideoOut = cvCreateVideoWriter("multifocus.avi", fourcc, 24, size, 1);

	// 딜레이된 프레임을 제거함
	int nCutFrame = delayFrame;
	for (int i = 0; i < nCutFrame; i++) {
		rightImage = cvQueryFrame(capture1);
	}


	while (1) {
		start_time = clock();

		//getting images
		leftImage = cvQueryFrame(capture1);
		rightImage = cvQueryFrame(capture2);
		if (!leftImage || !rightImage)
			break;
		cvFlip(leftImage, leftImage, 0);
		cvFlip(leftImage, leftImage, 1);
		cvTranspose(leftImage, leftImageRotation);
		cvTranspose(rightImage, rightImageRotation);


		//disparity map image generation
		//maker.setImage(leftImageRotation, rightImageRotation);
		//preDisparityMap3 = cvCloneImage(preDisparityMap2);
		//preDisparityMap2 = cvCloneImage(preDisparityMap1);
		//preDisparityMap1 = cvCloneImage(disparityMap);
		//disparityMap = maker.getSegmentBasedDisparityMap();
		//cvAddWeighted(preDisparityMap1, 0.25, disparityMap, 0.25, 0, filtered1);
		//cvAddWeighted(preDisparityMap2, 0.25, preDisparityMap3, 0.25, 0, filtered2);
		//cvAddWeighted(filtered1, 1, filtered2, 1, 0, filteredDisaprityMap);
		//cvResize(filteredDisaprityMap, resizedDisparityMap);


		// getting view point changed image and focus fusion
		//cvp.setImg(resizedDisparityMap, leftImageRotation, rightImageRotation);
		//cvp.runcvt(&convertImage);
		fusion.setImg(leftImageRotation, rightImageRotation);
		fusion.backupsml(&multiFocusImage);


		//file save
//		cvTranspose(resizedDisparityMap, tempOut1);
		cvWriteFrame(disparityMapVideoOut, tempOut1);
		cvTranspose(multiFocusImage, tempOut2);
		cvWriteFrame(multifocusVideoOut, tempOut2);


		cvWaitKey(1);
		end_time = clock();
		cout << (double)1000 / (end_time - start_time) << "   FPS" << endl;

	}
	cvReleaseVideoWriter(&disparityMapVideoOut);
	cvReleaseVideoWriter(&multifocusVideoOut);

	cvReleaseImage(&tempOut1);
	cvReleaseImage(&tempOut2);


//	cvReleaseImage(&resizedDisparityMap);

	//cvReleaseImage(&filtered1);
	//cvReleaseImage(&filtered2);
	//cvReleaseImage(&filteredDisaprityMap);
	//cvReleaseImage(&filteredDisaprityMapRotation);
	//cvReleaseImage(&preDisparityMap1);
	//cvReleaseImage(&preDisparityMap2);
	//cvReleaseImage(&preDisparityMap3);
	//cvReleaseImage(&disparityMap);

	cvReleaseImage(&leftImageRotation);
	cvReleaseImage(&rightImageRotation);

	cvReleaseCapture(&capture1);
	cvReleaseCapture(&capture2);

	cvReleaseMat(&mx1);
	cvReleaseMat(&my1);
	cvReleaseMat(&mx2);
	cvReleaseMat(&my2);

	return;
}

int main()
{
	//CVP cvp;
	//IplImage* disparity = cvLoadImage(CVPdisp,0);
	//IplImage* rightImage = cvLoadImage(CVPright);
	//IplImage* leftImage = cvLoadImage(CVPleft);
	////IplImage* disparity = cvLoadImage("disp.png",0);
	////IplImage* rightImage = cvLoadImage("right.png");
	////IplImage* leftImage = cvLoadImage("left.png");
	//IplImage* convertImage = NULL;
	//IplImage* convertInterpol = NULL;

	//cvp.setImg(disparity, leftImage, rightImage);
	//cvp.runcvt(&convertImage);
	//cvSaveImage("결과사진/venus/CVPconvertImage.jpg", convertImage);

	//CVP cvp2;
	//cvp2.setImg(disparity, leftImage, rightImage);
	//cvp2.runcvtinterpol(&convertInterpol);
	//cvShowImage("leftImage", leftImage);
	//cvShowImage("rightImage", rightImage);
	//cvShowImage("convertImage", convertImage);
	//cvSaveImage("결과사진/venus/CVPdisp.jpg", disparity);
	//cvSaveImage("결과사진/venus/CVPleftImage.jpg", leftImage);
	//cvSaveImage("결과사진/venus/CVPrightImage.jpg", rightImage);
	//cvSaveImage("결과사진/venus/CVPconvertInterpolationImage.jpg", convertInterpol);


	//Fusion fusion;
	//IplImage* image1 = cvLoadImage(MFimage1);
	//IplImage* image2 = cvLoadImage(MFimage2);
	//IplImage* multiFocusImage=NULL;

	//fusion.setImg(image1, image2);
	////fusion.setImg(convertImage, leftImage);
	//fusion.sml(&multiFocusImage);

	//cvShowImage("image1", image1);
	//cvShowImage("image2", image2);
	//cvShowImage("MultiFocusImage", multiFocusImage);
	//cvSaveImage("MFImg.jpg", multiFocusImage);


	//cvWaitKey(0);
	//return 0;

	const char* path1 = "MFTestVideo4_1.mp4";
	const char* path2 = "MFTestVideo4_2.mp4";

	//const char* path1 = "MFTestVideo2_1.mp4";
	//const char* path2 = "MFTestVideo2_2.mp4";
	//const char* path1 = "LEFT.MOV";
	//const char* path2 = "RIGHT.MOV";
	//const char* path1 = "left.avi";
	//const char* path2 = "right.avi";

	getDisparityMapAndMultifocusVideos(path1, path2, 0);

	cout << "finished." << endl;

	return 0;
}