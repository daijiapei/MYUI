
#include "UIAbsoluteLayout.h"


namespace MYUI
{
	CAbsoluteLayoutUI::CAbsoluteLayoutUI()
	{
	}

	CAbsoluteLayoutUI::~CAbsoluteLayoutUI()
	{
	}

	const CMuiString CAbsoluteLayoutUI::g_strClassName(_T("AbsoluteLayoutUI"));

	CMuiString CAbsoluteLayoutUI::GetClassName() const
	{
		return CAbsoluteLayoutUI::g_strClassName;
	}

	SIZE CAbsoluteLayoutUI::GetContentSize()
	{
		//注意，当xy是负数的时候，负数区域将无法显示
		//实际上可以做到兼容负数的，但是这样逻辑不合理
		SIZE szContent = {0};
		RECT rcPos;
		RECT rcMargin;
		CControlUI * pControl = nullptr;
		int nControlCount = m_Items.GetSize();

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];
			
			if(false == pControl->IsVisible())
			{
				continue;
			}
			rcPos = pControl->GetPos();
			rcMargin = pControl->GetMargin();
			ExpandRect(&rcPos, &rcMargin);
			szContent.cx = MAX(szContent.cx, rcPos.right);
			szContent.cy = MAX(szContent.cy, rcPos.bottom);
		}

		szContent.cx += m_rcInset.left + m_rcInset.right;
		szContent.cy += m_rcInset.top + m_rcInset.bottom;
		return szContent;
	}

	bool CAbsoluteLayoutUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		RECT rcChildItem;
		RECT rcThisRegoin;
		EnumFloatType type = EnumFloatType::FloatNon;
		CControlUI * pControl = nullptr;
		int nControlCount = m_Items.GetSize();

		if(false == __super::SetItem(rcItem, bMustUpdate)) return false;

		rcThisRegoin = m_rcContent;
		IndentRect(&rcThisRegoin, &m_rcInset);

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];
			
			EmptyRect(rcChildItem);//置空
			if(false == pControl->IsVisible())
			{
				//不显示打印区域就是空了
				goto loop;
			}

			type =  pControl->GetFloatType();

			if(EnumFloatType::FloatPointX & type)
			{
				rcChildItem.left = pControl->GetPointX() + rcThisRegoin.left;
			}
			else
			{
				rcChildItem.left = rcThisRegoin.left;
			}

			if(EnumFloatType::FloatPointY & type)
			{
				rcChildItem.top = pControl->GetPointY() + rcThisRegoin.top;
			}
			else
			{
				rcChildItem.top = rcThisRegoin.top;
			}

			if(EnumFloatType::FloatSizeCx & type)
			{
				rcChildItem.right = rcChildItem.left + pControl->GetWidth();
			}
			else
			{
				rcChildItem.right = rcThisRegoin.right;
			}

			if(EnumFloatType::FloatSizeCy & type)
			{
				rcChildItem.bottom = rcChildItem.top + pControl->GetHeight();
			}
			else
			{
				rcChildItem.bottom = rcThisRegoin.bottom;
			}

			//绝对布局的子控件外边距没有意义，所以不需要处理
			//rcChildMargin = pControl->GetMargin();
			//ExpandRect(&rcChildItem, &rcChildMargin);

loop:
			pControl->SetItem(rcChildItem, false);
		}
		return true;
	}
}