
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
		TRACE(_T("CBaseLayoutUI::~CBaseLayoutUI"));
		for(int i=0; m_Items.GetSize() > i; i++)
		{
			CControlUI * pControl = (CControlUI*)m_Items[i];
			delete pControl;
		}
	}

	void CBaseLayoutUI::SetShareInfo(TSHAREINFO * pShareInfo)
	{
        __super::SetShareInfo(pShareInfo);
		for(int i=0 ; m_Items.GetSize() > i; i++)
		{
			CControlUI * pControl = (CControlUI*)m_Items[i];
			pControl->SetShareInfo(pShareInfo);
		}
	}

	CMuiString CBaseLayoutUI::g_strClassName(_T("BaseLayoutUI"));

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

	int CBaseLayoutUI::GetCount() const
	{
		return m_Items.GetSize();
	}

	CControlUI * CBaseLayoutUI::FindControlByPoint(POINT &pt)
	{
		CControlUI * temp;
		POINT ptThis = pt;//��������
		RECT rcChildItem;

		int nCount = m_Items.GetSize();
		CControlUI * target = __super::FindControlByPoint(pt);

		//this != target ��ʾtarget����Ϊ����������NULL�����Է���
		//true == m_bControl��ʾ��������ģ��ؼ�����������ָ��
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
			pt.x -= rcChildItem.left - m_szScrollOffset.cx;
			pt.y -= rcChildItem.top - m_szScrollOffset.cy;
			temp = pControl->FindControlByPoint(pt);
			if(temp)
			{
				target = temp;
				break;
			}
			pt = ptThis;//���ܹ���ȡ�ɹ���Ҫ��ԭ�����������û����
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

	const RECT &CBaseLayoutUI::GetInset() const
	{
		return m_rcInset;
	}

	bool CBaseLayoutUI::OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate)
	{
		//���rcItem��m_rcItem��ֵ�е㲻ͬ��
		//m_rcItem������ڸ��ؼ���λ��
		//rcItem������������ڵ�λ�ã���һ������λ��
		SIZE szRound = {0};
		RECT rcChildPaint, rcChildClient;
		RECT rcThisClient, rcThisClientPaint;
		CControlUI * pControl;
		int nChildCount = m_Items.GetSize();
        HCLIP hClientOldClip = NULL, hChildOldClip = NULL, hItemOldClip = NULL;

        if (NULL == m_pShareInfo) return false;

		hItemOldClip = m_pShareInfo->pRenderEngine->EnterClip(rcItem, m_szBorderRound);
		if(false == __super::OnPaint(rcItem ,rcPaint, rcUpdate))
		{
			goto finish;
		}

		rcThisClient = m_rcClient;
		//�ͻ���������ṩ���ӿؼ���Ч��ͼ����
		OffsetRect(&rcThisClient, rcItem.left, rcItem.top);
		//��ȡ�ͻ�������Ч��ʾ����
		if(FALSE == IntersectRect(&rcThisClientPaint, &rcThisClient , &rcPaint))
		{
			goto finish;
		}

		if(FALSE == IsSameRect(m_rcClient, m_rcRawItem))
		{
			//���ڹ��������ṩ���ӿؼ���������Ҫ�ټ�ȥ������
			hClientOldClip = m_pShareInfo->pRenderEngine->EnterClip(rcThisClient,szRound);
		}

		for(int i=0; nChildCount > i; i++)
		{
			pControl = (CControlUI*)m_Items[i];
			rcChildClient = pControl->GetItem();
			szRound = pControl->GetBorderRound();
			OffsetRect(&rcChildClient, rcThisClient.left - m_szScrollOffset.cx ,
				rcThisClient.top - m_szScrollOffset.cy);
			//���н������㣬������ӿؼ�����Ч��ͼ����
			if(TRUE == IntersectRect(&rcChildPaint, &rcChildClient, &rcThisClientPaint))
			{
				//ӵ���ཻ������ܻ���
				hChildOldClip = NULL;
				if(szRound.cx || szRound.cy)
				{
					hChildOldClip = m_pShareInfo->pRenderEngine->EnterClip(rcChildClient,szRound);
				}
				
				pControl->OnPaint(rcChildClient ,rcChildPaint, rcUpdate);
				m_pShareInfo->pRenderEngine->LeaveClip(hChildOldClip);
			}
		}
		m_pShareInfo->pRenderEngine->LeaveClip(hClientOldClip);

finish:
		m_pShareInfo->pRenderEngine->LeaveClip(hItemOldClip);
		return true;
	}
}