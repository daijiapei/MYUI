
#include "UIText.h"

namespace MYUI
{
	CTextUI::CTextUI()
		: m_bShowHtml(false)
	{
		//m_hBorderPen = CreatePen(PS_SOLID,1 ,RGB(0,0,0));
	}

	CTextUI::~CTextUI()
	{
	}

	CMuiString CTextUI::g_strClassName(_T("TextUI"));

	CMuiString CTextUI::GetClassName() const
	{
		return CTextUI::g_strClassName;
	}

	void CTextUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(_T("showHtml"), strItem))
		{
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			SetShowHtml(CheckTrue(strValue));
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
		return ;
	}

	void CTextUI::SetShowHtml(bool bShow)
	{
		m_bShowHtml = bShow;
	}

	bool CTextUI::IsShowHtml() const
	{
		return m_bShowHtml;
	}

	void CTextUI::PaintText(const RECT& rcItem, const RECT& rcPaint)
	{
		RECT rcDraw = rcItem;
		if(false == m_bShowHtml)
		{
			__super::PaintText(rcDraw, rcPaint);
		}
		else
		{
			if(IndentRect(&rcDraw, &m_rcTextPadding))
			{
				m_pShareInfo->pRenderEngine->OnDrawHtmlText(rcDraw, m_strText,
					m_pShareInfo->FontArray, m_refTextColor);
			}
		}
		return;
	}

	LRESULT CTextUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		bool bRet = false;
		switch(message)
		{
		case WM_LBUTTONUP:
			{

			}break;
		case WM_MOUSEMOVE:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
			}break;
		case WM_SETCURSOR:
			{
			}break;
		default:
			break;
		}
		return __super::WndProc(hWnd, message, wParam, lParam);
	}

}