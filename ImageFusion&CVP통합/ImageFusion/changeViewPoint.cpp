#include "changeViewPoint.h"
#include <chrono>


CVP::CVP()
{
	disparity = NULL;
	imgR = NULL;
	imgL = NULL;
}

CVP::~CVP()
{
	cvReleaseImage(&disparity);
	cvReleaseImage(&imgR);
	cvReleaseImage(&imgL);
}

void CVP::setImg(IplImage* dis, IplImage* left, IplImage* right)
{
	if (dis != NULL&&left != NULL&&right != NULL)
	{
		disparity = cvCloneImage(dis);
		imgR = cvCloneImage(right);
		imgL = cvCloneImage(left);
	}
	else
	{
		cout << "No input Image!!!" << endl;
	}

	return;
}

void CVP::runcvt(IplImage** dst)
{
	CvSize size = cvGetSize(imgR);
	IplImage* disparityGray = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvCvtColor(disparity, disparityGray, CV_RGB2GRAY);
	IplImage* copyImgR = cvCloneImage(imgR);
	IplImage* cvtR2L = cvCloneImage(imgL);
	//cvSetZero(cvtR2L);
	IplImage* cvtR2LGray = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvCvtColor(imgL, cvtR2LGray, CV_RGB2GRAY);
	
	IplImage* imgRGray = cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvCvtColor(imgR, imgRGray, CV_RGB2GRAY);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < size.height; i++)
	{
		uchar* pDispG = (uchar*)(disparityGray->imageData + i*disparityGray->widthStep);
		uchar* prtCvtR2L = (uchar*)(cvtR2L->imageData + i*cvtR2L->widthStep);
		uchar* prtCvtR2LGray = (uchar*)(cvtR2LGray->imageData + i*cvtR2LGray->widthStep);
		uchar* prtCopyImgR = (uchar*)(copyImgR->imageData + i*copyImgR->widthStep);
		uchar* prtimgRGray = (uchar*)(imgRGray->imageData + i*imgRGray->widthStep);
		//for (int j = 0; j < size.width; j++)
		for (int j = size.width-1; j >=0; j--)
		{
			//int tempVal = pDispG[j] / PixelDiff - j + 1;
			int tempDisp = pDispG[j];
			int tempVal = j;
			tempVal -= tempDisp / PixelDiff;
			//if (tempVal < size.width)
			if (tempVal >= 0&&prtimgRGray[tempVal]!=0)
			{
				prtCvtR2LGray[j] = prtimgRGray[tempVal];
				prtimgRGray[tempVal] = 0;

				prtCvtR2L[3 * j] = prtCopyImgR[3 * tempVal];
				prtCvtR2L[3 * j + 1] = prtCopyImgR[3 * tempVal + 1];
				prtCvtR2L[3 * j + 2] = prtCopyImgR[3 * tempVal + 2];

				//prtCopyImgR[3 * tempVal] = 0;
				//prtCopyImgR[3 * tempVal + 1] = 0;
				//prtCopyImgR[3 * tempVal + 2] = 0;
			}
		}
	}

	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "시차변환 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	*dst = cvCloneImage(cvtR2L);
	return;

} 