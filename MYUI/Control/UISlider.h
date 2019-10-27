
#ifndef _MYUI_SLIDER_CONTROL_H_
#define _MYUI_SLIDER_CONTROL_H_

#include "UIProgress.h"

namespace MYUI
{
	class MYUI_API CSliderUI : public CProgressUI
	{
	public:
		CSliderUI();
		virtual ~CSliderUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		void SetStepBarSize(SIZE &size);
		const SIZE &GetStepBarSize() const;
		void SetStepBarColor(ARGBREF argb);
		ARGBREF GetStepBarColor();
		void SetStepBarImage(LPCTSTR strImage);
		LPCTSTR GetStepBarImage() const;
		//»æÖÆ
		virtual void PaintStatusImage(const RECT &rcItem,const RECT &rcPaint);
		
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		bool IsPointInStepBar(POINT &pt);

	protected:
		POINT m_ptOldMousePos;
		bool m_bSelectStepBar;
		SIZE m_szStepBar;
		ARGBREF m_refStepBarColor;
		CMuiString m_strStepImage;

	};

}

#endif