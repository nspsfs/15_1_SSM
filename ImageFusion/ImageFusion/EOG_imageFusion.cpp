#if 1

#include "opencv2\opencv.hpp"
#include "opencv2\highgui\highgui.hpp"
#include <iostream>
#include <chrono>

using namespace cv;

uchar usquare(uchar data)
{
	data *= data;
	return data;
}

int main()
{
	Mat img1 = imread("image/test3_1.bmp");
	Mat img2 = imread("image/test3_2.bmp");
	Mat img1g;
	Mat img2g;

	int i, j;

	cvtColor(img1, img1g, COLOR_BGR2GRAY);
	cvtColor(img2, img2g, COLOR_BGR2GRAY);

	Mat eog1(img1g.size().height, img1g.size().width, CV_8UC1);
	Mat eog2(img2g.size().height, img2g.size().width, CV_8UC1);
	Mat result = img1g.clone();

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	for (j = 1; j < img1g.size().width - 1; j++)
	{
		for (i = 1; i < img1g.size().height - 1; i++)
		{
			//eog1.at<uchar>(i, j) = abs(2 * img1g.at<uchar>(i, j) - img1g.at<uchar>(i - 1, j) - img1g.at < uchar>(i + 1, j)) + abs(2 * img1g.at<uchar>(i, j) - img1g.at<uchar>(i, j - 1) - img1g.at<uchar>(i, j + 1));
			//eog2.at<uchar>(i, j) = abs(2 * img2g.at<uchar>(i, j) - img2g.at<uchar>(i - 1, j) - img2g.at < uchar>(i + 1, j)) + abs(2 * img2g.at<uchar>(i, j) - img2g.at<uchar>(i, j - 1) - img2g.at<uchar>(i, j + 1));
			eog1.at<uchar>(i, j) = usquare(img1g.at<uchar>(i + 1, j) - img1g.at<uchar>(i, j)) + usquare(img1g.at<uchar>(i, j + 1) - img1g.at<uchar>(i, j));
			eog2.at<uchar>(i, j) = usquare(img2g.at<uchar>(i + 1, j) - img2g.at<uchar>(i, j)) + usquare(img2g.at<uchar>(i, j + 1) - img2g.at<uchar>(i, j));
			if (eog1.at<uchar>(i, j) > eog2.at<uchar>(i, j))
			{
				result.at<uchar>(i, j) = img1g.at<uchar>(i, j);
			}
			elsek
			{
				result.at<uchar>(i, j) = img2g.at<uchar>(i, j);
			}
		}
	}
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	imshow("img1", img1g);
	imshow("img2", img2g);
	imshow("eog1", eog1);
	imshow("eog2", eog2);
	imshow("result", result);
	imwrite("EOG.jpg", result);

	cvWaitKey(0);
}

#endif