#include <iostream>
#include "EDSDK.h"

using namespace std;

int main()
{
	//EDSDK �ʱ� ��������
	EdsError err = EDS_ERR_OK;
	EdsCameraRef camera = NULL;
	EdsCameraListRef cameraList = NULL;
	EdsUInt32 count1 = 0;
	bool isSDKLoaded = false;

	//SDK ����
	err = EdsInitializeSDK();
	if (err == EDS_ERR_OK)
	{
		isSDKLoaded = true;
	}

	//ī�޶� �ҷ�����
	if (err = EDS_ERR_OK)
	{
		err = EdsGetCameraList(&cameraList); //ī�޶� ����Ʈ �޾ƿ���
		//ī�޶� ���� �޾ƿ���
		if (err = EDS_ERR_OK)
		{
			err = EdsGetChildCount(cameraList, &count1);
			if (count1 == 0)
			{
				err = EDS_ERR_DEVICE_NOT_FOUND;
			}
		}

		if (err == EDS_ERR_OK)
		{
			err = EdsGetChildAtIndex(cameraList, 0, &camera);
		}

		//release camera list
		if (cameraList != NULL)
		{
			EdsRelease(cameraList);
			cameraList = NULL;
		}

		//Set event handler

		//Open session
		if (err = EDS_ERR_OK)
		{
			err = EdsOpenSession(camera);
		}

		//do something
		err = EdsSendCommand(camera, kEdsCameraCommand_TakePicture, 0);

		//close session
		if (err = EDS_ERR_OK)
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
