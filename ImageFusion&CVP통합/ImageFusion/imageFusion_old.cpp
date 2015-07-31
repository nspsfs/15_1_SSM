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


void Fusion::setimg(IplImage* image1, IplImage* image2)
{
	if (image1 != NULL&&image2 != NULL) {
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

void Fusion::sml(IplImage** dst)
{

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	int N = 1;
	bool compare[7000] = { false, };

	//SML 만드는 부분


	// watershed 알고리즘 실행
	temp = img1;
	IplImage* imgSegSrc = cvCloneImage(&temp);
	CvSize size = cvGetSize(imgSegSrc);
	CvMat* markers = cvCreateMat(size.height, size.width, CV_32SC1);
	cvSetZero(markers);
	int seedNum = 1;
	int seedStep = size.width / 10;
	for (int row = seedStep; row < size.height; row += seedStep) {
		int* ptr = (int*)(markers->data.ptr + row * markers->step);
		for (int col = seedStep; col < size.width; col += seedStep) {
			ptr[col] = seedNum;
			seedNum++;
		}
	}
	cvWatershed(imgSegSrc, markers);


	int smlCom1;
	int smlCom2;
	//#pragma omp parallel for schedule(dynamic)
	for (int row = seedStep; row < size.height - 1; row += seedStep)

	{
		int* ptrMarkers = (int*)(markers->data.ptr + row * markers->step);

		int* ptrImg1Gup = (int*)(img1G.data + (row - 1)*img1G.step);
		int* ptrImg1Gmid = (int*)(img1G.data + row*img1G.step);
		int* ptrImg1Gdown = (int*)(img1G.data + (row + 1)*img1G.step);

		int* ptrImg2Gup = (int*)(img2G.data + (row - 1)*img2G.step);
		int* ptrImg2Gmid = (int*)(img2G.data + row*img2G.step);
		int* ptrImg2Gdown = (int*)(img2G.data + (row + 1)*img2G.step);
		for (int col = seedStep; col < size.width - 1; col += seedStep)
		{
			int a1 = abs(2 * ptrImg1Gmid[col] - ptrImg1Gup[col] - ptrImg1Gdown[col]);
			int b1 = abs(2 * ptrImg1Gmid[col] - ptrImg1Gmid[col - 1] - ptrImg1Gmid[col + 1]);
			int a2 = abs(2 * ptrImg2Gmid[col] - ptrImg2Gup[col] - ptrImg2Gdown[col]);
			int b2 = abs(2 * ptrImg2Gmid[col] - ptrImg2Gmid[col - 1] - ptrImg2Gmid[col + 1]);

			smlCom1 = a1 + b1;
			smlCom2 = a2 + b2;

			if (smlCom1 > smlCom2)
			{
				if (ptrMarkers[col] > 0)
				{
					compare[ptrMarkers[col]] = true;
				}
			}
		}
	}


	for (int i = 0; i < markers->height; i++)
	{
		int* ptrMarkers = (int*)(markers->data.ptr + i*markers->step);
		char* ptrImg1 = (char*)(img1.data + i*img1.step);
		char* ptrImg2 = (char*)(img2.data + i*img2.step);

		for (int j = 0; j < markers->width; j++)
		{
			// 세그먼트 사이의 선 예외처리
			if (ptrMarkers[j] == -1)
			{
				if (j - 1 >= 0)
				{
					for (int k = 0; k < 3; k++) {
						ptrImg1[3 * j + k] = ptrImg1[3 * (j - 1) + k];
					}
				}
			}
			else
			{
				if (compare[ptrMarkers[j]] == false)
				{
					for (int k = 0; k < 3; k++)
					{
						//img1.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
						ptrImg1[3 * j + k] = ptrImg2[3 * j + k];
					}
				}
			}
		}
	}
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "SML 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;


	temp = img1;
	*dst = cvCloneImage(&temp);

	return;
}

void Fusion::segsml(IplImage** dst)
{

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	int N = 1;
	//bool compare[7000] = { false, };
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	int max1[7000] = { 0, };
	int max2[7000] = { 0, };
	int freq[7000] = { 0, };
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////

	//SML 만드는 부분


	// watershed 알고리즘 실행
	temp = img1;
	IplImage* imgSegSrc = cvCloneImage(&temp);
	CvSize size = cvGetSize(imgSegSrc);
	CvMat* markers = cvCreateMat(size.height, size.width, CV_32SC1);
	cvSetZero(markers);
	int seedNum = 1;
	int seedStep = size.width / 10;
	for (int row = seedStep; row < size.height; row += seedStep) {
		int* ptr = (int*)(markers->data.ptr + row * markers->step);
		for (int col = seedStep; col < size.width; col += seedStep) {
			ptr[col] = seedNum;
			seedNum++;
		}
	}
	cvWatershed(imgSegSrc, markers);


	int smlCom1;
	int smlCom2;
	//#pragma omp parallel for schedule(dynamic)
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	//for (int row = seedStep; row < size.height - 1; row += seedStep)
	for (int row = 0; row < size.height; row++)
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
	{
		int* ptrMarkers = (int*)(markers->data.ptr + row * markers->step);

		int* ptrImg1Gup = (int*)(img1G.data + (row - 1)*img1G.step);
		int* ptrImg1Gmid = (int*)(img1G.data + row*img1G.step);
		int* ptrImg1Gdown = (int*)(img1G.data + (row + 1)*img1G.step);

		int* ptrImg2Gup = (int*)(img2G.data + (row - 1)*img2G.step);
		int* ptrImg2Gmid = (int*)(img2G.data + row*img2G.step);
		int* ptrImg2Gdown = (int*)(img2G.data + (row + 1)*img2G.step);
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		//for (int col = seedStep; col < size.width - 1; col += seedStep)
		for (int col = 0; col < size.width; col++)
			///////////////////////////////////////////////////
			///////////////////////////////////////////////////
			///////////////////////////////////////////////////
		{
			int a1 = abs(2 * ptrImg1Gmid[col] - ptrImg1Gup[col] - ptrImg1Gdown[col]);
			int b1 = abs(2 * ptrImg1Gmid[col] - ptrImg1Gmid[col - 1] - ptrImg1Gmid[col + 1]);
			int a2 = abs(2 * ptrImg2Gmid[col] - ptrImg2Gup[col] - ptrImg2Gdown[col]);
			int b2 = abs(2 * ptrImg2Gmid[col] - ptrImg2Gmid[col - 1] - ptrImg2Gmid[col + 1]);

			smlCom1 = a1 + b1;
			smlCom2 = a2 + b2;

			if (smlCom1 > smlCom2)
			{
				if (ptrMarkers[col] > 0)
				{
					////compare[ptrMarkers[col]] = true;
					//comp[ptrMarkers[col]]++;
					//freq[ptrMarkers[col]]++;
					if (max1[ptrMarkers[col]] < smlCom1)
						max1[ptrMarkers[col]] = smlCom1;
				}
			}
			else
			{
				if (ptrMarkers[col] > 0)
				{
					//compare[ptrMarkers[col]] = true;
					//freq[ptrMarkers[col]]++;
					if (max2[ptrMarkers[col]] < smlCom2)
						max2[ptrMarkers[col]] = smlCom2;
				}
			}
		}
	}


	CvMat* mapmap = cvCreateMat(size.height, size.width, CV_32SC1);
	cvSetZero(mapmap);
	for (int i = 0; i < markers->height; i++)
	{
		int* ptrMarkers = (int*)(markers->data.ptr + i*markers->step);
		char* ptrImg1 = (char*)(img1.data + i*img1.step);
		char* ptrImg2 = (char*)(img2.data + i*img2.step);
		int* ptr = (int*)(mapmap->data.ptr + i * mapmap->step);

		for (int j = 0; j < markers->width; j++)
		{
			// 세그먼트 사이의 선 예외처리
			if (ptrMarkers[j] == -1)
			{
				if (j - 1 >= 0)
				{
					ptr[j] = 0;
					for (int k = 0; k < 3; k++) {
						ptrImg1[3 * j + k] = ptrImg1[3 * (j - 1) + k];
					}
				}
			}
			else
			{
				//if (freq[ptrMarkers[j]] / comp[ptrMarkers[j]]>=2)
				if (max1[ptrMarkers[j]] < max2[ptrMarkers[j]])
				{
					ptr[j] = 0;
					for (int k = 0; k < 3; k++)
					{
						//img1.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
						ptrImg1[3 * j + k] = ptrImg2[3 * j + k];
					}
				}
				else
					ptr[j] = 255;
				///////////////////////////////////////////////////
				///////////////////////////////////////////////////
				///////////////////////////////////////////////////
				//if (compare[ptrMarkers[j]] == false)
				//{
				//	for (int k = 0; k < 3; k++)
				//	{
				//		//img1.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
				//		ptrImg1[3 * j + k] = ptrImg2[3 * j + k];
				//	}
				//}
				///////////////////////////////////////////////////
				///////////////////////////////////////////////////
				///////////////////////////////////////////////////
			}
		}
	}
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "Segmentation기반 SML 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;


	temp = img1;
	*dst = cvCloneImage(&temp);

	return;
}

void Fusion::weightSml(IplImage** dst)
{

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	int N = 1;
	//bool compare[7000] = { false, };
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	long long int arrSml1[7000] = { 0, };
	long long int arrSml2[7000] = { 0, };
	int freq[7000] = { 0, };
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////

	//SML 만드는 부분


	// watershed 알고리즘 실행
	temp = img1;
	IplImage* imgSegSrc = cvCloneImage(&temp);
	temp = img2;
	IplImage* imgSegSrc2 = cvCloneImage(&temp);
	IplImage* mat2 = cvCloneImage(&temp);
	cvSet(mat2, cvScalarAll(2),0);
	cvDiv(imgSegSrc, mat2, imgSegSrc);
	cvDiv(imgSegSrc2, mat2, imgSegSrc2);
	cvAdd(imgSegSrc, imgSegSrc2, imgSegSrc);
	
	CvSize size = cvGetSize(imgSegSrc);
	CvMat* markers = cvCreateMat(size.height, size.width, CV_32SC1);
	cvSetZero(markers);
	int seedNum = 1;
	int seedStep = size.width / 10;
	for (int row = seedStep; row < size.height; row += seedStep) {
		int* ptr = (int*)(markers->data.ptr + row * markers->step);
		for (int col = seedStep; col < size.width; col += seedStep) {
			ptr[col] = seedNum;
			seedNum++;
		}
	}
	cvWatershed(imgSegSrc, markers);


	int smlCom1;
	int smlCom2;
	//#pragma omp parallel for schedule(dynamic)
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	//for (int row = seedStep; row < size.height - 1; row += seedStep)
	for (int row = 0; row < size.height; row++)
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
	{
		int* ptrMarkers = (int*)(markers->data.ptr + row * markers->step);

		int* ptrImg1Gup = (int*)(img1G.data + (row - 1)*img1G.step);
		int* ptrImg1Gmid = (int*)(img1G.data + row*img1G.step);
		int* ptrImg1Gdown = (int*)(img1G.data + (row + 1)*img1G.step);

		int* ptrImg2Gup = (int*)(img2G.data + (row - 1)*img2G.step);
		int* ptrImg2Gmid = (int*)(img2G.data + row*img2G.step);
		int* ptrImg2Gdown = (int*)(img2G.data + (row + 1)*img2G.step);
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		///////////////////////////////////////////////////
		//for (int col = seedStep; col < size.width - 1; col += seedStep)
		for (int col = 0; col < size.width; col++)
			///////////////////////////////////////////////////
			///////////////////////////////////////////////////
			///////////////////////////////////////////////////
		{
			int a1 = abs(2 * ptrImg1Gmid[col] - ptrImg1Gup[col] - ptrImg1Gdown[col]);
			int b1 = abs(2 * ptrImg1Gmid[col] - ptrImg1Gmid[col - seedStep] - ptrImg1Gmid[col + seedStep]);
			int a2 = abs(2 * ptrImg2Gmid[col] - ptrImg2Gup[col] - ptrImg2Gdown[col]);
			int b2 = abs(2 * ptrImg2Gmid[col] - ptrImg2Gmid[col - seedStep] - ptrImg2Gmid[col + seedStep]);

			smlCom1 = a1 + b1;
			smlCom2 = a2 + b2;
			if (ptrMarkers[col]>0)
			{
				arrSml1[ptrMarkers[col]] += smlCom1;
				arrSml2[ptrMarkers[col]] += smlCom2;
				freq[ptrMarkers[col]]++;
			}

			//if (smlCom1 > smlCom2)
			//{
			//	if (ptrMarkers[col] > 0)
			//	{
			//		//compare[ptrMarkers[col]] = true;
			//		comp[ptrMarkers[col]]++;
			//		freq[ptrMarkers[col]]++;
			//	}
			//}
			//else
			//{
			//	if (ptrMarkers[col] > 0)
			//	{
			//		//compare[ptrMarkers[col]] = true;
			//		freq[ptrMarkers[col]]++;
			//	}
			//}
		}
	}

	CvMat* mapmap = cvCreateMat(size.height, size.width, CV_32SC1);
	cvSetZero(mapmap);

	for (int i = 0; i < markers->height; i++)
	{
		int* ptrMarkers = (int*)(markers->data.ptr + i*markers->step);
		char* ptrImg1 = (char*)(img1.data + i*img1.step);
		char* ptrImg2 = (char*)(img2.data + i*img2.step);
		int* ptr = (int*)(mapmap->data.ptr + i * mapmap->step);

		for (int j = 0; j < markers->width; j++)
		{
			// 세그먼트 사이의 선 예외처리
			if (ptrMarkers[j] == -1)
			{
				if (j - 1 >= 0)
				{
					ptr[j] = 0;
					for (int k = 0; k < 3; k++) {
						ptrImg1[3 * j + k] = ptrImg1[3 * (j - 1) + k];
					}
				}
			}
			else
			{
				if (arrSml2[ptrMarkers[j]] > arrSml1[ptrMarkers[j]])
				{
					ptr[j] = 0;
					for (int k = 0; k < 3; k++)
					{
						//img1.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
						ptrImg1[3 * j + k] = ptrImg2[3 * j + k];
					}
				}
				else
					ptr[j] = 255;
				//if (freq[ptrMarkers[j]] / comp[ptrMarkers[j]] >= 2)
				//{
				//	for (int k = 0; k < 3; k++)
				//	{
				//		//img1.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
				//		ptrImg1[3 * j + k] = ptrImg2[3 * j + k];
				//	}
				//}
				///////////////////////////////////////////////////
				///////////////////////////////////////////////////
				///////////////////////////////////////////////////
				//if (compare[ptrMarkers[j]] == false)
				//{
				//	for (int k = 0; k < 3; k++)
				//	{
				//		//img1.at<Vec3b>(i, j)[k] = img2.at<Vec3b>(i, j)[k];
				//		ptrImg1[3 * j + k] = ptrImg2[3 * j + k];
				//	}
				//}
				///////////////////////////////////////////////////
				///////////////////////////////////////////////////
				///////////////////////////////////////////////////
			}
		}
	}
	std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
	std::cout << "Weight Segment기반 SML 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;


	temp = img1;
	*dst = cvCloneImage(&temp);

	return;
}

