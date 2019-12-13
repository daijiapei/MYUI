
#include "UICalendar.h"
#include "..\\Core\\UIWindow.h"
#include "..\\Layout\\UIVerticalLayout.h"
#include "..\\Layout\\UIHorizontalLayout.h"
namespace MYUI
{

/****************************************************************************
 * 日期控件相关的辅助函数
 *
 *
****************************************************************************/



#define get_pre_week(x)   (1==(x) ? 7 : (x)-1)
#define get_next_week(x)   (7==(x) ? 1 : (x)+1)

static const int mdate[] = {31,28,31,30,31,30,31,31,30,31,30,31};
static const TCHAR * mweek[] = {TEXT("一"),TEXT("二"),TEXT("三"),TEXT("四"),TEXT("五"),TEXT("六"),TEXT("日")};
static const TCHAR * chinese [] = {TEXT("星期一"),TEXT("星期二"),TEXT("星期三"),TEXT("星期四"),TEXT("星期五"),TEXT("星期六"),TEXT("星期日")};
static const TCHAR * english [] = {TEXT("MON"),TEXT("TUE"),TEXT("WED"),TEXT("THU"),TEXT("FRI"),TEXT("SAT"),TEXT("SUN")};
//计算星期几
static int weekday(int _year, int _mon, int _day)
{
    /* 
    蔡勒（Zeller）公式 
        历史上的某一天是星期几？未来的某一天是星期几？关于这个问题，有很多计算公式（两个通用计算公式和
    一些分段计算公式），其中最著名的是蔡勒（Zeller）公式。
        即w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1 
        
        公式中的符号含义如下，
         w：星期；
         c：年的高2位，即世纪-1
         y：年（两位数）；
         m：月（m大于等于3，小于等于14，即在蔡勒公式中，某年的1、2月要看作上一年的13、14月来计算，
              比如2003年1月1日要看作2002年的13月1日来计算）；
         d：日；
         [ ]代表取整，即只要整数部分。
        
        算出来的W除以7，余数是几就是星期几。如果余数是0，则为星期日。 
        如果结果是负数，负数求余数则需要特殊处理：
            负数不能按习惯的余数的概念求余数，只能按数论中的余数的定义求余。为了方便 
        计算，我们可以给它加上一个7的整数倍，使它变为一个正数，然后再求余数    
        
        以2049年10月1日（100周年国庆）为例，用蔡勒（Zeller）公式进行计算，过程如下： 
        蔡勒（Zeller）公式：w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1 
        =49+[49/4]+[20/4]-2×20+[26× (10+1)/10]+1-1 
        =49+[12.25]+5-40+[28.6] 
        =49+12+5-40+28 
        =54 (除以7余5) 
        即2049年10月1日（100周年国庆）是星期5。
    */
    int y, c, m, d;
    int w;
 
    if (_mon >= 3)
    {
        m = _mon;        
        y = _year % 100;
        c = _year / 100;
        d = _day;
    }
    else    /* 某年的1、2月要看作上一年的13、14月来计算 */
    {
        m = _mon + 12;
        y = (_year - 1) % 100;
        c = (_year - 1) / 100;
        d = _day;
    }
 
    w = y + y / 4 +  c / 4 - 2 * c + ((int)26*(m+1))/10 + d - 1;    
    if (w == 0)
    {
        w = 7;    /* 表示周日 */
    }
    else if (w < 0)    /* 如果w是负数，则计算余数方式不同 */
    {
        w = 7 - (-w) % 7;
    }
    else
    {
        w = w % 7;
    }
    return w;
}

static int get_month_last_date( int year, int month)
{
	int date = mdate[month-1];
	if(month == 2 && ((year%400==0)||(year%4==0)&&(year%100!=0)))
	{
		++date;
	}
	return date;
}

static void get_month_all_date(DATESTURCT * calendar, int year, int month)
{
	memset(calendar, 0, DATESTULEN);
	int week = weekday(year, month, 1);
	int length = DATEROW * DATECOL;
	int pyear = year;
	int pmonth = month-1;
	int pdate = 0;
	int nyear =year;
	int nmonth = month+1;
	int ndate = 1;
	int index = 0;

	int ldate = get_month_last_date(year, month);

	if(month==12)
	{
		nyear = year + 1;
		nmonth = 1;
	}

	if(month == 1)
	{
		pyear = year -1;
		pmonth = 12;
	}

	pdate = get_month_last_date(pyear, pmonth);
	for(index = 0; week - 1 > index; index++)
	{
		calendar[index].year = pyear;
		calendar[index].month = pmonth;
		calendar[index].day = pdate - week + index + 2;
	}

	pdate = 1;
	for(index; ldate + week - 1 > index; index++)
	{
		calendar[index].year = year;
		calendar[index].month = month;
		calendar[index].day = pdate ++;
	}

	pdate = 1;
	for(index; (DATEROW * DATECOL) > index; index++)
	{
		calendar[index].year = nyear;
		calendar[index].month = nmonth;
		calendar[index].day = pdate ++;
	}

	return;
}

/****************************************************************************
 *
 *
 *
****************************************************************************/
	class CDateWnd : public CWindowUI
	{
	public:
        CDateWnd(CControlUI * pParent, CControlUI * pDateView);
		~CDateWnd();

		bool SetSelectDate(DATESTURCT &date);
		bool ShowCalendar(int nYear, int nMonth);
	protected:
        virtual void OnNotify(TNOTIFYUI &notify);
        virtual LRESULT OnEvent(TEVENT &event);
	protected:
		CControlUI * m_pParent;
		CControlUI * m_pView;
	};

    CDateWnd::CDateWnd(CControlUI * pParent, CControlUI * pDateView)
        : m_pParent(pParent)
        , m_pView(pDateView)
	{
	}

	CDateWnd::~CDateWnd()
	{
	}

	bool CDateWnd::SetSelectDate(DATESTURCT &date)
	{
		CDateViewUI * pDateView = NULL;
		pDateView = (CDateViewUI*)m_pViewInfo->pRootControl->FindControlByName(CCalendarUI::strDateViewName);

		return pDateView->SetSelect(date);
	}

	bool CDateWnd::ShowCalendar(int nYear, int nMonth)
	{
		CMuiString strTemp;
		CDateViewUI * pDateView = NULL;
		CComboBoxUI * pYearBox = NULL;
		CComboBoxUI * pMonthBox = NULL;

		pYearBox = (CComboBoxUI*)m_pViewInfo->pRootControl->FindControlByName(CCalendarUI::strYearBoxName);
		pMonthBox = (CComboBoxUI*)m_pViewInfo->pRootControl->FindControlByName(CCalendarUI::strMonthBoxName);
		pDateView = (CDateViewUI*)m_pViewInfo->pRootControl->FindControlByName(CCalendarUI::strDateViewName);
		if(!pYearBox || !pMonthBox || !pDateView)
		{
			ASSERT(0 && "CDateWnd找不到必须具备的控件");
			return false;
		}

		strTemp.Format(_T("%04d年"), nYear);
		pYearBox->SetText(strTemp);
		strTemp.Format(_T("%02d月"), nMonth);
		pMonthBox->SetText(strTemp);

		pDateView->ShowCalendar(nYear, nMonth);
		return true;
	}

    void CDateWnd::OnNotify(TNOTIFYUI &notify)
	{
		CMuiString strTemp;
		CDateViewUI * pDateView = NULL;
		CComboBoxUI * pYearBox = NULL;
		CComboBoxUI * pMonthBox = NULL;
		DATESTURCT date;
		SYSTEMTIME time;
		CControlUI * pConntrol = (CControlUI*) notify.pSender;
		switch(notify.dwType)
		{
		case EnumNotifyMsg::ClickItem:
			{
				if(pConntrol->GetName() == CCalendarUI::strBtnGobackName)
				{
					GetLocalTime (&time);
					ShowCalendar(time.wYear, time.wMonth);
				}
			}break;
		case EnumNotifyMsg::SelectItem:
			{
				if(pConntrol->GetName() == CCalendarUI::strDateViewName)
				{
					pDateView = (CDateViewUI *)pConntrol;
					pDateView->GetSelect(date);
					strTemp.Format(_T("%04d-%02d-%02d"), date.year, date.month, date.day);
					m_pParent->SetText(strTemp);
                    ::ShowWindow(m_hWnd, FALSE);
                    ::PostMessage(m_hWnd, WM_BREAKLOOP, TRUE, NULL);
				}
				else if(pConntrol->GetName() == CCalendarUI::strYearBoxName)
				{
					pYearBox = static_cast<CComboBoxUI *>(pConntrol);

					pDateView = static_cast<CDateViewUI *>(m_pViewInfo->pRootControl->FindControlByName(CCalendarUI::strDateViewName));
					pDateView->ChangeCalendarYear(_tcstol(pYearBox->GetText(), NULL, 10));
					
				}
				else if(pConntrol->GetName() == CCalendarUI::strMonthBoxName)
				{
					pMonthBox = static_cast<CComboBoxUI *>(pConntrol);
					pDateView = static_cast<CDateViewUI *>(m_pViewInfo->pRootControl->FindControlByName(CCalendarUI::strDateViewName));
					pDateView->ChangeCalendarMonth(_tcstol(pMonthBox->GetText(), NULL, 10));
				}
			}break;
		}

		//return __super::OnNotify(notify);
	};

    LRESULT CDateWnd::OnEvent(TEVENT &event)
	{
		switch (event.dwType)
		{
		case EnumEventType::WindowInit:
			{
				CVerticalLayoutUI * pRootLayout = new CVerticalLayoutUI();

				pRootLayout->Add(m_pView);

				this->AttachFrameView(pRootLayout);
			}break;
		case EnumEventType::OnTimer:
			{

			}break;
		case EnumEventType::WindowShow:
			{
				
			}break;
		case EnumEventType::KillFocued:
			{
            ::PostMessage(GetWindowOwner(m_hWnd), WM_BREAKLOOP, TRUE,
                (LPARAM)static_cast<CControlUI*>(m_pParent));
			}break;
		case EnumEventType::WindowDestroy:
			{
				((CVerticalLayoutUI*)m_pViewInfo->pRootControl)->Remove(m_pView);
			}break;
		default:
			break;
		}

		return 0;
		//return __super::OnEvent(event);
	}


/****************************************************************************
 * CDateViewUI 类
 * 日期的界面
 *
****************************************************************************/

	CDateViewUI::CDateViewUI()
		: m_refLineColor(ARGB(0,180,180,180))
		, m_refWorkTextColor(ARGB(255,0,0,0))
		, m_refRestTextColor(ARGB(255,232,45,45))
		, m_refMajorBkColor(ARGB(255, 255,187,0))
		, m_refMinorBkColor(NULL)
		, m_refMinorTextColor(ARGB(255,120,120,120))
		, m_refSelectTextColor(ARGB(255,255,255,255))
		, m_refSelectBkColor(ARGB(255, 117, 142, 149))
		, m_refHotBkColor(ARGB(255, 109,182,255))
		, m_nYear(0)
		, m_nMonth(0)
		, m_nWeekHeight(40)
		, m_nHotIndex(-1)
	{
		m_SelectDate.year = m_SelectDate.month = m_SelectDate.day = 0;
	}

	CDateViewUI::~CDateViewUI()
	{
	}

	void CDateViewUI::SetLineColor(ARGBREF refColor)
	{
		m_refLineColor = refColor;
	}

	ARGBREF CDateViewUI::GetLineColor() const
	{
		return m_refLineColor;
	}

	void CDateViewUI::SetRestTextColor(ARGBREF refColor)
	{
		m_refRestTextColor = refColor;
	}

	ARGBREF CDateViewUI::GetRestTextColor() const
	{
		return m_refRestTextColor;
	}

	void CDateViewUI::SetWorkTextColor(ARGBREF refColor)
	{
		m_refWorkTextColor = refColor;
	}

	ARGBREF CDateViewUI::GetWorkTextColor() const
	{
		return m_refWorkTextColor;
	}

	void CDateViewUI::SetSelectTextColor(ARGBREF refColor)
	{
		m_refSelectTextColor = refColor;
	}

	ARGBREF CDateViewUI::GetSelectTextColor() const
	{
		return m_refSelectTextColor;
	}

	void CDateViewUI::SetSelectBkColor(ARGBREF refColor)
	{
		m_refSelectBkColor = refColor;
	}

	ARGBREF CDateViewUI::GetSelectBkColor() const
	{
		return m_refSelectBkColor;
	}

	void CDateViewUI::SetMajorBkColor(ARGBREF refColor)
	{
		m_refMajorBkColor = refColor;
	}

	ARGBREF CDateViewUI::GetMajorBkColor() const
	{
		return m_refMajorBkColor;
	}

	void CDateViewUI::SetMinorBkColor(ARGBREF refColor)
	{
		m_refMinorBkColor = refColor;
	}

	ARGBREF CDateViewUI::GetMinorBkColor() const
	{
		return m_refMinorBkColor;
	}

	void CDateViewUI::SetMinorTextColor(ARGBREF refColor)
	{
		m_refMinorTextColor = refColor;
	}

	ARGBREF CDateViewUI::GetMinorTextColor() const
	{
		return m_refMinorTextColor;
	}

	void CDateViewUI::SetHotBkColor(ARGBREF refColor)
	{
		m_refHotBkColor = refColor;
	}

	ARGBREF CDateViewUI::GetHotBkColor() const
	{
		return m_refHotBkColor;
	}

	void CDateViewUI::SetWeekHeight(int nHeight)
	{
		m_nWeekHeight = nHeight;
	}

	int CDateViewUI::GetWeekHeight()
	{
		return m_nWeekHeight;
	}

	void CDateViewUI::OnAttach(HWND hNewWnd)
	{
		SYSTEMTIME time;
		GetLocalTime (&time);
		ShowCalendar(time.wYear, time.wMonth);
		return __super::OnAttach(hNewWnd);
	}

	int CDateViewUI::GetPointItem(POINT point)
	{
		RECT rcItem;
		SIZE size = GetValidSize();
		int row = 0, col = 0;
		rcItem = m_rcRawItem;
		rcItem.top += m_nWeekHeight;

		if(FALSE == PointInRect(point, rcItem)) return -1;

		int nColOffset = (rcItem.right - rcItem.left) / DATECOL;
		int nRowOffset = (rcItem.bottom - rcItem.top) / DATEROW;
		point.y -= m_nWeekHeight;

		col = point.x / nColOffset;
		row = point.y / nRowOffset;

		if(col >= DATECOL || row >= DATEROW) return -1;

		return (row * DATECOL) + col;
	}

	void CDateViewUI::PaintText(const RECT& rcItem, const RECT& rcPaint)
	{
		TCHAR temp[10];
		RECT rect;
		POINT ptBegin, ptEnd;
		ARGBREF refTextColor = NULL;
		ARGBREF refBkColor = NULL;
		HFONT hFont = (HFONT)m_pShareInfo->FontArray->Select(m_nFontId);
		CRenderEngine * pEngine = m_pShareInfo->pRenderEngine;

		//开始绘制
		
		int index = 0;
		int nColOffset = (rcItem.right - rcItem.left) / DATECOL;
		int nRowOffset = (rcItem.bottom - rcItem.top - m_nWeekHeight) / DATEROW;

		//星期标签
		rect.left = rcItem.left;
		rect.top = rcItem.top;
		rect.right = rect.left + nColOffset;
		rect.bottom = rect.top + m_nWeekHeight;
		for(int i = 0; DATECOL > i; i++)
		{
			refTextColor = m_refTextColor;
			if(i >= 5)//周末
			{
				if(m_refRestTextColor)
				{
					refTextColor = m_refRestTextColor;
				}
			}
			else//本月的普通日子
			{
				if(m_refWorkTextColor)
				{
					refTextColor = m_refWorkTextColor;
				}
			}

			pEngine->OnDrawText(rect, mweek[i], refTextColor, hFont, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			OffsetRect(&rect, nColOffset , 0);
		}

		//日期单元格
		rect.left = rcItem.left;
		rect.top = rcItem.top + m_nWeekHeight;
		rect.right = rect.left + nColOffset;
		rect.bottom = rect.top + nRowOffset;
		if(m_refLineColor) rect.bottom -= 1;
		for(int i = 0; DATEROW > i; i++)
		{
			for(int j = 0; DATECOL > j; j++)
			{
				index = (i * DATECOL) + j;
				wsprintf(temp, _T("%d"), m_DateCalendar[index].day);

				refTextColor = m_refTextColor;
				refBkColor = NULL;

				if(m_DateCalendar[index].month == m_nMonth)
				{
					if(j >= 5)//周末
					{
						if(m_refRestTextColor)
						{
							refTextColor = m_refRestTextColor;
						}
					}
					else//本月的普通日子
					{
						if(m_refWorkTextColor)
						{
							refTextColor = m_refWorkTextColor;
						}
					}

					if(m_refMajorBkColor)
					{
						refBkColor = m_refMajorBkColor;
					}
				}
				else
				{
					if(m_refMinorTextColor)
					{
						refTextColor = m_refMinorTextColor;
					}
					if(m_refMinorBkColor)
					{
						refBkColor = m_refMinorBkColor;
					}
				}

				if(m_nHotIndex == index && m_refHotBkColor)
				{
					refBkColor = m_refHotBkColor;
				}

				if(m_SelectDate.year == m_DateCalendar[index].year &&
					m_SelectDate.month == m_DateCalendar[index].month &&
					 m_SelectDate.day == m_DateCalendar[index].day)
				{
					if(m_refSelectTextColor)
					{
						refTextColor = m_refSelectTextColor;
					}

					if(m_refSelectBkColor)
					{
						refBkColor = m_refSelectBkColor;
					}
				}

				if(refBkColor) pEngine->OnDrawColor(rect, refBkColor);
				pEngine->OnDrawText(rect, temp, refTextColor, hFont, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				OffsetRect(&rect, nColOffset , 0);
			}/*日期单元格 *列*/

			
			if(m_refLineColor)
			{
				ptBegin.x = rcItem.left;
				ptEnd.x = rcItem.right;
				ptBegin.y = ptEnd.y = rect.top;
				pEngine->OnDrawLine(ptBegin, ptEnd, 1, m_refLineColor);
			}

			OffsetRect(&rect, rcItem.left - rect.left , nRowOffset);
			
		}/*日期单元格 *行*/

		//结束绘制
	}

	void CDateViewUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
	}
		
	LRESULT CDateViewUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT point;
		int index = -1;
		switch (message)
		{
		case WM_LBUTTONDOWN:
			{
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);
				index = GetPointItem(point);

				if(-1 != index)
				{
					m_SelectDate = m_DateCalendar[index];
					this->Invalidate();
				}
			}break;
		case WM_LBUTTONDBLCLK:
			{
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);
				index = GetPointItem(point);

				if(-1 != index && hWnd)
				{
					m_SelectDate = m_DateCalendar[index];
					SendNotify(!hWnd, EnumNotifyMsg::SelectItem, index, (LPARAM)this);
				}
			}break;
		case WM_MOUSEMOVE:
			{
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);
				index = GetPointItem(point);

				if(index != m_nHotIndex)
				{
					m_nHotIndex = index;
					this->Invalidate();
				}
			}break;
		case WM_MOUSELEAVE:
			{
				index = -1;
				if(index != m_nHotIndex)
				{
					m_nHotIndex = index;
					this->Invalidate();
				}
			}break;
		default:
			break;
		}
		return __super::WndProc(hWnd, message, wParam, lParam);
	}

	bool CDateViewUI::GetSelect(DATESTURCT &date)
	{
		date = m_SelectDate;
		return true;
	}

	bool CDateViewUI::SetSelect(DATESTURCT &date)
	{
		m_SelectDate = date;
		Invalidate();
		return true;
	}

	bool CDateViewUI::ShowCalendar(int nYear, int nMonth)
	{
		m_nYear = nYear;
		m_nMonth = nMonth;
		get_month_all_date(m_DateCalendar, nYear, nMonth);
		Invalidate();
		return true;
	}

	bool CDateViewUI::ChangeCalendarYear(int nYear)
	{
		return ShowCalendar(nYear, m_nMonth);
	}

	bool CDateViewUI::ChangeCalendarMonth(int nMonth)
	{
		return ShowCalendar(m_nYear, nMonth);
	}


/****************************************************************************
 *
 *
 *
****************************************************************************/

	CCalendarUI::CCalendarUI()
		: m_pDialog(NULL)
		, m_pDialogView(NULL)
		, m_pDateView(NULL)
		, m_pBtnGoback(NULL)
		, m_pYearBox(NULL)
		, m_pMonthBox(NULL)
	{
		//m_hBorderPen = CreatePen(PS_SOLID,1 ,RGB(0,0,0));
		m_date.year = m_date.month = m_date.day = 0;

		m_pDialogView = CreateDialogView();
	}

	CCalendarUI::~CCalendarUI()
	{
		if(m_pDialog) delete m_pDialog;

		if(m_pDialogView) delete m_pDialogView;

	}

	CMuiString CCalendarUI::g_strClassName(_T("CalendarUI"));

	CMuiString CCalendarUI::strDateViewName(_T("sDateView"));
	CMuiString CCalendarUI::strBtnGobackName(_T("sBtnGoback"));
	CMuiString CCalendarUI::strMonthBoxName(_T("sCbxMonth"));
	CMuiString CCalendarUI::strYearBoxName(_T("sCbxYear"));

	CMuiString CCalendarUI::GetClassName() const
	{
		return CCalendarUI::g_strClassName;
	}

	void CCalendarUI::OnAttach(HWND hNewWnd)
	{
		SYSTEMTIME time;
		RECT rect;
		TCHAR strText[32];
		CLableUI * pLable = NULL;
		RECT rcTextPadding = {2,0,2,0};
		int nFondId = -1;
		rect.left = 0;
		rect.top = 0;
		rect.right = 390;
		rect.bottom = 400;

		if(0 == m_date.day || 0 == m_date.month || 0 == m_date.year)
		{
			GetLocalTime (&time);
			m_date.day = time.wDay;
			m_date.month = time.wMonth;
			m_date.year = time.wYear;
			m_strText.Format(_T("%04d-%02d-%02d"), m_date.year, m_date.month, m_date.day);
		}

		if(12 != m_pMonthBox->GetCount())
		{
			m_pMonthBox->RemoveAll();
			nFondId = m_pMonthBox->GetTextFont();
			rcTextPadding = m_pMonthBox->GetTextPadding();
			for(int i =0; 12 > i; i++)
			{
				pLable = new CLableUI();
				pLable->SetHotColor(ARGB(255,115,199,235));
				pLable->SetSelectColor(ARGB(255,115,199,235));
				pLable->SetTextPadding(rcTextPadding);
				pLable->SetHeight(28);
				pLable->SetTextFont(nFondId);
				pLable->SetTag(i+1);
				pLable->SetAttribute(_T("align"), _T("vcenter;left;"));

				wsprintf(strText,_T("%02d月"), i+1);
				pLable->SetText(strText);
				m_pMonthBox->Add(pLable);
			}
		}

		m_pMonthBox->SetSelectItem(m_date.month - 1);

		if(0 == m_pYearBox->GetCount())
		{
			m_pYearBox->RemoveAll();
			m_pYearBox->SetItemBoxHeight(28 * 12);
			nFondId = m_pYearBox->GetTextFont();
			rcTextPadding = m_pYearBox->GetTextPadding();
			for(int i =0; 200 > i; i++)
			{
				pLable = new CLableUI();
				pLable->SetHotColor(ARGB(255,115,199,235));
				pLable->SetSelectColor(ARGB(255,115,199,235));
				pLable->SetTextPadding(rcTextPadding);
				pLable->SetHeight(28);
				pLable->SetTextFont(nFondId);
				pLable->SetTag(m_date.year + i - 80);
				pLable->SetAttribute(_T("align"), _T("vcenter;left;"));

				wsprintf(strText,_T("%04d年"), m_date.year + i - 80);
				pLable->SetText(strText);
				m_pYearBox->Add(pLable);

				if(m_date.year == m_date.year + i - 80)
				{
					m_pYearBox->SetSelectItem(i);
				}
			}
		}

	}

	void CCalendarUI::OnDetach(HWND hOldWnd)
	{

	}

	bool CCalendarUI::GetDate(DATESTURCT &date)
	{
		return m_pDateView->GetSelect(date);
	}

	bool CCalendarUI::SetDate(DATESTURCT &date)
	{
		return m_pDateView->SetSelect(date);
	}

	CControlUI * CCalendarUI::CreateDialogView()
	{
		DWORD dwStyle = NULL;
		CControlUI * pTemp = NULL;
		CVerticalLayoutUI * pRootLayout = new CVerticalLayoutUI();
		pRootLayout->SetSize(CMuiSize(390,400));
		pRootLayout->SetBorderPen(1, ARGB(255,36,36,36));
		pRootLayout->SetBkColor(ARGB(255,255,255,255));
		pRootLayout->SetInset(CMuiRect(1,1,1,1));
		//头部
		CHorizontalLayoutUI * m_pHeader = new CHorizontalLayoutUI();

		m_pHeader->SetBkColor(ARGB(255,57,105,153));
		m_pHeader->SetHeight(50);

		m_pYearBox = new CComboBoxUI();
		m_pMonthBox = new CComboBoxUI();
		m_pBtnGoback = new CButtonUI();
		pTemp = new CControlUI();

		m_pYearBox->SetName(CCalendarUI::strYearBoxName);
		m_pYearBox->SetMargin(CMuiRect(20,0,0,0));
		m_pYearBox->SetSize(CMuiSize(100,28));
		m_pYearBox->SetTextPadding(CMuiRect(3,2,22,2));
		m_pYearBox->SetBkColor(ARGB(255,255,255,255));
		m_pYearBox->SetAttribute(_T("center"), _T("true"));

		m_pMonthBox->SetName(CCalendarUI::strMonthBoxName);
		m_pMonthBox->SetMargin(CMuiRect(20,0,0,0));
		m_pMonthBox->SetSize(CMuiSize(100,28));
		m_pMonthBox->SetTextPadding(CMuiRect(3,2,22,2));
		m_pMonthBox->SetBkColor(ARGB(255,255,255,255));
		m_pMonthBox->SetAttribute(_T("center"), _T("true"));

		m_pBtnGoback->SetName(CCalendarUI::strBtnGobackName);
		m_pBtnGoback->SetMargin(CMuiRect(0,0,20,0));
		m_pBtnGoback->SetBorderRound(CMuiSize(2,2));
		m_pBtnGoback->SetSize(CMuiSize(90,28));
		m_pBtnGoback->SetText(_T("返回今天"));
		m_pBtnGoback->SetNormalColor(ARGB(255,235,235,235));
		m_pBtnGoback->SetHotColor(ARGB(255,220,220,220));
		m_pBtnGoback->SetPushedColor(ARGB(255,220,220,220));

		m_pHeader->Add(m_pYearBox);
		m_pHeader->Add(m_pMonthBox);
		m_pHeader->Add(pTemp);
		m_pHeader->Add(m_pBtnGoback);


		CVerticalLayoutUI * pBody = new CVerticalLayoutUI();
		m_pDateView = new CDateViewUI();
		pTemp = new CControlUI();

		m_pDateView->SetName(CCalendarUI::strDateViewName);
		m_pDateView->SetWeekHeight(36);
		m_pDateView->SetWidth(350);
		m_pDateView->SetHeight(340);
		

		pBody->Add(m_pDateView);
		pBody->Add(pTemp);

		pRootLayout->Add(m_pHeader);
		pRootLayout->Add(pBody);
		return pRootLayout;
	}

	void CCalendarUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		TCHAR strTempItem[64];
		
		if(0 == _tcsnicmp(strItem, _T("btn"), 3))
		{
			_tcscpy(strTempItem, &strItem[3]);
			m_pBtnGoback->SetAttribute(strTempItem, strValue);
		}
		else if(0 == _tcsnicmp(strItem, _T("dtv"), 3))
		{
			_tcscpy(strTempItem, &strItem[3]);
			m_pDateView->SetAttribute(strTempItem, strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("vScroll"))) 
		{
			m_pYearBox->SetAttribute(strItem, strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("cbxvScroll"))) 
		{
			_tcscpy(strTempItem, &strItem[3]);
			m_pYearBox->SetAttribute(strTempItem, strValue);
		}
		else if(0 == _tcsnicmp(strItem, _T("cbx"), 3))
		{
			_tcscpy(strTempItem, &strItem[3]);
			m_pMonthBox->SetAttribute(strTempItem, strValue);
			m_pYearBox->SetAttribute(strTempItem, strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("HeaderBkColor")))
		{
			SetHeaderBkColor(CMuiColor(strValue));
		}
		else
		{
			__super::SetAttribute(strItem ,strValue);
		}
	}

    LRESULT CCalendarUI::Popup(LPARAM lParam)
    {
        RECT rcPos = { 0 };
        POINT Point = { 0 };
        SIZE DialogSize = {390, 400};
        CDateWnd * pDialog = NULL;
        LRESULT lResult = 0;

        if (m_pDialog)
        {
            ASSERT(0 && "CCalendarUI::Popup 不合适的调用时机, m_pDialog应该为空");
            return FALSE;
        }

        if (FALSE == GetItemFixedRect(rcPos)) return false;

        rcPos.left += 1;
        rcPos.right -= 1;
        ::MapWindowPoints(m_pShareInfo->hWnd, NULL, (LPPOINT)&rcPos, 2);
        
        Point = CalcPopupPoint(&rcPos, &DialogSize, CPOT_BOTTOM);
        rcPos.left = Point.x;
        rcPos.top = Point.y;
        rcPos.right = rcPos.left + DialogSize.cx;
        rcPos.bottom = rcPos.top + DialogSize.cy;

        pDialog = new CDateWnd(static_cast<CControlUI*>(this),
            static_cast<CControlUI*>(m_pDialogView));
        pDialog->CloneResource(m_pShareInfo);//使用同步资源，同步父窗口资源
        pDialog->Create((HINSTANCE)GetWindowLong(GETHWND(this), GWL_HINSTANCE), GETHWND(this),
            WS_BORDER | WS_POPUP | WS_CLIPSIBLINGS,
            _T("{E0BAF9EC-36B5-41EE-A4B3-B2DF52844943}"), _T("CDateWnd"));

        m_pDialog = pDialog;
        
        lResult = pDialog->Popup(rcPos);

        m_pDialog = NULL;

        pDialog->Destroy();
        delete pDialog;
        return lResult;
    }

	LRESULT CCalendarUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT pt;
		RECT rcButton;
		RECT rcDisplay = {0,0,0,0};
		bool bRet = false;
		switch(message)
		{
		case WM_GETSCROLL:
			{
				if(GSL_VERTICAL == wParam)
				{
					return (LRESULT)m_pYearBox->GetVerticalScrollBar();
				}
				return NULL;
			}break;
		case WM_LBUTTONDOWN:
			{
				pt.x = (short)GET_X_LPARAM(lParam);
				pt.y = (short)GET_Y_LPARAM(lParam);

				rcButton = m_rcRawItem;
				rcButton.left = rcButton.right - m_rcTextPadding.right;

                if (TRUE == PointInRect(pt, rcButton))
                {
                    if (m_pDialog)
                    {
                        m_pDialog->PostMessage(WM_BREAKLOOP, TRUE, NULL);
                    }
                    else
                    {
                        ::PostMessage(GETHWND(this), WM_POPUPDIALOG, (WPARAM)static_cast<IDialogPopup*>(this), NULL);
                    }
                }
			}break;
		case WM_KEYDOWN:
			{
            if (VK_RETURN == wParam)
            {
                if (m_pDialog)
                {
                    m_pDialog->PostMessage(WM_BREAKLOOP, TRUE, NULL);
                }
                else
                {
                    ::PostMessage(GETHWND(this), WM_POPUPDIALOG, (WPARAM)static_cast<IDialogPopup*>(this), NULL);
                }
                return false;
            }
			}break;
		case WM_MOUSEMOVE:
			{
				pt.x = (short)GET_X_LPARAM(lParam);
				pt.y = (short)GET_Y_LPARAM(lParam);
				rcButton = m_rcRawItem;
				rcButton.left = rcButton.right - m_rcTextPadding.right;

				if(TRUE == PointInRect(pt, rcButton))
				{
					m_bMouseInButton = true;
				}
				else
				{
					m_bMouseInButton = false;
				}
			}break;
		case WM_MOUSELEAVE:
			{
				m_bMouseInButton = false;
			}break;
		case WM_SETCURSOR:
			{
				if(true == m_bMouseInButton)
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
					return false;
				}
				else
				{
					//让消息流入__super中
					break;
				}
			}break;
		case WM_SETFOCUS:
			{
                TRACE(_T("CCalendarUI WM_SETFOCUS"));
				//m_pBoxWnd->SendMessage(WM_PARENTNOTIFY, TRUE, NULL);
			}break;
		case WM_KILLFOCUS:
			{
                TRACE(_T("CCalendarUI WM_KILLFOCUS"));
                if (m_pDialog)
                {
                    ::PostMessage(::GetWindowOwner(*m_pDialog), WM_BREAKLOOP,
                        TRUE, (LPARAM)static_cast<CControlUI*>(this));
                }
			}break;
		default:
			break;
		}

		return __super::WndProc(hWnd, message, wParam, lParam);
	}

//日历控件的属性非常多，下面是设置日历控件的代码

	//SetDateViewAttribute
	void CCalendarUI::SetDateViewLineColor(ARGBREF refColor)
	{
		m_pDateView->SetLineColor(refColor);
	}

	ARGBREF CCalendarUI::GetDateViewLineColor() const
	{
		return m_pDateView->GetLineColor();
	}

	void CCalendarUI::SetDateViewRestTextColor(ARGBREF refColor)
	{
		m_pDateView->SetRestTextColor(refColor);
	}

	ARGBREF CCalendarUI::GetDateViewRestTextColor() const
	{
		return m_pDateView->GetRestTextColor();
	}

	void CCalendarUI::SetDateViewWorkTextColor(ARGBREF refColor)
	{
		m_pDateView->SetWorkTextColor(refColor);
	}

	ARGBREF CCalendarUI::GetDateViewWorkTextColor() const
	{
		return m_pDateView->GetWorkTextColor();
	}

	void CCalendarUI::SetDateViewSelectTextColor(ARGBREF refColor)
	{
		m_pDateView->SetSelectTextColor(refColor);
	}

	ARGBREF CCalendarUI::GetDateViewSelectTextColor() const
	{
		return m_pDateView->GetSelectTextColor();
	}

	void CCalendarUI::SetDateViewSelectBkColor(ARGBREF refColor)
	{
		m_pDateView->SetSelectBkColor(refColor);
	}

	ARGBREF CCalendarUI::GetDateViewSelectBkColor() const
	{
		return m_pDateView->GetSelectBkColor();
	}

	void CCalendarUI::SetDateViewMajorBkColor(ARGBREF refColor)
	{
		m_pDateView->SetMajorBkColor(refColor);
	}

	ARGBREF CCalendarUI::GetDateViewMajorBkColor() const
	{
		return m_pDateView->GetMajorBkColor();
	}

	void CCalendarUI::SetDateViewMinorBkColor(ARGBREF refColor)
	{
		m_pDateView->SetMinorBkColor(refColor);
	}

	ARGBREF CCalendarUI::GetDateViewMinorBkColor() const
	{
		return m_pDateView->GetMinorBkColor();
	}

	void CCalendarUI::SetDateViewMinorTextColor(ARGBREF refColor)
	{
		m_pDateView->SetMinorTextColor(refColor);
	}

	ARGBREF CCalendarUI::GetDateViewMinorTextColor() const
	{
		return m_pDateView->GetMinorTextColor();
	}

	void CCalendarUI::SetDateViewHotBkColor(ARGBREF refColor)
	{
		m_pDateView->SetHotBkColor(refColor);
	}

	ARGBREF CCalendarUI::GetDateViewHotBkColor() const
	{
		return m_pDateView->GetHotBkColor();
	}

	void CCalendarUI::SetDateViewWeekHeight(int nHeight)
	{
		m_pDateView->SetWeekHeight(nHeight);
	}

	int CCalendarUI::GetDateViewWeekHeight()
	{
		return m_pDateView->GetWeekHeight();
	}

	void CCalendarUI::SetDateViewTextFont(int nFontId)
	{
		m_pDateView->SetTextFont(nFontId);
	}

	int CCalendarUI::GetDateViewTextFont()
	{
		return m_pDateView->GetTextFont();
	}

	//Set Goback Button Attribute
	void CCalendarUI::SetButtonNormalColor(ARGBREF refColor)
	{
		m_pBtnGoback->SetNormalColor(refColor);
	}

	void CCalendarUI::SetButtonHotColor(ARGBREF refColor)
	{
		m_pBtnGoback->SetHotColor(refColor);
	}

	void CCalendarUI::SetButtonPushedColor(ARGBREF refColor)
	{
		m_pBtnGoback->SetPushedColor(refColor);
	}

	ARGBREF CCalendarUI::GetButtonNormalColor() const
	{
		return m_pBtnGoback->GetNormalColor();
	}

	ARGBREF CCalendarUI::GetButtonHotColor() const
	{
		return m_pBtnGoback->GetHotColor();
	}

	ARGBREF CCalendarUI::GetButtonPushedColor() const
	{
		return m_pBtnGoback->GetPushedColor();
	}

	void CCalendarUI::SetButtonNormalImage(LPCTSTR strImage)
	{
		m_pBtnGoback->SetNormalImage(strImage);
	}

	LPCTSTR CCalendarUI::GetButtonNormalImage() const
	{
		return m_pBtnGoback->GetNormalImage();
	}

	void CCalendarUI::SetButtonHotImage(LPCTSTR strImage)
	{
		m_pBtnGoback->SetHotImage(strImage);
	}

	LPCTSTR CCalendarUI::GetButtonHotImage() const
	{
		return m_pBtnGoback->GetHotImage();
	}

	void CCalendarUI::SetButtonPushedImage(LPCTSTR strImage)
	{
		m_pBtnGoback->SetPushedImage(strImage);
	}

	LPCTSTR CCalendarUI::GetButtonPushedImage() const
	{
		return m_pBtnGoback->GetPushedImage();
	}

	void CCalendarUI::SetButtonTextFont(int nFontId)
	{
		m_pBtnGoback->SetTextFont(nFontId);
	}

	int CCalendarUI::GetButtonTextFont()
	{
		return m_pBtnGoback->GetTextFont();
	}

	//Set Year and Month ComboBox Attribute
	void CCalendarUI::SetComboNormalImage(LPCTSTR strImage)
	{
		m_pYearBox->SetNormalImage(strImage);
		m_pMonthBox->SetNormalImage(strImage);
	}

	LPCTSTR CCalendarUI::GetComboNormalImage() const
	{
		return m_pYearBox->GetNormalImage();
	}

	void CCalendarUI::SetComboHotImage(LPCTSTR strImage)
	{
		m_pYearBox->SetHotImage(strImage);
		m_pMonthBox->SetHotImage(strImage);
	}

	LPCTSTR CCalendarUI::GetComboHotImage() const
	{
		return m_pYearBox->GetHotImage();
	}

	void CCalendarUI::SetComboPushedImage(LPCTSTR strImage)
	{
		m_pYearBox->SetPushedImage(strImage);
		m_pMonthBox->SetPushedImage(strImage);
	}

	LPCTSTR CCalendarUI::GetComboPushedImage() const
	{
		return m_pYearBox->GetPushedImage();
	}

		
	void CCalendarUI::SetComboTextFont(int nFontId)
	{
		m_pYearBox->SetTextFont(nFontId);
		m_pMonthBox->SetTextFont(nFontId);
	}

	int CCalendarUI::GetComboTextFont()
	{
		return m_pYearBox->GetTextFont();
	}

	void CCalendarUI::SetTextPadding(const RECT &rect)
	{
		m_pYearBox->SetTextPadding(rect);
		m_pMonthBox->SetTextPadding(rect);
	}

	const RECT& CCalendarUI::GetTextPadding() const
	{
		return m_pYearBox->GetTextPadding();
	}

	void CCalendarUI::SetHeaderBkColor(ARGBREF refColor)
	{
		m_pHeader->SetBkColor(refColor);
	}

	ARGBREF CCalendarUI::GetHeaderBkColor() const
	{
		return m_pHeader->GetBkColor();
	}
}