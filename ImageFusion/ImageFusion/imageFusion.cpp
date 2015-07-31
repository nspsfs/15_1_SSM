#include "imageFusion.h"
#include <chrono>

Fusion::Fusion()
{
	img1 = NULL;
	img2 = NULL;
	img1G = NULL;
	img2G = NULL;
	sml1 = NULL;
	sml2 = NULL;
	result = NULL;
}

Fusion::~Fusion()
{
	sml1.release();
	sml2.release();
	result.release();
	img1.release();
	img2.release();
	img1G.release();
	img2G.release();
	result.release();
}

void Fusion::sml(IplImage** dst)
{
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	//#pragma omp parallel for schedule(dynamic)
	for (int j = 1; j < img1G.size().width - 1; j++)
	{
		for (int i = 1; i < img1G.size().height - 1; i++)
		{
			sml1.at<uchar>(i, j) = abs(2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i - 1, j) - img1G.at<uchar>(i + 1, j)) + abs(2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i, j - 1) - img1G.at<uchar>(i, j + 1));
			sml2.at<uchar>(i, j) = abs(2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i - 1, j) - img2G.at<uchar>(i + 1, j)) + abs(2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i, j - 1) - img2G.at<uchar>(i, j + 1));
			if (sml1.at<uchar>(i, j) > sml2.at<uchar>(i, j))
			{
				for (k = 0; k < 3; k++)
				{
					result.at<Vec3b>(i, j)[k] = img1.at<Vec3b>(i, j)[k];
				}
			}
			else
			{
				for (k = 0; k < 3; k++)
				{
					result.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
				}
			}
		}
	}

	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "SML 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	temp = result;
	*dst = cvCloneImage(&temp);
	return;
}

void Fusion::segsml(IplImage** dst)
{
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	int N = 1;
	int smlCom1;
	int smlCom2;
	int row;
	int col;
	int compare[7000] = { 0, };
	int freq[7000] = { 0, };
	//SML 만드는 부분
	temp = img1;
	IplImage* imgSegSrc = cvCloneImage(&temp);
	CvSize size = cvGetSize(imgSegSrc);
	CvMat* markers = cvCreateMat(size.height, size.width, CV_32SC1);
	cvSetZero(markers);
	int seedNum = 1;
	int seedStep = size.width / 30;
	//#pragma omp parallel for schedu
	for (row = seedStep; row < size.height; row += seedStep) {
		int* ptr = (int*)(markers->data.ptr + row * markers->step);
		for (col = seedStep; col < size.width; col += seedStep) {
			ptr[col] = seedNum;
			seedNum++;
		}
	}

	// watershed 알고리즘 실행
	cvWatershed(imgSegSrc, markers);



	seedStep /= 15;
	//Mat markersMat(markers);
	//#pragma omp parallel for schedule(dynamic)
	//for (i = 0; i < markers->height; i++) {
	//	int* ptrMarkers = (int*)(markers->data.ptr + i*markers->step);
	//	//uchar* dst = (uchar*)(img2->imageData + i*img2->widthStep);
	//	//uchar* ptr = (uchar*)(img->imageData + i* img->widthStep);
	//	for (j = 0; j < markers->width; j++) {
	//		//int idx = ptrMarkers[j];
	//		smlCom1 = ((2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i - 1, j) - img1G.at<uchar>(i + 1, j))*(2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i - 1, j) - img1G.at<uchar>(i + 1, j))) + ((2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i, j - 1) - img1G.at<uchar>(i, j + 1))*(2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i, j - 1) - img1G.at<uchar>(i, j + 1)));
	//		smlCom2 = ((2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i - 1, j) - img2G.at<uchar>(i + 1, j))*(2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i - 1, j) - img2G.at<uchar>(i + 1, j))) + ((2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i, j - 1) - img2G.at<uchar>(i, j + 1))*(2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i, j - 1) - img2G.at<uchar>(i, j + 1)));
	//		if (smlCom1 > smlCom2)
	//		{
	//			compare[ptrMarkers[j]] = true;
	//		}
	//		//if (idx == -1)
	//		//	dst[j * 3] = dst[j * 3 + 1] = dst[j * 3 + 2] = (uchar)255;
	//		//else if (idx < -1 || idx > seedNum)
	//		//	dst[j * 3 + 0] = dst[j * 3 + 1] = dst[j * 3 + 2] = (uchar)0;
	//		//else {
	//		//	dst[j * 3 + 0] = ptr[j * 3 + 0];
	//		//	dst[j * 3 + 1] = ptr[j * 3 + 1];
	//		//	dst[j * 3 + 2] = ptr[j * 3 + 2];
	//	}
	//}
	for (int row = seedStep; row < size.height; row += seedStep)
	{
		int* ptrptr = (int*)(markers->data.ptr + row * markers->step);
		for (int col = seedStep; col < size.width; col += seedStep)
		{
			if (row - 1 >= 0 && col - 1 >= 0 && row + 1 < size.height&&col + 1 < size.width)
			{
				smlCom1 = ((2 * img1G.at<uchar>(row, col) - img1G.at<uchar>(row - 1, col) - img1G.at<uchar>(row + 1, col))*(2 * img1G.at<uchar>(row, col) - img1G.at<uchar>(row - 1, col) - img1G.at<uchar>(row + 1, col))) + ((2 * img1G.at<uchar>(row, col) - img1G.at<uchar>(row, col - 1) - img1G.at<uchar>(row, col + 1))*(2 * img1G.at<uchar>(row, col) - img1G.at<uchar>(row, col - 1) - img1G.at<uchar>(row, col + 1)));
				smlCom2 = ((2 * img2G.at<uchar>(row, col) - img2G.at<uchar>(row - 1, col) - img2G.at<uchar>(row + 1, col))*(2 * img2G.at<uchar>(row, col) - img2G.at<uchar>(row - 1, col) - img2G.at<uchar>(row + 1, col))) + ((2 * img2G.at<uchar>(row, col) - img2G.at<uchar>(row, col - 1) - img2G.at<uchar>(row, col + 1))*(2 * img2G.at<uchar>(row, col) - img2G.at<uchar>(row, col - 1) - img2G.at<uchar>(row, col + 1)));
				if (smlCom1 > smlCom2)
				{
					//cout << (int)cvGet2D(markers,row,col) << endl;
					//i = (int)cvGetReal2D(&markers, row, col);//cvmGet(markers, row, col);
					//i = markersMat.at<int>(row, col);
					//compare[markersMat.at<uchar>(col, row)] = true;
					if (ptrptr[col] > 0)
					{
						compare[ptrptr[col]]++;
						freq[ptrptr[col]]++;
					}
					/*if (i > 0 && compare[i] == false)
					{
					compare[i] = true;
					}*/
				}
				else
				{
					if (ptrptr[col] > 0)
					{
						freq[ptrptr[col]]++;
					}
				}
			}
		}
	}


	//for (j = seedStep; j < img1G.size().width; j++)
	//{
	//	for (i = 0; i < img1G.size().height; i++)
	//	{
	//		
	//		//sml1.at<uchar>(i, j) = abs(2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i - N, j) - img1G.at<uchar>(i + N, j)) + abs(2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i, j - N) - img1G.at<uchar>(i, j + N));
	//		//sml2.at<uchar>(i, j) = abs(2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i - N, j) - img2G.at<uchar>(i + N, j)) + abs(2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i, j - N) - img2G.at<uchar>(i, j + N));
	//		//if (sml1.at<uchar>(i, j) > sml2.at<uchar>(i, j))
	//		//{
	//		//	msml.at<uchar>(i, j) = 255;
	//		//	/*for (k = 0; k < 3; k++)
	//		//	{
	//		//		result.at<Vec3b>(i, j)[k] = img1.at<Vec3b>(i, j)[k];
	//		//	}*/
	//		//}
	//		//else
	//		//{
	//		//	msml.at<uchar>(i, j) = 0;
	//		//	/*for (k = 0; k < 3; k++)
	//		//	{
	//		//		result.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
	//		//	}*/
	//		//}
	//	}
	//}

	//

	//
	//#pragma omp parallel for schedule(dynamic)
	for (i = 0; i < markers->height; i++)
	{
		int* ptrMarkers = (int*)(markers->data.ptr + i*markers->step);
		for (j = 0; j < markers->width; j++)
		{
			int idx = ptrMarkers[j];
			if (idx == -1)
			{
				if (j - 1 >= 0)
				{
					smlCom1 = -1;
				}
				else
				{
					smlCom1 = 0;
				}
				for (k = 0; k < 3; k++)
				{
					img1.at<Vec3b>(i, j)[k] = img1.at<Vec3b>(i, j + smlCom1)[k];
				}
			}
			else
			{
				/*if (compare[idx] == false)
				{
				for (k = 0; k < 3; k++)
				{
				img1.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
				}
				}*/
				if (freq[idx] != 0 && (double)(compare[idx] / freq[idx]) < 0.5)
				{
					for (k = 0; k < 3; k++)
					{
						img1.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
					}
				}
			}
		}
	}


	////////////////	for (i = 0; i < img1G.size().height; i++)
	////////////////	{
	////////////////		for (j = 0; j < img1G.size().width; j++)
	////////////////		{
	////////////////
	////////////////			if (markersMat.at<uchar>(i, j) == -1)
	////////////////			{
	////////////////				if (j - 1 >= 0)
	////////////////				{
	////////////////					smlCom1 = -1;
	////////////////				}
	////////////////				else
	////////////////				{
	////////////////					smlCom1 = 0;
	////////////////				}
	////////////////				for (k = 0; k < 3; k++)
	////////////////				{
	////////////////					img1.at<Vec3b>(i, j)[k] = img1.at<Vec3b>(i, j + smlCom1)[k];
	////////////////				}
	////////////////			}
	////////////////			else
	////////////////			{
	////////////////				if (compare[markersMat.at<uchar>(i, j)] == false)
	////////////////				{
	////////////////					for (k = 0; k < 3; k++)
	////////////////					{
	////////////////						img1.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
	////////////////					}
	////////////////				}
	////////////////			}
	////////////////		}
	////////////////	}

	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "SML 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

	temp = img1;
	*dst = cvCloneImage(&temp);

	return;

}

void Fusion::mmsml(IplImage** dst)
{
	Mat msml = img1G.clone();
	msml = Scalar(0);
	int N = 3;
	//SML 만드는 부분
	for (j = N; j < img1G.size().width - N; j++)
	{
		for (i = N; i < img1G.size().height - N; i++)
		{
			sml1.at<uchar>(i, j) = abs(2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i - N, j) - img1G.at<uchar>(i + N, j)) + abs(2 * img1G.at<uchar>(i, j) - img1G.at<uchar>(i, j - N) - img1G.at<uchar>(i, j + N));
			sml2.at<uchar>(i, j) = abs(2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i - N, j) - img2G.at<uchar>(i + N, j)) + abs(2 * img2G.at<uchar>(i, j) - img2G.at<uchar>(i, j - N) - img2G.at<uchar>(i, j + N));
			//if (sml1.at<uchar>(i, j) > sml2.at<uchar>(i, j))
			//{
			//	msml.at<uchar>(i, j) = 255;
			//	/*for (k = 0; k < 3; k++)
			//	{
			//		result.at<Vec3b>(i, j)[k] = img1.at<Vec3b>(i, j)[k];
			//	}*/
			//}
			//else
			//{
			//	msml.at<uchar>(i, j) = 0;
			//	/*for (k = 0; k < 3; k++)
			//	{
			//		result.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
			//	}*/
			//}
		}
	}

	//SML비교맵 만드는 부분

	for (j = N; j < img1G.size().width - N; j++)
	{
		for (i = N; i < img1G.size().height - N; i++)
		{
			if (sml1.at<uchar>(i, j) < sml2.at<uchar>(i, j))
			{
				msml.at<uchar>(i, j) = sml2.at<uchar>(i, j);
				for (k = N*(-1); k < N + 1; k++)
				{
					if (k != 0)
					{
						msml.at<uchar>(i, j) += sml2.at<uchar>(i + k, j);
						msml.at<uchar>(i, j) += sml2.at<uchar>(i, j + k);
					}
				}
				/*msml.at<uchar>(i, j) += sml2.at<uchar>(i-1, j);
				msml.at<uchar>(i, j) += sml2.at<uchar>(i+1, j);
				msml.at<uchar>(i, j) += sml2.at<uchar>(i, j-1);
				msml.at<uchar>(i, j) += sml2.at<uchar>(i, j+1);*/
			}
			else
			{
				msml.at<uchar>(i, j) = 0;
			}
		}
	}

	Mat morphology;
	Size size(5, 5);
	Mat rectKernel = getStructuringElement(MORPH_RECT, size);
	int iteations = 1;
	Point anchor(-1, -1);
	Mat erodeImage;
	erode(msml, erodeImage, rectKernel, anchor, iteations);
	imshow("erodeImage", erodeImage);

	Mat dilateImage;
	dilate(msml, dilateImage, rectKernel, anchor, iteations);
	imshow("dilateImage", dilateImage);

	imshow("msml", msml);

	temp = result;
	*dst = cvCloneImage(&temp);
	return;
}

void Fusion::setimg(IplImage* image1, IplImage* image2)
{
	if (image1 != NULL&&image2 != NULL)
	{
		img1 = cvarrToMat(image1);
		img2 = cvarrToMat(image2);
		cvtColor(img1, img1G, COLOR_BGR2GRAY);
		cvtColor(img2, img2G, COLOR_BGR2GRAY);

		sml1 = img1G.clone();
		sml2 = img2G.clone();
		sml1 = Scalar(0);
		sml2 = Scalar(0);
		result = img1.clone();
	}

	return;
}