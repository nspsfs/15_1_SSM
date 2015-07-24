#if 0

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

	Mat tenen1(img1g.size().height, img1g.size().width, CV_8UC1);
	Mat tenen2(img2g.size().height, img2g.size().width, CV_8UC1);
	Mat result = img1g.clone();

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	for (j = 1; j < img1g.size().width - 1; j++)
	{
		for (i = 1; i < img1g.size().height - 1; i++)
		{
			tenen1.at<uchar>(i, j) = sqrt(usquare(img1g.at<uchar>(i + 1, j - 1) + 2 * img1g.at<uchar>(i + 1, j) + img1g.at<uchar>(i + 1, j + 1) - (img1g.at<uchar>(i - 1, j - 1) + 2 * img1g.at<uchar>(i - 1, j) + img1g.at<uchar>(i - 1, j + 1))) + usquare(img1g.at<uchar>(i - 1, j - 1) + 2 * img1g.at<uchar>(i, j - 1) + img1g.at<uchar>(i + 1, j - 1) + img1g.at<uchar>(i - 1, j + 1) + 2 * img1g.at<uchar>(i, j + 1) + img1g.at<uchar>(i + 1, j + 1)));
			tenen2.at<uchar>(i, j) = sqrt(usquare(img2g.at<uchar>(i + 1, j - 1) + 2 * img2g.at<uchar>(i + 1, j) + img2g.at<uchar>(i + 1, j + 1) - (img2g.at<uchar>(i - 1, j - 1) + 2 * img2g.at<uchar>(i - 1, j) + img2g.at<uchar>(i - 1, j + 1))) + usquare(img2g.at<uchar>(i - 1, j - 1) + 2 * img2g.at<uchar>(i, j - 1) + img2g.at<uchar>(i + 1, j - 1) + img2g.at<uchar>(i - 1, j + 1) + 2 * img2g.at<uchar>(i, j + 1) + img2g.at<uchar>(i + 1, j + 1)));
			//tenen1.at<uchar>(i, j) = usquare(img1g.at<uchar>(i + 1, j) - img1g.at<uchar>(i, j)) + usquare(img1g.at<uchar>(i, j + 1) - img1g.at<uchar>(i, j));
			//tenen2.at<uchar>(i, j) = usquare(img2g.at<uchar>(i + 1, j) - img2g.at<uchar>(i, j)) + usquare(img2g.at<uchar>(i, j + 1) - img2g.at<uchar>(i, j));
			if (tenen1.at<uchar>(i, j) > tenen2.at<uchar>(i, j))
			{
				result.at<uchar>(i, j) = img1g.at<uchar>(i, j);
			}
			else
			{
				result.at<uchar>(i, j) = img2g.at<uchar>(i, j);
			}
		}
	}
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	imshow("img1", img1g);
	imshow("img2", img2g);
	imshow("tenen1", tenen1);
	imshow("tenen2", tenen2);
	imshow("result", result);
	imwrite("tenengrad.jpg", result);

	cvWaitKey(0);
}

#endif
