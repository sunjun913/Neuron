#include "pch.h"
#include "CNeuronTrainConfView.h"

IMPLEMENT_DYNAMIC(CNeuronTrainConfView, CDockablePane)

BEGIN_MESSAGE_MAP(CNeuronTrainConfView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_TRAINCONFIGVIEW_PREPARE_DATA, &CNeuronTrainConfView::OnPrepareConfigData)
END_MESSAGE_MAP()

CNeuronTrainConfView::CNeuronTrainConfView()
{
	m_bPreAnaylsis = false;
	m_nConnection = 0;
	m_nWeightMethod = 0;
	m_dRewireProbability = 0.035;
	m_iKneurons = 20;
}

CNeuronTrainConfView::~CNeuronTrainConfView()
{

}

int CNeuronTrainConfView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_ctrlPropGird.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		return -1;      // fail to create
	}

	InitPropGird();

	AdjustLayout();

	return 0;
}

void CNeuronTrainConfView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustLayout();
}

void CNeuronTrainConfView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_ctrlPropGird.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CNeuronTrainConfView::InitPropGird()
{
	m_ctrlPropGird.EnableHeaderCtrl(FALSE);
	m_ctrlPropGird.EnableDescriptionArea();
	m_ctrlPropGird.SetVSDotNetLook();
	m_ctrlPropGird.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Pre-Analysis"));
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Information analysis"), (_variant_t)false, _T("Specifies if the information analysis is employed in your application.")));

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Topologies"));
	
	CMFCPropertyGridProperty* pProp21 = new CMFCPropertyGridProperty(_T("Connections"), _T("Fully connected"), _T("One of: Fully connected or Small world connected."));
	pProp21->AddOption(_T("Fully connected"));
	pProp21->AddOption(_T("Small world connected"));
	
	CMFCPropertyGridProperty* pProp22 = new CMFCPropertyGridProperty(_T("Small world parameters"));
	pProp22->AddSubItem(new CMFCPropertyGridProperty(_T("Rewire probability"), (_variant_t)0.035f,_T("Specifies rewire probability")));
	pProp22->AddSubItem(new CMFCPropertyGridProperty(_T("K cluster size"), (_variant_t)20l, _T("Specifies K cluster size")));
	
	pGroup2->AddSubItem(pProp21);
	pGroup2->AddSubItem(pProp22);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Weight update method"));
	CMFCPropertyGridProperty* pProp31 = new CMFCPropertyGridProperty(_T("Methods"), _T("Hebian"), _T("One of: Heibian or Wan abdullah."));
	pProp31->AddOption(_T("Hebian"));
	pProp31->AddOption(_T("Wan abdullah"));
	pGroup3->AddSubItem(pProp31);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("States update method"));
	CMFCPropertyGridProperty* pProp41 = new CMFCPropertyGridProperty(_T("Methods"), _T("Signum"), _T("One of: Signum."));
	pProp41->AddOption(_T("Signum"));
	//pProp41->AddOption(_T("Relu"));
	pGroup4->AddSubItem(pProp41);

	m_ctrlPropGird.AddProperty(pGroup1);
	m_ctrlPropGird.AddProperty(pGroup2);
	m_ctrlPropGird.AddProperty(pGroup3);
	m_ctrlPropGird.AddProperty(pGroup4);
}

void CNeuronTrainConfView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	m_ctrlPropGird.SetFocus();
}

LRESULT CNeuronTrainConfView::OnPrepareConfigData(WPARAM wParam, LPARAM lParam)
{
	int count = m_ctrlPropGird.GetPropertyCount();

	for (int i = 0; i < count; i++)
	{
		CMFCPropertyGridProperty* pProperty = m_ctrlPropGird.GetProperty(i);
		int subItemCount = pProperty->GetSubItemsCount();
		for (int j = 0; j < subItemCount; j++)
		{
			CMFCPropertyGridProperty *pSubProperty = pProperty->GetSubItem(j);
			CString strName = pSubProperty->GetName();
			COleVariant var = pSubProperty->GetValue();
			if (strName == _T("Information analysis") && i==0)
			{
				m_bPreAnaylsis = var.boolVal;
			}
			if (strName == _T("Small world parameters") && i == 1 && j == 1)
			{
				CMFCPropertyGridProperty *pSubItem1 = pSubProperty->GetSubItem(0);
				CMFCPropertyGridProperty *pSubItem2 = pSubProperty->GetSubItem(1);

				COleVariant var1 = pSubItem1->GetValue();
				COleVariant var2 = pSubItem2->GetValue();
				m_dRewireProbability = var1.fltVal;
				m_iKneurons = var2.intVal;
			}
			if (strName == _T("Connections") && i==1)
			{
				CString strTemp = var.bstrVal;
				if (strTemp == _T("Fully connected"))
				{
					m_nConnection = 0;
				}
				if (strTemp == _T("Small world connected"))
				{
					m_nConnection = 1;
				}
			}
			if (strName == _T("Methods") && i == 2)
			{
				CString strTemp = var.bstrVal;
				if (strTemp == _T("Hebian"))
				{
					m_nWeightMethod = 0;
				}
				if (strTemp == _T("Wan abdullah"))
				{
					m_nWeightMethod = 1;
				}
			}
		}
	}

	return 0;
}

