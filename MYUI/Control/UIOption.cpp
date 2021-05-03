
#include "UIOption.h"

namespace MYUI
{
	COptionUI::COptionUI()
	{
		//m_hBorderPen = CreatePen(PS_SOLID,1 ,RGB(0,0,0));
	}

	COptionUI::~COptionUI()
	{
	}

	const CMuiString COptionUI::g_strClassName(_T("OptionUI"));

	CMuiString COptionUI::GetClassName() const
	{
		return COptionUI::g_strClassName;
	}
	
	LRESULT COptionUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		bool bRet = false;
		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				if(STATE_CHECKED & GetState())
				{
					if(true == m_strGroupName.IsEmpty())
					{
						//没有加入组,
						//有加入组的话,重复点击不取消选中
						CallWndProc(WMU_SETCHECK, FALSE, 0);
					}
				}
				else
				{
					CallWndProc(WMU_SETCHECK, TRUE, 0);
				}

				Invalidate();
			}break;
		case WMU_GETGROUP:
			{
				_tcscpy((TCHAR *)lParam, m_strGroupName);
				return 0;
			}break;
		case WMU_SETCHECK:
			{
				if(STATE_CHECKED & GetState())
				{
					RemoveState(STATE_CHECKED);
					SendNotify(EnumNotify::CheckItem, wParam, TRUE);//原来的状态
				}
				else
				{
					AddState(STATE_CHECKED);
					SendNotify(EnumNotify::CheckItem, wParam, FALSE);//原来的状态
				}
				this->Invalidate();
				return 0;
			}break;
		default:
			break;
		}
		
		return __super::WndProc(message, wParam, lParam);
	}

	void COptionUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("groupname"))) 
		{
			SetGroupName(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("foreNormalImage")))
		{
			SetForeNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("foreUnknowImage")))
		{
			SetForeUnknowImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("foreCheckImage")))
		{
			SetForeCheckImage(strValue);
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
		return ;
	}

	void COptionUI::PaintStatusImage(const RECT& rcUpdate)
	{
		LPCTSTR strImage = m_strForeNormalImage;
		//先绘制Lable作为背景
		__super::PaintStatusImage(rcUpdate);

		//再绘制前景图片

		//通过状态优先级，来判断显示哪一张图片
		if(m_dwState & STATE_CHECKED)
		{
			if(!m_strForeCheckImage.IsEmpty())
			{
				strImage = m_strForeCheckImage;
			}
		}
		else
		{
			if((m_dwState & STATE_UNKNOW) && !m_strForeUnknowImage.IsEmpty())
			{
				strImage = m_strForeUnknowImage;
			}
		}

		if(_T('\0') != strImage[0])
		{
			m_pShareInfo->pRenderEngine->OnDrawImage(m_rcClient, strImage);
			return;
		}
	}

	void COptionUI::SetGroupName(LPCTSTR strGroup)
	{
		m_strGroupName = strGroup;
	}

	bool COptionUI::SetCheck(bool bCheck)
	{
		return false;
	}

	bool COptionUI::GetCheck()
	{
		return STATE_CHECKED & GetState();
	}

	LPCTSTR COptionUI::GetGroupName() const
	{
		return m_strGroupName;
	}

	//bAttach = false移除分组属性，bAttach = true 加入某个分组
	bool COptionUI::AttachToGroup(bool bAttach)
	{
		if(NULL == m_pShareInfo) return false;
		if(true == m_strGroupName.IsEmpty()) return false;

		if(true == bAttach)
		{
			
		}
		else
		{
		}
		return false;
	}

	void COptionUI::OnAttach(HWND hNewWnd)
	{
		return;
	}

	void COptionUI::OnDetach(HWND hOldWnd)
	{
		return;
	}

	void COptionUI::SetState(DWORD dwState)
	{
		if(STATE_CHECKED & GetState())
		{
			dwState &= (~STATE_UNKNOW);
		}

		return __super::SetState(dwState);
	}

	void COptionUI::SetForeUnknowImage(LPCTSTR strImage)
	{
		m_strForeUnknowImage = strImage;
	}

	LPCTSTR COptionUI::GetForeUnknowImage() const
	{
		return m_strForeUnknowImage;
	}

	void COptionUI::SetForeNormalImage(LPCTSTR strImage)
	{
		m_strForeNormalImage = strImage;
	}

	LPCTSTR COptionUI::GetForeNormalImage() const
	{
		return m_strForeNormalImage;
	}

	void COptionUI::SetForeCheckImage(LPCTSTR strImage)
	{
		m_strForeCheckImage = strImage;
	}

	LPCTSTR COptionUI::GetForeCheckImage() const
	{
		return m_strForeCheckImage;
	}
}