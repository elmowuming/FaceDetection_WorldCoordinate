#include <stdio.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include <Kinect.h>
using namespace cv;
using namespace std;

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
	// IKinectSensor(型別) 取得感應器
	// @pSensor 感應器物件
	cout << "Try to get default sensor" << endl;
	IKinectSensor* pSensor = nullptr;
	if (GetDefaultKinectSensor(&pSensor) != S_OK)
	{
		cerr << "Get Sensor failed" << endl;
		return -1;
	}

	// 打開感應器
	cout << "Try to open sensor" << endl;
	if (pSensor->Open() != S_OK)
	{
		cerr << "Can't open sensor" << endl;
		return -1;
	}

	// IDepthFrameSource(型別) 取得彩色影像
	cout << "Try to get color source" << endl;
	IColorFrameSource* pFrameSource = nullptr;
	if (pSensor->get_ColorFrameSource(&pFrameSource) != S_OK)
	{
		cerr << "Can't get color frame source" << endl;
		return -1;
	}

	// @pFrameDescription 取得畫面寬度資訊，記錄到iWidth、iHeight
	cout << "get color frame description" << endl<< endl;
	int		iWidth = 0;
	int		iHeight = 0;
	IFrameDescription* pFrameDescription = nullptr;
	if (pFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
	{
		pFrameDescription->get_Width(&iWidth);
		pFrameDescription->get_Height(&iHeight);
	}
	pFrameDescription->Release();
	pFrameDescription = nullptr;

	cout << "彩色影像大小：寬 " << iWidth << " , 高 " << iHeight << endl <<endl;

	// IDepthFrameReader(型別) 取得彩色影像讀取器
	cout << "Try to get color frame reader" << endl << endl;
	IColorFrameReader* pFrameReader = nullptr;
	if (pFrameSource->OpenReader(&pFrameReader) != S_OK)
	{
		cerr << "Can't get color frame reader" << endl;
		return -1;
	}
	
	//-----------------------------------------------深度-------------------------------
	// IKinectSensor(型別) 取得感應器
	// @pSensor 感應器物件
	IKinectSensor* nSensor = nullptr;
	GetDefaultKinectSensor(&nSensor);
	// 打開感應器
	nSensor->Open();
	// IDepthFrameSource(型別) 取得深度影像來源
	IDepthFrameSource* nFrameSource = nullptr;
	nSensor->get_DepthFrameSource(&nFrameSource);

	// @pFrameDescription 取得畫面寬度資訊，記錄到nWidth、nHeight
	IFrameDescription* nFrameDescription = nullptr;
	nFrameSource->get_FrameDescription(&nFrameDescription);

	int        nWidth = 0;
	int        nHeight = 0;

	nFrameDescription->get_Width(&nWidth);
	nFrameDescription->get_Height(&nHeight);
	nFrameDescription->Release();
	nFrameDescription = nullptr;
	cout << "深度影像大小：寬 " << nWidth << " , 高 " << nHeight << endl << endl;



	// 取得深度 最大、最小值
	UINT16 uDepthMin = 0, uDepthMax = 0;
	nFrameSource->get_DepthMinReliableDistance(&uDepthMin);
	nFrameSource->get_DepthMaxReliableDistance(&uDepthMax);
	cout << "深度範圍: "<< uDepthMin << " – " << uDepthMax << endl << endl;

	// OpenCV定義圖
	Mat mDepthImg(iHeight, nWidth, CV_16UC1);
	Mat mImg8bit(iHeight, nWidth, CV_8UC1);

	// IDepthFrameReader(型別) 取得深度影像讀取器
	IDepthFrameReader* nFrameReader = nullptr;
	nFrameSource->OpenReader(&nFrameReader);
	//------------------------------------------end--------------------------------


	// 釋放 Frame source
	cout << "Release frame source" << endl;
	pFrameSource->Release();
	pFrameSource = nullptr;



	// OpenCV定義圖
	Mat	mImg(iHeight, iWidth, CV_8UC4);
	UINT uBufferSize = iHeight * iWidth * 4 * sizeof(BYTE);
	



	//main loop
	while (true)
	{
		// 打開彩色影像
		IColorFrame* pFrame = nullptr;
		// 打開深度影像
		IDepthFrame* nFrame = nullptr;

		double Threeworld;

		UINT16*    nBuffer = nullptr;
		int nx = nWidth / 2, ny = nHeight / 2;
		size_t idx = nx + nWidth * ny;


		//DepthSpacePoint  DepthPoint = { iWidth,iHeight };
		ColorSpacePoint  ColorPoint;
		

		//ICoordinateMapper * mapper;
		//pSensor->get_CoordinateMapper(&mapper);
		//mapper->MapDepthPointToCameraSpace(iWidth, iHeight, &depth);
		//pCoordinateMapper->MapDepthPointToCameraSpace(nWidth * nHeight, reinterpret_cast<UINT16*>(mDepthImg.data), &depth);
		

		//AcquireLatestFrame 讀取最新資料
		if (nFrameReader->AcquireLatestFrame(&nFrame) == S_OK)
		{
			// 將深度圖複製到圖像
			nFrame->CopyFrameDataToArray(nWidth * nHeight, reinterpret_cast<UINT16*>(mDepthImg.data));
			UINT    nBufferSize = 0;
			nFrame->AccessUnderlyingBuffer(&nBufferSize, &nBuffer);

			cout << nBuffer[idx] << endl;

			//cout << "Depth++" << endl;


			/*for (int ny = 0; ny < nHeight; ++ny)
			{
				for (int nx = 0; nx < nWidth; ++nx)
				{
					DepthSpacePoint  DepthPoint = { static_cast<float>(nx), static_cast<float>(ny) };
					//深度 轉 彩色
					pCoordinateMapper->MapDepthPointToColorSpace(DepthPoint, idx, &ColorPoint);
				}
			}*/

			/*Mat mDepthImg(nHeight, nWidth, CV_16UC1, nBuffer);
			// 從16位轉換為8位
			mDepthImg.convertTo(mImg8bit, CV_8U, 255.0f / uDepthMax);
			imshow("Depth Map", mImg8bit);*/

			// 釋放影像
			nFrame->Release();
		}
			

		if (pFrameReader->AcquireLatestFrame(&pFrame) == S_OK)
		{
			//cout << "Color++" << endl;



				// Copy to OpenCV image
				if (pFrame->CopyConvertedFrameDataToArray(uBufferSize, mImg.data, ColorImageFormat_Bgra) == S_OK)
				{

					//-------------------------------------人臉辨識


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

						for (int j = 0; j < nHeight; ++j)
						{
							for (int i = 0; i < nWidth; ++i)
							{
								DepthSpacePoint  DepthPoint = { static_cast<float>(i), static_cast<float>(j) };
								//深度 轉 彩色
								pCoordinateMapper->MapDepthPointToColorSpace(DepthPoint, idx, &ColorPoint);
							}
						}

						//深度 轉 彩色
						//pCoordinateMapper->MapDepthPointToColorSpace(DepthPoint, idx, &ColorPoint);

						//計算3D座標
						ColorPoint.X = ((faces[i].x + faces[i].width) / 2 - camD[2]) * nBuffer[idx] / camD[0];
						ColorPoint.Y = ((faces[i].y + faces[i].height) / 2 - camD[5])  / camD[4];

						cout << "2D座標" << "X:" << (faces[i].x + faces[i].width) / 2 << "  Y:" << (faces[i].y + faces[i].height) / 2 << endl;
						cout << "3D座標" << "X:" << ColorPoint.X << "  Y:" << ColorPoint.Y << endl << endl;
						imshow(window_name1, mImg);
					}

						}

				else
				{
					cerr << "Data copy error" << endl;
				}
				// 釋放frame
				pFrame->Release();
			}
		

		// 4f. check keyboard input
		if (waitKey(10) == VK_ESCAPE){
			break;
		}
	}

	// 釋放 frame reader
	cout << "Release frame reader" << endl;
	pFrameReader->Release();
	pFrameReader = nullptr;

	// 關閉 Sensor
	cout << "close sensor" << endl;
	pSensor->Close();

	// 釋放 Sensor
	cout << "Release sensor" << endl;
	pSensor->Release();
	pSensor = nullptr;


	nFrameReader->Release();
	nFrameReader = nullptr;
	// 釋放 Frame source
	nFrameSource->Release();
	nFrameSource = nullptr;
	// 關閉 Sensor
	nSensor->Close();
	// 釋放 Sensor
	nSensor->Release();
	nSensor = nullptr;


	return 0;
}

