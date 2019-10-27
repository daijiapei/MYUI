
#ifndef _MYUI_COMBO_CONTROL_H_
#define _MYUI_COMBO_CONTROL_H_

#include "UIButton.h"
#include "UIEdit.h"

namespace MYUI
{

	class MYUI_API CComboUI : public CEditUI
	{
	public:
		CComboUI();
		virtual ~CComboUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:

	};

}

#endif