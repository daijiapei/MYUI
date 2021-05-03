
#include "UIBaseLayout.h"


namespace MYUI
{
/***************************************************************************
 * CBaseLayoutUI�࣬���ֻ���
 *
 *
***************************************************************************/
	CBaseLayoutUI::CBaseLayoutUI()
	{
		m_bControl = false;
		EmptyRect(m_rcInset);
	}

	CBaseLayoutUI::~CBaseLayoutUI()
	{
		MUITRACE(_T("CBaseLayoutUI::~CBaseLayoutUI"));
		for(int i=0; m_Items.GetSize() > i; i++)
		{
			CControlUI * pControl = (CControlUI*)m_Items[i];
			delete pControl;
		}
	}

	void CBaseLayoutUI::SetShareInfo(MUISHAREINFO * pShareInfo)
	{
        __super::SetShareInfo(pShareInfo);
		for(int i=0 ; m_Items.GetSize() > i; i++)
		{
			CControlUI * pControl = (CControlUI*)m_Items[i];
			pControl->SetShareInfo(pShareInfo);
		}
	}

	const CMuiString CBaseLayoutUI::g_strClassName(_T("BaseLayoutUI"));

	CMuiString CBaseLayoutUI::GetClassName() const
	{
		return CBaseLayoutUI::g_strClassName;
	}

	void CBaseLayoutUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("Inset")))
		{
			SetInset(CMuiRect(strValue));
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
	}

	//ע�������и�����add����remove�Ƿ�ɹ����������
	//�ӿؼ������ڵ�bug�������ݲ�����
	bool CBaseLayoutUI::Add(CControlUI* pControl)
	{
		int nSize = m_Items.GetSize();
		return AddAt(pControl, nSize);
	}

	bool CBaseLayoutUI::AddAt(CControlUI* pControl,int iIndex)
	{
		pControl->SetParent(this);
		return m_Items.InsertAt(iIndex,pControl);
	}

	bool CBaseLayoutUI::Remove(CControlUI* pControl)
	{
		int nIndex = m_Items.Find(pControl);
		return RemoveAt(nIndex);
	}

	bool CBaseLayoutUI::RemoveAt(int nIndex)
	{
		CControlUI * pControl = (CControlUI*)m_Items[nIndex];
		pControl->SetParent(nullptr);
		return m_Items.Remove(nIndex);
	}

	void CBaseLayoutUI::RemoveAll()
	{
		int size = m_Items.GetSize();
		while(size--)
		{
			RemoveAt(size);
		}
	}

	CControlUI * CBaseLayoutUI::Find(int nIndex)
	{
		return (CControlUI *) m_Items[nIndex];
	}

	int CBaseLayoutUI::Find(CControlUI * pControl)
	{
		return m_Items.Find(pControl);
	}

	int CBaseLayoutUI::GetCount()
	{
		return m_Items.GetSize();
	}

	CControlUI * CBaseLayoutUI::FindControlByPoint(POINT &Point)
	{
		CControlUI * temp;
		POINT ptThis = Point;//��������
		RECT rcChildItem;

		int nCount = m_Items.GetSize();
		CControlUI * target = __super::FindControlByPoint(Point);

		//this != target ��ʾtarget����Ϊ����������NULL�����Է���
		//true == m_bControl��ʾ��������ģ��ؼ�������Ҫ���������Ƿ����ӿؼ������Է���
		if(this != target || true == m_bControl) return target;

		//���һ�������Ƿ񻹴������ӿؼ��У�����ǣ��򷵻��ӿؼ�
		//ע�⣬��Ϊ����ʱ���ǰ�˳����Ƶģ�����ؼ�λ�÷����ص�
		//�Ȼ��ƵĿؼ��ᱻ����ƵĿؼ�������
		//�������Ǽ���pointָ��Ŀؼ�ʱ��Ҫ���з��������
		//���������㺯���У�ҲҪע��ͬ�������⣬���㷽��һ��Ҫ����Ʒ�ʽ�෴
		while(nCount--)//������ѯ
		{
			CControlUI * pControl = (CControlUI *)m_Items[nCount];
			if(false == pControl->IsVisible())
			{
				continue;
			}
			rcChildItem = pControl->GetItem();
			Point.x -= rcChildItem.left - m_szScrollOffset.cx;
			Point.y -= rcChildItem.top - m_szScrollOffset.cy;
			temp = pControl->FindControlByPoint(Point);
			if(temp)
			{
				target = temp;
				break;
			}
			Point = ptThis;//���ܹ���ȡ�ɹ���Ҫ��ԭ����
		}
		return target;
	}

	CControlUI * CBaseLayoutUI::FindControlByName(LPCTSTR strName)
	{
		int nCount = m_Items.GetSize();
		CControlUI * target = __super::FindControlByName(strName);

		if(target || true == m_bControl) return target;

		while(nCount--)//������ѯ
		{
			CControlUI * pControl = (CControlUI *)m_Items[nCount];

			if(target = pControl->FindControlByName(strName))
			{
				break;
			}
		}
		return target;
	}

	void CBaseLayoutUI::SetInset(RECT &rect)
	{
		m_rcInset = rect;
	}

	const RECT &CBaseLayoutUI::GetInset()
	{
		return m_rcInset;
	}

	void CBaseLayoutUI::PaintContent(const RECT& rcUpdate)
	{
		SIZE szRound = {0};
		RECT rcChildPaint, rcChildItem, rcChildUpdate;
		CControlUI * pControl;
		int nChildCount = m_Items.GetSize();
        HCLIP hChildOldClip = NULL;

		//�ͻ���������ṩ���ӿؼ���Ч��ͼ����
		//OffsetRect(&rcThisClient, rcItem.left, rcItem.top);
		//��ȡ�ͻ�������Ч��ʾ����

		for(int i=0; nChildCount > i; i++)
		{
			hChildOldClip = NULL;
			pControl = (CControlUI*)m_Items[i];
			rcChildItem = pControl->GetItem();

			if (false == pControl->IsVisible())
			{
				continue;
			}
			
			//���ӿؼ�����ʾ����
			if(FALSE == ::IntersectRect(&rcChildPaint, &rcChildItem, &m_rcClient))
			{
				continue;
			}

			//���ӿؼ��ĸ�������
			if (FALSE == ::IntersectRect(&rcChildUpdate, &rcChildPaint, &rcUpdate))
			{
				continue;
			}

			szRound = pControl->GetBorderRound();
			if (szRound.cx || szRound.cy)
			{
				hChildOldClip = m_pShareInfo->pRenderEngine->EnterClip(rcChildItem, szRound);
			}

			::OffsetRect(&rcChildUpdate, -rcChildItem.left, -rcChildItem.top);
			m_pShareInfo->pRenderEngine->OffsetDrawPoint(rcChildItem.left, rcChildItem.top);
			pControl->OnPaint(rcChildUpdate);
			m_pShareInfo->pRenderEngine->OffsetDrawPoint(-rcChildItem.left, -rcChildItem.top);

			if (hChildOldClip)
			{
				m_pShareInfo->pRenderEngine->LeaveClip(hChildOldClip);
			}
		}

	}
}