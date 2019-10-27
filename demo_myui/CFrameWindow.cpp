
#include "CFrameWindow.h"
#include <atlbase.h>
#include <atlwin.h>
CVerticalLayoutUI * g_pLayout = NULL;
CScrollBarUI * g_pVerticalSrcollBar = NULL;

CActiveXUI * g_pActiveX = NULL;

CControlUI * CBuilderControl::CreateControl(const char* strClass)
{
	CControlUI * pControl = NULL;
	CBuilder * pBuilder = new CBuilder(NULL);
	TCHAR strSkin[MAX_PATH] = {0};

#if 0
	wsprintf(strSkin, _T("file='%s'"), _T("skin/"));
#else
	wsprintf(strSkin, _T("file='%s'"), _T("../Debug/skin/"));
#endif

	if(0 == stricmp(strClass, "Film"))
	{
		pControl = pBuilder->Create(strSkin, _T("Tile/Film.xml"));
	}

	delete pBuilder;
	return pControl;
}

CFrameWindow::CFrameWindow()
{
    m_pMenUI = NULL;
};

CFrameWindow::~CFrameWindow()
{
};

void CFrameWindow::OnNotify(TNOTIFYUI &notify)
{
	//想知道wParam, lParam对应了什么参数吗？说实话，由于项目还处于初期，
	//我也说不准wParam, lParam到底对应了什么参数，自己在断点中拦截到对应
	//的通知后，看[调用堆栈]追踪一下吧。
	CControlUI * pControl = (CControlUI*) notify.pSender;
	switch(notify.dwType)
	{
	case EnumNotifyMsg::ClickItem:
	{
        if (CButtonUI::g_strClassName == pControl->GetClassName())
	    {
		    //MessageBox(m_hWnd, pConntrol->GetText(), _T("你点击了："), NULL);
            //if (m_pMenUI && pControl->GetName() == _T("btnMenu"))
            //{
            //    m_pMenUI->Popup(NULL, CMuiRect(400, 400, 400, 400));
            //}
	    }
	}break;
	case EnumNotifyMsg::ActiveItem:
		{
        pControl = (CControlUI*)notify.lParam;
			//MessageBox(m_hWnd, pConntrol->GetClassName(), pConntrol->GetText(), NULL);
		}break;
	case EnumNotifyMsg::TimerCall:
		{
        pControl->KillTimer(10);
        MessageBox(m_hWnd, pControl->GetClassName(), pControl->GetName(), NULL);
		}break;
	case EnumNotifyMsg::CheckItem:
		{
			CControlUI * pShow = NULL;
			if(FALSE == notify.wParam)
			{
				//FALSE = 取消选中
				//TRUE = 选中
				break;
			}

            if (pControl->GetName() == _T("optLv"))
			{
				pShow = m_pViewInfo->pRootControl->FindControlByName(_T("ListView"));
			}
            else if (pControl->GetName() == _T("optAbs"))
			{
				pShow = m_pViewInfo->pRootControl->FindControlByName(_T("Absolute"));
			}
            else if (pControl->GetName() == _T("optFilm"))
			{
				pShow = m_pViewInfo->pRootControl->FindControlByName(_T("Film"));
			}
            else if (pControl->GetName() == _T("optWeb"))
			{
				pShow = m_pViewInfo->pRootControl->FindControlByName(_T("WebBrowser"));
			}

			if(pShow)
			{
				//::MessageBox(m_hWnd, pConntrol->GetName(), pConntrol->GetClassName(), NULL);
				CTabLayoutUI * pTab = static_cast<CTabLayoutUI *>(pShow->GetParent());
				pTab->SelectItem(pShow);
			}
		}break;
	}

	//return __super::OnNotify(notify);
};

LRESULT CFrameWindow::OnEvent(TEVENT &event)
{
	CDragItemUI * pDragItem = NULL;
	switch (event.dwType)
	{
	case EnumEventType::WindowInit:
		{
			OnCreate(event.wParam, event.lParam);
		}break;
	case EnumEventType::WindowReady:
		{
			OnReady(event.wParam, event.lParam);
		}break;
	case EnumEventType::DragOver:
		{
			pDragItem = (CDragItemUI *)event.wParam;
			pDragItem->Destroy();
			delete pDragItem;

		}break;
	case EnumEventType::OnTimer:
		{
			::KillTimer(m_hWnd, event.wParam);
			CBaseLayoutUI * pLayout = (CBaseLayoutUI*)m_pViewInfo->pRootControl;
			static CControlUI * pControl = NULL;
			switch(event.wParam)
			{
			case 1:
				{
					pControl = pLayout->Find((int) 1);
					pLayout->Remove(pControl);

					::SetTimer(m_hWnd, 2, 5000, NULL);
				}break;
			case 2:
				{
					pLayout->Add(pControl);
				}break;
			}
			
			pLayout->Update();
			
			//this->Close();
		}break;
	case EnumEventType::WindowDestroy:
		{
			SetSkin(NULL);
			PostQuitMessage(0);
		}break;
	default:
		break;
	}

	return 0;
	//return __super::OnEvent(event);
}

bool CFrameWindow::OnBefore(PVOID pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule)
{
	POINT point;
	RECT rcPos;
	SIZE szRound = {0};
	CControlUI * pText = reinterpret_cast<CControlUI *>(pControl);
	if(WM_LBUTTONDOWN == message && pText->GetName() == _T("htmltext"))
	{
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);

		if(pText->GetItemDisplayRect(rcPos))
		{
			CDragItemUI * pDragItem = new CDragItemUI();
			szRound = pText->GetBorderRound();
			pDragItem->Create(m_hInstance, this, rcPos, WM_LBUTTONUP, 255, &szRound);
			return true;
		}
	}
	return false;
}

bool CFrameWindow::OnAfter(PVOID pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule)
{
	return false;
}

LRESULT CALLBACK CFrameWindow::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	CControlUI * pControl = NULL;
	POINT point;
	CDragItemUI * pDragItem = NULL;
	SIZE szRound = {10,10};
	switch (message)
	{
	case WM_LBUTTONDOWN:
		{
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
		}break;
	default:
		break;
	}
	return __super::WndProc(message, wParam, lParam);
}

LRESULT CFrameWindow::OnTimer(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CFrameWindow::OnCreate2(WPARAM wParam, LPARAM lParam)
{
	TCHAR strSkin[MAX_PATH] = {0};
#if 0
	wsprintf(strSkin, _T("file='%s'"), _T("skin/"));
#else
	wsprintf(strSkin, _T("file='%s'"), _T("../Debug/skin/"));
#endif
	SetSkin(strSkin);
	//SetSkin(_T("skin\\"));

	CVerticalLayoutUI * pRootLayout = new CVerticalLayoutUI();
	pRootLayout->SetBkColor(ARGB(255,255,0,255));
	pRootLayout->SetBorderPen(2, ARGB(255,0,255,0));
	pRootLayout->SetInset(CMuiRect(100,100,100,100));

	CControlUI * pControl = new CControlUI();
	pControl->SetBkColor(ARGB(255,100,100,100));
	pRootLayout->Add(pControl);
	
	CRichEditUI * pRichEdit = new CRichEditUI();
	pRichEdit->SetBkColor(ARGB(255,255,255,255));
	pRootLayout->Add(pRichEdit);

	this->AttachFrameView(pRootLayout);
	
	return 0;
}

LRESULT CFrameWindow::OnCreate(WPARAM wParam, LPARAM lParam)
{
	TCHAR strSkin[MAX_PATH] = {0};
#if 0
	wsprintf(strSkin, _T("file='%s'"), _T("skin/"));
#else
	wsprintf(strSkin, _T("file='%s'"), _T("../Debug/skin/"));
#endif
	SetSkin(strSkin);
	//SetSkin(_T("skin\\"));

	CControlUI * pRootLayout = nullptr;
	
	CBuilderControl * pCallback = new CBuilderControl();
	CBuilder * pBuilder = new CBuilder(this, pCallback);
	pRootLayout = pBuilder->Create(strSkin, _T("frame.xml"));
	delete pBuilder;
	delete pCallback;
	this->AttachFrameView(pRootLayout);
	
    m_pMenUI = new CMenuUI();
    m_pMenUI->Create(m_hInstance, _T("MYMENU"), strSkin, _T("menu.xml"));

    CControlUI * pControl = pRootLayout->FindControlByName(_T("btnMenu"));

    if (pControl)
    {
        pControl->SetMenu(m_pMenUI);
    }

	return 0;
}

LRESULT CFrameWindow::OnReady(WPARAM wParam, LPARAM lParam)
{
    //return 0;
	CListViewUI * pListView  = (CListViewUI*)m_pViewInfo->pRootControl->FindControlByName(_T("ListView"));

	TCHAR strText[256];
	LVROWINFO rowInfo = {0};
	HDROWINFO hdInfo = {0};
	rowInfo.nHeight = 30;
	int nCol = pListView->GetColumnCount();
	hdInfo.strText = strText;
	for(int i =0; 600 > i; i++)
	{
		pListView->AddRow(rowInfo, i);

		for(int col = 0 ; nCol > col; col++)
		{
			hdInfo.nTextLenght = wsprintf(strText ,_T("%03d行, %02d列"), i + 1, col + 1);
			pListView->SetSubItem(hdInfo,col, i);
		}
	}

	CControlUI * pText = m_pViewInfo->pRootControl->FindControlByName(_T("htmltext"));
	if(pText)
	{
		//这个Hooker拦截WM_LBUTTONDOWN消息，对htmltext控件按住不放，可以产生一个DragItem控件
		//那个有QQ图片的控件就是htmltext
		pText->SetHooker(dynamic_cast<IControlHooker *>(this));
	}

	CEditUI * pEdit = static_cast<CEditUI *>(m_pViewInfo->pRootControl->FindControlByName(_T("edtMessage")));
	
	HDC hdc = GetDC(m_hWnd);
	SIZE szText;
	TEXTMETRIC tm = {0};
	GetTextMetrics(hdc, &tm);
	GetTextExtentPoint(hdc, _T("\t"),  1, &szText);
	
	ReleaseDC(m_hWnd, hdc);
	

	CTabLayoutUI * pTab = static_cast<CTabLayoutUI *>(m_pViewInfo->pRootControl->FindControlByName(_T("Table")));
	pTab->SelectItem(0);
	return 0;
}
