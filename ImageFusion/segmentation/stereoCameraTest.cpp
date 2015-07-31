#include <iostream>
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace std;

int main()
{
	CvCapture* capture1 = cvCaptureFromCAM(2);
	CvCapture* capture2 = cvCaptureFromCAM(6);
	IplImage* frame1 = cvQueryFrame(capture1);
	IplImage* frame2 = cvQueryFrame(capture2);

	cvShowImage("test1", frame1);
	cvShowImage("test2",frame2);
	cvWaitKey(0);
	cvReleaseImage(&frame1);
	cvReleaseImage(&frame2); 
	cvReleaseCapture(&capture1);
	cvReleaseCapture(&capture2);
}