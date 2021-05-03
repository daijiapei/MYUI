
#include "UIWidget.h"

namespace MYUI
{
	CWidgetUI::CWidgetUI()
	{
		//m_hBorderPen = CreatePen(PS_SOLID,1 ,RGB(0,0,0));
	}

	CWidgetUI::~CWidgetUI()
	{
	}

	const CMuiString CWidgetUI::g_strClassName(_T("WidgetUI"));
	const CMuiString CWidgetUI::g_strPropName(_T("MYUI::CWidgetUI::Pointer"));

	CMuiString CWidgetUI::GetClassName() const
	{
		if (m_strClassName.IsEmpty())
		{
			return CWidgetUI::g_strClassName;
		}
		
		return m_strClassName;
	}

	void CWidgetUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if (0 == _tcsicmp(strItem, _T("ClassName")))
		{
			m_strClassName = strValue;
		}
		return __super::SetAttribute(strItem, strValue);
	}

	void CWidgetUI::SetVisible(bool bVisible)
	{
		if (::IsWindow(m_hWidget))
		{
			m_bVisible = ::IsWindowVisible(m_hWidget);

			if (bVisible != m_bVisible)
			{
				::ShowWindow(m_hWidget, bVisible);
			}
		}

		return __super::SetVisible(bVisible);
	}

	bool CWidgetUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		RECT rcPos;
		__super::SetItem(rcItem, bMustUpdate);

		GetClientFixedRect(rcPos);

		MoveWindow(m_hWidget, rcPos.left, rcPos.top, 
			rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, TRUE);
		
		return true;
	}

	bool CWidgetUI::OnPaint(const RECT& rcUpdate)
	{
		return IsVisible() && m_pShareInfo;
	}

	void CWidgetUI::OnScrollBarMove(CScrollBarUI* pSender, int nShift)
	{
		return __super::OnScrollBarMove(pSender, nShift);
	}

	SIZE CWidgetUI::GetContentSize()
	{
		return GetValidSize();
	}

	void CWidgetUI::OnAttach(HWND hNewWnd)
	{
		//m_hWidget = ::CreateWindowEx(m_dwStyleEx, m_strClassName, m_strText,
		//	rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		//	hNewWnd, NULL, m_hInstance, this);
		::SetProp(m_hWidget, g_strPropName, this);
		m_pfnSrcWndProc = (WNDPROC)::GetWindowLongPtr(m_hWidget, GWL_WNDPROC);
		::SetWindowLongPtr(m_hWidget, GWL_WNDPROC, (LONG)CWidgetUI::FrameWndProc);
	}

	void CWidgetUI::OnDetach(HWND hNewWnd)
	{
		if (::IsWindow(m_hWidget))
		{
			::DestroyWindow(m_hWidget);
		}
		m_hWidget = NULL;
	}

	LRESULT CWidgetUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
		{

		}break;
		default:
		{
		}break;
		}

		return ::CallWindowProc(m_pfnSrcWndProc, m_hWidget, message, wParam, lParam);
	}

	LRESULT CALLBACK CWidgetUI::FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		CWidgetUI* pWidget = (CWidgetUI*)::GetProp(hwnd, g_strPropName);

		return pWidget->WndProc(message, wParam, lParam);
	}
}