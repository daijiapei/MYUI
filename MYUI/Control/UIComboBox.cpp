
#include "UIComboBox.h"

#include "..\\Core\\UIWindow.h"

namespace MYUI
{
/**********************************************************************************
 * 点击ComboBox会弹出一个下拉列表，首先我们将下拉列表封装一下
***********************************************************************************/

	class CComboBoxWnd : public CWindowUI
	{
	public:
        CComboBoxWnd(CControlUI * pParent, CListBoxUI * pListBox);
		~CComboBoxWnd();

		//bool SetVerticalScrollBarActive(bool bActive);//是否一直显示滚动条
		//bool GetVerticalScrollBarActive();
		//void SetVerticalScrollBar(CScrollBarUI * pScrollBar);
		//CScrollBarUI * GetVerticalScrollBar() const;

		//bool AddItem(CControlUI * pControl);
		//void RemoveItem(int nIndex);
		//void RemoveItem(CControlUI *pControl);
		//bool SetSelectItem(int nIndex);

	protected:
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
        virtual void OnNotify(MUINOTIFY& notify);
		virtual LRESULT OnEvent(MUIEVENT &event);

	private:
		CListBoxUI * m_pListBox;
		CControlUI * m_pParent;
	};
	
    CComboBoxWnd::CComboBoxWnd(CControlUI * pParent, CListBoxUI * pListBox)
	{
        m_pParent = static_cast<CControlUI*>(pParent);
        m_pListBox = static_cast<CListBoxUI*>(pListBox);
	}

	CComboBoxWnd::~CComboBoxWnd()
	{

	}


	LRESULT CALLBACK CComboBoxWnd::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				//没有可以选择的项
				if(m_pListBox->GetCount() == 0)
				{
					::ShowWindow(m_hWnd, FALSE);
					return 0;
				};
			}break;
        case WM_KILLFOCUS:
        {
			/*::PostMessage(GetWindowOwner(m_hWnd), WM_BREAKLOOP, TRUE,
				(LPARAM)static_cast<CControlUI*>(m_pParent));*/
        }break;
		}

		return __super::WndProc(message, wParam, lParam);
	}

    void CComboBoxWnd::OnNotify(MUINOTIFY& Notify)
	{
		switch(Notify.dwType)
		{
		case EnumNotify::SelectItem:
			{
				CControlUI * pConntrol = (CControlUI*) Notify.lParam;
				
				if(pConntrol && m_pParent)
				{
                    ::ShowWindow(m_hWnd, FALSE);
                    ::PostMessage(m_hWnd, WMU_BREAKLOOP, TRUE, NULL);
					m_pParent->SetText(pConntrol->GetText());
					m_pParent->SendNotify(Notify.dwType, Notify.wParam, Notify.lParam);
				}
			}break;
		}

		//return __super::OnNotify(notify);
	};

	LRESULT CComboBoxWnd::OnEvent(MUIEVENT &event)
	{
		switch (event.dwType)
		{
		case EnumEvent::WindowInit:
			{
				CVerticalLayoutUI * pRootLayout = new CVerticalLayoutUI();

				pRootLayout->SetBorderPen(1, ARGB(255,100,100,100));
				pRootLayout->SetBkColor(ARGB(255,255,255,255));
				pRootLayout->SetInset(CMuiRect(1,1,1,1));

				pRootLayout->Add(m_pListBox);
				this->AttachFrameView(pRootLayout);
			}break;
		case EnumEvent::OnTimer:
			{

			}break;
		case EnumEvent::WindowShow:
			{
			
			}break;
		case EnumEvent::KillFocued:
			{
				::PostMessage(GetWindowOwner(m_hWnd), WMU_BREAKLOOP, TRUE,
				(LPARAM)static_cast<CControlUI*>(m_pParent));
			}break;
		case EnumEvent::WindowDestroy:
		{
            CVerticalLayoutUI * pRootLayout = static_cast<CVerticalLayoutUI*>(GetRootControl());
            pRootLayout->Remove(m_pListBox);
		}break;
		default:
			break;
		}

		return 0;
	}

/**********************************************************************************
 * 下面开始处理ComboBox
***********************************************************************************/

	CComboBoxUI::CComboBoxUI()
		: m_pDialog(NULL)
		, m_nItemBoxHeight(0)
		, m_bMouseInButton(false)
	{
		m_pListBox = new CListBoxUI();
		m_pListBox->SetBkColor(ARGB(255,255,255,255));
	}

	CComboBoxUI::~CComboBoxUI()
	{
		delete m_pListBox;
	}

	const CMuiString CComboBoxUI::g_strClassName(_T("ComboBoxUI"));

	CMuiString CComboBoxUI::GetClassName() const
	{
		return CComboBoxUI::g_strClassName;
	}

	bool CComboBoxUI::Add(CControlUI* pControl)
	{
		return m_pListBox->Add(pControl);
	}

	bool CComboBoxUI::AddAt(CControlUI* pControl, int nIndex)
	{
		return m_pListBox->AddAt(pControl, nIndex);
	}

	bool CComboBoxUI::Remove(CControlUI* pControl)
	{
		return m_pListBox->Remove(pControl);
	}

	bool CComboBoxUI::RemoveAt(int nIndex)
	{
		return m_pListBox->RemoveAt(nIndex);
	}

	void CComboBoxUI::RemoveAll()
	{
		return m_pListBox->RemoveAll();
	}

	CControlUI * CComboBoxUI::Find(int nIndex)
	{
		return m_pListBox->Find(nIndex);
	}

	int CComboBoxUI::Find(CControlUI * pControl)
	{
		return m_pListBox->Find(pControl);
	}

	int CComboBoxUI::GetCount()
	{
		return m_pListBox->GetCount();
	}

	bool CComboBoxUI::SetSelectItem(int nIndex)
	{
		LPCTSTR strText = NULL;
		if(true == m_pListBox->SetSingleSelect(nIndex))
		{
			strText = m_pListBox->Find(nIndex)->GetText();
			this->SetText(strText);
		}
		return false;
	}

	int CComboBoxUI::GetSelectIndex()
	{
		CControlUI * pControl = m_pListBox->GetSelect();
		if(pControl)
		{
			return m_pListBox->Find(pControl);
		}
		return -1;
	}

	CControlUI * CComboBoxUI::GetSelectItem()
	{
		return m_pListBox->GetSelect();
	}

	void CComboBoxUI::SetItemBoxHeight(int nHeight)
	{
		m_nItemBoxHeight = nHeight;
	}

	int CComboBoxUI::GetItemBoxHeight()
	{
		return m_nItemBoxHeight;
	}

	RECT CComboBoxUI::CalcButtonPostion()
	{
		RECT rcButton = m_rcRawItem;

		if (1)
		{
			rcButton.left = rcButton.right - m_rcTextPadding.right;
		}
		else
		{
			rcButton.right = m_rcTextPadding.left;
		}
		
		return rcButton;
	}

    LRESULT CComboBoxUI::Popup(LPARAM lParam)
    {
        LRESULT lResult = FALSE;
        CComboBoxWnd * pDialog = NULL;

        RECT rcPos = { 0 };
        int nItemBoxHeight = 0;
        
        if (m_pDialog)
        {
            MUIASSERT(0 && "CComboBoxUI::Popup 不合适的调用时机, m_pDialog应该为空");
            return FALSE;
        }
        
        //计算位置
        if (FALSE == GetItemFixedRect(rcPos)) return NULL;

        rcPos.top = rcPos.bottom;

        nItemBoxHeight = MAX(m_pListBox->GetContentSize().cy + 2,//加2是边框的高度
            rcPos.bottom - rcPos.top);

        if (m_nItemBoxHeight > 0)
        {
            nItemBoxHeight = MIN(m_nItemBoxHeight, nItemBoxHeight);
        }

        rcPos.bottom = rcPos.top + nItemBoxHeight;

        ::MapWindowPoints(GETHWND(this), NULL, (LPPOINT)&rcPos, 2);

        //显示dialog
        pDialog = new CComboBoxWnd(static_cast<CControlUI*>(this), m_pListBox);
        //pDialog->CloneResource(m_pShareInfo);
        pDialog->Create((HINSTANCE)GetWindowLong(GETHWND(this), GWL_HINSTANCE), GETHWND(this),
            WS_BORDER | WS_POPUP | WS_CLIPSIBLINGS,
            _T("{2F581E91-B25B-4A39-AFE8-96087151DFA7}"), _T("ComboBoxWnd"));

        m_pDialog = pDialog;
        lResult = pDialog->Popup(rcPos);
        m_pDialog = NULL;

        pDialog->Destroy();
        delete pDialog;
        return lResult;
    }

	LRESULT CComboBoxUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		RECT rcButton;
		RECT rcDisplay = {0,0,0,0};
		bool bRet = false;
		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				pt.x = (short)GET_X_LPARAM(lParam);
				pt.y = (short)GET_Y_LPARAM(lParam);

				rcButton = CalcButtonPostion();

                if (TRUE == PointInRect(pt, rcButton))
                {
                    if (m_pDialog)
                    {
                        m_pDialog->PostMessage(WMU_BREAKLOOP, TRUE, NULL);
                    }
                    else
                    {
                        //::PostMessage(GETHWND(this), WM_POPUPDIALOG, (WPARAM)static_cast<IDialogPopup*>(this), NULL);
						this->Popup(lParam);
                    }
					return 0;
                }
			}break;
		case WM_KEYDOWN:
			{
				if(VK_RETURN == wParam)
				{
					//ShowDialog(!::IsWindowVisible(* m_pDialog));
                    if (m_pDialog)
                    {
                        m_pDialog->PostMessage(WMU_BREAKLOOP, TRUE, NULL);
                    }
                    else
                    {
                        ::PostMessage(GETHWND(this), WMU_POPUPDIALOG, (WPARAM)static_cast<IDialogPopup*>(this), NULL);
                    }
					MUITRACE(_T("CComboBoxUI WM_KEYDOWN From Edit"));
					return false;
				}
			}break;
		case WM_MOUSEMOVE:
			{
				pt.x = (short)GET_X_LPARAM(lParam);
				pt.y = (short)GET_Y_LPARAM(lParam);
				rcButton = CalcButtonPostion();

				if(TRUE == PointInRect(pt, rcButton))
				{
					m_bMouseInButton = true;
				}
				else
				{
					m_bMouseInButton = false;
				}
			}break;
		case WM_MOUSELEAVE:
			{
				m_bMouseInButton = false;
			}break;
		case WM_SETCURSOR:
			{
				if(true == m_bMouseInButton)
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
					return false;
				}
				else
				{
					//让消息流入__super中
					break;
				}
			}break;
		case WM_SETFOCUS:
			{
                MUITRACE(_T("CComboBoxUI WM_SETFOCUS"));
			}break;
		case WM_KILLFOCUS:
			{
				MUITRACE(_T("CComboBoxUI WM_KILLFOCUS"));
                if (m_pDialog)
                {
                    ::PostMessage(::GetWindowOwner(*m_pDialog), WMU_BREAKLOOP,
                        TRUE, (LPARAM)static_cast<CControlUI*>(this));
                }
			}break;
		default:
			break;
		}

		return __super::WndProc(message, wParam, lParam);
	}

	bool CComboBoxUI::SetHorizontalScrollBarActive(bool bActive)
	{
		return m_pListBox->SetHorizontalScrollBarActive(bActive);
	}

	bool CComboBoxUI::GetHorizontalScrollBarActive()
	{
		return m_pListBox->GetHorizontalScrollBarActive();
	}

	void CComboBoxUI::SetHorizontalScrollBar(CScrollBarUI * pScrollBar)
	{
		return m_pListBox->SetHorizontalScrollBar(pScrollBar);
	}

	CScrollBarUI * CComboBoxUI::GetHorizontalScrollBar() const
	{
		return m_pListBox->GetHorizontalScrollBar();
	}

	bool CComboBoxUI::SetVerticalScrollBarActive(bool bActive)
	{
		return m_pListBox->SetVerticalScrollBarActive(bActive);
	}

	bool CComboBoxUI::GetVerticalScrollBarActive()
	{
		return m_pListBox->GetVerticalScrollBarActive();
	}

	void CComboBoxUI::SetVerticalScrollBar(CScrollBarUI * pScrollBar)
	{
		return m_pListBox->SetVerticalScrollBar(pScrollBar);
	}

	CScrollBarUI * CComboBoxUI::GetVerticalScrollBar() const
	{
		return m_pListBox->GetVerticalScrollBar();
	}
}