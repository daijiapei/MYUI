
#include "UIWindow.h"
#include "UIThread.h"
#include "../Render/GdiEngine.h"
#include "../Control/UIOption.h"
#include "../Layout/UIVerticalLayout.h"
#include "../ExtControl/UIToolTip.h"

#define ISMOUSEDOWN(m) \
((WM_LBUTTONDOWN == m \
|| WM_MBUTTONDOWN == m \
|| WM_RBUTTONDOWN == m) ? 1:0)

namespace MYUI
{
	CWindowUI::CWindowUI() 
		: m_nFontId(-1)
		, m_dwHoverTime(800)
		, m_bShowInScreen(FALSE)
		, m_pHost(NULL)
	{
		m_pShareInfo = new MUISHAREINFO;
		m_pViewInfo = new MUIVIEWINFO;
		memset(m_pShareInfo, 0, sizeof(MUISHAREINFO));
		memset(m_pViewInfo, 0, sizeof(MUIVIEWINFO));

		m_pViewInfo->SizeMax.cx = m_pViewInfo->SizeMax.cy = 99999;
		m_pViewInfo->ptLeftMouse.x = m_pViewInfo->ptLeftMouse.y = 0;
	}

	CWindowUI::~CWindowUI()
	{

		for (int i = 0; m_pShareInfo->FontArray.GetSize() > i; i++)
		{
			DeleteObject((HFONT)m_pShareInfo->FontArray.GetAt(i));
		}

        for (int i = 0; m_MenuArray.GetSize() > i; i++)
        {
            IMenuPopup * pMenu = reinterpret_cast<IMenuPopup*>(m_MenuArray.GetAt(i));
            delete pMenu;
        }

		delete m_pShareInfo;
		delete m_pViewInfo;

	}

    LPVOID CWindowUI::GetInterface(LPCTSTR strName)
    {
        if (0 == _tcsicmp(strName, _T("IWindowEvent")))
        {
            return static_cast<IWindowEvent*>(this);
        }
        else if (0 == _tcsicmp(strName, _T("IControlNotify")))
        {
            return static_cast<IControlNotify*>(this);
        }
        else if (0 == _tcsicmp(strName, _T("INotify")))
        {
            return static_cast<INotify*>(this);
        }
        else if (0 == _tcsicmp(strName, _T("CWindowUI")))
        {
            return static_cast<CWindowUI*>(this);
        }

        return NULL;
    }

	void CWindowUI::SetMinSize(SIZE size)
	{
		m_pViewInfo->SizeMin = size;
	}

	const SIZE &CWindowUI::GetMinSize() const
	{
		return m_pViewInfo->SizeMin;
	}

	void CWindowUI::SetMaxSize(SIZE size)
	{
		m_pViewInfo->SizeMax = size;
	}

	const SIZE &CWindowUI::GetMaxSize() const
	{
		return m_pViewInfo->SizeMax;
	}

	void CWindowUI::SetTextFont(int FontId)
	{
		m_nFontId = FontId;
	}

	int CWindowUI::GetTextFont() const
	{
		return m_nFontId;
	}

	void CWindowUI::SetHoverTime(DWORD dwHoverTime)
	{
		m_dwHoverTime = dwHoverTime;
	}

	DWORD CWindowUI::GetHoverTimer() const
	{
		return m_dwHoverTime;
	}

    BOOL CWindowUI::UpdateLayeredWindow(COLORREF refKey, DWORD dwFlags /*= ULW_ALPHA*/)
    {
        POINT Point = { 0 };
        RECT Rect = { 0 };
        BLENDFUNCTION Blend = { 0 };
        SIZE Size = { 0 };
        DWORD dwStyleEx = NULL;
        HDC hdcDest = NULL;
        HDC hdcSrc = NULL;
        BOOL bRet = FALSE;

        dwStyleEx = GetWindowLong(m_hWnd, GWL_EXSTYLE);
        dwStyleEx |= WS_EX_LAYERED;
        SetWindowLong(m_hWnd, GWL_EXSTYLE, dwStyleEx);

        Blend.BlendOp = 0;
        Blend.BlendFlags = 0;
        Blend.AlphaFormat = 1;
        Blend.SourceConstantAlpha = 255;

        ::GetClientRect(m_hWnd, &Rect);
        Size.cx = Rect.right - Rect.left;
        Size.cy = Rect.bottom - Rect.top;

        m_pShareInfo->pRenderEngine->BeginPaint(Rect);
        m_pViewInfo->pRootControl->OnPaint(Rect);

        hdcDest = GetDC(m_hWnd);
        hdcSrc = m_pShareInfo->pRenderEngine->GetMemDC();

        bRet = ::UpdateLayeredWindow(m_hWnd, hdcDest, &Point, &Size,
            hdcSrc, &Point, refKey, &Blend, dwFlags);

        EmptyRect(Rect);
        m_pShareInfo->pRenderEngine->EndPaint(Rect);
        ::ReleaseDC(m_hWnd, hdcDest);

        return bRet;
    }

    LRESULT CWindowUI::Popup(RECT rcPos)
    {
        MUIASSERT(::IsWindow(m_hWnd));
        LRESULT lResult = 0;
        MSG Msg = { 0 };
        CControlUI * pControl = NULL;
        ::SetWindowPos(m_hWnd, HWND_TOPMOST, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
            rcPos.bottom - rcPos.top, SWP_SHOWWINDOW);

        while (TRUE)
        {
            pControl = NULL;
            if (::IsWindow(m_hWnd) && PeekMessage(&Msg, 0, 0, 0, PM_NOREMOVE))
            {
                if (Msg.hwnd)
                {
                    switch (Msg.message)
                    {
                    case WMU_POPUPDIALOG:
                    {
                        if (m_hWnd != Msg.hwnd) goto BREAKLOOP;
                    }break;
                    case WMU_POPUPMENU:
                    {

                        if (this->GetInterface(_T("IMenuPopup")) == (LPVOID)Msg.wParam)
                        {
                            //同一个菜单, 不需要重复弹出
                            PeekMessage(&Msg, 0, Msg.message, Msg.message, PM_REMOVE);
                            continue;
                        }

                        if (m_hWnd != Msg.hwnd) goto BREAKLOOP;
                    }break;
                    case WMU_BREAKLOOP:
                    {
                        if (FALSE == Msg.wParam) goto BREAKLOOP;//不需要做任何判断，单纯地退出

                        pControl = reinterpret_cast<CControlUI *>(Msg.lParam);
                        
                        if (m_hWnd != GetFocus() && (pControl ? !pControl->IsFocued() : TRUE))
                        {
                            goto BREAKLOOP;
                        }
                    }break;
                    default:
                        break;
                    }
                }
                
                PeekMessage(&Msg, 0, Msg.message, Msg.message, PM_REMOVE);

                ::TranslateMessage(&Msg);
                ::DispatchMessage(&Msg);
            }
            else
            {
                WaitMessage();
            }
        }

    BREAKLOOP:
		if (::IsWindow(m_hWnd) && ::IsWindowVisible(m_hWnd))
		{
			::ShowWindow(m_hWnd, FALSE);
		}

        return Msg.message;
    }

	void CWindowUI::AttachFrameView(CControlUI * pControl)
	{
		if(pControl == m_pViewInfo->pRootControl) return ;

		if(NULL != m_pViewInfo->pRootControl)
		{
            m_pViewInfo->pRootControl->SetParent(NULL);
			delete m_pViewInfo->pRootControl;
			m_pViewInfo->pRootControl = NULL;
		}
		m_pViewInfo->pRootControl = static_cast<CControlUI*>(pControl);

		m_pViewInfo->pHotControl = NULL;
		m_pViewInfo->pFocusControl = NULL;
		m_pViewInfo->pPushedControl = NULL;
		m_pShareInfo->pNotify = dynamic_cast<INotify *>(this) ;
		
		if(m_pViewInfo->pRootControl)
		{
			m_pViewInfo->pRootControl->SetParent(NULL);
			m_pViewInfo->pRootControl->SetShareInfo(m_pShareInfo);
		}
		InvalidateRect(m_hWnd, NULL, FALSE);
	}

	CControlUI * CWindowUI::GetFrameView() const
	{
		return m_pViewInfo->pRootControl;
	}


	//file/folder='' resid='' restype=''
	//filename/foldername
	//strSkin == NULL 相当于卸载
	bool CWindowUI::SetSkin(LPCTSTR strSkin)
	{
        bool bRet = false;
        if (m_pShareInfo->pRenderEngine)
        {
            bRet = m_pShareInfo->pRenderEngine->SetSkinFolder(strSkin);

            if (bRet)
            {
				m_pShareInfo->strSkinFolder = strSkin ? strSkin : _T("");

				for (int i = 0; m_Parasite.GetSize() > i; i++)
				{
					CWindowUI* pParasite = (CWindowUI*)m_Parasite[i];
					pParasite->SetSkin(strSkin);
				}

				if (m_hWnd && ::IsWindow(m_hWnd))
				{
					InvalidateRect(m_hWnd, NULL, FALSE);
				}
            }
        }
        return bRet;
	}

	LPCTSTR CWindowUI::GetSkin() const
	{
		if (m_pShareInfo->pRenderEngine)
		{
			m_pShareInfo->pRenderEngine->GetSkinFolder();
		}
		return NULL;
	}

	bool CWindowUI::AddFont(int Fontid, HFONT hFont)
	{
		MUIASSERT(m_pShareInfo);

		if (-1 == Fontid || NULL == hFont) return false;

		if (false == m_pShareInfo->FontArray.Add(Fontid, hFont))
		{
			return false;
		}

		//在 builder 的时候，下面两个都不会执行的，不会浪费效率
		for (int i = 0; m_Parasite.GetSize() > i; i++)
		{
			CWindowUI* pParasite = (CWindowUI*)m_Parasite[i];
			pParasite->AddFont(Fontid, hFont);
		}

		if (m_pViewInfo && m_pViewInfo->pRootControl)
		{
			m_pViewInfo->pRootControl->CallWndProc(WM_FONTCHANGE, 0, 0);
		}

		return true;
	}

	HFONT CWindowUI::GetFont(int Fontid)
	{
		MUIASSERT(m_pShareInfo);
		if(!m_pShareInfo) return NULL;

		return (HFONT)m_pShareInfo->FontArray.Select(Fontid);
	}

	bool CWindowUI::AddGroup(LPCTSTR strGroup)
	{
		return m_GroupArray.Insert(strGroup, NULL);
	}

	void CWindowUI::SetCaption(CControlUI * pControl)
	{
		MUIASSERT(m_pViewInfo);
		m_pViewInfo->pCaptionControl = pControl;
	}

	CControlUI * CWindowUI::GetCaption() const
	{
		return m_pViewInfo->pCaptionControl;
	}

	CControlUI * CWindowUI::GetFocusControl()
	{
		MUIASSERT(m_pViewInfo);
		return m_pViewInfo->pFocusControl;
	}

	CControlUI* CWindowUI::FindControl(LPCTSTR strName)
	{
		if (NULL == m_pViewInfo || NULL == m_pViewInfo->pRootControl)
		{
			return NULL;
		}

		return m_pViewInfo->pRootControl->FindControlByName(strName);
	}

	CControlUI* CWindowUI::FindControl(POINT& Point)
	{
		if (NULL == m_pViewInfo || NULL == m_pViewInfo->pRootControl)
		{
			return NULL;
		}

		return m_pViewInfo->pRootControl->FindControlByPoint(Point);
	}

	CControlUI* CWindowUI::GetRootControl()
	{
		if (NULL == m_pViewInfo)
		{
			return NULL;
		}

		return m_pViewInfo->pRootControl;
	}

	bool CWindowUI::SetHost(CWindowUI* pHost)
	{

		if (m_pHost)
		{
			m_pHost->RemoveParasite(this);
			m_pHost = NULL;
		}

		if (pHost)
		{
			m_pHost = pHost;
			return pHost->AddParasite(this);
		}
		
		return true;
	}

	bool CWindowUI::AddParasite(CWindowUI* pParasite)
	{
		return m_Parasite.Add(pParasite);
	}

	bool CWindowUI::RemoveParasite(CWindowUI* pParasite)
	{
		return m_Parasite.Remove(pParasite);
	}

    BOOL CWindowUI::PostBreakMessage(BOOL bCheck, CControlUI * pParent)
    {
        MUIASSERT(m_hWnd);
        HWND hWnd = ::GetWindowOwner(m_hWnd);

        hWnd = hWnd ? hWnd : m_hWnd;

        return ::PostMessage(hWnd, WMU_BREAKLOOP, bCheck, (LPARAM)pParent);
    }

	//用来过滤pControl的SendNotify，方便对消息进行监控处理
    void CWindowUI::SendNotify(MUINOTIFY &Notify)
	{
		TCHAR strText[MAX_PATH / 8];
		CControlUI * pControl = (CControlUI *)Notify.pSender;
		CControlUI * pTmpControl = NULL;

		MUITRACE(_T("CWindowUI::SendNotify CControlUI=%s , type=%d"), 
			(LPCTSTR)pControl->GetName(), Notify.dwType);

		switch (Notify.dwType)
		{
		case EnumNotify::NonMessage:
			{
				MUIASSERT(0 && "CWindowUI::HandleMessage = EnumNotifyMsg::NonMessage");
			}break;
		case EnumNotify::SelectItem:
			{
				if(pControl->GetName() == _T("restorebtn"))//还原按钮
				{
					//SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
					::ShowWindow(m_hWnd, SW_RESTORE);
					break;
				}
			}break;
		case EnumNotify::ClickItem:
			{
				if(pControl->GetName() == _T("closebtn"))
				{
					PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
					//DestroyWindow(m_hWnd);
					break;
				}
				else if(pControl->GetName() == _T("minbtn"))
				{
					SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
					::ShowWindow(m_hWnd, SW_MINIMIZE);
					break;
				}
				else if(pControl->GetName() == _T("maxbtn"))
				{
					//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
					::ShowWindow(m_hWnd, SW_MAXIMIZE);
					break;
				}
				else
				{
					// not thing
				}
			}break;
		case EnumNotify::CheckItem:
			{
				if(TRUE == Notify.wParam)//被选中
				{
					pControl->CallWndProc(WMU_GETGROUP, sizeof(strText),(LPARAM)strText);

					if(_T('\0') != strText[0])//存在分组
					{
						pTmpControl = (CControlUI *)m_GroupArray.Find(strText);
						if(pTmpControl && pTmpControl != pControl)
						{
							//判断窗口是为了避免控件移动到别的窗口了还产生消息
							if(pTmpControl->GetWindow() == m_hWnd)
							{
								pTmpControl->CallWndProc(WMU_SETCHECK, FALSE, NULL);
							}
						}
						m_GroupArray.Set(strText, pControl);
					}
				}
			}break;
		default:
			break;
		}

		//最后发出notify让用户接收
		this->OnNotify(Notify);
		return;
	}

	//MouseProc的核心是管理控件消息流！
	LRESULT CWindowUI::MouseProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT Point;

		RECT rcItem = {0};
		CControlUI * pTmpControl = NULL;
		CControlUI * pControl = NULL;

		if(nullptr == m_pViewInfo->pRootControl) return 0;
		Point.x = GET_X_LPARAM(lParam);
		Point.y = GET_Y_LPARAM(lParam);

		//第一找出这个鼠标消息应该传给哪个控件
		if(m_pViewInfo->pPushedControl)
		{
			//有pPushedControl表示鼠标处于捕捉状态，要将消息传给pPushedControl的控件
			//如果pPushedControl不是显示状态，那么point的位置是相对于窗口的位置
			//一般不会出现不显示的情况
			if(m_pViewInfo->pPushedControl->GetItemFixedRect(rcItem))
			{
				//如果pPushedControl是显示状态，那么point是相对于控件的位置
				Point.x -= rcItem.left;
				Point.y -= rcItem.top;
			}
			pControl = m_pViewInfo->pPushedControl;
		}
		else
		{
			pControl = m_pViewInfo->pRootControl->FindControlByPoint(Point);
		}

		//第二🙅‍，将消息进行转换，
		//主要转换出WM_MOUSELEAVE/WM_MOUSEENTER/WM_SETFOCUS/WM_KILLFOCUS四个消息
		switch (message)
		{
		case WM_MOUSELEAVE:
		{
			//原始的WM_MOUSELEAVE，是鼠标离开窗口发出的，所以处理后返回即可
			//有人问这个要不要通知一下pPushedControl，其实是不用的，因为
			//如果pHotControl等于pPushedControl，那么pPushedControl同样收到了通知
			//如果pHotControl不等于pPushedControl,那么pPushedControl在之前切换
			//热点的时候已经收到了WM_MOUSELEAVE通知，因为pHotControl不等于pPushedControl
			//的时候，之前一定发生过热点切换事件
			if (m_pViewInfo->pHotControl)
			{
				pTmpControl = m_pViewInfo->pHotControl;
				m_pViewInfo->pHotControl = NULL;

				return pTmpControl->CallWndProc(WM_MOUSELEAVE, 0, 0);
			}
		}break;//WM_MOUSELEAVE
		case WM_MOUSEMOVE:
		{
			if (pControl && pControl == m_pViewInfo->pPushedControl)
			{
				//此处有个一个bug，就是使用当pPushedControl超出窗口显示范围时，且鼠标已经移除窗口
				//FindControlByPoint函数仍有可能返回非空
				if (NULL == pControl->FindControlByPoint(Point))
				{
					if (STATE_HOT & pControl->GetState())
					{
						m_pViewInfo->pHotControl = NULL;
						pControl->CallWndProc(WM_MOUSELEAVE, 0, 0);
					}
				}
				else
				{
					//看这段代码的时候要注意，当存在pPushedControl时，
					//pHotControl要么等于pPushedControl， 要么为空
					if (NULL == (STATE_HOT & pControl->GetState()))
					{
						m_pViewInfo->pHotControl = pControl;
						pControl->CallWndProc(WMU_MOUSEENTER, wParam, MAKELONG(Point.x, Point.y));
					}
				}
			}
			else if (pControl != m_pViewInfo->pHotControl)
			{
				//当鼠标移动时，有可能离开一个控件，进入另一个控件，在此进行处理
				//此处仅进行WM_MOUSELEAVE 与 WM_MOUSEENTER的转换，意义在于切换热点

				//切换热点
				pTmpControl = m_pViewInfo->pHotControl;
				m_pViewInfo->pHotControl = pControl;
				//逻辑：先离开一个房间，才能进入另外一个房间
				//先发出离开通知，
				if (NULL != pTmpControl)
				{
					pTmpControl->CallWndProc(WM_MOUSELEAVE, 0, 0);
				}

				//再发出进入通知
				if (NULL != pControl)
				{
					pControl->CallWndProc(WMU_MOUSEENTER, wParam, MAKELONG(Point.x, Point.y));
				}
			}
		}break;//WM_MOUSEMOVE
		default:
		{
			if (ISMOUSEDOWN(message) && pControl && pControl != m_pViewInfo->pFocusControl)
			{
				//鼠标按下，给目标控件设置焦点
				if (true == pControl->IsControl())
				{
					::SendMessage(m_hWnd, WM_SETFOCUS, NULL, (LPARAM)pControl);
				}
				else
				{
					//pControl是一个布局，布局是不会获得焦点的，
					//if (m_pViewInfo->pFocusControl)
					//{
					//	pTmpControl = m_pViewInfo->pFocusControl;
					//	m_pViewInfo->pFocusControl = NULL;
					//	pTmpControl->CallWndProc(WM_KILLFOCUS, NULL, NULL);
					//}
				}
			}
		}break;//default
		}

		//第三步将原来的消息发送到对应的控件上
		if(pControl)
		{
			pControl->CallWndProc(message, wParam, MAKELONG(Point.x, Point.y));
		}

		//判断是否落在标题栏
		if (pControl && pControl == m_pViewInfo->pCaptionControl)//标题栏
		{
			if(WM_LBUTTONDOWN == message)
			{
				if(NULL == (STATE_PUSHED & pControl->GetState()))//没有SetCaptrue
				{
					pControl->SetCapture();
				}
			}
			else if (WM_LBUTTONUP == message)
			{
                if (m_pViewInfo->pPushedControl)
                {
                    m_pViewInfo->pPushedControl->ReleaseCapture();
                }
			}
			else if(WM_LBUTTONDBLCLK == message)
			{
				if(m_pViewInfo->pPushedControl)
				{
					m_pViewInfo->pPushedControl->ReleaseCapture();
				}
				
				if(::IsZoomed(m_hWnd))
				{
					::ShowWindow(m_hWnd, SW_RESTORE);
					//SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
				}
				else
				{
					::ShowWindow(m_hWnd, SW_MAXIMIZE);
					//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
				}
				MUITRACE(_T("CWindowUI::MouseProc WM_NCLBUTTONDBLCLK"));
			}
		}

		return 0;
	}
		
	LRESULT CWindowUI::KeyBoardProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		CControlUI * pControl = NULL;
		BYTE keyState[256] = {0};

		if(NULL == m_pViewInfo->pFocusControl)
		{
			goto end;
		}

 		switch (message)
		{
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		
		case WM_CHAR:
		case WM_SYSCHAR:
			{
				if(_T('\0') != m_pViewInfo->tSurplus && WM_CHAR == message && 
					m_pViewInfo->tSurplus == (TCHAR)wParam)
				{
					m_pViewInfo->tSurplus = _T('\0');
					MUITRACE(_T("WM_CHAR = %x"), wParam);
					return 0;
				}

				m_pViewInfo->pFocusControl->CallWndProc(message, wParam,lParam);

				//如果系统键按下，消息就不必流入DefWindowProc
				if(WM_KEYDOWN == message)
				{
					if(TRUE == GetKeyboardState(keyState))
					{
						if(keyState[VK_CONTROL] & 128 || keyState[VK_MENU] & 128
							|| keyState[VK_LWIN] & 128 || keyState[VK_RWIN] & 128)
						{
							m_pViewInfo->tSurplus = (TCHAR)wParam - (_T('A') - 1);
							MUITRACE(_T("GetKeyboardState = %x"), wParam);
							return 0;
						}
					}
				}
			}break;
		case WM_IME_KEYDOWN:
		case WM_IME_KEYUP:
		case WM_IME_CHAR:
			{
				//在中文输入法中，这个消息只能接收输入法发送过来的消息，
				//数字，英文字符是接收不到的，而WM_CHAR是WM_IME_CHAR的超集，
				//能够接收数字，英文等字符，所以处理WM_CHAR就够了
				//还有输入法的消息，是将WM_KEYDOWN等消息传递给DefWndProc处理后，
				//转换而来的，所以WM_KEYDOWN等消息不能直接返回，必须交给
				//DefWndProc处理，否则键盘输入后，后续接收不到输入法字符消息
			}break;
		default:
			break;
		}

		//if(MCS_DISABLEIME & m_pShareInfo->pFocusControl->GetStyle())
		//{
		//	//禁用输入法,只要WM_KEYDOWN不流入DefWindowProc，就不会产生输入法信息
		//	return 0;
		//}
end:

		return DefWindowProc(m_hWnd, message, wParam, lParam);
	}

	LRESULT CALLBACK CWindowUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		RECT * pRect= NULL, Rect = { 0 };
		MONITORINFO oMonitor = {};
		PAINTSTRUCT ps;
		POINT Point;
		CControlUI * pControl = NULL;
		MUIEVENT Event = { 0 };

		DWORD dwStype = NULL;
		int nWidth =0 , nHeight = 0;
		//先在这里从统一处理一下

		//TRACE(_T("CWindowUI::WndProc message = 0x%08x"), message);
		switch (message)
		{
		case WM_NCCREATE://第二条消息
		{
			m_pShareInfo->hWnd = m_hWnd;

			//请求一个渲染引擎
			Event.dwType = EnumEvent::RequestRenderEngine;
			m_pShareInfo->pRenderEngine = (CRenderEngine*)OnEvent(Event);
			if (NULL == m_pShareInfo->pRenderEngine)
			{
				//返回空，那么使用宿主引擎或者默认引擎
				if (m_pHost)
				{
					m_pShareInfo->pRenderEngine = CRenderEngine::Create(m_pHost->m_pShareInfo->pRenderEngine->GetName(), m_hWnd);
				}
				else
				{
					m_pShareInfo->pRenderEngine = new CGdiRenderEngine(m_hWnd);
				}
				
			}

			return TRUE;
		}break;
		case WM_NCDESTROY://能够收到的，最后一条消息，倒数第二条
		{
			Event.dwType = EnumEvent::OnFinal;//一般在里面执行delete this
			OnEvent(Event);

			SetSkin(NULL);
			if (m_pShareInfo->pRenderEngine)
			{
				delete m_pShareInfo->pRenderEngine;
				m_pShareInfo->pRenderEngine = NULL;
			}

			if (m_pViewInfo->pRootControl)
			{
				delete m_pViewInfo->pRootControl;
				m_pViewInfo->pRootControl = NULL;
			}
		}break;
		case WM_GETMINMAXINFO://窗口大小改变前
			{
				if (m_pViewInfo->SizeMin.cx > 0) 
					((MINMAXINFO*)lParam)->ptMinTrackSize.x = m_pViewInfo->SizeMin.cx;
				if (m_pViewInfo->SizeMin.cy > 0) 
					((MINMAXINFO*)lParam)->ptMinTrackSize.y = m_pViewInfo->SizeMin.cy;
				if (m_pViewInfo->SizeMax.cx > 0) 
					((MINMAXINFO*)lParam)->ptMaxTrackSize.x = m_pViewInfo->SizeMax.cx;
				if (m_pViewInfo->SizeMax.cy > 0) 
					((MINMAXINFO*)lParam)->ptMaxTrackSize.y = m_pViewInfo->SizeMax.cy;
				//((MINMAXINFO*)lParam)->ptMaxSize = m_SizeMax;
				return 0;
			}break;
		case WM_TIMER:
			{
				return this->OnEvent(Event);
			}break;
		//捕捉下面三条消息隐藏标题栏
		case WM_NCCALCSIZE://屏蔽标题栏
			{
				if ( TRUE == wParam )
				{
					LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
					pRect=&pParam->rgrc[0];
				}
				else
				{
					pRect=(LPRECT)lParam;
				}

				if (TRUE == ::IsZoomed(m_hWnd))
				{	
					// 最大化时，计算当前显示器最适合宽高度
					MUITRACE(_T("CWindowUI WM_NCCALCSIZE"));
					
					oMonitor.cbSize = sizeof(oMonitor);
					::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);

					*pRect = oMonitor.rcWork;
					MUITRACE(_T("WM_NCCALCSIZE left=%d, top=%d, right=%d, bottom=%d"),
						pRect->left, pRect->top, pRect->right, pRect->bottom);
					OffsetRect(pRect, -pRect->left, -pRect->top);
					return WVR_REDRAW;
				}

				if (FALSE == ::IsIconic(m_hWnd))
				{
					//窗口不是最小化
					//return (0 == wParam) ? TRUE : FALSE;
					return !wParam;
				}
			}break;
		case WM_NCACTIVATE://屏蔽标题栏
			{
				//当WM_NCACTIVATE消息的wParam参数为true 时表示窗体被激活，
				//为false 时表示窗体改为未被激活的状态，这个时候。消息的返回值。
				//为true 表示允许激活其它的窗体，为false 表示不允许激活其他的窗体
				//TRACE((wParam == 0) ? _T("WM_NCACTIVATE = TRUE") : _T("WM_NCACTIVATE = FALSE"));
				return (wParam == 0) ? TRUE : FALSE;
			}break;
		case WM_NCPAINT://屏蔽非客户区绘制
			{
				//屏蔽窗口的客户区（标题栏和边框），让整个窗口变成客户区
				//dwStype = (DWORD) GetWindowLong(m_hWnd, GWL_STYLE);

				//窗口不会被拖动大小，让消息流入DefWndProc，这样不会产生圆角
				//窗口允许改变大小时，就返回给系统，但是这样会产生圆角。
				//if(WS_SIZEBOX == (dwStype & WS_SIZEBOX) ||
				//	(m_SizeMin.cx != m_SizeMax.cx || m_SizeMin.cy != m_SizeMax.cy))
				//{
					  //return 0;
				//}
			}break;
		case WM_NCHITTEST: 
			{
				//鼠标在非客户区移动
				//没有边框了，我们不能拖动窗口，额外处理一下
                Point.x = GET_X_LPARAM(lParam);
                Point.y = GET_Y_LPARAM(lParam);
                ::ScreenToClient(m_hWnd, &Point);
				//RECT rcClient;
				::GetClientRect(m_hWnd, &Rect);

				//不是最大化
				dwStype = (DWORD) GetWindowLong(m_hWnd, GWL_STYLE);
				if (WS_SIZEBOX == (dwStype & WS_SIZEBOX) && !::IsZoomed(m_hWnd))
				{
					RECT rcSizeBox = { 4,4,4,4 };
                    if (Point.y < Rect.top + rcSizeBox.top)
					{
                        if (Point.x < Rect.left + rcSizeBox.left) return HTTOPLEFT;
                        if (Point.x > Rect.right - rcSizeBox.right) return HTTOPRIGHT;
						return HTTOP;
					}
                    else if (Point.y > Rect.bottom - rcSizeBox.bottom)
					{
                        if (Point.x < Rect.left + rcSizeBox.left) return HTBOTTOMLEFT;
                        if (Point.x > Rect.right - rcSizeBox.right) return HTBOTTOMRIGHT;
						return HTBOTTOM;
					}

                    if (Point.x < Rect.left + rcSizeBox.left) return HTLEFT;
                    if (Point.x > Rect.right - rcSizeBox.right) return HTRIGHT;
				}

				return HTCLIENT;
			}break;
		case WM_NCLBUTTONDBLCLK:
			{
				//其实并不会收到这条消息，WM_NCHITTEST并不返回HTCAPTION(标题栏),所以大部分的WM_NCBUTTON消息并不会发生
				//这个时候我们就需要将一些客户区消息，转换成非客户区消息，比如MW_LBUTTONDBLCLK转成WM_NCLBUTTONDBLCLK
				//或许你觉得很麻烦，但其实，如果WM_NCHITTEST返回HTCAPTION的话，这样更麻烦，因为不返回HTCAPTION，我们
				//只需要将少部分的WM_BUTTON转成WM_NCBUTTON，如果返回HTCAPTION，那么几乎所有的WM_NCBUTTON消息都要转换
				//成WM_BUTTON
				MUITRACE(_T("WM_NCLBUTTONDBLCLK"));
				if(::IsZoomed(m_hWnd))
				{
					::ShowWindow(m_hWnd, SW_RESTORE);
				}
				else
				{
					::ShowWindow(m_hWnd, SW_MAXIMIZE);
					//SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
				}
				return 0;
			}break;
		//正常消息，CreateWindow完成后收到
		case WM_CANCELMODE:
			{
				//SetCaptrue的时候会发出这个消息，wParam和lParam都不使用
				if(1 == wParam)//wParam和lParam都不使用,那我们就自己定义一下，据为己用
				{
					m_pViewInfo->pPushedControl = reinterpret_cast<CControlUI*>(lParam);
				}
			}break;
		case WM_CREATE://窗口创建成功
			{
				m_pShareInfo->pToolTip = new CToolTipUI();
				m_pShareInfo->pToolTip->Create(m_hInstance);

				m_pShareInfo->hPaintDC = ::GetDC(m_hWnd);

				//通知初始化
				Event.dwType = EnumEvent::WindowInit;
				Event.lParam = lParam;
				this->OnEvent(Event);

				Event.dwType = EnumEvent::WindowReady;
				Event.wParam = wParam;
				Event.lParam = lParam;

				return this->OnEvent(Event);
			}break;
		case WM_CLOSE://窗口准备关闭
			{
				if(m_pViewInfo->pRootControl)
				{
					m_pViewInfo->pRootControl->SetShareInfo(NULL);
				}
				m_pViewInfo->pFocusControl = NULL;
				m_pViewInfo->pHotControl = NULL;
				m_pViewInfo->pPushedControl = NULL;
			}break;
		case WM_DESTROY://其他窗口已经关闭
			{
				//窗口结束通知
				Event.dwType = EnumEvent::WindowDestroy;
				OnEvent(Event);

				if(m_pShareInfo->hPaintDC)
				{
					::ReleaseDC(m_hWnd, m_pShareInfo->hPaintDC);
				}

			}break;
		case WM_SHOWWINDOW:
			{
				if(m_pViewInfo->pCaptionControl)
				{
					//把还原按钮找出来，并设置它的状态，因为还原按钮是随着窗口是否最大化
					//而发生改变的
					pControl = m_pViewInfo->pCaptionControl->FindControlByName(_T("restorebtn"));

					if(pControl)
					{
						if(TRUE == ::IsZoomed(m_hWnd))
						{
							pControl->AddState(STATE_SELECT);
						}
						else
						{
							pControl->RemoveState(STATE_SELECT);
						}
						pControl->Invalidate();
					}
				}

				Event.dwType = EnumEvent::WindowShow;
				Event.wParam = wParam;
				Event.lParam = lParam;
				return this->OnEvent(Event);
			}break;
		case WM_GETFONT:
			{
				return (LRESULT)m_pShareInfo->FontArray.Select(m_nFontId);
			}break;
		case WM_SIZE:
			{
				//m_bRepaint = TRUE;
				Rect.left = 0;
				Rect.top = 0;
				Rect.right = LOWORD(lParam);
				Rect.bottom = HIWORD(lParam);

				if(NULL == m_pViewInfo->pRootControl) break;

				//注意，始祖控件的外边距没有生效
				m_pViewInfo->pRootControl->SetItem(Rect, true);

				//设置窗口
				SIZE szRound = m_pViewInfo->pRootControl->GetBorderRound();
				if((szRound.cx || szRound.cy) && !::IsIconic(m_hWnd))
				{
					Rect.right += 1;
					Rect.bottom += 1;
					HRGN hRgn = ::CreateRoundRectRgn(Rect.left, Rect.top, Rect.right, Rect.bottom,
						szRound.cx, szRound.cy);
					::SetWindowRgn(m_hWnd, hRgn, TRUE);
					::DeleteObject(hRgn);
				}

				::InvalidateRect(m_hWnd, NULL, FALSE);
			}break;
		case WM_DISPLAYCHANGE://屏幕分辨率改变
			{
				nWidth = LOWORD(lParam);
				nHeight = HIWORD(lParam);
			}break;
			//鼠标消息
		case WM_MOUSEACTIVATE:
			{
				if(m_pViewInfo && m_pViewInfo->pFocusControl)
				{
					m_pViewInfo->pFocusControl->CallWndProc(message, wParam, lParam);
				}
			}break;
		case WM_MOUSEMOVE:
			{
				TRACKMOUSEEVENT tme;
				if(m_pViewInfo && m_pViewInfo->pCaptionControl)
				{
					if(m_pViewInfo->pCaptionControl == m_pViewInfo->pPushedControl)
					{
						//标题栏处于按下状态，拖动窗口
						//TRACE(_T("WM_MOUSEMOVE Drop Caption"));
                        GetCursorPos(&Point);
						::GetWindowRect(m_hWnd, &Rect);
                        ScreenToClient(m_hWnd, &Point);
						//TRACE(_T("WM_MOUSEMOVE rect  left=%d, top =%d"), Rect.left , Rect.top);
						//TRACE(_T("WM_MOUSEMOVE point point.x=%d, point.y=%d"), Point.x , Point.y);
						//TRACE(_T("WM_MOUSEMOVE Mouse pm.x=%d, pm.y=%d"), m_pViewInfo->ptLeftMouse.x , m_pViewInfo->ptLeftMouse.y);
						//TRACE(_T("WM_MOUSEMOVE diff  x  =%d,  y   =%d"), Rect.left + Point.x - m_pViewInfo->ptLeftMouse.x ,
							//Rect.top + Point.y - m_pViewInfo->ptLeftMouse.y);

                        ::SetWindowPos(m_hWnd, 0, Rect.left + Point.x - m_pViewInfo->ptLeftMouse.x,
                            Rect.top + Point.y - m_pViewInfo->ptLeftMouse.y,
							0 , 0, SWP_NOSIZE);
						return 0;
					}
				}

				//添加鼠标盘旋通知
				
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE|TME_HOVER;
				tme.dwHoverTime = m_dwHoverTime;
				tme.hwndTrack = m_hWnd;
				::TrackMouseEvent(&tme);

				//m_pViewInfo->ptLeftMouse.x = GET_X_LPARAM(lParam);
				//m_pViewInfo->ptLeftMouse.y = GET_Y_LPARAM(lParam);
				//TRACE(_T("CWindowUI::WM_MOUSEMOVE"));
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_LBUTTONDBLCLK://左双击
			{
				//弥补一条鼠标按下消息，使消息配对，所以凡是双击消息都要注意了，MYUI的消息跟Windows有区别
				//Windows双击：WM_LBUTTONDOWN->WM_LBUTTONUP->WM_LBUTTONDBLCLK->WM_LBUTTONUP
				//MYUI双击：   WM_LBUTTONDOWN->WM_LBUTTONUP->WM_LBUTTONDOWN->WM_LBUTTONDBLCLK->WM_LBUTTONUP
				//WndProc(WM_LBUTTONDOWN, wParam, lParam);
				MUITRACE(_T("WM_LBUTTONDBLCLK"));
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_LBUTTONDOWN://左按下
			{
				MUITRACE(_T("WM_LBUTTONDOWN"));
				m_pViewInfo->ptLeftMouse.x = GET_X_LPARAM(lParam);
				m_pViewInfo->ptLeftMouse.y = GET_Y_LPARAM(lParam);
				
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_LBUTTONUP://左弹起
			{
				MUITRACE(_T("WM_LBUTTONUP"));
				m_pViewInfo->ptLeftMouse.x = GET_X_LPARAM(lParam);
				m_pViewInfo->ptLeftMouse.y = GET_Y_LPARAM(lParam);

				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MBUTTONDBLCLK://中双击
			{
				MUITRACE(_T("WM_MBUTTONDBLCLK"));
				//WndProc(WM_MBUTTONDOWN, wParam, lParam);
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MBUTTONDOWN://中按下
			{
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MBUTTONUP://中弹起
			{
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_RBUTTONDBLCLK://右双击
			{
				MUITRACE(_T("WM_RBUTTONDBLCLK"));
				//WndProc(WM_RBUTTONDOWN, wParam, lParam);
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_RBUTTONDOWN://右按下
			{
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_RBUTTONUP://右弹起
			{
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MOUSEWHEEL:
			{
				//WM_MOUSEWHEEL的鼠标位置是屏幕位置
                Point.x = LOWORD(lParam);
                Point.y = HIWORD(lParam);
                ScreenToClient(m_hWnd, &Point);//转换一下
                lParam = MAKELONG(Point.x, Point.y);
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MOUSEHOVER:
			{
				//TRACE(_T("WM_MOUSEHOVER"));
				MouseProc(message, wParam, lParam);
			}break;
		case WM_MOUSELEAVE:
			{
				//TRACE(_T("WM_MOUSELEAVE"));
				MouseProc(message, wParam, lParam);
			}break;
			//绘制消息
		case WM_SETCURSOR:
			{
				if(m_pViewInfo && m_pViewInfo->pHotControl)
				{
					return m_pViewInfo->pHotControl->CallWndProc(message, wParam, lParam);
				}
			}break;
		case WM_MOVE:
			{
				RECT rcShow = {0};
				BOOL bShowInScreen = m_bShowInScreen;

				::GetWindowRect(m_hWnd, &Rect);
				oMonitor.cbSize = sizeof(oMonitor);
				::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);

				if(FALSE == ::IntersectRect(&rcShow, &Rect, &oMonitor.rcWork))
				{
					m_bShowInScreen = FALSE;
					return 0;
				}

				if(TRUE == IsSameRect(rcShow, Rect))
				{
					m_bShowInScreen = TRUE;
				}
				else
				{
					m_bShowInScreen = FALSE;
				}

				if(TRUE == bShowInScreen && TRUE == m_bShowInScreen) return 0;

				::InvalidateRect(m_hWnd, &rcShow, FALSE);
				return 0;
			}break;
		case WM_PAINT://前景绘制
			{
				//判断是否需要进行绘制
				if (!m_pViewInfo->pRootControl || !m_pShareInfo->pRenderEngine) break;

				//需要绘制，下面正式开始
				BeginPaint(m_hWnd, &ps);
				if(TRUE == IsValidRect(ps.rcPaint))
				{
					::GetClientRect(m_hWnd, &Rect);
					::OffsetRect(&Rect, -Rect.left, -Rect.top);
					m_pShareInfo->pRenderEngine->BeginPaint(Rect);
					m_pViewInfo->pRootControl->OnPaint(ps.rcPaint);
					m_pShareInfo->pRenderEngine->EndPaint(ps.rcPaint);
				}

				EndPaint(m_hWnd, &ps);

				return 0;
			}break;
		case WM_PRINTCLIENT:
			{
				RECT rcClient = {0};
				::GetClientRect(m_hWnd, &rcClient);
				HDC hdc = (HDC)wParam;
				int nSave = ::SaveDC(hdc);
				
				MUIASSERT(0 && "这个事件一直没触发过，所以在这里设置一个错误，作为提示");
				if((PRF_CHILDREN & lParam))
				{
					HWND hChildWnd = ::GetWindow(m_hWnd, GW_CHILD);
					while(hChildWnd)
					{
						RECT rcPos = {0};
						::GetWindowRect(hChildWnd, &rcPos);
                        ::MapWindowPoints(m_hWnd, HWND_DESKTOP, reinterpret_cast<LPPOINT>(&rcPos), 2);
						::SetWindowOrgEx(hdc, -rcPos.left, -rcPos.top, NULL);

						::SendMessage(hChildWnd, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
						hChildWnd = ::GetWindow(hChildWnd, GW_HWNDNEXT);
					}
				}
				::RestoreDC(hdc, nSave);

			}break;
		case WM_ERASEBKGND://背景绘制
			{
				(HDC)wParam;
				return TRUE;//告诉系统已经绘制过了，不必再绘制
			}break;
		case WM_GETDLGCODE:
			{
				if(lParam)
				{
					LPMSG lpmsg = (LPMSG)lParam;
					if( lpmsg->message == WM_CHAR)
					{
							return DLGC_WANTCHARS;
					}
				}
			   return 0;
			}break;
        case WMU_POPUPMENU:
        {
            if (wParam)
            {
                Point.x = GET_X_LPARAM(lParam);
                Point.y = GET_Y_LPARAM(lParam);
                reinterpret_cast<IMenuPopup*>(wParam)->Popup(static_cast<INotify*>(this), Point);
            }
            return !!wParam;
        }break; 
        case WMU_POPUPDIALOG:
        {
            //if (wParam)
            //{
            //    reinterpret_cast<IDialogPopup*>(wParam)->Popup(lParam);
            //}
            return !!wParam;
        }break;
		case WM_REQUESTINFO:
			{
				switch (wParam)
				{
				case MRQF_SHAREINFO:
					{
						return (LRESULT) m_pShareInfo;
					}break;
				case MRQF_VIEWINFO:
					{
						return (LRESULT) m_pViewInfo;
					}break;
				case MRQF_GETFOCUS:
					{
						return (LRESULT) m_pViewInfo->pFocusControl;
					}break;
				default:
					{
						MUIASSERT(0 && "无效的WM_REQUESTINFO参数");
					}break;
				}
			}break;
		//焦点
		case WM_SETFOCUS:
			{
				MUITRACE(_T("CWindowUI %s: WM_SETFOCUS"), m_strClassName);

				if(NULL == lParam)
				{
					m_pViewInfo->bFocusWnd = TRUE;

					Event.dwType = EnumEvent::SetFocued;
					Event.wParam = wParam;
					Event.lParam = lParam;
					return this->OnEvent(Event);
				}
				else
				{
					if(FALSE == m_pViewInfo->bFocusWnd) ::SetFocus(m_hWnd);

					if(m_pViewInfo->pFocusControl != reinterpret_cast<CControlUI *>(lParam))
					{
						pControl = m_pViewInfo->pFocusControl;
						m_pViewInfo->pFocusControl = reinterpret_cast<CControlUI *>(lParam);
						if(pControl)
						{
							pControl->CallWndProc(WM_KILLFOCUS, 0, 0);
						}
					}

					pControl = m_pViewInfo->pFocusControl;
					pControl->CallWndProc(message, 0, 0);
					return 0;
				}
			}break;
		case WM_KILLFOCUS:
			{
				MUITRACE(_T("CWindowUI %s: WM_KILLFOCUS"), m_strClassName);
				//撤掉焦点，并且通知失去焦点
				m_pViewInfo->bFocusWnd = FALSE;
				pControl = m_pViewInfo->pFocusControl;
				m_pViewInfo->pFocusControl = NULL;

				if(pControl)
				{
					pControl->CallWndProc(message, wParam, lParam);
				}

				Event.dwType = EnumEvent::KillFocued;
				Event.wParam = wParam;
				Event.lParam = lParam;
				return this->OnEvent(Event);
			}break;
		//系统自定义的消息
		default:
			{
				MUIASSERT(m_pShareInfo);
				//键盘消息
				if((message >= WM_KEYFIRST /*&& WM_KEYLAST >= message */ && WM_IME_KEYLAST >= message)
					|| (message >= WM_IME_SETCONTEXT && WM_IME_KEYUP >= message))
				{
					return KeyBoardProc(message, wParam, lParam);
				}
			}break;
		}

		return DefWindowProc(m_hWnd, message, wParam, lParam);
	}

}