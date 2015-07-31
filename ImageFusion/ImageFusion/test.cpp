#include <iostream>
#include "imageFusion.h"
#include "changeViewPoint.h"

using namespace std;

int main()
{
	Fusion fusion;
	CVP cvp;
	IplImage* disparity = cvLoadImage("image/disp.png");
	IplImage* rightImage = cvLoadImage("image/right.png");
	IplImage* leftImage = cvLoadImage("image/left.png");
	IplImage* image1 = cvLoadImage("image/test2_1.bmp");
	IplImage* image2 = cvLoadImage("image/test2_2.bmp");
	IplImage* convertImage = NULL;
	IplImage* multiFocusImage=NULL;

	//cvp class --> 시차 문제 해결
	//cvp.setimg(disparity, leftImage, rightImage);	//Image 셋팅(디스패리티, 왼쪽, 오른쪽)
	cvp.setimg(disparity, rightImage, leftImage);	//Image 셋팅(디스패리티, 왼쪽, 오른쪽)
	cvp.runcvt(&convertImage);						//시차문제 해결하여 dst이미지에 저장
													//CVP클래스 내의 MaxPixel값을 조절(최대 값(255)일때의 픽셀 위치차이)하면 된다.

	//fusion class --> Multi Focus Image Fusion
	//fusion.setimg(rightImage, convertImage);		//Image 셋팅(각각의 다른 초점을 가진 두개의 이미지)
	//fusion.sml(&multiFocusImage);					//Multi Focus Image Fusion을 수행한 결과를  dst이미지에 저장
	fusion.setimg(image1, image2);
	//fusion.sml(&multiFocusImage);
	fusion.segsml(&multiFocusImage);


	cvShowImage("MultiFocusImage", multiFocusImage);
	cvShowImage("convertImage", convertImage);
	//cvShowImage("disparity", disparity);
	//cvShowImage("leftImage", leftImage);
	//cvShowImage("rightImage", rightImage);
	//cvShowImage("image1", image1);
	//cvShowImage("Image2", image2);
	cvWaitKey(0);
	//cvDestroyAllWindows();
	//cvReleaseImage(&disparity);
	//cvReleaseImage(&rightImage);
	//cvReleaseImage(&leftImage);
	//cvReleaseImage(&convertImage);
	//cvReleaseImage(&multiFocusImage);


	return 0;
}