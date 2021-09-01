#include "pch.h"
#include "CNeuronImageView.h"

IMPLEMENT_DYNAMIC(CNeuronImageView, CDockablePane)

BEGIN_MESSAGE_MAP(CNeuronImageView, CDockablePane)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

CNeuronImageView::CNeuronImageView()
{

}

CNeuronImageView::~CNeuronImageView()
{

}

int CNeuronImageView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE ;

	if (!m_ctrlStatic.Create(nullptr, dwViewStyle, rectDummy,this))
	{
		return -1;
	}

	AdjustLayout();

	return 0;
}

void CNeuronImageView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	AdjustLayout();
}

void CNeuronImageView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_ctrlStatic.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CNeuronImageView::SetImagePath(const CString strImagePath)
{
	m_strImagePath = strImagePath;
	m_ctrlStatic.PrepareImage(m_strImagePath);
}







