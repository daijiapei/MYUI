
#include "UIProgress.h"

namespace MYUI
{
	CProgressUI::CProgressUI()
		: m_nMinValue(0)
		, m_nMaxValue(100)
		, m_nValue(0)
	{
	}

	CProgressUI::~CProgressUI()
	{
	}

	CMuiString CProgressUI::g_strClassName(_T("ProgressUI"));

	CMuiString CProgressUI::GetClassName() const
	{
		return CProgressUI::g_strClassName;
	}

	void CProgressUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("horizontal"))) 
		{
			SetHorizontal(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("maxValue"))) 
		{
			SetMaxValue(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("minValue"))) 
		{
			SetMinValue(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("Value"))) 
		{
			SetValue(_tcstol(strValue, NULL, 10));
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
	}

	bool CProgressUI::IsHorizontal()
	{
		return MPGS_HORIZONTAL & m_dwStyle;
	}

	void CProgressUI::SetHorizontal(bool bHorizontal)
	{
		if(bHorizontal)
		{
			m_dwStyle |= MPGS_HORIZONTAL;
		}
		else
		{
			m_dwStyle &= (~MPGS_HORIZONTAL);
		}
	}

	int CProgressUI::GetMinValue() const
	{
		return m_nMinValue;
	}

	void CProgressUI::SetMinValue(int nMin)
	{
		m_nMinValue = MAX(0,nMin);
		SetValue(m_nValue);
	}

	int CProgressUI::GetMaxValue() const
	{
		return m_nMaxValue;
	}

	void CProgressUI::SetMaxValue(int nMax)
	{
		m_nMaxValue = nMax;
		SetValue(m_nValue);
	}

	int CProgressUI::GetValue() const
	{
		return m_nValue;
	}

	void CProgressUI::SetValue(int nValue)
	{
		this->CallWndProc(m_pShareInfo ? m_pShareInfo->hWnd : NULL, WMU_SETVALUE, nValue, 0);
	}

	void CProgressUI::PaintStatusImage( const RECT& rcItem, const RECT& rcPaint)
	{
		RECT rcDraw = rcItem;
		int nValue = m_nValue;
		int nShift = 0;
		if(MPGS_VERTICAL & m_dwStyle)//´¹Ö±
		{
			nShift = m_nValue * (rcItem.bottom - rcItem.top) / m_nMaxValue ;
			rcDraw.top = rcDraw.bottom - nShift;
		}
		else//Ë®Æ½
		{
			nShift = m_nValue * (rcItem.right - rcItem.left) / m_nMaxValue ;
			rcDraw.right = rcDraw.left + nShift;
		}

		__super::PaintStatusImage(rcDraw, rcPaint);
	}

	LRESULT CProgressUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		int nOldValue = m_nValue;
		switch (message)
		{
		case WMU_SETVALUE:
			{
				m_nValue = MIN(m_nMaxValue, MAX(m_nMinValue, wParam));
				this->Invalidate();
				SendNotify(!hWnd, SelectChange, m_nValue, nOldValue);
				return 0;
			}break;
		}
		return __super::WndProc(hWnd, message, wParam, lParam);
	}
}