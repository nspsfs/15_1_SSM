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
	int pixelDiff = 10;
	if (imgL.empty() || imgR.empty() || disparity.empty())
	{
		cout << "disparity image와 Left&Right 이미지가 필요합니다.";
		return;
	}

	copyImgR = imgR.clone();
	if (disparity.channels() == 1)  {
		dispG = disparity.clone();
	}
	else  {
		cvtColor(disparity, dispG, CV_BGR2GRAY);
	}
	cvtColor(imgL, imgLG, CV_BGR2GRAY);
	cvtColor(copyImgR, imgRG, CV_BGR2GRAY);


	Mat cvtL2RC = imgR.clone();
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	int height = dispG.size().height;
	int width = dispG.size().width;
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < height; i++)
	{
		uchar* pDispG = (uchar*)(dispG.data + i*dispG.step);
		uchar* prtCvt2RC = (uchar*)(cvtL2RC.data + i*cvtL2RC.step);
		uchar* prtCopyImgR = (uchar*)(copyImgR.data + i*copyImgR.step);
		for (int j = 0; j < width; j++)
		{
			int tempVal = pDispG[j] / pixelDiff + j + 1;
			if (tempVal < width)
			{
				prtCvt2RC[3 * j] = prtCopyImgR[3 * tempVal];
				prtCvt2RC[3 * j + 1] = prtCopyImgR[3 * tempVal + 1];
				prtCvt2RC[3 * j + 2] = prtCopyImgR[3 * tempVal + 2];

				prtCopyImgR[3 * tempVal] = 0;
				prtCopyImgR[3 * tempVal + 1] = 0;
				prtCopyImgR[3 * tempVal + 2] = 0;
			}
		}
	}
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "시차변환1(보간 제외) 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	//convertImg = cvtL2RC.clone();
	//start = std::chrono::system_clock::now();
	//for (int i = 0; i < dispG.size().height; i++)
	//{
	//	for (int j = 0; j < dispG.size().width; j++)
	//	{
	//		if (convertImg.at<Vec3b>(i, j)[0] == 0)
	//		{
	//			for (int k = 0; k < 3; k++)
	//			{
	//				convertImg.at<Vec3b>(i, j)[k] = imgL.at<Vec3b>(i, j)[k];
	//			}
	//		}

	//	}
	//}
	//sec = std::chrono::system_clock::now() - start;
	//std::cout << "보간 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;
	////결과는  convertImg!!!!

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