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

	//cvp class --> ���� ���� �ذ�
	//cvp.setimg(disparity, leftImage, rightImage);	//Image ����(���и�Ƽ, ����, ������)
	cvp.setimg(disparity, rightImage, leftImage);	//Image ����(���и�Ƽ, ����, ������)
	cvp.runcvt(&convertImage);						//�������� �ذ��Ͽ� dst�̹����� ����
													//CVPŬ���� ���� MaxPixel���� ����(�ִ� ��(255)�϶��� �ȼ� ��ġ����)�ϸ� �ȴ�.

	//fusion class --> Multi Focus Image Fusion
	//fusion.setimg(rightImage, convertImage);		//Image ����(������ �ٸ� ������ ���� �ΰ��� �̹���)
	//fusion.sml(&multiFocusImage);					//Multi Focus Image Fusion�� ������ �����  dst�̹����� ����
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