
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
		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		
	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;

	private:

	};

}

#endif