﻿
#include "UIListView.h"

namespace MYUI
{

	static MUILINKED * find_index_for_linked(int index, MUILINKED * link, int size)
	{
		int offset = 0;
		MUILINKED * target = link;

		if(NULL == link || 0 == size) return NULL;
		if(0 == index) return link;

		if(size > index && size > -index)
		{
			if(index > 0)//正数
			{
				if(size / 2 > index)
				{
					offset = index;
					goto gonext;
				}
				else
				{
					offset = size - index;
					goto gopre;
				}
			}
			else//负数
			{
				if((size / 2) > (-index))
				{
					offset = -index;
					goto gopre;
				}
				else
				{
					offset = size + index;
					goto gonext;
				}
			}
			
		}
		else
		{
			goto failed;
		}

gonext:
		
		while(offset)
		{
			target = target->pNext;
			offset --;
		}
		return target;

gopre:
		while(offset)
		{
			target = target->pPrev;
			offset --;
		}
		return target;

failed:
		return NULL;
	}

/*********************************************************************************************
 * CListHeaderUI 
 *
 *
*********************************************************************************************/

	CListHeaderUI::CListHeaderUI()
		: m_pHdRow(NULL)
		, m_nRowCount(0)
	{
		SetWidth(100);
	}

	CListHeaderUI::~CListHeaderUI()
	{
	}

/*
	bool CListHeaderUI::Add(CControlUI* pControl)
	{
		int nSize = m_Nodes.GetSize();
		return AddAt(pControl, nSize);
	}

	bool CListHeaderUI::AddAt(CControlUI* pControl,int iIndex)
	{
		pControl->SetParent(this);
		return m_Nodes.InsertAt(iIndex,pControl);
	}

	bool CListHeaderUI::Remove(CControlUI* pControl)
	{
		int nIndex = m_Nodes.Find(pControl);
		return RemoveAt(nIndex);
	}

	bool CListHeaderUI::RemoveAt(int nIndex)
	{
		CControlUI * pControl = (CControlUI*)m_Nodes[nIndex];
		pControl->SetParent(nullptr);
		return m_Nodes.Remove(nIndex);
	}

	void CListHeaderUI::RemoveAll()
	{
		int size = m_Nodes.GetSize();
		while(size--)
		{
			RemoveAt(size);
		}
	}

	CControlUI * CListHeaderUI::Find(int nIndex)
	{
		return (CControlUI *) m_Nodes[nIndex];
	}

	int CListHeaderUI::Find(CControlUI * pControl)
	{
		return m_Nodes.Find(pControl);
	}

	int CListHeaderUI::GetCount() const
	{
		return m_Nodes.GetSize();
	}

*/

	bool CListHeaderUI::AddRow(MUIHDROWINFO &info, int nRow)
	{
		MUIHEADERROW * pNewRow = NULL;
		MUIHEADERROW * pTmpRow = NULL;
		CListHeaderUI * pHeader = NULL;
		int len = 0;
		if(NULL == m_pHdRow)
		{
			if(0 == nRow || -1 == nRow)
			{
				pNewRow = new MUIHEADERROW;
				pNewRow->Link.pPrev = pNewRow->Link.pNext = (MUILINKED*)pNewRow;
				pNewRow->Info = info;

				if(info.strText)
				{
					len = _tcslen(info.strText);
					pNewRow->Info.strText = new TCHAR[len + 1];
					_tcscpy(pNewRow->Info.strText, info.strText);
				}
				m_pHdRow = pNewRow;
				m_nRowCount ++;
				return true;
			}
		}
		else
		{
			pTmpRow = (MUIHEADERROW*) find_index_for_linked(nRow - 1,&m_pHdRow->Link, m_nRowCount);

			if(pTmpRow)
			{
				pNewRow = new MUIHEADERROW;
				pNewRow->Link.pPrev = pNewRow->Link.pNext = (MUILINKED*)pNewRow;
				pNewRow->Info = info;

				if(info.strText)
				{
					len = _tcslen(info.strText);
					pNewRow->Info.strText = new TCHAR[len + 1];
					_tcscpy(pNewRow->Info.strText, info.strText);
				}

				pNewRow->Link.pPrev = (MUILINKED*)pTmpRow;
				pNewRow->Link.pNext = pTmpRow->Link.pNext;

				pTmpRow->Link.pNext->pPrev = (MUILINKED*)pNewRow;
				pTmpRow->Link.pNext = (MUILINKED*)pNewRow;
				
				
				m_nRowCount ++;
				if(0 == nRow) m_pHdRow = pNewRow;
				return true;
			}
			
		}

		return false;
	}

	bool CListHeaderUI::GetRow(MUIHDROWINFO &info, int nRow)
	{
		MUIHEADERROW * pRow;
		pRow = (MUIHEADERROW*) find_index_for_linked(nRow ,&m_pHdRow->Link, m_nRowCount);

		if(pRow)
		{
			info = pRow->Info;
			return true;
		}

		return false;
	}

	bool CListHeaderUI::SetRow(MUIHDROWINFO &info, int nRow)
	{
		MUIHEADERROW * pRow;
		pRow = (MUIHEADERROW*) find_index_for_linked(nRow ,&m_pHdRow->Link, m_nRowCount);

		if(pRow)
		{
			pRow->Info.dwState = info.dwState;
			pRow->Info.lParam = info.lParam;
			pRow->Info.refBkColor = info.refBkColor;

			//如果info.strText为空，不会删除原来的strText
			if(info.strText && info.strText != pRow->Info.strText)
			{
				if(pRow->Info.strText)
				{
					delete pRow->Info.strText;
					pRow->Info.strText = NULL;
				}

				int len = _tcslen(info.strText);
				pRow->Info.strText = new TCHAR[len + 1];
				_tcscpy(pRow->Info.strText, info.strText);
			}
			
			return true;
		}

		return false;
	}

	bool CListHeaderUI::DelRow(int nRow)
	{
		MUIHEADERROW * pRow;
		MUIHEADERROW * pPreRow, * pNextRow;
		pRow = (MUIHEADERROW*) find_index_for_linked(nRow, &m_pHdRow->Link, m_nRowCount);

		if(pRow)
		{
			pPreRow = (MUIHEADERROW *)pRow->Link.pPrev;
			pNextRow = (MUIHEADERROW *)pRow->Link.pNext;

			pPreRow->Link.pNext = pRow->Link.pNext;
			pNextRow->Link.pPrev = pRow->Link.pPrev;

			if(pRow == m_pHdRow)
			{
				if(1 == m_nRowCount)
				{
					//唯一一行删除了
					m_pHdRow = NULL;
				}
				else
				{
					m_pHdRow = pNextRow;
				}
			}

			m_nRowCount--;
			delete pRow;
			return true;
		}

		return false;
	}




/*********************************************************************************************
 * CListViewUI 
 *
 *
*********************************************************************************************/

	CListViewUI::CListViewUI()
		: m_nHeaderHeight(24)
		, m_nInfoBarWidth(20)
		, m_refGridLinesColor(ARGB(255,200,200,200))
		, m_refBetweenLinesColor(ARGB(255,200,200,200))
		, m_refBetweenRowColor(NULL)
		, m_pLvRow(NULL)
		, m_nRowCount(0)
		, m_nRowAutoHeight(28)
		, m_pHotHeader(NULL)
		, m_pPushedHeader(NULL)
		, m_dwFocus(LVDF_NON)
	{
		m_dwStyle |= MLVS_SHOWINFOBAR;
		m_refNormalColor = ARGB(255,156,199,230);
		m_refSelectColor = ARGB(255,0,122,204);
	}

	CListViewUI::~CListViewUI()
	{
	}

	const CMuiString CListViewUI::g_strClassName = _T("ListViewUI");

	CMuiString CListViewUI::GetClassName() const
	{
		return CListViewUI::g_strClassName;
	}

	void CListViewUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("HeaderBkColor"))) 
		{
			SetHeaderBkColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("HeaderBkImage"))) 
		{
			SetHeaderBkImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("HeaderHeight"))) 
		{
			SetHeaderHeight(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("InfoBarWidth"))) 
		{
			SetInfoBarWidth(_tcstol(strValue, NULL, 10));
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
	}

	bool CListViewUI::Add(CControlUI* pControl)
	{
		int nSize = m_Headers.GetSize();
		return AddAt(pControl, nSize);
	}

	bool CListViewUI::AddAt(CControlUI* pControl,int iIndex)
	{
		pControl->SetParent(this);
		return m_Headers.InsertAt(iIndex,pControl);
	}

	bool CListViewUI::Remove(CControlUI* pControl)
	{
		int nIndex = m_Headers.Find(pControl);
		return RemoveAt(nIndex);
	}

	bool CListViewUI::RemoveAt(int nIndex)
	{
		CControlUI * pControl = (CControlUI*)m_Headers[nIndex];
		pControl->SetParent(nullptr);
		return m_Headers.Remove(nIndex);
	}

	void CListViewUI::RemoveAll()
	{
		int size = m_Headers.GetSize();
		while(size--)
		{
			RemoveAt(size);
		}
	}

	CControlUI * CListViewUI::Find(int nIndex)
	{
		return (CControlUI *) m_Headers[nIndex];
	}

	int CListViewUI::Find(CControlUI * pControl)
	{
		return m_Headers.Find(pControl);
	}

	int CListViewUI::GetCount()
	{
		return m_Headers.GetSize();
	}

	int CListViewUI::GetColumnCount()
	{
		return m_Headers.GetSize();
	}

	int CListViewUI::GetRowCount()
	{
		return m_nRowCount;
	}

	void CListViewUI::SetShareInfo(MUISHAREINFO * pShareInfo)
	{
		__super::SetShareInfo(pShareInfo);

		for(int i=0 ; m_Headers.GetSize() > i; i++)
		{
			CControlUI * pControl = (CControlUI*)m_Headers[i];
			pControl->SetShareInfo(pShareInfo);
		}
	}

	SIZE CListViewUI::GetContentSize()
	{
		SIZE szContent = {0};
		MUILISTVIEWROW * pLvRow = m_pLvRow;
		CListHeaderUI * pHeader = nullptr;
		int nRowCount = m_nRowCount;
		int nHeaderCount = m_Headers.GetSize();

		//直接拿最后一个CListHeaderUI位置
		for(int i=0; nHeaderCount > i; i++)
		{
			pHeader =  (CListHeaderUI*)m_Headers[i];
			
			if(false == pHeader->IsVisible())
			{
				continue;
			}
			szContent.cx += pHeader->GetWidth();
		}

		while(nRowCount--)
		{
			szContent.cy += pLvRow->Info.nHeight;
			pLvRow = (MUILISTVIEWROW*)pLvRow->Link.pNext;
		}

		//body底部靠近滚动条不好看
		
		return szContent;
	}

	bool CListViewUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		//对于header控件来说，没有外边距这种东西
		RECT rcHeaderItem;
		POINT ptNextPos={0};//下一个控件的位置
		RECT rcContainer = rcItem;//用来欺骗CContainerUI
		CControlUI * pControl = nullptr;
		RECT rcScrollItem;
		int nHeaderCount = m_Headers.GetSize();

		if(false == bMustUpdate && TRUE == IsSameRect(m_rcItem, rcItem)) return false;

		rcContainer.top += m_nHeaderHeight;
		if(TRUE == IsValidRect(rcContainer))
		{
			//用来欺骗CContainerUI,让它为我们调整滚动条
			CContainerUI::SetItem(rcContainer, bMustUpdate);
		}
		//欺骗完成后，再设置真正的rcItem
		m_rcRawItem = m_rcItem = rcItem;
		OffsetRect(&m_rcRawItem, -m_rcRawItem.left, -m_rcRawItem.top);
		OffsetRect(&m_rcClient, 0, m_nHeaderHeight);
		OffsetRect(&m_rcContent, 0, m_nHeaderHeight);

		if(m_pVerticalScrollBar)
		{
			rcScrollItem = m_pVerticalScrollBar->GetItem();
			::OffsetRect(&rcScrollItem, 0, m_nHeaderHeight);
			m_pVerticalScrollBar->SetItem(rcScrollItem, false);
		}

		if(m_pHorizontalScrollBar)
		{
			rcScrollItem = m_pHorizontalScrollBar->GetItem();
			::OffsetRect(&rcScrollItem, 0, m_nHeaderHeight);
			m_pHorizontalScrollBar->SetItem(rcScrollItem, false);
		}

		if(nHeaderCount == 0 ) return true;//布局中没有其他控件就直接返回
		
		ptNextPos.x = m_rcContent.left;
		ptNextPos.y = 0;
		for(int i=0; nHeaderCount > i; i++)
		{
			pControl =  (CControlUI*)m_Headers[i];

			//置空
			EmptyRect(rcHeaderItem);
			
			if(false == pControl->IsVisible())
			{
				//不显示打印区域就是空了
				goto loop;
			}

			rcHeaderItem.left = ptNextPos.x;
			rcHeaderItem.top = ptNextPos.y;
			
			rcHeaderItem.right = rcHeaderItem.left + pControl->GetWidth();
			rcHeaderItem.bottom = rcHeaderItem.top + m_nHeaderHeight;

			ptNextPos.x += rcHeaderItem.right - rcHeaderItem.left;
loop:
			pControl->SetItem(rcHeaderItem, false);
		}
		return true;
	}

	void CListViewUI::PaintStatusImage(const RECT &rcUpdate)
	{
		SIZE szRound = {0};//listview子控件中，不存在外缩进和半圆效果
		POINT ptBegin, ptEnd;
		RECT rcHeaderUpdate, rcHeaderItem;
		CControlUI * pHeader;
		int xOffset = 0;
		int nHeaderCount = m_Headers.GetSize();

		for(int i=0; nHeaderCount > i; i++)
		{
			pHeader = (CControlUI*)m_Headers[i];
			rcHeaderItem = pHeader->GetItem();

			if(false == pHeader->IsVisible() || m_rcRawItem.left > rcHeaderItem.right)
			{
				//这一列没显示区域，不用绘制了
				continue;
			}

			if(rcHeaderItem.left > m_rcRawItem.right)//超出绘制区域的列
			{
				xOffset = rcHeaderItem.right;//保存一下记录
				break;
			}

			//进行交叉运算，计算出子控件的有效绘图区域
			if(TRUE == IntersectRect(&rcHeaderUpdate, &rcHeaderItem, &rcUpdate))
			{
				xOffset = rcHeaderItem.right;//保存一下记录
				//拥有相交区域才能绘制
				OffsetRect(&rcHeaderUpdate, -rcHeaderItem.left, -rcHeaderItem.top);

				m_pShareInfo->pRenderEngine->OffsetDrawPoint(rcHeaderItem.left, rcHeaderItem.top);
				pHeader->OnPaint(rcHeaderUpdate);
				m_pShareInfo->pRenderEngine->OffsetDrawPoint(-rcHeaderItem.left, -rcHeaderItem.top);

				if(GetAValue(m_refBetweenLinesColor))//绘制分隔线，要放在pControl->OnPaint后面，免得覆盖
				{
					ptBegin.x = ptEnd.x = rcHeaderItem.left;
					ptBegin.y = rcHeaderItem.top;
					ptEnd.y = rcHeaderItem.bottom;
					m_pShareInfo->pRenderEngine->OnDrawLine(ptBegin, ptEnd, 1, m_refBetweenLinesColor);
				}
				
			}
		}

		//最后将空闲区域填补上
		if(rcUpdate.right > xOffset)
		{
			rcHeaderItem.top = m_rcRawItem.top;
			rcHeaderItem.left = xOffset;
			rcHeaderItem.right = m_rcRawItem.right;
			rcHeaderItem.bottom = rcHeaderItem.top + m_nHeaderHeight;

			if(IsValidRect(rcHeaderItem))
			{
				if(GetAValue(m_refHeaderBkColor))
				{
					m_pShareInfo->pRenderEngine->OnDrawColor(rcHeaderItem, m_refHeaderBkColor);
				}

				if(!m_strHeaderBkImage.IsEmpty())
				{
					m_pShareInfo->pRenderEngine->OnDrawImage(rcHeaderItem, m_strHeaderBkImage);
				}
			}

			if(GetAValue(m_refBetweenLinesColor))//绘制分隔线，要放在pControl->OnPaint后面，免得覆盖
			{
				ptBegin.x = ptEnd.x = xOffset;
				ptEnd.y = ptBegin.y + m_nHeaderHeight;
				m_pShareInfo->pRenderEngine->OnDrawLine(ptBegin, ptEnd, 1, m_refBetweenLinesColor);

				ptBegin.x = ptEnd.x = m_rcRawItem.right - 1;
				m_pShareInfo->pRenderEngine->OnDrawLine(ptBegin, ptEnd, 1, m_refBetweenLinesColor);
			}
		}
	}

	void CListViewUI::PaintContent(const RECT & rcUpdate)
	{
		SIZE szRound = {0};//listview子控件中，不存在外缩进和半圆效果
		POINT ptBegin, ptEnd;
		RECT rcHeaderItem;
		RECT rcDraw;
		SIZE siOffset;
		ARGBREF refItemBkColor = NULL;
		CListHeaderUI * pHeader;
		MUILISTVIEWROW * pLvRow = m_pLvRow;
		MUIHEADERROW * pHdRow = NULL;
		RECT rcSelectRegion;//选择的区域
		CRenderEngine * pEnglne = m_pShareInfo->pRenderEngine;

		//我们可以变换一下思维，将选中区域，当成一个RECT, 
		//当前正在绘制的列和行，当成一个POINT
		if(m_ptSelectEndItem.x > m_ptSelectBeginItem.x)
		{
			rcSelectRegion.left = m_ptSelectBeginItem.x;
			rcSelectRegion.right = m_ptSelectEndItem.x;
		}
		else
		{
			rcSelectRegion.left = m_ptSelectEndItem.x;
			rcSelectRegion.right = m_ptSelectBeginItem.x;
		}

		if(m_ptSelectEndItem.y > m_ptSelectBeginItem.y)
		{
			rcSelectRegion.top = m_ptSelectBeginItem.y;
			rcSelectRegion.bottom = m_ptSelectEndItem.y;
		}
		else
		{
			rcSelectRegion.top = m_ptSelectEndItem.y;
			rcSelectRegion.bottom = m_ptSelectBeginItem.y;
		}
		
		int nHeaderCount = m_Headers.GetSize();

		int nBeginCol = 0, nBeginRow = 0;//从哪一行开始绘制
		int xOffset = m_rcContent.left;
		int yOffset = m_rcContent.top;
		int nClientWidht = m_rcClient.right - m_rcClient.left;
		int nClientHeight = m_rcClient.bottom - m_rcClient.top;
		//::OffsetRect(&rcThisClient,  - m_szScrollOffset.cx, - m_szScrollOffset.cy);

		//找出从那一行开始绘制，在大数据时效果明显

		if(0 == m_nRowCount) return;

		for(int i = 0; m_nRowCount > i; i++)
		{
			yOffset += pLvRow->Info.nHeight;
			if(yOffset > m_rcClient.left)
			{
				yOffset -= pLvRow->Info.nHeight;//还原
				nBeginRow = i;
				break;
			}

			pLvRow = (MUILISTVIEWROW*)pLvRow->Link.pNext;
		}

		for(int col = 0; nHeaderCount > col; col++)
		{
			pHeader = (CListHeaderUI*)m_Headers[col];
			rcHeaderItem = pHeader->GetItem();

			if(false == pHeader->IsVisible() || m_rcClient.left > rcHeaderItem.right)
			{
				//这一列没显示区域，不用绘制了
				continue;
			}

			if(rcHeaderItem.left > m_rcClient.right)//超出绘制区域的列
			{
				break;
			}

			pHdRow = (MUIHEADERROW *)find_index_for_linked(nBeginRow, &pHeader->m_pHdRow->Link, pHeader->m_nRowCount);
			pLvRow = (MUILISTVIEWROW *)find_index_for_linked(nBeginRow, &this->m_pLvRow->Link, this->m_nRowCount);
			rcDraw.left = rcHeaderItem.left;
			rcDraw.right = rcHeaderItem.right;
			rcDraw.top = yOffset;
			rcDraw.bottom = rcDraw.top + pLvRow->Info.nHeight;

			for(int row = nBeginRow; m_nRowCount > row; row++)
			{
				if(rcDraw.top > m_rcClient.bottom)//超出绘制区域的行
				{
					break;
				}

				refItemBkColor = pHdRow->Info.refBkColor;

				if(TRUE == PointInRect2(col, row, rcSelectRegion))
				{
					if(LVDF_ITEM == m_dwFocus)
					{
						if(m_refSelectColor) refItemBkColor = m_refSelectColor;
					}
					else
					{
						if(m_refNormalColor) refItemBkColor = m_refNormalColor;
					}
				}

				if(GetAValue(refItemBkColor))
				{
					pEnglne->OnDrawColor(rcDraw, refItemBkColor);
				}
				
				if(GetAValue(m_refGridLinesColor))
				{
					pEnglne->OnDrawFrame(rcDraw, m_refGridLinesColor);
				}

				pEnglne->OnDrawText(rcDraw, pHdRow->Info.strText, 0xff000000, m_hFont, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				
				rcDraw.top += pLvRow->Info.nHeight;
				pLvRow = (MUILISTVIEWROW *)pLvRow->Link.pNext;
				pHdRow = (MUIHEADERROW *)pHdRow->Link.pNext;
				rcDraw.bottom += pLvRow->Info.nHeight;

			}
		}

		return ;
	}

	bool CListViewUI::AddRow(MUILVROWINFO &Info,int nRow /* -1 最后一行*/)
	{
		MUILISTVIEWROW * pNewRow = NULL;
		MUILISTVIEWROW * pTmpRow = NULL;
		CListHeaderUI * pHeader = NULL;
		if(NULL == m_pLvRow)
		{
			if(0 == nRow || -1 == nRow)
			{
				pNewRow = new MUILISTVIEWROW;
				pNewRow->Link.pPrev = pNewRow->Link.pNext = (MUILINKED*)pNewRow;
				pNewRow->Info = Info;

				AddHeaderRow(nRow);
				m_pLvRow = pNewRow;
				m_nRowCount ++;
				return true;
			}
		}
		else
		{
			pTmpRow = (MUILISTVIEWROW*) find_index_for_linked(nRow - 1, (MUILINKED*)m_pLvRow, m_nRowCount);

			if(pTmpRow)
			{
				pNewRow = new MUILISTVIEWROW;
				pNewRow->Link.pPrev = pNewRow->Link.pNext = (MUILINKED*)pNewRow;
				pNewRow->Info = Info;

				pNewRow->Link.pPrev = (MUILINKED*)pTmpRow;
				pNewRow->Link.pNext = pTmpRow->Link.pNext;

				pTmpRow->Link.pNext->pPrev = (MUILINKED*)pNewRow;
				pTmpRow->Link.pNext = (MUILINKED*)pNewRow;

				AddHeaderRow(nRow);
				m_nRowCount ++;
				if(0 == nRow) m_pLvRow = pNewRow;
				return true;
			}
			
		}

		return false;
	}

	bool CListViewUI::GetRow(MUILVROWINFO &info, int nRow)
	{
		MUILISTVIEWROW * pRow;
		pRow = (MUILISTVIEWROW*) find_index_for_linked(nRow ,&m_pLvRow->Link, m_nRowCount);

		if(pRow)
		{
			info = pRow->Info;
			return true;
		}

		return false;

	}

	bool CListViewUI::SetRow(MUILVROWINFO &info, int nRow)
	{
		MUILISTVIEWROW * pRow;
		pRow = (MUILISTVIEWROW*) find_index_for_linked(nRow ,&m_pLvRow->Link, m_nRowCount);

		if(pRow)
		{
			pRow->Info = info;
			return true;
		}

		return false;
	}

	bool CListViewUI::DelRow(int nRow)
	{
		MUILISTVIEWROW * pRow;
		MUILISTVIEWROW * pPreRow, * pNextRow;
		pRow = (MUILISTVIEWROW*) find_index_for_linked(nRow, &m_pLvRow->Link, m_nRowCount);

		if(pRow)
		{
			pPreRow = (MUILISTVIEWROW *)pRow->Link.pPrev;
			pNextRow = (MUILISTVIEWROW *)pRow->Link.pNext;

			pPreRow->Link.pNext = pRow->Link.pNext;
			pNextRow->Link.pPrev = pRow->Link.pPrev;

			if(pRow == m_pLvRow)
			{
				if(1 == m_nRowCount)
				{
					//唯一一行删除了
					m_pLvRow = NULL;
				}
				else
				{
					m_pLvRow = pNextRow;
				}
			}
			DelHeaderRow(nRow);
			m_nRowCount--;
			delete pRow;
			return true;
		}

		return false;
	}
	//或许你发现怎么没有AddCol[添加列]等函数，其实IControlArray接口就是用来添加列的

	bool CListViewUI::AddHeaderRow(int nRow)
	{
		CListHeaderUI * pHeader = NULL;
		MUIHDROWINFO info ={0};

		int nHeaderCount = m_Headers.GetSize();

		for(int i=0; nHeaderCount > i; i++)
		{
			pHeader = (CListHeaderUI*)m_Headers[i];
			//进过封装后，pHeader->AddRow是不允许出错的
			pHeader->AddRow(info, nRow);
		}
		return true;
	}

	bool CListViewUI::DelHeaderRow(int nRow)
	{
		CListHeaderUI * pHeader = NULL;
		int nHeaderCount = m_Headers.GetSize();

		for(int i=0; nHeaderCount > i; i++)
		{
			pHeader = (CListHeaderUI*)m_Headers[i];
			//进过封装后，pHeader->DelRow是不允许出错的
			pHeader->DelRow(nRow);
		}
		return true;
	}

	//首先AddRow新增一行，然后再SetSubItem
	bool CListViewUI::SetSubItem(MUIHDROWINFO &info, int nCol, int nRow)
	{
		CListHeaderUI * pHeader = (CListHeaderUI *)m_Headers[nCol];
		if(NULL == pHeader) return false;

		return pHeader->SetRow(info, nRow);
	}

	bool CListViewUI::GetSubItem(MUIHDROWINFO &info, int nCol, int nRow)
	{
		CListHeaderUI * pHeader = (CListHeaderUI *)m_Headers[nCol];
		if(NULL == pHeader) return false;
		return pHeader->GetRow(info, nRow);
	}


	void CListViewUI::SetHeaderBkColor(ARGBREF refColor)
	{
		m_refHeaderBkColor = refColor;
	}

	ARGBREF CListViewUI::GetHeaderBkColor() const
	{
		return m_refHeaderBkColor;
	}

	void CListViewUI::SetHeaderBkImage(LPCTSTR strImage)
	{
		m_strHeaderBkImage = strImage;
	}

	LPCTSTR CListViewUI::GetHeaderBkImage() const
	{
		return m_strHeaderBkImage;
	}

	void CListViewUI::SetHeaderHeight(int nHeight)
	{
		m_nHeaderHeight = nHeight;
	}

	int CListViewUI::GetHeaderHeight() const
	{
		return m_nHeaderHeight;
	}

	void CListViewUI::SetInfoBarWidth(int nWidth)
	{
		m_nInfoBarWidth = nWidth;
	}

	int CListViewUI::GetInfoBarWidth() const
	{
		return m_nInfoBarWidth;
	}

	CListHeaderUI * CListViewUI::FindHeaderByPoint(POINT &pt)
	{
		POINT ptThis = pt;
		RECT rcHeaderItem;
		CListHeaderUI * pHeader; 
		int nHeaderCount = m_Headers.GetSize();

		if(pt.y > m_nHeaderHeight) return NULL;
		pt.x +=  m_szScrollOffset.cx;
		for(int i=0; nHeaderCount > i; i++)
		{
			pHeader = (CListHeaderUI *)m_Headers[i];
			if(false == pHeader->IsVisible())
			{
				continue;
			}
			
			rcHeaderItem = pHeader->GetItem();
			
			if(TRUE == PointInRect(pt, rcHeaderItem))
			{
				pt.x -= rcHeaderItem.left;
				pt.y -= rcHeaderItem.top;
				return pHeader;
			}
		}
		pt = ptThis;
		return NULL;
	}

	CListHeaderUI * CListViewUI::FindHeaderByName(LPCTSTR strName)
	{
		if(NULL == strName || _T('\0') == strName[0])
		{
			return NULL;
		}

		int nHeaderCount = m_Headers.GetSize() ;
		CListHeaderUI * pHeader = NULL;

		for(int i =0 ; nHeaderCount > i; i++)
		{
			pHeader = (CListHeaderUI *)m_Headers[i];
			if(pHeader->FindControlByName(strName))
			{
				return pHeader;
			}
		}

		return NULL;
	}

	bool CListViewUI::FindItemByPoint(POINT pt, int &nCol, int &nRow)
	{
		int nColWidht = - m_szScrollOffset.cx;
		int nRowHeight = m_nHeaderHeight - m_szScrollOffset.cy;

		MUILISTVIEWROW * pLvRow = m_pLvRow;
		CListHeaderUI * pHeader; 
		int nHeaderCount = m_Headers.GetSize();

		if(nColWidht > pt.x || nRowHeight > pt.y || 
			FALSE == PointInRect(pt, m_rcClient))
		{
			nCol = nRow = -1;
			return false;//没有命中
		}

		for(int i =0; nHeaderCount > i; i++)
		{
			pHeader = (CListHeaderUI *)m_Headers[i];

			if(false == pHeader->IsVisible()) continue;

			nColWidht += pHeader->GetWidth();

			if(nColWidht > pt.x) 
			{
				nCol = i;
				break;//跳出，执行下一步
			}
		}

		if(-1 == nCol)
		{
			nCol = nRow = -1;
			return false;
		}

		for(int i=0; m_nRowCount > i ; i++)
		{
			nRowHeight += pLvRow->Info.nHeight;

			if(nRowHeight > pt.y)
			{
				nRow = i;
				break;//跳出，执行下一步
			}
			pLvRow = (MUILISTVIEWROW *) pLvRow->Link.pNext;
		}

		if(-1 == nRow)
		{
			nCol = nRow = -1;
			return false;
		}

		return true;
	}

	bool CListViewUI::SetSelect(POINT *ptBegin, POINT * ptEnd)
	{
		bool bRet = false;
		POINT ptTmp;
		if(NULL == ptBegin && NULL == ptEnd)
		{
			m_ptSelectBeginItem.x = m_ptSelectBeginItem.y = -1;
			m_ptSelectEndItem.x = m_ptSelectEndItem.y = -1;
			bRet = true;
		}
		else if(ptBegin && ptEnd)
		{
			if(m_nRowCount > ptBegin->y && m_Headers.GetSize() > ptBegin->x &&
				m_nRowCount > ptEnd->y && m_Headers.GetSize() > ptEnd->x)
			{
				m_ptSelectBeginItem.x = ptBegin->x;
				m_ptSelectBeginItem.y = ptBegin->y;

				m_ptSelectEndItem.x = ptEnd->x;
				m_ptSelectEndItem.y = ptEnd->y;
				bRet = true;
			}
		}
		else if(ptBegin && NULL == ptEnd)
		{
			if(m_nRowCount > ptBegin->y && m_Headers.GetSize() > ptBegin->x)
			{
				m_ptSelectBeginItem.x = ptBegin->x;
				m_ptSelectBeginItem.y = ptBegin->y;

				m_ptSelectEndItem.x = ptBegin->x;
				m_ptSelectEndItem.y = ptBegin->y;
				bRet = true;
			}
		}
		else
		{
			MUIASSERT(0 && "CListViewUI::SetSelect 不支持这种赋值操作");
		}

		if(true == bRet)
		{
			this->Invalidate();
		}
		return false;
	}

	LRESULT CListViewUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		CListHeaderUI * pHeader;
		MUILISTVIEWROW * pLvRow = NULL;
		MUIHEADERROW * pHdRow = NULL;
		POINT point;
		int nCol, nRow;
		switch(message)
		{
		case WM_MOUSELEAVE:
			{
				if(m_pHotHeader)
				{
					m_pHotHeader->CallWndProc(message, 0, 0);
					m_pHotHeader = NULL;
				}
				if(m_pPushedHeader && m_pPushedHeader != m_pHotHeader)
				{
					m_pPushedHeader->CallWndProc(message, 0, 0);
				}
				this->Invalidate();
			}break;
		case WM_MOUSEMOVE:
			{
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);
				pHeader = FindHeaderByPoint(point);

				if(pHeader != m_pHotHeader)
				{
					//当鼠标移动时，有可能离开一个控件，进入另一个控件，在此进行处理
			
					//切换热点
					//逻辑：先离开一个房间，才能进入另外一个房间
					//先发出离开通知，
					if(NULL != m_pHotHeader)
					{
						m_pHotHeader->CallWndProc(WM_MOUSELEAVE, 0, 0);
					}
					m_pHotHeader = pHeader;
					//再发出进入通知
					if(NULL != m_pHotHeader)
					{
						m_pHotHeader->CallWndProc(WMU_MOUSEENTER, wParam, MAKELONG(point.x, point.y));
					}
					Invalidate();
				}

				if(pHeader)
				{
					pHeader->CallWndProc(message, wParam, MAKELONG(point.x, point.y));
				}
				else
				{
					m_pHotHeader = NULL;
				}

				if(m_pPushedHeader && m_pPushedHeader != pHeader)
				{
					m_pPushedHeader->CallWndProc(message, 0, lParam);
				}

				if(pHeader) break;//鼠标在表头，就不用检查是否在表体了

				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);
				if(m_dwState & STATE_PUSHED && LVDF_ITEM == m_dwFocus && 
					true == FindItemByPoint(point, nCol, nRow))
				{
					point.x = nCol;
					point.y = nRow;
					SetSelect(&m_ptSelectBeginItem, &point);
				}

			}break;
		case WM_LBUTTONDOWN:
			{
				m_dwFocus = LVDF_NON;
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);

				if(pHeader = FindHeaderByPoint(point))
				{
					m_dwFocus = LVDF_HEADER;
					if(NULL == m_pPushedHeader)
					{
						m_pPushedHeader = pHeader;
					}
					else
					{
						//基本不会出现这种情况
						m_pPushedHeader->CallWndProc(message, wParam, lParam);
					}
					pHeader->CallWndProc(message, wParam, MAKELONG(point.x, point.y));
				}
				else
				{
					point.x = GET_X_LPARAM(lParam);
					point.y = GET_Y_LPARAM(lParam);
					if(true == FindItemByPoint(point, nCol, nRow))
					{
						m_dwFocus = LVDF_ITEM;
						point.x = nCol;
						point.y = nRow;
						SetSelect(&point, NULL);
					}
				}
			}break;
		case WM_LBUTTONUP:
			{
				if(pHeader = FindHeaderByPoint(point))
				{
					pHeader->CallWndProc(message, wParam, MAKELONG(point.x, point.y));
				}
				if(m_pPushedHeader && m_pPushedHeader != pHeader)
				{
					m_pPushedHeader->CallWndProc(message, 0, lParam);
					m_pPushedHeader = NULL;
				}
			}break;
		case WMU_LBUTTONCLICK:
			{
				
			}break;
		case WM_LBUTTONDBLCLK:
			{
				if(pHeader = FindHeaderByPoint(point))
				{
					pHeader->CallWndProc(message, wParam, MAKELONG(point.x, point.y));
				}
			}break;
		case WM_KILLFOCUS:
			{
				m_dwFocus = LVDF_NON;
			}break;
		}
		return __super::WndProc(message, wParam, lParam);
	}

}