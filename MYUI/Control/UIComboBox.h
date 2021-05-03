
#ifndef _MYUI_COMBOBOX_CONTROL_H_
#define _MYUI_COMBOBOX_CONTROL_H_

#include "UICombo.h"
#include "UIListBox.h"
namespace MYUI
{
	class CComboBoxWnd;

    class MYUI_API CComboBoxUI : public CEditUI, public IControlArray , protected IDialogPopup
	{
	public:
		CComboBoxUI();
		virtual ~CComboBoxUI();
		const static CMuiString g_strClassName;

		virtual CMuiString GetClassName() const;

		virtual bool Add(CControlUI* pControl);
		virtual bool AddAt(CControlUI* pControl, int nIndex);
		virtual bool Remove(CControlUI* pControl);
		virtual bool RemoveAt(int nIndex);
		virtual void RemoveAll();
		virtual CControlUI * Find(int nIndex);
		virtual int Find(CControlUI * pControl);
		virtual int GetCount();
	public:

		void SetItemBoxHeight(int nHeight);
		int GetItemBoxHeight();

		bool SetSelectItem(int nIndex);

		int GetSelectIndex();
		CControlUI * GetSelectItem();

		virtual bool SetHorizontalScrollBarActive(bool bActive);//是否一直显示滚动条
		virtual bool GetHorizontalScrollBarActive();
		virtual void SetHorizontalScrollBar(CScrollBarUI * pScrollBar);
		virtual CScrollBarUI * GetHorizontalScrollBar() const;

		virtual bool SetVerticalScrollBarActive(bool bActive);//是否一直显示滚动条
		virtual bool GetVerticalScrollBarActive();
		virtual void SetVerticalScrollBar(CScrollBarUI * pScrollBar);
		virtual CScrollBarUI * GetVerticalScrollBar() const;

		//绘制
		
	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
		RECT CalcButtonPostion();
        LRESULT Popup(LPARAM lParam);

	protected:
		CComboBoxWnd * m_pDialog;//下拉列表窗口
		CListBoxUI * m_pListBox;//下拉表的布局
		int m_nItemBoxHeight;
		bool m_bMouseInButton;

	};

}

#endif