#if 0

#include <opencv2\core\core.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\contrib\contrib.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <conio.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv2\legacy\legacy.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
	Mat disparity = imread("image/test_disparity.jpg");
	Mat imgL = imread("image/test_L.jpg");
	Mat imgR = imread("image/test_R.jpg");
	Mat dispG, imgLG, imgRG;

	int tempVal, i, j, k;
	cvtColor(disparity, dispG, CV_BGR2GRAY);
	cvtColor(imgL, imgLG, CV_BGR2GRAY);
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
			if (j + (tempVal / 16)+1 < dispG.size().width)
			{
				cvtR2L.at<uchar>(i, j) = imgLG.at<uchar>(i, j + (tempVal / 16)+1);
				cvtR2LC.at<Vec3b>(i, j)[0] = imgL.at<Vec3b>(i, j + (tempVal / 16)+1)[0];
				imgL.at<Vec3b>(i, j + (tempVal / 16)+1)[0]=0;
				cvtR2LC.at<Vec3b>(i, j)[1] = imgL.at<Vec3b>(i, j + (tempVal / 16)+1)[1];
				imgL.at<Vec3b>(i, j + (tempVal / 16)+1)[1] = 0;
				cvtR2LC.at<Vec3b>(i, j)[2] = imgL.at<Vec3b>(i, j + (tempVal / 16)+1)[2];
				imgL.at<Vec3b>(i, j + (tempVal / 16)+1)[2] = 0;
			}
		}
	}

	Mat interpol = cvtR2LC.clone();
	for (i = 0; i < dispG.size().height; i++)
	{
		for (j = 0; j < dispG.size().width; j++)
		{
			if (interpol.at<Vec3b>(i, j)[0] == 0)
			{
				for (k = 0; k < 3; k++)
				{
					interpol.at<Vec3b>(i, j)[k] = imgR.at<Vec3b>(i, j)[k];
				}
			}
			
		}
	}


	imshow("Disp", dispG);
	imshow("Left", imgL);
	imshow("Right", imgR);
	imshow("convert", cvtR2L);
	imshow("convertColor", cvtR2LC);
	imshow("interpolation", interpol);

	imwrite("cvt.jpg", interpol);

	waitKey(0);

	return 0;

}

#endif