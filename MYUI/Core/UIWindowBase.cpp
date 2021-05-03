
#include "UIWindowBase.h"
#include "UIWindow.h"

namespace MYUI
{
	CWindowBase::CWindowBase()
		: m_hWnd(NULL)
		, m_hInstance(NULL)
	{
		memset(m_strClassName, 0, sizeof(m_strClassName));
		memset(m_strWindowText, 0, sizeof(m_strWindowText));
	}

	CWindowBase::~CWindowBase()
	{

	}

	CWindowBase::operator HWND() const
	{
		return m_hWnd;
	}


	HWND CWindowBase::Create(HINSTANCE hInstance, HWND hWndParent, DWORD dwStyle,
		LPCTSTR strClassName, LPCTSTR strWindowText, RECT* pRect)
	{
		m_hInstance = hInstance;
		RECT rect = { 0,0,0,0 };
		if (pRect) rect = *pRect;

		if (false == RegisterClass(strClassName)) return NULL;

		m_hWnd = ::CreateWindow(strClassName, strWindowText,
			//WS_CLIPSIBLINGS | WS_CLIPCHILDREN 其他窗口不能激活
			//WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			/*WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME  |  WS_MINIMIZEBOX ,*/
			//WS_OVERLAPPEDWINDOW,
			dwStyle | (hWndParent ? NULL : WS_CLIPCHILDREN),
			rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
			hWndParent, NULL, m_hInstance, this);

		_tcscpy_s(m_strClassName, strClassName);
		_tcscpy_s(m_strWindowText, strWindowText);
		MUIASSERT(m_hWnd != NULL);
		return m_hWnd;
	}

	HWND CWindowBase::CreateEx(HINSTANCE hInstance, HWND hWndParent, DWORD dwStyleEx, DWORD dwStyle,
		LPCTSTR strClassName, LPCTSTR strWindowText, RECT* pRect)
	{
		m_hInstance = hInstance;
		RECT rect = { 0, 0, 0, 0 };
		if (pRect) rect = *pRect;

		if (false == RegisterClass(strClassName)) return NULL;

		m_hWnd = ::CreateWindowEx(dwStyleEx, strClassName, strWindowText,
			//WS_CLIPSIBLINGS | WS_CLIPCHILDREN 其他窗口不能激活
			//WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			/*WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME  |  WS_MINIMIZEBOX ,*/
			//WS_OVERLAPPEDWINDOW,
			dwStyle | (hWndParent ? NULL : WS_CLIPCHILDREN),
			rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
			hWndParent, NULL, m_hInstance, this);

		_tcscpy_s(m_strClassName, strClassName);
		_tcscpy_s(m_strWindowText, strWindowText);
		MUIASSERT(m_hWnd != NULL);
		return m_hWnd;
	}

	LRESULT CALLBACK CWindowBase::FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		CWindowBase* pWindow = NULL;
		MUIEVENT Event = { 0 };
		switch (message)
		{
		case WM_NCCREATE://第二条消息
		{
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			pWindow = static_cast<CWindowBase*>(lpcs->lpCreateParams);
			::SetWindowLongPtr(hwnd, 0, reinterpret_cast<LPARAM>(pWindow));
			pWindow->m_hWnd = hwnd;
			return pWindow->WndProc(message, wParam, lParam);
		}break;
		case WM_NCDESTROY://能够收到的，最后一条消息，倒数第二条
		{
			pWindow = reinterpret_cast<CWindowBase*>(::GetWindowLongPtr(hwnd, 0));
			if (NULL != pWindow)
			{
				::SetWindowLongPtr(pWindow->m_hWnd, 0, 0L);
				pWindow->WndProc(message, wParam, lParam);
				pWindow->m_hWnd = NULL;
			}
		}break;
		case WM_GETMINMAXINFO:
		{
			//第一条消息，我们不管
			//注意，后续的消息中，如果改变窗口大小，还是会收到这条消息的
			//交给default处理
			pWindow = NULL;
		}//break;
		case WM_QUIT:
		{
			//最后一条消息，往往是收不到的，交给default处理
		}//break;
		default:
		{
			pWindow = reinterpret_cast<CWindowBase*>(::GetWindowLongPtr(hwnd, 0));
			if (pWindow)
			{
				return pWindow->WndProc(message, wParam, lParam);
			}
		}break;
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	BOOL CWindowBase::SetLayeredWindowAttributes(COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
	{
		DWORD dwStyleEx = GetWindowLong(m_hWnd, GWL_EXSTYLE);
		dwStyleEx |= WS_EX_LAYERED;
		SetWindowLong(m_hWnd, GWL_EXSTYLE, dwStyleEx);
		return ::SetLayeredWindowAttributes(m_hWnd, crKey, bAlpha, dwFlags);
	}

	BOOL CWindowBase::Close(LONG nRet)
	{
		MUIASSERT(::IsWindow(m_hWnd));
		if (!::IsWindow(m_hWnd)) return FALSE;
		::PostMessage(m_hWnd, WM_CLOSE, nRet, 0L);
		return TRUE;
	}

	BOOL CWindowBase::Destroy()
	{
		MUIASSERT(::IsWindow(m_hWnd));
		return ::DestroyWindow(m_hWnd);
	}

	BOOL CWindowBase::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
	{
		MUIASSERT(::IsWindow(m_hWnd));
		if (!::IsWindow(m_hWnd)) return FALSE;
		return ::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
	}

	LRESULT CWindowBase::ShowModal(bool bShow /*= true*/, bool bEnableParent /*= true*/)
	{
		MUIASSERT(::IsWindow(m_hWnd));
		LRESULT lResult = 0;
		HWND hOwnerWnd = GetWindowOwner(m_hWnd);
		MSG Msg = { 0 };

		::ShowWindow(m_hWnd, bShow ? TRUE : FALSE);
		if (bEnableParent && hOwnerWnd)
		{
			::EnableWindow(hOwnerWnd, FALSE);
		}

		while (::IsWindow(m_hWnd) && ::GetMessage(&Msg, NULL, 0, 0))
		{
			if (WM_CLOSE == Msg.message)
			{
				lResult = Msg.wParam;
			}

			::TranslateMessage(&Msg);
			::DispatchMessage(&Msg);
		}

		if (bEnableParent && hOwnerWnd)
		{
			::EnableWindow(hOwnerWnd, TRUE);
			::SetFocus(hOwnerWnd);
		}

		return lResult;
	}


	bool CWindowBase::RegisterClass(LPCTSTR strClassName)
	{
		WNDCLASS WndClass = { 0 };
		//wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		WndClass.style = CS_DBLCLKS;
		WndClass.lpfnWndProc = CWindowBase::FrameWndProc;
		WndClass.cbClsExtra = sizeof(void*);
		WndClass.cbWndExtra = sizeof(void*);
		WndClass.hInstance = m_hInstance;
		WndClass.hIcon = NULL;
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hbrBackground = CreateSolidBrush(NULL_BRUSH);
		WndClass.lpszMenuName = NULL;
		WndClass.lpszClassName = strClassName;

		ATOM ret = ::RegisterClass(&WndClass);
		MUIASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
		return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
	}

	HWND CWindowBase::GetHandle()
	{
		return m_hWnd;
	}

	void CWindowBase::CenterWindow()
	{
		MUIASSERT(::IsWindow(m_hWnd));
		MUIASSERT((GetWindowStyle(m_hWnd) & WS_CHILD) == 0);
		RECT rcDlg = { 0 };
		::GetWindowRect(m_hWnd, &rcDlg);
		RECT rcArea = { 0 };
		RECT rcCenter = { 0 };
		HWND hWnd = this->m_hWnd;
		HWND hWndParent = ::GetParent(m_hWnd);
		HWND hWndCenter = ::GetWindowOwner(m_hWnd);
		if (hWndCenter != NULL)
			hWnd = hWndCenter;

		// 处理多显示器模式下屏幕居中
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
		rcArea = oMonitor.rcWork;

		if (hWndCenter == NULL)
			rcCenter = rcArea;
		else
			::GetWindowRect(hWndCenter, &rcCenter);

		int DlgWidth = rcDlg.right - rcDlg.left;
		int DlgHeight = rcDlg.bottom - rcDlg.top;

		// Find dialog's upper left based on rcCenter
		int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
		int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

		// The dialog is outside the screen, move it inside
		if (xLeft < rcArea.left) xLeft = rcArea.left;
		else if (xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
		if (yTop < rcArea.top) yTop = rcArea.top;
		else if (yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;
		::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	void CWindowBase::SetIcon(UINT nRes)
	{
		HICON hIcon = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		MUIASSERT(hIcon);
		::SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);
		hIcon = (HICON)::LoadImage(m_hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		MUIASSERT(hIcon);
		::SendMessage(m_hWnd, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);
	}

	LRESULT CWindowBase::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
	{
		MUIASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
	}

	LRESULT CWindowBase::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
	{
		MUIASSERT(::IsWindow(m_hWnd));
		return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
	}

}