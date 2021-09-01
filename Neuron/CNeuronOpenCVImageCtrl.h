#pragma once
#include <afxwin.h>
#include <opencv.hpp>

using namespace cv;

class CNeuronOpenCVImageCtrl :
	public CStatic
{
public:
	CNeuronOpenCVImageCtrl();
	virtual ~CNeuronOpenCVImageCtrl();
	void PrepareImage(const CString strImagePath, const UINT nImage = 0);
	void PrepareRetrievalImage(double* pattern,int iRow,int iCol);
	Mat* GetImage() { return &m_Image; }

protected:
	void DrawImage();

private:
	CString    m_strImagePath;
	Mat        m_Image;
	BITMAPINFO m_bitInfo;
	bool       m_bPrepared;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

