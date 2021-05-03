
#include "UIContainer.h"
#include <setjmp.h>
namespace MYUI
{
	CContainerUI::CContainerUI()
		: m_pHorizontalScrollBar(NULL)
		, m_pVerticalScrollBar(NULL)
	{
		m_szScrollOffset.cx = m_szScrollOffset.cy = 0;
		m_szContent.cx = m_szContent.cy = 0;
		
		EmptyRect(m_rcContent);
	}

	CContainerUI::~CContainerUI()
	{
		if(m_pVerticalScrollBar) delete m_pVerticalScrollBar;
		if(m_pHorizontalScrollBar) delete m_pHorizontalScrollBar;
	}

	void CContainerUI::SetShareInfo(MUISHAREINFO * pShareInfo)
	{
		__super::SetShareInfo(pShareInfo);

		if(m_pVerticalScrollBar)
		{
			m_pVerticalScrollBar->SetShareInfo(pShareInfo);
		}

		if(m_pHorizontalScrollBar)
		{
			m_pHorizontalScrollBar->SetShareInfo(pShareInfo);
		}
	}

	void CContainerUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("vScroll"))) 
		{
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				SetVerticalScrollBar(new CVerticalScrollBarUI());
			}
		}
		else if(0 == _tcsicmp(strItem, _T("hScroll"))) 
		{
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				SetHorizontalScrollBar(new CHorizontailScrollBarUI());
			}
		}
		else if(0 == _tcsicmp(strItem, _T("vScrollActive"))) 
		{
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			SetVerticalScrollBarActive(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("hScrollActive"))) 
		{
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			SetHorizontalScrollBarActive(CheckTrue(strValue));
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
	}


	LPVOID CContainerUI::GetInterface(LPCTSTR strName)
	{
		if (0 == _tcsicmp(strName, _T("CContainerUI")))
		{
			return static_cast<CContainerUI*>(this);
		}
		
		return __super::GetInterface(strName);
	}

	CControlUI * CContainerUI::FindControlByPoint(POINT &Point)
	{
		RECT rcChildItem;
		POINT ptThis = Point;//保存起来
		if(NULL == __super::FindControlByPoint(Point)) return NULL;
		//要检查一下坐标是否落在子控件或者滚动条上

		//先判断是否落在滚动条，
		if(m_pVerticalScrollBar)
		{
			//GetItemClient能够排除交汇区
			rcChildItem = m_pVerticalScrollBar->GetItemClient();
			Point.x -= rcChildItem.left;
			Point.y -= rcChildItem.top;
			if(m_pVerticalScrollBar->IsShowing()
				&& m_pVerticalScrollBar->FindControlByPoint(Point))
			{
				return m_pVerticalScrollBar;
			}
			Point = ptThis;//还原
		}

		if(m_pHorizontalScrollBar)
		{
			//GetItemClient能够排除交汇区
			rcChildItem = m_pHorizontalScrollBar->GetItemClient();
			Point.x -= rcChildItem.left;
			Point.y -= rcChildItem.top;
			if(m_pHorizontalScrollBar->IsShowing()
				&& m_pHorizontalScrollBar->FindControlByPoint(Point))
			{
				return m_pHorizontalScrollBar;
			}
			Point = ptThis;//还原
		}

		return this;
	}

	bool CContainerUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		//这里会修改m_rcClient的值
		//修改方式是减去滚动条的位置
		
		ScrollBarStyle dwStyle = ScrollBarStyle::NonStyle;
		ScrollBarStyle dwScrollBarStyle = ScrollBarStyle::NonStyle;
		RECT rcBarIntersect;//两个滚动条同时显示，会出现一个交叉区域
		RECT rcVScollBar ,rcHScollBar;//滚动条位置，包括了交叉区域

		SIZE szContent = GetContentSize();//需要多少的显示空间
		bool bChange = __super::SetItem(rcItem, bMustUpdate);

		if(false == bChange && szContent.cx == m_szContent.cx
			&& szContent.cy == m_szContent.cy)
		{
			//item没有发生变动，内容区尺寸也没有发生变动，那么就不用修改滚动条了
			return false;
		}

		m_szContent = szContent;
		if(nullptr == m_pVerticalScrollBar && nullptr == m_pHorizontalScrollBar)
		{
			//木有滚动条
			//return bChange;
			goto end;
		}

		rcBarIntersect = rcHScollBar = rcVScollBar = m_rcRawItem;

		//垂直滚动条
		if(m_pVerticalScrollBar)
		{
			if(m_pVerticalScrollBar->IsActive())//一直显示滚动条
			{
				dwStyle = (ScrollBarStyle)(dwStyle | ScrollBarStyle::BarVertical);
			}
			else
			{
				if(m_szContent.cy > m_rcClient.bottom - m_rcClient.top)
				{
					dwStyle = (ScrollBarStyle)(dwStyle | ScrollBarStyle::BarVertical);
				}
			}

			if(dwStyle & ScrollBarStyle::BarVertical)
			{
				dwScrollBarStyle = m_pVerticalScrollBar->GetStyle();
				//修改m_rcClient的值，腾出滚动条空间
				if(dwScrollBarStyle & ScrollBarStyle::RightVerticalStyle)
				{
					//右侧垂直滚动条
					m_rcClient.right = m_rcRawItem.right - m_pVerticalScrollBar->GetWidth();
					rcVScollBar.left = m_rcClient.right;
				}
				else
				{
					//左侧垂直滚动条
					m_rcClient.left = m_rcRawItem.left + m_pVerticalScrollBar->GetWidth();
					rcVScollBar.right = m_rcClient.left;
				}
			}
			else
			{
				EmptyRect(rcVScollBar);
			}
		}

		//水平滚动条
		if(m_pHorizontalScrollBar)
		{
			if(m_pHorizontalScrollBar->IsActive())//一直显示滚动条
			{
				dwStyle = (ScrollBarStyle)(dwStyle | ScrollBarStyle::BarHorizontal);
			}
			else//有需要的时候才显示
			{
				if(m_szContent.cx > m_rcClient.right - m_rcClient.left)
				{
					dwStyle = (ScrollBarStyle)(dwStyle | ScrollBarStyle::BarHorizontal);
				}
			}

			if(dwStyle & ScrollBarStyle::BarHorizontal)
			{
				dwScrollBarStyle = m_pHorizontalScrollBar->GetStyle();
				//修改m_rcClient的值，腾出滚动条空间
				if(dwScrollBarStyle & ScrollBarStyle::BottomHorizontalStyle)
				{
					//底部水平滚动条
					m_rcClient.bottom = m_rcRawItem.bottom - m_pHorizontalScrollBar->GetHeight();
					rcHScollBar.top = m_rcClient.bottom;
				}
				else
				{
					//顶部水平滚动条
					m_rcClient.top = m_rcRawItem.top + m_pHorizontalScrollBar->GetHeight();
					rcHScollBar.bottom = m_rcClient.top;
				}

				//因为垂直滚动条是首先计算的，如果后来水平滚动条也显示出来了，显示的高度将会改变，再判断多一次
				if(m_pVerticalScrollBar && ((~dwStyle) & ScrollBarStyle::BarVertical))
				{
					//垂直滚动条有效，但是本来没有显示，接下来进行显示操作
					//新的滚动条出现，会占用显示控件，所以需要重新调整一次
					if(m_szContent.cy > m_rcClient.bottom - m_rcClient.top)
					{
						rcVScollBar = m_rcRawItem;
						dwStyle = (ScrollBarStyle)(dwStyle | ScrollBarStyle::BarVertical);//加上标志
						dwScrollBarStyle = m_pVerticalScrollBar->GetStyle();
						//修改m_rcClient的值，腾出滚动条空间
						if(dwScrollBarStyle & ScrollBarStyle::RightVerticalStyle)
						{
							//右侧垂直滚动条
							m_rcClient.right -= m_pVerticalScrollBar->GetWidth();
							rcVScollBar.left = m_rcClient.right;
						}
						else
						{
							//左侧垂直滚动条
							m_rcClient.left += m_pVerticalScrollBar->GetWidth();
							rcVScollBar.right = m_rcClient.left;
						}
					}
				}
			}
			else
			{
				EmptyRect(rcHScollBar);
			}
		}
		
		if((dwStyle & ScrollBarStyle::BarVertical) && (dwStyle & ScrollBarStyle::BarHorizontal))
		{
			//两个滚动条同时出现，设置他们的交汇点
			//这个操作一定是成功的，失败请调试
			MUIASSERT(IntersectRect(&rcBarIntersect, &rcVScollBar, &rcHScollBar));
		}
		else
		{
			//没有显示或者只显示了一个滚动条，那么交汇点为空
			EmptyRect(rcBarIntersect);
		}

		if(m_pVerticalScrollBar)
		{
			m_pVerticalScrollBar->SetBarIntersect(rcBarIntersect);
			m_pVerticalScrollBar->SetItem(rcVScollBar, false);
			m_pVerticalScrollBar->SetDragBarMaxShift(m_szContent.cy - (m_rcClient.bottom - m_rcClient.top));
		}

		if(m_pHorizontalScrollBar)
		{
			m_pHorizontalScrollBar->SetBarIntersect(rcBarIntersect);
			m_pHorizontalScrollBar->SetItem(rcHScollBar, false);
			m_pHorizontalScrollBar->SetDragBarMaxShift(m_szContent.cx - (m_rcClient.right - m_rcClient.left));
		}

end:
		//最后计算内容区偏移
		m_rcContent = m_rcClient;
		m_rcContent.right = m_rcContent.left + MAX(m_szContent.cx, m_rcClient.right - m_rcClient.left);
		m_rcContent.bottom = m_rcContent.top + MAX(m_szContent.cy, m_rcClient.bottom - m_rcClient.top);

		OffsetRect(&m_rcContent, -m_szScrollOffset.cx, -m_szScrollOffset.cy);
		return true;
	}

	bool CContainerUI::OnPaint(const RECT& rcUpdate)
	{
		//这个rcItem跟m_rcItem的值有点不同，
		//m_rcItem是相对于父控件的位置
		//rcItem是相对整个窗口的位置，是一个绝对位置
		RECT rcScrollUpdate, rcScrollItem;

		if(false == __super::OnPaint(rcUpdate))
		{
			return false;
		}

		if(m_pVerticalScrollBar && m_pVerticalScrollBar->IsShowing())
		{
			//计算出绘制区域，开始绘制
			rcScrollItem = m_pVerticalScrollBar->GetItem();
			if(TRUE == IntersectRect(&rcScrollUpdate, &rcScrollItem, &rcUpdate))
			{
				//拥有相交区域才能绘制
				OffsetRect(&rcScrollUpdate, -rcScrollItem.left, -rcScrollItem.top);
				m_pShareInfo->pRenderEngine->OffsetDrawPoint(rcScrollItem.left, rcScrollItem.top);
				m_pVerticalScrollBar->OnPaint(rcScrollUpdate);
				m_pShareInfo->pRenderEngine->OffsetDrawPoint(-rcScrollItem.left, -rcScrollItem.top);
			}
		}

		if(m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsShowing())
		{
			//计算出绘制区域，开始绘制
			rcScrollItem = m_pHorizontalScrollBar->GetItem();
			if(TRUE == IntersectRect(&rcScrollUpdate, &rcScrollItem, &rcUpdate))
			{
				//拥有相交区域才能绘制
				OffsetRect(&rcScrollUpdate, -rcScrollItem.left, -rcScrollItem.top);
				m_pShareInfo->pRenderEngine->OffsetDrawPoint(rcScrollItem.left, rcScrollItem.top);
				m_pHorizontalScrollBar->OnPaint(rcScrollUpdate);
				m_pShareInfo->pRenderEngine->OffsetDrawPoint(-rcScrollItem.left, -rcScrollItem.top);
			}
		}
		
		return true;
	}

	void CContainerUI::PaintText(const RECT& rcUpdate)
	{
		//这个控件不需要绘制文字

		//提取客户区的有效显示区域
		RECT rcThisUpdate;
		SIZE szRound = { 0 };
		if (FALSE == IntersectRect(&rcThisUpdate, &m_rcClient, &rcUpdate))
		{
			return;
		}

		HCLIP hClientOldClip = NULL;
		if (FALSE == IsSameRect(m_rcClient, m_rcRawItem))
		{
			//存在滚动条，提供给子控件的区域需要再减去滚动条
			hClientOldClip = m_pShareInfo->pRenderEngine->EnterClip(m_rcClient, szRound);
		}

		PaintContent(rcUpdate);

		if (hClientOldClip)
		{
			m_pShareInfo->pRenderEngine->LeaveClip(hClientOldClip);
		}

		return;
	}

	void CContainerUI::PaintContent(const RECT& rcUpdate)
	{
		return;
	}

	bool CContainerUI::SetHorizontalScrollBarActive(bool bActive)
	{
		if(m_pHorizontalScrollBar)
		{
			m_pHorizontalScrollBar->SetActive(bActive);
			return true;
		}
		return false;
	}

	bool CContainerUI::GetHorizontalScrollBarActive()
	{
		if(m_pHorizontalScrollBar)
		{
			return m_pHorizontalScrollBar->IsActive();
		}
		return false;
	}

	bool CContainerUI::SetVerticalScrollBarActive(bool bActive)
	{
		if(m_pVerticalScrollBar)
		{
			m_pVerticalScrollBar->SetActive(bActive);
			return true;
		}
		return false;
	}

	bool CContainerUI::GetVerticalScrollBarActive()
	{
		if(m_pVerticalScrollBar)
		{
			return m_pVerticalScrollBar->IsActive();
		}
		return false;
	}

	void CContainerUI::SetVerticalScrollBar(CScrollBarUI * pScrollBar)
	{
		if(m_pVerticalScrollBar) delete m_pVerticalScrollBar;
		m_pVerticalScrollBar = pScrollBar;
		if(m_pVerticalScrollBar)
		{
			m_pVerticalScrollBar->SetParent(this);
		}
	}

	CScrollBarUI * CContainerUI::GetVerticalScrollBar() const
	{
		return m_pVerticalScrollBar;
	}

	void CContainerUI::SetHorizontalScrollBar(CScrollBarUI * pScrollBar)
	{
		if(m_pHorizontalScrollBar) delete m_pHorizontalScrollBar;
		m_pHorizontalScrollBar = pScrollBar;
		if(m_pHorizontalScrollBar)
		{
			m_pHorizontalScrollBar->SetParent(this);
		}
	}

	CScrollBarUI * CContainerUI::GetHorizontalScrollBar() const
	{
		return m_pHorizontalScrollBar;
	}

	void CContainerUI::OnScrollBarMove(CScrollBarUI* pSender, int nShift)
	{
		if (NULL == pSender)
		{
			return;
		}

		if(pSender == m_pVerticalScrollBar)
		{
			m_szScrollOffset.cy = nShift;
			this->Renewal();
		}

		if(pSender == m_pHorizontalScrollBar)
		{
			m_szScrollOffset.cx = nShift;
			this->Renewal();
		}
	}

	const SIZE &CContainerUI::GetScrollBarShift() const
	{
		return m_szScrollOffset;
	}

	LRESULT CContainerUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		bool bRet = false;
		switch(message)
		{
        case WM_RBUTTONUP:
        {
            
        }break;
		case WM_MOUSEWHEEL:
			{
				if(m_pVerticalScrollBar)
				{
					return m_pVerticalScrollBar->CallWndProc(message, wParam, lParam);
				}
			}break;
		case WM_MOUSEMOVE:
			{
			}break;
		default:
			break;
		}
		return __super::WndProc(message, wParam, lParam);
	}
}