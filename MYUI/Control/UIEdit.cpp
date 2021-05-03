
#include "UIEdit.h"
#include <imm.h>  
#pragma comment (lib ,"imm32.lib") 

#define EDIT_AUTO_BUFFER_LENGHT    1024
namespace MYUI
{
	CEditUI::CEditUI()
		: m_nSelStart(0)
		, m_nSelLenght(0)
		, m_nRowSpace(0)
		, m_hIMC(NULL)
		, m_bShowCaret(false)
	{
		m_refSelBkColor = ARGB(255,156,199,230);
		m_refSelTextColor = ARGB(255,0,0,0);
		m_refHintColor = ARGB(255, 159,171,198);
	}

	CEditUI::~CEditUI()
	{
	}

	const CMuiString CEditUI::g_strClassName(_T("EditUI"));

	CMuiString CEditUI::GetClassName() const
	{
		return CEditUI::g_strClassName;
	}

	void CEditUI::SetStyle(DWORD dwStyle)
	{
		if(MES_PASSWORD & dwStyle)
		{
            m_TextBuffer.SetPwdStyle(true);
		}
		else
		{
            m_TextBuffer.SetPwdStyle(false);
		}
		return __super::SetStyle(dwStyle);
	}

	void CEditUI::SetTextFont(int FontId)
	{
		__super::SetTextFont(FontId);
		MUITRACE(_T("CGdiRenderEngine::SetCaret"));
		TEXTMETRIC tm = { 0 };
		RECT rcCaret = { 0 };
		HFONT hOldFont = NULL;
		HDC hDC = NULL;

		if (FALSE == ::IsWindow(GETHWND(this))) return;

		hDC = ::GetDC(GETHWND(this));
		if (m_hFont)
		{
			hOldFont = (HFONT)SelectObject(hDC, m_hFont);
		}
		
		GetTextMetrics(hDC, &tm);

		if (m_hFont)
		{
			(HFONT)SelectObject(hDC, hOldFont);
		}
		::ReleaseDC(GETHWND(this), hDC);

		m_CaretInfo.Size.cx = MAX(1, tm.tmHeight / 10);
		m_CaretInfo.Size.cy = tm.tmHeight;
	}

	void CEditUI::SetTextColor(ARGBREF argb)
	{
		__super::SetTextColor(argb);
		m_CaretInfo.Color = argb;
	}

	void CEditUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("rowSpace"))) 
		{
			SetRowSpace(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("text")))
		{
			m_TextBuffer.ClearText();
			m_TextBuffer.InsterText(0, 0, strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("hint"))) 
		{
			SetHint(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("hintColor"))) 
		{
			SetHintColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("password"))) 
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MES_PASSWORD;
			}
			else
			{
				dwStyle &= (~MES_PASSWORD);
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("center"))) 
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif

			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MES_CENTER;
			}
			else
			{
				dwStyle &= (~MES_CENTER);
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("multiLine"))) 
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif

			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MES_MULTILINE;
			}
			else
			{
				dwStyle &= (~MES_MULTILINE);
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("linefree"))) 
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MES_LINEFREE;
			}
			else
			{
				dwStyle &= (~MES_LINEFREE);
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("onlyRead"))) 
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MES_READONLY;
			}
			else
			{
				dwStyle &= (~MES_READONLY);
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("IsNumber"))) 
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MES_NUMBER;
			}
			else
			{
				dwStyle &= (~MES_NUMBER);
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("lower"))) 
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MES_LOWERCASE;
			}
			else
			{
				dwStyle &= (~MES_LOWERCASE);
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("upper"))) 
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MES_UPPERCASE;
			}
			else
			{
				dwStyle &= (~MES_UPPERCASE);
			}
			SetStyle(dwStyle);
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}

		return;
	}

	SIZE CEditUI::GetContentSize()
	{
		SIZE szContent = {0};
		bool bHasNext = false;
		int nWidth = 0, nHeight = 0;
		RECT rcClient = m_rcRawItem;;
		if(!(m_dwStyle & MES_MULTILINE))
		{
			return szContent;
		}
		
		if(m_dwStyle & MES_LINEFREE)
		{
			if(m_pVerticalScrollBar)
			{
				rcClient.right -= m_pVerticalScrollBar->GetWidth();
			}

			rcClient.left += m_rcTextPadding.left;
			rcClient.right -= m_rcTextPadding.right;
			
			szContent = m_pShareInfo->pRenderEngine->GetTextSize(rcClient,
				m_TextBuffer, m_TextBuffer.GetLenght(), m_hFont,
				m_nRowSpace, TOS_AUTOLINEFREE | TOS_MULTILLINE);

			szContent.cy += m_rcTextPadding.top + m_rcTextPadding.bottom;
		}
		else
		{
			//TOS_MULTILLINE旗标中，rcClient 其实是忽略的
			szContent = m_pShareInfo->pRenderEngine->GetTextSize(rcClient,
				m_TextBuffer, m_TextBuffer.GetLenght(), m_hFont,
				m_nRowSpace, TOS_MULTILLINE);

			szContent.cx += m_rcTextPadding.right + m_rcTextPadding.left;
			szContent.cy += m_rcTextPadding.bottom + m_rcTextPadding.top;
		}

		return szContent;
	}

	CMuiString CEditUI::GetText() const
	{
		return m_TextBuffer.GetSrcText();
	}

	void CEditUI::SetText(LPCTSTR pstrText)
	{
		m_TextBuffer.ClearText();
		m_TextBuffer.InsterText(0, 0, pstrText);

		this->SetItem(m_rcItem, true);
		this->Invalidate();
	}

	void CEditUI::SetHint(LPCTSTR strHint)
	{
		m_strHint = strHint;
	}

	LPCTSTR CEditUI::GetHint() const
	{
		return m_strHint;
	}

	void CEditUI::SetHintColor(ARGBREF refColor)
	{
		m_refHintColor = refColor;
	}

	ARGBREF CEditUI::GetHintColor() const
	{
		return m_refHintColor;
	}

	int CEditUI::GetRowSpace()
	{
		return m_nRowSpace;
	}

	void CEditUI::SetRowSpace(int nRowSpace)
	{
		m_nRowSpace = nRowSpace;
	}

	void CEditUI::PaintContent(const RECT& rcUpdate)
	{
		SIZE szText;
		POINT ptOutput;
		DWORD dwType = NULL;
		RECT rcText = m_rcContent;
		SIZE szBroder = {0};

		if(_T('\0') == m_TextBuffer[0] && _T('\0') == m_strHint[0]) return;

		IndentRect(&rcText, &m_rcTextPadding);
		int nSelStart = m_nSelStart, nSelLenght = m_nSelLenght;
		if(MES_MULTILINE & m_dwStyle)
		{
			dwType |= TOS_MULTILLINE;
		}
		if(MES_CENTER & m_dwStyle)
		{
			dwType |= TOS_CENTER;
		}
		if(MES_LINEFREE & m_dwStyle)
		{
			dwType |= TOS_AUTOLINEFREE | TOS_MULTILLINE;
		}
		ptOutput.x = rcText.left;
		ptOutput.y = rcText.top;

		if(0 > nSelLenght)
		{
			nSelStart += nSelLenght;
			nSelLenght = -nSelLenght;
		}

		if(FALSE == (STATE_FOCUS & m_dwState) && m_TextBuffer.IsEmpty() && false == m_strHint.IsEmpty())
		{
			m_pShareInfo->pRenderEngine->OnTextOut(rcText, ptOutput,
				m_strHint, m_strHint.GetLength() , m_refHintColor,
				m_hFont, m_nRowSpace, dwType, NULL);
		}
		else if(false == m_TextBuffer.IsEmpty())
		{
			if(MES_CENTER & m_dwStyle)
			{
				szText = m_pShareInfo->pRenderEngine->GetTextSize(rcText, m_TextBuffer,
					m_TextBuffer.GetLenght(), m_hFont, 0, dwType);
				rcText.left += MAX(0, (rcText.right - rcText.left - szText.cx) / 2);
				rcText.right -= MAX(0, (rcText.right - rcText.left - szText.cx) / 2);
				ptOutput.x = rcText.left;
			}
			//首先绘制0 到选中索引的位置
			m_pShareInfo->pRenderEngine->OnTextOut(rcText, ptOutput,
				m_TextBuffer, nSelStart , m_refTextColor,
				m_hFont, m_nRowSpace, dwType, NULL);

			m_pShareInfo->pRenderEngine->OnTextOut(rcText, ptOutput,
				&m_TextBuffer[nSelStart], nSelLenght , m_refSelTextColor,
				m_hFont, m_nRowSpace, dwType, m_refSelBkColor);

			//最后绘制索引后面的文字
			m_pShareInfo->pRenderEngine->OnTextOut(rcText, ptOutput,
				&m_TextBuffer[nSelStart + nSelLenght] ,
				m_TextBuffer.GetLenght() - (nSelStart + nSelLenght) , 
				m_refTextColor, m_hFont, m_nRowSpace, dwType, NULL);
		}

		OnDrawCaret(rcUpdate);
		
	}

	void CEditUI::OnDrawCaret(const RECT& rcUpdate)
	{
		RECT rcCaret = { 0 };
		if (FALSE == m_CaretInfo.Visible || FALSE == m_CaretInfo.Showing) return;

		rcCaret.left = m_rcContent.left + m_CaretInfo.Point.x;
		rcCaret.top = m_rcContent.top + m_CaretInfo.Point.y;
		rcCaret.right = rcCaret.left + m_CaretInfo.Size.cx;
		rcCaret.bottom = rcCaret.top + m_CaretInfo.Size.cy;

		if (FALSE == IsContainRect(rcCaret, m_rcClient)) return;

		m_pShareInfo->pRenderEngine->OnDrawColor(rcCaret, m_refTextColor);
	}

	LRESULT CEditUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT point;
		bool bRet = false;
		DWORD dwType = NULL;
		RECT rcText = {0}, rcDisplay = {0};
		int nEncodeLenght = 0;
		int nTextHeight = 0;
		HWND hWnd = GETHWND(this);
		HFONT hFont = (HFONT)m_hFont;
		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				MUITRACE(_T("CEditUI::WM_LBUTTONDOWN"));
				OnLeftButtonDown(hWnd, message, wParam, lParam);
			}break;
		case WM_LBUTTONUP:
			{

			}break;
		case WM_MOUSEMOVE:
			{
				OnMouseMove(hWnd, message, wParam, lParam);
			}break;
		case WM_MOUSEWHEEL:
			{
				if(false == m_bEnabled || !(m_dwStyle & MES_MULTILINE))
				{
					return false;
				}
			}break;
		case WM_KEYDOWN:
			{
				OnKeyDown(hWnd, message, wParam, lParam);
			}break;
		case WM_CHAR:
			{
				OnChat(hWnd, message, wParam, lParam);
			}break;
		case WM_IME_CHAR:
			{
				//处理WM_CHAR就可以了
				/*m_strText += (TCHAR)msg.wParam;
				this->Invalidate();
				TRACE(_T("WM_IME_CHAR = %c"), (TCHAR)msg.wParam);*/
			}break;
		case WM_SETCURSOR:
			{
				if(m_bEnabled)
				{
					SetCursor(LoadCursor(NULL, IDC_IBEAM));
				}
				return false;
			}break;
		case WM_MOUSEHOVER:
			{

			}break;
		case WM_TIMER:
		{
			if (CaretTimerId != wParam) break;

			//GetClientDisplayRect(rcDisplay);
			m_CaretInfo.Showing = !m_CaretInfo.Showing;
			this->Invalidate();
			return 0;
		}break;
		case WM_SETFOCUS:
			{
				MUITRACE(_T("CEditUI::WM_SETFOCUS"));
				if(0x000000F0 & GetStyle() && NULL == m_hIMC)
				{
					m_hIMC = ImmAssociateContext(m_pShareInfo->hWnd, NULL);
				}
				m_CaretInfo.Visible = TRUE;
				m_CaretInfo.Showing = FALSE;
				this->SetTimer(CaretTimerId, CaretTimeOut);
				//return 0;
			}break;
		case WM_KILLFOCUS:
			{
				//DestroyCaret();
				MUITRACE(_T("CEditUI::WM_KILLFOCUS"));
				if(m_hIMC)
				{
					ImmAssociateContext(m_pShareInfo->hWnd, m_hIMC);
				}
				m_hIMC = NULL;
				m_bShowCaret = false;
				m_CaretInfo.Visible = FALSE;
				this->KillTimer(CaretTimerId);
				//return 0;
			}break;
		case WMU_MOUSEENTER://鼠标第一次进入
		case WM_MOUSELEAVE://鼠标离开
			{
				
			}break;
		default:
			break;
		}

		return __super::WndProc(message, wParam, lParam);
	}

	LRESULT CEditUI::OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		//DWORD dwType = NULL;
		//RECT rcText = m_rcContent, rcDisplay = {0};
		int nEncodeLenght = 0;
		int nTextHeight = 0;
		HGLOBAL hMem = NULL;
		TCHAR * strText = NULL;
		
		//键按下消息
		if(FALSE == m_bEnabled) return false;

		switch ((TCHAR)wParam)
		{
		case VK_LEFT:
			{
				nEncodeLenght = check_pre_text_encode_lenght(m_TextBuffer, m_nSelStart) ;
				m_nSelStart = MAX(0, m_nSelStart - nEncodeLenght);
				if(0 != m_nSelLenght)
				{
					m_nSelLenght = 0;
					Invalidate();
				}
				this->SetCaret(NULL);

			}break;
		case VK_RIGHT:
			{
				nEncodeLenght = check_encode_lenght(&m_TextBuffer[m_nSelStart]);
				m_nSelStart = MAX(0, m_nSelStart + nEncodeLenght);

				if(0 != m_nSelLenght)
				{
					m_nSelLenght = 0;
					Invalidate();
				}
				this->SetCaret(NULL);
			}break;
		case VK_UP:
			{
				pt = m_CaretInfo.Point;
				nTextHeight = m_pShareInfo->pRenderEngine->GetTextHeight(m_hFont);
				pt.y -= (nTextHeight / 2) - m_nRowSpace;
				
				if(0 != m_nSelLenght)
				{
					m_nSelLenght = 0;
					Invalidate();
				}
				this->SetCaret(&pt);

			}break;
		case VK_DOWN:
			{
				pt = m_CaretInfo.Point;
				nTextHeight = m_pShareInfo->pRenderEngine->GetTextHeight(m_hFont);
				pt.y += (nTextHeight * 3 / 2) + m_nRowSpace;

				if(0 != m_nSelLenght)
				{
					m_nSelLenght = 0;
					Invalidate();
				}
				this->SetCaret(&pt);

			}break;
		case VK_HOME: // HOME 键
			break;
		case VK_END:  // END 键
			break;
		case VK_BACK: // 退格键
			{
				//WM_CHAR处理
			}break;
		case VK_DELETE: 
			{  // 删除键
			} break;
		case _T('X'):
			{
				if(0 == m_nSelLenght) break;
				if(GetKeyState(VK_CONTROL) > 0) break;//Ctrl没有按下
				if(MES_PASSWORD & m_dwStyle) break;

				MUITRACE(_T("Ctrl + X KeyDown"));
				int nStart = m_nSelStart, nLenght = m_nSelLenght;

				if(0 > nLenght)
				{
					nStart += nLenght;
					nLenght = -nLenght;
				}

				hMem = GlobalAlloc(GMEM_MOVEABLE, (nLenght + 1) * sizeof(TCHAR));
				strText = (TCHAR*) GlobalLock(hMem);
				memcpy(strText, &m_TextBuffer[nStart], nLenght * sizeof(TCHAR));
				strText[nLenght] = _T('\0');
				GlobalUnlock(hMem);
				
				OpenClipboard(m_pShareInfo->hWnd);
				EmptyClipboard();
				SetClipboardData(CF_TEXT, strText);
				CloseClipboard();

				m_TextBuffer.DeleteText(m_nSelStart,m_nSelLenght);
				m_nSelLenght = 0;
				m_nSelStart = nStart;

				this->SetCaret(NULL);
				this->SetItem(m_rcItem, true);
				this->Invalidate();

			}break;
		case _T('C'):
			{
				if(0 == m_nSelLenght) break;
				if(GetKeyState(VK_CONTROL) > 0) break;//Ctrl没有按下
				if(MES_PASSWORD & m_dwStyle) break;

				MUITRACE(_T("Ctrl + C KeyDown"));
				int nStart = m_nSelStart, nLenght = m_nSelLenght;

				if(0 > nLenght)
				{
					nStart += nLenght;
					nLenght = -nLenght;
				}

				hMem = GlobalAlloc(GMEM_MOVEABLE, (nLenght + 1) * sizeof(TCHAR));
				strText = (TCHAR*) GlobalLock(hMem);
				memcpy(strText, &m_TextBuffer[nStart], nLenght * sizeof(TCHAR));
				strText[nLenght] = _T('\0');
				GlobalUnlock(hMem);
				
				OpenClipboard(m_pShareInfo->hWnd);
				EmptyClipboard();
				SetClipboardData(CF_TEXT, strText);
				CloseClipboard();

			}break;
		case _T('V'):
			{
				if(GetKeyState(VK_CONTROL) > 0) break;//Ctrl没有按下
				if(MES_PASSWORD & m_dwStyle) break;

				MUITRACE(_T("Ctrl + V KeyDown"));
				OpenClipboard(m_pShareInfo->hWnd);
				if(IsClipboardFormatAvailable(CF_TEXT))//存在文本
				{
					hMem = GetClipboardData(CF_TEXT);
					strText = (TCHAR *)GlobalLock(hMem);

					if(strText && _T('\0') != strText[0])
					{
						//settext
						if(0 > m_nSelLenght)
						{
							m_nSelStart += m_nSelLenght;
							m_nSelLenght = -m_nSelLenght;
						}

						m_TextBuffer.InsterText(m_nSelStart, m_nSelLenght, strText);
						
						m_nSelLenght = 0;
						m_nSelStart += _tcslen(strText);

						this->SetCaret(NULL);
						this->SetItem(m_rcItem, true);
						this->Invalidate();
					}

					GlobalUnlock(hMem);
				}
				CloseClipboard();
			}break;
		}
		return 0;
	}

	LRESULT CEditUI::OnChat(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		bool bRet = false;
		DWORD dwType = NULL;
		RECT rcText = {0}, rcDisplay = {0};
		int nEncodeLenght = 0;
		int nTextHeight = 0;
		HFONT hFont = m_hFont;

		if(false == m_bEnabled || MES_READONLY & m_dwStyle) return false;

		MUITRACE(_T("CEditUI::OnChat = %c, 0x%x"),(TCHAR)wParam,(TCHAR)wParam);
		switch((TCHAR)wParam)
		{
		case _T('\b'):// 退格键
			{
				//处理文字缓冲
				MUITRACE(_T("m_nSelStart = %d, m_nSelLenght= %d"), m_nSelStart, m_nSelLenght);

				//没有可以删除的文字
				if(0 == m_nSelStart && 0 == m_nSelLenght) return false;

				if(0 == m_nSelLenght)
				{
					m_nSelLenght = check_pre_text_encode_lenght(m_TextBuffer, m_nSelStart) ;
					m_nSelStart -= m_nSelLenght;
				}
				else if(0 > m_nSelLenght)
				{
					m_nSelStart += m_nSelLenght;
					m_nSelLenght = -m_nSelLenght;
				}

				m_nSelStart = MAX(0, m_nSelStart);
				m_TextBuffer.DeleteText(m_nSelStart, m_nSelLenght);
				m_nSelLenght = 0;

				//处理光标位置

				this->SetCaret(NULL);

			}break;
		default:
			{
				if(0 > m_nSelLenght)
				{
					m_nSelStart += m_nSelLenght;
					m_nSelLenght = -m_nSelLenght;
				}

				m_TextBuffer.InsterText(m_nSelStart, m_nSelLenght,(TCHAR)wParam);
				m_nSelLenght = 0;
				m_nSelStart++;

				this->SetCaret(NULL);

			}break;
		}

		this->SendNotify(EnumNotify::TextChange, wParam, message);
		this->SetItem(m_rcItem, true);
		this->Invalidate();

		return 0;
	}

	LRESULT CEditUI::OnLeftButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT Point = { 0 };
		bool bRet = false;
		DWORD dwType = NULL;
		RECT rcText = {0}, rcDisplay = {0}, rcFixed = {0};
		int nEncodeLenght = 0;
		int nTextHeight = 0;
		
		MUITRACE(_T("m_dwState = %x"), m_dwState);
		if(FALSE == (STATE_FOCUS & m_dwState) || false == m_bEnabled)
		{
			//有焦点和启用才能设置光标
			return bRet;
		}

		Point.x = GET_X_LPARAM(lParam);
		Point.y = GET_Y_LPARAM(lParam);
		m_nSelLenght = 0;
		this->SetCaret(&Point);
		this->Invalidate();
		MUITRACE(_T("TestTextIndex = %d"), m_nSelStart);

		return 0;
	}

	LRESULT CEditUI::OnLeftButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		bool bRet = false;
		DWORD dwType = NULL;
		RECT rcText = {0}, rcDisplay = {0}, rcFixed = {0};
		int nEncodeLenght = 0;
		int nTextHeight = 0;

		if(STATE_PUSHED & m_dwState)
		{
			
		}

		return 0;
	}

	LRESULT CEditUI::OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT Point;
		bool bRet = false;
		DWORD dwType = NULL;
		RECT rcText = {0}, rcDisplay = {0}, rcFixed = {0};
		int nEncodeLenght = 0;
		int nTextHeight = 0;

		if(FALSE == (STATE_PUSHED & m_dwState) || FALSE == (STATE_FOCUS & m_dwState))
		{
			return false;
		}

		Point.x = GET_X_LPARAM(lParam);
		Point.y = GET_Y_LPARAM(lParam);

		if(MES_MULTILINE & m_dwStyle)
		{
			dwType |= TOS_MULTILLINE;
		}
		if(MES_CENTER & m_dwStyle)
		{
			dwType |= TOS_CENTER;
		}
		if(MES_LINEFREE & m_dwStyle)
		{
			dwType |= TOS_AUTOLINEFREE;
		}

		rcText = m_rcContent;
		IndentRect(&rcText, &m_rcTextPadding);//内缩
		OffsetRect(&rcText, -m_szScrollOffset.cx, -m_szScrollOffset.cy);//滚动偏移

		int nIndex = m_pShareInfo->pRenderEngine->TestTextIndex(rcText,
			Point, m_TextBuffer, m_TextBuffer.GetLenght(),
			m_hFont, m_nRowSpace, dwType);

		if(m_nSelLenght != nIndex - m_nSelStart)
		{
			m_nSelLenght = nIndex - m_nSelStart;
			this->Invalidate();
		}

		Point.x -= m_rcContent.left;
		Point.y -= m_rcContent.top;
		
		m_CaretInfo.Point = Point;

		return 0;
	}

	bool CEditUI::SetCaret(POINT * ptMouse)
	{
		RECT rcText = m_rcContent;
		DWORD dwType = NULL;
		RECT rcDisplay = {0}, rcFixed = {0}, rcIndent = {0};
		POINT Point;

		m_bShowCaret = true;
		//处理光标位置
		if(MES_MULTILINE & m_dwStyle)
		{
			dwType |= TOS_MULTILLINE;
		}
		if(MES_CENTER & m_dwStyle)
		{
			dwType |= TOS_CENTER;
		}
		if(MES_LINEFREE & m_dwStyle)
		{
			dwType |= TOS_AUTOLINEFREE | TOS_MULTILLINE;
		}

		
		MUITRACE(_T("SetCaret::rcText.left=%d; top = %d, right=%d, bottom=%d"), 
			rcText.top, rcText.top, rcText.right, rcText.bottom);
		MUITRACE(_T("SetCaret::rcPadd.left=%d; top = %d, right=%d, bottom=%d"), 
			m_rcTextPadding.top, m_rcTextPadding.top, m_rcTextPadding.right, m_rcTextPadding.bottom);

		if(IndentRect(&rcText, &m_rcTextPadding))
		{
			MUITRACE(_T("SetCaret::rcText.left=%d; top = %d, right=%d, bottom=%d"),
				rcText.top, rcText.top, rcText.right, rcText.bottom);
			if(ptMouse)//根据点击位置求光标
			{
				Point = *ptMouse;
				MUITRACE(_T("SetCaret::ptMouse.x = %d; ptMouse.y = %d"), Point.x, Point.y);
				m_nSelStart = m_pShareInfo->pRenderEngine->TestTextIndex(rcText,
					Point, m_TextBuffer, m_TextBuffer.GetLenght(),
					m_hFont, m_nRowSpace, dwType);
				MUITRACE(_T("SetCaret::Point.x = %d; Point.y = %d"), Point.x, Point.y);
				Point.x -= m_rcContent.left;
				Point.y -= m_rcContent.top;
			}
			else
			{
				//根据文本索引求光标
				Point = m_pShareInfo->pRenderEngine->GetTextPos(rcText, m_nSelStart, m_TextBuffer,
					m_hFont, m_nRowSpace, dwType);
			}

			m_CaretInfo.Point = Point;

			return true;
		}

		return false;//光标是否有显示的位置
	}
}