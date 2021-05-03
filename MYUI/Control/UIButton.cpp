
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

	const CMuiString CButtonUI::g_strClassName(_T("ButtonUI"));

	CMuiString CButtonUI::GetClassName() const
	{
		return CButtonUI::g_strClassName;
	}

	LRESULT CButtonUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT point ;
		switch(message)
		{
		case WMU_LBUTTONCLICK:
			{
				SendNotify(EnumNotify::ClickItem, wParam, lParam);
			}break;
		case WM_MOUSEMOVE:
			{
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);
			}break;
		case WMU_MOUSEENTER:
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
		return __super::WndProc(message, wParam, lParam);
	}
}