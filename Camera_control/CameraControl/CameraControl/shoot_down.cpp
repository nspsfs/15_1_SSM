#include <iostream>
#include <windows.h>
#include "Header/EDSDK.h"
#include <thread>
#include <string>
#include <time.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"

using namespace std;

void cameraSetup(EdsCameraRef* camera, EdsCameraRef* camera2);
void recordStart(EdsCameraRef camera, EdsCameraRef camera2);
void recordStop(EdsCameraRef camera, EdsCameraRef camera2);
void finish(EdsCameraRef camera, EdsCameraRef camera2);
void downEvent(EdsCameraRef camera, EdsCameraRef camera2);
EdsError EDSCALLBACK handleObjectEvent(EdsObjectEvent event, EdsBaseRef object, EdsVoid * context);
EdsError EDSCALLBACK handleObjectEvent2(EdsObjectEvent event, EdsBaseRef object, EdsVoid * context);
EdsError downloadImage(EdsDirectoryItemRef directoryItem);
void getNowTime();
string nowtime;

EdsError takePicture(EdsCameraRef camera)
{
	EdsError err;
	err = EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_Completely);
	EdsSendCommand(camera, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_OFF);
	return err;
}

int main()
{
	EdsError err1 = EDS_ERR_OK;
	EdsCameraRef camera = NULL;
	EdsCameraRef camera2 = NULL;
	cameraSetup(&camera, &camera2);
	bool end = false;
	bool isShoot = false;
	int input;
	while (1)
	{
		cin >> input;
		switch (input)
		{
		case 0:
			finish(camera,camera2);
			break;
		case 1:
			if (isShoot == false)
			{
				isShoot = true;
				recordStart(camera, camera2);
			}
			break;
		case 2:
			if (isShoot == true)
			{
				recordStop(camera, camera2);
				finish(camera, camera2);
				end = true;
			}
			break;
		}
		if (end == true) break;
	}

	
	return 0;
}

void cameraSetup(EdsCameraRef* camera, EdsCameraRef* camera2)
{
	EdsError err1 = EDS_ERR_OK;
	EdsError err2 = EDS_ERR_OK;
	EdsCameraRef camera11 = NULL;
	EdsCameraRef camera22 = NULL;
	EdsCameraListRef cameraList = NULL;
	EdsUInt32 count = 0;

	err1 = EdsInitializeSDK();

	if (err1 == EDS_ERR_OK)
	{
		err1 = EdsGetCameraList(&cameraList);
		if (err1 = EDS_ERR_OK)	//cameraList 성공
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
		err1 = EdsGetChildAtIndex(cameraList, 0, &camera11);
		err2 = EdsGetChildAtIndex(cameraList, 1, &camera22);
		if (err1 == EDS_ERR_OK) cout << "camera1 is on" << endl;
		if (err2 == EDS_ERR_OK) cout << "camera2 is on" << endl;
	}

	if (cameraList != NULL)
	{
		EdsRelease(cameraList);
		cameraList = NULL;
	}

	//openSession
	if (err1 == EDS_ERR_OK || err2 == EDS_ERR_OK)
	{
		err1 = EdsOpenSession(camera11);
		err2 = EdsOpenSession(camera22);
	}

	EdsUInt32 device;
	EdsUInt32 device2;
	err1 = EdsGetPropertyData(camera11, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
	err2 = EdsGetPropertyData(camera22, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);

	if (err1 == EDS_ERR_OK || err2 == EDS_ERR_OK)
	{
		device |= kEdsEvfOutputDevice_TFT;
		device2 |= kEdsEvfOutputDevice_TFT;
		err1 = EdsSetPropertyData(camera11, kEdsPropID_Evf_OutputDevice, 0, sizeof(device), &device);
		err2 = EdsSetPropertyData(camera22, kEdsPropID_Evf_OutputDevice, 0, sizeof(device2), &device2);
	}

	EdsUInt32 saveTo = kEdsSaveTo_Camera; //원래 kEdsSaveTo_Camera
	EdsUInt32 saveTo2 = kEdsSaveTo_Camera; //원래 kEdsSaveTo_Camera
	err1 = EdsSetPropertyData(camera11, kEdsPropID_SaveTo, 0, sizeof(saveTo), &saveTo);
	err2 = EdsSetPropertyData(camera22, kEdsPropID_SaveTo, 0, sizeof(saveTo2), &saveTo2);

	*camera = camera11;
	*camera2 = camera22;

	return;
}

void recordStart(EdsCameraRef camera, EdsCameraRef camera2)
{
	EdsError err1 = EDS_ERR_OK;
	EdsError err2 = EDS_ERR_OK;
	EdsUInt32 record_start = 4;
	EdsUInt32 record_start2 = 4;
	DWORD start;
	DWORD end;
	err1 = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_start), &record_start);
	start = GetTickCount();
	err2 = EdsSetPropertyData(camera2, kEdsPropID_Record, 0, sizeof(record_start), &record_start2);
	end = GetTickCount();
	cout << "Delay : " << end - start << "(" << (end-start) / 30 << " frame)" << endl;
	cvDestroyWindow("Example1");
	cvDestroyWindow("Example2");
}

void recordStop(EdsCameraRef camera, EdsCameraRef camera2)
{
	EdsError err1 = EDS_ERR_OK;
	EdsError err2 = EDS_ERR_OK;
	EdsUInt32 record_stop = 0;
	EdsUInt32 record_stop2 = 0;
	err1 = EdsSetPropertyData(camera, kEdsPropID_Record, 0, sizeof(record_stop), &record_stop);
	err2 = EdsSetPropertyData(camera2, kEdsPropID_Record, 0, sizeof(record_stop2), &record_stop2);
}

void finish(EdsCameraRef camera, EdsCameraRef camera2)
{
	EdsError err1 = EDS_ERR_OK;
	EdsError err2 = EDS_ERR_OK;
	//Close session
	err1 = EdsCloseSession(camera);
	err2 = EdsCloseSession(camera2);

	getNowTime();

	err1 = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, handleObjectEvent, NULL);
	err2 = EdsSetObjectEventHandler(camera2, kEdsObjectEvent_All, handleObjectEvent2, NULL);
	err1 = EdsOpenSession(camera);
	err2 = EdsOpenSession(camera2);
	err1 = EdsCloseSession(camera);
	err2 = EdsCloseSession(camera2);

	if (camera != NULL)
	{
		EdsRelease(camera);
	}
	if (camera2 != NULL)
	{
		EdsRelease(camera2);
	}
	EdsTerminateSDK();
}

EdsError downloadImage(EdsDirectoryItemRef directoryItem, int num_camera)
{
	cout << "downloadImage 실행" << endl;
	EdsError err = EDS_ERR_OK;
	EdsStreamRef stream = NULL;
	// Get directory item information
	EdsDirectoryItemInfo dirItemInfo;
	err = EdsGetDirectoryItemInfo(directoryItem, &dirItemInfo);
	// Create file stream for transfer destination
	if (err == EDS_ERR_OK)
	{
		string str_path;
		if (num_camera == 1)
		{
			str_path = "[LEFT]" + nowtime + ".MOV";
		}
		else if (num_camera == 2)
		{
			str_path = "[RIGHT]" + nowtime + ".MOV";
		}

		const char* ch_dest = str_path.c_str();
		err = EdsCreateFileStream(ch_dest,
			kEdsFileCreateDisposition_CreateAlways,
			kEdsAccess_ReadWrite, &stream);
		//err = EdsCreateFileStream(dirItemInfo.szFileName,
		//	kEdsFileCreateDisposition_CreateAlways,
		//	kEdsAccess_ReadWrite, &stream);

	}
	// Download image
	if (err == EDS_ERR_OK)
	{
		cout << dirItemInfo.szFileName << " downaload Start!" << endl;
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
		downloadImage(object, 1);
		break;
	case  kEdsObjectEvent_DirItemRequestTransfer:
		cout << "RequestTransfer" << endl;
		err = EdsGetDirectoryItemInfo(object, &objectInfo);
		downloadImage(object, 1);
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

EdsError EDSCALLBACK handleObjectEvent2(EdsObjectEvent event, EdsBaseRef object, EdsVoid * context) {

	EdsDirectoryItemInfo objectInfo;
	EdsError err = EDS_ERR_OK;
	cout << "Handler 실행" << endl;

	switch (event) {
	case kEdsObjectEvent_DirItemCreated:
		cout << "Created" << endl;
		err = EdsGetDirectoryItemInfo(object, &objectInfo);
		downloadImage(object, 2);
		break;
	case  kEdsObjectEvent_DirItemRequestTransfer:
		cout << "RequestTransfer" << endl;
		err = EdsGetDirectoryItemInfo(object, &objectInfo);
		downloadImage(object, 2);
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

void downEvent(EdsCameraRef camera, EdsCameraRef camera2)
{
	EdsError err1 = EDS_ERR_OK;
	err1 = EdsSetObjectEventHandler(camera, kEdsObjectEvent_All, handleObjectEvent, NULL);
}

void getNowTime()
{
	time_t now;
	time(&now);
	char buff[20];
	strftime(buff, 20, "%Y%m%d%H%M%S", localtime(&now));
	nowtime = (string)buff;
	return;
}
