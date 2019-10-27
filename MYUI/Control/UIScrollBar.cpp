
#include "UIScrollBar.h"

namespace MYUI
{
	CScrollBarUI::CScrollBarUI(ScrollBarStyle style)
		: m_nMaxShift(0)
		, m_nNowShift(0)
		, m_bActive(false)
		, m_dwPointIn(NULL)
		, m_dwShiftStyle(ScrollShiftStyle::Auto)
	{
		m_dwStyle = style;

		if(m_dwStyle & ScrollBarStyle::BarVertical)
		{
			SetWidth(GetSystemMetrics(SM_CXVSCROLL));
		}

		if(m_dwStyle & ScrollBarStyle::BarHorizontal)
		{
			SetHeight(GetSystemMetrics(SM_CYHSCROLL));
		}

		m_ptOldMousePos.x = m_ptOldMousePos.y = 0;
		EmptyRect(m_rcBarIntersect);
		EmptyRect(m_rcDragButton);
		EmptyRect(m_rcFrontButton);
		EmptyRect(m_rcFinalButton);
		EmptyRect(m_rcTrackButton);
	}

	CScrollBarUI::~CScrollBarUI()
	{
	}
	
	CMuiString CScrollBarUI::g_strClassName(_T("ScrollBarUI"));
	CMuiString CHorizontailScrollBarUI::g_strClassName(_T("HScrollBarUI"));
	CMuiString CVerticalScrollBarUI::g_strClassName(_T("VScrollBarUI"));

	CMuiString CScrollBarUI::GetClassName() const
	{
		return CScrollBarUI::g_strClassName;
	}

	void CScrollBarUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("FrontNormalImage"))) 
		{
			SetFrontNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("FrontHotImage"))) 
		{
			SetFrontHotImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("FrontPushedImage"))) 
		{
			SetFrontPushedImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("FrontDisabledImage"))) 
		{
			SetFrontDisabledImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("FinalNormalImage"))) 
		{
			SetFinalNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("FinalHotImage"))) 
		{
			SetFinalHotImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("FinalPushedImage"))) 
		{
			SetFinalPushedImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("FinalDisabledImage"))) 
		{
			SetFinalDisabledImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("TrackNormalImage"))) 
		{
			SetTrackNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("TrackHotImage"))) 
		{
			SetTrackHotImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("TrackPushedImage"))) 
		{
			SetTrackPushedImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("TrackDisabledImage"))) 
		{
			SetTrackDisabledImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("DragBtnNormalImage"))) 
		{
			SetDragBtnNormalImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("DragBtnHotImage"))) 
		{
			SetDragBtnHotImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("DragBtnPushedImage"))) 
		{
			SetDragBtnPushedImage(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("DragBtnDisabledImage"))) 
		{
			SetDragBtnDisabledImage(strValue);
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
	}

	void CScrollBarUI::SetFilterFloatType(EnumFloatType type)
	{
		return;
	}

	void CScrollBarUI::SetActive(bool bActive)
	{
		if(m_bActive == bActive) return;

		if(bActive != IsShowing())
		{
			m_bActive = bActive;
			this->Update();
			return ;
		}
		m_bActive = bActive;
	}

	bool CScrollBarUI::IsActive()
	{
		return m_bActive;
	}

	bool CScrollBarUI::IsShowing()//是否正在显示
	{
		if(m_bActive) return true;

		if(m_bVisible && m_nMaxShift) return true;

		return false;
	}

	void CScrollBarUI::SetBarIntersect(RECT &rect)
	{
		m_rcBarIntersect = rect;
	}

	ScrollBarStyle CScrollBarUI::GetStyle() const
	{
		return m_dwStyle;
	}

	//位置
	void CScrollBarUI::SetShiftStyle(ScrollShiftStyle style)
	{
	}

	ScrollShiftStyle CScrollBarUI::GetShiftStyle()
	{
		return m_dwShiftStyle;
	}

	void CScrollBarUI::SetDragBarMaxShift(int nShift)
	{
		if(nShift == m_nMaxShift ) return;
		m_nMaxShift = MAX(0, nShift);
		CalcDragBarRect();
		SetDragBarShift(m_nNowShift);
	}

	int CScrollBarUI::GetDragBarMaxShift()
	{
		return m_nMaxShift;
	}

	void CScrollBarUI::SetDragBarShift(int nShift)
	{
		nShift = MAX(0, MIN(nShift, m_nMaxShift));
		if(m_nNowShift == nShift) return;
		m_nNowShift = nShift;

		ASSERT(m_pParentContrl && "滚动条必须拥有父窗口！");
		IScrollBarMove * pInterface = dynamic_cast<IScrollBarMove *>(m_pParentContrl);
		if(pInterface)
		{
			pInterface->OnScrollBarMove(this, m_nNowShift);
		}
	}

	int CScrollBarUI::GetDragBarShift()
	{
		return m_nNowShift;
	}

	bool CScrollBarUI::CalcDragBarRect()
	{
		EmptyRect(m_rcDragButton);//置空
		if(m_dwStyle & ScrollBarStyle::BarVertical)
		{
			//拖把按钮
			if(m_rcTrackButton.bottom - m_rcTrackButton.top > m_ItemSize.cx / 2
				&& m_nMaxShift)
			{
				//需要有足够的位置才能显示
				m_rcDragButton.left = m_rcTrackButton.left;
				m_rcDragButton.top = m_rcTrackButton.top;
				m_rcDragButton.right = m_rcTrackButton.right;

				m_rcDragButton.bottom = m_rcDragButton.top + MAX(m_ItemSize.cx / 2, \
					(m_rcTrackButton.bottom - m_rcTrackButton.top) \
					* (m_rcRawItem.bottom - m_rcRawItem.top) \
					/ ((m_rcRawItem.bottom - m_rcRawItem.top) + m_nMaxShift));
			}
		}

		if(m_dwStyle & ScrollBarStyle::BarHorizontal)
		{
			//拖把按钮
			if(m_rcTrackButton.bottom - m_rcTrackButton.top > m_ItemSize.cy / 2
				&& m_nMaxShift)
			{
				//需要有足够的位置才能显示
				m_rcDragButton.left = m_rcTrackButton.left;
				m_rcDragButton.top = m_rcTrackButton.top;
				m_rcDragButton.bottom = m_rcTrackButton.bottom;

				m_rcDragButton.right = m_rcDragButton.left + MAX(m_ItemSize.cy / 2, \
					(m_rcTrackButton.right - m_rcTrackButton.left) \
					* (m_rcRawItem.right - m_rcRawItem.left) \
					/ ((m_rcRawItem.right - m_rcRawItem.left) + m_nMaxShift));
			}
		}

		return !!IsValidRect(m_rcDragButton);
	}

	bool CScrollBarUI::SetItem(RECT & rcItem, bool bMustUpdate)
	{
		if(false == __super::SetItem(rcItem, bMustUpdate)) return false;
		//排除交汇区域后，计算出滚动条可绘制的区域
		/*m_rcClient*/
		if(false == IsEmptyRect(m_rcBarIntersect))//不为空
		{
			if(m_dwStyle & ScrollBarStyle::BarVertical)
			{
				if(m_rcBarIntersect.top == m_rcRawItem.top)
				{
					m_rcClient.top = m_rcBarIntersect.bottom;
				}
				if(m_rcBarIntersect.bottom == m_rcRawItem.bottom)
				{
					m_rcClient.bottom = m_rcBarIntersect.top;
				}
			}
			if(m_dwStyle & ScrollBarStyle::BarHorizontal)
			{
				if(m_rcBarIntersect.left == m_rcRawItem.left)
				{
					m_rcClient.left = m_rcBarIntersect.right;
				}
				if(m_rcBarIntersect.right == m_rcRawItem.right)
				{
					m_rcClient.right = m_rcBarIntersect.left;
				}
			}
		}/*m_rcClient*/

		if(m_dwStyle & ScrollBarStyle::BarVertical)
		{
			//前导按钮
			m_rcFrontButton.left = m_rcClient.left;
			m_rcFrontButton.top = m_rcClient.top;
			m_rcFrontButton.right = m_rcClient.right;
			m_rcFrontButton.bottom = m_rcClient.top + m_ItemSize.cx;

			//后导按钮
			m_rcFinalButton.left = m_rcClient.left;
			m_rcFinalButton.top = m_rcClient.bottom - m_ItemSize.cx;
			m_rcFinalButton.right = m_rcClient.right;
			m_rcFinalButton.bottom = m_rcClient.bottom;

			//拖把轨道
			m_rcTrackButton.left = m_rcClient.left;
			m_rcTrackButton.top = m_rcFrontButton.bottom;
			m_rcTrackButton.right = m_rcClient.right;
			m_rcTrackButton.bottom = m_rcFinalButton.top;
		}

		if(m_dwStyle & ScrollBarStyle::BarHorizontal)
		{
			//前导按钮
			m_rcFrontButton.left = m_rcClient.left;
			m_rcFrontButton.top = m_rcClient.top;
			m_rcFrontButton.right = m_rcClient.left + m_ItemSize.cy;
			m_rcFrontButton.bottom = m_rcClient.bottom;

			//后导按钮
			m_rcFinalButton.left = m_rcClient.right - m_ItemSize.cy;
			m_rcFinalButton.top = m_rcClient.top;
			m_rcFinalButton.right = m_rcClient.right;
			m_rcFinalButton.bottom = m_rcClient.bottom;

			//拖把轨道
			m_rcTrackButton.left = m_rcFrontButton.right;
			m_rcTrackButton.top = m_rcClient.top;
			m_rcTrackButton.right = m_rcFinalButton.left;
			m_rcTrackButton.bottom = m_rcClient.bottom;
		}

		CalcDragBarRect();
		return true;
	}

	//绘制
	bool CScrollBarUI::OnPaint( RECT rcItem, RECT rcPaint, RECT rcUpdate)
	{
		//RECT rcItem = rcItemFixed;
		//RECT rcItem = GetRawRect(m_rcItemPaintFixed);
		if(false == m_bVisible) return false;

		RECT rcDraw = m_rcRawItem;
		//rcPaint是父窗口提供的有效绘图区域
		//rcClient参数是m_rcItem的绝对位置, 所以要将m_rcClient偏移到绝对位置：
		OffsetRect(&rcDraw, rcItem.left , rcItem.top);
		
		//PaintBkColor(rcDraw);

		PaintFrontButton(rcItem);
		PaintFinalButton(rcItem);
		PaintTrackButton(rcItem);
		PaintDragButton(rcItem);
		//PaintIntersect(hMemDc, rcSrcDraw);

		//PaintBorder(rcDraw);
		return true;
	}

	void CScrollBarUI::PaintTrackButton(const RECT &rcItem)
	{
		RECT rcDest = m_rcTrackButton;
		OffsetRect(&rcDest, rcItem.left, rcItem.top);
		
		LPCTSTR strImage = _T("");;
		//通过状态优先级，来判断显示哪一张图片
		if(!m_strTrackNormalImage.IsEmpty())
		{
			strImage = m_strTrackNormalImage.GetData();
		}

		if(STATE_HOT & m_dwState && !m_strTrackHotImage.IsEmpty())
		{
			strImage = m_strTrackHotImage.GetData();
		}

		if(STATE_HOT & m_dwState && STATE_PUSHED & m_dwState &&
			!m_strTrackPushedImage.IsEmpty())
		{
			strImage = m_strTrackPushedImage.GetData();
		}

		if(_T('\0') != strImage[0])
		{
			m_pShareInfo->pRenderEngine->OnDrawImage(rcDest, strImage);
		}
		else
		{
			m_pShareInfo->pRenderEngine->OnDrawBroder(rcDest,
			m_refBorderColor, m_nBorderSize,CMuiSize(0,0));
		}
	}

	void CScrollBarUI::PaintFrontButton(const RECT &rcItem)
	{
		RECT rcDest = m_rcFrontButton;
		OffsetRect(&rcDest, rcItem.left, rcItem.top);

		LPCTSTR strImage = _T("");;

		//通过状态优先级，来判断显示哪一张图片
		if(!m_strFrontNormalImage.IsEmpty())
		{
			strImage = m_strFrontNormalImage.GetData();
		}

		if(STATE_HOT & m_dwState && !m_strFrontHotImage.IsEmpty())
		{
			strImage = m_strFrontHotImage.GetData();
		}

		if(STATE_HOT & m_dwState && STATE_PUSHED & m_dwState &&
			!m_strTrackPushedImage.IsEmpty())
		{
			strImage = m_strFrontPushedImage.GetData();
		}

		if(_T('\0') != strImage[0])
		{
			m_pShareInfo->pRenderEngine->OnDrawImage(rcDest, strImage);
		}
		else
		{
			m_pShareInfo->pRenderEngine->OnDrawBroder(rcDest,
			m_refBorderColor, m_nBorderSize, CMuiSize(0,0));
		}
	}

	void CScrollBarUI::PaintFinalButton(const RECT &rcItem)
	{
		RECT rcDest = m_rcFinalButton;
		OffsetRect(&rcDest, rcItem.left, rcItem.top);
		
		LPCTSTR strImage = _T("");;
		//通过状态优先级，来判断显示哪一张图片
		if(!m_strFinalNormalImage.IsEmpty())
		{
			strImage = m_strFinalNormalImage.GetData();
		}

		if(STATE_HOT & m_dwState && !m_strFinalHotImage.IsEmpty())
		{
			strImage = m_strFinalHotImage.GetData();
		}

		if(STATE_HOT & m_dwState && STATE_PUSHED & m_dwState &&
			!m_strTrackPushedImage.IsEmpty())
		{
			strImage = m_strFinalPushedImage.GetData();
		}

		if(_T('\0') != strImage[0])
		{
			m_pShareInfo->pRenderEngine->OnDrawImage(rcDest, strImage);
		}
		else
		{
			m_pShareInfo->pRenderEngine->OnDrawBroder(rcDest,
			m_refBorderColor, m_nBorderSize,CMuiSize(0,0));
		}

	}

	void CScrollBarUI::PaintDragButton(const RECT &rcItem)
	{
		RECT rcDest = m_rcDragButton;
		OffsetRect(&rcDest, rcItem.left, rcItem.top);
		if(m_dwStyle & ScrollBarStyle::BarVertical)
		{
			OffsetRect(&rcDest, 0 , GetDragBarOffset());
		}
		if(m_dwStyle & ScrollBarStyle::BarHorizontal)
		{
			OffsetRect(&rcDest, GetDragBarOffset(), 0);
		}

		LPCTSTR strImage = _T("");;
		//通过状态优先级，来判断显示哪一张图片
		if(!m_strDragBtnNormalImage.IsEmpty())
		{
			strImage = m_strDragBtnNormalImage.GetData();
		}

		if(STATE_HOT & m_dwState && !m_strDragBtnHotImage.IsEmpty())
		{
			strImage = m_strDragBtnHotImage.GetData();
		}

		if(STATE_HOT & m_dwState && STATE_PUSHED & m_dwState &&
			!m_strDragBtnPushedImage.IsEmpty())
		{
			strImage = m_strDragBtnPushedImage.GetData();
		}

		if(_T('\0') != strImage[0])
		{
			m_pShareInfo->pRenderEngine->OnDrawImage(rcDest, strImage);
		}
		else
		{
			m_pShareInfo->pRenderEngine->OnDrawBroder(rcDest,
			m_refBorderColor, m_nBorderSize,CMuiSize(0,0));
		}

		return;
	}

	void CScrollBarUI::PaintIntersect(const RECT &rcItem)
	{
		RECT rcDest = m_rcBarIntersect;
		SIZE siOffset;
		HBRUSH hBrush = CreateSolidBrush(RGB(200,200,200));
		
		DeleteObject(hBrush);
		return;
	}

	void CScrollBarUI::SetState(DWORD dwState)
	{
		if(dwState != m_dwState)
		{
			TRACE(_T("CScrollBarUI::SetState : 0x%x"), dwState);
			__super::SetState(dwState);
			this->Invalidate();
		}
		return ;
	}

	DWORD CScrollBarUI::CalcPointIn(const POINT& pt)
	{
		RECT rcDest = m_rcDragButton;
		if(PointInRect(pt, m_rcFrontButton))
		{
			return SBPI_FRONTBTN;
		}
		else if(PointInRect(pt, m_rcFinalButton))
		{
			return SBPI_FINALBTN;
		}
		else if(PointInRect(pt, m_rcBarIntersect))
		{
			return SBPI_INTERSECT;
		}
		else if(PointInRect(pt, m_rcTrackButton))
		{
			//命中轨道时，先判断一下是否命中拖把按钮
			if(m_dwStyle & ScrollBarStyle::BarVertical)
			{
				OffsetRect(&rcDest, 0, GetDragBarOffset());
			}
			else if(m_dwStyle & ScrollBarStyle::BarHorizontal)
			{
				OffsetRect(&rcDest, GetDragBarOffset(), 0);
			}

			if(PointInRect(pt, rcDest))
			{
				return SBPI_DRAGBTN;
			}
			else
			{
				return SBPI_TRACKBTN;
			}
		}
		return NULL;
	}

	int CScrollBarUI::GetDragBarOffset()
	{
		int nMaxOffset = 0;
		int nOffset = 0;
		if(m_dwStyle & ScrollBarStyle::BarVertical)
		{
			nMaxOffset = (m_rcTrackButton.bottom - m_rcTrackButton.top) -
				(m_rcDragButton.bottom - m_rcDragButton.top);
		}
		if(m_dwStyle & ScrollBarStyle::BarHorizontal)
		{
			nMaxOffset = (m_rcTrackButton.right - m_rcTrackButton.left) -
				(m_rcDragButton.right - m_rcDragButton.left);
		}

		if(m_nMaxShift)
		{
			nOffset = nMaxOffset * m_nNowShift / m_nMaxShift;
		}
		
		return nOffset;
	}

	void CScrollBarUI::SetFrontNormalImage(LPCTSTR pStrImage)
	{
		m_strFrontNormalImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetFrontNormalImage()
	{
		return m_strFrontNormalImage;
	}

	void CScrollBarUI::SetFrontHotImage(LPCTSTR pStrImage)
	{
		m_strFrontHotImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetFrontHotImage()
	{
		return m_strFrontHotImage;
	}

	void CScrollBarUI::SetFrontPushedImage(LPCTSTR pStrImage)
	{
		m_strFrontPushedImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetFrontPushedImage()
	{
		return m_strFrontPushedImage;
	}

	void CScrollBarUI::SetFrontDisabledImage(LPCTSTR pStrImage)
	{
		m_strFrontDisabledImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetFrontDisabledImage()
	{
		return m_strFrontDisabledImage;
	}

	void CScrollBarUI::SetFinalNormalImage(LPCTSTR pStrImage)
	{
		m_strFinalNormalImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetFinalNormalImage()
	{
		return m_strFinalNormalImage;
	}

	void CScrollBarUI::SetFinalHotImage(LPCTSTR pStrImage)
	{
		m_strFinalHotImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetFinalHotImage()
	{
		return m_strFinalHotImage;
	}

	void CScrollBarUI::SetFinalPushedImage(LPCTSTR pStrImage)
	{
		m_strFinalPushedImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetFinalPushedImage()
	{
		return m_strFinalPushedImage;
	}

	void CScrollBarUI::SetFinalDisabledImage(LPCTSTR pStrImage)
	{
		m_strFrontDisabledImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetFinalDisabledImage()
	{
		return m_strFinalDisabledImage;
	}

	void CScrollBarUI::SetTrackNormalImage(LPCTSTR pStrImage)
	{
		m_strTrackNormalImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetTrackNormalImage()
	{
		return m_strTrackNormalImage;
	}

	void CScrollBarUI::SetTrackHotImage(LPCTSTR pStrImage)
	{
		m_strTrackHotImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetTrackHotImage()
	{
		return m_strTrackHotImage;
	}

	void CScrollBarUI::SetTrackPushedImage(LPCTSTR pStrImage)
	{
		m_strTrackPushedImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetTrackPushedImage()
	{
		return m_strTrackPushedImage;
	}

	void CScrollBarUI::SetTrackDisabledImage(LPCTSTR pStrImage)
	{
		m_strTrackDisabledImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetTrackDisabledImage()
	{
		return m_strTrackDisabledImage;
	}

	void CScrollBarUI::SetDragBtnNormalImage(LPCTSTR pStrImage)
	{
		m_strDragBtnNormalImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetDragBtnNormalImage()
	{
		return m_strDragBtnNormalImage;
	}

	void CScrollBarUI::SetDragBtnHotImage(LPCTSTR pStrImage)
	{
		m_strDragBtnHotImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetDragBtnHotImage()
	{
		return m_strDragBtnHotImage;
	}

	void CScrollBarUI::SetDragBtnPushedImage(LPCTSTR pStrImage)
	{
		m_strDragBtnPushedImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetDragBtnPushedImage()
	{
		return m_strDragBtnPushedImage;
	}

	void CScrollBarUI::SetDragBtnDisabledImage(LPCTSTR pStrImage)
	{
		m_strDragBtnDisabledImage = pStrImage;
	}

	LPCTSTR CScrollBarUI::GetDragBtnDisabledImage()
	{
		return m_strDragBtnDisabledImage;
	}

	LRESULT CScrollBarUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		bool bRet = false;
		int nShift = 0;
		int nMaxOffset = 0;
		DWORD dwPos = 0;

		if(false == m_bEnabled) return false;

		switch(message)
		{
		case WM_TIMER:
			{
				return 0;
			}break;
		case WM_MOUSEWHEEL:
			{
				nShift = (short)HIWORD(wParam);//返回一个滚动的大小,一般是120的倍数,正数代表滚动向前,负数代表向后
				//short nKey = (short)LOWORD(msg.wParam); //返回当前同时用鼠标键的时候按下了哪些键
				//常规用户习惯是三个滑轮点作为一个周期，所以取每条滚动消息的三分一
				SetDragBarShift(m_nNowShift + nShift / 3 * -1);
				return false;
			}break;
		case WM_LBUTTONDOWN:
			{
				pt.x = GET_X_LPARAM(lParam);
				pt.y = GET_Y_LPARAM(lParam);
				m_dwPointIn = CalcPointIn(pt);

				this->SetCapture();
				dwPos = GetMessagePos();
				m_ptOldMousePos.x = GET_X_LPARAM(dwPos);
				m_ptOldMousePos.y = GET_Y_LPARAM(dwPos);

				if(SBPI_FRONTBTN == m_dwPointIn)
				{
					SetDragBarShift(m_nNowShift - 40);
				}
				else if(SBPI_FINALBTN == m_dwPointIn)
				{
					SetDragBarShift(m_nNowShift + 40);
				}
				else if(SBPI_TRACKBTN == m_dwPointIn)
				{
				}
				else
				{
					break;
				}

				this->Invalidate();
			}break;
		case WM_LBUTTONUP:
			{
				m_ptOldMousePos.x = m_ptOldMousePos.y = 0;
				if(STATE_PUSHED & m_dwState)
				{
					this->ReleaseCapture();
				}
				m_dwPointIn = NULL;
			}break;
		case WM_MOUSEMOVE:
			{
				if(SBPI_DRAGBTN == m_dwPointIn)
				{
					dwPos = GetMessagePos();
					pt.x = GET_X_LPARAM(dwPos);
					pt.y = GET_Y_LPARAM(dwPos);

					if(m_dwStyle & ScrollBarStyle::BarVertical)
					{
						nShift = pt.y - m_ptOldMousePos.y;

						nMaxOffset = (m_rcTrackButton.bottom - m_rcTrackButton.top) -
							(m_rcDragButton.bottom - m_rcDragButton.top);
					}
					if(m_dwStyle & ScrollBarStyle::BarHorizontal)
					{
						nShift = pt.x - m_ptOldMousePos.x;

						nMaxOffset = (m_rcTrackButton.right - m_rcTrackButton.left) -
							(m_rcDragButton.right - m_rcDragButton.left);
					}

					nShift = m_nMaxShift * nShift / nMaxOffset  ;

					//TRACE(_T("nShift = %d, nMaxOffset=%d, pt.x=%d, pt.y=%d"),
					//	nShift, nMaxOffset, m_ptOldMousePos.x, m_ptOldMousePos.y);
					SetDragBarShift(m_nNowShift + nShift);
					m_ptOldMousePos = pt;
				}
			}break;
		default:
			break;
		}
		return __super::WndProc(hWnd, message, wParam, lParam);
	}


}