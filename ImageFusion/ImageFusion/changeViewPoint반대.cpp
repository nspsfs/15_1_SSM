#include "changeViewPoint.h"

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
	cvtR2LC.release();
	cvtR2L.release();
	convertImg.release();
	copyImgL.release();
}

void CVP::runcvt(IplImage** dst)
{
	if (imgL.empty()||imgR.empty()||disparity.empty())
	{
		cout << "disparity image와 Left&Right 이미지가 필요합니다.";
		return;
	}

	copyImgL = imgL.clone();
	int tempVal, i, j, k;
	cvtColor(disparity, dispG, CV_BGR2GRAY);
	cvtColor(copyImgL, imgLG, CV_BGR2GRAY);
	cvtColor(imgR, imgRG, CV_BGR2GRAY);

	Mat cvtR2L = dispG.clone();
	Mat cvtR2LC = disparity.clone();
	cvtR2LC = Scalar(0, 0, 0);
	cvtR2L = Scalar(0);

	for (i = 0; i < dispG.size().height; i++)
	{
		for (j = 0; j < dispG.size().width; j++)
		{
			tempVal = dispG.at<uchar>(i, j);
			if (j + (tempVal / 16) + 1 < dispG.size().width)
			{
				cvtR2L.at<uchar>(i, j) = imgLG.at<uchar>(i, j + (tempVal / 16) + 1);
				cvtR2LC.at<Vec3b>(i, j)[0] = copyImgL.at<Vec3b>(i, j + (tempVal / 16) + 1)[0];
				copyImgL.at<Vec3b>(i, j + (tempVal / 16) + 1)[0] = 0;
				cvtR2LC.at<Vec3b>(i, j)[1] = copyImgL.at<Vec3b>(i, j + (tempVal / 16) + 1)[1];
				copyImgL.at<Vec3b>(i, j + (tempVal / 16) + 1)[1] = 0;
				cvtR2LC.at<Vec3b>(i, j)[2] = copyImgL.at<Vec3b>(i, j + (tempVal / 16) + 1)[2];
				copyImgL.at<Vec3b>(i, j + (tempVal / 16) + 1)[2] = 0;
			}
		}
	}
	
	convertImg = cvtR2LC.clone();
	for (i = 0; i < dispG.size().height; i++)
	{
		for (j = 0; j < dispG.size().width; j++)
		{
			if (convertImg.at<Vec3b>(i, j)[0] == 0)
			{
				for (k = 0; k < 3; k++)
				{
					convertImg.at<Vec3b>(i, j)[k] = imgR.at<Vec3b>(i, j)[k];
				}
			}

		}
	}

	//결과는  convertImg!!!!

	temp = convertImg;
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