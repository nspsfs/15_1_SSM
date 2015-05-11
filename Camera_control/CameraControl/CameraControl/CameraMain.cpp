#include <iostream>
#include <windows.h>
#include "Header/EDSDK.h"

using namespace std;

void camera1_ready(EdsCameraRef& camera1)
{

}

void camera2_ready(EdsCameraRef& camera2)
{

}

int main()
{
	//EDSDK 초기 설정관련
	EdsError err1 = EDS_ERR_OK;
	EdsError err2 = EDS_ERR_OK;
	EdsCameraRef camera = NULL;
	EdsCameraRef camera2 = NULL;
	EdsCameraListRef cameraList = NULL;
	EdsUInt32 count = 0;
	bool isSDKLoaded = false;
	int input;

	DWORD start;
	DWORD end;

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
			err2 = EdsGetChildAtIndex(cameraList, 1, &camera2);
			if (err1 == EDS_ERR_OK) cout << "camera1 is on" << endl;
			if (err2 == EDS_ERR_OK) cout << "camera2 is on" << endl;
		}

		if (cameraList != NULL)
		{
			EdsRelease(cameraList);
			cameraList = NULL;
		}

		//openSession
		if (err1 == EDS_ERR_OK&&err2 == EDS_ERR_OK)
		{
			err1 = EdsOpenSession(camera);
			err2 = EdsOpenSession(camera2);
		}

		//영상촬영 준비
		EdsUInt32 device;
		EdsUInt32 device2;
		EdsUInt32 record_start = 4;
		EdsUInt32 record_stop = 0;
		EdsUInt32 record_start2 = 4;
		EdsUInt32 record_stop2 = 0;
		err1 = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
		err2 = EdsGetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);

		if (err1 == EDS_ERR_OK&&err2 == EDS_ERR_OK)
		{
			device |= kEdsEvfOutputDevice_TFT;
			device2 |= kEdsEvfOutputDevice_TFT;
			err1 = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
			err2 = EdsSetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);
		}

		EdsUInt32 saveTo = kEdsSaveTo_Camera;
		EdsUInt32 saveTo2 = kEdsSaveTo_Camera;
		err1 = EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, sizeof(saveTo), &saveTo);
		err2 = EdsSetPropertyData(camera2, kEdsPropID_SaveTo, 0, sizeof(saveTo2), &saveTo2);

		//command
		while (1)
		{
			cin >> input;
			if (input == 1)
			{
				start = GetTickCount();
				//err1 = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_start), &record_start);
				err2 = EdsSetPropertyData(camera2, kEdsPropID_Record, 0, sizeof(record_start2), &record_start2);
				end = GetTickCount();
				//cout << end - start << endl;
				cout << end - start << endl;
				Sleep(1000 - (end - start));
				cout << GetTickCount() - start << endl;
				//start = GetTickCount();
				//err2 = EdsSetPropertyData(camera2, kEdsPropID_Record, 0, sizeof(record_start2), &record_start2);
				err1 = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_start), &record_start);
				end = GetTickCount();
				cout << end - start << endl;
			}
			else if (input == 2)
			{
				err1 = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_stop), &record_stop);
				err2 = EdsSetPropertyData(camera2, kEdsPropID_Record, 0, sizeof(record_stop2), &record_stop2);
			}
			else if (input == 0)
			{
				err1 = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
				err2 = EdsGetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);
				break;
			}
		}

		//close session
		if (err1 == EDS_ERR_OK)
		{
			err1 = EdsCloseSession(camera);
			err2 = EdsCloseSession(camera2);
		}

		//release camera
		if (camera != NULL)
		{
			EdsRelease(camera);
		}
		if (camera2 != NULL)
		{
			EdsRelease(camera2);
		}

		if (isSDKLoaded)	//Terminate SDK
		{
			EdsTerminateSDK();
		}
	}
	return 0;
}