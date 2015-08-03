#include <iostream>
#include "imageFusion.h"
#include "changeViewPoint.h"

using namespace std;

int main()
{
	CVP cvp;
	IplImage* disparity = cvLoadImage("image/test1_disparity.jpg",0);
	IplImage* rightImage = cvLoadImage("image/test1_R.jpg");
	IplImage* leftImage = cvLoadImage("image/test1_L.jpg");
	//IplImage* disparity = cvLoadImage("disp.png",0);
	//IplImage* rightImage = cvLoadImage("right.png");
	//IplImage* leftImage = cvLoadImage("left.png");
	IplImage* convertImage = NULL;

	cvp.setImg(disparity, leftImage, rightImage);
	cvp.runcvt(&convertImage);
	cvShowImage("leftImage", leftImage);
	cvShowImage("rightImage", rightImage);
	cvShowImage("convertImage", convertImage);
	cvSaveImage("convertImage.jpg", convertImage);


	Fusion fusion;
	IplImage* image1 = cvLoadImage("test2_1.bmp");
	IplImage* image2 = cvLoadImage("test2_2.bmp");
	IplImage* multiFocusImage=NULL;

	fusion.setImg(image1, image2);
	//fusion.setImg(convertImage, leftImage);
	fusion.sml(&multiFocusImage);

	cvShowImage("image1", image1);
	cvShowImage("image2", image2);
	cvShowImage("MultiFocusImage", multiFocusImage);
	//cvSaveImage("MFImg.jpg", multiFocusImage);


	cvWaitKey(0);
	return 0;
}