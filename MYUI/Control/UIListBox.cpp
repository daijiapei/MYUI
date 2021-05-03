
#include "UIListBox.h"

namespace MYUI
{
	CListBoxUI::CListBoxUI()
		: m_pHotControl(NULL)
		, m_pPushedControl(NULL)
		, m_pSelectControl(NULL)
	{
		m_bControl = true;
		m_bCentered = false;
	}

	CListBoxUI::~CListBoxUI()
	{
	}

	const CMuiString CListBoxUI::g_strClassName(_T("ListBoxUI"));

	CMuiString CListBoxUI::GetClassName() const
	{
		return CListBoxUI::g_strClassName;
	}

	CControlUI * CListBoxUI::GetItemByPoint(POINT &Point)
	{
		POINT ptThis = Point;//保存起来
		RECT rcChildItem;
		CControlUI * target = NULL;
		int nCount = m_Items.GetSize();
		//TRACE(_T("m_Items.GetSize() = %d"), nCount);
		//注意，因为绘制时，是按顺序绘制的，如果控件位置发生重叠
		//先绘制的控件会被后绘制的控件所覆盖
		//所以我们计算point指向的控件时，要进行反方向计算
		//在其他计算函数中，也要注意同样的问题，计算方向一定要与绘制方式相反
		while(nCount--)//倒序轮询
		{
			CControlUI * pControl = (CControlUI *)m_Items[nCount];
			if(false == pControl->IsVisible())
			{
				continue;
			}
			rcChildItem = pControl->GetItem();
			//pt.x += m_szScrollOffset.cx;
			//pt.y += m_szScrollOffset.cy;

			if(TRUE == PointInRect(Point, rcChildItem))
			{
				Point.x -= rcChildItem.left;
				Point.y -= rcChildItem.top;
				target = pControl;
				break;
			}
			Point = ptThis;//不能够获取成功，要还原参数，避免用户误会
		}
		//TRACE(_T("nCount = %d"), nCount);
		return target;
	}

	LRESULT CListBoxUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		CControlUI * pControl = NULL,* pTmpControl = NULL;

		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				if(IsEnabled() && (pControl = GetItemByPoint(pt)))
				{
					if(m_pPushedControl)
					{
						m_pPushedControl->RemoveState(STATE_PUSHED);
					}
					m_pPushedControl = pControl;
					m_pPushedControl->AddState(STATE_PUSHED);

					pControl->CallWndProc(message, wParam, MAKELONG(pt.x, pt.y));
				}
			}break;
		case WM_LBUTTONUP:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);

				if(IsEnabled() && (pControl = GetItemByPoint(pt))) 
				{
					if(STATE_PUSHED & pControl->GetState())
					{
						if((MLBXS_MULTI & m_dwStyle) && 0 > GetKeyState(VK_CONTROL))
						{
							//多行选择属性 + Ctrl键按下
							if(STATE_SELECT & pControl->GetState())
							{
								pControl->RemoveState(STATE_SELECT);
							}
							else
							{
								pControl->AddState(STATE_SELECT);
							}
						}
						else
						{
							this->ClearSelect();
							pControl->AddState(STATE_SELECT);
						}

						m_pSelectControl = pControl;

						SendNotify(SelectItem,!!(STATE_SELECT & pControl->GetState()), (LPARAM)pControl);
					}

					pControl->CallWndProc(message, wParam, MAKELONG(pt.x, pt.y));
					pControl->RemoveState(STATE_PUSHED);

					if(m_pPushedControl && pControl != m_pPushedControl)
					{
						m_pPushedControl->RemoveState(STATE_PUSHED);
					}

					m_pPushedControl = NULL;
				}
			}break;
		case WM_LBUTTONDBLCLK:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				if(pControl = GetItemByPoint(pt)) 
				{
					SendNotify(EnumNotify::ActiveItem, this->Find(pControl), (LPARAM)pControl);
				}
			}break;
		case WM_MOUSEMOVE:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				pControl = GetItemByPoint(pt);
				if(pControl != m_pHotControl)
				{
					//当鼠标移动时，有可能离开一个控件，进入另一个控件，在此进行处理
			
					//切换热点
					pTmpControl = m_pHotControl;
					m_pHotControl = pControl;
					//逻辑：先离开一个房间，才能进入另外一个房间
					//先发出离开通知，
					if(NULL != pTmpControl)
					{
						pTmpControl->CallWndProc(WM_MOUSELEAVE, 0, 0);
					}

					//再发出进入通知
					if(NULL != pControl)
					{
						pControl->CallWndProc(WMU_MOUSEENTER, wParam, MAKELONG(pt.x, pt.y));
					}
				}
			}break;
		case WM_MOUSEHOVER:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				if(pControl = GetItemByPoint(pt)) 
				{
					pControl->CallWndProc(WM_MOUSEHOVER, wParam, MAKELONG(pt.x, pt.y));
				}
			}break;
		case WM_MOUSELEAVE:
			{
				if(NULL != m_pHotControl)
				{
					pTmpControl = m_pHotControl;
					m_pHotControl = NULL;
					pTmpControl->CallWndProc(WM_MOUSELEAVE, 0, 0);
				}
			}break;
		case WM_KEYDOWN:
			{
				switch (wParam)
				{
				case VK_UP:
					{
					}break;
				case VK_DOWN:
					{
					}break;
				case VK_RETURN:
					{
					}break;
				default:
					break;
				}
			}break;
		case WM_RBUTTONDOWN:
			{
			}break;
		default:
			break;
		}
		return __super::WndProc(message, wParam, lParam);
	}

	bool CListBoxUI::SetSingleSelect(int index)
	{
		CControlUI * pControl = (CControlUI *)m_Items.GetAt(index);

		if(NULL == pControl)
		{
			return false;
		}

		ClearSelect();

		m_pSelectControl = pControl;
		pControl->SetState(STATE_SELECT);
		return true;
	}

	bool CListBoxUI::AddSelect(int index)//MLBXS_MULTI下添加一个STATE_SELECT
	{
		CControlUI * pControl = NULL;
		if(NULL == (MLBXS_MULTI & m_dwStyle)) return false;
		pControl = (CControlUI *)m_Items.GetAt(index);

		MUIASSERT(pControl && "子控件下标越界拉！！");
		if(NULL == pControl) return false;

		pControl->SetState(STATE_SELECT);
		return true;
	}

	bool CListBoxUI::RemoveSelect(int index)//移除控件的STATE_SELECT
	{
		CControlUI * pControl = NULL;
		pControl = (CControlUI *)m_Items.GetAt(index);

		MUIASSERT(pControl && "子控件下标越界拉！！");
		if(NULL == pControl) return false;

		pControl->RemoveState(STATE_SELECT);
		if(pControl == m_pSelectControl) m_pSelectControl = NULL;
		return true;
	}

	CControlUI *  CListBoxUI::GetSelect()
	{
		MUIASSERT(NULL == (MLBXS_MULTI & m_dwStyle));

		if(NULL == (MLBXS_MULTI & m_dwStyle))
		{
			return m_pSelectControl;
		}
		return NULL;
	}

	void CListBoxUI::SetStyle(DWORD dwStyle)
	{
		DWORD dwNew = MLBXS_MULTI & dwStyle;
		DWORD dwOld = MLBXS_MULTI & m_dwStyle;

		if(dwOld != dwNew)
		{
			ClearSelect();
		}
		__super::SetStyle(dwStyle);
	}

	bool CListBoxUI::AddAt(CControlUI* pControl, int nIndex)
	{
		//callback函数
		if(STATE_SELECT & pControl->GetState())
		{
			pControl->RemoveState(STATE_SELECT);
		}

		return __super::AddAt(pControl,nIndex);
	}

	bool CListBoxUI::RemoveAt(int nIndex)
	{
		CControlUI * pControl = (CControlUI *)m_Items.GetAt(nIndex);
		if(pControl)
		{
			if(STATE_SELECT & pControl->GetState())
			{
				pControl->RemoveState(STATE_SELECT);
			}
			
			if(m_pSelectControl == pControl) m_pSelectControl = NULL;
		}

		return RemoveAt(nIndex);
	}

	int CListBoxUI::GetSelectCount()
	{
		int nCount = 0;
		for(int i = 0; m_Items.GetSize() > i; i++)
		{
			if(STATE_SELECT & ((CControlUI*)m_Items[i])->GetState())
			{
				++nCount;
			}
		}
		return nCount;
	}

	int CListBoxUI::GetNextSelectIndex(int nBeginIndex)
	{
		int index = -1;

		for(int i = nBeginIndex; m_Items.GetSize() > nBeginIndex; i++)
		{
			if(STATE_SELECT & ((CControlUI *)m_Items[i])->GetState())
			{
				index = i;
				break;
			}
		}

		return index;
	}

	void CListBoxUI::ClearSelect()
	{
		if(MLBXS_MULTI & m_dwStyle)
		{
			for(int i =0; m_Items.GetSize() > i; i ++)
			{
				if(STATE_SELECT & ((CControlUI *)m_Items[i])->GetState())
				{
					((CControlUI *)m_Items[i])->RemoveState(STATE_SELECT);
				}
			}
		}
		else
		{
			if(m_pSelectControl) m_pSelectControl->RemoveState(STATE_SELECT);
		}
		
		m_pSelectControl = NULL;
	}
}