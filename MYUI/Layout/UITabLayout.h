
#ifndef _MYUI_TAB_LAYOUT_H_
#define _MYUI_TAB_LAYOUT_H_

#include "UIBaseLayout.h"

namespace MYUI
{
	//标签布局只显示m_pSelect控件，其余的隐藏起来，
	//忽略xy,cx,cy值，每个子控件的大小默认和CTabLayoutUI一样
	//标签布局有些计算是重复计算的，但不会过多地消耗效率
	//这样做的原因是为了将操作都集成到一个函数中，这样能更方便地修改接口
	class MYUI_API CTabLayoutUI : public CBaseLayoutUI
	{
	public:
		CTabLayoutUI();
		virtual ~CTabLayoutUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);


		virtual bool SetItem(RECT rcItem, bool bMustUpdate) ;
		virtual SIZE GetContentSize();

		virtual CControlUI * GetSelect();
		virtual int GetIndex();
		virtual bool SelectItem(int nIndex);
		virtual bool SelectItem(CControlUI * pControl);
		

		virtual bool AddAt(CControlUI* pControl, int iIndex);
		virtual bool RemoveAt(int iIndex);

		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
	protected:

	private:
		CControlUI * m_pSelect;
	};
}

#endif