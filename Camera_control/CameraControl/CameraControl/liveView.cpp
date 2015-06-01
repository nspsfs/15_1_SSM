#include <iostream>
#include <windows.h>
#include "Header/EDSDK.h"
#include <thread>
#include <string>
#include <time.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "jpeglib.h"

using namespace std;

int main()
{
	// Initialize part
	EdsError err1=EDS_ERR_OK;
	EdsError err2 = EDS_ERR_OK;
	EdsCameraRef camera1 = NULL;
	EdsCameraRef camera2 = NULL;
	EdsCameraListRef cameraList = NULL;
	EdsUInt32 count = 0;

	err1 = EdsInitializeSDK();

	if (err1 == EDS_ERR_OK)
	{
		err1 = EdsGetCameraList(&cameraList);
		if (err1 = EDS_ERR_OK)	//cameraList ¼º°ø
		{
			err1 = EdsGetChildCount(cameraList, &count);
			if (count == 0)
			{
				err1 = EDS_ERR_DEVICE_NOT_FOUND;
			}
		}
	}

	if (err1 == EDS_ERR_OK)
	{
		err1 = EdsGetChildAtIndex(cameraList, 0, &camera1);
		if (err1 == EDS_ERR_OK) cout << "camera1 is on" << endl;
		err2 = EdsGetChildAtIndex(cameraList, 1, &camera2);
		if (err2 == EDS_ERR_OK) cout << "camera2 is on" << endl;
	}

	if (cameraList != NULL)
	{
		EdsRelease(cameraList);
		cameraList = NULL;
	}

	//openSession
	if (err1 == EDS_ERR_OK)
	{
		err1 = EdsOpenSession(camera1);
		err2 = EdsOpenSession(camera2);
	}

	EdsUInt32 device;
	EdsUInt32 device2;
	err1 = EdsGetPropertyData(camera1, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	err2 = EdsGetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);

	if (err1 == EDS_ERR_OK)
	{
		device |= kEdsEvfOutputDevice_PC;
		device2 |= kEdsEvfOutputDevice_PC;
		err1 = EdsSetPropertyData(camera1, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
		err2 = EdsSetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);
	}

	// download stream part
	EdsStreamRef stream = NULL;
	EdsEvfImageRef evfImage = NULL;
	EdsStreamRef stream2 = NULL;
	EdsEvfImageRef evfImage2 = NULL;


	///display part

	IplImage* img;
	IplImage* img2;
	char c;
	//img = cvLoadImage("live1.jpg");
	cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Example2", CV_WINDOW_AUTOSIZE);
	
	while (1)
	{
		
		err1 = EdsCreateMemoryStream(0, &stream);
		err1 = EdsCreateFileStream("live1.jpg", kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
		err1 = EdsCreateEvfImageRef(stream, &evfImage);
		err1 = EdsDownloadEvfImage(camera1, evfImage);
		EdsRelease(stream);
		EdsRelease(evfImage);
		err2 = EdsCreateMemoryStream(0, &stream2);
		err2 = EdsCreateFileStream("live2.jpg", kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream2);
		err2 = EdsCreateEvfImageRef(stream2, &evfImage2);
		err2 = EdsDownloadEvfImage(camera2, evfImage2);
		EdsRelease(stream2);
		EdsRelease(evfImage2);

		img = cvLoadImage("live1.jpg");
		img2 = cvLoadImage("live2.jpg");

		cvShowImage("Example1", img);
		cvShowImage("Example2", img2);
		//Sleep(33);
		cvReleaseImage(&img);
		cvReleaseImage(&img2);

		c = cvWaitKey(33);
		if (c == 27)	break;
		
		
	}
		//cvWaitKey(0);

	cvDestroyWindow("Example1");
	cvDestroyWindow("Example2");

	///

	if (stream!=NULL)
	{
		EdsRelease(stream);
		stream = NULL;
	}

	if (evfImage != NULL)
	{
		EdsRelease(evfImage);
		evfImage = NULL;
	}

	//// loop part
	int input;
	while (1)
	{
		cin >> input;
		if (input == 0)
		{
			break;
		}
	}

	err1 = EdsGetPropertyData(camera1, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);

	if (err1 = EDS_ERR_OK)
	{
		device &= ~kEdsEvfOutputDevice_PC;

		err1 = EdsSetPropertyData(camera1, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	}

	//Close session
	err1 = EdsCloseSession(camera1);

	if (camera1 != NULL)
	{
		EdsRelease(camera1);
	}
	
	EdsTerminateSDK();
}