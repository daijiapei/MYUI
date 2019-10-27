
#include "UIBaseLayout.h"


namespace MYUI
{
/***************************************************************************
 * CBaseLayoutUI类，布局基类
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

	//注意这里有个不管add或者remove是否成功，都会调整
	//子控件父窗口的bug，现在暂不处理
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
		POINT ptThis = pt;//保存起来
		RECT rcChildItem;

		int nCount = m_Items.GetSize();
		CControlUI * target = __super::FindControlByPoint(pt);

		//this != target 表示target可能为滚动条或者NULL，所以返回
		//true == m_bControl表示布局正在模拟控件，返回自身指针
		if(this != target || true == m_bControl) return target;

		//检查一下坐标是否还存在于子控件中，如果是，则返回子控件
		
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
			pt.x -= rcChildItem.left - m_szScrollOffset.cx;
			pt.y -= rcChildItem.top - m_szScrollOffset.cy;
			temp = pControl->FindControlByPoint(pt);
			if(temp)
			{
				target = temp;
				break;
			}
			pt = ptThis;//不能够获取成功，要还原参数，避免用户误会
		}
		return target;
	}

	CControlUI * CBaseLayoutUI::FindControlByName(LPCTSTR strName)
	{
		int nCount = m_Items.GetSize();
		CControlUI * target = __super::FindControlByName(strName);

		if(target || true == m_bControl) return target;

		while(nCount--)//倒序轮询
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
		//这个rcItem跟m_rcItem的值有点不同，
		//m_rcItem是相对于父控件的位置
		//rcItem是相对整个窗口的位置，是一个绝对位置
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
		//客户区域才是提供给子控件有效绘图区域
		OffsetRect(&rcThisClient, rcItem.left, rcItem.top);
		//提取客户区的有效显示区域
		if(FALSE == IntersectRect(&rcThisClientPaint, &rcThisClient , &rcPaint))
		{
			goto finish;
		}

		if(FALSE == IsSameRect(m_rcClient, m_rcRawItem))
		{
			//存在滚动条，提供给子控件的区域需要再减去滚动条
			hClientOldClip = m_pShareInfo->pRenderEngine->EnterClip(rcThisClient,szRound);
		}

		for(int i=0; nChildCount > i; i++)
		{
			pControl = (CControlUI*)m_Items[i];
			rcChildClient = pControl->GetItem();
			szRound = pControl->GetBorderRound();
			OffsetRect(&rcChildClient, rcThisClient.left - m_szScrollOffset.cx ,
				rcThisClient.top - m_szScrollOffset.cy);
			//进行交叉运算，计算出子控件的有效绘图区域
			if(TRUE == IntersectRect(&rcChildPaint, &rcChildClient, &rcThisClientPaint))
			{
				//拥有相交区域才能绘制
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