
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
		CComboBoxWnd(CControlUI * pListBox);
		~CComboBoxWnd();

		//bool SetVerticalScrollBarActive(bool bActive);//�Ƿ�һֱ��ʾ������
		//bool GetVerticalScrollBarActive();
		//void SetVerticalScrollBar(CScrollBarUI * pScrollBar);
		//CScrollBarUI * GetVerticalScrollBar() const;

		//bool AddItem(CControlUI * pControl);
		//void RemoveItem(int nIndex);
		//void RemoveItem(CControlUI *pControl);
		//bool SetSelectItem(int nIndex);
		int GetSelect() const;
		void SetParent(CControlUI * pParent);
	protected:
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
        virtual void OnNotify(TNOTIFYUI &notify);
		virtual LRESULT OnEvent(TEVENT &event);

	private:
		CListBoxUI * m_pListBox;
		CControlUI * m_pParent;
	};
	
	CComboBoxWnd::CComboBoxWnd(CControlUI * pListBox)
		: m_pParent(NULL)
	{
        m_pListBox = static_cast<CListBoxUI*>(pListBox);
	}

	CComboBoxWnd::~CComboBoxWnd()
	{

	}

	void CComboBoxWnd::SetParent(CControlUI * pParent)
	{
		m_pParent = pParent;
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
		case WM_CHECKHIDE:
			{
				ASSERT(m_pParent && "CComboBoxWnd::WndProc m_pParent ����Ϊ��");
				CControlUI * pControl = NULL;
				pControl = (CControlUI*)::SendMessage(::GetParent(m_hWnd), WM_REQUESTINFO, MRQF_GETFOCUS, NULL);

				TRACE(_T("pControl = %s , FOCUSE=%d"),(LPCTSTR)(pControl ? pControl->GetClassName() : _T("NULL")), 
					(int)(::GetFocus() == m_hWnd ? 1 : 0));

				//����һ��ΪʲôWM_CHECKHIDEҪʹ���첽���忴������ж���䣺
				//������㲻��ComboBox�ͱ������У���رձ����ڡ�
				//��һ������ת������һ������Ĵ�������У���Ϣ���չ��������ǣ�
				//WM_KILLFOCUS -> WM_SETFOCUSE -> WM_LBUTTONDOWN��������Ҫ��WM_LBUTTONDOWN֮��
				//����֪��pFocusedControl�Ƿ����m_pParent ���� ::GetFocus() �Ƿ���� m_hWnd
				//Ҳ����˵������WM_KILLFOCUS��ʱ��WM_CHECKHIDE ������WM_KILLFOCUS�����ģ���
				//�������жϳ������������Ƿ���ϣ���Ϊ���ʱ�򣬴��������ڽ���ת���Ŀյ��ڣ�
				//����Ҫʹ��PostMessage������Ϣ�ŵ�����������ý���ת����ɣ����ж��Ƿ���Ҫ����
				if(NULL == m_pParent || (pControl != m_pParent && ::GetFocus() != m_hWnd))
				{
					::ShowWindow(m_hWnd, FALSE);
				}
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
					this->ShowWindow(false);
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
				TRACE(_T("CComboBoxWnd::OnEvent WM_KILLFOCUS"));
				PostMessage(WM_CHECKHIDE, NULL, NULL);
			}break;
		case EnumEventType::WindowDestroy:
			{

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
		m_pDialog = new CComboBoxWnd(m_pListBox);
	}

	CComboBoxUI::~CComboBoxUI()
	{
		//delete m_pDialog;
		//delete m_pListBox;
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

	void CComboBoxUI::OnAttach(HWND hNewWnd)
	{
        m_pDialog->SetSyncResource(dynamic_cast<CWindowUI*>(m_pShareInfo->pNotify));//ʹ��ͬ����Դ��ͬ����������Դ
		m_pDialog->Create((HINSTANCE) GetWindowLong(hNewWnd,GWL_HINSTANCE),hNewWnd,
			WS_BORDER | WS_POPUP | WS_CLIPSIBLINGS, 
			_T("{2F581E91-B25B-4A39-AFE8-96087151DFA7}"), _T("ComboBoxWnd"));
        
		m_pDialog->SetParent(this);
	}

	void CComboBoxUI::OnDetach(HWND hOldWnd)
	{
		if(::IsWindow(*m_pDialog))
		{
			m_pDialog->Destroy();
		}
	}

	void CComboBoxUI::SetItemBoxHeight(int nHeight)
	{
		m_nItemBoxHeight = nHeight;
	}

	int CComboBoxUI::GetItemBoxHeight()
	{
		return m_nItemBoxHeight;
	}

	bool CComboBoxUI::ShowDialog(bool bShow /*= true*/)
	{
		CControlUI * pControl = NULL;
		RECT rcItem;
		POINT pt;
		int nItemBoxHeight = 0;
		if(NULL == m_pDialog) return false;

		ASSERT(::IsWindow(*m_pDialog));
		if(!!::IsWindowVisible(*m_pDialog) == bShow)
		{
			return true;
		}

		if(false == bShow)
		{
			m_pDialog->ShowWindow(false);
		}
		else
		{
			if(TRUE == GetItemFixedRect(rcItem))
			{
				pt.x = rcItem.left;
				pt.y = rcItem.bottom;

				nItemBoxHeight = MAX(m_pListBox->GetContentSize().cy + 2,//��2�Ǳ߿�ĸ߶�
					rcItem.bottom - rcItem.top);

				if(m_nItemBoxHeight > 0)
				{
					nItemBoxHeight = MIN(m_nItemBoxHeight,nItemBoxHeight);
				}

				::ClientToScreen(m_pShareInfo->hWnd, &pt);
				::SetWindowPos(*m_pDialog, HWND_TOPMOST, pt.x, pt.y, 
					rcItem.right - rcItem.left, nItemBoxHeight ,
					SWP_SHOWWINDOW);
			}
		}

		//::InvalidateRect(m_pShareInfo->hWnd, NULL, FALSE);
		return true;
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

				if(FALSE == PointInRect(pt, rcButton))
				{
					//����Ϣ����__super��
					ShowDialog(false);
					TRACE(_T("CComboBoxUI WM_LBUTTONDOWN From Edit"));
					break;
				}
				else
				{
					if(true == IsEnabled())
					{
						ShowDialog(!::IsWindowVisible(* m_pDialog));
						TRACE(_T("CComboBoxUI WM_LBUTTONDOWN From Button"));
					}
				}
			}break;
		case WM_KEYDOWN:
			{
				if(true == IsEnabled() && VK_RETURN == wParam)
				{
					ShowDialog(!::IsWindowVisible(* m_pDialog));
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
				//m_pDialog->SendMessage(WM_PARENTNOTIFY, TRUE, NULL);
			}break;
		case WM_KILLFOCUS:
			{
				TRACE(_T("CComboBoxUI WM_KILLFOCUS"));
				m_pDialog->PostMessage(WM_CHECKHIDE);
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