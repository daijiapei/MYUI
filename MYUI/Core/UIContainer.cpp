
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

	void CContainerUI::SetShareInfo(TSHAREINFO * pShareInfo)
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
			ASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				SetVerticalScrollBar(new CVerticalScrollBarUI());
			}
		}
		else if(0 == _tcsicmp(strItem, _T("hScroll"))) 
		{
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				SetHorizontalScrollBar(new CHorizontailScrollBarUI());
			}
		}
		else if(0 == _tcsicmp(strItem, _T("vScrollActive"))) 
		{
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			SetVerticalScrollBarActive(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("hScrollActive"))) 
		{
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			SetHorizontalScrollBarActive(CheckTrue(strValue));
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
	}

	CControlUI * CContainerUI::FindControlByPoint(POINT &pt)
	{
		POINT ptThis = pt;//��������
		RECT rcChildItem;

		if(NULL == __super::FindControlByPoint(pt)) return NULL;
		//Ҫ���һ�������Ƿ������ӿؼ����߹�������

		//���ж��Ƿ����ڹ��������ǵĻ��������Լ�
		if(m_pVerticalScrollBar)
		{
			//GetItemClient�ܹ��ų�������
			rcChildItem = m_pVerticalScrollBar->GetItemClient();
			pt.x -= rcChildItem.left;
			pt.y -= rcChildItem.top;
			if(m_pVerticalScrollBar->IsShowing()
				&& m_pVerticalScrollBar->FindControlByPoint(pt))
			{
				return m_pVerticalScrollBar;
			}
			pt = ptThis;
		}

		if(m_pHorizontalScrollBar)
		{
			//GetItemClient�ܹ��ų�������
			rcChildItem = m_pHorizontalScrollBar->GetItemClient();
			pt.x -= rcChildItem.left;
			pt.y -= rcChildItem.top;
			if(m_pHorizontalScrollBar->IsShowing()
				&& m_pHorizontalScrollBar->FindControlByPoint(pt))
			{
				return m_pHorizontalScrollBar;
			}
			pt = ptThis;
		}
		return this;
	}

	bool CContainerUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		//������޸�m_rcClient��ֵ
		//�޸ķ�ʽ�Ǽ�ȥ��������λ��
		
		ScrollBarStyle dwStyle = ScrollBarStyle::NonStyle;
		ScrollBarStyle dwScrollBarStyle = ScrollBarStyle::NonStyle;
		RECT rcBarIntersect;//����������ͬʱ��ʾ�������һ����������
		RECT rcVScollBar ,rcHScollBar;//������λ�ã������˽�������

		SIZE szContent = GetContentSize();//��Ҫ���ٵ���ʾ�ռ�
		bool bChange = __super::SetItem(rcItem, bMustUpdate);

		if(false == bChange && szContent.cx == m_szContent.cx
			&& szContent.cy == m_szContent.cy)
		{
			//itemû�з����䶯���������ߴ�Ҳû�з����䶯����ô�Ͳ����޸Ĺ�������
			return false;
		}

		m_szContent = szContent;
		if(nullptr == m_pVerticalScrollBar && nullptr == m_pHorizontalScrollBar)
		{
			//ľ�й�����
			//return bChange;
			goto end;
		}

		rcBarIntersect = rcHScollBar = rcVScollBar = m_rcRawItem;

		//��ֱ������
		if(m_pVerticalScrollBar)
		{
			if(m_pVerticalScrollBar->IsActive())//һֱ��ʾ������
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
				//�޸�m_rcClient��ֵ���ڳ��������ռ�
				if(dwScrollBarStyle & ScrollBarStyle::RightVerticalStyle)
				{
					//�Ҳഹֱ������
					m_rcClient.right = m_rcRawItem.right - m_pVerticalScrollBar->GetWidth();
					rcVScollBar.left = m_rcClient.right;
				}
				else
				{
					//��ഹֱ������
					m_rcClient.left = m_rcRawItem.left + m_pVerticalScrollBar->GetWidth();
					rcVScollBar.right = m_rcClient.left;
				}
			}
			else
			{
				EmptyRect(rcVScollBar);
			}
		}

		//ˮƽ������
		if(m_pHorizontalScrollBar)
		{
			if(m_pHorizontalScrollBar->IsActive())//һֱ��ʾ������
			{
				dwStyle = (ScrollBarStyle)(dwStyle | ScrollBarStyle::BarHorizontal);
			}
			else//����Ҫ��ʱ�����ʾ
			{
				if(m_szContent.cx > m_rcClient.right - m_rcClient.left)
				{
					dwStyle = (ScrollBarStyle)(dwStyle | ScrollBarStyle::BarHorizontal);
				}
			}

			if(dwStyle & ScrollBarStyle::BarHorizontal)
			{
				dwScrollBarStyle = m_pHorizontalScrollBar->GetStyle();
				//�޸�m_rcClient��ֵ���ڳ��������ռ�
				if(dwScrollBarStyle & ScrollBarStyle::BottomHorizontalStyle)
				{
					//�ײ�ˮƽ������
					m_rcClient.bottom = m_rcRawItem.bottom - m_pHorizontalScrollBar->GetHeight();
					rcHScollBar.top = m_rcClient.bottom;
				}
				else
				{
					//����ˮƽ������
					m_rcClient.top = m_rcRawItem.top + m_pHorizontalScrollBar->GetHeight();
					rcHScollBar.bottom = m_rcClient.top;
				}

				//��Ϊ��ֱ�����������ȼ���ģ��������ˮƽ������Ҳ��ʾ�����ˣ���ʾ�ĸ߶Ƚ���ı䣬���ж϶�һ��
				if(m_pVerticalScrollBar && ((~dwStyle) & ScrollBarStyle::BarVertical))
				{
					//��ֱ��������Ч�����Ǳ���û����ʾ��������������ʾ����
					//�µĹ��������֣���ռ����ʾ�ؼ���������Ҫ���µ���һ��
					if(m_szContent.cy > m_rcClient.bottom - m_rcClient.top)
					{
						rcVScollBar = m_rcRawItem;
						dwStyle = (ScrollBarStyle)(dwStyle | ScrollBarStyle::BarVertical);//���ϱ�־
						dwScrollBarStyle = m_pVerticalScrollBar->GetStyle();
						//�޸�m_rcClient��ֵ���ڳ��������ռ�
						if(dwScrollBarStyle & ScrollBarStyle::RightVerticalStyle)
						{
							//�Ҳഹֱ������
							m_rcClient.right -= m_pVerticalScrollBar->GetWidth();
							rcVScollBar.left = m_rcClient.right;
						}
						else
						{
							//��ഹֱ������
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
			//����������ͬʱ���֣��������ǵĽ����
			//�������һ���ǳɹ��ģ�ʧ�������
			ASSERT(IntersectRect(&rcBarIntersect, &rcVScollBar, &rcHScollBar));
		}
		else
		{
			//û����ʾ����ֻ��ʾ��һ������������ô�����Ϊ��
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
		//������������ƫ��
		m_rcContent = m_rcClient;
		m_rcContent.right = m_rcContent.left + MAX(m_szContent.cx, m_rcClient.right - m_rcClient.left);
		m_rcContent.bottom = m_rcContent.top + MAX(m_szContent.cy, m_rcClient.bottom - m_rcClient.top);
		//���ƺ���ȡ�ǣ��ټ���ƫ��
		//OffsetRect(&m_rcContent, -m_szScrollOffset.cx, -m_szScrollOffset.cy);
		return true;
	}

	bool CContainerUI::OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate)
	{
		//���rcItem��m_rcItem��ֵ�е㲻ͬ��
		//m_rcItem������ڸ��ؼ���λ��
		//rcItem������������ڵ�λ�ã���һ������λ��

		RECT rcScrollPaint, rcScrollClient;

		if(false == __super::OnPaint(rcItem ,rcPaint, rcUpdate))
		{
			return false;
		}

		if(m_pVerticalScrollBar && m_pVerticalScrollBar->IsShowing())
		{
			//������������򣬿�ʼ����
			rcScrollClient = m_pVerticalScrollBar->GetItem();
			OffsetRect(&rcScrollClient,rcItem.left, rcItem.top);
			if(TRUE == IntersectRect(&rcScrollPaint, &rcScrollClient, &rcPaint))
			{
				//ӵ���ཻ������ܻ���
				m_pVerticalScrollBar->OnPaint(rcScrollClient ,rcScrollPaint, rcUpdate);
			}
		}

		if(m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsShowing())
		{
			//������������򣬿�ʼ����
			rcScrollClient = m_pHorizontalScrollBar->GetItem();
			OffsetRect(&rcScrollClient,rcItem.left, rcItem.top);
			if(TRUE == IntersectRect(&rcScrollPaint, &rcScrollClient, &rcPaint))
			{
				//ӵ���ཻ������ܻ���
				m_pHorizontalScrollBar->OnPaint(rcScrollClient ,rcScrollPaint, rcUpdate);
			}
		}

		return true;
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

	void CContainerUI::OnScrollBarMove(LPCVOID pSender, int nShift)
	{
		if(pSender == m_pVerticalScrollBar && m_pVerticalScrollBar)
		{
			m_szScrollOffset.cy = nShift;
			this->Invalidate();
		}

		if(pSender == m_pHorizontalScrollBar && m_pHorizontalScrollBar)
		{
			m_szScrollOffset.cx = nShift;
			this->Invalidate();
		}
	}

	const SIZE &CContainerUI::GetScrollBarShift() const
	{
		return m_szScrollOffset;
	}

	void CContainerUI::PaintText(const RECT& rcItem, const RECT& rcPaint)
	{
		return ;
	}

	LRESULT CContainerUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		bool bRet = false;
		switch(message)
		{
        case WM_RBUTTONUP:
        {
            
        }break;
		case WM_GETSCROLL:
			{
				if(GSL_VERTICAL == wParam)
				{
					return (LRESULT)GetVerticalScrollBar();
				}
				if(GSL_HORIZONTAIL == wParam)
				{
					return (LRESULT)GetHorizontalScrollBar();
				}
				return NULL;
			}break;
		case WM_MOUSEWHEEL:
			{
				if(m_bEnabled && m_pVerticalScrollBar)
				{
					return m_pVerticalScrollBar->CallWndProc(hWnd, message, wParam, lParam);
				}
			}break;
		case WM_MOUSEMOVE:
			{
			}break;
		default:
			break;
		}
		return __super::WndProc(hWnd, message, wParam, lParam);
	}
}