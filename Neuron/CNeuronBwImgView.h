#pragma once
#include "resource.h"
#include <afxdockablepane.h>
#include "CNeuronOpenCVImageCtrl.h"
#include <opencv.hpp>

using namespace cv;


class CNeuronBwImgView :
	public CDockablePane
{
public:
	CNeuronBwImgView();
	virtual ~CNeuronBwImgView();
	void AdjustLayout();
	void SetImagePath(const CString strImagePath);
	Mat* GetImage() { return m_ctrlStatic.GetImage(); };

protected:
	DECLARE_DYNAMIC(CNeuronBwImgView)

private:
	CNeuronOpenCVImageCtrl    m_ctrlStatic;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

