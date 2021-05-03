
#include "UITabLayout.h"

namespace MYUI
{
	CTabLayoutUI::CTabLayoutUI()
		: m_pSelect(nullptr)
	{
	}

	CTabLayoutUI::~CTabLayoutUI()
	{
	}

	const CMuiString CTabLayoutUI::g_strClassName(_T("TabLayoutUI"));

	CMuiString CTabLayoutUI::GetClassName() const
	{
		return CTabLayoutUI::g_strClassName;
	}

	void CTabLayoutUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		__super::SetAttribute(strItem, strValue);
	}

	CControlUI * CTabLayoutUI::GetSelect()
	{
		return m_pSelect;
	}

	int CTabLayoutUI::GetIndex()
	{
		return m_Items.Find(m_pSelect);
	}

	bool CTabLayoutUI::AddAt(CControlUI* pControl, int nIndex)
	{
		pControl->SetVisible(false);
		if(false == __super::AddAt(pControl, nIndex))
		{
			return false;
		}
		
		if(nullptr == m_pSelect)
		{
			SelectItem(pControl);
		}
		
		return true;
	}

	bool CTabLayoutUI::RemoveAt(int nIndex)
	{
		CControlUI * pControl = (CControlUI *)m_Items[nIndex];

		do
		{
			if(nullptr == pControl)
			{
				return false;
			}

			if(pControl == m_pSelect)
			{
				m_pSelect->SetVisible(false);
				if(1 == nIndex)
				{
					m_pSelect = nullptr;
					//pControl == m_pSelect && 1 == nIndex
					//说明只有一个控件，现在都被移除了，所以没控件显示了
					break;
				}

				int nSize = m_Items.GetSize();
				if(nIndex + 1 == nSize)
				{
					//nIndex是最后一个控件，索引向前移动
					m_pSelect = (CControlUI*)m_Items[nIndex - 1];
				}
				else
				{
					//nIndex并不是最后一个控件，索引向后移动
					m_pSelect = (CControlUI*)m_Items[nIndex + 1];
				}
				m_pSelect->SetVisible(true);
			}
		}while(0);

		return __super::RemoveAt(nIndex);
	}

	bool CTabLayoutUI::SelectItem(int nIndex)
	{
		CControlUI * pControl = (CControlUI *)m_Items[nIndex];
		
		if(pControl)
		{
			if(m_pSelect)
			{
				m_pSelect->SetVisible(false);
			}
			m_pSelect = pControl;
			m_pSelect->SetVisible(true);
			return true;
		}
		return false;
	}

	bool CTabLayoutUI::SelectItem(CControlUI * pControl)
	{
		return SelectItem(m_Items.Find(pControl));
	}

	SIZE CTabLayoutUI::GetContentSize()
	{
		SIZE szContent = {0};
		
		//CTabLayoutUI控件不会出现滚动条，所以永远返回零
		//所以CTabLayoutUI控件最好设置宽和高给它（或者min_size）
		//想显示滚动条，请在子控件中添加滚动条
		return szContent;
	}

	bool CTabLayoutUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		RECT rcChildItem;
		RECT rcChildMargin;
		CControlUI * pControl = nullptr;
		int nControlCount = m_Items.GetSize();

		if(false == CControlUI::SetItem(rcItem, bMustUpdate)) return false;
		if(nControlCount == 0 ) return true;//布局中没有其他控件就直接返回

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];

			//置空
			EmptyRect(rcChildItem);
			
			//if(true == pControl->IsVisible())
			{
				rcChildItem = m_rcClient;
				//缩进相对于父控件的内边距
				IndentRect(&rcChildItem, &m_rcInset);
				//缩进相对于自己的外边距
				rcChildMargin = pControl->GetMargin();
				IndentRect(&rcChildItem, &rcChildMargin);
			}

			pControl->SetItem(rcChildItem, bMustUpdate);
		}
		return true;
	}

	bool CTabLayoutUI::OnPaint(const RECT& rcUpdate)
	{
		bool bResult = false;
		RECT rcChildItem = m_rcClient;
		RECT rcChildUpdate;
		//先绘制自己，没有滚动条的
		
		if(false == CControlUI::OnPaint(rcUpdate)) return false;

		if (NULL == m_pSelect) return true;

		rcChildItem = m_pSelect->GetItem();
		if(IntersectRect(&rcChildUpdate, &rcChildItem, &rcUpdate))
		{
			OffsetRect(&rcChildUpdate, -rcChildItem.left, -rcChildItem.top);
			bResult = m_pSelect->OnPaint(rcChildUpdate);
		}

		return true;
	}
}