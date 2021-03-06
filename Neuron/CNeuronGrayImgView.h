#pragma once
#include <afxdockablepane.h>
#include "CNeuronOpenCVImageCtrl.h"

#include "resource.h"

class CNeuronGrayImgView :
	public CDockablePane
{
public:
	CNeuronGrayImgView();
	virtual ~CNeuronGrayImgView();
	void AdjustLayout();
	void SetImagePath(const CString strImagePath);
    CString GetImagePath() {
		return m_strImagePath;}

protected:
	DECLARE_DYNAMIC(CNeuronGrayImgView)

private:
	CNeuronOpenCVImageCtrl    m_ctrlStatic;
	CString                   m_strImagePath;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

