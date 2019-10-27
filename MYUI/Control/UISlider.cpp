
#include "UISlider.h"

namespace MYUI
{
	CSliderUI::CSliderUI()
		: m_refStepBarColor(NULL)
		, m_bSelectStepBar(false)
	{
		m_szStepBar.cx = m_szStepBar.cy = 0;
		m_ptOldMousePos.x = m_ptOldMousePos.y = 0;

	}

	CSliderUI::~CSliderUI()
	{
	}

	CMuiString CSliderUI::g_strClassName(_T("SliderUI"));

	CMuiString CSliderUI::GetClassName() const
	{
		return CSliderUI::g_strClassName;
	}

	void CSliderUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("StepBarSize"))) 
		{
			SetStepBarSize(CMuiSize(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("StepBarColor"))) 
		{
			SetStepBarColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("StepBarImage"))) 
		{
			SetStepBarImage(strValue);
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
		
	}

	void CSliderUI::SetStepBarSize(SIZE &size)
	{
		m_szStepBar = size;
	}

	const SIZE &CSliderUI::GetStepBarSize() const
	{
		return m_szStepBar;
	}

	void CSliderUI::SetStepBarColor(ARGBREF argb)
	{
		m_refStepBarColor = argb;
	}

	ARGBREF CSliderUI::GetStepBarColor()
	{
		return m_refStepBarColor;
	}

	void CSliderUI::SetStepBarImage(LPCTSTR strImage)
	{
		m_strStepImage = strImage;
	}

	LPCTSTR CSliderUI::GetStepBarImage() const
	{
		return m_strStepImage;
	}

	void CSliderUI::PaintStatusImage(const RECT &rcItem,const RECT &rcPaint)
	{
		RECT rcDraw = {0};
		rcDraw.right = m_szStepBar.cx;
		rcDraw.bottom = m_szStepBar.cy;
		int nValue = m_nValue;
		int nShift = 0;

		__super::PaintStatusImage(rcItem, rcPaint);

		if(MPGS_VERTICAL & m_dwStyle)//垂直
		{
			nShift = m_nValue * ((rcItem.bottom - rcItem.top) - m_szStepBar.cy)
				/ m_nMaxValue ;
			OffsetRect(&rcDraw, rcItem.left, rcItem.bottom - m_szStepBar.cy);
			OffsetRect(&rcDraw, 0, -nShift);
		}
		else//水平
		{
			nShift = m_nValue * ((rcItem.right - rcItem.left) - m_szStepBar.cx)
				/ m_nMaxValue;
			OffsetRect(&rcDraw, rcItem.left, rcItem.top);
			OffsetRect(&rcDraw, nShift, 0);
		}

		if(NULL != m_refStepBarColor)
		{
			m_pShareInfo->pRenderEngine->OnDrawColor(rcDraw, m_refStepBarColor);
		}

		if(!m_strStepImage.IsEmpty())
		{
			m_pShareInfo->pRenderEngine->OnDrawImage(rcDraw, m_strStepImage);
		}
		return;
	}

	LRESULT CSliderUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT point;
		DWORD dwPos;
		int nShift = 0;
		RECT rcStepBar = {0};
		int nMaxOffset;
		RECT rcItem = m_rcRawItem;
		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);
				m_ptOldMousePos = point;
				if(FALSE  == PointInRect(point, m_rcRawItem)) break;

				//到这里，说明鼠标在控件内点击

				rcStepBar.right = m_szStepBar.cx;
				rcStepBar.bottom = m_szStepBar.cy;
				nShift = m_nValue * ((m_rcRawItem.right - m_rcRawItem.left) - m_szStepBar.cx)
					/ (m_nMaxValue - m_nMinValue) ;
				OffsetRect(&rcStepBar, nShift, 0);

				if(FALSE == PointInRect(point, rcStepBar))
				{
					//没击中StepBar，那么先将StepBar移动到鼠标的位置
					
					if(MPGS_VERTICAL & m_dwStyle)//垂直
					{
						int nset = m_szStepBar.cy / 2;
						point.y = MIN(rcItem.bottom , MAX(rcItem.top , point.y - nset));//命中靶心
						nShift = point.y * m_nMaxValue / ((rcItem.bottom - rcItem.top) - m_szStepBar.cy);

					}
					else//水平
					{
						int nset = m_szStepBar.cx / 2;
						point.x = MIN(rcItem.right, MAX(rcItem.left, point.x - nset));//命中靶心
						nShift = point.x * m_nMaxValue / ((rcItem.right - rcItem.left) - m_szStepBar.cx);

					}

					this->SetValue(nShift);
				}

				m_bSelectStepBar = true;
				this->SetCapture();
				
			}break;
		case WM_LBUTTONUP:
			{
				if(STATE_PUSHED & m_dwState)
				{
					this->ReleaseCapture();
				}
				m_bSelectStepBar = false;
				m_ptOldMousePos.x = m_ptOldMousePos.y = 0;
			}break;
		case WM_MOUSEMOVE:
			{
				if(true == m_bSelectStepBar)
				{
					point.x = GET_X_LPARAM(lParam);
					point.y = GET_Y_LPARAM(lParam);

					if(MPGS_VERTICAL & m_dwStyle)//垂直
					{
						int nset = m_szStepBar.cy / 2;
						point.y = MIN(rcItem.bottom , MAX(rcItem.top , point.y - nset));//命中靶心
						nShift = point.y * m_nMaxValue / ((rcItem.bottom - rcItem.top) - m_szStepBar.cy);

					}
					else//水平
					{
						int nset = m_szStepBar.cx / 2;
						point.x = MIN(rcItem.right, MAX(rcItem.left, point.x - nset));//命中靶心
						nShift = point.x * m_nMaxValue / ((rcItem.right - rcItem.left) - m_szStepBar.cx);

					}
					this->SetValue(nShift);
				}
			}break;
		case WM_MOUSEWHEEL:
			{
				if(true == IsEnabled())
				{
					nShift = (short)HIWORD(wParam);//返回一个滚动的大小,一般是120的倍数,正数代表滚动向前,负数代表向后
					SetValue(m_nValue + (nShift / 10) / 2 * -1);
				}
				return 0;
			}break;
		}
		return __super::WndProc(hWnd, message, wParam, lParam);
	}
}