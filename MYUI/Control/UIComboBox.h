
#ifndef _MYUI_COMBOBOX_CONTROL_H_
#define _MYUI_COMBOBOX_CONTROL_H_

#include "UICombo.h"
#include "UIListBox.h"
namespace MYUI
{
	class CComboBoxWnd;

	class MYUI_API CComboBoxUI : public CEditUI , public IControlArray
	{
	public:
		CComboBoxUI();
		virtual ~CComboBoxUI();
		static CMuiString g_strClassName;

		virtual CMuiString GetClassName() const;

		virtual bool Add(CControlUI* pControl);
		virtual bool AddAt(CControlUI* pControl, int nIndex);
		virtual bool Remove(CControlUI* pControl);
		virtual bool RemoveAt(int nIndex);
		virtual void RemoveAll();
		virtual CControlUI * Find(int nIndex);
		virtual int Find(CControlUI * pControl);
		virtual int GetCount() const;
	public:

		void SetItemBoxHeight(int nHeight);
		int GetItemBoxHeight();

		bool SetSelectItem(int nIndex);

		int GetSelectIndex();
		CControlUI * GetSelectItem();

		virtual bool SetHorizontalScrollBarActive(bool bActive);//�Ƿ�һֱ��ʾ������
		virtual bool GetHorizontalScrollBarActive();
		virtual void SetHorizontalScrollBar(CScrollBarUI * pScrollBar);
		virtual CScrollBarUI * GetHorizontalScrollBar() const;

		virtual bool SetVerticalScrollBarActive(bool bActive);//�Ƿ�һֱ��ʾ������
		virtual bool GetVerticalScrollBarActive();
		virtual void SetVerticalScrollBar(CScrollBarUI * pScrollBar);
		virtual CScrollBarUI * GetVerticalScrollBar() const;

		LRESULT NotifyCall(HOBJECT hObject, EnumNotifyMsg * notify);
		//����
		
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void OnAttach(HWND hNewWnd);//���ؼ����ӵ��µĴ���ʱ�������һ�θú���
		virtual void OnDetach(HWND hOldWnd);//���ؼ��뿪һ������ʱ�������һ�θú��� 

		bool ShowDialog(bool bShow = true);

	protected:
		CComboBoxWnd * m_pDialog;//�����б�����
		CListBoxUI * m_pListBox;//�������Ĳ���
		int m_nItemBoxHeight;
		bool m_bMouseInButton;

	};

}

#endif