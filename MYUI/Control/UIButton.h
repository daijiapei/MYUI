
#ifndef _MYUI_BUTTON_CONTROL_H_
#define _MYUI_BUTTON_CONTROL_H_

#include "UIText.h"

//按钮控件的特性是当遇到鼠标时图片状态会发生变化

namespace MYUI
{
	class MYUI_API CButtonUI : public CTextUI
	{
	public:
		CButtonUI();
		virtual ~CButtonUI();
		static CMuiString g_strClassName;

		virtual CMuiString GetClassName() const;

	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	};

}

#endif