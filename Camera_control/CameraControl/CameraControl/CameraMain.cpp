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
			cout << "camera1 is on" << endl;

				err = EdsGetChildAtIndex(cameraList, 1, &camera2);
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
		EdsUInt32 device2;
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

		if (count > 1)
		{
			err = EdsGetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);

			if (err == EDS_ERR_OK)
			{
				device |= kEdsEvfOutputDevice_TFT;
				err = EdsSetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);
			}

			EdsUInt32 saveTo = kEdsSaveTo_Camera;
			err = EdsSetPropertyData(camera2, kEdsPropID_SaveTo, 0, sizeof(saveTo), &saveTo);
		}

		//do something
		while (1)
		{
			cin >> input;
				//err = EdsSendCommand(camera, kEdsCameraCommand_TakePicture, 0);	//사진촬영 파트
			if (input == 1)
			{
				err = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_start), &record_start);
				if (count>1)	err = EdsSetPropertyData(camera2, kEdsPropID_Record, 0, sizeof(record_start), &record_start);
			}
			else if (input == 2)
			{
				err = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_start), &record_stop);
				if (count>1)	err = EdsSetPropertyData(camera2, kEdsPropID_Record, 0, sizeof(record_start), &record_stop);
			}
			else if (input == 0)
			{
				err = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
				if (count>1)	err = EdsGetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);
				break;
			}

		}
		

		//close session
		if (err == EDS_ERR_OK)
		{
			err = EdsCloseSession(camera);
			if (count>1) err = EdsCloseSession(camera2);
		}

		//release camera
		if (camera != NULL)
		{
			EdsRelease(camera);
			EdsRelease(camera2);
		}

		//Terminate SDK
		if (isSDKLoaded)
		{
			EdsTerminateSDK();
		}
	}


	return 0;
}