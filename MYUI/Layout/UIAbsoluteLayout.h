
#ifndef __MYUI_ABSOLUTE_LAYOUT_H__
#define __MYUI_ABSOLUTE_LAYOUT_H__

#include "UIBaseLayout.h"

namespace MYUI
{
	//绝对布局，如果xy，没有设置，则设置为布局的xy值（加上内缩进）
	//如果cx,cy，没有设置，则设置为布局的cx.cy减去子控件的x.y
	//值得注意的一个问题是，如果子控件发生重叠
	//OnPaint和FindControlByPoint都是没有问题的，但是GetDisplayRect却不能取得正确的显示区域
	//因为GetDisplayRect只考虑父控件是否遮挡了它，没有考虑兄弟控件是否遮挡了它
	class MYUI_API CAbsoluteLayoutUI : public CBaseLayoutUI
	{
	public:
		CAbsoluteLayoutUI();
		virtual ~CAbsoluteLayoutUI();
		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual bool SetItem(RECT rcItem, bool bMustUpdate) override;
		virtual SIZE GetContentSize();

	protected:
		
	};
}

#endif