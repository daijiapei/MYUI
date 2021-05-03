
#ifndef _MYUI_LIST_CONTROL_H_
#define _MYUI_LIST_CONTROL_H_

#include "..//Layout//UIHorizontalLayout.h"
#include "..//Layout//UIVerticalLayout.h"

namespace MYUI
{

#define MLBXS_SINGLE  0x00000000//��ѡ
#define MLBXS_MULTI   0x00000200//��ѡ

	class MYUI_API CListBoxUI : public CVerticalLayoutUI
	{
	public:
		CListBoxUI();
		virtual ~CListBoxUI();
		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		bool SetSingleSelect(int index);//�������STATE_SELECT��Ȼ������һ��STATE_SELECT
		bool AddSelect(int index);//MLBXS_MULTI�����һ��STATE_SELECT
		bool RemoveSelect(int index);//�Ƴ��ؼ���STATE_SELECT
		void ClearSelect();//�������STATE_SELECT

		CControlUI * GetSelect();//ֻ��MLBXS_SINGLE��ʱ����Ч

		int GetSelectCount();//�ж��ٸ�STATE_SELECT
		int GetNextSelectIndex(int nBeginIndex);//��ʹ��MLBXS_MULTIʱ������ѯ��ʹ�øĺ�����ȡ������STATE_SELECT
		
		virtual void SetStyle(DWORD dwStyle);
		virtual bool AddAt(CControlUI* pControl, int nIndex);
		virtual bool RemoveAt(int nIndex);
	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;
		CControlUI * GetItemByPoint(POINT &pt);
	protected:
		CControlUI * m_pHotControl;
		CControlUI * m_pPushedControl;

		CControlUI * m_pSelectControl;//[MLBXS_SINGLE]��ǰѡ����

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