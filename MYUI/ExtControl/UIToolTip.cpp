
#include "UIToolTip.h"

namespace MYUI
{
	CToolTipUI::CToolTipUI()
		: m_hWnd(NULL)
		, m_nAlpha(255)
		, m_refBkColor(ARGB(0, 254,254,255))
		, m_refBorderColor(ARGB(0,118,118,118))
		, m_hFont(NULL)
	{
		m_rcTextPadding.left = m_rcTextPadding.right = 5;
		m_rcTextPadding.top = m_rcTextPadding.bottom = 4;
		m_strText[0] = _T('\0');
	}

	CToolTipUI::~CToolTipUI()
	{
		if(m_hFont) ::DeleteObject(m_hFont);
	}

	LPCTSTR CToolTipUI::g_strClassName = _T("{8E4346A2-E2C1-4434-A232-2079CFBB4263}");

	LRESULT CALLBACK CToolTipUI::FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		CToolTipUI * pWindow = NULL;

		switch (message)
		{
		case WM_NCCREATE://第二条消息
			{
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				pWindow = static_cast<CToolTipUI*>(lpcs->lpCreateParams);
				pWindow->m_hWnd = hwnd;
				::SetWindowLongPtr(hwnd, 0, reinterpret_cast<LPARAM>(pWindow));

				return TRUE;
			}break;
		case WM_NCDESTROY://能够收到的，最后一条消息，倒数第二条
			{
				pWindow = reinterpret_cast<CToolTipUI*>(::GetWindowLongPtr(hwnd, 0));
				if(NULL != pWindow)
				{
					::SetWindowLongPtr(pWindow->m_hWnd, 0, 0L);
				}
			}break;
		default:
			{
				pWindow = reinterpret_cast<CToolTipUI*>(::GetWindowLongPtr(hwnd, 0));
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
	BOOL CToolTipUI::Close(LONG nRet)
	{
		MUIASSERT(::IsWindow(m_hWnd));
		if( !::IsWindow(m_hWnd) ) return FALSE;
		::PostMessage(m_hWnd, WM_CLOSE, nRet, 0L);
		return TRUE;
	}

	BOOL CToolTipUI::Destroy()
	{
		MUIASSERT(::IsWindow(m_hWnd));
		return ::DestroyWindow(m_hWnd);
	}

	bool CToolTipUI::RegisterClass(HINSTANCE hInstance)
	{
		WNDCLASS wndclass = {0};
		wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		wndclass.lpfnWndProc = CToolTipUI::FrameWndProc;
		wndclass.cbClsExtra = sizeof(void *) ;
		wndclass.cbWndExtra = sizeof(void *);
		wndclass.hInstance = hInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
		wndclass.hbrBackground = CreateSolidBrush(NULL_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = CToolTipUI::g_strClassName;

		ATOM ret = ::RegisterClass(&wndclass);
		MUIASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
		return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
	}

	HWND CToolTipUI::Create(HINSTANCE hInstance)
	{
		if(false == RegisterClass(hInstance)) return NULL;

        m_hWnd = ::CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED, CToolTipUI::g_strClassName, _T("\0"),
			WS_POPUP | WS_CLIPSIBLINGS,
			0, 0, 0, 0,
			NULL , NULL, hInstance, this);
		
		MUIASSERT(m_hWnd!=NULL);
		return m_hWnd;
	}

	void CToolTipUI::ShowWindow(BOOL bShow)
	{
		MUIASSERT(::IsWindow(m_hWnd));
		::ShowWindow(m_hWnd, bShow);
	}

	BOOL CToolTipUI::ShowTip(POINT point)
	{
		HDC hdc = ::GetDC(m_hWnd);
		SIZE szText = {0}, szTmp;
		SIZE szCursor = {0}, szScreen = {0};
		BOOL bHasNext = FALSE;
		int nLenght = _tcslen(m_strText);
		int nStart = 0, nCount = 0, nTabCount = 0;
		HFONT hOldFont = NULL;
		if(m_hFont) hOldFont = (HFONT)::SelectObject(hdc, m_hFont);

		GetTextExtentPoint(hdc, _T("\t"), 2, &szTmp);
		int nTabWidth = szTmp.cx;

		do
		{
			bHasNext = GetStringLine(m_strText, nLenght, nStart, &nCount, &nTabCount);
			//注意，换行符也必须计算在内，正常的输入是看不到换行符占据的位置的
			//但是当有内容选择时，选择区需要将换行符占据的区域也绘制出来
			GetTextExtentPoint(hdc, m_strText + nStart, nCount, &szTmp);
			szTmp.cx += nTabCount * (7 - 1) * nTabWidth;
			
			szText.cx = MAX(szText.cx, szTmp.cx);
			szText.cy += szTmp.cy ;
			nStart += nCount;
		}while (bHasNext);

		if(hOldFont) ::SelectObject(hdc, hOldFont);
		::ReleaseDC(m_hWnd, hdc);


		szScreen.cx = GetSystemMetrics(SM_CXSCREEN);
		szScreen.cy = GetSystemMetrics(SM_CYSCREEN);
		szCursor.cx = GetSystemMetrics(SM_CXCURSOR);
		szCursor.cy = GetSystemMetrics(SM_CYCURSOR);
		
		point.y += szCursor.cy;
		szText.cx += m_rcTextPadding.left + m_rcTextPadding.right;
		szText.cy += m_rcTextPadding.top + m_rcTextPadding.bottom;
		int cx = point.x + szText.cx;
		int cy = point.y + szText.cy;

		if(cx > szScreen.cx) //控件超出显示范围
		{
			point.x = point.x - szText.cx;
		}

		if(cy > szScreen.cy) //控件超出显示范围
		{
			point.y = point.y -  szText.cy;
		}

		::SetWindowPos(m_hWnd ,HWND_TOPMOST, point.x, point.y , szText.cx, szText.cy, 
			SWP_SHOWWINDOW | SWP_NOACTIVATE);
		
		return TRUE;
	}

	void CToolTipUI::SetText(LPCTSTR strText)
	{
		_tcscpy(m_strText, strText);
	}

	LPCTSTR CToolTipUI::GetText() const
	{
		return m_strText;
	}

	void CToolTipUI::SetTextPadding(const RECT &rect)
	{
		m_rcTextPadding = rect;
	}

	const RECT& CToolTipUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CToolTipUI::SetBkColor(ARGBREF refColor)
	{
		m_refBkColor = refColor;
	}

	ARGBREF CToolTipUI::GetBkColor() const
	{
		return m_refBkColor;
	}

	void CToolTipUI::SetBorderColor(ARGBREF refColor)
	{
		m_refBorderColor = refColor;
	}

	ARGBREF CToolTipUI::GetBorderColor() const
	{
		return m_refBorderColor;
	}

	void CToolTipUI::SetFont(HFONT hFont)
	{
		m_hFont = hFont;
		if(::IsWindow(m_hWnd))
		{
			SetLayeredWindowAttributes(m_hWnd, NULL, m_nAlpha, LWA_ALPHA);
		}
	}

	HFONT CToolTipUI::GetFont() const
	{
		return m_hFont;
	}

	void CToolTipUI::SetAlpha(BYTE nAlpha)
	{
		m_nAlpha = MAX(0 , MIN(255, nAlpha));
	}

	BYTE CToolTipUI::GetAlpha() const
	{
		return m_nAlpha;
	}

	LRESULT CALLBACK CToolTipUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		RECT rect;
		POINT point;
		HDC hdc;
		PAINTSTRUCT ps;

		switch(message)
		{
		case WM_CREATE:
			{
				SetLayeredWindowAttributes(m_hWnd, NULL, m_nAlpha, LWA_ALPHA);

				if(NULL == m_hFont)
				{
					m_hFont = CreateFont(14 ,0,0,0, FW_NORMAL,FALSE,FALSE,0,
						CHINESEBIG5_CHARSET,OUT_CHARACTER_PRECIS,
						CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,
						FF_MODERN,_T("宋体"));
				}
			}break;
		case WM_SIZE:
			{
				InvalidateRect(m_hWnd, NULL, FALSE);
			}break;
		case WM_PAINT:
			{
				hdc = ::BeginPaint (m_hWnd, &ps);
				::GetClientRect(m_hWnd, &rect);
				
				HFONT hOldFont = NULL;
				if(m_hFont) hOldFont = (HFONT)::SelectObject(hdc, m_hFont);

				ARGBREF refColor = ::SetBkColor(hdc, m_refBkColor);
				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

				HBRUSH hBrush = ::CreateSolidBrush(m_refBorderColor);
				::FrameRect(hdc, &rect, hBrush);
				::DeleteObject(hBrush);

				int nBkMode = SetBkMode(hdc,TRANSPARENT);
				if(TRUE == IndentRect(&rect, &m_rcTextPadding))
				{
					::DrawText(hdc, m_strText, -1, &rect, 
						DT_VCENTER | DT_LEFT | DT_WORDBREAK | DT_EXPANDTABS);
				}
				
				SetBkMode(hdc, nBkMode);
				::SetBkColor(hdc, refColor);

				if(hOldFont) ::SelectObject(hdc, hOldFont);

				::EndPaint (m_hWnd, &ps);
				return 0;
			}break;
		case WM_ERASEBKGND:
			{
				return TRUE;
			}break;
		case WM_DESTROY:
			{
			}break;
		}
		return ::DefWindowProc(m_hWnd, message, wParam, lParam);
	}

}