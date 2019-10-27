
#include "UIButton.h"

namespace MYUI
{
	CButtonUI::CButtonUI()
	{
		//m_hBorderPen = CreatePen(PS_SOLID,1 ,RGB(0,0,0));
	}

	CButtonUI::~CButtonUI()
	{
	}

	CMuiString CButtonUI::g_strClassName(_T("ButtonUI"));

	CMuiString CButtonUI::GetClassName() const
	{
		return CButtonUI::g_strClassName;
	}

	LRESULT CButtonUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT point ;
		switch(message)
		{
		case WM_LBUTTONCLICK:
			{
				//如果hWnd = NULL, 则说明这个控件作为了一个Item，不能发出Notify
				//何为item? 比如listBox中的每一项，就是一个Item，如果点击了listBox
				//的某一项，那么不应该由item发出通知，应该由listbox发出通知
				if(true == IsEnabled()) 
				{
					SendNotify(!hWnd, EnumNotifyMsg::ClickItem, wParam, lParam);
				}
			}break;
		case WM_MOUSEMOVE:
			{
				point.x = LOWORD(lParam);
				point.y = HIWORD(lParam);
			}break;
		case WM_MOUSEENTER:
			{
				//ShowCursor(TRUE);
				//ret = SetCursor(LoadCursor(NULL,IDC_CROSS));
			}break;
		case WM_SETCURSOR:
			{
				if(true == IsEnabled())
				{
					SetCursor(LoadCursor(NULL,IDC_HAND));
					return 0;
				}
			}break;
		case WM_MOUSELEAVE:
			{
				//SetCursor(LoadCursor(NULL, IDC_ARROW));
			}break;
		default:
			break;
		}
		return __super::WndProc(hWnd, message, wParam, lParam);
	}
}