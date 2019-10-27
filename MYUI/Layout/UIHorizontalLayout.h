
#ifndef _MYUI_HORIZONTAIL_LAYOUT_H_
#define _MYUI_HORIZONTAIL_LAYOUT_H_

#include "UIBaseLayout.h"

namespace MYUI
{
	//水平布局，控件从左往右排列，忽略xy值，
	//如果子控件没有设置cy，则子控件的高跟CHorizontalLayoutUI一样
	//如果子控件没有设置cx，则子控件的宽按CHorizontalLayoutUI宽度的多余空间平均分配
	class MYUI_API CHorizontalLayoutUI : public CBaseLayoutUI
	{
	public:
		CHorizontalLayoutUI();
		virtual ~CHorizontalLayoutUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual SIZE GetContentSize();
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
	protected:
		bool m_bCentered;//尽量将控件居中
	};
}


#endif