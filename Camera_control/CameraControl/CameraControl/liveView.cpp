#include <iostream>
#include "Header\EDSDK.h"


using namespace std;

int main()
{
	//초기 셋팅파트
	//EDSDK 초기 설정관련
	EdsError err1 = EDS_ERR_OK;
	EdsCameraRef camera = NULL;
	EdsCameraListRef cameraList = NULL;
	unsigned char* data = NULL;
	unsigned long size = 0;
	EdsUInt32 count = 0;
	bool isSDKLoaded = false;
	int input;

	err1 = EdsInitializeSDK();


	//카메라 불러오기
	if (err1 == EDS_ERR_OK)
	{
		isSDKLoaded = true;

		err1 = EdsGetCameraList(&cameraList);
		if (err1 = EDS_ERR_OK)	//cameraList 성공
		{
			err1 = EdsGetChildCount(cameraList, &count);
			if (count == 0)
			{
				err1 = EDS_ERR_DEVICE_NOT_FOUND;
			}
		}

		if (err1 == EDS_ERR_OK)
		{
			err1 = EdsGetChildAtIndex(cameraList, 0, &camera);
			if (err1 == EDS_ERR_OK) cout << "camera1 is on" << endl;
		}

		if (cameraList != NULL)
		{
			EdsRelease(cameraList);
			cameraList = NULL;
		}

		//openSession
		if (err1 == EDS_ERR_OK)
		{
			err1 = EdsOpenSession(camera);
		}
	}

	//영상촬영 준비
	EdsUInt32 device;
	EdsUInt32 record_start = 4;
	EdsUInt32 record_stop = 0;
	err1 = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);

	if (err1 == EDS_ERR_OK)
	{
		device |= kEdsEvfOutputDevice_PC;
		err1 = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	}

	EdsStreamRef stream = NULL;
	EdsEvfImageRef evfImage = NULL;

	err1 = EdsCreateMemoryStream(0, &stream);
	// Create EvfImageRef.
	if (err1 == EDS_ERR_OK)
	{
		err1 = EdsCreateEvfImageRef(stream, &evfImage);
	}

	// Download live view image data.
	if (err1 == EDS_ERR_OK)
	{
		err1 = EdsDownloadEvfImage(camera, evfImage);
	}

	err1 = EdsGetPointer(stream, (EdsVoid**)& data);
	err1 = EdsGetLength(stream, &size);


	// Release stream
	if (stream != NULL)
	{
		EdsRelease(stream);
		stream = NULL;
	}
	// Release evfImage
	if (evfImage != NULL)
	{
		EdsRelease(evfImage);
		evfImage = NULL;
	}





	while (1)
	{
		cin >> input;
		if (input == 0)
		{
			device &= ~kEdsEvfOutputDevice_PC;
			err1 = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
			break;
		}
	}

	if (isSDKLoaded == true)
	{
		err1 = EdsTerminateSDK();
	}


}