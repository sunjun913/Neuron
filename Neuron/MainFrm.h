
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ChildView.h"
#include "CNeuronImageView.h"
#include "CNeuronBwImgView.h"
#include "CNeuronGrayImgView.h"
#include "CNeuronHopfieldNetwork.h"
#include "CNeuronTrainView.h"
#include "CNeuronRetrievalView.h"
#include "CNeuronTrainConfView.h"
#include "MessageHeader.h"

#include <thread>
#include <iostream>
#include <fstream>
#include <opencv.hpp>

using namespace cv;
using namespace std;

static int iNumImgView = 0;
static int iNumGrayImgView = 0;
static int iNumBwImgView = 0;
static int iNumberOfTrainBwView = 0;

const static CString szMnist_train_image = _T("train-images.idx3-ubyte");
const static CString szMnist_train_label = _T("train-labels.idx1-ubyte");
const static CString szMnist_test_image = _T("t10k-images.idx3-ubyte");
const static CString szMnist_test_label = _T("t10k-labels.idx1-ubyte");

class CMainFrame : public CFrameWndEx
{
	
public:
	CMainFrame() noexcept;
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);
	virtual BOOL OnCloseDockingPane(CDockablePane* pWnd);
	virtual BOOL OnCloseMiniFrame(CPaneFrameWnd* pWnd);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCToolBarImages m_UserImages;
	CChildView        m_wndView;
	CNeuronTrainView      m_wndTrainView;
	CNeuronTrainConfView  m_wndTrainConfView;
	CNeuronRetrievalView  m_wndRetrievalView;

	CNeuronHopfieldNetwork   m_net;

private:
	int               m_iSetCvt2Gary;
	int               m_iSetCvt2Bw;
	CObList           m_viewList;
	CStringList       m_bwImgList;
	CStringList       m_grayImgList;
	int               m_nCols;
	int               m_nRows;
	HANDLE            m_hThread;
	DWORD             m_dwThreadID;
	CString           m_strMnistTrainImagesPath;
	CString           m_strMnistTrainLabelsPath;
	CString           m_strMnistTestImagesPath;
	CString           m_strMnistTestLabelsPath;
	CString           m_strMnistFolderPath;

	double** sw_clustering_coefficient_mat;
	double** sw_average_path_length_mat;

protected:
	void AdjustFrmPaneLayout();
	int CheckBwImgViewNumber();
	bool CheckImageViewExist(const CString strFilePathname);
	static UINT WINAPI TrainNetworkThread(LPVOID pParam);
	static UINT WINAPI EvaluateProbabilityOfClusteringCoefficientAndAvgLength(LPVOID pParam);
	static UINT WINAPI EvaluateSimilarityOfPK(LPVOID pParam);
	uint32_t ReverseInt(uint32_t value){
		value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
		return (value << 16) | (value >> 16);
	}

	bool CheckMnistFilesExit();
	bool CheckMnistFileContents();

	void RemovePathFromList(const CString strPath, CStringList* pList);
	void RemoveViewFromList(CObject* pObject,CObList* pList);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnFileOpenimage();
	afx_msg void OnFileOpenmnistdataset();
	afx_msg void OnFileExportmnist();
	afx_msg void OnUpdateImageprocessingConvert2blackwhite(CCmdUI *pCmdUI);
	afx_msg void OnUpdateImageprocessingConvert2gray(CCmdUI *pCmdUI);
	afx_msg void OnImageprocessingStartprocess();
	afx_msg void OnImageprocessingConvert2gray();
	afx_msg void OnImageprocessingConvert2blackwhite();
	afx_msg void OnTrainStartstoring();
	afx_msg void OnTrainStartretrieval();
	afx_msg void OnTrain();
	afx_msg void OnEvaluationPKcLen();
	afx_msg void OnEvaluationsSimilarityPK();
	afx_msg LRESULT OnTrainViewShow(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};


