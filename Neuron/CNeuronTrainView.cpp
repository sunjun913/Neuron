#include "pch.h"
#include "CNeuronTrainView.h"

IMPLEMENT_DYNAMIC(CNeuronTrainView, CDockablePane)

BEGIN_MESSAGE_MAP(CNeuronTrainView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_TRAIN_PREGRESS, &CNeuronTrainView::OnTrainProgress)
	ON_MESSAGE(WM_TRAIN_COMPLETE, &CNeuronTrainView::OnTrainComplete)
END_MESSAGE_MAP()

CNeuronTrainView::CNeuronTrainView()
{
	m_nTopology = 0;//fully connected
}

CNeuronTrainView::~CNeuronTrainView()
{

}

int CNeuronTrainView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE;
	const DWORD dwEditStyle = WS_VSCROLL | WS_HSCROLL| WS_CHILD | WS_VISIBLE | ES_LEFT |ES_AUTOVSCROLL | ES_AUTOHSCROLL| ES_MULTILINE| ES_READONLY;

	if (!m_ctrlEdit.Create(dwEditStyle, rectDummy, this, ID_NEURON_TRAIN_VIEW_EDIT))
	{
		return -1;
	}

	if (!m_ctrlProgress.Create(dwViewStyle, rectDummy, this, 0))
	{
		return -1;
	}

	AdjustLayout();

	return 0;

}


void CNeuronTrainView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	AdjustLayout();
}

void CNeuronTrainView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	
	int cyTlb = 26;

	m_ctrlEdit.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height()-cyTlb-1, SWP_NOACTIVATE | SWP_NOZORDER);
	m_ctrlProgress.SetWindowPos(NULL, rectClient.left, rectClient.top+rectClient.Height()-cyTlb-1, rectClient.Width(), cyTlb+1, SWP_NOACTIVATE | SWP_NOZORDER);
	
}


LRESULT CNeuronTrainView::OnTrainProgress(WPARAM wParam, LPARAM lParam)
{
	m_ctrlProgress.StepIt();
	return 0;
}

LRESULT CNeuronTrainView::OnTrainComplete(WPARAM wParam, LPARAM lParam)
{
	double** weights = (double**)wParam;
	int     num = (int)lParam;
	CString strTemp, strContent;

	if (m_nTopology == 0)
	{
		for (int x = 0; x < num; x++)
		{
			for (int y = 0; y < num; y++)
			{
				strTemp.Format(_T("%lf;\t"), weights[x * num + y]);

				strContent += strTemp;
			}
			strContent += _T("\r\n");
		}
	}
	else if (m_nTopology == 1)
	{
		for (int x = 0; x < num; x++)
		{
			for (int y = 0; y < m_nKneurons; y++)
			{
				strTemp.Format(_T("%lf;\t"), weights[x][y]);

				strContent += strTemp;
			}
			strContent += _T("\r\n");
		}
	}

	m_ctrlEdit.SetWindowText(strContent);

	return 0;
}