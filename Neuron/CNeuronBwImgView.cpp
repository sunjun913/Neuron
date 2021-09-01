#include "pch.h"
#include "CNeuronBwImgView.h"

IMPLEMENT_DYNAMIC(CNeuronBwImgView, CDockablePane)

BEGIN_MESSAGE_MAP(CNeuronBwImgView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


CNeuronBwImgView::CNeuronBwImgView()
{
	
}

CNeuronBwImgView::~CNeuronBwImgView()
{

}


int CNeuronBwImgView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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


void CNeuronBwImgView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustLayout();
}

void CNeuronBwImgView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_ctrlStatic.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CNeuronBwImgView::SetImagePath(const CString strImagePath)
{
	CString strTemp = strImagePath;
	m_ctrlStatic.PrepareImage(strImagePath, 2);
}