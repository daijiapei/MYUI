
#include "UIWindow.h"
#include "UserHandle.h"
#include "../Render/GdiEngine.h"
#include "../Control/UIOption.h"
#include "../Layout/UIVerticalLayout.h"
#include "../ExtControl/UIToolTip.h"

namespace MYUI
{
	CWindowUI::CWindowUI() 
		: m_hWnd(NULL)
		, m_hInstance(NULL)
		, m_nFontId(-1)
		, m_dwHoverTime(800)
		, m_bShowInScreen(FALSE)
	{
		m_pShareInfo = new TSHAREINFO;
		m_pViewInfo = new VIEWINFO;
		memset(m_pShareInfo, 0, sizeof(TSHAREINFO));
		memset(m_pViewInfo, 0, sizeof(VIEWINFO));

		m_pShareInfo->bHostType = TRUE;
		m_pShareInfo->FontArray = new CMuiIdArray(10);
		m_pShareInfo->strSkinFolder = new TCHAR[MAX_PATH];
		memset(m_pShareInfo->strSkinFolder, 0, sizeof(TCHAR) * MAX_PATH);

		m_pViewInfo->SizeMax.cx = m_pViewInfo->SizeMax.cy = 99999;
		m_pViewInfo->ptLeftMouse.x = m_pViewInfo->ptLeftMouse.y = 0;
	}

	CWindowUI::~CWindowUI()
	{
		delete m_pViewInfo;

		if(TRUE == m_pShareInfo->bHostType)
		{
			//宿主资源模式，负责申请和释放资源
			if(m_pShareInfo->FontArray)
			{
				for(int i = 0; m_pShareInfo->FontArray->GetSize() > i; i++)
				{
					DeleteObject((HFONT)m_pShareInfo->FontArray->GetAt(i));
				}
				delete m_pShareInfo->FontArray;
			}
			
			if(m_pShareInfo->strSkinFolder)
			{
				delete m_pShareInfo->strSkinFolder;
			}
		}
		
		delete m_pShareInfo;

        for (int i = 0; m_MenuArray.GetSize() > i; i++)
        {
            IMenuPopup * pMenu = reinterpret_cast<IMenuPopup*>(m_MenuArray.GetAt(i));
            delete pMenu;
        }
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

	CWindowUI::operator HWND() const
	{
		return m_hWnd;
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

	LRESULT CALLBACK CWindowUI::FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		CWindowUI * pWindow = NULL;
		TEVENT event = {0};
		switch (message)
		{
		case WM_NCCREATE://第二条消息
			{
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				pWindow = static_cast<CWindowUI*>(lpcs->lpCreateParams);
				pWindow->m_hWnd = hwnd;
				pWindow->m_pShareInfo->hWnd = hwnd;
				::SetWindowLongPtr(hwnd, 0, reinterpret_cast<LPARAM>(pWindow));
				
				//请求一个渲染引擎
				event.dwType = EnumEventType::RequestRenderEngine;
				pWindow->m_pShareInfo->pRenderEngine = (CRenderEngine*)pWindow->OnEvent(event);
				if(NULL == pWindow->m_pShareInfo->pRenderEngine)
				{
					//返回空，那么使用默认引擎
					pWindow->m_pShareInfo->pRenderEngine = new CGdiRenderEngine(pWindow->m_hWnd);
				}

                if (pWindow->m_pShareInfo->strSkinFolder)
                {
                    pWindow->SetSkin(pWindow->m_pShareInfo->strSkinFolder);
                }

				return TRUE;
			}break;
		case WM_NCDESTROY://能够收到的，最后一条消息，倒数第二条
			{
				pWindow = reinterpret_cast<CWindowUI*>(::GetWindowLongPtr(hwnd, 0));
				if(NULL != pWindow) 
				{
					::SetWindowLongPtr(pWindow->m_hWnd, 0, 0L);
					//窗口结束通知
					event.dwType = EnumEventType::WindowDestroy;
					pWindow->OnEvent(event);

					if(pWindow->m_pShareInfo->pRenderEngine)
					{
						delete pWindow->m_pShareInfo->pRenderEngine;
						pWindow->m_pShareInfo->pRenderEngine = NULL;
					}

					if(pWindow->m_pViewInfo->pRootControl)
					{
						delete pWindow->m_pViewInfo->pRootControl;
						pWindow->m_pViewInfo->pRootControl = NULL;
					}

					event.dwType = EnumEventType::OnFinal;//一般在里面执行delete this
					pWindow->OnEvent(event);
				}
			}break;
		case WM_GETMINMAXINFO:
			{
				//第一条消息，我们不管
				//注意，后续的消息中，如果改变窗口大小，还是会收到这条消息的
				//交给default处理
				pWindow = NULL;
			}
		case WM_QUIT:
			{
				//最后一条消息，往往是收不到的，交给default处理
			}
		default:
			{
				pWindow = reinterpret_cast<CWindowUI*>(::GetWindowLongPtr(hwnd, 0));
				if(pWindow)
				{
					return pWindow->WndProc(message, wParam, lParam);
				}
			}break;
		}
		return DefWindowProc (hwnd, message, wParam, lParam);
	}

	HWND CWindowUI::Create(HINSTANCE hInstance, HWND hWndParent, DWORD dwStyle, 
		LPCTSTR strClassName ,LPCTSTR strWindowText, RECT * pRect)
	{
		m_hInstance = hInstance;
		m_strClassName = strClassName;
		m_strWindowText = strWindowText;
		RECT rect = {0,0,0,0};
		if(pRect) rect = *pRect;

		if(false == RegisterClass()) return NULL;

		m_hWnd = ::CreateWindow(strClassName,strWindowText,
		//WS_CLIPSIBLINGS | WS_CLIPCHILDREN 其他窗口不能激活
		//WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		/*WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME  |  WS_MINIMIZEBOX ,*/
		//WS_OVERLAPPEDWINDOW,
		dwStyle | (hWndParent ? NULL : WS_CLIPCHILDREN),
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		hWndParent,NULL, m_hInstance,this);
		ASSERT(m_hWnd!=NULL);
		return m_hWnd;
	}

	BOOL CWindowUI::Destroy()
	{
		ASSERT(::IsWindow(m_hWnd));
		return ::DestroyWindow(m_hWnd);
	}

	BOOL CWindowUI::Close(LONG nRet)
	{
		ASSERT(::IsWindow(m_hWnd));
		if( !::IsWindow(m_hWnd) ) return FALSE;
		::PostMessage(m_hWnd, WM_CLOSE, nRet, 0L);
		return TRUE;
	}

	void CWindowUI::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
	{
		ASSERT(::IsWindow(m_hWnd));
		if( !::IsWindow(m_hWnd) ) return;
		::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
	}

    LRESULT CWindowUI::ShowModal(bool bShow /*= true*/, bool bEnableParent /*= true*/, UINT uBreakMessage /*= -1*/)
	{
		ASSERT(::IsWindow(m_hWnd));
        LRESULT lResult = 0;
		HWND hOwnerWnd = GetWindowOwner(m_hWnd);
        MSG Msg = { 0 };
        IUserHandle * pHandle = CUserHandleTable::GetThreadHandle(NULL);

        ::ShowWindow(m_hWnd, bShow ? TRUE : FALSE);
        if (bEnableParent && hOwnerWnd)
        {
            ::EnableWindow(hOwnerWnd, FALSE);
        }

        while (::IsWindow(m_hWnd) && ::GetMessage(&Msg, NULL, 0, 0))
        {
            //::SleepEx(0 , TRUE);
            if (NULL == Msg.hwnd)
            {
                if (pHandle)
                {
                    if (Msg.message >= WM_USER && WM_USER + 0x7FFF >= Msg.message)
                    {
                        pHandle->Callback(Msg.message - WM_USER, Msg.wParam, Msg.lParam);
                        continue;
                    }
                }
            }
#ifdef ENABLE_TIMER_LPARAM
            else
            {

                //WM_TIMER中lParam是回调函数，如果不为空将会在DispatchMessage 中调用该函数，
                //而用户的WndProc将收不到WM_TIMER通知。但是有些用户更希望lParam作为一个参数
                //而不是函数，所以我们提前将WM_TIMER消息发送到用户窗口并返回，这样就能实现
                //lparam 作为定时器参数的目的
                if (WM_TIMER == Msg.message)
                {
                    ::SendMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
                    continue;
                }
            }
#endif

            if (WM_CLOSE == Msg.message)
            {
                lResult = Msg.wParam;
			}

            ::TranslateMessage(&Msg);
            ::DispatchMessage(&Msg);

            if (uBreakMessage == Msg.message && m_hWnd == Msg.hwnd)
            {
                break;
            }
		}

        if (bEnableParent && hOwnerWnd)
        {
            ::EnableWindow(hOwnerWnd, TRUE);
            ::SetFocus(hOwnerWnd);
        }

        if (WM_QUIT == Msg.message)
        {
            lResult = Msg.wParam;
        }

        return lResult;
	}

    LRESULT CWindowUI::Popup(RECT rcPos)
    {
        ASSERT(::IsWindow(m_hWnd));
        LRESULT lResult = 0;
        MSG Msg = { 0 };
        IUserHandle * pHandle = CUserHandleTable::GetThreadHandle(NULL);
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
                    case WM_POPUPDIALOG:
                    {
                        if (m_hWnd != Msg.hwnd) goto BREAKLOOP;
                    }break;
                    case WM_POPUPMENU:
                    {

                        if (this->GetInterface(_T("IMenuPopup")) == (LPVOID)Msg.wParam)
                        {
                            //同一个菜单, 不需要重复弹出
                            PeekMessage(&Msg, 0, Msg.message, Msg.message, PM_REMOVE);
                            continue;
                        }

                        if (m_hWnd != Msg.hwnd) goto BREAKLOOP;
                    }break;
                    case WM_BREAKLOOP:
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

                if (NULL == Msg.hwnd)
                {
                    if (pHandle)
                    {
                        if (Msg.message >= WM_USER && WM_USER + 0x7FFF >= Msg.message)
                        {
                            pHandle->Callback(Msg.message - WM_USER, Msg.wParam, Msg.lParam);
                            continue;
                        }
                    }
                }
#ifdef ENABLE_TIMER_LPARAM
                else
                {
                    if (WM_TIMER == Msg.message)
                    {
                        ::SendMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
                        continue;
                    }
                }
#endif

                ::TranslateMessage(&Msg);
                ::DispatchMessage(&Msg);
            }
            else
            {
                WaitMessage();
            }
        }

    BREAKLOOP:
        if (::IsWindow(m_hWnd) && ::IsWindowVisible(m_hWnd)) ::ShowWindow(m_hWnd, FALSE);

        return Msg.message;
    }

	bool CWindowUI::RegisterClass()
	{
		WNDCLASS wndclass = {0};
		//wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		wndclass.style = CS_DBLCLKS;
		wndclass.lpfnWndProc = CWindowUI::FrameWndProc;
		wndclass.cbClsExtra = sizeof(void *) ;
		wndclass.cbWndExtra = sizeof(void *);
		wndclass.hInstance = m_hInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
		wndclass.hbrBackground = CreateSolidBrush(NULL_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = m_strClassName;

		ATOM ret = ::RegisterClass(&wndclass);
		ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
		return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
	}

	HWND CWindowUI::GetHandle()
	{
		return m_hWnd;
	}

	void CWindowUI::CenterWindow()
	{
		ASSERT(::IsWindow(m_hWnd));
		ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD)==0);
		RECT rcDlg = { 0 };
		::GetWindowRect(m_hWnd, &rcDlg);
		RECT rcArea = { 0 };
		RECT rcCenter = { 0 };
		HWND hWnd= this->m_hWnd;
		HWND hWndParent = ::GetParent(m_hWnd);
		HWND hWndCenter = ::GetWindowOwner(m_hWnd);
		if (hWndCenter!=NULL)
			hWnd=hWndCenter;

		// 处理多显示器模式下屏幕居中
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
		rcArea = oMonitor.rcWork;

		if( hWndCenter == NULL )
			rcCenter = rcArea;
		else
			::GetWindowRect(hWndCenter, &rcCenter);

		int DlgWidth = rcDlg.right - rcDlg.left;
		int DlgHeight = rcDlg.bottom - rcDlg.top;

		// Find dialog's upper left based on rcCenter
		int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
		int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

		// The dialog is outside the screen, move it inside
		if( xLeft < rcArea.left ) xLeft = rcArea.left;
		else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
		if( yTop < rcArea.top ) yTop = rcArea.top;
		else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
		::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void CWindowUI::SetIcon(UINT nRes)
	{
		HICON hIcon = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		ASSERT(hIcon);
		::SendMessage(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
		hIcon = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		ASSERT(hIcon);
		::SendMessage(m_hWnd, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
	}

	LRESULT CWindowUI::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
	{
		ASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
	} 

	LRESULT CWindowUI::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
	{
		ASSERT(::IsWindow(m_hWnd));
		return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
	}

	CMuiString CWindowUI::GetClassName()
	{
		return m_strClassName;
	}

	CMuiString CWindowUI::GetWindowText() 
	{
		return m_strWindowText;
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

    bool CWindowUI::CloneResource(TSHAREINFO * pShareInfo)
    {
        if (m_pShareInfo->bHostType)
        {
            //宿主资源模式，负责申请和释放资源
            if (m_pShareInfo->FontArray)
            {
                for (int i = 0; m_pShareInfo->FontArray->GetSize() > i; i++)
                {
                    DeleteObject((HFONT)m_pShareInfo->FontArray->GetAt(i));
                }
                delete m_pShareInfo->FontArray;
            }

            if (m_pShareInfo->strSkinFolder)
            {
                delete m_pShareInfo->strSkinFolder;
            }

            m_pShareInfo->bHostType = FALSE;
            m_pShareInfo->FontArray = NULL;
            m_pShareInfo->strSkinFolder = NULL;
        }

        if (pShareInfo)
        {
            m_pShareInfo->FontArray = pShareInfo->FontArray;
            m_pShareInfo->strSkinFolder = pShareInfo->strSkinFolder;
            this->SetSkin(m_pShareInfo->strSkinFolder);
            return true;
        }
        return false;
    }

    bool CWindowUI::SetSyncResource(CWindowUI *pHostWindow)
    {
        TSHAREINFO * pShareInfo = NULL;

        if (m_pShareInfo->bHostType)
        {
            //宿主资源模式，负责申请和释放资源
            if (m_pShareInfo->FontArray)
            {
                for (int i = 0; m_pShareInfo->FontArray->GetSize() > i; i++)
                {
                    DeleteObject((HFONT)m_pShareInfo->FontArray->GetAt(i));
                }
                delete m_pShareInfo->FontArray;
            }

            if (m_pShareInfo->strSkinFolder)
            {
                delete m_pShareInfo->strSkinFolder;
            }

            m_pShareInfo->bHostType = FALSE;
            m_pShareInfo->FontArray = NULL;
            m_pShareInfo->strSkinFolder = NULL;
        }

        pShareInfo = pHostWindow->m_pShareInfo;

#ifdef _DEBUG
        if (!pShareInfo || !pShareInfo->strSkinFolder || !pShareInfo->FontArray)
        {
            ASSERT(0 && "没有在合适的实际调用CWindowUI::SetSyncResource，所以不能获取有效的资源参数");
        }
#endif
        if (pShareInfo)
        {
            m_pShareInfo->FontArray = pShareInfo->FontArray;
            m_pShareInfo->strSkinFolder = pShareInfo->strSkinFolder;
            this->SetSkin(m_pShareInfo->strSkinFolder);
            return true;
        }
        return false;
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

            if (strSkin)
            {
                _tcscpy(m_pShareInfo->strSkinFolder, strSkin);
            }
        }
        return bRet;
	}

	LPCTSTR CWindowUI::GetSkin() const
	{
		return m_pShareInfo->strSkinFolder;
	}

	bool CWindowUI::AddFont(int Fontid, HFONT hFont)
	{
		ASSERT(m_pShareInfo && m_pShareInfo->FontArray);

		if(NULL == m_pShareInfo->FontArray) return false;

		return m_pShareInfo->FontArray->Add(Fontid, hFont);
	}

	HFONT CWindowUI::GetFont(int Fontid)
	{
		ASSERT(m_pShareInfo && m_pShareInfo->FontArray);
		if(!m_pShareInfo || !m_pShareInfo->FontArray) return NULL;

		return (HFONT)m_pShareInfo->FontArray->Select(Fontid);
	}

	bool CWindowUI::AddGroup(LPCTSTR strGroup)
	{
		return m_GroupArray.Insert(strGroup, NULL);
	}

	void CWindowUI::SetCaption(CControlUI * pControl)
	{
		ASSERT(m_pViewInfo);
		m_pViewInfo->pCaptionControl = pControl;
	}

	CControlUI * CWindowUI::GetCaption() const
	{
		return m_pViewInfo->pCaptionControl;
	}

	CControlUI * CWindowUI::GetFocusControl()
	{
		ASSERT(m_pViewInfo);
		return m_pViewInfo->pFocusControl;
	}

    BOOL CWindowUI::PostBreakMessage(BOOL bCheck, CControlUI * pParent)
    {
        ASSERT(m_hWnd);
        HWND hWnd = ::GetWindowOwner(m_hWnd);

        hWnd = hWnd ? hWnd : m_hWnd;

        return ::PostMessage(hWnd, WM_BREAKLOOP, bCheck, (LPARAM)pParent);
    }

	//用来过滤pControl的SendNotify，方便对消息进行监控处理
    void CWindowUI::SendNotify(TNOTIFYUI &notify)
	{
		TCHAR strText[MAX_PATH / 8];
		CControlUI * pControl = (CControlUI *)notify.pSender;
		CControlUI * pTmpControl = NULL;

		TRACE(_T("CWindowUI::SendNotify CControlUI=%s , type=%d"), 
			(LPCTSTR)pControl->GetName(), notify.dwType);

		switch (notify.dwType)
		{
		case EnumNotifyMsg::NonMessage:
			{
				ASSERT(0 && "CWindowUI::HandleMessage = EnumNotifyMsg::NonMessage");
			}break;
		case EnumNotifyMsg::SelectItem:
			{
				if(pControl->GetName() == _T("restorebtn"))//还原按钮
				{
					//SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
					::ShowWindow(m_hWnd, SW_RESTORE);
					break;
				}
			}break;
		case EnumNotifyMsg::ClickItem:
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
		case EnumNotifyMsg::CheckItem:
			{
				if(TRUE == notify.wParam)//被选中
				{
					pControl->CallWndProc(m_hWnd, WMU_GETGROUP, sizeof(strText),(LPARAM)strText);

					if(_T('\0') != strText[0])//存在分组
					{
						pTmpControl = (CControlUI *)m_GroupArray.Find(strText);
						if(pTmpControl && pTmpControl != pControl)
						{
							//判断窗口是为了避免控件移动到别的窗口了还产生消息
							if(pTmpControl->GetWindow() == m_hWnd)
							{
								pTmpControl->CallWndProc(m_hWnd, WMU_SETCHECK, FALSE, NULL);
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
		this->OnNotify(notify);
		return;
	}

	//MouseProc的核心是管理控件消息流！
	LRESULT CWindowUI::MouseProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT point;

		RECT rcItem = {0};
		CControlUI * pTmpControl = NULL;
		CControlUI * pControl = NULL;

		if(nullptr == m_pViewInfo->pRootControl) return 0;
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);

		if(m_pViewInfo->pPushedControl)
		{
			//如果pPushedControl不是显示状态，那么point的位置是相对于窗口的位置
			//一般不会出现不显示的情况
			if(m_pViewInfo->pPushedControl->GetItemFixedRect(rcItem))
			{
				//如果pPushedControl是显示状态，那么point是相对于控件的位置
				point.x -= rcItem.left;
				point.y -= rcItem.top;
			}
			pControl = m_pViewInfo->pPushedControl;
		}
		else
		{
			pControl = m_pViewInfo->pRootControl->FindControlByPoint(point);
		}

		if(WM_MOUSELEAVE == message && NULL != m_pViewInfo->pHotControl)
		{
			//原始的WM_MOUSELEAVE，是鼠标离开窗口发出的，所以处理后返回即可
			//有人问这个要不要通知一下pPushedControl，其实是不用的，因为
			//如果pHotControl等于pPushedControl，那么pPushedControl同样收到了通知
			//如果pHotControl不等于pPushedControl,那么pPushedControl在之前切换
			//热点的时候已经收到了WM_MOUSELEAVE通知，因为pHotControl不等于pPushedControl
			//的时候，之前一定发生过热点切换事件

			pTmpControl = m_pViewInfo->pHotControl;
			m_pViewInfo->pHotControl = NULL;

			return pTmpControl->CallWndProc(m_hWnd, WM_MOUSELEAVE, 0, 0);
		}
		else if(WM_MOUSEMOVE == message && pControl && pControl == m_pViewInfo->pPushedControl)
		{
			if(NULL == pControl->FindControlByPoint(point))
			{
				if(STATE_HOT & pControl->GetState())
				{
					m_pViewInfo->pHotControl = NULL;
					pControl->CallWndProc(m_hWnd, WM_MOUSELEAVE, 0, 0);
				}
			}
			else
			{
				if(NULL == (STATE_HOT & pControl->GetState()))
				{
					m_pViewInfo->pHotControl = pControl;
					pControl->CallWndProc(m_hWnd, WM_MOUSEENTER, wParam, MAKELONG(point.x, point.y));
				}
			}
		}
		else if(WM_MOUSEMOVE == message && pControl != m_pViewInfo->pHotControl)
		{
			//当鼠标移动时，有可能离开一个控件，进入另一个控件，在此进行处理
			//此处仅进行WM_MOUSELEAVE 与 WM_MOUSEENTER的转换，意义在于切换热点
			
			//切换热点
			pTmpControl = m_pViewInfo->pHotControl;
			m_pViewInfo->pHotControl = pControl;
			//逻辑：先离开一个房间，才能进入另外一个房间
			//先发出离开通知，
			if(NULL != pTmpControl)
			{
				pTmpControl->CallWndProc(m_hWnd, WM_MOUSELEAVE, 0, 0);
			}

			//再发出进入通知
			if(NULL != pControl)
			{
				pControl->CallWndProc(m_hWnd, WM_MOUSEENTER, wParam, MAKELONG(point.x, point.y));
			}
		}
		else if(WM_LBUTTONDOWN == message && pControl && pControl != m_pViewInfo->pFocusControl)
		{
			if(true == pControl->IsControl())
			{
				//pControl是一个控件，让他获得焦点
				pControl->SetFocus();
			}
			else
			{
				//pControl是一个布局，布局是不会获得焦点的，所以我们让本来的控件失去焦点
				if(m_pViewInfo->pFocusControl)
				{
					pTmpControl = m_pViewInfo->pFocusControl;
					m_pViewInfo->pFocusControl = NULL;
					pTmpControl->CallWndProc(m_hWnd, WM_KILLFOCUS, NULL, NULL);
				}
			}
		}

		if(pControl)//当鼠标处于Pushed状态，并移出窗口，pControl会为空
		{
			pControl->CallWndProc(m_hWnd, message, wParam, MAKELONG(point.x, point.y));
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
				if(NULL != (STATE_PUSHED & pControl->GetState()))//SetCaptrue还没清除
				{
					pControl->ReleaseCapture();
				}
			}
			else if(WM_LBUTTONDBLCLK == message)
			{
				//双击标题栏
				//这里发出WM_LBUTTONUP通知是为了弥补一个BUG，比如窗口最大化后，鼠标相对于窗口的位置就发生了改变，
				//这个时候会收到WM_MOUSEMOVE，并且标题栏是按下的，那么这个时候窗口将会跟随着鼠标的位置进行移动
				//所以这里发出一个WM_LBUTTONUP消息，释放pPushedControl，这样就不会发生窗口移动事件
				//m_pViewInfo->pPushedControl->CallWndProc(m_hWnd, WM_LBUTTONUP, wParam, MAKELONG(point.x, point.y));
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
				TRACE(_T("CWindowUI::MouseProc WM_NCLBUTTONDBLCLK"));
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
					TRACE(_T("WM_CHAR = %x"), wParam);
					return 0;
				}

				m_pViewInfo->pFocusControl->CallWndProc(m_hWnd, message, wParam,lParam);

				//如果系统键按下，消息就不必流入DefWindowProc
				if(WM_KEYDOWN == message)
				{
					if(TRUE == GetKeyboardState(keyState))
					{
						if(keyState[VK_CONTROL] & 128 || keyState[VK_MENU] & 128
							|| keyState[VK_LWIN] & 128 || keyState[VK_RWIN] & 128)
						{
							m_pViewInfo->tSurplus = (TCHAR)wParam - (_T('A') - 1);
							TRACE(_T("GetKeyboardState = %x"), wParam);
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
		RECT * pRect= NULL, rect = { 0 };
		MONITORINFO oMonitor = {};
		PAINTSTRUCT ps;
		POINT Point;
		CControlUI * pControl = NULL;
		TEVENT event = { 0 };

		DWORD dwStype = NULL;
		int nWidth =0 , nHeight = 0;
		//先在这里从统一处理一下
		CONTROLTIMER * pControlTimer = NULL;

		//TRACE(_T("CWindowUI::WndProc message = 0x%08x"), message);
		switch (message)
		{
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
				pControlTimer = (CONTROLTIMER*)m_atrControlTimer.find((__int64)wParam);

				if(NULL == pControlTimer)
				{
					event.dwType = EnumEventType::OnTimer;
					event.wParam = wParam;
					event.lParam = lParam;
					return this->OnEvent(event);
				}
				else
				{
					pControl = (CControlUI*)pControlTimer->pControl;
					return pControl->CallWndProc(m_hWnd, message, 
						pControlTimer->nIDEvnet, lParam);
				}
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
					TRACE(_T("CWindowUI WM_NCCALCSIZE"));
					
					oMonitor.cbSize = sizeof(oMonitor);
					::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);

					*pRect = oMonitor.rcWork;
					TRACE(_T("WM_NCCALCSIZE left=%d, top=%d, right=%d, bottom=%d"),
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
				//下面的说明有待商榷
				//如果是因为WM_SIZE而更新，WM_PAINT的绘制速度无法跟上WM_SIZE的变化速度
				//在窗口拖动变大时，将会出现黑色区域，解决方法如下是：return 0L;
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
				::GetClientRect(m_hWnd, &rect);

				//不是最大化
				dwStype = (DWORD) GetWindowLong(m_hWnd, GWL_STYLE);
				if (WS_SIZEBOX == (dwStype & WS_SIZEBOX) && !::IsZoomed(m_hWnd))
				{
					RECT rcSizeBox = { 4,4,4,4 };
                    if (Point.y < rect.top + rcSizeBox.top)
					{
                        if (Point.x < rect.left + rcSizeBox.left) return HTTOPLEFT;
                        if (Point.x > rect.right - rcSizeBox.right) return HTTOPRIGHT;
						return HTTOP;
					}
                    else if (Point.y > rect.bottom - rcSizeBox.bottom)
					{
                        if (Point.x < rect.left + rcSizeBox.left) return HTBOTTOMLEFT;
                        if (Point.x > rect.right - rcSizeBox.right) return HTBOTTOMRIGHT;
						return HTBOTTOM;
					}

                    if (Point.x < rect.left + rcSizeBox.left) return HTLEFT;
                    if (Point.x > rect.right - rcSizeBox.right) return HTRIGHT;
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
				TRACE(_T("WM_NCLBUTTONDBLCLK"));
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
				event.dwType = EnumEventType::WindowInit;
				event.lParam = lParam;
				this->OnEvent(event);

				event.dwType = EnumEventType::WindowReady;
				event.wParam = wParam;
				event.lParam = lParam;

				return this->OnEvent(event);
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
				if(m_pShareInfo->hPaintDC)
				{
					::ReleaseDC(m_hWnd, m_pShareInfo->hPaintDC);
				}

				if(m_pShareInfo->pToolTip)
				{
					m_pShareInfo->pToolTip->Destroy();
					delete m_pShareInfo->pToolTip;
					m_pShareInfo->pToolTip = NULL;
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

				event.dwType = EnumEventType::WindowShow;
				event.wParam = wParam;
				event.lParam = lParam;
				return this->OnEvent(event);
			}break;
		case WM_GETFONT:
			{
				return (LRESULT)m_pShareInfo->FontArray->Select(m_nFontId);
			}break;
		case WM_SIZE:
			{
				//m_bRepaint = TRUE;
				rect.left = 0;
				rect.top = 0;
				rect.right = LOWORD(lParam);
				rect.bottom = HIWORD(lParam);

				if(NULL == m_pViewInfo->pRootControl) break;

				//注意，始祖控件的外边距没有生效
				m_pViewInfo->pRootControl->SetItem(rect, true);

				//设置窗口
				SIZE szRound = m_pViewInfo->pRootControl->GetBorderRound();
				if((szRound.cx || szRound.cy) && !::IsIconic(m_hWnd))
				{
					rect.right += 1;
					rect.bottom += 1;
					HRGN hRgn = ::CreateRoundRectRgn(rect.left, rect.top, rect.right, rect.bottom,
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
					m_pViewInfo->pFocusControl->CallWndProc(m_hWnd, message, wParam, lParam);
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
						::GetWindowRect(m_hWnd, &rect);
                        ScreenToClient(m_hWnd, &Point);
						//TRACE(_T("WM_MOUSEMOVE rect  left=%d, top =%d"), rect.left ,rect.top);
						//TRACE(_T("WM_MOUSEMOVE point point.x=%d, point.y=%d"), point.x ,point.y);
						//TRACE(_T("WM_MOUSEMOVE Mouse pm.x=%d, pm.y=%d"), ptMouse.x ,ptMouse.y);
						//TRACE(_T("WM_MOUSEMOVE diff  x  =%d,  y   =%d"), rect.left + point.x - ptMouse.x ,rect.top + point.y - ptMouse.y);
                        ::SetWindowPos(m_hWnd, 0, rect.left + Point.x - m_pViewInfo->ptLeftMouse.x,
                            rect.top + Point.y - m_pViewInfo->ptLeftMouse.y,
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
				WndProc(WM_LBUTTONDOWN, wParam, lParam);
				TRACE(_T("WM_LBUTTONDBLCLK"));
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_LBUTTONDOWN://左按下
			{
				TRACE(_T("WM_LBUTTONDOWN"));
				m_pViewInfo->ptLeftMouse.x = GET_X_LPARAM(lParam);
				m_pViewInfo->ptLeftMouse.y = GET_Y_LPARAM(lParam);
				
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_LBUTTONUP://左弹起
			{
				TRACE(_T("WM_LBUTTONUP"));
				m_pViewInfo->ptLeftMouse.x = GET_X_LPARAM(lParam);
				m_pViewInfo->ptLeftMouse.y = GET_Y_LPARAM(lParam);

				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MBUTTONDBLCLK://中双击
			{
				TRACE(_T("WM_MBUTTONDBLCLK"));
				WndProc(WM_MBUTTONDOWN, wParam, lParam);
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
				TRACE(_T("WM_RBUTTONDBLCLK"));
				WndProc(WM_RBUTTONDOWN, wParam, lParam);
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
					return m_pViewInfo->pHotControl->CallWndProc(m_hWnd, message, wParam, lParam);
				}
			}break;
		case WM_MOVE:
			{
				RECT rcShow = {0};
				BOOL bShowInScreen = m_bShowInScreen;

				::GetWindowRect(m_hWnd, &rect);
				oMonitor.cbSize = sizeof(oMonitor);
				::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);

				if(FALSE == ::IntersectRect(&rcShow, &rect, &oMonitor.rcWork))
				{
					m_bShowInScreen = FALSE;
					return 0;
				}

				if(TRUE == IsSameRect(rcShow, rect))
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
					::GetClientRect(m_hWnd, &rect);
					m_pShareInfo->pRenderEngine->BeginPaint(rect);
					m_pViewInfo->pRootControl->OnPaint(rect, 
						m_pViewInfo->pRootControl->GetItem(), ps.rcPaint);
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
				
				ASSERT(0 && "这个事件一直没触发过，所以在这里设置一个错误，作为提示");
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
		//控件发出通知呼叫
		case WM_CONTROLMSG_POST:
		case WM_CONTROLMSG_SEND:
			{
				pControl = (CControlUI *)wParam;

				return pControl->CallWndProc(((MSG*)lParam)->hwnd , ((MSG*)lParam)->message,
					((MSG*)lParam)->wParam, ((MSG*)lParam)->lParam);

				if(WM_CONTROLMSG_POST == message && lParam)
				{
					delete (void *)lParam;
				}
			}break;
		case WM_NOTIFYEX:
			{
				//wParam 是发出通知的窗口句柄
				return FALSE;
			}break;
		case WM_SETTIMER:
			{
				ASSERT(lParam && "你没有设置 CONTROLTIMER");

				if(NULL == lParam) return FALSE;
				
				if(FALSE == wParam)//killTimer
				{
					pControlTimer = (CONTROLTIMER*)lParam;
					::KillTimer(m_hWnd, (UINT_PTR)(char *)pControlTimer->pControl + pControlTimer->nIDEvnet);
					pControlTimer = (CONTROLTIMER*)m_atrControlTimer.remove((__int64)(char *)
						pControlTimer->pControl + pControlTimer->nIDEvnet);

					if(pControlTimer)
					{
						delete pControlTimer;
					}
				}
				else//TRUE : SetTimer
				{
					pControlTimer = new CONTROLTIMER;
					*pControlTimer = *(CONTROLTIMER*)lParam;
					m_atrControlTimer.insert((__int64)(char *)pControlTimer->pControl + pControlTimer->nIDEvnet, 
						pControlTimer);
					::SetTimer(m_hWnd,(UINT_PTR) (char *)pControlTimer->pControl + pControlTimer->nIDEvnet, 
						pControlTimer->uElapse,  NULL);
				}
				return TRUE;
			}break;
        case WM_POPUPMENU:
        {
            if (wParam)
            {
                Point.x = GET_X_LPARAM(lParam);
                Point.y = GET_Y_LPARAM(lParam);
                reinterpret_cast<IMenuPopup*>(wParam)->Popup(static_cast<INotify*>(this), Point);
            }
            return !!wParam;
        }break; 
        case WM_POPUPDIALOG:
        {
            if (wParam)
            {
                reinterpret_cast<IDialogPopup*>(wParam)->Popup(lParam);
            }
            return !!wParam;
        }
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
						ASSERT(0 && "无效的WM_REQUESTINFO参数");
					}break;
				}
			}break;
		//焦点
		case WM_SETFOCUS:
			{
				TRACE(_T("CWindowUI %s: WM_SETFOCUS"), m_strClassName);

				if(NULL == lParam)
				{
					m_pViewInfo->bFocusWnd = TRUE;

					event.dwType = EnumEventType::SetFocued;
					event.wParam = wParam;
					event.lParam = lParam;
					return this->OnEvent(event);
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
							pControl->CallWndProc(m_hWnd, WM_KILLFOCUS, 0, 0);
						}
					}

					pControl = m_pViewInfo->pFocusControl;
					pControl->CallWndProc(m_hWnd, message, 0, 0);
					return 0;
				}
			}break;
		case WM_KILLFOCUS:
			{
				TRACE(_T("CWindowUI %s: WM_KILLFOCUS"), m_strClassName);
				//撤掉焦点，并且通知失去焦点
				m_pViewInfo->bFocusWnd = FALSE;
				pControl = m_pViewInfo->pFocusControl;
				m_pViewInfo->pFocusControl = NULL;

				if(pControl)
				{
					pControl->CallWndProc(m_hWnd, message, wParam, lParam);
				}

				event.dwType = EnumEventType::KillFocued;
				event.wParam = wParam;
				event.lParam = lParam;
				return this->OnEvent(event);
			}break;
		//系统自定义的消息
		default:
			{
				ASSERT(m_pShareInfo);
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