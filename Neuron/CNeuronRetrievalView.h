#pragma once
#include <afxdockablepane.h>
#include "CNeuronOpenCVImageCtrl.h"
#include "MessageHeader.h"
#include "resource.h"

class CNeuronRetrievalView :
	public CDockablePane
{
public:
	CNeuronRetrievalView();
	virtual ~CNeuronRetrievalView();
	void AdjustLayout();
	void SetRetrievalImage(double* pattern, int iRow, int iCol);

private:
	CNeuronOpenCVImageCtrl    m_ctrlStatic;

protected:
	DECLARE_DYNAMIC(CNeuronRetrievalView)
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

