
#include "UITileLayout.h"

namespace MYUI
{
	CTileLayoutUI::CTileLayoutUI()
		: m_dwAdaptType(TileAdaptType::AutoAdapt)
		, m_nAdaptCount(0)
	{
		int nWidht = GetSystemMetrics(SM_CXVSCROLL) * GetSystemMetrics(SM_CXVSCROLL) / 2;
		m_siAdaptItem.cx =  nWidht;
		m_siAdaptItem.cy = m_siAdaptItem.cx * 4 / 3;

		EmptyRect(m_rcAdaptPadding);
	}

	CTileLayoutUI::~CTileLayoutUI()
	{
	}

	CMuiString CTileLayoutUI::g_strClassName(_T("TileLayoutUI"));

	CMuiString CTileLayoutUI::GetClassName() const
	{
		return CTileLayoutUI::g_strClassName;
	}

	void CTileLayoutUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("AdaptType"))) 
		{
			TileAdaptType type = AutoAdapt;
			if(0 == _tcsicmp(strValue, _T("WidthAdapt")))
			{
				type = WidthAdapt;
			}
			else if(0 == _tcsicmp(strValue, _T("HeightAdap")))
			{
				type = HeightAdap;
			}
			else if(0 == _tcsicmp(strValue, _T("ColumnAdapt")))
			{
				type = ColumnAdapt;
			}
			else if(0 == _tcsicmp(strValue, _T("RowAdapt")))
			{
				type = RowAdapt;
			}
			else
			{
				type = AutoAdapt;
			}
			//SetAdaptType(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("AdaptColumn"))) 
		{
			SetAdaptColumn(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("AdaptPadding"))) 
		{
			SetAdaptPadding(CMuiRect(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("AdaptItemSize"))) 
		{
			SetAdaptItemSize(CMuiSize(strValue));
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
	}


	SIZE CTileLayoutUI::GetContentSize()
	{
		SIZE szContent = {0};
		int nActiveCount = 0, nColumn = 0, nRow = 0;
		CControlUI * pControl = nullptr;
		int nControlCount = m_Items.GetSize();

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];
			if(false == pControl->IsVisible())
			{
				continue;
			}
			++nActiveCount;
		}

		if(0 < nActiveCount)
		{
			nColumn = GetAdaptColumn();
			nRow = nActiveCount / nColumn;
			if(nActiveCount % nColumn) nRow += 1;
		}
		
		//子控件需要的尺寸
		szContent.cx = m_siAdaptItem.cx * nColumn;
		szContent.cy = m_siAdaptItem.cy * nRow;

		//外边距需要的尺寸
		szContent.cx += m_rcMargin.left + m_rcMargin.right;
		szContent.cy += m_rcMargin.top + m_rcMargin.bottom;

		//内边距㤇的尺寸
		szContent.cx += m_rcInset.left + m_rcInset.right;
		szContent.cy += m_rcInset.top + m_rcInset.bottom;
		return szContent;
	}

	bool CTileLayoutUI::SetAdaptType(TileAdaptType type)
	{
		if(TileAdaptType::WidthAdapt != type 
			&& TileAdaptType::HeightAdap != type 
			&& TileAdaptType::ColumnAdapt != type 
			&& TileAdaptType::RowAdapt != type
			&& TileAdaptType::AutoAdapt != type)
		{
			return false;
		}
		//设置网格控件内部适配器的模式
		//不同的m_dwAdaptType会影响到GetAdaptColumn的计算模式
		m_dwAdaptType = type;
		return true;
	}

	bool CTileLayoutUI::SetAdaptColumn(int nCount)
	{
		m_nAdaptCount = nCount;
		return true;
	}

	int CTileLayoutUI::GetAdaptColumn() const
	{
		int nColumn = 0;
		int nActiveCount = 0;
		int nItemCount = m_Items.GetSize();
		
		for(int i= 0; nItemCount > i; i++)
		{
			CControlUI * pContorl = (CControlUI*)m_Items[i];
			if(pContorl->IsVisible())
			{
				++nActiveCount;
			}
		}

		switch(m_dwAdaptType)
		{
		case TileAdaptType::WidthAdapt:
			{
				int nClientWidth = m_rcClient.right - m_rcClient.left;
				nColumn = nClientWidth / m_siAdaptItem.cx;
			}break;
		case TileAdaptType::HeightAdap:
			{
				if(0 == nActiveCount) break;
				int nClientHeight = m_rcClient.bottom - m_rcClient.top;
				int nRow = nClientHeight / m_siAdaptItem.cy;
				nRow = MAX(nRow, 1);

				nColumn = nActiveCount / nRow;
				if(nActiveCount % nRow)
				{
					//不能整除，绘制的时候将会多出一行，
					//多显示一列，就可以了
					nColumn += 1;
				}
			}break;
		case TileAdaptType::ColumnAdapt:
			{
				nColumn = m_nAdaptCount;
			}break;
		case TileAdaptType::RowAdapt:
			{
				if(0 == nActiveCount) break;
				nColumn = nActiveCount / m_nAdaptCount;
				if(nActiveCount % m_nAdaptCount)
				{
					//不能整除，绘制的时候将会多出一行，
					//多显示一列，就可以了
					nColumn += 1;
				}
			}break;
		default:
			break;
		}

		nColumn = MAX(nColumn , 1);//至少显示一列
		return nColumn;
	}

	bool CTileLayoutUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		RECT rcChildItem;
		RECT rcChildMargin;
		int nColumn = 0, nRow = 0;//计算出需要显示多少列和行
		POINT ptNextPos={0};//下一个控件的位置

		CControlUI * pControl = nullptr;
		int nControlCount = m_Items.GetSize();
		if(false == __super::SetItem(rcItem, bMustUpdate)) return false;
		if(nControlCount == 0 ) return true;//布局中没有其他控件就直接返回

		//根据布局风格计算列
		nColumn = GetAdaptColumn();
		nRow = 0;
		for(int i=0; nControlCount > i; i++)
		{
			if(0 == i % nColumn)
			{
				ptNextPos.x = m_rcInset.left;
				ptNextPos.y = m_rcInset.top + m_siAdaptItem.cy * nRow;
				++nRow;
			}
			pControl =  (CControlUI*)m_Items[i];
			//置空
			EmptyRect(rcChildItem);
			
			if(false == pControl->IsVisible())
			{
				//不显示打印区域就是空了
				goto loop;
			}

			rcChildItem.left = ptNextPos.x;
			rcChildItem.top = ptNextPos.y;
			rcChildItem.right = rcChildItem.left + m_siAdaptItem.cx;
			rcChildItem.bottom = rcChildItem.top + m_siAdaptItem.cy;

			ptNextPos.x += m_siAdaptItem.cx;
			//添加adapt item内间距，
			IndentRect(&rcChildItem, &m_rcAdaptPadding);
			//缩进外边距
			rcChildMargin = pControl->GetMargin();
			IndentRect(&rcChildItem, &rcChildMargin);
loop:
			pControl->SetItem(rcChildItem, false);
		}
		return true;
	}

	TileAdaptType CTileLayoutUI::GetAdaptType()
	{
		return m_dwAdaptType;
	}

	void CTileLayoutUI::SetAdaptPadding(RECT &rect)
	{
		m_rcAdaptPadding = rect;
	}

	const RECT &CTileLayoutUI::GetAdaptPadding() const
	{
		return m_rcAdaptPadding;
	}

	void CTileLayoutUI::SetAdaptItemSize(SIZE &size)
	{
		m_siAdaptItem = size;
	}

	const SIZE &CTileLayoutUI::GetAdaptItemSize() const
	{
		return m_siAdaptItem;
	}

}