#include "pch.h"
#include "CNeuronOpenCVImageCtrl.h"

BEGIN_MESSAGE_MAP(CNeuronOpenCVImageCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CNeuronOpenCVImageCtrl::CNeuronOpenCVImageCtrl()
{
	m_strImagePath = _T("");
	m_bPrepared = false;
}

CNeuronOpenCVImageCtrl::~CNeuronOpenCVImageCtrl()
{

}

void CNeuronOpenCVImageCtrl::PrepareImage(const CString strImagePath, const UINT nImage)
{
	m_strImagePath = strImagePath;
	
	USES_CONVERSION;
	m_Image = imread(W2A(m_strImagePath));

	switch (nImage)
	{
	case 0:
		break;
	case 1: //gray image
		cvtColor(m_Image, m_Image, CV_RGB2GRAY);
		break;
	case 2: // black white image
		cvtColor(m_Image, m_Image, CV_RGB2GRAY);
		threshold(m_Image, m_Image, 170, 255, CV_THRESH_BINARY);
		break;
	}

	switch (m_Image.channels())
	{
	 case 1:
		cvtColor(m_Image, m_Image, CV_GRAY2BGRA);
		break;
	 case 3:
		cvtColor(m_Image, m_Image, CV_BGR2BGRA);
		break;
	 default:
		break;
	}

	int pixelBytes = m_Image.channels()*(m_Image.depth() + 1);

	m_bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	m_bitInfo.bmiHeader.biWidth = m_Image.cols;
	m_bitInfo.bmiHeader.biHeight = -m_Image.rows;
	m_bitInfo.bmiHeader.biPlanes = 1;
	m_bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bitInfo.bmiHeader.biCompression = BI_RGB;
	m_bitInfo.bmiHeader.biClrImportant = 0;
	m_bitInfo.bmiHeader.biClrUsed = 0;
	m_bitInfo.bmiHeader.biSizeImage = 0;
	m_bitInfo.bmiHeader.biXPelsPerMeter = 0;
	m_bitInfo.bmiHeader.biYPelsPerMeter = 0;

	m_bPrepared = true;
}

void CNeuronOpenCVImageCtrl::PrepareRetrievalImage(double* pattern,int iRow,int iCol)
{
	Mat image(iRow, iCol, CV_8UC1);
	for (int i = 0; i < iRow; i++)
	{
		for (int j = 0; j < iCol; j++)
		{
			if (pattern[i*iRow + j] == 1)
			{
				image.at<uchar>(i, j) = 255;
			}
			else
			{
				image.at<uchar>(i, j) = 0;
			}
		}
	}
	
	m_Image = image.clone();

	switch (m_Image.channels())
	{
	case 1:
		cvtColor(m_Image, m_Image, CV_GRAY2BGRA);
		break;
	case 3:
		cvtColor(m_Image, m_Image, CV_BGR2BGRA);
		break;
	default:
		break;
	}

	int pixelBytes = m_Image.channels()*(m_Image.depth() + 1);

	m_bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	m_bitInfo.bmiHeader.biWidth = m_Image.cols;
	m_bitInfo.bmiHeader.biHeight = -m_Image.rows;
	m_bitInfo.bmiHeader.biPlanes = 1;
	m_bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bitInfo.bmiHeader.biCompression = BI_RGB;
	m_bitInfo.bmiHeader.biClrImportant = 0;
	m_bitInfo.bmiHeader.biClrUsed = 0;
	m_bitInfo.bmiHeader.biSizeImage = 0;
	m_bitInfo.bmiHeader.biXPelsPerMeter = 0;
	m_bitInfo.bmiHeader.biYPelsPerMeter = 0; 

	m_bPrepared = true;
}


void CNeuronOpenCVImageCtrl::DrawImage()
{
	if (!m_bPrepared)
	{
		return;
	}

	CDC *pDC = this->GetDC();

	StretchDIBits(
		pDC->GetSafeHdc(),
		0, 0, m_Image.cols, m_Image.rows,
		0, 0, m_Image.cols, m_Image.rows,
		m_Image.data,
		&m_bitInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
	
	ReleaseDC(pDC);
}

void CNeuronOpenCVImageCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CStatic::OnPaint() for painting messages
	DrawImage();
}

BOOL CNeuronOpenCVImageCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect = { 0 };
	GetClientRect(&rect); 
	pDC->FillRect(&rect, &CBrush(RGB(255, 255, 255)));

	return CStatic::OnEraseBkgnd(pDC);
}
