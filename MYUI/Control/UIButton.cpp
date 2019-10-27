
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
				//���hWnd = NULL, ��˵������ؼ���Ϊ��һ��Item�����ܷ���Notify
				//��Ϊitem? ����listBox�е�ÿһ�����һ��Item����������listBox
				//��ĳһ���ô��Ӧ����item����֪ͨ��Ӧ����listbox����֪ͨ
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