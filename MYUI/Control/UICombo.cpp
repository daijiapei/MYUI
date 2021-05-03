
#include "UICombo.h"
#include "..\\Core\\UIWindow.h"

namespace MYUI
{
	CComboUI::CComboUI()
	{
		m_nHotBorderSize = 0;
		m_refHotBorderColor = NULL;
	}

	CComboUI::~CComboUI()
	{
	}

	const CMuiString CComboUI::g_strClassName(_T("ComboUI"));

	CMuiString CComboUI::GetClassName() const
	{
		return CComboUI::g_strClassName;
	}

	LRESULT CComboUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		CControlUI * pControl = NULL;
		bool bNeedUpdate = false;
		POINT pt = {0,0};
		bool bSelect = false;
		RECT rcButton;

		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				pt.x = (short)GET_X_LPARAM(lParam);
				pt.y = (short)GET_Y_LPARAM(lParam);

				rcButton = m_rcRawItem;
				rcButton.left = rcButton.right - m_rcTextPadding.right;

				if(FALSE == PointInRect(pt, rcButton))
				{
					//让消息流入__super中
					break;
				}

				//if(TRUE == PointInRect(pt, m_rcEdit))
				{
					/*pt.x -= m_rcEdit.left;
					pt.y -= m_rcEdit.top;
					msgTmp.lParam = MAKELONG(pt.x, pt.y);*/
					//m_pEdit->HandleMessage(msgTmp,notifyTmp);
				}
			}break;
		case WM_LBUTTONUP:
			{
			}
		case WM_MOUSEMOVE:
			{
				pt.x = (short)GET_X_LPARAM(lParam);
				pt.y = (short)GET_Y_LPARAM(lParam);

				rcButton = m_rcClient;
				OffsetRect(&rcButton, -rcButton.left, -rcButton.top);
				//rcButton.left = m_rcEdit.right;

				pt.x -= m_rcClient.left;
				pt.y -= m_rcClient.top;

			}break;
		case WM_MOUSEHOVER:
			{
			}break;
		case WM_MOUSELEAVE:
			{
			}break;
		case WM_MOUSEWHEEL:
			{
				//如果允许鼠标轮滚动，则返回非零值；否则返回0
			}break;
		case WM_RBUTTONDOWN:
			{
			}break;
		case WM_RBUTTONUP:
			{
			}break;
		case WM_KEYDOWN:
			{
				TCHAR code = (TCHAR)wParam;
				{
					switch (code)
					{
					case VK_RETURN:
						{
						}break;
					default:
						break;
					}
				}
			}break;
		default:
			break;
		}

		return __super::WndProc(message, wParam, lParam);
	}

}