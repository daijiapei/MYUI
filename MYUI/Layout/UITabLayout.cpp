
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

	CMuiString CTabLayoutUI::g_strClassName(_T("TabLayoutUI"));

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
					//˵��ֻ��һ���ؼ������ڶ����Ƴ��ˣ�����û�ؼ���ʾ��
					break;
				}

				int nSize = m_Items.GetSize();
				if(nIndex + 1 == nSize)
				{
					//nIndex�����һ���ؼ���������ǰ�ƶ�
					m_pSelect = (CControlUI*)m_Items[nIndex - 1];
				}
				else
				{
					//nIndex���������һ���ؼ�����������ƶ�
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
		
		//CTabLayoutUI�ؼ�������ֹ�������������Զ������
		//����CTabLayoutUI�ؼ�������ÿ�͸߸���������min_size��
		//����ʾ�������������ӿؼ�����ӹ�����
		return szContent;
	}

	bool CTabLayoutUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		RECT rcChildItem;
		RECT rcChildMargin;
		CControlUI * pControl = nullptr;
		int nControlCount = m_Items.GetSize();

		if(false == CControlUI::SetItem(rcItem, bMustUpdate)) return false;
		if(nControlCount == 0 ) return true;//������û�������ؼ���ֱ�ӷ���

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];

			//�ÿ�
			EmptyRect(rcChildItem);
			
			if(false == pControl->IsVisible())
			{
				//����ʾ��ӡ������ǿ���
				goto loop;
			}

			rcChildItem = m_rcClient;
			//��������ڸ��ؼ����ڱ߾�
			IndentRect(&rcChildItem, &m_rcInset);
			//����������Լ�����߾�
			rcChildMargin = pControl->GetMargin();
			IndentRect(&rcChildItem, &rcChildMargin);
loop:
			pControl->SetItem(rcChildItem, false);
		}
		return true;
	}

	bool CTabLayoutUI::OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate)
	{
		bool bResult = false;
		RECT rcChildItem = m_rcClient;
		RECT rcChildPaint;
		//�Ȼ����Լ���û�й�������
		
		HCLIP hOldClip = m_pShareInfo->pRenderEngine->EnterClip(rcPaint, m_szBorderRound);
		if(false == CControlUI::OnPaint(rcItem, rcPaint, rcUpdate))
		{
			goto finish;
		}

		//�ٻ���m_pSelect��OK��
		if(m_pSelect && OffsetRect(&rcChildItem, rcItem.left, rcItem.top)
			&& IntersectRect(&rcChildPaint, &rcChildItem, &rcPaint))
		{
			bResult = m_pSelect->OnPaint(rcChildItem, rcChildPaint, rcUpdate);
		}

finish:
		m_pShareInfo->pRenderEngine->LeaveClip(hOldClip);
		return bResult;
	}
}