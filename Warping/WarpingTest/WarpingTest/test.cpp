#if 0
#include <iostream>
#include "changeViewPoint.h"

using namespace std;

int main()
{
	CVP cvp;
	/*cvp.disparity = imread("image/test_disparity.jpg");
	cvp.imgL = imread("image/test_L.jpg");
	cvp.imgR = imread("image/test_R.jpg");*/
	IplImage* disparityImage = cvLoadImage("image/test_disparity.jpg");
	IplImage* leftImage = cvLoadImage("image/test_L.jpg");
	IplImage* rightImage = cvLoadImage("image/test_R.jpg");

	cvp.setimg(disparityImage, leftImage, rightImage);

	IplImage* output=NULL;
	cvp.runcvt(&output);

	cvShowImage("test2", output);
	waitKey(0);
	cvReleaseImage(&output);
	
	return 0;
}
#endif