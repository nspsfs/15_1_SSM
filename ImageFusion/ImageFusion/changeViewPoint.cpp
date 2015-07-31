//#include "stdafx.h"
#include "changeViewPoint.h"
#include <chrono>

CVP::CVP()
{

}

CVP::~CVP()
{
	disparity.release();
	imgL.release();
	imgR.release();
	dispG.release();
	imgLG.release();
	imgRG.release();
	cvtL2RC.release();
	cvtL2R.release();
	convertImg.release();
	copyImgR.release();
}

void CVP::runcvt(IplImage** dst)
{
	//int pixelDiff = imgL.size().height / MaxPixel;
	int pixelDiff = 1;
	if (imgL.empty() || imgR.empty() || disparity.empty())
	{
		cout << "disparity image와 Left&Right 이미지가 필요합니다.";
		return;
	}

	copyImgR = imgR.clone();
	int tempVal, i, j, k;
	if (disparity.channels() == 1)  {
		dispG = disparity.clone();
	}
	else  {
		cvtColor(disparity, dispG, CV_BGR2GRAY);
	}
	cvtColor(imgL, imgLG, CV_BGR2GRAY);
	cvtColor(copyImgR, imgRG, CV_BGR2GRAY);

	//Mat cvtL2R = dispG.clone();
	Mat cvtL2RC = imgL.clone();
	//cvtL2RC = Scalar(0, 0, 0);
	//cvtL2R = Scalar(0);
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
//#pragma omp parallel for schedule(dynamic)
	for (i = 0; i < dispG.size().height; i++)
	{
		for (j = 0; j < dispG.size().width; j++)
		{
			tempVal = dispG.at<uchar>(i, j);
			if (j + (tempVal / pixelDiff) + 1 < dispG.size().width)
				//if (j - (tempVal / MaxPixel) > 0)
			{
				//cvtL2R.at<uchar>(i, j) = imgRG.at<uchar>(i, j + (tempVal / pixelDiff) + 1);
				cvtL2RC.at<Vec3b>(i, j)[0] = copyImgR.at<Vec3b>(i, j + (tempVal / pixelDiff) + 1)[0];
				copyImgR.at<Vec3b>(i, j + (tempVal / pixelDiff) + 1)[0] = 0;
				cvtL2RC.at<Vec3b>(i, j)[1] = copyImgR.at<Vec3b>(i, j + (tempVal / pixelDiff) + 1)[1];
				copyImgR.at<Vec3b>(i, j + (tempVal / pixelDiff) + 1)[1] = 0;
				cvtL2RC.at<Vec3b>(i, j)[2] = copyImgR.at<Vec3b>(i, j + (tempVal / pixelDiff) + 1)[2];
				copyImgR.at<Vec3b>(i, j + (tempVal / pixelDiff) + 1)[2] = 0;
			}
		}
	}
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "시차변환1(보간 제외) 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	//convertImg = cvtL2RC.clone();
	//start = std::chrono::system_clock::now();
//#pragma omp parallel for schedu
//	for (i = 0; i < dispG.size().height; i++)
//	{
//		for (j = 0; j < dispG.size().width; j++)
//		{
//			if (convertImg.at<Vec3b>(i, j)[0] == 0)
//			{
//				for (k = 0; k < 3; k++)
//				{
//					convertImg.at<Vec3b>(i, j)[k] = imgL.at<Vec3b>(i, j)[k];
//				}
//			}
//
//		}
//	}
//	sec = std::chrono::system_clock::now() - start;
//	std::cout << "보간 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;
	//결과는  convertImg!!!!

	temp = cvtL2RC;
	*dst = cvCloneImage(&temp);
	return;
}

void CVP::setimg(IplImage* dis, IplImage* left, IplImage* right)
{
	if (dis != NULL&&left != NULL&&right != NULL)
	{
		disparity = cvarrToMat(dis);
		imgL = cvarrToMat(left);
		imgR = cvarrToMat(right);
	}

	return;
}