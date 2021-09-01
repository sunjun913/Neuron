#include "pch.h"
#include "CNeuronGrayImgView.h"

IMPLEMENT_DYNAMIC(CNeuronGrayImgView, CDockablePane)

BEGIN_MESSAGE_MAP(CNeuronGrayImgView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CNeuronGrayImgView::CNeuronGrayImgView()
{
}

CNeuronGrayImgView::~CNeuronGrayImgView()
{

}

int CNeuronGrayImgView::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void CNeuronGrayImgView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustLayout();
}

void CNeuronGrayImgView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_ctrlStatic.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CNeuronGrayImgView::SetImagePath(const CString strImagePath)
{
	CString strTemp = strImagePath;
	m_ctrlStatic.PrepareImage(strImagePath, 1);
}