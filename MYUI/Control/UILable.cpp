
#include "UILable.h"

namespace MYUI
{
	CLableUI::CLableUI()
		: m_refNormalColor(NULL)
		, m_refHotColor(NULL)
		, m_refSelectColor(NULL)
		, m_refFocusedColor(NULL)
		, m_refPushedColor(NULL)
		, m_refHotTextColor(NULL)
		, m_refSelectTextColor(NULL)
		, m_refFocusedTextColor(NULL)
		, m_refPushedTextColor(NULL)
		, m_nHotBorderSize(0)
		, m_refHotBorderColor(NULL)
        , m_nFocusedBorderSize(0)
        , m_refFocusedBorderColor(NULL)
	{
		EmptyRect(m_rcTextPadding);
	}

	CLableUI::~CLableUI()
	{
	}

	CMuiString CLableUI::g_strClassName(_T("LableUI"));

	CMuiString CLableUI::GetClassName() const
	{
		return CLableUI::g_strClassName;
	}

	void CLableUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		TCHAR strText[64];
		if(0 == _tcsicmp(strItem, _T("textPadding"))) 
		{
			SetTextPadding(CMuiRect(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("normalcolor"))) 
		{
			SetNormalColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("hotcolor"))) 
		{
			SetHotColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("selectcolor"))) 
		{
			SetSelectColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("pushedcolor"))) 
		{
			SetPushedColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("focusedcolor"))) 
		{
			SetFocusedColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("normalImage"))) 
		{
			SetNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("hotImage"))) 
		{
			SetHotImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("selectImage"))) 
		{
			SetSelectImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("pushedImage"))) 
		{
			SetPushedImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("focusedImage"))) 
		{
			SetFocusedImage(strValue);
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
		else if(0 == _tcsicmp(strItem, _T("focusedtextcolor"))) 
		{
			SetFocusedTextColor(CMuiColor(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("align")))
		{
			int index = 0;
			DWORD dwStyle = GetStyle();
			strText[0] = _T('\0');
			while(_T('\0') != *strValue)
			{
				if(_T(';') != *strValue)
				{
					strText[index++] = *strValue;
				}
				
				if(_T(';') == strValue[0] || _T('\0') == strValue[1])
				{
					strText[index] = _T('\0');
					index = 0;
					if(0 == _tcsicmp(strText, _T("left")))
					{
						dwStyle |= MLBES_LEFT;
					}
					else if(0 == _tcsicmp(strText, _T("right")))
					{
						dwStyle |= MLBES_RIGHT;
					}
					else if(0 == _tcsicmp(strText, _T("center")))
					{
						dwStyle |= MLBES_CENTER;
					}
					else if(0 == _tcsicmp(strText, _T("vcenter")))
					{
						dwStyle |= MLBES_VCENTER;
					}
					else
					{
						ASSERT(0 && "CLableUI::SetAttribute strItem=align strValue is unknow");
					}

					strText[0] = _T('\0');
				}

				*strValue++;
			}

			SetStyle(dwStyle);
		}
		else if(0 == _tcsicmp(strItem, _T("HotBorderSize"))) 
		{
			SetHotBorderSize(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("HotBorderColor"))) 
		{
			SetHotBorderColor(CMuiColor(strValue));
		}
        else if(0 == _tcsicmp(strItem, _T("FocusedBorderSize"))) 
		{
			SetFocusedBorderSize(_tcstol(strValue, NULL, 10));
		}
		else if(0 == _tcsicmp(strItem, _T("FocusedBorderColor"))) 
		{
			SetFocusedBorderColor(CMuiColor(strValue));
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
	}

	void CLableUI::SetTextPadding(const RECT &rect)
	{
		m_rcTextPadding = rect;
	}

	const RECT &CLableUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CLableUI::SetNormalImage(LPCTSTR strImage)
	{
		m_strNormalImage = strImage;
	}

	LPCTSTR CLableUI::GetNormalImage() const
	{
		return m_strNormalImage;
	}

	void CLableUI::SetHotImage(LPCTSTR strImage)
	{
		m_strHotImage = strImage;
	}

	LPCTSTR CLableUI::GetHotImage() const
	{
		return m_strHotImage;
	}

	void CLableUI::SetSelectImage(LPCTSTR strImage)
	{
		m_strSelectImage = strImage;
	}

	LPCTSTR CLableUI::GetSelectImage() const
	{
		return m_strSelectImage;
	}

	void CLableUI::SetPushedImage(LPCTSTR strImage)
	{
		m_strPushedImage = strImage;
	}

	LPCTSTR CLableUI::GetPushedImage() const
	{
		return m_strPushedImage;
	}

	void CLableUI::SetFocusedImage(LPCTSTR strImage)
	{
		m_strFocusedImage = strImage;
	}

	LPCTSTR CLableUI::GetFocusedImage() const
	{
		return m_strFocusedImage;
	}

	void CLableUI::SetNormalColor(ARGBREF argb)
	{
		m_refNormalColor = argb;
	}

	ARGBREF CLableUI::GetNormalColor()
	{
		return m_refNormalColor;
	}

	void CLableUI::SetHotColor(ARGBREF argb)
	{
		m_refHotColor = argb;
	}

	ARGBREF CLableUI::GetHotColor()
	{
		return m_refHotColor;
	}

	void CLableUI::SetSelectColor(ARGBREF argb)
	{
		m_refSelectColor = argb;
	}

	ARGBREF CLableUI::GetSelectColor()
	{
		return m_refSelectColor;
	}

	void CLableUI::SetPushedColor(ARGBREF argb)
	{
		m_refPushedColor = argb;
	}

	ARGBREF CLableUI::GetPushedColor()
	{
		return m_refPushedColor;
	}

	void CLableUI::SetFocusedColor(ARGBREF argb)
	{
		m_refFocusedColor = argb;
	}

	ARGBREF CLableUI::GetFocusedColor()
	{
		return m_refFocusedColor;
	}

	void CLableUI::SetHotTextColor(ARGBREF argb)
	{
		m_refHotTextColor = argb;
	}

	ARGBREF CLableUI::GetHotTextColor()
	{
		return m_refHotTextColor;
	}

	void CLableUI::SetSelectTextColor(ARGBREF argb)
	{
		m_refSelectTextColor = argb;
	}

	ARGBREF CLableUI::GetSelectTextColor()
	{
		return m_refSelectTextColor;
	}

	void CLableUI::SetPushedTextColor(ARGBREF argb)
	{
		m_refPushedTextColor = argb;
	}

	ARGBREF CLableUI::GetPushedTextColor()
	{
		return m_refPushedTextColor;
	}

	void CLableUI::SetFocusedTextColor(ARGBREF argb)
	{
		m_refFocusedTextColor = argb;
	}

	ARGBREF CLableUI::GetFocusedTextColor()
	{
		return m_refFocusedTextColor;
	}

	int CLableUI::GetHotBorderSize()
	{
		return m_nHotBorderSize;
	}

	void CLableUI::SetHotBorderSize(int size)
	{
		m_nHotBorderSize = size;
	}

	ARGBREF CLableUI::GetHotBorderColor() const
	{
		return m_refHotBorderColor;
	}

	void CLableUI::SetHotBorderColor(ARGBREF refColor)
	{
		m_refHotBorderColor = refColor;
	}

    int CLableUI::GetFocusedBorderSize()
    {
        return m_nFocusedBorderSize;
    }

	void CLableUI::SetFocusedBorderSize(int size)
    {
        m_nFocusedBorderSize = size;
    }

	ARGBREF CLableUI::GetFocusedBorderColor() const
    {
        return m_refFocusedBorderColor;
    }

	void CLableUI::SetFocusedBorderColor(ARGBREF refColor)
    {
        m_refFocusedBorderColor = refColor;
    }

	void CLableUI::SetState(DWORD dwState)
	{
		if(m_dwState == dwState) return ;

		__super::SetState(dwState);
		//Invalidate();
	}

	void CLableUI::PaintBorder(const RECT& rcItem, const RECT& rcPaint)
	{
        int nBorderSize = 0;
        ARGBREF refBorderColor = NULL;
		if((STATE_HOT & m_dwState) && m_nHotBorderSize && GetAValue(m_refHotBorderColor))
		{
			nBorderSize = m_nHotBorderSize;
            refBorderColor = m_refHotBorderColor;
		}

        if((STATE_FOCUS & m_dwState) && m_nFocusedBorderSize && GetAValue(m_refFocusedBorderColor))
		{
            nBorderSize = m_nFocusedBorderSize;
            refBorderColor = m_refFocusedBorderColor;
		}

        if(nBorderSize && refBorderColor)
        {
            m_pShareInfo->pRenderEngine->OnDrawBroder(rcItem ,
				refBorderColor, nBorderSize, m_szBorderRound);
			return;
        }
		
		return __super::PaintBorder(rcItem, rcPaint);
	}

	void CLableUI::PaintText(const RECT& rcItem, const RECT& rcPaint)
	{
		ARGBREF refTextColor = m_refTextColor;
		RECT rcClient = m_rcClient;
		DWORD dwDrawTextStyle = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
		OffsetRect(&rcClient, rcItem.left, rcItem.top);
		HFONT hFont = (HFONT)m_pShareInfo->FontArray->Select(m_nFontId);
		if(m_strText.IsEmpty()) return ;

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

			if(m_dwState & STATE_FOCUS && NULL != m_refFocusedTextColor)
			{
				refTextColor = m_refFocusedTextColor;
			}

			if(m_dwState & STATE_PUSHED && m_dwState & STATE_HOT && NULL != m_refPushedTextColor)
			{
				refTextColor = m_refPushedTextColor;
			}
		}

		if(NULL != m_dwStyle)
		{
			dwDrawTextStyle = DT_SINGLELINE ;
			if(MLBES_LEFT & m_dwStyle)
			{
				dwDrawTextStyle |= DT_LEFT;
			}

			if(MLBES_RIGHT & m_dwStyle)
			{
				dwDrawTextStyle |= DT_RIGHT;
			}

			if(MLBES_CENTER & m_dwStyle)
			{
				dwDrawTextStyle |= DT_CENTER;
			}

			if(MLBES_VCENTER & m_dwStyle)
			{
				dwDrawTextStyle |= DT_VCENTER;
			}
		}

		m_pShareInfo->pRenderEngine->OnDrawText(rcClient, m_strText,
			refTextColor, hFont, dwDrawTextStyle);
	}

	void CLableUI::PaintStatusImage(const RECT& rcItem, const RECT& rcPaint)
	{
		RECT rcClient = rcItem;
		LPCTSTR strImage = m_strNormalImage;
		ARGBREF refColor = m_refNormalColor;
		if(false == m_bEnabled) return;


		//OffsetRect(&rcClient, rcItem.left, rcItem.top);
		//通过状态优先级，来判断显示哪一张图片
		if(m_dwState & STATE_HOT && !m_strHotImage.IsEmpty())
		{
			strImage = m_strHotImage;
		}

		if(m_dwState & STATE_SELECT && !m_strSelectImage.IsEmpty())
		{
			strImage = m_strSelectImage;
		}

		if(m_dwState & STATE_FOCUS && !m_strFocusedImage.IsEmpty())
		{
			strImage = m_strFocusedImage;
		}

		if(m_dwState & STATE_PUSHED &&  m_dwState & STATE_HOT && !m_strPushedImage.IsEmpty())
		{
			strImage = m_strPushedImage;
		}

		if(_T('\0') != strImage[0])
		{
			m_pShareInfo->pRenderEngine->OnDrawImage(rcClient, strImage);
			return;
		}

		//如果没有图片，那就绘制颜色
		if(m_dwState & STATE_HOT && NULL != m_refHotColor)
		{
			refColor = m_refHotColor;
		}

		if(m_dwState & STATE_SELECT && NULL != m_refSelectColor)
		{
			refColor = m_refSelectColor;
		}

		if(m_dwState & STATE_FOCUS && NULL != m_refFocusedColor)
		{
			refColor = m_refFocusedColor;
		}

		if(m_dwState & STATE_PUSHED && m_dwState & STATE_HOT && NULL != m_refPushedColor)
		{
			refColor = m_refPushedColor;
		}

		if(NULL != refColor)
		{
			m_pShareInfo->pRenderEngine->OnDrawColor(rcClient, refColor);
			return ;
		}
		
		return ;
	}

	LRESULT CLableUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		CControlUI * pControl = NULL;
		POINT point = {0,0};
		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				if(true == m_bEnabled)
				{
					this->SetCapture();
					if(NULL != m_refPushedColor || !m_strPushedImage.IsEmpty() || 
						NULL != m_refPushedTextColor)
					{
						this->Invalidate();
					}
				}
			}break;
		case WM_LBUTTONUP:
			{
				if(true == m_bEnabled)
				{
					if(NULL != m_refPushedColor || !m_strPushedImage.IsEmpty() || 
						NULL != m_refPushedTextColor)
					{
						this->Invalidate();
					}

					if(STATE_PUSHED & m_dwState && STATE_HOT & m_dwState)
					{
                        this->ReleaseCapture();
						this->CallWndProc(hWnd, WM_LBUTTONCLICK, wParam, lParam);
					}
				}
			}break;
		case WM_MOUSEMOVE:
			{
				point.x = (short)LOWORD(lParam);
				point.y = (short)HIWORD(lParam);
			}break;
		case WM_MOUSEENTER://鼠标第一次进入
		case WM_MOUSELEAVE://鼠标离开
			{
				//需要有对应的Hot设置才会更新
				if(true == m_bEnabled)
				{
					if(NULL != m_refHotColor || !m_strHotImage.IsEmpty() || 
						NULL != m_refHotTextColor)
					{
						this->Invalidate();
						break;//产生一次刷新即可
					}

					if(m_nHotBorderSize && m_refHotBorderColor)
					{
						this->Invalidate();
						break;//产生一次刷新即可
					}
				}
			}break;
		case WM_MOUSEHOVER://鼠标停留
			{
			}break;
		default:
			break;
		}

		return __super::WndProc(hWnd, message, wParam, lParam);
	}
}