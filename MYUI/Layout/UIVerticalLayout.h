
#ifndef _MYUI_VERTICAL_LAYOUT_H_
#define _MYUI_VERTICAL_LAYOUT_H_

#include "UIBaseLayout.h"

namespace MYUI
{
	//垂直布局，控件从上往下排列，忽略xy值，
	//如果子控件没有设置cx，则子控件的宽跟CVerticalLayoutUI一样
	//如果子控件没有设置cy，则子控件的高按CVerticalLayoutUI高度的多余空间平均分配

	class MYUI_API CVerticalLayoutUI : public CBaseLayoutUI
	{
	public:
		CVerticalLayoutUI();
		virtual ~CVerticalLayoutUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual SIZE GetContentSize();
	protected:
		bool m_bCentered;//尽量将控件居中
	};
}

#endif