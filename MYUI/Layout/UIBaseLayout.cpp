
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

	int CBaseLayoutUI::GetCount()
	{
		return m_Items.GetSize();
	}

	CControlUI * CBaseLayoutUI::FindControlByPoint(POINT &Point)
	{
		CControlUI * temp;
		POINT ptThis = Point;//保存起来
		RECT rcChildItem;

		int nCount = m_Items.GetSize();
		CControlUI * target = __super::FindControlByPoint(Point);

		//this != target 表示target可能为滚动条或者NULL，所以返回
		//true == m_bControl表示布局正在模拟控件，不需要查阅坐标是否在子控件，所以返回
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
			Point.x -= rcChildItem.left - m_szScrollOffset.cx;
			Point.y -= rcChildItem.top - m_szScrollOffset.cy;
			temp = pControl->FindControlByPoint(Point);
			if(temp)
			{
				target = temp;
				break;
			}
			Point = ptThis;//不能够获取成功，要还原参数
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

		//客户区域才是提供给子控件有效绘图区域
		//OffsetRect(&rcThisClient, rcItem.left, rcItem.top);
		//提取客户区的有效显示区域

		for(int i=0; nChildCount > i; i++)
		{
			hChildOldClip = NULL;
			pControl = (CControlUI*)m_Items[i];
			rcChildItem = pControl->GetItem();

			if (false == pControl->IsVisible())
			{
				continue;
			}
			
			//求子控件的显示区域
			if(FALSE == ::IntersectRect(&rcChildPaint, &rcChildItem, &m_rcClient))
			{
				continue;
			}

			//求子控件的更新区域
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