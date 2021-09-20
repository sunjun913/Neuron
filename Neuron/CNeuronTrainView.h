#pragma once
#include <afxdockablepane.h>
#include "CNeuronOpenCVImageCtrl.h"
#include "MessageHeader.h"
#include "resource.h"

class CNeuronTrainView :
	public CDockablePane
{
public:
	CNeuronTrainView();
	virtual ~CNeuronTrainView();
	void SetTopology(const int nTopology) {
		m_nTopology = nTopology;}
	void AdjustLayout();
	void SetRangeOfProgressBar(const int iNeuron) {
		m_ctrlProgress.SetRange(0, iNeuron-1);
	}
	void SetKneuron(const int iKneurons) {
		m_nKneurons = iKneurons;
	}
private:
	CEdit                     m_ctrlEdit;
	CProgressCtrl             m_ctrlProgress;
	int                       m_nTopology;
	int                       m_nKneurons;

protected:
	DECLARE_DYNAMIC(CNeuronTrainView)
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnTrainProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrainComplete(WPARAM wParam, LPARAM lParam);
};

