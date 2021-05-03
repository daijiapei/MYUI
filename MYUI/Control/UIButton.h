
#ifndef _MYUI_BUTTON_CONTROL_H_
#define _MYUI_BUTTON_CONTROL_H_

#include "UIText.h"

//��ť�ؼ��������ǵ��������ʱͼƬ״̬�ᷢ���仯

namespace MYUI
{
	class MYUI_API CButtonUI : public CTextUI
	{
	public:
		CButtonUI();
		virtual ~CButtonUI();
		const static CMuiString g_strClassName;

		virtual CMuiString GetClassName() const;

	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);


	};

}

#endif