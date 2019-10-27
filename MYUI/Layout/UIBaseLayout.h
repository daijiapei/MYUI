

#ifndef _MYUI_BASE_LAYOUT_H_
#define _MYUI_BASE_LAYOUT_H_


#include "..//Core//UIContainer.h"

namespace MYUI
{
	class MYUI_API CBaseLayoutUI : public IControlArray, public CContainerUI
	{
	public:
		CBaseLayoutUI();
		virtual ~CBaseLayoutUI();
		virtual void SetShareInfo(TSHAREINFO * pShareInfo);
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		virtual CControlUI * FindControlByPoint(POINT &pt);
		virtual CControlUI * FindControlByName(LPCTSTR strName);
		//位置
		virtual void SetInset(RECT &rect);
		virtual const RECT &GetInset() const;

		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);

	public:

		//CControlArray
		virtual bool Add(CControlUI* pControl);
		virtual bool AddAt(CControlUI* pControl, int nIndex);
		virtual bool Remove(CControlUI* pControl);
		virtual bool RemoveAt(int nIndex);
		virtual void RemoveAll();
		virtual CControlUI * Find(int nIndex);
		virtual int Find(CControlUI * pControl);
		virtual int GetCount() const;
	protected:
		RECT m_rcInset;//内边距在m_rcClient内缩进，形成一个区域，然后提供给子控件
		CMuiPtrArray m_Items;
	};
}

#endif