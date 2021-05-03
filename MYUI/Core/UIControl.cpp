
#include "UIControl.h"
#include "UIThread.h"

namespace MYUI
{
	CControlUI::CControlUI()
		: m_pTag(NULL)
		, m_pShareInfo(NULL)
		, m_pParentContrl(NULL)
        , m_pMenu(NULL)
		, m_bEnabled(true)
		, m_bPenetrated(false)
		, m_dwState(STATE_DEFAULT)
		, m_bControl(true)
	{
	}

	CControlUI::~CControlUI()
	{
		MUITRACE(_T("CControlUI::~CControlUI"));
	}

	bool CControlUI::IsControl()
	{
		return m_bControl;
	}

	bool CControlUI::GetItemDisplayRect(RECT &rcDisplay)
	{
		RECT rcMatch = m_rcRawItem;
		OffsetRect(&rcMatch, m_rcItem.left, m_rcItem.top);
		return GetDisplayRect(rcDisplay, rcMatch);
	}

	bool CControlUI::GetClientDisplayRect(RECT &rcDisplay)
	{
		RECT rcMatch = m_rcClient;
		OffsetRect(&rcMatch, m_rcItem.left, m_rcItem.top);
		return GetDisplayRect(rcDisplay, rcMatch);
	}

	bool CControlUI::GetDisplayRect(RECT &rcDisplay,const RECT &rcMatch)
	{
		//控件在窗口中显示的位置，如果在窗口中有显示，则返回true
		//没有显示则返回false
		//注意，如果在绝对布局中，发生重叠的控件，
		//这个函数不会考虑重叠问题，返回的区域可能大于显示区域
		RECT rcParentClient;
		RECT rcParentItem;
		RECT rcDisplayTmp = rcMatch;
		
		CControlUI * pControl = this;

		if (false == pControl->IsVisible())
		{
			return false;
		}

		while (pControl = pControl->GetParent())
		{
			if(false == pControl->IsVisible())
			{
				return false;
			}

			rcParentItem = pControl->GetItem();
			rcParentClient = pControl->GetItemClient();
			OffsetRect(&rcDisplayTmp, rcParentItem.left, rcParentItem.top);
			if(false == IntersectRect(&rcDisplayTmp, &rcDisplayTmp, &rcParentClient))
			{
				return false;
			}
		}

		rcDisplay = rcDisplayTmp;
		return true;
	}

	bool CControlUI::GetItemFixedRect(RECT &rcFixed)
	{
		return GetFixedRect(rcFixed, m_rcRawItem);
	}

	bool CControlUI::GetClientFixedRect(RECT& rcFixed)
	{
		return GetFixedRect(rcFixed, m_rcClient);
	}

	bool CControlUI::GetFixedRect(RECT &rcFixed, const RECT & rcMatch)
	{
		RECT rcParentItem;
		RECT rcFixedTmp = rcMatch;

		CControlUI * pControl = this;

		do
		{
			if(false == pControl->IsVisible())
			{
				return false;
			}

			rcParentItem = pControl->GetItem();
			OffsetRect(&rcFixedTmp, rcParentItem.left, rcParentItem.top);
		}while (pControl = pControl->GetParent());

		rcFixed = rcFixedTmp;
		return true;
	}

	void CControlUI::SetShareInfo(MUISHAREINFO * pShareInfo)
	{
		HWND hOldWnd = m_pShareInfo ? m_pShareInfo->hWnd : NULL ;
		HWND hNewWnd = pShareInfo ? pShareInfo->hWnd : NULL ;

		if(hOldWnd != hNewWnd)
		{
			if(NULL != hOldWnd)
			{
				this->OnDetach(hOldWnd);//先脱离一个窗口
			}

			m_pShareInfo = pShareInfo;

			if(NULL != hNewWnd)
			{
				this->OnAttach(hNewWnd);//再附加到一个窗口
			}
		}
		else
		{
			m_pShareInfo = pShareInfo;
		}
	}

	bool CControlUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		if(false == __super::SetItem(rcItem, bMustUpdate) && m_bControl) return false;

		return true;
	}

	void CControlUI::SetTextFont(int FontId)
	{
		__super::SetTextFont(FontId);
		if (0 > m_nFontId)
		{
			m_hFont = NULL;
		}
		else
		{
			if (m_pShareInfo)
			{
				m_hFont = (HFONT)m_pShareInfo->FontArray.Select(m_nFontId);
			}
		}
	}

	void CControlUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("bkcolor"))) 
		{
			SetBkColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("font"))) 
		{
			SetTextFont(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("bordersize"))) 
		{
			SetBorderSize(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("bordercolor"))) 
		{
			SetBorderColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("round"))) 
		{
			SetBorderRound(CMuiSize(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("bkImage"))) 
		{
			SetBkImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("disableImage"))) 
		{
			SetDisabledImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("pos"))) 
		{
			SetPos(CMuiRect(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("size")))
		{
			SetSize(CMuiSize(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("width"))) 
		{
			SetWidth(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("height"))) 
		{
			SetHeight(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("point"))) 
		{
			SetPoint(CMuiPoint(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("pointX"))) 
		{
			SetPointX(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("pointY"))) 
		{
			SetPointY(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("min"))) 
		{
			SetMinSize(CMuiSize(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("minCx"))) 
		{
			SetMinCx(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("minCy"))) 
		{
			SetMinCy(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("max"))) 
		{
			SetMaxSize(CMuiSize(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("maxCx"))) 
		{
			SetMaxCx(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("maxCy"))) 
		{
			SetMaxCy(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("margin"))) 
		{
			SetMargin(CMuiRect(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("visible"))) 
		{
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			SetVisible(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("enabled"))) 
		{
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			SetEnabled(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("Penetrated"))) 
		{
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			SetPenetrated(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("name"))) 
		{
			SetName(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("text"))) 
		{
			SetText(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("textcolor")))
		{
			SetTextColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("tiptext")))
		{
			SetTipText(strValue);
		}
		else
		{
			MUITRACE(_T("SetAttribute: Item=%s, Value=%s"), strItem, strValue);
			MUIASSERT(0 && "CControlUI::SetAttribute：no define strItem");
		}
	}

	CControlUI * CControlUI::FindControlByPoint(POINT &Point)
	{
		HRGN hrgn = NULL;
		CControlUI * pControl = NULL;

		if (true == m_bPenetrated)
		{
			return NULL;
		}

		if(m_szBorderRound.cx || m_szBorderRound.cy)
		{
			hrgn = ::CreateRoundRectRgn(m_rcRawItem.left, m_rcRawItem.top, m_rcRawItem.right + 1,
				m_rcRawItem.bottom + 1, m_szBorderRound.cx, m_szBorderRound.cy);
			if(TRUE == ::PtInRegion(hrgn, Point.x, Point.y))
			{
				pControl = this;
			}
			::DeleteObject(hrgn);
		}
		else if(PointInRect(Point, m_rcRawItem))
		{
			pControl = this;
		}
		return pControl;
	}

	CControlUI * CControlUI::FindControlByName(LPCTSTR strName)
	{
		if(strName && m_strName == strName) return this;
		return NULL;
	}

	CControlUI * CControlUI::FindSubControlByPoint(POINT &pt)
	{
		return NULL;
	}

	CControlUI * CControlUI::FindSubControlByName(LPCTSTR strName)
	{
		return NULL;
	}

	bool CControlUI::SetFocus()
	{
		MUIASSERT(m_pShareInfo && m_pShareInfo->hWnd);
		if(!m_pShareInfo || !m_pShareInfo->hWnd) return false;
		::SendMessage(GETHWND(this), WM_SETFOCUS, NULL, (LPARAM)this);
		return true;
	}

	bool CControlUI::IsFocued()
	{
		return STATE_FOCUS & m_dwState;
	}

	bool CControlUI::IsEnabled()
	{
		return m_bEnabled;
	}

	void CControlUI::SetEnabled(bool bVisible)
	{
		if(bVisible == m_bEnabled) return;

		m_bEnabled = bVisible;
		Invalidate();
	}

	bool CControlUI::IsVisible()
	{
		//控件显示了，不代表一定能看见，不一定在显示区域
		//你也可以调用GetDisplayRect 获取更详细的显示信息
		if(false == m_bVisible)
		{
			return false;
		}
		else
		{
			if(m_pParentContrl)
			{
				return m_pParentContrl->IsVisible();
			}
		}
		return m_bVisible;
	}

	void CControlUI::SetVisible(bool bVisible)
	{
		if(m_bVisible == bVisible) return;
		m_bVisible = bVisible;
		
		//因为布局总是相对的，一个控件的显示将会影响到兄弟控件
		//所以这里要从父控件重置布局，这样其他兄弟控件的位置也会得到调整
		this->Update();
	}

	bool CControlUI::IsPenetrated()
	{
		return m_bPenetrated;
	}

	void CControlUI::SetPenetrated(bool bPenetrated)
	{
		m_bPenetrated = bPenetrated;
	}

	void CControlUI::OnPosChange(LPVOID pElement)
	{
		this->Update();
	}

	void CControlUI::OnViewChange(LPVOID pElement)
	{
		this->Invalidate();
	}

	void CControlUI::OnAttach(HWND hNewWnd)
	{
		SetTextFont(m_nFontId);
		return __super::OnAttach(hNewWnd);
	}

	void CControlUI::OnDetach(HWND hOldWnd)
	{
		//bug
		//在离开窗口时，应该判断一下窗口的热点等，是不是当前控件
		//如果是，要把这些数据置空
		SetState(0);
		m_hFont = NULL;
		return __super::OnDetach(hOldWnd);
	}

	void CControlUI::PaintBkColor(const RECT& rcUpdate)
	{
		//RECT rcClient = m_rcRawItem;
		//OffsetRect(&rcClient, rcItem.left, rcItem.top);
		m_pShareInfo->pRenderEngine->OnDrawColor(m_rcClient, m_refBkColor);
	}

	void CControlUI::PaintStatusImage(const RECT& rcUpdate)
	{
		return ;
	}

	void CControlUI::PaintText(const RECT& rcUpdate)
	{
		return;
	}

	void CControlUI::PaintBorder(const RECT& rcUpdate)
	{
		m_pShareInfo->pRenderEngine->OnDrawBroder(m_rcRawItem, m_refBorderColor, m_nBorderSize, m_szBorderRound);
	}

	void CControlUI::PaintBkImage(const RECT& rcUpdate)
	{
		LPCTSTR strImage = m_strBkImage;
		if(!m_strDisabledImage.IsEmpty() && false == IsEnabled())
		{
			strImage = m_strDisabledImage;
		}

		if(_T('\0') == strImage[0]) return;
		m_pShareInfo->pRenderEngine->OnDrawImage(m_rcClient, strImage);
	}

	bool CControlUI::Invalidate()
	{
		//调用Update函数，才是正确的重绘方式
		//用户尽量不要直接调用该函数，因为该函数不会进行区域调整
		RECT rect;
		if(m_pShareInfo && GetItemDisplayRect(rect))
		{
			InvalidateRect(m_pShareInfo->hWnd, &rect, FALSE);
			return true;
		}
		return false;
	}

	bool CControlUI::Update()
	{
		CControlUI * pControl = this;
		if(m_pParentContrl)
		{
			pControl = m_pParentContrl;
		}

		pControl->SetItem(pControl->GetItem(), true);
		return pControl->Invalidate();
	}

    bool CControlUI::Renewal()
    {
        this->SetItem(this->GetItem(), true);
        return this->Invalidate();
    }

	bool CControlUI::OnPaint(const RECT& rcUpdate)
	{
		//这个rcItem跟m_rcItem的值有点不同，
		//m_rcItem是相对于父控件的位置
		//rcItem是相对整个窗口的位置，是一个绝对位置
		if(!m_bVisible || !m_pShareInfo) return false;

		//rcPaint是父窗口提供的有效绘图区域
		//rcClient参数是m_rcItem的绝对位置, 所以要将m_rcClient偏移到绝对位置：

		/*从这里进行缓冲绘制*/
		PaintBkColor(rcUpdate);
		PaintBkImage(rcUpdate);
		PaintStatusImage(rcUpdate);
		PaintText(rcUpdate);
		PaintBorder(rcUpdate);

		return true;
	}

	void CControlUI::SetState(DWORD dwState)
	{
		m_dwState = dwState;
	}

	void CControlUI::AddState(DWORD dwState)
	{
		dwState |= m_dwState;
		SetState(dwState);
	}

	void CControlUI::RemoveState(DWORD dwState)
	{
		dwState = m_dwState & (~dwState);
		SetState(dwState);
	}

	DWORD CControlUI::GetState() const
	{
		return m_dwState;
	}

	CMuiString CControlUI::GetName() const
	{
		return m_strName;
	}

	void CControlUI::SetName(LPCTSTR pstrText)
	{
		m_strName = pstrText;
	}

	void CControlUI::SetTag(UINT_PTR pTag)
	{
		m_pTag = pTag;
	}

	UINT_PTR CControlUI::GetTag() const
	{
		return m_pTag;
	}

	void CControlUI::SetTipText(LPCTSTR strTipText)
	{
		m_strToolTip = strTipText;
	}

	LPCTSTR CControlUI::GetTipText() const
	{
		return m_strToolTip;
	}

	LPVOID CControlUI::GetInterface(LPCTSTR strName)
	{
		if (0 == _tcsicmp(strName, _T("CItemViewInfo")))
		{
			return static_cast<CItemViewInfo*>(this);
		}
		else if (0 == _tcsicmp(strName, _T("CItemPosition")))
		{
			return static_cast<CItemPosition*>(this);
		}
		else if (0 == _tcsicmp(strName, _T("CUIMessage")))
		{
			return static_cast<CUIMessage*>(this);
		}
		else if (0 == _tcsicmp(strName, _T("CControlUI")))
		{
			return static_cast<CControlUI*>(this);
		}

		return NULL;
	}

	CMuiString CControlUI::GetClassName() const
	{
		return _T("CControlUI");
	}

	CControlUI * CControlUI::GetAncestorControl()
	{
		if(m_pParentContrl)
		{
			return m_pParentContrl->GetAncestorControl();
		}
		return this;
	}

	HWND CControlUI::GetWindow() const
	{
		return NULL == m_pShareInfo ? NULL : m_pShareInfo->hWnd;
	}

	//发出Notify通知
	BOOL CControlUI::SendNotify(EnumNotify emNotify, WPARAM wParam/* = NULL*/, LPARAM lParam/* = NULL*/)
	{
		MUINOTIFY Notify;
		BOOL bChild = m_pParentContrl ? m_pParentContrl->IsControl() : FALSE;
        INotify * pNotify = m_pShareInfo ? m_pShareInfo->pNotify : NULL;
        Notify.pSender = this;
        Notify.dwType = emNotify;
        Notify.wParam = wParam;
        Notify.lParam = lParam;

		if (bChild != bChild)
		{
			//如果bChild = TRUE, 则说明这个控件作为了一个Item，不能发出Notify
			//何为item? 比如listBox中的每一项，就是一个Item，如果点击了listBox
			//的某一项，那么不应该由item发出通知，应该由listbox发出通知

            if (m_pParentContrl && m_pParentContrl->m_pShareInfo)
            {
                pNotify = m_pParentContrl->m_pShareInfo->pNotify;
                Notify.pSender = m_pParentContrl;
            }
            else
            {
                return false;
            }
		}
		
        if (pNotify)
        {
            pNotify->SendNotify(Notify);
            return true;
        }
        
        return false;
	}


    void CControlUI::SetMenu(IMenuPopup * pMenu)
    {
        m_pMenu = pMenu;
    }

    IMenuPopup * CControlUI::GetMenu() const
    {
        return m_pMenu;
    }

	BOOL CControlUI::SetCapture()
	{
		MUIASSERT(m_pShareInfo && m_pShareInfo->hWnd);
		if(!m_pShareInfo || !::IsWindow(m_pShareInfo->hWnd)) return FALSE;

		//必须先发出这个消息
		::SendMessage(m_pShareInfo->hWnd, WM_CANCELMODE, 1, (LPARAM)this);
		if(m_pShareInfo->hWnd != ::GetCapture())
		{
			::SetCapture(m_pShareInfo->hWnd);
		}
		
		AddState(STATE_PUSHED);
		return TRUE;
	}

	BOOL CControlUI::ReleaseCapture()
	{
		MUIASSERT(m_pShareInfo && m_pShareInfo->hWnd);
		if(!m_pShareInfo || !m_pShareInfo->hWnd) return FALSE;

		::ReleaseCapture();
		::SendMessage(m_pShareInfo->hWnd, WM_CANCELMODE, 1, NULL);
		RemoveState(STATE_PUSHED);
		return TRUE;
	}

	BOOL CControlUI::SetHooker(IControlHooker * pHooker, int index /*= -1 放到最后一个*/)
	{
		if(-1 == index)
		{
			return !!m_Hookers.Add(pHooker);
		}
		else
		{
			return !!m_Hookers.InsertAt(index, pHooker);
		}
		return FALSE;
	}

	BOOL CControlUI::UnHooker(IControlHooker * pHooker)
	{
		//因为一个Hooker可能寄生于多个CControlUI中，所以UnHooker是不会delete Hooker的
		//请用户选择适当的时机，delete所有hooker
		return !!m_Hookers.Remove(pHooker);
	}

	IControlHooker * CControlUI::UnHooker(int index)
	{
		//因为一个Hooker可能寄生于多个CControlUI中，所以UnHooker是不会delete Hooker的
		//请用户选择适当的时机，delete所有hooker
		return (IControlHooker *)m_Hookers.Remove(index, 1);
	}

	BOOL CControlUI::SendWindowMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lResult)
	{
		LRESULT lRet = 0;
		if (m_pShareInfo && ::IsWindow(m_pShareInfo->hWnd))
		{
			lRet = ::SendMessage(m_pShareInfo->hWnd, message, wParam, lParam);

			if (lResult) *lResult = lRet;
			return TRUE;
		}
		return FALSE;
	}

	BOOL CControlUI::PostWindowMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (m_pShareInfo && ::IsWindow(m_pShareInfo->hWnd))
		{
			return ::PostMessage(m_pShareInfo->hWnd, message, wParam, lParam);
		}
		return FALSE;
	}

	CControlUI * CControlUI::GetParent() const
	{
		return m_pParentContrl;
	}

	VOID CControlUI::SetParent(CControlUI* pParentContrl)
	{
		m_pParentContrl = pParentContrl;
		if(m_pParentContrl)
		{
			SetShareInfo(m_pParentContrl->m_pShareInfo);
		}
		else
		{
			SetShareInfo(nullptr);
		}
	}

	LRESULT CControlUI::CallWndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		IControlHooker * pHooker;
		int nCount = m_Hookers.GetSize();
		LRESULT lResule = 0;
		HWND hWnd = m_pShareInfo ? m_pShareInfo->hWnd : NULL;
        if (false == m_bEnabled)
        {
            //当控件禁用，不接受键鼠消息
            if (message >= WM_MOUSEFIRST && WM_MOUSELAST >= message)
            {
                return 0;
            }

            if (message >= WM_KEYFIRST && WM_KEYLAST >= message)
            {
                return 0;
            }
        }

#ifdef ENABLE_CONTROL_HOOK
		for(int i=0; nCount > i; i++)
		{
			pHooker = (IControlHooker *)m_Hookers[i];
			if(true == pHooker->OnBefore(this, hWnd, message, wParam, lParam, lResule))
			{
				//返回真就不往下转发了
				return lResule;
			}
		}
#endif

		lResule = WndProc(message, wParam, lParam);

#ifdef ENABLE_CONTROL_HOOK
		for(int i=0; nCount > i; i++)
		{
			pHooker = (IControlHooker *)m_Hookers[i];
			if(true == pHooker->OnAfter(this, hWnd, message, wParam, lParam, lResule))
			{
				//返回真就不往下转发了
				return lResule;
			}
		}
#endif

		return lResule;
	}

	LRESULT CControlUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		CControlUI * pControl = NULL;
        POINT Point = { 0, 0 };
        RECT rcPos = { 0 };
        SIZE Size = { 0 };
		bool bRet = false;
		HWND hWnd = m_pShareInfo ? m_pShareInfo->hWnd : NULL;
		//如果要产生一条OnNotify通知，返回true;
		switch(message)
		{
		case WM_TIMER:
			{
				SendNotify(EnumNotify::TimerCall, wParam, lParam);
			}break;
		case WM_LBUTTONDOWN:
			{

			}break;
		case WM_LBUTTONUP:
			{
                
			}break;
		case WMU_LBUTTONCLICK:
			{

			}break;
		case WM_MOUSEMOVE:
			{
                Point.x = (short)GET_X_LPARAM(lParam);
                Point.y = (short)GET_Y_LPARAM(lParam);

			}break;
		case WMU_MOUSEENTER://鼠标第一次进入
			{
				if(m_bEnabled)
				{
					AddState(STATE_HOT);
				}
			}break;
		case WM_MOUSEHOVER://鼠标停留
			{
				if(false == m_strToolTip.IsEmpty() && true == GetItemFixedRect(rcPos))
				{
					Point.x = (short)GET_X_LPARAM(lParam) + rcPos.left;
					Point.y = (short)GET_Y_LPARAM(lParam) + rcPos.top;
					SendNotify(EnumNotify::ShowTip, MAKELONG(Point.x, Point.y), (LPARAM)m_strToolTip.GetData());
				}

			}break;
		case WM_MOUSELEAVE://鼠标离开
			{
				if(m_bEnabled)
				{
					RemoveState(STATE_HOT);
				}

				if(m_pShareInfo && m_pShareInfo->pToolTip)
				{
					m_pShareInfo->pToolTip->ShowWindow(FALSE);
				}
			}break;
		case WM_MOUSEWHEEL:
			{
				//WM_MOUSEWHEEL消息，自己不能处理的话，提交给父窗口处理
				if(m_pParentContrl)
				{
					return m_pParentContrl->SendMessage(message, wParam, lParam);
				}
			}break;
		case WM_RBUTTONDOWN:
			{
			}break;
		case WM_RBUTTONUP:
		{
            if (m_pMenu && GetItemFixedRect(rcPos))
            {
                Point.x = (short)LOWORD(lParam);
                Point.y = (short)HIWORD(lParam);

                rcPos.left += Point.x;
                rcPos.top += Point.y;

                rcPos.right = rcPos.left;
                rcPos.bottom = rcPos.top;
                ::MapWindowPoints(m_pShareInfo->hWnd, NULL, (LPPOINT)&rcPos, 2);

                Size = m_pMenu->GetSize();
                Point = CalcPopupPoint(&rcPos, &Size, CPOT_RIGHT);

                SendWindowMessage(WMU_POPUPMENU, (WPARAM)m_pMenu, MAKELONG(Point.x, Point.y));
            }
		}break;
		case WM_SETCURSOR:
			{
				if(false == m_bEnabled)//禁用
				{
					SetCursor(LoadCursor(NULL, IDC_NO));
				}
				else
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
			}break;
		case WM_SETFOCUS:
			{
				AddState(STATE_FOCUS);
				Invalidate();
				SendNotify(EnumNotify::SetFocus);
			}break;
		case WM_KILLFOCUS:
			{
				RemoveState(STATE_FOCUS);
				Invalidate();
				SendNotify(EnumNotify::KillFocus);
			}break;
		default:
			break;
		}
		return 0;
	}
}