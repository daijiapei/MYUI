
#include "UIControl.h"

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
		TRACE(_T("CControlUI::~CControlUI"));
	}

	bool CControlUI::IsControl()
	{
		return m_bControl;
	}

	bool CControlUI::GetItemDisplayRect(RECT &rcDisplay)
	{
		RECT rc = m_rcRawItem;
		OffsetRect(&rc, m_rcItem.left, m_rcItem.top);
		return GetDisplayRect(rcDisplay, rc);
	}

	bool CControlUI::GetClientDisplayRect(RECT &rcDisplay)
	{
		RECT rc = m_rcClient;
		OffsetRect(&rc, m_rcItem.left, m_rcItem.top);
		return GetDisplayRect(rcDisplay, rc);
	}

	bool CControlUI::GetDisplayRect(RECT &rcDisplay,const RECT &rc)
	{
		//控件在窗口中显示的位置，如果在窗口中有显示，则返回true
		//没有显示则返回false
		//注意，如果在绝对布局中，发生重叠的控件，
		//这个函数不会考虑重叠问题，返回的区域可能大于显示区域
		RECT rcParentClient;
		RECT rcDisplayTmp = rc;

		CControlUI * pControl = this;
		IScrollBarMove * pInterface = nullptr;

		if(false == m_bVisible) return false;

		while(pControl = pControl->GetParent())
		{
			if(false == pControl->IsVisible())
			{
				return false;
			}

			pInterface = dynamic_cast<IScrollBarMove *>(pControl);
			if(pInterface)
			{
				OffsetRect(&rcDisplayTmp, - pInterface->GetScrollBarShift().cx,
					-pInterface->GetScrollBarShift().cy);
			}

			rcParentClient = pControl->GetItemClient();
			OffsetRect(&rcDisplayTmp, rcParentClient.left, rcParentClient.top);
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
		RECT rc = m_rcRawItem;
		OffsetRect(&rc, m_rcItem.left, m_rcItem.top);
		return GetFixedRect(rcFixed, rc);
	}

	bool CControlUI::GetClientFixedRect(RECT &rcFixed)
	{
		RECT rc = m_rcClient;
		OffsetRect(&rc, m_rcItem.left, m_rcItem.top);
		return GetFixedRect(rcFixed, rc);
	}

	bool CControlUI::GetFixedRect(RECT &rcFixed, const RECT &rc)
	{
		RECT rcParentClient;
		RECT rcFixedTmp = rc;

		CControlUI * pControl = this;
		IScrollBarMove * pInterface = nullptr;

		if(false == m_bVisible) return false;

		while(pControl = pControl->GetParent())
		{
			if(false == pControl->IsVisible())
			{
				return false;
			}

			pInterface = dynamic_cast<IScrollBarMove *>(pControl);
			if(pInterface)
			{
				OffsetRect(&rcFixedTmp, - pInterface->GetScrollBarShift().cx,
					-pInterface->GetScrollBarShift().cy);
			}

			rcParentClient = pControl->GetItemClient();
			OffsetRect(&rcFixedTmp, rcParentClient.left, rcParentClient.top);
		}
		rcFixed = rcFixedTmp;
		return true;
	}

	void CControlUI::SetShareInfo(TSHAREINFO * pShareInfo)
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
			ASSERT(CheckBoer(strValue));
#endif
			SetVisible(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("enabled"))) 
		{
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			SetEnabled(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("Penetrated"))) 
		{
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
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
			TRACE(_T("SetAttribute: Item=%s, Value=%s"), strItem, strValue);
			ASSERT(0 && "CControlUI::SetAttribute：no define strItem");
		}
	}

	CControlUI * CControlUI::FindControlByPoint(POINT &pt)
	{
		HRGN hrgn = NULL;
		CControlUI * pControl = NULL;
		if(m_szBorderRound.cx || m_szBorderRound.cy)
		{
			hrgn = ::CreateRoundRectRgn(m_rcRawItem.left, m_rcRawItem.top, m_rcRawItem.right + 1,
				m_rcRawItem.bottom + 1, m_szBorderRound.cx, m_szBorderRound.cy);
			if(TRUE == ::PtInRegion(hrgn, pt.x, pt.y))
			{
				pControl = this;
			}
			::DeleteObject(hrgn);
		}
		else if(false == m_bPenetrated && PointInRect(pt, m_rcRawItem))
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
		ASSERT(m_pShareInfo && m_pShareInfo->hWnd);
		if(!m_pShareInfo || !m_pShareInfo->hWnd) return false;
		::SendMessage(m_pShareInfo->hWnd, WM_SETFOCUS, NULL, (LPARAM)this);
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

	void CControlUI::PaintBkColor(const RECT& rcItem, const RECT& rcPaint)
	{
		//RECT rcClient = m_rcRawItem;
		//OffsetRect(&rcClient, rcItem.left, rcItem.top);
		m_pShareInfo->pRenderEngine->OnDrawColor(rcItem, m_refBkColor);
	}

	void CControlUI::PaintStatusImage(const RECT& rcItem, const RECT& rcPaint)
	{
		return ;
	}

	void CControlUI::PaintText(const RECT& rcItem, const RECT& rcPaint)
	{
		return;
	}

	void CControlUI::PaintBorder(const RECT& rcItem, const RECT& rcPaint)
	{
		m_pShareInfo->pRenderEngine->OnDrawBroder(rcItem ,m_refBorderColor, m_nBorderSize, m_szBorderRound);
	}

	void CControlUI::PaintBkImage(const RECT& rcItem, const RECT& rcPaint)
	{
		LPCTSTR strImage = m_strBkImage;
		RECT rcClient = m_rcClient;
		if(!m_strDisabledImage.IsEmpty() && false == IsEnabled())
		{
			strImage = m_strDisabledImage;
		}

		if(_T('\0') == strImage[0]) return;
		OffsetRect(&rcClient, rcItem.left, rcItem.top);
		m_pShareInfo->pRenderEngine->OnDrawImage(rcClient, strImage);
	}

	void CControlUI::OnAttach(HWND hNewWnd)
	{
		return;
	}

	void CControlUI::OnDetach(HWND hOldWnd)
	{
		return;
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

	bool CControlUI::OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate)
	{
		//这个rcItem跟m_rcItem的值有点不同，
		//m_rcItem是相对于父控件的位置
		//rcItem是相对整个窗口的位置，是一个绝对位置
		if(!m_bVisible || !IsValidRect(rcPaint) || !m_pShareInfo) return false;

		if(FALSE == IsContainRect(rcPaint, rcUpdate))
		{
			return false;
		}

		//rcPaint是父窗口提供的有效绘图区域
		//rcClient参数是m_rcItem的绝对位置, 所以要将m_rcClient偏移到绝对位置：

		/*从这里进行缓冲绘制*/
		PaintBkColor(rcItem, rcPaint);
		PaintBkImage(rcItem, rcPaint);
		PaintStatusImage(rcItem, rcPaint);
		PaintText(rcItem, rcPaint);
		PaintBorder(rcItem, rcPaint);

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
	bool CControlUI::SendNotify(BOOL bChild, EnumNotifyMsg dwType, WPARAM wParam/* = NULL*/, LPARAM lParam/* = NULL*/)
	{
        TNOTIFYUI Notify;
        INotify * pNotify = m_pShareInfo ? m_pShareInfo->pNotify : NULL;
        Notify.pSender = this;
        Notify.dwType = dwType;
        Notify.wParam = wParam;
        Notify.lParam = lParam;

		if(bChild)
		{
			//如果bChild = TRUE, 则说明这个控件作为了一个Item，不能发出Notify
			//何为item? 比如listBox中的每一项，就是一个Item，如果点击了listBox
			//的某一项，那么不应该由item发出通知，应该由listbox发出通知

			//不能发出notify，则我们通知父窗口
			ASSERT(m_pParentContrl && "SendNotify.bChild = TURE ,m_pParentContrl 不能为空！");

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

	BOOL CControlUI::SetTimer(UINT nIDEvnet, UINT uElapse)
	{
		CONTROLTIMER timer;
		//bNotify 是否产生 Notify通知，有些控件希望自己在内部处理消息，不需要发出Notify
		if(nIDEvnet > sizeof(CControlUI))
		{
			//为了保证ID唯一性，Timer的id以this指针为基址进行偏移，所以为了保证
			//id 唯一性，id 必须在控件大小的范围内，这样才不会与其他控件产生冲突
			ASSERT(0 && "nIDEvnet 不能大于sizeof(CControlUI)，请重新设置你的nIDEvnet");
			return false;
		}

		if(!m_pShareInfo || !m_pShareInfo->hWnd)
		{
			ASSERT(0 && "控件还没有附加到窗口，请附加到窗口后再SetTimer");
			return false;
		}

		timer.pControl = this;
		timer.nIDEvnet = nIDEvnet;
		timer.uElapse = uElapse;
		return !!::SendMessage(m_pShareInfo->hWnd, WM_SETTIMER, TRUE, (LPARAM)&timer);
	}

	BOOL CControlUI::KillTimer(UINT nIDEvnet)
	{
		CONTROLTIMER timer;
		if(nIDEvnet > sizeof(CControlUI))
		{
			//为了保证ID唯一性，Timer的id以this指针为基址进行偏移，所以为了保证
			//id 唯一性，id 必须在控件大小的范围内，这样才不会与其他控件产生冲突
			ASSERT(0 && "nIDEvnet 不能大于sizeof(CControlUI)，请重新设置你的nIDEvnet");
			return false;
		}

		if(!m_pShareInfo || !m_pShareInfo->hWnd)
		{
			ASSERT(0 && "控件还没有附加到窗口，请附加到窗口后再KillTimer");
			return false;
		}
		
		timer.pControl = this;
		timer.nIDEvnet = nIDEvnet;
		timer.uElapse = NULL;
		return !!::SendMessage(m_pShareInfo->hWnd, WM_SETTIMER, FALSE, (LPARAM)&timer);
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
		ASSERT(m_pShareInfo && m_pShareInfo->hWnd);
		if(!m_pShareInfo || !m_pShareInfo->hWnd) return FALSE;

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
		ASSERT(m_pShareInfo && m_pShareInfo->hWnd);
		if(!m_pShareInfo || !m_pShareInfo->hWnd) return FALSE;

		::SendMessage(m_pShareInfo->hWnd, WM_CANCELMODE, 1, NULL);
		::ReleaseCapture();
		RemoveState(STATE_PUSHED);
		return TRUE;
	}

	BOOL CControlUI::SetHooker(IControlHooker * pHooker, int index /*= -1 放到最后一个*/)
	{
		if(-1 == index)
		{
			return !!m_pHookers.Add(pHooker);
		}
		else
		{
			return !!m_pHookers.InsertAt(index, pHooker);
		}
		return FALSE;
	}

	BOOL CControlUI::UnHooker(IControlHooker * pHooker)
	{
		//因为一个Hooker可能寄生于多个CControlUI中，所以UnHooker是不会delete Hooker的
		//请用户选择适当的时机，delete所有hooker
		return !!m_pHookers.Remove(pHooker);
	}

	IControlHooker * CControlUI::UnHooker(int index)
	{
		//因为一个Hooker可能寄生于多个CControlUI中，所以UnHooker是不会delete Hooker的
		//请用户选择适当的时机，delete所有hooker
		return (IControlHooker *)m_pHookers.Remove(index, 1);
	}

	//用法跟windows的SendMessage一样
	LRESULT CControlUI::SendMessage(UINT message, WPARAM wParam/* = NULL*/, LPARAM lParam/* = NULL*/)
	{
		return WndProc(m_pShareInfo ? m_pShareInfo->hWnd : NULL, message, wParam, lParam);
	}

	//用法跟windows的PostMessage一样
	LRESULT CControlUI::PostMessage(UINT message, WPARAM wParam/* = NULL*/, LPARAM lParam /*= NULL*/)
	{
		ASSERT(m_pShareInfo && ::IsWindow(m_pShareInfo->hWnd));
		MSG * msg = new MSG;
		memset(msg, 0, sizeof(TMSG));

		msg->hwnd = m_pShareInfo->hWnd;
		msg->message = message;
		msg->wParam = wParam;
		msg->lParam = lParam;

		return ::PostMessage(m_pShareInfo ? m_pShareInfo->hWnd : NULL, WM_CONTROLMSG_POST,(WPARAM)this, (LPARAM)msg);
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

	LRESULT CControlUI::CallWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		IControlHooker * pHooker;
		int nCount = m_pHookers.GetSize();
		LRESULT lResule = 0;

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
			pHooker = (IControlHooker *)m_pHookers[i];
			if(true == pHooker->OnBefore(this, hWnd, message, wParam, lParam, lResule))
			{
				//返回真就不往下转发了
				return lResule;
			}
		}
#endif

		lResule = WndProc(hWnd, message, wParam, lParam);

#ifdef ENABLE_CONTROL_HOOK
		for(int i=0; nCount > i; i++)
		{
			pHooker = (IControlHooker *)m_pHookers[i];
			if(true == pHooker->OnAfter(this, hWnd, message, wParam, lParam, lResule))
			{
				//返回真就不往下转发了
				return lResule;
			}
		}
#endif

		return lResule;
	}

	LRESULT CControlUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		CControlUI * pControl = NULL;
        POINT Point = { 0, 0 };
        RECT rcPos = { 0 };
        SIZE Size = { 0 };
		bool bRet = false;
		//如果要产生一条OnNotify通知，返回true;
		switch(message)
		{
		case WM_TIMER:
			{
				SendNotify(!hWnd, EnumNotifyMsg::TimerCall, wParam, lParam);
			}break;
		case WM_LBUTTONDOWN:
			{

			}break;
		case WM_LBUTTONUP:
			{
                
			}break;
		case WM_LBUTTONCLICK:
			{

			}break;
		case WM_MOUSEMOVE:
			{
                Point.x = (short)LOWORD(lParam);
                Point.y = (short)HIWORD(lParam);

			}break;
		case WM_MOUSEENTER://鼠标第一次进入
			{
				if(m_bEnabled)
				{
					AddState(STATE_HOT);
				}
			}break;
		case WM_MOUSEHOVER://鼠标停留
			{
				if(m_pShareInfo && m_pShareInfo->pToolTip && !m_strToolTip.IsEmpty())
				{
					m_pShareInfo->pToolTip->SetText(m_strToolTip);
                    ::GetCursorPos(&Point);
                    m_pShareInfo->pToolTip->ShowTip(Point);
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

                ::PostMessage(GETHWND(this), WM_POPUPMENU, (WPARAM)m_pMenu, MAKELONG(Point.x, Point.y));
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
				SendNotify(FALSE, EnumNotifyMsg::SetFocus);
			}break;
		case WM_KILLFOCUS:
			{
				RemoveState(STATE_FOCUS);
				Invalidate();
				SendNotify(FALSE, EnumNotifyMsg::KillFocus);
			}break;
		default:
			break;
		}
		return 0;
	}
}