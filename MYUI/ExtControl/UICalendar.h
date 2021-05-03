
#ifndef _MYUI_CALENDAR_CONTROL_H_
#define _MYUI_CALENDAR_CONTROL_H_

#include "../Control/UILable.h"
#include "../Control/UIComboBox.h"

namespace MYUI
{


    typedef struct __MUIDATE{
	    int year;
	    int month;
	    int day;
    }MUIDATE, *LPMUIDATE;


    typedef struct __MUIDATESTRING{
	    TCHAR year[4];
	    TCHAR tch1[1];
	    TCHAR month[2];
	    TCHAR tch2[1];
	    TCHAR day[2];
	    TCHAR end[1];
    }MUIDATESTRING, *LPMUIDATESTRING;

#define DATEROW   6  //行
#define DATECOL   7  //列
#define DATESTULEN   (sizeof(MUIDATE) * (DATEROW) * (DATECOL))

	enum
	{
		DateRow = 6,
		DateCol = 7,
		DateStructSize = (sizeof(MUIDATE) * (DateRow) * (DateCol))
	};

	class CDateWnd;

	class MYUI_API CDateViewUI : public CControlUI
	{
	public:
		CDateViewUI();
		~CDateViewUI();
		virtual void PaintText(const RECT& rcUpdate) override;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		//绘制
		
		bool GetSelect(MUIDATE &date);
		bool SetSelect(MUIDATE &date);

		bool ShowCalendar(int nYear, int nMonth);
		bool ChangeCalendarYear(int nYear);
		bool ChangeCalendarMonth(int nMonth);
	public:
		void SetLineColor(ARGBREF refColor);
		ARGBREF GetLineColor() const;
		void SetRestTextColor(ARGBREF refColor);
		ARGBREF GetRestTextColor() const;
		void SetWorkTextColor(ARGBREF refColor);
		ARGBREF GetWorkTextColor() const;
		void SetSelectTextColor(ARGBREF refColor);
		ARGBREF GetSelectTextColor() const;
		void SetSelectBkColor(ARGBREF refColor);
		ARGBREF GetSelectBkColor() const;
		void SetMajorBkColor(ARGBREF refColor);
		ARGBREF GetMajorBkColor() const;
		void SetMinorBkColor(ARGBREF refColor);
		ARGBREF GetMinorBkColor() const;
		void SetMinorTextColor(ARGBREF refColor);
		ARGBREF GetMinorTextColor() const;
		void SetHotBkColor(ARGBREF refColor);
		ARGBREF GetHotBkColor() const;

		void SetWeekHeight(int nHeight);
		int GetWeekHeight();
	protected:
		//void DrawItem(HDC hdc, RECT * rect , DATESTURCT * date, int month, DWORD state);
		//void DrawLine(HDC hdc, int x, int y, int xend);
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
		virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数

		int GetPointItem(POINT point);
	protected:
		int m_nHotIndex;
		int m_nWeekHeight;
		MUIDATE m_SelectDate;
		MUIDATE m_DateCalendar[DATEROW * DATECOL];
		ARGBREF m_refLineColor;
		ARGBREF m_refRestTextColor;
		ARGBREF m_refWorkTextColor;
		
		//这里的bkcolor，是某一项的bk，不是整个控件的BK
		ARGBREF m_refSelectTextColor;
		ARGBREF m_refSelectBkColor;
		ARGBREF m_refMajorBkColor;
		ARGBREF m_refMinorBkColor;
		ARGBREF m_refMinorTextColor;//不是本月的日期颜色
		ARGBREF m_refHotBkColor;

		int m_nMonth;
		int m_nYear;
	};

	
    class MYUI_API CCalendarUI : public CLableUI, protected IDialogPopup
	{
	public:
		CCalendarUI();
		virtual ~CCalendarUI();
		static CMuiString g_strClassName;

		static CMuiString strDateViewName;
		static CMuiString strBtnGobackName;
		static CMuiString strMonthBoxName;
		static CMuiString strYearBoxName;

		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		//绘制
		
		bool GetDate(MUIDATE &date);
		bool SetDate(MUIDATE &date);
	public:
		//SetDateViewAttribute
		void SetDateViewLineColor(ARGBREF refColor);
		ARGBREF GetDateViewLineColor() const;
		void SetDateViewRestTextColor(ARGBREF refColor);
		ARGBREF GetDateViewRestTextColor() const;
		void SetDateViewWorkTextColor(ARGBREF refColor);
		ARGBREF GetDateViewWorkTextColor() const;
		void SetDateViewSelectTextColor(ARGBREF refColor);
		ARGBREF GetDateViewSelectTextColor() const;
		void SetDateViewSelectBkColor(ARGBREF refColor);
		ARGBREF GetDateViewSelectBkColor() const;
		void SetDateViewMajorBkColor(ARGBREF refColor);
		ARGBREF GetDateViewMajorBkColor() const;
		void SetDateViewMinorBkColor(ARGBREF refColor);
		ARGBREF GetDateViewMinorBkColor() const;
		void SetDateViewMinorTextColor(ARGBREF refColor);
		ARGBREF GetDateViewMinorTextColor() const;
		void SetDateViewHotBkColor(ARGBREF refColor);
		ARGBREF GetDateViewHotBkColor() const;
		void SetDateViewWeekHeight(int nHeight);
		int GetDateViewWeekHeight();
		void SetDateViewTextFont(int nFontId);
		int GetDateViewTextFont();

		//Set Goback Button Attribute
		void SetButtonNormalColor(ARGBREF refColor);
		void SetButtonHotColor(ARGBREF refColor);
		void SetButtonPushedColor(ARGBREF refColor);

		ARGBREF GetButtonNormalColor() const;
		ARGBREF GetButtonHotColor() const;
		ARGBREF GetButtonPushedColor() const;

		void SetButtonNormalImage(LPCTSTR strImage);
		void SetButtonHotImage(LPCTSTR strImage);
		void SetButtonPushedImage(LPCTSTR strImage);

		LPCTSTR GetButtonHotImage() const;
		LPCTSTR GetButtonNormalImage() const;
		LPCTSTR GetButtonPushedImage() const;

		void SetButtonTextFont(int nFontId);
		int GetButtonTextFont();

		//Set Year and Month ComboBox Attribute

		void SetComboNormalImage(LPCTSTR strImage);
		LPCTSTR GetComboNormalImage() const;
		void SetComboHotImage(LPCTSTR strImage);
		LPCTSTR GetComboHotImage() const;
		void SetComboPushedImage(LPCTSTR strImage);
		LPCTSTR GetComboPushedImage() const;
		
		void SetComboTextFont(int nFontId);
		int GetComboTextFont();
		void SetTextPadding(const RECT &rect);
		const RECT& GetTextPadding() const;

		//Set Header Attribute
		void SetHeaderBkColor(ARGBREF refColor);
		ARGBREF GetHeaderBkColor() const;
	protected:
		virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数
		virtual void OnDetach(HWND hOldWnd);//当控件离开一个窗口时，会调用一次该函数 

        LRESULT Popup(LPARAM lParam);
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
		CControlUI * CreateDialogView();

	protected:
		CDateWnd * m_pDialog;
		MUIDATE m_date;
		bool m_bMouseInButton;

		CControlUI * m_pDialogView;
		CDateViewUI * m_pDateView;
		CComboBoxUI * m_pYearBox;
		CComboBoxUI * m_pMonthBox;
		CButtonUI * m_pBtnGoback;
		CHorizontalLayoutUI * m_pHeader;
	};

}

#endif