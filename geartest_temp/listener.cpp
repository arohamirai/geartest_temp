//////////////////////////////////////////////////////////////////////
// Listener.cpp: implementation of the CListener class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Listener.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CvFont font;

CListener::CListener()
{
	r = 0;
	c = 0;
	r1 = 0;
	c1 = 0;
	m_min = 3;
	m_max = 10;
	m_pParent = NULL;
	m_pDrawCWnd = NULL;
	//cvNamedWindow("result",1);
	////////////////////////////
	try
	{
		ModelID.ReadShapeModel(_T("ModelID.shm"));
		ModelID1.ReadShapeModel(_T("ModelID1.shm"));
	}
	catch(HException &except)
	{
		if (except.ErrorCode() == H_ERR_FNF)
		{
			MessageBox(NULL,_T("未发现模板文件，请检查并重试!"),NULL,NULL);
			return;

		}
	}
	GetShapeModelContours(&m_ModelContours, ModelID, 1);
	GetShapeModelContours(&m_ModelContours1, ModelID1, 1);
	CreateDistanceTransformXld(m_ModelContours, "point_to_segment", m_max, &m_DistanceTransformID);
	CreateDistanceTransformXld(m_ModelContours1, "point_to_segment", m_max, &m_DistanceTransformID1);
}

//////////////////////////////////////////////////////////////////////////

CListener::~CListener() 
{
}

//////////////////////////////////////////////////////////////////////////

void CListener::SetParent( CWnd *pParent)
{
	m_pParent = pParent;
}

//////////////////////////////////////////////////////////////////////////
// Notify parent, device is lost.
//////////////////////////////////////////////////////////////////////////

void CListener::deviceLost( Grabber& param) 
{
	if( m_pParent != NULL)
		m_pParent->PostMessage(MESSAGEDEVICELOST,0,0);
}

//////////////////////////////////////////////////////////////////////////
// Set the CWnd that will be used to render the resulting image.
//////////////////////////////////////////////////////////////////////////

void CListener::SetViewCWnd(CWnd *pView)
{
	m_pDrawCWnd = pView;
	RECT r;
	m_pDrawCWnd->GetClientRect(&r);
	m_WindowSize.cx = r.right;
	m_WindowSize.cy = r.bottom;
}
//////////////////////////////////////////////////////////////////////////
// Callback handler.
//////////////////////////////////////////////////////////////////////////

void CListener::frameReady( Grabber& param, smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber)
{
	pBuffer->lock();
	DoImageProcessing( pBuffer );
	action();
	DrawBuffer(pBuffer);
	pBuffer->unlock();
}

//////////////////////////////////////////////////////////////////////////
// Draw the image buffer into the DrawCWnd.
//////////////////////////////////////////////////////////////////////////

void CListener::DrawBuffer( smart_ptr<MemBuffer> pBuffer)
{

	if( m_pDrawCWnd != NULL)
	{
		if( pBuffer != 0 )
		{ 
			CDC *pDC = m_pDrawCWnd->GetDC();
			pDC->SetStretchBltMode(COLORONCOLOR);

			smart_ptr<BITMAPINFOHEADER> pInf = pBuffer->getBitmapInfoHeader();
			
			
			//创建位图信息头，并用colorimg的图像数据填充该位图
			BITMAPINFOHEADER bitHeader;
			bitHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitHeader.biWidth = colorimg.cols;
			bitHeader.biHeight = colorimg.rows;
			bitHeader.biPlanes = 1;
			bitHeader.biBitCount = 24;
			bitHeader.biCompression = BI_RGB;
			bitHeader.biSizeImage = 0;  // zero for BI_RGB
			bitHeader.biXPelsPerMeter = pDC->GetDeviceCaps(LOGPIXELSX);
			bitHeader.biYPelsPerMeter = pDC->GetDeviceCaps(LOGPIXELSY);
			bitHeader.biClrUsed = 3;
			bitHeader.biClrImportant = 0; // zero for all use all colors

			BITMAPINFO bitMapInfo;
			bitMapInfo.bmiHeader = bitHeader;


			
			int nLines = StretchDIBits( 	pDC->GetSafeHdc(),// Handle to the device
											0, 
											0,
											m_WindowSize.cx,	// Dest. rectangle width.
											m_WindowSize.cy,	// Dest. rectangle height.
											0,				// X-coordinate of lower-left corner of the source rect.
											0,				// Y-coordinate of lower-left corner of the source rect.
											pInf->biWidth,	// Source rectangle width.
											pInf->biHeight, // Number of scan lines.
											colorimg.data,//pBuffer->getPtr(),	// Modified address of array with DIB bits.
											reinterpret_cast<LPBITMAPINFO>( &bitMapInfo ),	// Address of structure with bitmap info.
											DIB_RGB_COLORS,	// RGB or palette indices.
											SRCCOPY
											);
			m_pDrawCWnd->ReleaseDC(pDC);
		}	
	}	
}
//////////////////////////////////////////////////////////////////////////
// The image processing is done here. For the sake of simplicity, the
// image data is inverted by this sample code.
//////////////////////////////////////////////////////////////////////////

void CListener::DoImageProcessing( smart_ptr<MemBuffer> pBuffer)
{
	// Get the bitmap info header from the membuffer. It contains the bits per pixel,
	// width and height.
	smart_ptr<BITMAPINFOHEADER> pInf = pBuffer->getBitmapInfoHeader();

	// Now retrieve a pointer to the image. For organization of the image data, please
	// refer to:
	// http://www.imagingcontrol.com/ic/docs/html/class/Pixelformat.htm

	BYTE* pImageData = pBuffer->getPtr();
	m_inBuf = pImageData;
	// Calculate the size of the image.
	//int iImageSize = pInf->biWidth * pInf->biHeight * pInf->biBitCount / 8 ;
	////IplImage *img = cvCreateImage(cvSize(pInf->biWidth,pInf->biHeight),IPL_DEPTH_8U,1);
	//cv::Mat img(pInf->biHeight,pInf->biWidth,CV_8UC1);
	//img.data = pImageData;
	
	// Now loop through the data and change every byte. This small sample inverts every pixel.
	
}
void CListener::action()
{
	// Local iconic variables
	HObject  ho_GrayImage, ho_ImageSmooth;
	HObject  ho_Edges, ho_TransContours, ho_SelectedXLD, ho__TransContours;
	HObject  ho_ContourOut, ho_ContourPart, ho__ContourPart;
	HObject  ho_TransContours1, ho_SelectedXLD1, ho__TransContours1;
	HObject  ho_ContourOut1, ho_ContourPart1, ho__ContourPart1;
	HObject  ho_UnionEdges;
	// Local control variables
	HTuple  hv_ModelRow, hv_ModelColumn;
	HTuple  hv_ModelAngle, hv_ModelScale, hv_ModelScore;
	HTuple  hv_HomMat, hv_HomMat2DInvert, hv_ModelRow1, hv_ModelColumn1;
	HTuple  hv_ModelAngle1, hv_ModelScale1, hv_ModelScore1;
	HTuple  hv_HomMat1, hv_HomMat2DInvert1;
	HTuple  hv_Edges,hv_BigNum,hv_SmallNum;

	Mat srcimg(480,640,CV_8UC1,m_inBuf);//,colorimg;
	cvtColor(srcimg, colorimg, CV_GRAY2RGB );
	

	HImage src("byte",640,480,m_inBuf);
	//src.ReadImage("001.bmp");
	SmoothImage(src, &ho_ImageSmooth, "gauss", 0.5);
	EdgesSubPix(ho_ImageSmooth, &ho_Edges, "canny", 1, 20, 40);
	UnionAdjacentContoursXld(ho_Edges,&ho_UnionEdges,10,1,"attr_keep");
	//xld2vector(colorimg,ho_UnionEdges,CV_RGB(0,0,255));

	FindScaledShapeModel(ho_ImageSmooth, ModelID, HTuple(0).TupleRad(), HTuple(60).TupleRad(), 
		0.85, 1.15, 0.5, 1, 0.5, "least_squares", (HTuple(6).Append(1)), 0.75, &hv_ModelRow, 
		&hv_ModelColumn, &hv_ModelAngle, &hv_ModelScale, &hv_ModelScore);
	//Matching 01: transform the model contours into the detected positions
	if(hv_ModelScore.TupleLength() > 0 )
	{
		r = hv_ModelRow[0].D();
		c = hv_ModelColumn[0].D();

		HomMat2dIdentity(&hv_HomMat);
		HomMat2dScale(hv_HomMat, HTuple(hv_ModelScale[0]), HTuple(hv_ModelScale[0]), 
			0, 0, &hv_HomMat);
		HomMat2dRotate(hv_HomMat, HTuple(hv_ModelAngle[0]), 0, 0, &hv_HomMat);
		HomMat2dTranslate(hv_HomMat, HTuple(hv_ModelRow[0]), HTuple(hv_ModelColumn[0]), 
			&hv_HomMat);
		//show match result
		AffineTransContourXld(m_ModelContours, &ho_TransContours, hv_HomMat);
		xld2vector(colorimg,ho_TransContours,CV_RGB(0,255,0));

		//transate to model and distance transition
		HomMat2dInvert(hv_HomMat, &hv_HomMat2DInvert);
		AffineTransContourXld(ho_UnionEdges, &ho__TransContours, hv_HomMat2DInvert);
		ApplyDistanceTransformXld(ho__TransContours, &ho_ContourOut, m_DistanceTransformID);
		SegmentContourAttribXld(ho_ContourOut, &ho_ContourPart, "distance", "and", m_min, m_max-1);
		//show dismatch parts
		AffineTransContourXld(ho_ContourPart, &ho__ContourPart, hv_HomMat);
		CountObj(ho__ContourPart,&hv_BigNum);
		if ((int)(hv_BigNum.L()) > 0)
			putText(colorimg,"BIG is not OK",cvPoint(10,50),FONT_HERSHEY_SIMPLEX, 1.0,CV_RGB(255,0,0));
			xld2vector(colorimg,ho__ContourPart,CV_RGB(255,0,0));
	}
	else
	{
		r = 100;
		c = 100;
	}

	FindScaledShapeModel(ho_ImageSmooth, ModelID1, HTuple(0).TupleRad(), HTuple(60).TupleRad(), 
		0.85, 1.15, 0.5, 1, 0.5, "least_squares", (HTuple(6).Append(1)), 0.75, &hv_ModelRow1, 
		&hv_ModelColumn1, &hv_ModelAngle1, &hv_ModelScale1, &hv_ModelScore1);
	//Matching 0: transform the model contours into the detected positions
	int m = hv_ModelScore1.Length();
	if(hv_ModelScore1.TupleLength()>0)
	{
		r1 = hv_ModelRow1[0].D();
		c1 = hv_ModelColumn1[0].D();

		HomMat2dIdentity(&hv_HomMat1);
		HomMat2dScale(hv_HomMat1, HTuple(hv_ModelScale1[0]), HTuple(hv_ModelScale1[0]), 
			0, 0, &hv_HomMat1);
		HomMat2dRotate(hv_HomMat1, HTuple(hv_ModelAngle1[0]), 0, 0, 
			&hv_HomMat1);
		HomMat2dTranslate(hv_HomMat1, HTuple(hv_ModelRow1[0]), HTuple(hv_ModelColumn1[0]), 
			&hv_HomMat1);
		//show match result
		AffineTransContourXld(m_ModelContours1, &ho_TransContours1, hv_HomMat1);
		xld2vector(colorimg,ho_TransContours1,CV_RGB(0,255,0));
		//transate to model
		HomMat2dInvert(hv_HomMat1, &hv_HomMat2DInvert1);
		AffineTransContourXld(ho_UnionEdges, &ho__TransContours1, hv_HomMat2DInvert1);
		//distance transition
		ApplyDistanceTransformXld(ho__TransContours1, &ho_ContourOut1, m_DistanceTransformID1);
		SegmentContourAttribXld(ho_ContourOut1, &ho_ContourPart1, "distance", "and", m_min,m_max-1);
		//show dismatch parts
		AffineTransContourXld(ho_ContourPart1, &ho__ContourPart1, hv_HomMat1);
		CountObj(ho__ContourPart1,&hv_SmallNum);
		if ((int)(hv_SmallNum.L()) > 0)
			putText(colorimg,"Small is not OK",cvPoint(10,100),FONT_HERSHEY_SIMPLEX, 1.0,CV_RGB(255,0,0));
			xld2vector(colorimg,ho__ContourPart1,CV_RGB(255,0,0));
	}
	else
	{

		r1 = 100;
		c1 = 100;
	}

	int x1 = hv_ModelScore.Length();
	int x2 = hv_ModelScore1.Length();
	int x3 = hv_SmallNum.Length();
	int x4 = hv_BigNum.Length();

	if ( (hv_ModelScore.Length() != 0) && (hv_ModelScore1.Length() != 0) && (hv_SmallNum.Length() == 0) && (hv_BigNum.Length() == 0))
	{
		MessageBox(NULL,_T("dd"),NULL,MB_YESNO);
		putText(colorimg,"OK",cvPoint(10,50),FONT_HERSHEY_SIMPLEX, 1.0,CV_RGB(0,255,0));
	}
	// show ceter of the match parts
	/*line(colorimg,Point(c- length,r),Point(c + length, r),Scalar(0,0,255),2);
	line(colorimg,Point(c , r- length),Point(c, r + length),Scalar(0,0,255),2);
	circle(colorimg,Point(c1,r1),10,Scalar(0,255,0),3);*/
	//CameraShowBufferImage(pShowWnd,colorimg.data, 640, 480, true, true);
	cv::circle(colorimg,cvPoint(100,100),30,CV_RGB(255,0,0));
	//m_inBufcolor = colorimg.data;
	//imshow("result",colorimg);
}


void CListener::xld2vector(Mat &mat, HObject &xld, Scalar color)
{
	vector<vector<Point>> contours;
	HObject ho_xld;
	HTuple hv_Number,hv_Row,hv_Col;
	CountObj(xld, &hv_Number);
	int numObj = hv_Number.L();
	if (numObj > 0)
	{
		for (int i=1; i<=numObj; i++)
		{
			vector<Point> tempVec;
			SelectObj(xld, &ho_xld, i);
			GetContourXld(ho_xld, &hv_Row, &hv_Col);

			int numPoint =  hv_Col.Length();
			for (int j = 0; j < numPoint; j++)
			{
				tempVec.push_back(Point(hv_Col.DArr()[j],hv_Row.DArr()[j]));
			}
			contours.push_back(tempVec);
		}
		for (int i = 0; i < numObj; i ++)
		{
			drawContours(mat,contours,i-1,color);
		}
	}else
		return;

}