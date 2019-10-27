
#include "UIAbsoluteLayout.h"


namespace MYUI
{
	CAbsoluteLayoutUI::CAbsoluteLayoutUI()
	{
	}

	CAbsoluteLayoutUI::~CAbsoluteLayoutUI()
	{
	}

	CMuiString CAbsoluteLayoutUI::g_strClassName(_T("AbsoluteLayoutUI"));

	CMuiString CAbsoluteLayoutUI::GetClassName() const
	{
		return CAbsoluteLayoutUI::g_strClassName;
	}

	SIZE CAbsoluteLayoutUI::GetContentSize()
	{
		//ע�⣬��xy�Ǹ�����ʱ�򣬸��������޷���ʾ
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
		//RECT rcChildMargin;
		EnumFloatType type = EnumFloatType::FloatNon;
		CControlUI * pControl = nullptr;
		int nControlCount = m_Items.GetSize();

		if(false == __super::SetItem(rcItem, bMustUpdate)) return false;
		if(nControlCount == 0 ) return true;//������û�������ؼ���ֱ�ӷ���

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];
			
			EmptyRect(rcChildItem);//�ÿ�
			if(false == pControl->IsVisible())
			{
				//����ʾ��ӡ������ǿ���
				goto loop;
			}

			type =  pControl->GetFloatType();

			if(EnumFloatType::FloatPointX & type)
			{
				rcChildItem.left = pControl->GetPointX();
			}
			else
			{
				rcChildItem.left = 0;
			}

			if(EnumFloatType::FloatPointY & type)
			{
				rcChildItem.top = pControl->GetPointY();
			}
			else
			{
				rcChildItem.top = 0;
			}

			if(EnumFloatType::FloatSizeCx & type)
			{
				rcChildItem.right = rcChildItem.left + pControl->GetWidth();
			}
			else
			{
				rcChildItem.right = m_rcClient.right;
			}

			if(EnumFloatType::FloatSizeCy & type)
			{
				rcChildItem.bottom = rcChildItem.top + pControl->GetHeight();
			}
			else
			{
				rcChildItem.bottom = m_rcClient.bottom;
			}

			//���Բ��ֵ���߾಻��Ч
			//rcChildMargin = pControl->GetMargin();
			//ExpandRect(&rcChildItem, &rcChildMargin);
			OffsetRect(&rcChildItem, m_rcInset.left, m_rcInset.top);

loop:
			pControl->SetItem(rcChildItem, false);
		}
		return true;
	}
}