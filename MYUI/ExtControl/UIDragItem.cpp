
#include "UIDragItem.h"

namespace MYUI
{

	CDragItemUI::CDragItemUI()
		: m_pTag(NULL)
		, m_hWnd(NULL)
		, m_pPointWnd(NULL)
		, m_hViewBmp(NULL)
		, m_uOverMessage(NULL)
		, m_nAlpha(255)
	{
		EmptyRect(m_rcDrag);
		m_ptMouse.x = m_ptMouse.y = 0;
		m_szRound.cx = m_szRound.cy = 0;
	}

	CDragItemUI::~CDragItemUI()
	{
	}

	CMuiString CDragItemUI::g_strClassName = _T("{BFEDFC99-C7A4-4338-82A1-FCD472BBEA57}");

	LRESULT CALLBACK CDragItemUI::FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		CDragItemUI * pWindow = NULL;

		switch (message)
		{
		case WM_NCCREATE://第二条消息
			{
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				pWindow = static_cast<CDragItemUI*>(lpcs->lpCreateParams);
				pWindow->m_hWnd = hwnd;
				::SetWindowLongPtr(hwnd, 0, reinterpret_cast<LPARAM>(pWindow));
				::SetCapture(hwnd);
				return TRUE;
			}break;
		case WM_NCDESTROY://能够收到的，最后一条消息，倒数第二条
			{
				pWindow = reinterpret_cast<CDragItemUI*>(::GetWindowLongPtr(hwnd, 0));
				if(NULL != pWindow)
				{
					::SetWindowLongPtr(pWindow->m_hWnd, 0, 0L);
				}
				::ReleaseCapture();
			}break;
		default:
			{
				pWindow = reinterpret_cast<CDragItemUI*>(::GetWindowLongPtr(hwnd, 0));
				if(pWindow)
				{
					return pWindow->WndProc(message, wParam, lParam);
				}
			}break;
		}
		return DefWindowProc (hwnd, message, wParam, lParam);
	}

	//Close是异步关闭，一般我们调用关闭函数后，就会delete 窗口类，
	//所以最好使用Destroy这个同步关闭函数，不熟悉消息机制的话，
	//CDragItemUI中不要使用异步关闭
	BOOL CDragItemUI::Close(LONG nRet)
	{
		ASSERT(::IsWindow(m_hWnd));
		if( !::IsWindow(m_hWnd) ) return FALSE;
		::PostMessage(m_hWnd, WM_CLOSE, nRet, 0L);
		return TRUE;
	}

	BOOL CDragItemUI::Destroy()
	{
		ASSERT(::IsWindow(m_hWnd));
		return ::DestroyWindow(m_hWnd);
	}

	void CDragItemUI::SetTag(LPVOID pTag)
	{
		m_pTag = pTag;
	}

	LPVOID CDragItemUI::GetTag() const
	{
		return m_pTag;
	}

	bool CDragItemUI::RegisterClass(HINSTANCE hInstance)
	{
		WNDCLASS wndclass = {0};
		wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		wndclass.lpfnWndProc = CDragItemUI::FrameWndProc;
		wndclass.cbClsExtra = sizeof(void *) ;
		wndclass.cbWndExtra = sizeof(void *);
		wndclass.hInstance = hInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
		wndclass.hbrBackground = CreateSolidBrush(NULL_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = CDragItemUI::g_strClassName;

		ATOM ret = ::RegisterClass(&wndclass);
		ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
		return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
	}

	HWND CDragItemUI::Create(HINSTANCE hInstance, CWindowUI * pPointWnd, RECT &rcDrag,
		UINT uOverMessage, int nAlpha , SIZE * szRound)
	{
		m_rcDrag = rcDrag;
		m_pPointWnd = pPointWnd;
		m_uOverMessage = uOverMessage;
		m_nAlpha = MAX(0 , MIN(255, nAlpha));
		if(szRound) m_szRound = *szRound;
		if(NULL == pPointWnd)
		{
			ASSERT(pPointWnd && "IWindowEvent * pPointWnd 必须设置才能产生回调！");
			return NULL;
		}

		if(false == RegisterClass(hInstance)) return NULL;
		::GetCursorPos(&m_ptMouse);
		m_hWnd = ::CreateWindowEx(WS_EX_LAYERED, CDragItemUI::g_strClassName,_T("\0"),
			WS_POPUP | WS_CLIPSIBLINGS,
			0, 0, m_rcDrag.right - m_rcDrag.left, m_rcDrag.bottom - m_rcDrag.top,
			NULL , NULL, hInstance, this);
		
		ASSERT(m_hWnd!=NULL);
		
		return m_hWnd;
	}

	LRESULT CALLBACK CDragItemUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		RECT rect;
		POINT point;
		HDC hdc;
		PAINTSTRUCT ps;

		switch(message)
		{
		case WM_CREATE:
			{
				hdc = ::GetDC(m_hWnd);
				m_hViewBmp = ::CreateCompatibleBitmap(hdc, m_rcDrag.right - m_rcDrag.left, m_rcDrag.bottom - m_rcDrag.top);
				m_hViewDc = ::CreateCompatibleDC(hdc);
				::SelectObject(m_hViewDc, m_hViewBmp);

				HDC hPointDc = ::GetDC(*m_pPointWnd);
				::BitBlt(m_hViewDc, 0, 0, m_rcDrag.right - m_rcDrag.left, m_rcDrag.bottom - m_rcDrag.top,
					hPointDc, m_rcDrag.left, m_rcDrag.top, SRCCOPY);

				SetLayeredWindowAttributes(m_hWnd, NULL, m_nAlpha, LWA_ALPHA);

				rect = m_rcDrag;
				::MapWindowRect(*m_pPointWnd, NULL, &rect);
				::SetWindowPos(m_hWnd, HWND_TOPMOST, rect.left, rect.top, 0 , 0, SWP_NOSIZE | SWP_SHOWWINDOW);
				
			}break;
		case WM_SIZE:
			{
				rect.left = rect.top = 0;
				rect.right = LOWORD(lParam);
				rect.bottom = HIWORD(lParam);

				if((m_szRound.cx || m_szRound.cy) && !::IsIconic(m_hWnd))
				{
					rect.right += 1;
					rect.bottom += 1;
					HRGN hRgn = ::CreateRoundRectRgn(rect.left, rect.top, rect.right, rect.bottom,
						m_szRound.cx, m_szRound.cy);
					::SetWindowRgn(m_hWnd, hRgn, TRUE);
					::DeleteObject(hRgn);
				}

				InvalidateRect(m_hWnd, NULL, FALSE);
			}break;
		case WM_MOUSEMOVE:
			{
				::GetCursorPos(&point);

				::GetWindowRect(m_hWnd, &rect);

				::SetWindowPos(m_hWnd, HWND_TOPMOST, 
					rect.left + point.x - CDragItemUI::m_ptMouse.x ,
					rect.top + point.y - CDragItemUI::m_ptMouse.y,
					0 , 0, SWP_NOSIZE);

				m_ptMouse = point;
				return 0;
			}break;
		case WM_PAINT:
			{
				hdc = ::BeginPaint (m_hWnd, &ps);
				::BitBlt(hdc, 0, 0, m_rcDrag.right - m_rcDrag.left, m_rcDrag.bottom - m_rcDrag.top,
					m_hViewDc, 0, 0, SRCCOPY);

				::EndPaint (m_hWnd, &ps);
				return 0;
			}break;
		case WM_ERASEBKGND:
			{
				return TRUE;
			}break;
		case WM_DESTROY:
			{
				if(m_hViewBmp) ::DeleteObject(m_hViewBmp);
				if(m_hViewDc) ::DeleteDC(m_hViewDc);
				
			}break;
		}

		if(message == m_uOverMessage)
		{
			//一般我们是通过WM_LBUTTONDOWN时，产生一个DragItem控件，然后在
			//WM_LBUTTONUP消息中释放DragItem，所以一般m_uOverMessage 的值是 WM_LBUTTONUP
			TEVENT event;
			::GetCursorPos(&point);
			event.dwType = DragOver;
			event.wParam = (WPARAM)this;
			event.lParam = MAKELONG(point.x, point.y);
			if(m_pPointWnd)
			{
				m_pPointWnd->OnEvent(event);
			}
		}

		return ::DefWindowProc(m_hWnd, message, wParam, lParam);
	}

}