
#ifndef _MYUI_LIST_CONTROL_H_
#define _MYUI_LIST_CONTROL_H_

#include "..//Layout//UIHorizontalLayout.h"
#include "..//Layout//UIVerticalLayout.h"

namespace MYUI
{

#define MLBXS_SINGLE  0x00000000//单选
#define MLBXS_MULTI   0x00000200//多选

	class MYUI_API CListBoxUI : public CVerticalLayoutUI
	{
	public:
		CListBoxUI();
		virtual ~CListBoxUI();
		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		bool SetSingleSelect(int index);//清除所有STATE_SELECT，然后设置一个STATE_SELECT
		bool AddSelect(int index);//MLBXS_MULTI下添加一个STATE_SELECT
		bool RemoveSelect(int index);//移除控件的STATE_SELECT
		void ClearSelect();//清除所有STATE_SELECT

		CControlUI * GetSelect();//只有MLBXS_SINGLE的时候有效

		int GetSelectCount();//有多少个STATE_SELECT
		int GetNextSelectIndex(int nBeginIndex);//当使用MLBXS_MULTI时，在轮询中使用改函数，取得所有STATE_SELECT
		
		virtual void SetStyle(DWORD dwStyle);
		virtual bool AddAt(CControlUI* pControl, int nIndex);
		virtual bool RemoveAt(int nIndex);
	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;
		CControlUI * GetItemByPoint(POINT &pt);
	protected:
		CControlUI * m_pHotControl;
		CControlUI * m_pPushedControl;

		CControlUI * m_pSelectControl;//[MLBXS_SINGLE]当前选中项

	private:

	};

	//class MYUI_API CListHeaderItemUI : public CHorizontalLayoutUI
	//{
	//public:
	//	CListHeaderItemUI();
	//	~CListHeaderItemUI();
	//	static CMuiString g_strClassName;
	//	virtual CMuiString GetClassName() const;

	//	int GetItemWidht(int nIndex);
	//protected:
	//	
	//private:

	//};

	

    typedef struct __MUILISTDRAW_INFO
	{
		MUISHAREINFO * pShareInfo;
	}MUILISTDRAWINFO;

	/*class MYUI_API CListElementUI 
	{
	public:
		CListElementUI();
		~CListElementUI();

		bool SetText(int nIndex, LPCTSTR strText);
		LPCTSTR GetText(int nIndex) const;
	private:
		CMuiPtrArray m_TextArray;
	};

	class MYUI_API CListViewUI : public CControlUI
	{
	public:
		CListViewUI();
		~CListViewUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		bool SelectItem(int nIndex);

		LPVOID GetHeader() const;
		LPVOID GetFilter() const;
		LPVOID GetList() const;

		int GetColumns() const;
		int GetCount() const;

	protected:


	private:

	};*/
}

#endif