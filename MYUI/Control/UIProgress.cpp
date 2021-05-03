
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

	const CMuiString CProgressUI::g_strClassName(_T("ProgressUI"));

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
		this->CallWndProc(WMU_SETVALUE, nValue, 0);
	}

	void CProgressUI::PaintStatusImage(const RECT& rcUpdate)
	{
		RECT rcDraw = m_rcClient;
		RECT rcOldClient = m_rcClient;
		int nValue = m_nValue;
		int nShift = 0;
		if(MPGS_VERTICAL & m_dwStyle)//垂直
		{
			nShift = m_nValue * (rcDraw.bottom - rcDraw.top) / m_nMaxValue ;
			rcDraw.top = rcDraw.bottom - nShift;
		}
		else//水平
		{
			nShift = m_nValue * (rcDraw.right - rcDraw.left) / m_nMaxValue ;
			rcDraw.right = rcDraw.left + nShift;
		}

		//欺骗下层，让下层帮我们完成绘制
		m_rcClient = rcDraw;
		__super::PaintStatusImage(rcUpdate);
		m_rcClient = rcOldClient;
	}

	LRESULT CProgressUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		int nOldValue = m_nValue;
		switch (message)
		{
		case WMU_SETVALUE:
			{
				m_nValue = MIN(m_nMaxValue, MAX(m_nMinValue, wParam));
				this->Invalidate();
				SendNotify(EnumNotify::SelectChange, m_nValue, nOldValue);
				return 0;
			}break;
		}
		return __super::WndProc(message, wParam, lParam);
	}
}