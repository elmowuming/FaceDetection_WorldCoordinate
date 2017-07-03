#include <stdio.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

#include <Kinect.h>
using namespace cv;
using namespace std;


//-----------------------kinect v2內部參數------------------------------
double camD[9] = { 366.66		, 0			, 261.96,
0		, 366.63	, 207.61,
0		, 0			, 1 };
//---------------------------------------------------------------------
//載入"haarcascade_frontalface_alt.xml"人臉特徵分類器
String face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade("C:/Users/user/Desktop/ColorCamera/ColorCamera/haarcascade_frontalface_alt.xml");

//視窗名稱
String window_name = "Capture - Face detection";
String window_name1 = "Face detection";
//--------------------------------------------------------------------

int main(int argc, char** argv)
{
	// --------------------取得感應器----------------------------------
	cout << "取得感應器" << endl;
	IKinectSensor* pSensor = nullptr;
	if (GetDefaultKinectSensor(&pSensor) != S_OK)
	{
		cerr << "Get Sensor failed" << endl;
		return -1;
	}

	// --------------------打開感應器-----------------------------------
	cout << "打開感應器" << endl << endl;
	if (pSensor->Open() != S_OK)
	{
		cerr << "Can't open sensor" << endl;
		return -1;
	}

	//---------------------------------彩色影像----------------------------------
	IColorFrameReader* pColorFrameReader = nullptr;
	UINT uColorBufferSize = 0;
	UINT uColorPointNum = 0;
	int iColorWidth = 0, iColorHeight = 0;
	cout << "打開彩色影像" << endl;

	{
		IColorFrameSource* pFrameSource = nullptr;
		if (pSensor->get_ColorFrameSource(&pFrameSource) != S_OK)
		{
			cerr << "Can't get color frame source" << endl;
			return -1;
		}

		// 取得 畫面寬度資訊，記錄到iColorWidth、iColorHeight
		cout << "get color frame description" << endl;
		IFrameDescription* pFrameDescription = nullptr;
		if (pFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
		{
			pFrameDescription->get_Width(&iColorWidth);
			pFrameDescription->get_Height(&iColorHeight);

			uColorPointNum = iColorWidth * iColorHeight;
			uColorBufferSize = uColorPointNum * 4 * sizeof(BYTE);

			cout << "彩色影像大小：寬 " << iColorWidth << " , 高 " << iColorHeight << endl;
		}
		pFrameDescription->Release();
		pFrameDescription = nullptr;

		// 取得 彩色影像讀取器
		cout << "Try to get color frame reader" << endl;
		if (pFrameSource->OpenReader(&pColorFrameReader) != S_OK)
		{
			cerr << "Can't get color frame reader" << endl;
			return -1;
		}

		// 釋放 彩色影像讀取器
		cout << "釋放 彩色影像讀取器" << endl << endl;
		pFrameSource->Release();
		pFrameSource = nullptr;
	}


	//-------------------------------深度影像-------------------------------
	IDepthFrameReader* pDepthFrameReader = nullptr;
	UINT uDepthPointNum = 0;
	int iDepthWidth = 0, iDepthHeight = 0;
	cout << "打開深度影像" << endl;
	{
		IDepthFrameSource* pFrameSource = nullptr;
		if (pSensor->get_DepthFrameSource(&pFrameSource) != S_OK)
		{
			cerr << "Can't get depth frame source" << endl;
			return -1;
		}

		// 取得 畫面寬度資訊，記錄到iDepthWidth、iDepthHeight
		cout << "get depth frame description" << endl;
		IFrameDescription* pFrameDescription = nullptr;
		if (pFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
		{
			pFrameDescription->get_Width(&iDepthWidth);
			pFrameDescription->get_Height(&iDepthHeight);

			uDepthPointNum = iDepthWidth * iDepthHeight;
		}
		pFrameDescription->Release();
		pFrameDescription = nullptr;
		cout << "深度影像大小：寬 " << iDepthWidth << " , 高 " << iDepthHeight << endl;

		// 取得 深度影像讀取器
		cout << "取得 深度影像讀取器" << endl;
		if (pFrameSource->OpenReader(&pDepthFrameReader) != S_OK)
		{
			cerr << "Can't get depth frame reader" << endl;
			return -1;
		}

		// 釋放 深度影像讀取器
		cout << "釋放 深度影像讀取器" << endl << endl;
		pFrameSource->Release();
		pFrameSource = nullptr;
	}

	//------------------------------------------座標轉換-----------------------------
	ICoordinateMapper* pCoordinateMapper = nullptr;
	if (pSensor->get_CoordinateMapper(&pCoordinateMapper) != S_OK)
	{
		cerr << "get_CoordinateMapper failed" << endl;
		return -1;
	}

	//-----------------------------------------OpenCV定義圖-----------------------------
	Mat	mImg(iColorHeight, iColorWidth, CV_8UC4);
	Mat mDepthImg(iDepthHeight, iDepthWidth, CV_16UC1);

	//-----------------------------------------While-----------------------------------
	UINT16*				pDepthPoints = new UINT16[uDepthPointNum];


	while (true)
	{
		ColorSpacePoint  ColorPoint;
		DepthSpacePoint DepthPoint;


		UINT16*    nBuffer = nullptr;
		int DepthX = iDepthWidth / 2, DepthY = iDepthHeight / 2;
		size_t idx = DepthX + iDepthWidth * DepthY;


		//---------------------------------------打開深度影像-------------------------------------------------
		IDepthFrame* pDepthFrame = nullptr;
		if (pDepthFrameReader->AcquireLatestFrame(&pDepthFrame) == S_OK)
		{
			pDepthFrame->CopyFrameDataToArray(uDepthPointNum, reinterpret_cast<UINT16*>(mDepthImg.data));
			UINT    nBufferSize = 0;
			pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &nBuffer);
			cout << nBuffer[idx] << endl;


			pDepthFrame->Release();
			pDepthFrame = nullptr;
		}





		//-----------------------------------------------座標轉換---------------------------------------


		for (int i = 0; i < mDepthImg.rows; ++i)
		{
		for (int j = 0; j < mDepthImg.cols; ++j)
		{
		DepthPoint.X = i;
		DepthPoint.Y = j;

		pCoordinateMapper->MapDepthPointToColorSpace(DepthPoint, idx, &ColorPoint);

		}
		}






		//------------------------------------打開彩色影像-------------------------------------
		IColorFrame* pColorFrame = nullptr;
		if (pColorFrameReader->AcquireLatestFrame(&pColorFrame) == S_OK)
		{
			pColorFrame->CopyConvertedFrameDataToArray(uColorBufferSize, mImg.data, ColorImageFormat_Bgra);

			vector<Rect>faces(3);  //vector類型的容器，裡面放很多Rect儲存人臉的矩形左上原點、矩形寬度、長度
			Mat CamImg_gray;

			cvtColor(mImg, CamImg_gray, COLOR_BGR2GRAY);	//轉灰度圖
			equalizeHist(CamImg_gray, CamImg_gray);			//值方圖等化(提高對比度)

															//偵測臉部部分，detectMultiScale為檢測人臉，並用vector保存人臉的座標、大小
			face_cascade.detectMultiScale(CamImg_gray, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(150, 150));


			for (size_t i = 0; i < faces.size(); i++)
			{
				Point center(faces[i].x + faces[i].width, faces[i].y + faces[i].height);	//找人臉中心

																							//畫矩形 (輸入圖, 左上角點Point(x,y),對角線角點Point(右下 x,y), 線的顏色, 線的粗細)
				rectangle(mImg, Point(faces[i].x + 30, faces[i].y), Point(faces[i].x - 30 + faces[i].width, faces[i].y + faces[i].height), Scalar(0, 255, 0), 2);

				

				//計算3D座標
				ColorPoint.X = ((faces[i].x + faces[i].width) / 2 - camD[2]) *nBuffer[idx] / camD[0];
				ColorPoint.Y = ((faces[i].y + faces[i].height) / 2 - camD[5]) / camD[4];

				cout << "2D座標" << "X:" << (faces[i].x + faces[i].width) / 2 << "  Y:" << (faces[i].y + faces[i].height) / 2 << endl;
				cout << "3D座標" << "X:" << ColorPoint.X << "  Y:" << ColorPoint.Y << endl << endl;
				imshow(window_name1, mImg);
			}

			pColorFrame->Release();
			pColorFrame = nullptr;
		}

		if (waitKey(30) == VK_ESCAPE)
		{
			break;
		}
	}




	cout << "Release coordinate mapper" << endl;
	pCoordinateMapper->Release();
	pCoordinateMapper = nullptr;

	// release frame reader
	cout << "Release frame reader" << endl;
	pColorFrameReader->Release();
	pColorFrameReader = nullptr;
	pDepthFrameReader->Release();
	pDepthFrameReader = nullptr;


	// Close Sensor
	cout << "close sensor" << endl;
	pSensor->Close();

	// Release Sensor
	cout << "Release sensor" << endl;
	pSensor->Release();
	pSensor = nullptr;

	return 0;


}
