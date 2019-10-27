
#ifndef _MYUI_TEXT_CONTROL_H_
#define _MYUI_TEXT_CONTROL_H_

#include "UILable.h"

namespace MYUI
{
	class MYUI_API CTextUI : public CLableUI
	{
	public:
		CTextUI();
		virtual ~CTextUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

	public:
		void SetShowHtml(bool bShow);
		bool IsShowHtml() const;

	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void PaintText(const RECT& rcItem, const RECT& rcPaint);
	protected:
		bool m_bShowHtml;

	private:

	};

}

#endif