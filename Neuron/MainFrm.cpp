
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "Neuron.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND(ID_FILE_OPENIMAGE, &CMainFrame::OnFileOpenimage)
	ON_COMMAND(ID_FILE_OPENMNISTDATASET, &CMainFrame::OnFileOpenmnistdataset)
	ON_COMMAND(ID_FILE_EXPORTMNIST, &CMainFrame::OnFileExportmnist)
	ON_UPDATE_COMMAND_UI(ID_IMAGEPROCESSING_CONVERT2BLACKWHITE, &CMainFrame::OnUpdateImageprocessingConvert2blackwhite)
	ON_UPDATE_COMMAND_UI(ID_IMAGEPROCESSING_CONVERT2GRAY, &CMainFrame::OnUpdateImageprocessingConvert2gray)
	ON_COMMAND(ID_IMAGEPROCESSING_STARTPROCESS, &CMainFrame::OnImageprocessingStartprocess)
	ON_COMMAND(ID_IMAGEPROCESSING_CONVERT2GRAY, &CMainFrame::OnImageprocessingConvert2gray)
	ON_COMMAND(ID_IMAGEPROCESSING_CONVERT2BLACKWHITE, &CMainFrame::OnImageprocessingConvert2blackwhite)
	ON_COMMAND(ID_TRAIN_STARTSTORING, &CMainFrame::OnTrainStartstoring)
	ON_COMMAND(ID_TRAIN_STARTRETRIEVAL, &CMainFrame::OnTrainStartretrieval)
	ON_COMMAND(ID_TRAIN, &CMainFrame::OnTrain)
	ON_MESSAGE(WM_TRAINVIEW_SHOW, &CMainFrame::OnTrainViewShow)
	ON_COMMAND(ID_EVALUATION_P_KC_LEN, &CMainFrame::OnEvaluationPKcLen)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
	m_iSetCvt2Gary = 0;
	m_iSetCvt2Bw = 0;
	m_nCols = 0;
	m_nRows = 0;
}

CMainFrame::~CMainFrame()
{
	//m_net._free_netwrok();
	m_net._free_smallworld_network();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == nullptr)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	if (!m_wndTrainConfView.Create(_T("Network configuration view"), this, CRect(0, 50, 200, 200), TRUE, ID_NEURON_TRAIN_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		return FALSE;
	}
	m_wndTrainConfView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndTrainConfView.ShowPane(FALSE, FALSE, FALSE);

	if (!m_wndTrainView.Create(_T("Train view"), this, CRect(0, 50, 200, 200), TRUE, ID_NEURON_TRAIN_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		return FALSE; // failed to create
	}
	m_wndTrainView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndTrainView.ShowPane(FALSE, FALSE, FALSE);

	if (!m_wndRetrievalView.Create(_T("Retrieval view"), this, CRect(0, 50, 200, 200), TRUE, ID_NEURON_RETRIEVAL_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
	  return FALSE; // failed to create
	}
	m_wndRetrievalView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndRetrievalView.ShowPane(FALSE, FALSE, FALSE);

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

void CMainFrame::OnFileOpenimage()
{
	// TODO: Add your command handler code here
	CFileDialog fileDlg(TRUE, _T("*.bmp"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT,
		_T("Image file(*.jpg)|*.jpg|Image file(*.bmp)|*.bmp| All Files (*.*) |*.*||"));
	CNeuronImageView* pImgView = NULL;
	if (fileDlg.DoModal() == IDOK)
	{
		POSITION pos = fileDlg.GetStartPosition();
		CString strFilename;
		while (pos != NULL)
		{
			strFilename = fileDlg.GetNextPathName(pos);
			pImgView = new CNeuronImageView();
			pImgView->Create(_T("Image view"), this, CRect(0, 0, 200, 400), TRUE, iNumImgView + ID_NEURON_IMAGE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI);
			pImgView->EnableDocking(CBRS_ALIGN_ANY);
			pImgView->SetImagePath(strFilename);
			m_viewList.AddTail(pImgView);
			pImgView->ShowPane(TRUE, FALSE, TRUE);
			DockPane(pImgView);
			iNumImgView++;
		}
	}
	AdjustFrmPaneLayout();
}

void CMainFrame::OnFileOpenmnistdataset()
{
	// TODO: Add your command handler code here
	CFileDialog fileDlg(TRUE, _T("*.*"), NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("Mnist data file(*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		m_strMnistFolderPath.Format(_T("%s"),fileDlg.GetFolderPath());
		m_strMnistTrainImagesPath.Format(_T("%s\\%s"), fileDlg.GetFolderPath(), szMnist_train_image);
		m_strMnistTrainLabelsPath.Format(_T("%s\\%s"), fileDlg.GetFolderPath(), szMnist_train_label);
		m_strMnistTestImagesPath.Format(_T("%s\\%s"), fileDlg.GetFolderPath(), szMnist_test_image);
		m_strMnistTestLabelsPath.Format(_T("%s\\%s"), fileDlg.GetFolderPath(), szMnist_test_label);
	}

}

void CMainFrame::OnFileExportmnist()
{
	// TODO: Add your command handler code here
	if (!CheckMnistFilesExit())
	{
		return;
	}

	ifstream mnist_train_images(m_strMnistTrainImagesPath, ios::in | ios::binary);
	ifstream mnist_train_labels(m_strMnistTrainLabelsPath, ios::in | ios::binary);
	ifstream mnist_test_images(m_strMnistTestImagesPath, ios::in | ios::binary);
	ifstream mnist_test_labels(m_strMnistTestLabelsPath, ios::in | ios::binary);

	uint32_t magic;
	uint32_t num_images;
	uint32_t num_Labels;
	uint32_t rows;
	uint32_t cols;

	if (CheckMnistFileContents())
	{
		mnist_train_images.read(reinterpret_cast<char*>(&magic), 4);
		magic = ReverseInt(magic);
		mnist_train_images.read(reinterpret_cast<char*>(&num_images), 4);
		num_images = ReverseInt(num_images);
		mnist_train_images.read(reinterpret_cast<char*>(&rows), 4);
		rows = ReverseInt(rows);
		mnist_train_images.read(reinterpret_cast<char*>(&cols), 4);
		cols = ReverseInt(cols);

		mnist_train_labels.read(reinterpret_cast<char*>(&num_Labels), 4);
		num_Labels = ReverseInt(num_Labels);

		for (uint i = 0; i < num_images; i++)
		{
			Mat tempImg(rows, cols, CV_8UC1);

			char*  pixels = (char*)malloc(sizeof(char)*rows*cols);
			mnist_train_images.read(pixels, rows * cols);

			char label;
			mnist_train_labels.read(&label, 1);

			for (uint x = 0; x < rows; x++)
			{
				uchar* ptr = tempImg.ptr<uchar>(x);
				for (uint y = 0; y < cols; y++)
				{
					if (pixels[x * cols + y] == 0)
						ptr[y] = 0;
					else
						ptr[y] = 255;
				}
			}

			CString strTemp;
			strTemp.Format(_T("%s//%d.jpg"), m_strMnistFolderPath, i);
			string strExportFile = CStringA(strTemp);
			imwrite(strExportFile, tempImg);
			free(pixels);
		}

	}

	mnist_train_images.close();
	mnist_train_labels.close();
	mnist_test_images.close();
	mnist_test_labels.close();
}

void CMainFrame::OnUpdateImageprocessingConvert2blackwhite(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_iSetCvt2Bw);
}

void CMainFrame::OnUpdateImageprocessingConvert2gray(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_iSetCvt2Gary);
}

void CMainFrame::OnImageprocessingConvert2gray()
{
	// TODO: Add your command handler code here
	if (m_iSetCvt2Gary == 0)
	{
		m_iSetCvt2Gary = 1;
	}
	else
	{
		m_iSetCvt2Gary = 0;
	}
}

void CMainFrame::OnImageprocessingConvert2blackwhite()
{
	// TODO: Add your command handler code here
	if (m_iSetCvt2Bw == 0)
	{
		m_iSetCvt2Bw = 1;
	}
	else
	{
		m_iSetCvt2Bw = 0;
	}
}

void CMainFrame::OnImageprocessingStartprocess()
{
	// TODO: Add your command handler code here

	POSITION pos = m_viewList.GetHeadPosition();
	
	CObject* pObject = NULL;
	CNeuronImageView*   pImgView = NULL;
	CNeuronGrayImgView* pGrayImgView = NULL;
	CNeuronBwImgView*   pBwImgView = NULL;
	
	while (pos != NULL)
	{
		pObject = m_viewList.GetNext(pos);
		if (pObject->IsKindOf(RUNTIME_CLASS(CNeuronImageView)))
		{
			pImgView = (CNeuronImageView *)pObject;
			pImgView->ShowPane(TRUE, FALSE, TRUE);
			if (m_iSetCvt2Gary >= 1)
			{
				if (m_grayImgList.Find(pImgView->GetImagePath()) == NULL)
				{
					pGrayImgView = new CNeuronGrayImgView();
					pGrayImgView->Create(_T("Gray Image view"), this, CRect(0, 50, 200, 400), TRUE, iNumGrayImgView + ID_NEURON_GRAY_IMAGE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI);
					pGrayImgView->EnableDocking(CBRS_ALIGN_ANY);
					pGrayImgView->SetImagePath(pImgView->GetImagePath());
					m_viewList.AddTail(pGrayImgView);
					m_grayImgList.AddTail(pImgView->GetImagePath());
					DockPane(pGrayImgView);
					iNumGrayImgView++;
				}
     		}

			if (m_iSetCvt2Bw >= 1)
			{
				if (m_bwImgList.Find(pImgView->GetImagePath()) == NULL)
				{
					pBwImgView = new CNeuronBwImgView();
					pBwImgView->Create(_T("Black white Image view"), this, CRect(0, 50, 200, 400), TRUE, iNumBwImgView + ID_NEURON_BLACKWHITE_IMAGE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI);
					pBwImgView->EnableDocking(CBRS_ALIGN_ANY);
					pBwImgView->SetImagePath(pImgView->GetImagePath());
					m_viewList.AddTail(pBwImgView);
					m_bwImgList.AddTail(pImgView->GetImagePath());
					DockPane(pBwImgView);
					iNumBwImgView++;
				}
			}
		}
	}

	AdjustFrmPaneLayout();
}

void CMainFrame::OnTrain()
{
	// TODO: Add your command handler code here
	m_wndTrainConfView.ShowPane(TRUE, FALSE, TRUE);
	DockPane(&m_wndTrainConfView);
}

void CMainFrame::OnTrainStartstoring()
{
	// TODO: Add your command handler code here
	iNumberOfTrainBwView = CheckBwImgViewNumber();

	if (iNumberOfTrainBwView <= 0)
	{
		return;
	}

	m_hThread = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)TrainNetworkThread,
		this,
		0,
		&m_dwThreadID);

}

void CMainFrame::OnTrainStartretrieval()
{
	// TODO: Add your command handler code here
	if (m_nRows == 0 || m_nCols == 0)
	{
		return;
	}

	scalar* pattern = m_net._get_current_pattern();
	
	m_wndRetrievalView.SetRetrievalImage(pattern, m_nRows, m_nCols);
	m_wndRetrievalView.ShowPane(TRUE, FALSE, TRUE);
	DockPane(&m_wndTrainView);
	
}

void CMainFrame::AdjustFrmPaneLayout()
{
	POSITION pos = m_viewList.GetHeadPosition();
	CObject* pObject = NULL;
	CNeuronImageView*   pImgView = NULL;
	CNeuronGrayImgView* pGrayImgView = NULL;
	CNeuronBwImgView*   pBwImgView = NULL;

	while (pos != NULL)
	{
		pObject = m_viewList.GetNext(pos);
		if (pObject->IsKindOf(RUNTIME_CLASS(CNeuronImageView)))
		{
			pImgView = (CNeuronImageView*)pObject;
			pImgView->ShowPane(TRUE, FALSE, TRUE);
			DockPane(pImgView);
		}
		else if (pObject->IsKindOf(RUNTIME_CLASS(CNeuronGrayImgView)))
		{
			pGrayImgView = (CNeuronGrayImgView*)pObject;
			pGrayImgView->ShowPane(TRUE, FALSE, TRUE);
			DockPane(pGrayImgView);
		}
		else if (pObject->IsKindOf(RUNTIME_CLASS(CNeuronBwImgView)))
		{
			pBwImgView = (CNeuronBwImgView*)pObject;
			pBwImgView->ShowPane(TRUE, FALSE, TRUE);
			DockPane(pBwImgView);
		}
	}

}

int CMainFrame::CheckBwImgViewNumber()
{
	int iBwImgViewNum = 0;

	POSITION pos = m_viewList.GetHeadPosition();
	
	CObject* pObject = NULL;
	int iCol = 0;
	int iRow = 0;
	CNeuronBwImgView* pBwImgView = NULL;
	
	while (pos != NULL)
	{
		pObject = m_viewList.GetNext(pos);
		if (pObject->IsKindOf(RUNTIME_CLASS(CNeuronBwImgView)))
		{
			pBwImgView = (CNeuronBwImgView*)pObject;
			Mat* pTempImg = pBwImgView->GetImage();
			if (pBwImgView->IsWindowVisible())
			{
				if (iBwImgViewNum > 0)
				{
					if (iCol != pTempImg->cols || iRow != pTempImg->rows)
					{
						return -1;
					}
				}

				iCol = pTempImg->cols;
				iRow = pTempImg->rows;
				iBwImgViewNum++;
			}
		}
	}
	m_nCols = iCol;
	m_nRows = iRow;
	return iBwImgViewNum;
}

UINT WINAPI CMainFrame::TrainNetworkThread(LPVOID pParam)
{
	CMainFrame* pThis = (CMainFrame*)pParam;

	double** _patterns = (scalar**)malloc(sizeof(scalar*)*iNumberOfTrainBwView);
	int iPatterns = 0;
	int iNeurons = 0;
	POSITION pos = pThis->m_viewList.GetHeadPosition();
	CObject* pObject = NULL;
	CNeuronBwImgView* pBwImgView = NULL;

	while (pos != NULL)
	{
		pObject = pThis->m_viewList.GetNext(pos);
		if (pObject->IsKindOf(RUNTIME_CLASS(CNeuronBwImgView)))
		{
			pBwImgView = (CNeuronBwImgView*)pObject;
			if (pBwImgView->IsWindowVisible())
			{
				Mat* pTempImg = pBwImgView->GetImage();
				Mat Image = pTempImg->clone();
				iNeurons = Image.cols * Image.rows;
				cvtColor(Image, Image, CV_RGB2GRAY);
				threshold(Image, Image, 170, 255, CV_THRESH_BINARY);

				_patterns[iPatterns] = (scalar*)malloc(sizeof(scalar) * iNeurons);

				for (int i = 0; i < Image.rows; i++)
				{
					for (int j = 0; j < Image.cols; j++)
					{
						uchar data = Image.at<uchar>(i, j);
						if (data > 0)
						{
							_patterns[iPatterns][i*Image.cols + j] = 1;
						}
						else
						{
							_patterns[iPatterns][i*Image.rows + j] = 0;
						}
					}

				}
				iPatterns++;
			}
		}
	}

	::PostMessage(pThis->GetSafeHwnd(), WM_TRAINVIEW_SHOW, (WPARAM)iNeurons, NULL);
	::SendMessage(pThis->m_wndTrainConfView.GetSafeHwnd(), WM_TRAINCONFIGVIEW_PREPARE_DATA, NULL, NULL);
	
	bool bMutualAnalysis = pThis->m_wndTrainConfView.getMutualAnalysis();
	int iWeightMethod = pThis->m_wndTrainConfView.getWeightMethod();
	pThis->m_wndTrainView.SetTopology(pThis->m_wndTrainConfView.getTopology());

	if (pThis->m_wndTrainConfView.getTopology() == 1)
	{
		pThis->m_net._free_smallworld_network();
		double dRewireProbability = pThis->m_wndTrainConfView.getRewireProbability();
		int iKneurons = pThis->m_wndTrainConfView.getKneuron();
		pThis->m_wndTrainView.SetKneuron(pThis->m_wndTrainConfView.getKneuron());
		pThis->m_net._allocate_smallworld_network(iNeurons, _patterns, iNumberOfTrainBwView,iKneurons,dRewireProbability, &pThis->m_wndTrainView);
		pThis->m_net._train_smallworld_netwrok(iWeightMethod);
	}
	else
	{
		pThis->m_net._free_netwrok();
		if (!pThis->m_net._allocate_network(iNeurons, _patterns, iNumberOfTrainBwView, &pThis->m_wndTrainView))
		{
			return 0;
		}
		pThis->m_net._train_network(iWeightMethod);
	}


	for (int p = 0; p < iPatterns; p++)
	{
		free(_patterns[p]);
	}

	return 0;
}

LRESULT CMainFrame::OnTrainViewShow(WPARAM wParam, LPARAM lParam)
{
	int iNeurons = (int)wParam;
	m_wndTrainView.SetRangeOfProgressBar(iNeurons);
	m_wndTrainView.ShowPane(TRUE, FALSE, TRUE);
	DockPane(&m_wndTrainView);
	return 0l;
}

bool CMainFrame::CheckMnistFilesExit()
{
	ifstream mnist_train_images(m_strMnistTrainImagesPath, ios::in | ios::binary);
	ifstream mnist_train_labels(m_strMnistTrainLabelsPath.GetBuffer(0), ios::in | ios::binary);
	ifstream mnist_test_images(m_strMnistTestImagesPath.GetBuffer(0), ios::in | ios::binary);
	ifstream mnist_test_labels(m_strMnistTestLabelsPath.GetBuffer(0), ios::in | ios::binary);
	
	if (mnist_train_images.is_open() == false)
	{
		return false;
	}

	if (mnist_train_labels.is_open() == false)
	{
		return false;
	}

	if (mnist_test_images.is_open() == false)
	{
		return false;
	}

	if (mnist_test_labels.is_open() == false)
	{
		return false;
	}

	mnist_train_images.close();
	mnist_train_labels.close();
	mnist_test_images.close();
	mnist_test_labels.close();

	return true;
}

bool CMainFrame::CheckMnistFileContents()
{
	ifstream mnist_train_images(m_strMnistTrainImagesPath, ios::in | ios::binary);
	ifstream mnist_train_labels(m_strMnistTrainLabelsPath, ios::in | ios::binary);
	ifstream mnist_test_images(m_strMnistTestImagesPath, ios::in | ios::binary);
	ifstream mnist_test_labels(m_strMnistTestLabelsPath, ios::in | ios::binary);

	uint32_t magic;
	uint32_t num_images;
	uint32_t num_labels;

	if (mnist_train_images.is_open())
	{
		mnist_train_images.read(reinterpret_cast<char*>(&magic), 4);
		magic = ReverseInt(magic);
		if (magic != 2051)
		{
			return false;
		}
	}
	
	mnist_train_labels.read(reinterpret_cast<char*>(&magic), 4);
	magic = ReverseInt(magic);
	if (magic != 2049)
	{
		return false;
	}

	mnist_train_images.read(reinterpret_cast<char*>(&num_images), 4);
	num_images = ReverseInt(num_images);

	mnist_train_labels.read(reinterpret_cast<char*>(&num_labels), 4);
	num_labels = ReverseInt(num_labels);

	if (num_images != num_labels)
	{
		return false;
	}

	mnist_train_images.close();
	mnist_train_labels.close();
	mnist_test_images.close();
	mnist_test_labels.close();

	return true;
}

void CMainFrame::OnEvaluationPKcLen()
{
	// TODO: Add your command handler code here
	iNumberOfTrainBwView = CheckBwImgViewNumber();

	if (iNumberOfTrainBwView <= 0)
	{
		return;
	}

	m_hThread = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)EvaluateProbabilityOfClusteringCoefficientAndAvgLength,
		this,
		0,
		&m_dwThreadID);

}

UINT WINAPI CMainFrame::EvaluateProbabilityOfClusteringCoefficientAndAvgLength(LPVOID pParam)
{
	CMainFrame* pThis = (CMainFrame*)pParam;

	double** _patterns = (scalar**)malloc(sizeof(scalar*)*iNumberOfTrainBwView);
	int iPatterns = 0;
	int iNeurons = 0;
	POSITION pos = pThis->m_viewList.GetHeadPosition();
	CObject* pObject = NULL;
	CNeuronBwImgView* pBwImgView = NULL;

	while (pos != NULL)
	{
		pObject = pThis->m_viewList.GetNext(pos);
		if (pObject->IsKindOf(RUNTIME_CLASS(CNeuronBwImgView)))
		{
			pBwImgView = (CNeuronBwImgView*)pObject;
			if (pBwImgView->IsWindowVisible())
			{
				Mat* pTempImg = pBwImgView->GetImage();
				Mat Image = pTempImg->clone();
				iNeurons = Image.cols * Image.rows;
				cvtColor(Image, Image, CV_RGB2GRAY);
				threshold(Image, Image, 170, 255, CV_THRESH_BINARY);

				_patterns[iPatterns] = (scalar*)malloc(sizeof(scalar) * iNeurons);

				for (int i = 0; i < Image.rows; i++)
				{
					for (int j = 0; j < Image.cols; j++)
					{
						uchar data = Image.at<uchar>(i, j);
						if (data > 0)
						{
							_patterns[iPatterns][i*Image.cols + j] = 1;
						}
						else
						{
							_patterns[iPatterns][i*Image.rows + j] = 0;
						}
					}

				}
				iPatterns++;
			}
		}
	}

	ofstream csvFile1,csvFile2;
	csvFile1.open("clustering_coefficient.csv", ios::out | ios::trunc);
	csvFile2.open("average_path_length.csv", ios::out | ios::trunc);

	for (int k = 0; k <= iNeurons; k+=5 )
	{
		if (k < 2)
		{
			continue;
		}
		else
		{
			for (double p = 0.0; p <= 1.001; p+=0.05)
			{
				pThis->m_net._allocate_smallworld_network(iNeurons, _patterns, iNumberOfTrainBwView, k, p, &pThis->m_wndTrainView);
				pThis->m_net._caculate_sw_clustering_coefficient();
				pThis->m_net._caculate_sw_average_path_length();

				csvFile1 << pThis->m_net._get_sw_clustering_coefficient() << ',';
				csvFile2 << pThis->m_net._get_sw_average_path_length() << ',';

				pThis->m_net._free_smallworld_network();
			}
			csvFile1 <<endl;
			csvFile2 <<endl;
		}
	}

	csvFile1.close();
	csvFile2.close();
	
	return 0;
}