#pragma once
#include <afxdockablepane.h>
#include "CNeuronOpenCVImageCtrl.h"
#include <opencv.hpp>

#include "resource.h"

using namespace cv;

class CNeuronImageView :
	public CDockablePane
{
public:
	CNeuronImageView();
	virtual ~CNeuronImageView();
	void AdjustLayout();
	void SetImagePath(const CString strImagePath);
	CString GetImagePath() { return m_strImagePath; }

protected:	
	DECLARE_DYNAMIC(CNeuronImageView)

private:
	CNeuronOpenCVImageCtrl    m_ctrlStatic;
	CString                   m_strImagePath;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

