#include "pch.h"
#include "CNeuronRetrievalView.h"

BEGIN_MESSAGE_MAP(CNeuronRetrievalView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CNeuronRetrievalView::CNeuronRetrievalView()
{

}

CNeuronRetrievalView::~CNeuronRetrievalView()
{

}

int CNeuronRetrievalView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE;

	if (!m_ctrlStatic.Create(nullptr, dwViewStyle, rectDummy, this))
	{
		return -1;
	}

	AdjustLayout();

	return 0;
}


void CNeuronRetrievalView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustLayout();
}

void CNeuronRetrievalView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_ctrlStatic.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CNeuronRetrievalView::SetRetrievalImage(double* pattern,int iRow,int iCol)
{
	m_ctrlStatic.PrepareRetrievalImage(pattern,iRow,iCol);
}