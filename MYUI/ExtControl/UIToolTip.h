
#ifndef _MYUI_TOOLTIP_CONTROL_H_
#define _MYUI_TOOLTIP_CONTROL_H_

#include "../base.h"

namespace MYUI
{
	//将控件的屏幕位置传进来，CDragItemUI会根据位置绘制一个画面，然后
	//拖动窗口，鼠标释放会收到通知
	//常规调用模板 :
	//CDragItemUI * pDragItem = new CDragItemUI();
	//pDragItem->Create(hInstance, pWindow, rect, WM_LBUTTONUP, 220, &szRound);
	class MYUI_API CToolTipUI 
	{
	public:
		CToolTipUI();
		virtual ~CToolTipUI();
		static LPCTSTR g_strClassName;

		HWND Create(HINSTANCE hInstance);
		BOOL Close(LONG nRet = 0);//异步关闭，
		BOOL Destroy();

		void ShowWindow(BOOL bShow);
		BOOL ShowTip(POINT point);

		void SetText(LPCTSTR strText);
		LPCTSTR GetText() const;
		void SetTextPadding(const RECT &rect);
		const RECT &GetTextPadding() const;
		void SetBkColor(ARGBREF refColor);
		ARGBREF GetBkColor() const;
		void SetBorderColor(ARGBREF refColor);
		ARGBREF GetBorderColor() const;
		void SetFont(HFONT hFont);
		HFONT GetFont() const;
		void SetAlpha(BYTE nAlpha);
		BYTE GetAlpha() const;

	protected:
		
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	private:
		static LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		bool RegisterClass(HINSTANCE hInstance);

	protected:
		HWND m_hWnd;
		BYTE m_nAlpha;
		HDC m_hViewDc;
		TCHAR m_strText[1024];
		RECT m_rcTextPadding;
		ARGBREF m_refBkColor;
		ARGBREF m_refBorderColor;
		HFONT m_hFont;
	};

}

#endif