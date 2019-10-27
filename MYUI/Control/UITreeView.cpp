
#include "UITreeView.h"

namespace MYUI
{

/***************************************************************************
 * CTreeNodeReinUI 类 CTreeViewUI中的节点
 * CTreeNodeReinUI 设计格式
 *    ______________________________
 *    | > | √ |  text             |
 *    ------------------------------
 * 符号 > ：默认是 > 点击后变成向下
 * 符号 √：默认是什么都没有的，当鼠标点击变成STATE_CHECK状态时，显示√
 * text   : 就是控件的文本
***************************************************************************/


/***************************************************************************
 * CTreeNodeUI 类 CTreeViewUI中的节点
 * CTreeNodeUI 设计格式
 *    _________________________________
 *    | > | √ |  text                |  = CTreeNodeReinUI
 *    ---------------------------------
 *        |   子节点 1                |  = CVerticalLayoutUI::m_items[0]
 *        -----------------------------
 *        |   子节点 2                |  = CVerticalLayoutUI::m_items[1]
 *        -----------------------------
 *        |   子节点 3                |  = CVerticalLayoutUI::m_items[2]
 *        -----------------------------
***************************************************************************/
	CTreeNodeUI::CTreeNodeUI()
		: m_bExpanded(false)
		, m_refHotTextColor(NULL)
		, m_refSelectTextColor(NULL)
		, m_refPushedTextColor(NULL)
		, m_dwPointIn(NULL)
		, m_nExpandOffset(-1)
		//, m_refHotColor(ARGB(255,173, 214,255))
		//, m_refFocusSelectColor(ARGB(255,0,122,204))
		//, m_refNormalSelectColor(ARGB(255,204,206,219))
		, m_refHotColor(ARGB(255,173, 214,255))
		, m_refFocusSelectColor(ARGB(255,0,122,204))
		, m_refNormalSelectColor(ARGB(255,51,153,250))
	{
		EmptyRect(m_rcTextPadding);
		SetHeight(22);
	}

	CTreeNodeUI::~CTreeNodeUI()
	{
		TRACE(_T("CTreeNodeUI::~CTreeNodeUI"));
		for(int i=0; m_Nodes.GetSize() > i; i++)
		{
			CControlUI * pControl = (CControlUI*)m_Nodes[i];
			delete pControl;
		}
	}

	CMuiString CTreeNodeUI::g_strClassName(_T("TreeNodeUI"));

	CMuiString CTreeNodeUI::GetClassName() const
	{
		return CTreeNodeUI::g_strClassName;
	}

		//注意这里有个不管add或者remove是否成功，都会调整
	//子控件父窗口的bug，现在暂不处理
	bool CTreeNodeUI::Add(CControlUI* pControl)
	{
		int nSize = m_Nodes.GetSize();
		return AddAt(pControl, nSize);
	}

	bool CTreeNodeUI::AddAt(CControlUI* pControl,int iIndex)
	{
		pControl->SetParent(this);
		return m_Nodes.InsertAt(iIndex,pControl);
	}

	bool CTreeNodeUI::Remove(CControlUI* pControl)
	{
		int nIndex = m_Nodes.Find(pControl);
		return RemoveAt(nIndex);
	}

	bool CTreeNodeUI::RemoveAt(int nIndex)
	{
		CControlUI * pControl = (CControlUI*)m_Nodes[nIndex];
		pControl->SetParent(nullptr);
		return m_Nodes.Remove(nIndex);
	}

	void CTreeNodeUI::RemoveAll()
	{
		int size = m_Nodes.GetSize();
		while(size--)
		{
			RemoveAt(size);
		}
	}

	CControlUI * CTreeNodeUI::Find(int nIndex)
	{
		return (CControlUI *) m_Nodes[nIndex];
	}

	int CTreeNodeUI::Find(CControlUI * pControl)
	{
		return m_Nodes.Find(pControl);
	}

	int CTreeNodeUI::GetCount() const
	{
		return m_Nodes.GetSize();
	}

	void CTreeNodeUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("ExpandNormalImage"))) 
		{
			SetExpandNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("ExpandHotImage"))) 
		{
			SetExpandHotImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("UnExpandNormalImage"))) 
		{
			SetUnExpandNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("UnExpandHotImage"))) 
		{
			SetUnExpandHotImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("CheckNormalImage"))) 
		{
			SetCheckNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("CheckHotImage"))) 
		{
			SetCheckHotImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("UnCheckNormalImage"))) 
		{
			SetUnCheckNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("UnCheckHotImage"))) 
		{
			SetUnCheckHotImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("hottextcolor"))) 
		{
			SetHotTextColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("selecttextcolor"))) 
		{
			SetSelectTextColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("pushedtextcolor"))) 
		{
			SetPushedTextColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("textPadding"))) 
		{
			SetTextPadding(CMuiRect(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("expandOffset")))
		{
			SetExpandOffset(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("expanded")))
		{
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			SetExpanded(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("showcheck")))
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MTNS_SHOWCHECK;
			}
			else
			{
				dwStyle &= ~MTNS_SHOWCHECK;
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("hideExpand")))
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MTNS_HIDEEXPAND;
			}
			else
			{
				dwStyle &= ~MTNS_HIDEEXPAND;
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("onlyRead")))
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MTNS_READONLY;
			}
			else
			{
				dwStyle &= ~MTNS_READONLY;
			}
			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("NormalSelectColor")))
		{
			SetNormalSelectColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("FocusSelectColor")))
		{
			SetFocusSelectColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("HotColor")))
		{
			SetHotColor(CMuiColor(strValue));
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
		return ;
	}

	void CTreeNodeUI::SetTextPadding(const RECT &rect)
	{
		m_rcTextPadding = rect;
	}

	const RECT &CTreeNodeUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CTreeNodeUI::SetExpandOffset(int nOffset)
	{
		m_nExpandOffset = nOffset;
	}

	int CTreeNodeUI::GetExpandOffset() const
	{
		return m_nExpandOffset;
	}

	void CTreeNodeUI::SetExpanded(bool bExpand)
	{
		if(bExpand == m_bExpanded) return;
		m_bExpanded = bExpand;
		if(m_pShareInfo && m_pShareInfo->hWnd)
		{
			this->Update();
		}
	}

	bool CTreeNodeUI::GetExpanded() const
	{
		return m_bExpanded;
	}

	void CTreeNodeUI::SetChecked(bool bCheck)
	{
		this->OnEvent(TINE_SETCHECK, (LPVOID)!!bCheck, NULL);
		if(MTNS_SHOWCHECK & m_dwStyle)
		{
			this->Invalidate();
		}
	}

	bool CTreeNodeUI::GetChecked() const
	{
		return STATE_CHECKED & GetState();
	}

	void CTreeNodeUI::SetExpandNormalImage(LPCTSTR strImage)
	{
		m_strExpandNormalImage = strImage;
	}

	LPCTSTR CTreeNodeUI::GetExpandNormalImage() const
	{
		return m_strExpandNormalImage;
	}

	void CTreeNodeUI::SetExpandHotImage(LPCTSTR strImage)
	{
		m_strExpandHotImage = strImage;
	}

	LPCTSTR CTreeNodeUI::GetExpandHotImage() const
	{
		return m_strExpandHotImage;
	}

	void CTreeNodeUI::SetUnExpandNormalImage(LPCTSTR strImage)
	{
		m_strUnExpandNormalImage = strImage;
	}

	LPCTSTR CTreeNodeUI::GetUnExpandNormalImage() const
	{
		return m_strUnExpandNormalImage;
	}

	void CTreeNodeUI::SetUnExpandHotImage(LPCTSTR strImage)
	{
		m_strUnExpandHotImage = strImage;
	}

	LPCTSTR CTreeNodeUI::GetUnExpandHotImage() const 
	{
		return m_strUnExpandHotImage;
	}

	void CTreeNodeUI::SetCheckNormalImage(LPCTSTR strImage)
	{
		m_strCheckNormalImage = strImage;
	}

	LPCTSTR CTreeNodeUI::GetCheckNormalImage() const
	{
		return m_strCheckNormalImage;
	}

	void CTreeNodeUI::SetCheckHotImage(LPCTSTR strImage)
	{
		m_strCheckHotImage = strImage;
	}

	LPCTSTR CTreeNodeUI::GetCheckHotImage() const 
	{
		return m_strCheckHotImage;
	}

	void CTreeNodeUI::SetUnCheckNormalImage(LPCTSTR strImage)
	{
		m_strUnCheckNormalImage = strImage;
	}

	LPCTSTR CTreeNodeUI::GetUnCheckNormalImage() const
	{
		return m_strUnCheckNormalImage;
	}

	void CTreeNodeUI::SetUnCheckHotImage(LPCTSTR strImage)
	{
		m_strUnCheckHotImage = strImage;
	}

	LPCTSTR CTreeNodeUI::GetUnCheckHotImage() const 
	{
		return m_strUnCheckHotImage;
	}

	void CTreeNodeUI::SetHotTextColor(ARGBREF argb)
	{
		m_refHotTextColor = argb;
	}

	ARGBREF CTreeNodeUI::GetHotTextColor()
	{
		return m_refHotTextColor;
	}

	void CTreeNodeUI::SetSelectTextColor(ARGBREF argb)
	{
		m_refSelectTextColor = argb;
	}

	ARGBREF CTreeNodeUI::GetSelectTextColor()
	{
		return m_refSelectTextColor;
	}

	void CTreeNodeUI::SetPushedTextColor(ARGBREF argb)
	{
		m_refPushedTextColor = argb;
	}

	ARGBREF CTreeNodeUI::GetPushedTextColor()
	{
		return m_refPushedTextColor;
	}

	void CTreeNodeUI::SetNormalSelectColor(ARGBREF refColor)
	{
		m_refNormalSelectColor = refColor;
	}

	ARGBREF CTreeNodeUI::GetNormalSelectColor() const
	{
		return m_refNormalSelectColor;
	}

	void CTreeNodeUI::SetFocusSelectColor(ARGBREF refColor)
	{
		m_refFocusSelectColor = refColor;
	}

	ARGBREF CTreeNodeUI::GetFocusSelectColor() const
	{
		return m_refFocusSelectColor;
	}

	void CTreeNodeUI::SetHotColor(ARGBREF refColor)
	{
		m_refHotColor = refColor;
	}

	ARGBREF CTreeNodeUI::GetHotColor() const
	{
		return m_refHotColor;
	}

	SIZE CTreeNodeUI::GetValidSize()
	{
		//内边距是计算在内的
		SIZE size;
		SIZE szContent = {0};
		SIZE siValidSize;
		CControlUI * pControl = nullptr;
		//先计算出rein的尺寸，
		size.cx = EnumFloatType::FloatSizeCx & m_FloatType? m_ItemSize.cx : 0;
		size.cy = EnumFloatType::FloatSizeCy & m_FloatType? m_ItemSize.cy : 0;

		if(true == m_bExpanded)
		{
			int nControlCount = m_Nodes.GetSize();
			for(int i=0; nControlCount > i; i++)
			{
				pControl =  (CControlUI*)m_Nodes[i];
			
				if(false == pControl->IsVisible())
				{
					continue;
				}
				siValidSize = pControl->GetValidSize();
				szContent.cx = MAX(szContent.cx, siValidSize.cx);
				szContent.cy += siValidSize.cy;
			}
		}
		
		size.cx += MAX(szContent.cx, size.cx);
		size.cy += szContent.cy;

		size.cx = MAX(m_SizeMin.cx, MIN(size.cx, m_SizeMax.cx));
		size.cy = MAX(m_SizeMin.cy, MIN(size.cy, m_SizeMax.cy));
		size.cx += m_rcMargin.left + m_rcMargin.right;
		size.cy += m_rcMargin.top + m_rcMargin.bottom;

		return size;
	}

	void CTreeNodeUI::SetShareInfo(TSHAREINFO * pShareInfo)
	{
		__super::SetShareInfo(pShareInfo);

		for(int i=0; m_Nodes.GetSize() > i; i++)
		{
			CControlUI * pControl = (CControlUI*)m_Nodes[i];
			pControl->SetShareInfo(pShareInfo);
		}
	}

	bool CTreeNodeUI::SetItem(RECT rcItem,bool bMustUpdate)
	{
/***************************************************************************
 * CTreeNodeUI 设计格式
 * 注意：我们在SetItem的时候不设置Expand偏移，在OnPaint的时候再设置偏移
 * 如果在SetItem中设置Expand偏移，点击空旷的Expand区域时，
 * FindControlByPoint 将找不到匹配的控件
 *    _________________________________
 *    | > | √ |  text                |  = CTreeNodeUI/CTreeNodeReinUI
 *    - E -----------------------------
 *    | x |   子节点 1                |  = CTreeNodeUI::m_Nodes[0]
 *    | p -----------------------------
 *    | a |   子节点 2                |  = CTreeNodeUI::m_Nodes[1]
 *    | n -----------------------------
 *    | d |   子节点 3                |  = CTreeNodeUI::m_Nodes[2]
 *    ---------------------------------
***************************************************************************/

		
		//计算子控件的打印区域，要注意子控件的高，是否显示
		RECT rcThisRegoin;//根据客户区计算出的子控件显示区域
		RECT rcChildItem;
		RECT rcChildMargin;
		POINT ptNextPos={0};//下一个控件的位置
		SIZE siValidSize;

		CControlUI * pControl = nullptr;

		//如果控件不存在指定高度，那么就与rein一样高！
		int nThisHeight = this->GetHeight();
		int nExpandOffset = -1 == m_nExpandOffset ? nThisHeight : m_nExpandOffset;
		
		EnumFloatType type = EnumFloatType::FloatNon;
		int nControlCount = m_Nodes.GetSize();
		if(false == __super::SetItem(rcItem, bMustUpdate)) return false;
		if(nControlCount == 0 ) return true;//布局中没有其他控件就直接返回

		if(true == m_bExpanded)
		{
			m_rcClient.bottom = m_rcClient.top + nThisHeight;
		}
		TRACE(_T("CTreeNodeUI::m_rcClient = %d,%d,%d,%d"), m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom);
		//先设置内缩进
		rcThisRegoin = m_rcRawItem;
		//注意：我们在SetItem的时候不设置Expand偏移，在OnPaint的时候再设置偏移
		ptNextPos.x = rcThisRegoin.left + nExpandOffset;
		ptNextPos.y = rcThisRegoin.top + nThisHeight;

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Nodes[i];
			//置空
			EmptyRect(rcChildItem);
			
			if(false == m_bExpanded || false == pControl->IsVisible())
			{
				//没有展开或者不显示打印区域就是空了
				goto loop;
			}
			
			rcChildItem.left = ptNextPos.x;
			rcChildItem.top = ptNextPos.y;

			siValidSize = pControl->GetValidSize();
			type =  pControl->GetFloatType();
			if(EnumFloatType::FloatSizeCx & type)//指定宽度，其实最好不要指定
			{
				rcChildItem.right = rcChildItem.left + siValidSize.cx;
			}
			else
			{
				rcChildItem.right = rcThisRegoin.right + nExpandOffset;
			}

			if(EnumFloatType::FloatSizeCy & type)//指定了高度
			{
				rcChildItem.bottom = rcChildItem.top + siValidSize.cy;
			}
			else//不指定就使用Rein的高度
			{
				rcChildItem.bottom += rcChildItem.top + nThisHeight;
			}

			ptNextPos.y += rcChildItem.bottom - rcChildItem.top;
			//减去外边距
			rcChildMargin = pControl->GetMargin();
			IndentRect(&rcChildItem, &rcChildMargin);
loop:
            pControl->SetItem(rcChildItem, false);
		}

		return true;
	}

	bool CTreeNodeUI::OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate)
	{
		SIZE szRound = {0};
		//这个rcItem跟m_rcItem的值有点不同，
		//m_rcItem是相对于父控件的位置
		//rcItem是相对整个窗口的位置，是一个绝对位置
		
		RECT rcChildPaint, rcChildClient;
		RECT rcThisClient, rcThisClientPaint;
		CControlUI * pControl = NULL;

		int nChildCount = m_Nodes.GetSize();
		HCLIP hChildOldClip = NULL, hItemOldClip = NULL;

		if(true == m_bExpanded && nChildCount > 0)
		{
			hItemOldClip = m_pShareInfo->pRenderEngine->EnterClip(rcItem, m_szBorderRound);
		}

		if(false == __super::OnPaint(rcItem, rcPaint, rcUpdate))//绘制rein
		{
			goto finish;
		}

		if(false == m_bExpanded) goto finish;//没有展开
		/*绘制child start*/

		rcThisClient = m_rcRawItem;
		//客户区域才是提供给子控件有效绘图区域
		//注意：我们在SetItem的时候不设置Expand偏移，在OnPaint的时候再设置偏移

		OffsetRect(&rcThisClient, rcItem.left, rcItem.top);
		//提取客户区的有效显示区域
		if(FALSE == IntersectRect(&rcThisClientPaint, &rcThisClient , &rcPaint))
		{
			goto finish;
		}

		for(int i=0; nChildCount > i; i++)
		{
			pControl = (CControlUI*)m_Nodes[i];
			rcChildClient = pControl->GetItem();
			szRound = pControl->GetBorderRound();
			OffsetRect(&rcChildClient, rcThisClient.left, rcThisClient.top);
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
		/*绘制child end*/
finish:
		m_pShareInfo->pRenderEngine->LeaveClip(hItemOldClip);
		return true;
		
	}

	void CTreeNodeUI::PaintBkColor( const RECT& rcItem, const RECT& rcPaint)
	{
		return;
	}

	void CTreeNodeUI::PaintBkImage( const RECT& rcItem, const RECT& rcPaint)
	{
		return;
	}

	void CTreeNodeUI::PaintStatusImage( const RECT& rcItem, const RECT& rcPaint)
	{
		RECT rcDraw = m_rcClient;
		LPCTSTR strImage = NULL;
		LPCTSTR strExpandText = _T("+");
		int nThisHeight = this->GetHeight();

		if(STATE_UNKNOW & m_dwState) return;

		if(FALSE == (MTNS_HIDEEXPAND & m_dwStyle) || m_Nodes.GetSize() > 0)
		{
			strImage = _T('\0');
			if(true == m_bExpanded)//展开
			{
				strExpandText = _T("-");
				strImage = m_strExpandNormalImage;
				if(TNPI_EXPANDED == m_dwPointIn && !m_strExpandHotImage.IsEmpty())
				{
					strImage = m_strExpandHotImage;
				}
			}
			else//没有展开
			{
				strExpandText = _T("+");
				strImage = m_strUnExpandNormalImage;
				if(TNPI_EXPANDED == m_dwPointIn && !m_strUnExpandHotImage.IsEmpty())
				{
					strImage = m_strUnExpandHotImage;
				}
			}

			rcDraw.right = rcDraw.left + (m_rcClient.bottom - m_rcClient.top);
			OffsetRect(&rcDraw, rcItem.left, rcItem.top);
			if(_T('\0') == strImage[0])
			{
				m_pShareInfo->pRenderEngine->OnDrawText(rcDraw, strExpandText, 0xFF000000, 
					NULL, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
			else
			{
				m_pShareInfo->pRenderEngine->OnDrawImage(rcDraw, strImage);
			}
		}

		if(MTNS_SHOWCHECK & m_dwStyle)//显示check
		{
			strImage = _T('\0');
			if(STATE_CHECKED & m_dwState)//选中
			{
				strImage = m_strCheckNormalImage;
				if(TNPI_CHECKED == m_dwPointIn && !m_strCheckHotImage.IsEmpty())
				{
					strImage = m_strCheckHotImage;
				}
			}
			else//没选中
			{
				strImage = m_strUnCheckNormalImage;
				if(TNPI_CHECKED == m_dwPointIn && !m_strUnCheckHotImage.IsEmpty())
				{
					strImage = m_strUnCheckHotImage;
				}
			}

			if(_T('\0') != strImage[0])//图片拿到了
			{
				rcDraw = m_rcClient;
				rcDraw.right = rcDraw.left + nThisHeight + m_rcTextPadding.left;
				int nOffset = nThisHeight;
				nOffset += nOffset / 10;
				OffsetRect(&rcDraw, rcItem.left + nOffset, rcItem.top);
				
				m_pShareInfo->pRenderEngine->OnDrawImage(rcDraw, strImage);
			}
		}
	}

	void CTreeNodeUI::PaintText(const RECT& rcItem, const RECT& rcPaint)
	{
		int nOffset = 0;
		ARGBREF refTextColor = m_refTextColor;
		RECT rcClient = m_rcClient;
		HFONT hFont = (HFONT)m_pShareInfo->FontArray->Select(m_nFontId);
		if(m_strText.IsEmpty()) return ;

		DWORD dwDrawTextStyle = DT_SINGLELINE | DT_LEFT | DT_VCENTER;

		if(STATE_UNKNOW & m_dwState) return;

		nOffset = m_rcClient.bottom - m_rcClient.top;
		nOffset += nOffset / 10;
		
		if(MTNS_SHOWCHECK & m_dwStyle)//显示check
		{
			nOffset *= 2;
		}
		rcClient.left += nOffset;
		OffsetRect(&rcClient,rcItem.left, rcItem.top);
		if(FALSE == IndentRect(&rcClient, &m_rcTextPadding)) return ;

		if(false == m_bEnabled)
		{
			//注意判断顺序
			if(m_dwState & STATE_HOT && NULL != m_refHotTextColor)
			{
				refTextColor = m_refHotTextColor;
			}

			if(m_dwState & STATE_SELECT && NULL != m_refSelectTextColor)
			{
				refTextColor = m_refSelectTextColor;
			}

			if(m_dwState & STATE_PUSHED && m_dwState & STATE_HOT && NULL != m_refPushedTextColor)
			{
				refTextColor = m_refPushedTextColor;
			}
		}

		m_pShareInfo->pRenderEngine->OnDrawText(rcClient, m_strText,
			refTextColor, hFont, dwDrawTextStyle);
	}

	bool CTreeNodeUI::Update()
	{
        return this->Renewal();
	}

    bool CTreeNodeUI::Renewal()
    {
        if (NULL == m_pParentContrl)
        {
            return CControlUI::Renewal();
        }

        return m_pParentContrl->Renewal();
    }

	bool CTreeNodeUI::Invalidate()
	{
        if (NULL == m_pParentContrl)
        {
            return this->Invalidate();
        }
        return m_pParentContrl->Invalidate();
	}

	DWORD CTreeNodeUI::CalcPointIn(const POINT& pt)
	{
		int nThisHeight = this->GetHeight();
		RECT rcRein = m_rcClient;

		rcRein.right = rcRein.left + nThisHeight;
		if(TRUE == PointInRect(pt, rcRein))
		{
			return TNPI_EXPANDED;
		}

		if(MTNS_SHOWCHECK & m_dwStyle)
		{
			OffsetRect(&rcRein, nThisHeight + (nThisHeight / 10), 0);
			rcRein.right += m_rcTextPadding.left;
			if(TRUE == PointInRect(pt, rcRein))
			{
				return TNPI_CHECKED;
			}
		}

		rcRein.left = rcRein.right;
		rcRein.right = m_rcClient.right - m_rcTextPadding.right;
		if(TRUE == PointInRect(pt, rcRein))
		{
			return TNPI_TEXT;
		}
		return NULL;
	}

	CControlUI * CTreeNodeUI::FindControlByPoint(POINT &pt)
	{
		POINT ptThis = pt;//保存起来
		RECT rcChildItem;
		int nExpandOffset = m_nExpandOffset;
		CControlUI * target = __super::FindControlByPoint(pt);
		int nCount = m_Nodes.GetSize();
		if(target != this) return NULL;

		if(PointInRect(pt, m_rcClient)) return this;//in the rein

		if(false == m_bExpanded) return target;//没有展开就不用检测子控件了

		//检查一下坐标是否还存在于子控件中，如果是，则返回子控件
		//注意，因为绘制时，是按顺序绘制的，如果控件位置发生重叠
		//先绘制的控件会被后绘制的控件所覆盖
		//所以我们计算point指向的控件时，要进行反方向计算
		//在其他计算函数中，也要注意同样的问题，计算方向一定要与绘制方式相反
		nExpandOffset = -1 == m_nExpandOffset ? this->GetHeight() : m_nExpandOffset;
		while(nCount--)//倒序轮询
		{
			CControlUI * pControl = (CControlUI *)m_Nodes[nCount];
			if(false == pControl->IsVisible())
			{
				continue;
			}
			rcChildItem = pControl->GetItem();
			pt.x -= rcChildItem.left - nExpandOffset;
			pt.y -= rcChildItem.top;
			target = pControl->FindControlByPoint(pt);

			if(target) break;
			target = this;//还原
			pt = ptThis;//不能够获取成功，要还原参数，避免用户误会
		}

		pt.x -= nExpandOffset;
		return target;
	}

	LRESULT CTreeNodeUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		DWORD dwOldPointIn = m_dwPointIn;
		switch (message)
		{
		case WM_LBUTTONDOWN:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				m_dwPointIn = CalcPointIn(pt);
				TRACE(_T("CTreeNodeUI CalcPointIn = 0x%04x, text=%s"), m_dwPointIn, m_strText);
				if(TNPI_EXPANDED == m_dwPointIn)
				{
					SetExpanded(!m_bExpanded);
				}
				else if(TNPI_CHECKED == m_dwPointIn)
				{
					SetChecked(!GetChecked());
				}
				else
				{
					this->AddState(STATE_SELECT);
					this->Invalidate();
				}
			}break;
		case WM_LBUTTONDBLCLK:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				m_dwPointIn = CalcPointIn(pt);
				if(TNPI_TEXT == m_dwPointIn || NULL == m_dwPointIn)
				{
					SetExpanded(!m_bExpanded);
				}
			}break;
		case WM_MOUSEMOVE:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				m_dwPointIn = CalcPointIn(pt);

				TRACE(_T("CTreeNodeUI CalcPointIn = 0x%04x, text=%s"), m_dwPointIn, m_strText);

				if(m_dwPointIn != dwOldPointIn)
				{
					this->Invalidate();
				}
			}break;
		case WM_MOUSELEAVE:
			{
				m_dwPointIn = NULL;
				if(m_dwPointIn != dwOldPointIn)
				{
					this->Invalidate();
				}
			}break;
		case WMU_SETCHECK:
			{
				if(FALSE == wParam)
				{
					m_dwState &= ~STATE_CHECKED;
				}
				else
				{
					m_dwState |= STATE_CHECKED;
				}
			}break;
		default:
			break;
		}
		return 0;
	}

	bool CTreeNodeUI::OnEvent(DWORD dwEvent, LPVOID wParam, LPVOID lParam)
	{
		int nChildCount = m_Nodes.GetSize();
		CControlUI * pControl = this;
		CNodeInterface * pNodeInterface = NULL;

		switch (dwEvent)
		{
		case TINE_DRAWNODEBK:
			{
				//if(pNodeInterface = dynamic_cast<CNodeInterface *>(pControl))
				OnDrawNodeBk(*(RECT*)wParam, *(RECT*)lParam);
				return false;
			}break;
		case TINE_SETCHECK:
			{
				pControl->CallWndProc(NULL, WMU_SETCHECK, (WPARAM)wParam, (LPARAM)NULL);
				if(MTNS_CHECKCUR & m_dwStyle)
				{
					return true;//只选择当前项
				}
			}break;
		default:
			{
				if(true == CTreeViewUI::HandleNodeEvent(pControl, dwEvent, wParam, lParam))
				{
					return true;
				}
			}break;
		}

		//向下转发一波
		for(int i=0; nChildCount > i; i++)
		{
			pControl = (CControlUI*)m_Nodes[i];
			if(pNodeInterface = dynamic_cast<CNodeInterface *>(pControl))
			{
				if(true == pNodeInterface->OnEvent(dwEvent, wParam, lParam))
				{
					return true;
				}
			}
			else
			{
				if(true == CTreeViewUI::HandleNodeEvent(pControl, dwEvent, wParam, lParam))
				{
					return true;
				}
			}
		}
		return false;
	}

	void CTreeNodeUI::OnDrawNodeBk(const RECT &rcItem, const RECT &rcPaint)
	{
		//这个绘制NodeBk绘制模板，大家可以Copy一下，写到你的自定义节点控件里面
		//之所以要额外定义一个绘制背景的方法，是因为TreeNode的位置都是经过偏移的
		//如果用控件原有的背景进行绘制，那么得到的图像是偏移都的图像，而实际情况
		//中，我们的TreeNode背景，是需要与TreeView对齐的
		RECT rcThisClient = m_rcClient;
		RECT rcChildItem, rcChildPaint;
		SIZE szRound = {0};
		int nChildCount = m_Nodes.GetSize();
		int nExpandOffset = 0 > m_nExpandOffset ? this->GetHeight() : m_nExpandOffset;
		CControlUI* pControl = NULL;
		CLableUI * pLable = NULL;
		CNodeInterface * pNodeInterface = NULL;
		OffsetRect(&rcThisClient, rcItem.left, rcItem.top);
		ARGBREF refColor = NULL;

		if(STATE_HOT & m_dwState && m_refHotColor)
		{
			refColor = m_refHotColor;
		}

		if(STATE_SELECT & m_dwState && m_refNormalSelectColor)
		{
			refColor = m_refNormalSelectColor;
		}

		if(GetAValue(refColor))
		{
			m_pShareInfo->pRenderEngine->OnDrawColor(rcThisClient, refColor);
		}

		//通知子节点
		if(false == m_bEnabled) return;//没有展开

		for(int i = 0; nChildCount > i; i++)
		{
			pControl = (CControlUI*)m_Nodes[i];

			if(pNodeInterface = dynamic_cast<CNodeInterface *>(pControl))
			{
				rcChildItem = pControl->GetItem();

				OffsetRect(&rcChildItem, rcThisClient.left - nExpandOffset , rcThisClient.top );
				//进行交叉运算，计算出子控件的有效绘图区域
				if(TRUE == IntersectRect(&rcChildPaint, &rcChildItem, &rcPaint))
				{
					//拥有相交区域才能绘制
					pNodeInterface->OnEvent(TINE_DRAWNODEBK, &rcChildItem ,&rcChildPaint);
				}
			}
			else
			{
				//没有继承CNodeInterface可能是一片叶子，
				
				//兼容一下CLable控件
				if(pLable = dynamic_cast<CLableUI *>(pControl))
				{
					
				}

				m_pShareInfo->pRenderEngine->OnDrawColor(rcChildItem, refColor);
			}
			
		}
		return;	
	}

/***************************************************************************
 * CTreeViewUI 类 
 * CTreeViewUI 负责管理节点需要说明的是，节点不一定要 CTreeNodeUI，比如
 * 我们熟悉的QQ好友列表，就是TreeNode(第一层) + ListBox(第二层)组成的
 * 但是必须用TreeNode才能添加下一层TreeNode，而ListBox等其他控件不能添加
 * 子节点。（主要控件概念中node与Item的区别）
 *
 *
 *
***************************************************************************/
	CTreeViewUI::CTreeViewUI()
		: m_pSelectCcontrol(NULL)
		, m_pHotControl(NULL)
	{
		m_bControl = true;
		m_bCentered = false;
	}

	CTreeViewUI::~CTreeViewUI()
	{
	}

	CMuiString CTreeViewUI::g_strClassName(_T("TreeViewUI"));

	CMuiString CTreeViewUI::GetClassName() const
	{
		return CTreeViewUI::g_strClassName;
	}

	CControlUI * CTreeViewUI::GetNodeByPoint(POINT &pt)
	{
		POINT ptThis = pt;//保存起来
		RECT rcChildItem;
		CControlUI * target = NULL;
		int nCount = m_Items.GetSize();
		//TRACE(_T("m_Nodes.GetSize() = %d"), nCount);
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
			pt.x += m_szScrollOffset.cx;
			pt.y += m_szScrollOffset.cy;

			if(TRUE == PointInRect(pt, rcChildItem))
			{
				pt.x -= rcChildItem.left;
				pt.y -= rcChildItem.top;
				target = pControl->FindControlByPoint(pt);

				if(target) break;
			}
			pt = ptThis;//不能够获取成功，要还原参数，避免用户误会
		}
		//TRACE(_T("nCount = %d"), nCount);
		return target;
	}

	bool CTreeViewUI::OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate)
	{
		//这个rcItem跟m_rcItem的值有点不同，
		//m_rcItem是相对于父控件的位置
		//rcItem是相对整个窗口的位置，是一个绝对位置
		SIZE szRound = {0};
		RECT rcChildPaint, rcChildItem;
		RECT rcThisClient, rcThisClientPaint;
		CControlUI * pControl;
		CNodeInterface * pNodeInterface;
		int nChildCount = m_Items.GetSize();
		HCLIP hClientOldClip = NULL, hChildOldClip = NULL;

		HCLIP hItemOldClip = m_pShareInfo->pRenderEngine->EnterClip(rcItem, m_szBorderRound);
		if(false == CContainerUI::OnPaint(rcItem ,rcPaint, rcUpdate))
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
			rcChildItem = pControl->GetItem();
			szRound = pControl->GetBorderRound();
			OffsetRect(&rcChildItem, rcThisClient.left - m_szScrollOffset.cx ,
				rcThisClient.top - m_szScrollOffset.cy);
			//进行交叉运算，计算出子控件的有效绘图区域
			if(TRUE == IntersectRect(&rcChildPaint, &rcChildItem, &rcThisClientPaint))
			{
				//拥有相交区域才能绘制
				hChildOldClip = NULL;
				if(szRound.cx || szRound.cy)
				{
					hChildOldClip = m_pShareInfo->pRenderEngine->EnterClip(rcChildItem,szRound);
				}
				
				//绘制背景
				if(pNodeInterface = dynamic_cast<CNodeInterface *>(pControl))
				{
					pNodeInterface->OnEvent(TINE_DRAWNODEBK, &rcChildItem, &rcChildPaint);
				}

				pControl->OnPaint(rcChildItem ,rcChildPaint, rcUpdate);
				m_pShareInfo->pRenderEngine->LeaveClip(hChildOldClip);
			}
		}
		m_pShareInfo->pRenderEngine->LeaveClip(hClientOldClip);

finish:
		m_pShareInfo->pRenderEngine->LeaveClip(hItemOldClip);
		return true;
	}

	void CTreeViewUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("singleSelect"))) 
		{
			DWORD dwStyle = GetStyle();
#ifdef _DEBUG
			ASSERT(CheckBoer(strValue));
#endif
			if(TRUE == CheckTrue(strValue))
			{
				dwStyle |= MTVS_SINGLESELECT;
			}
			else
			{
				dwStyle &= ~MTVS_SINGLESELECT;
			}
			SetStyle(dwStyle);
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}

		return;
	}

	bool CTreeViewUI::HandleNodeEvent(CControlUI *pControl, DWORD dwEvent,LPVOID wParam, LPVOID lParam)
	{
		switch (dwEvent)
		{
		case TINE_DRAWNODEBK:
			{
			}break;
		case TINE_SETSELECT:
			{
				if(FALSE == wParam)
				{
					pControl->RemoveState(STATE_SELECT);
				}
				else
				{
					pControl->AddState(STATE_SELECT);
				}
			}break;
		case TINE_GETNODEINDEX:
			{
				if(lParam == pControl)
				{
					return true;
				}
				else
				{
					//索引++
					(*(int *)wParam)++;
				}
			}
		default:
			break;
		}

		return false;
	}

	void CTreeViewUI::CallNodeEvent(DWORD dwEvent, LPVOID wParam, LPVOID lParam)
	{
		CControlUI * pControl = NULL,* pTmpControl = NULL;
		CNodeInterface * pNodeInterface = NULL;
		int nChildCount = m_Items.GetSize();

		for(int i = 0; nChildCount > i; i++)
		{
			pControl = (CControlUI *)m_Items[i];
			if(pNodeInterface = dynamic_cast<CNodeInterface *>(pControl))
			{
				pNodeInterface->OnEvent(dwEvent, wParam, lParam);
			}
			else
			{
				if(CTreeViewUI::HandleNodeEvent(pControl, dwEvent, wParam, lParam))
				{
					break;
				}
			}
		}
		return;
	}

	LRESULT CTreeViewUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		CControlUI * pControl = NULL,* pTmpControl = NULL;
		CNodeInterface * pNodeInterface = NULL;

		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				if(m_bEnabled && (pControl = GetNodeByPoint(pt)))
				{
					int nState = GetKeyState(VK_CONTROL);
					if(MTVS_SINGLESELECT & m_dwStyle || GetKeyState(VK_CONTROL) >= 0)
					{
						//只能选择一个节点，或者Ctrl键按下没有按下
						//通知所有节点，清除字节的STATE_SELECT状态
						this->CallNodeEvent(TINE_SETSELECT, FALSE, NULL);
					}
					
					pControl->CallWndProc(NULL, message, wParam, MAKELONG(pt.x,pt.y));
					SendNotify(!hWnd, SelectItem, !!(STATE_SELECT & pControl->GetState()), (LPARAM)pControl);
				}
			}break;
		case WM_LBUTTONUP:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);

			}break;
		case WM_LBUTTONDBLCLK:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				if(m_bEnabled && (pControl = GetNodeByPoint(pt)))
				{
					int nIndex = 0;
					pControl->CallWndProc(NULL, message, wParam, MAKELONG(pt.x,pt.y));
					this->CallNodeEvent(TINE_GETNODEINDEX, &nIndex, pControl);
					SendNotify(!hWnd, ActiveItem, nIndex, (LPARAM)pControl);
				}
			}break;
		case WM_MOUSEMOVE:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				
				if(false == m_bEnabled) break;
				pControl = GetNodeByPoint(pt);

				if(pControl != m_pHotControl)
				{
					//当鼠标移动时，有可能离开一个控件，进入另一个控件，在此进行处理
					//切换热点
					pTmpControl = m_pHotControl;
					m_pHotControl = pControl;
					//逻辑：先离开一个房间，才能进入另外一个房间
					//先发出离开通知，
					if(NULL != pTmpControl)
					{
						pTmpControl->CallWndProc(NULL, WM_MOUSELEAVE, 0, 0);
					}

					//再发出进入通知
					if(NULL != pControl)
					{
						pControl->CallWndProc(NULL, WM_MOUSEENTER, wParam, MAKELONG(pt.x, pt.y));
					}
				}

				if(NULL != pControl)
				{
					pControl->CallWndProc(NULL, message, wParam, MAKELONG(pt.x, pt.y));
				}
			}break;
		case WM_MOUSEHOVER:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				//if(pControl = GetItemByPoint(pt)) 
				//{
				//	pControl->WndProc(hWnd, WM_MOUSEHOVER, wParam, MAKELONG(pt.x, pt.y));
				//}
			}break;
		case WM_MOUSELEAVE:
			{
				if(NULL != m_pHotControl)
				{
					pControl = m_pHotControl;
					m_pHotControl = NULL;
					pControl->CallWndProc(hWnd, WM_MOUSELEAVE, 0, 0);
				}
			}break;
		case WM_KEYDOWN:
			{
				switch (wParam)
				{
				case VK_UP:
					{
					}break;
				case VK_DOWN:
					{
					}break;
				case VK_RETURN:
					{
					}break;
				default:
					break;
				}
			}break;
		case WM_RBUTTONDOWN:
			{
			}break;
		
		default:
			break;
		}
		return __super::WndProc(hWnd, message, wParam, lParam);
	}
}