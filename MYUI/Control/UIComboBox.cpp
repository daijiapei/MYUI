
#include "UIComboBox.h"

#include "..\\Core\\UIWindow.h"

namespace MYUI
{
/**********************************************************************************
 * ���ComboBox�ᵯ��һ�������б��������ǽ������б��װһ��
***********************************************************************************/

	class CComboBoxWnd : public CWindowUI
	{
	public:
        CComboBoxWnd(CControlUI * pParent, CListBoxUI * pListBox);
		~CComboBoxWnd();

		//bool SetVerticalScrollBarActive(bool bActive);//�Ƿ�һֱ��ʾ������
		//bool GetVerticalScrollBarActive();
		//void SetVerticalScrollBar(CScrollBarUI * pScrollBar);
		//CScrollBarUI * GetVerticalScrollBar() const;

		//bool AddItem(CControlUI * pControl);
		//void RemoveItem(int nIndex);
		//void RemoveItem(CControlUI *pControl);
		//bool SetSelectItem(int nIndex);

	protected:
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
        virtual void OnNotify(TNOTIFYUI &notify);
		virtual LRESULT OnEvent(TEVENT &event);

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
				//û�п���ѡ�����
				if(m_pListBox->GetCount() == 0)
				{
					::ShowWindow(m_hWnd, FALSE);
					return 0;
				};
			}break;
        case WM_KILLFOCUS:
        {
            ::PostMessage(GetWindowOwner(m_hWnd), WM_BREAKLOOP, TRUE,
                (LPARAM)static_cast<CControlUI*>(m_pParent));
        }break;
		}

		return __super::WndProc(message, wParam, lParam);
	}

    void CComboBoxWnd::OnNotify(TNOTIFYUI &notify)
	{
		switch(notify.dwType)
		{
		case EnumNotifyMsg::SelectItem:
			{
				CControlUI * pConntrol = (CControlUI*) notify.lParam;
				
				if(pConntrol && m_pParent)
				{
                    ::ShowWindow(m_hWnd, FALSE);
                    ::PostMessage(m_hWnd, WM_BREAKLOOP, TRUE, NULL);
					m_pParent->SetText(pConntrol->GetText());
					m_pParent->SendNotify(FALSE, notify.dwType, notify.wParam, notify.lParam);
				}
			}break;
		}

		//return __super::OnNotify(notify);
	};

	LRESULT CComboBoxWnd::OnEvent(TEVENT &event)
	{
		switch (event.dwType)
		{
		case EnumEventType::WindowInit:
			{
				CVerticalLayoutUI * pRootLayout = new CVerticalLayoutUI();

				pRootLayout->SetBorderPen(1, ARGB(255,100,100,100));
				pRootLayout->SetBkColor(ARGB(255,255,255,255));
				pRootLayout->SetInset(CMuiRect(1,1,1,1));

				pRootLayout->Add(m_pListBox);
				this->AttachFrameView(pRootLayout);
			}break;
		case EnumEventType::OnTimer:
			{

			}break;
		case EnumEventType::WindowShow:
			{
			
			}break;
		case EnumEventType::KillFocued:
			{
			}break;
		case EnumEventType::WindowDestroy:
		{
            CVerticalLayoutUI * pRootLayout = static_cast<CVerticalLayoutUI*>(m_pViewInfo->pRootControl);
            pRootLayout->Remove(m_pListBox);
		}break;
		default:
			break;
		}

		return 0;
		//return __super::OnEvent(event);
	}

/**********************************************************************************
 * ���濪ʼ����ComboBox
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

	CMuiString CComboBoxUI::g_strClassName(_T("ComboBoxUI"));

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

	int CComboBoxUI::GetCount() const
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

    LRESULT CComboBoxUI::Popup(LPARAM lParam)
    {
        LRESULT lResult = FALSE;
        CComboBoxWnd * pDialog = NULL;

        RECT rcPos = { 0 };
        int nItemBoxHeight = 0;
        
        if (m_pDialog)
        {
            ASSERT(0 && "CComboBoxUI::Popup �����ʵĵ���ʱ��, m_pDialogӦ��Ϊ��");
            return FALSE;
        }
        
        //����λ��
        if (FALSE == GetItemFixedRect(rcPos)) return NULL;

        rcPos.top = rcPos.bottom;

        nItemBoxHeight = MAX(m_pListBox->GetContentSize().cy + 2,//��2�Ǳ߿�ĸ߶�
            rcPos.bottom - rcPos.top);

        if (m_nItemBoxHeight > 0)
        {
            nItemBoxHeight = MIN(m_nItemBoxHeight, nItemBoxHeight);
        }

        rcPos.bottom = rcPos.top + nItemBoxHeight;

        ::MapWindowPoints(GETHWND(this), NULL, (LPPOINT)&rcPos, 2);

        //��ʾdialog
        pDialog = new CComboBoxWnd(static_cast<CControlUI*>(this), m_pListBox);
        pDialog->CloneResource(m_pShareInfo);
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

	LRESULT CComboBoxUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

				rcButton = m_rcRawItem;
				rcButton.left = rcButton.right - m_rcTextPadding.right;

                if (TRUE == PointInRect(pt, rcButton))
                {
                    if (m_pDialog)
                    {
                        m_pDialog->PostMessage(WM_BREAKLOOP, TRUE, NULL);
                    }
                    else
                    {
                        ::PostMessage(GETHWND(this), WM_POPUPDIALOG, (WPARAM)static_cast<IDialogPopup*>(this), NULL);
                    }
                }
			}break;
		case WM_KEYDOWN:
			{
				if(VK_RETURN == wParam)
				{
					//ShowDialog(!::IsWindowVisible(* m_pDialog));
                    if (m_pDialog)
                    {
                        m_pDialog->PostMessage(WM_BREAKLOOP, TRUE, NULL);
                    }
                    else
                    {
                        ::PostMessage(GETHWND(this), WM_POPUPDIALOG, (WPARAM)static_cast<IDialogPopup*>(this), NULL);
                    }
					TRACE(_T("CComboBoxUI WM_KEYDOWN From Edit"));
					return false;
				}
			}break;
		case WM_MOUSEMOVE:
			{
				pt.x = (short)GET_X_LPARAM(lParam);
				pt.y = (short)GET_Y_LPARAM(lParam);
				rcButton = m_rcRawItem;
				rcButton.left = rcButton.right - m_rcTextPadding.right;

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
					//����Ϣ����__super��
					break;
				}
			}break;
		case WM_SETFOCUS:
			{
                TRACE(_T("CComboBoxUI WM_SETFOCUS"));
			}break;
		case WM_KILLFOCUS:
			{
				TRACE(_T("CComboBoxUI WM_KILLFOCUS"));
                if (m_pDialog)
                {
                    ::PostMessage(::GetWindowOwner(*m_pDialog), WM_BREAKLOOP,
                        TRUE, (LPARAM)static_cast<CControlUI*>(this));
                }
			}break;
		default:
			break;
		}

		return __super::WndProc(hWnd, message, wParam, lParam);
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