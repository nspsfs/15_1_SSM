#include <iostream>
#include "Header/EDSDK.h"

using namespace std;

int main()
{
	//EDSDK 초기 설정관련
	EdsError err = EDS_ERR_OK;
	EdsCameraRef camera = NULL;
	EdsCameraRef camera2 = NULL;
	EdsCameraListRef cameraList = NULL;
	EdsUInt32 count = 0;
	bool isSDKLoaded = false;

	int input;

	err = EdsInitializeSDK();

	//카메라 불러오기
	if (err == EDS_ERR_OK)
	{
		isSDKLoaded = true;

		err = EdsGetCameraList(&cameraList); //카메라 리스트 받아오기
		//카메라 갯수 받아오기
		if (err = EDS_ERR_OK)
		{
			err = EdsGetChildCount(cameraList, &count);
			if (count == 0)
			{
				err = EDS_ERR_DEVICE_NOT_FOUND;
			}
		}

		if (err == EDS_ERR_OK)
		{
			err = EdsGetChildAtIndex(cameraList, 0, &camera);
			if (count > 1)
			{
				err = EdsGetChildAtIndex(cameraList, 1, &camera2);
			}
		}

		//release camera list
		if (cameraList != NULL)
		{
			EdsRelease(cameraList);
			cameraList = NULL;
		}

		//Set event handler

		//Open session
		if (err == EDS_ERR_OK)
		{
			err = EdsOpenSession(camera);
			if (count > 1)
			{
				err = EdsOpenSession(camera2);
			}
		}

		//영상촬영 준비
		EdsUInt32 device;
		EdsUInt32 record_start = 4;
		EdsUInt32 record_stop = 0;
		err = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);

		if (err == EDS_ERR_OK)
		{
			device |= kEdsEvfOutputDevice_TFT;
			err = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
		}

		EdsUInt32 saveTo = kEdsSaveTo_Camera;
		err = EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, sizeof(saveTo), &saveTo);

		//do something
		while (1)
		{
			cin >> input;
				//err = EdsSendCommand(camera, kEdsCameraCommand_TakePicture, 0);	//사진촬영 파트
			if (input == 1)
			{
				err = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_start), &record_start);
			}
			else if (input == 2)
			{
				err = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_start), &record_stop);
			}
			else if (input == 0)
			{
				err = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
				break;
			}

		}
		

		//close session
		if (err == EDS_ERR_OK)
		{
			err = EdsCloseSession(camera);
		}

		//release camera
		if (camera != NULL)
		{
			EdsRelease(camera);
		}

		//Terminate SDK
		if (isSDKLoaded)
		{
			EdsTerminateSDK();
		}
	}


	return 0;
}