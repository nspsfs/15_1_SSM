#include <iostream>
#include "ne_imageFusion.h"

using namespace std;

int main()
{
	Fusion fusion;
	IplImage* image1 = cvLoadImage("image/test3_1.bmp",1);
	IplImage* image2 = cvLoadImage("image/test3_2.bmp",1);
	IplImage* multiFocusImage = NULL;

	fusion.setImg(image1, image2);
	fusion.sml(&multiFocusImage);

	cvShowImage("image1", image1);
	cvShowImage("image2", image2);
	cvShowImage("multiFocusImage", multiFocusImage);

	cvWaitKey(0);

	return 0;
}