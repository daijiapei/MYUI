
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
		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		void SetStepBarSize(SIZE &size);
		const SIZE &GetStepBarSize() const;
		void SetStepBarColor(ARGBREF argb);
		ARGBREF GetStepBarColor();
		void SetStepBarImage(LPCTSTR strImage);
		LPCTSTR GetStepBarImage() const;
		//»æÖÆ
		virtual void PaintStatusImage(const RECT &rcUpdate) override;
		
	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;

	protected:
		POINT m_ptOldMousePos;
		bool m_bSelectStepBar;
		SIZE m_szStepBar;
		ARGBREF m_refStepBarColor;
		CMuiString m_strStepImage;

	};

}

#endif