
#ifndef _MYUI_SCROLLBAR_CONTROL_H_
#define _MYUI_SCROLLBAR_CONTROL_H_

#include "..\\Core\\UIControl.h"

namespace MYUI
{
	enum ScrollBarStyle
	{
		NonStyle = 0, //无任何风格，将不做任何处理，包括不显示滚动条
		LeftVerticalStyle = 0x01, //左垂直滚动条
		RightVerticalStyle = 0x02, //右垂直滚动条
		AutoVerticalStyle = RightVerticalStyle, //默认垂直滚动条
		TopHorizontalStyle = 0x04, //左水平滚动条
		BottomHorizontalStyle = 0x08, //右水平滚动条
		AutoHorizontalStyle = BottomHorizontalStyle, //默认水平滚动条

		//下面用来判断是否拥有滚动条风格
		BarVertical = 0x03,
		BarHorizontal = 0x0C,
		BarHas = 0x0F 
	};

	enum ScrollShiftStyle
	{
		SameDirection = 1,
		ContraDirection = -1,
		Auto = ContraDirection
	};

	

	class MYUI_API CScrollBarUI : public CControlUI
	{
	public:
		//一个滚动条由前导按钮，后导按钮，拖拽按钮，轨道，四个部分组成
		//前导按钮，后导按钮，合称向导按钮
		//注意在垂直滚动条中，向导按钮的宽由滚动条的宽决定
		//注意在水平滚动条中，向导按钮的高由滚动条的高决定
		CScrollBarUI(ScrollBarStyle style);
		virtual ~CScrollBarUI();
		
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual void SetFilterFloatType(EnumFloatType type);
		ScrollBarStyle GetStyle() const;
		void SetActive(bool bActive);
		bool IsActive();
		bool IsShowing();//是否正在显示

		void SetShiftStyle(ScrollShiftStyle style);
		ScrollShiftStyle GetShiftStyle();
		void SetDragBarMaxShift(int nShift);
		int GetDragBarMaxShift();//滚动条最大可以移动的距离,一般是【控件大小-客户区】
		void SetDragBarShift(int nShift);//拖拽按钮的位置
		int GetDragBarShift();

		void SetBarIntersect(RECT &rect);
		//向导按钮，就是向前向后这两个按钮，二者使用相同的参数
		//注意，如果创建的是垂直滚动条，向导按钮的宽，是根据垂直滚动条的宽设定的
		//所以在垂直滚动条中，向导按钮的宽无效
		//注意，如果创建的是水平滚动条，向导按钮的高，是根据水平滚动条的高设定的
		//所以在水平滚动条中，向导按钮的高无效
		bool CalcDragBarRect();
		virtual bool SetItem(RECT &rcItem, bool bMustUpdate);
		virtual bool OnPaint(const RECT& rcUpdate);

		//图片
		void SetFrontNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontNormalImage();
		void SetFrontHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontHotImage();
		void SetFrontPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontPushedImage();
		void SetFrontDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontDisabledImage();

		void SetFinalNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetFinalNormalImage();
		void SetFinalHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFinalHotImage();
		void SetFinalPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFinalPushedImage();
		void SetFinalDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetFinalDisabledImage();

		void SetTrackNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetTrackNormalImage();
		void SetTrackHotImage(LPCTSTR pStrImage);
		LPCTSTR GetTrackHotImage();
		void SetTrackPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetTrackPushedImage();
		void SetTrackDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetTrackDisabledImage();

		void SetDragBtnNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetDragBtnNormalImage();
		void SetDragBtnHotImage(LPCTSTR pStrImage);
		LPCTSTR GetDragBtnHotImage();
		void SetDragBtnPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetDragBtnPushedImage();
		void SetDragBtnDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetDragBtnDisabledImage();

	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
		virtual void SetState(DWORD dwState);
		int GetDragBarOffset();

		DWORD CalcPointIn(const POINT& pt);

		void PaintTrackButton(const RECT& rcUpdate);//承载拖拽按钮的轨道
		void PaintFrontButton(const RECT& rcUpdate);//开始的按钮
		void PaintFinalButton(const RECT& rcUpdate);//末尾的按钮
		void PaintDragButton(const RECT& rcUpdate);//拖拽按钮
		void PaintIntersect(const RECT& rcUpdate);//交汇区域

	private:
		POINT m_ptOldMousePos;

#define SBPI_DRAGBTN   0x0001
#define SBPI_FRONTBTN  0x0002
#define SBPI_FINALBTN  0x0004
#define SBPI_TRACKBTN  0x0008
#define SBPI_INTERSECT 0x0010
		DWORD m_dwPointIn;//命中那个部分
		ScrollBarStyle m_dwStyle;
		ScrollShiftStyle m_dwShiftStyle;
		bool m_bActive;//是否一直显示
		int m_nMaxShift;//可移动的，最大位置
		int m_nNowShift;//当前位置

		RECT m_rcBarIntersect;//垂直滚动条与水平滚动条的交汇点
		RECT m_rcFrontButton;//开始的按钮
		RECT m_rcFinalButton;//末尾的按钮
		RECT m_rcDragButton;//拖拽按钮
		RECT m_rcTrackButton;//承载拖拽按钮的轨道

		//一般情况下，滚动条是不需要获得焦点的
		CMuiString m_strFrontNormalImage;//普通状态
		CMuiString m_strFrontHotImage;//鼠标经过控件状态
		CMuiString m_strFrontPushedImage;//忙碌状态，比如正在点击控件
		CMuiString m_strFrontDisabledImage;//禁用状态

		CMuiString m_strFinalNormalImage;//普通状态
		CMuiString m_strFinalHotImage;//鼠标经过控件状态
		CMuiString m_strFinalPushedImage;//忙碌状态，比如正在点击控件
		CMuiString m_strFinalDisabledImage;//禁用状态

		CMuiString m_strTrackNormalImage;//普通状态
		CMuiString m_strTrackHotImage;//鼠标经过控件状态
		CMuiString m_strTrackPushedImage;//忙碌状态，比如正在点击控件
		CMuiString m_strTrackDisabledImage;//禁用状态

		CMuiString m_strDragBtnNormalImage;//普通状态
		CMuiString m_strDragBtnHotImage;//鼠标经过控件状态
		CMuiString m_strDragBtnPushedImage;//忙碌状态，比如正在点击控件
		CMuiString m_strDragBtnDisabledImage;//禁用状态
	};

	//定义两个简单一点的滚动条，方便使用
	class MYUI_API CHorizontailScrollBarUI : public CScrollBarUI
	{
	public:
		CHorizontailScrollBarUI()
			: CScrollBarUI(AutoHorizontalStyle)
		{  0;  }

		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const
		{
			return CHorizontailScrollBarUI::g_strClassName;
		}
	};
	
	class MYUI_API CVerticalScrollBarUI : public CScrollBarUI
	{
	public:
		CVerticalScrollBarUI()
			: CScrollBarUI(AutoVerticalStyle)
		{  0;  }

		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const
		{
			return CVerticalScrollBarUI::g_strClassName;
		}
	};
	
	class IScrollBarMove
	{
		//pSender = 目标滚动条
	public:
		virtual void OnScrollBarMove(CScrollBarUI* pSender, int nShift) = 0;
		virtual const SIZE& GetScrollBarShift() const = 0;
	};
}

#endif