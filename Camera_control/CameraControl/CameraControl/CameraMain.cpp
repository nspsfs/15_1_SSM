#if 0

#include <iostream>
#include <windows.h>
#include "Header/EDSDK.h"

using namespace std;

//download function
EdsError downloadImage(EdsDirectoryItemRef directoryItem)
{
	EdsError err = EDS_ERR_OK;
	EdsStreamRef stream = NULL;
	// Get directory item information
	EdsDirectoryItemInfo dirItemInfo;
	err = EdsGetDirectoryItemInfo(directoryItem, &dirItemInfo);
	// Create file stream for transfer destination
	if (err == EDS_ERR_OK)
	{
		err = EdsCreateFileStream(dirItemInfo.szFileName,
			kEdsFileCreateDisposition_CreateAlways,
			kEdsAccess_ReadWrite, &stream);

	}
	// Download image
	if (err == EDS_ERR_OK)
	{
		cout << dirItemInfo.szFileName<< " downaload Start!" << endl;
		err = EdsDownload(directoryItem, dirItemInfo.size, stream);
	}
	// Issue notification that download is complete
	if (err == EDS_ERR_OK)
	{
		err = EdsDownloadComplete(directoryItem);
		cout << "download complete" << endl;
	}
	// Release stream
	if (stream != NULL)
	{
		EdsRelease(stream);
		stream = NULL;
	}
	return err;
}

EdsError EDSCALLBACK handleObjectEvent(EdsObjectEvent event, EdsBaseRef object, EdsVoid * context) {

	EdsDirectoryItemInfo objectInfo;
	EdsError err = EDS_ERR_OK;
	cout << "Handler 실행" << endl;

	switch (event) {
	case kEdsObjectEvent_DirItemCreated:
		cout << "Created" << endl;
		err = EdsGetDirectoryItemInfo(object, &objectInfo);
		downloadImage(object);
		break;
	case  kEdsObjectEvent_DirItemRequestTransfer:
		cout << "RequestTransfer" << endl;
		err = EdsGetDirectoryItemInfo(object, &objectInfo);
		downloadImage(object);
		break;
	default:
		break;
	}
	//Release object
	if (object) {
		EdsRelease(object);
	}
	return err;
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
	bool isShoot = false;
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
			//Download Parts
			err1 = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, handleObjectEvent, NULL);
			//err2 = EdsSetObjectEventHandler(camera2, kEdsObjectEvent_All, handleObjectEvent, NULL);
		}

		if (cameraList != NULL)
		{
			EdsRelease(cameraList);
			cameraList = NULL;
		}

		

		//openSession
		if (err1 == EDS_ERR_OK||err2 == EDS_ERR_OK)
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

		if (err1 == EDS_ERR_OK||err2 == EDS_ERR_OK)
		{
			device |= kEdsEvfOutputDevice_TFT;
			device2 |= kEdsEvfOutputDevice_TFT;
			err1 = EdsSetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
			err2 = EdsSetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);
		}

		

		EdsUInt32 saveTo = kEdsSaveTo_Camera; //원래 kEdsSaveTo_Camera
		EdsUInt32 saveTo2 = kEdsSaveTo_Camera; //원래 kEdsSaveTo_Camera
		err1 = EdsSetPropertyData(camera, kEdsPropID_SaveTo, 0, sizeof(saveTo), &saveTo);
		err2 = EdsSetPropertyData(camera2, kEdsPropID_SaveTo, 0, sizeof(saveTo2), &saveTo2);

		//command
		while (1)
		{
			cin >> input;
			if (input == 1)
			{
				isShoot = true;
				err1 = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_start), &record_start);
				start = GetTickCount();
				err2 = EdsSetPropertyData(camera2, kEdsPropID_Record, 0, sizeof(record_start2), &record_start2);
				end = GetTickCount();
				cout << "delay : " << end - start << endl;
			}
			else if (input == 2)
			{
				err1 = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_stop), &record_stop);
				err2 = EdsSetPropertyData(camera2, kEdsPropID_Record, 0, sizeof(record_stop2), &record_stop2);
				
				isShoot = false;

			}
			else if (input == 3)
			{
				err1 = EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_Completely);
				EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_OFF);
			}
			else if (input == 0)
			{
				if (isShoot == false)
				{
					err1 = EdsGetPropertyData(camera, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
					err2 = EdsGetPropertyData(camera2, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);
					break;
				}
				else
				{
					cout << "촬영 정지 먼저하세요" << endl;
				}
			}
		}

		

		//close session
		if (err1 == EDS_ERR_OK)
		{
			err1 = EdsCloseSession(camera);
			err2 = EdsCloseSession(camera2);
		}
		err1 = EdsOpenSession(camera);
		err1 = EdsCloseSession(camera);
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

#endif