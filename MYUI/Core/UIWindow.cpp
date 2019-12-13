
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
			//������Դģʽ������������ͷ���Դ
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
		case WM_NCCREATE://�ڶ�����Ϣ
			{
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				pWindow = static_cast<CWindowUI*>(lpcs->lpCreateParams);
				pWindow->m_hWnd = hwnd;
				pWindow->m_pShareInfo->hWnd = hwnd;
				::SetWindowLongPtr(hwnd, 0, reinterpret_cast<LPARAM>(pWindow));
				
				//����һ����Ⱦ����
				event.dwType = EnumEventType::RequestRenderEngine;
				pWindow->m_pShareInfo->pRenderEngine = (CRenderEngine*)pWindow->OnEvent(event);
				if(NULL == pWindow->m_pShareInfo->pRenderEngine)
				{
					//���ؿգ���ôʹ��Ĭ������
					pWindow->m_pShareInfo->pRenderEngine = new CGdiRenderEngine(pWindow->m_hWnd);
				}

                if (pWindow->m_pShareInfo->strSkinFolder)
                {
                    pWindow->SetSkin(pWindow->m_pShareInfo->strSkinFolder);
                }

				return TRUE;
			}break;
		case WM_NCDESTROY://�ܹ��յ��ģ����һ����Ϣ�������ڶ���
			{
				pWindow = reinterpret_cast<CWindowUI*>(::GetWindowLongPtr(hwnd, 0));
				if(NULL != pWindow) 
				{
					::SetWindowLongPtr(pWindow->m_hWnd, 0, 0L);
					//���ڽ���֪ͨ
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

					event.dwType = EnumEventType::OnFinal;//һ��������ִ��delete this
					pWindow->OnEvent(event);
				}
			}break;
		case WM_GETMINMAXINFO:
			{
				//��һ����Ϣ�����ǲ���
				//ע�⣬��������Ϣ�У�����ı䴰�ڴ�С�����ǻ��յ�������Ϣ��
				//����default����
				pWindow = NULL;
			}
		case WM_QUIT:
			{
				//���һ����Ϣ���������ղ����ģ�����default����
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
		//WS_CLIPSIBLINGS | WS_CLIPCHILDREN �������ڲ��ܼ���
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

                //WM_TIMER��lParam�ǻص������������Ϊ�ս�����DispatchMessage �е��øú�����
                //���û���WndProc���ղ���WM_TIMER֪ͨ��������Щ�û���ϣ��lParam��Ϊһ������
                //�����Ǻ���������������ǰ��WM_TIMER��Ϣ���͵��û����ڲ����أ���������ʵ��
                //lparam ��Ϊ��ʱ��������Ŀ��
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
                            //ͬһ���˵�, ����Ҫ�ظ�����
                            PeekMessage(&Msg, 0, Msg.message, Msg.message, PM_REMOVE);
                            continue;
                        }

                        if (m_hWnd != Msg.hwnd) goto BREAKLOOP;
                    }break;
                    case WM_BREAKLOOP:
                    {
                        if (FALSE == Msg.wParam) goto BREAKLOOP;//����Ҫ���κ��жϣ��������˳�

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

		// �������ʾ��ģʽ����Ļ����
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
            //������Դģʽ������������ͷ���Դ
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
            //������Դģʽ������������ͷ���Դ
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
            ASSERT(0 && "û���ں��ʵ�ʵ�ʵ���CWindowUI::SetSyncResource�����Բ��ܻ�ȡ��Ч����Դ����");
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
	//strSkin == NULL �൱��ж��
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

	//��������pControl��SendNotify���������Ϣ���м�ش���
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
				if(pControl->GetName() == _T("restorebtn"))//��ԭ��ť
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
				if(TRUE == notify.wParam)//��ѡ��
				{
					pControl->CallWndProc(m_hWnd, WMU_GETGROUP, sizeof(strText),(LPARAM)strText);

					if(_T('\0') != strText[0])//���ڷ���
					{
						pTmpControl = (CControlUI *)m_GroupArray.Find(strText);
						if(pTmpControl && pTmpControl != pControl)
						{
							//�жϴ�����Ϊ�˱���ؼ��ƶ�����Ĵ����˻�������Ϣ
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

		//��󷢳�notify���û�����
		this->OnNotify(notify);
		return;
	}

	//MouseProc�ĺ����ǹ���ؼ���Ϣ����
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
			//���pPushedControl������ʾ״̬����ôpoint��λ��������ڴ��ڵ�λ��
			//һ�㲻����ֲ���ʾ�����
			if(m_pViewInfo->pPushedControl->GetItemFixedRect(rcItem))
			{
				//���pPushedControl����ʾ״̬����ôpoint������ڿؼ���λ��
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
			//ԭʼ��WM_MOUSELEAVE��������뿪���ڷ����ģ����Դ���󷵻ؼ���
			//���������Ҫ��Ҫ֪ͨһ��pPushedControl����ʵ�ǲ��õģ���Ϊ
			//���pHotControl����pPushedControl����ôpPushedControlͬ���յ���֪ͨ
			//���pHotControl������pPushedControl,��ôpPushedControl��֮ǰ�л�
			//�ȵ��ʱ���Ѿ��յ���WM_MOUSELEAVE֪ͨ����ΪpHotControl������pPushedControl
			//��ʱ��֮ǰһ���������ȵ��л��¼�

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
			//������ƶ�ʱ���п����뿪һ���ؼ���������һ���ؼ����ڴ˽��д���
			//�˴�������WM_MOUSELEAVE �� WM_MOUSEENTER��ת�������������л��ȵ�
			
			//�л��ȵ�
			pTmpControl = m_pViewInfo->pHotControl;
			m_pViewInfo->pHotControl = pControl;
			//�߼������뿪һ�����䣬���ܽ�������һ������
			//�ȷ����뿪֪ͨ��
			if(NULL != pTmpControl)
			{
				pTmpControl->CallWndProc(m_hWnd, WM_MOUSELEAVE, 0, 0);
			}

			//�ٷ�������֪ͨ
			if(NULL != pControl)
			{
				pControl->CallWndProc(m_hWnd, WM_MOUSEENTER, wParam, MAKELONG(point.x, point.y));
			}
		}
		else if(WM_LBUTTONDOWN == message && pControl && pControl != m_pViewInfo->pFocusControl)
		{
			if(true == pControl->IsControl())
			{
				//pControl��һ���ؼ���������ý���
				pControl->SetFocus();
			}
			else
			{
				//pControl��һ�����֣������ǲ����ý���ģ����������ñ����Ŀؼ�ʧȥ����
				if(m_pViewInfo->pFocusControl)
				{
					pTmpControl = m_pViewInfo->pFocusControl;
					m_pViewInfo->pFocusControl = NULL;
					pTmpControl->CallWndProc(m_hWnd, WM_KILLFOCUS, NULL, NULL);
				}
			}
		}

		if(pControl)//����괦��Pushed״̬�����Ƴ����ڣ�pControl��Ϊ��
		{
			pControl->CallWndProc(m_hWnd, message, wParam, MAKELONG(point.x, point.y));
		}

		//�ж��Ƿ����ڱ�����
		if (pControl && pControl == m_pViewInfo->pCaptionControl)//������
		{
			if(WM_LBUTTONDOWN == message)
			{
				if(NULL == (STATE_PUSHED & pControl->GetState()))//û��SetCaptrue
				{
					pControl->SetCapture();
				}
			}
			else if (WM_LBUTTONUP == message)
			{
				if(NULL != (STATE_PUSHED & pControl->GetState()))//SetCaptrue��û���
				{
					pControl->ReleaseCapture();
				}
			}
			else if(WM_LBUTTONDBLCLK == message)
			{
				//˫��������
				//���﷢��WM_LBUTTONUP֪ͨ��Ϊ���ֲ�һ��BUG�����細����󻯺��������ڴ��ڵ�λ�þͷ����˸ı䣬
				//���ʱ����յ�WM_MOUSEMOVE�����ұ������ǰ��µģ���ô���ʱ�򴰿ڽ������������λ�ý����ƶ�
				//�������﷢��һ��WM_LBUTTONUP��Ϣ���ͷ�pPushedControl�������Ͳ��ᷢ�������ƶ��¼�
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

				//���ϵͳ�����£���Ϣ�Ͳ�������DefWindowProc
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
				//���������뷨�У������Ϣֻ�ܽ������뷨���͹�������Ϣ��
				//���֣�Ӣ���ַ��ǽ��ղ����ģ���WM_CHAR��WM_IME_CHAR�ĳ�����
				//�ܹ��������֣�Ӣ�ĵ��ַ������Դ���WM_CHAR�͹���
				//�������뷨����Ϣ���ǽ�WM_KEYDOWN����Ϣ���ݸ�DefWndProc�����
				//ת�������ģ�����WM_KEYDOWN����Ϣ����ֱ�ӷ��أ����뽻��
				//DefWndProc���������������󣬺������ղ������뷨�ַ���Ϣ
			}break;
		default:
			break;
		}

		//if(MCS_DISABLEIME & m_pShareInfo->pFocusControl->GetStyle())
		//{
		//	//�������뷨,ֻҪWM_KEYDOWN������DefWindowProc���Ͳ���������뷨��Ϣ
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
		//���������ͳһ����һ��
		CONTROLTIMER * pControlTimer = NULL;

		//TRACE(_T("CWindowUI::WndProc message = 0x%08x"), message);
		switch (message)
		{
		case WM_GETMINMAXINFO://���ڴ�С�ı�ǰ
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
		//��׽����������Ϣ���ر�����
		case WM_NCCALCSIZE://���α�����
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
					// ���ʱ�����㵱ǰ��ʾ�����ʺϿ�߶�
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
					//���ڲ�����С��
					//return (0 == wParam) ? TRUE : FALSE;
					return !wParam;
				}
			}break;
		case WM_NCACTIVATE://���α�����
			{
				//��WM_NCACTIVATE��Ϣ��wParam����Ϊtrue ʱ��ʾ���屻���
				//Ϊfalse ʱ��ʾ�����Ϊδ�������״̬�����ʱ����Ϣ�ķ���ֵ��
				//Ϊtrue ��ʾ�����������Ĵ��壬Ϊfalse ��ʾ�������������Ĵ���
				//TRACE((wParam == 0) ? _T("WM_NCACTIVATE = TRUE") : _T("WM_NCACTIVATE = FALSE"));
				return (wParam == 0) ? TRUE : FALSE;
			}break;
		case WM_NCPAINT://���ηǿͻ�������
			{
				//�����˵���д���ȶ
				//�������ΪWM_SIZE�����£�WM_PAINT�Ļ����ٶ��޷�����WM_SIZE�ı仯�ٶ�
				//�ڴ����϶����ʱ��������ֺ�ɫ���򣬽�����������ǣ�return 0L;
				//���δ��ڵĿͻ������������ͱ߿򣩣����������ڱ�ɿͻ���
				//dwStype = (DWORD) GetWindowLong(m_hWnd, GWL_STYLE);

				//���ڲ��ᱻ�϶���С������Ϣ����DefWndProc�������������Բ��
				//��������ı��Сʱ���ͷ��ظ�ϵͳ���������������Բ�ǡ�
				//if(WS_SIZEBOX == (dwStype & WS_SIZEBOX) ||
				//	(m_SizeMin.cx != m_SizeMax.cx || m_SizeMin.cy != m_SizeMax.cy))
				//{
					  //return 0;
				//}
			}break;
		case WM_NCHITTEST: 
			{
				//����ڷǿͻ����ƶ�
				//û�б߿��ˣ����ǲ����϶����ڣ����⴦��һ��
                Point.x = GET_X_LPARAM(lParam);
                Point.y = GET_Y_LPARAM(lParam);
                ::ScreenToClient(m_hWnd, &Point);
				//RECT rcClient;
				::GetClientRect(m_hWnd, &rect);

				//�������
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
				//��ʵ�������յ�������Ϣ��WM_NCHITTEST��������HTCAPTION(������),���Դ󲿷ֵ�WM_NCBUTTON��Ϣ�����ᷢ��
				//���ʱ�����Ǿ���Ҫ��һЩ�ͻ�����Ϣ��ת���ɷǿͻ�����Ϣ������MW_LBUTTONDBLCLKת��WM_NCLBUTTONDBLCLK
				//��������ú��鷳������ʵ�����WM_NCHITTEST����HTCAPTION�Ļ����������鷳����Ϊ������HTCAPTION������
				//ֻ��Ҫ���ٲ��ֵ�WM_BUTTONת��WM_NCBUTTON���������HTCAPTION����ô�������е�WM_NCBUTTON��Ϣ��Ҫת��
				//��WM_BUTTON
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
		//������Ϣ��CreateWindow��ɺ��յ�
		case WM_CANCELMODE:
			{
				//SetCaptrue��ʱ��ᷢ�������Ϣ��wParam��lParam����ʹ��
				if(1 == wParam)//wParam��lParam����ʹ��,�����Ǿ��Լ�����һ�£���Ϊ����
				{
					m_pViewInfo->pPushedControl = reinterpret_cast<CControlUI*>(lParam);
				}
			}break;
		case WM_CREATE://���ڴ����ɹ�
			{
				m_pShareInfo->pToolTip = new CToolTipUI();
				m_pShareInfo->pToolTip->Create(m_hInstance);

				m_pShareInfo->hPaintDC = ::GetDC(m_hWnd);

				//֪ͨ��ʼ��
				event.dwType = EnumEventType::WindowInit;
				event.lParam = lParam;
				this->OnEvent(event);

				event.dwType = EnumEventType::WindowReady;
				event.wParam = wParam;
				event.lParam = lParam;

				return this->OnEvent(event);
			}break;
		case WM_CLOSE://����׼���ر�
			{
				if(m_pViewInfo->pRootControl)
				{
					m_pViewInfo->pRootControl->SetShareInfo(NULL);
				}
				m_pViewInfo->pFocusControl = NULL;
				m_pViewInfo->pHotControl = NULL;
				m_pViewInfo->pPushedControl = NULL;
			}break;
		case WM_DESTROY://���������Ѿ��ر�
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
					//�ѻ�ԭ��ť�ҳ���������������״̬����Ϊ��ԭ��ť�����Ŵ����Ƿ����
					//�������ı��
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

				//ע�⣬ʼ��ؼ�����߾�û����Ч
				m_pViewInfo->pRootControl->SetItem(rect, true);

				//���ô���
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
		case WM_DISPLAYCHANGE://��Ļ�ֱ��ʸı�
			{
				nWidth = LOWORD(lParam);
				nHeight = HIWORD(lParam);
			}break;
			//�����Ϣ
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
						//���������ڰ���״̬���϶�����
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
				//����������֪ͨ
				
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
		case WM_LBUTTONDBLCLK://��˫��
			{
				//�ֲ�һ����갴����Ϣ��ʹ��Ϣ��ԣ����Է���˫����Ϣ��Ҫע���ˣ�MYUI����Ϣ��Windows������
				//Windows˫����WM_LBUTTONDOWN->WM_LBUTTONUP->WM_LBUTTONDBLCLK->WM_LBUTTONUP
				//MYUI˫����   WM_LBUTTONDOWN->WM_LBUTTONUP->WM_LBUTTONDOWN->WM_LBUTTONDBLCLK->WM_LBUTTONUP
				WndProc(WM_LBUTTONDOWN, wParam, lParam);
				TRACE(_T("WM_LBUTTONDBLCLK"));
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_LBUTTONDOWN://����
			{
				TRACE(_T("WM_LBUTTONDOWN"));
				m_pViewInfo->ptLeftMouse.x = GET_X_LPARAM(lParam);
				m_pViewInfo->ptLeftMouse.y = GET_Y_LPARAM(lParam);
				
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_LBUTTONUP://����
			{
				TRACE(_T("WM_LBUTTONUP"));
				m_pViewInfo->ptLeftMouse.x = GET_X_LPARAM(lParam);
				m_pViewInfo->ptLeftMouse.y = GET_Y_LPARAM(lParam);

				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MBUTTONDBLCLK://��˫��
			{
				TRACE(_T("WM_MBUTTONDBLCLK"));
				WndProc(WM_MBUTTONDOWN, wParam, lParam);
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MBUTTONDOWN://�а���
			{
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MBUTTONUP://�е���
			{
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_RBUTTONDBLCLK://��˫��
			{
				TRACE(_T("WM_RBUTTONDBLCLK"));
				WndProc(WM_RBUTTONDOWN, wParam, lParam);
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_RBUTTONDOWN://�Ұ���
			{
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_RBUTTONUP://�ҵ���
			{
				return MouseProc(message, wParam, lParam);
			}break;
		case WM_MOUSEWHEEL:
			{
				//WM_MOUSEWHEEL�����λ������Ļλ��
                Point.x = LOWORD(lParam);
                Point.y = HIWORD(lParam);
                ScreenToClient(m_hWnd, &Point);//ת��һ��
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
			//������Ϣ
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
		case WM_PAINT://ǰ������
			{
				//�ж��Ƿ���Ҫ���л���
				if (!m_pViewInfo->pRootControl || !m_pShareInfo->pRenderEngine) break;

				//��Ҫ���ƣ�������ʽ��ʼ
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
				
				ASSERT(0 && "����¼�һֱû����������������������һ��������Ϊ��ʾ");
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
		case WM_ERASEBKGND://��������
			{
				(HDC)wParam;
				return TRUE;//����ϵͳ�Ѿ����ƹ��ˣ������ٻ���
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
		//�ؼ�����֪ͨ����
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
				//wParam �Ƿ���֪ͨ�Ĵ��ھ��
				return FALSE;
			}break;
		case WM_SETTIMER:
			{
				ASSERT(lParam && "��û������ CONTROLTIMER");

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
						ASSERT(0 && "��Ч��WM_REQUESTINFO����");
					}break;
				}
			}break;
		//����
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
				//�������㣬����֪ͨʧȥ����
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
		//ϵͳ�Զ������Ϣ
		default:
			{
				ASSERT(m_pShareInfo);
				//������Ϣ
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