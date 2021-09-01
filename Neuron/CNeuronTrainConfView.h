#pragma once
#include <afxdockablepane.h>
#include "MessageHeader.h"

class CNeuronTrainConfView :
	public CDockablePane
{
public:
	CNeuronTrainConfView();
	virtual ~CNeuronTrainConfView();

	bool   getMutualAnalysis() {
		return m_bMutualAnaylsis;}
	int    getTopology() {
		return m_nConnection;}
	double getRewireProbability() {
		return m_dRewireProbability;}
	int    getKneuron() {
		return m_iKneurons;	}
	int    getWeightMethod() {
		return m_nWeightMethod;}


protected:
	void AdjustLayout();
	void InitPropGird();

private:
	bool   m_bMutualAnaylsis;
	int    m_nConnection;
	int    m_nWeightMethod;
	double m_dRewireProbability;
	int    m_iKneurons;
	CMFCPropertyGridCtrl m_ctrlPropGird;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPrepareConfigData(WPARAM wParam, LPARAM lParam);
};

