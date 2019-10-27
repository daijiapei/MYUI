
#ifndef _MYUI_LISTVIEW_CONTROL_H_
#define _MYUI_LISTVIEW_CONTROL_H_

#include "..//Layout//UIHorizontalLayout.h"
#include "..//Layout//UIVerticalLayout.h"

namespace MYUI
{
	typedef struct _LINKED_
	{
		struct _LINKED_ * pre;
		struct _LINKED_ * next;
	}LINKED, * PLINKED;

	typedef struct _LVROWINFO_
	{
		int nHeight;
		LPVOID lParam;
	}LVROWINFO, *PLVROWINFO;

	typedef struct _LISTVIEWROW_
	{
		LINKED link;//这个用来指向上一行下一行
		LVROWINFO info;
	}LISTVIEWROW, *PLISTVIEWROW;

	typedef struct _HDROWINFO_
	{
		DWORD dwState;
		ARGBREF refBkColor;
		LPARAM lParam;
		int nTextLenght;
		TCHAR * strText;
	}HDROWINFO, PHDROWINFO;

	typedef struct _HEADERROW_
	{
		LINKED link;//这个用来指向同一列上一行下一行，是同一列！！
		HDROWINFO info;
	}HEADERROW, * PHEADERROW;

/*********************************************************************************************
 * CListHeaderUI 设计格式
 * 常用格式：
 *   ______________________________________________________________________________
 *   |          |          |          |          |          |          |          |
 *   |  head 1  |  head 2  |  head 3  |  head 4  |  head 5  |  head 6  |  head 7  |
 *   |__________|__________|__________|__________|__________|__________|__________|
 *
 * 多表头格式：
 *   ________________________________________________________________________________________
 *   |                            |                            |                            |                 
 *   |          head 1            |           head 2           |           head 3           | 
 *   |____________________________|____________________________|____________________________|
 *   |             |              |      |                     |                            |
 *   |  subhead 1  |              | sub1 |      subihead2      |          subhead 1         |
 *   |_____________|  subhead 2   |______|_____________________|____________________________|
 *   |      |      |              |      |      |      |       |        |        |          |
 *   | sub1 | sub2 |              | sub1 | sub1 | sub2 | sub3  |  sub1  |  sub2  |  sub3    |
 *   |______|______|______________|______|______|______|_______|________|________|__________|
 *
*********************************************************************************************/
	class CListViewUI;

#define MLHS_NOLYREAD       0x0001//该列只读，无法弹出绑定的控件
#define MLHS_UNCHENKSHOW    0x0002//ListView有一个InfoBar，可以选择哪个列隐藏/显示，这个标志表示不在infobar中显示该列
#define MLHS_SHOWCALC           0x0004//如果如果是数字列，则显示合计，要注意数据太大溢出问题

	class MYUI_API CListHeaderUI : public CLableUI
	{
		friend class CListViewUI;
	public:
		CListHeaderUI();
		~CListHeaderUI();

		/*
		//virtual bool Add(CControlUI* pControl);
		//virtual bool AddAt(CControlUI* pControl, int nIndex);
		//virtual bool Remove(CControlUI* pControl);
		//virtual bool RemoveAt(int nIndex);
		//virtual void RemoveAll();
		//virtual CControlUI * Find(int nIndex);
		//virtual int Find(CControlUI * pControl);
		//virtual int GetCount() const;
		*/

		bool AddRow(HDROWINFO &info,int nRow = -1);
		bool GetRow(HDROWINFO &info, int nRow = -1);
		bool SetRow(HDROWINFO &info, int nRow = -1);
		bool DelRow(int nRow = -1);

	protected:
		
	protected:

//表头绑定的数据类型，默认为文字，不同的数据类型，在排序，筛选，合计时，会调用不同的算法
#define LHDT_TEXT  0x0000//文字
#define LHDT_INT   0x0001//整数
#define LHDT_FLOAT 0x0002//浮点数
		DWORD m_dwDateType;//数据类型
		//CMuiPtrArray m_Nodes;
		HEADERROW * m_pHdRow;//CListHeaderUI不绘制m_pHdRow的数据，由ListView负责绘制
		int m_nRowCount;
	};


/*********************************************************************************************
 * CListViewUI 设计格式
 *    _____________________________________________________________
 *    |     |          |          |          |          |          |
 *    |     |  head 1  |  head 2  |  head 3  |  head 4  |  head 5  |    //表头
 *    |_____|__________|__________|__________|__________|__________|
 *    |     |          |          |          |          |          |
 *    |     | filter 1 | filter 2 | filter 3 | filter 4 | filter 5 |    //用于过滤，默认不显示
 *    |_____|__________|__________|__________|__________|__________|
 *    |     |          |          |          |          |          |
 *    | 显  |  item 1  |  item 2  |  item 3  |  item 4  |  item 5  |    //表体，默认不显示行号
 *    | 示  |__________|__________|__________|__________|__________|
 *    | 行  |          |          |          |          |          |
 *    | 号  |  item 1  |  item 2  |  item 3  |  item 4  |  item 5  |    //表体，默认不显示行号
 *    |_____|__________|__________|__________|__________|__________|
 *
 *
 *
*********************************************************************************************/

#define MLVS_NOLYREAD        0x0001//全部只读，无法弹出控件
#define MLVS_FULLROWSELECT   0x0002//整行选中，默认选中某一列的某一行
#define MLVS_SHOWFILTERBAR   0x0010//显示head与body之间的筛选栏
#define MLVS_SHOWINFOBAR     0x0020//显示左侧的信息栏
#define MLVS_SHOWROWCODE     0x0040//显示行号，要MLVS_SHOWBAR配合
#define MLVS_SHOWCALCBAR     0x0080//显示底部的合计栏
#define MLVS_SELECTCOL       0x0100//列筛选栏，用户可选择要显示/隐藏的列
#define MLVS_SELECTROW       0x0200//行筛选栏，用户可选择要显示/隐藏的栏(通过数据匹配)

	class MYUI_API CListViewUI : public CContainerUI, public IControlArray
	{
	public:
		CListViewUI();
		virtual ~CListViewUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		//这里的Add是用来添加listview头部的，body数据只能通过代码插入
		virtual bool Add(CControlUI* pControl);
		virtual bool AddAt(CControlUI* pControl, int nIndex);
		virtual bool Remove(CControlUI* pControl);
		virtual bool RemoveAt(int nIndex);
		virtual void RemoveAll();
		virtual CControlUI * Find(int nIndex);
		virtual int Find(CControlUI * pControl);
		virtual int GetCount() const;

		int GetColumnCount();//有多少列
		int GetRowCount();//有多少行

		virtual void SetShareInfo(TSHAREINFO * pShareInfo);
		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual SIZE GetContentSize();

		bool AddRow(LVROWINFO &info,int nRow = -1/* -1 最后一行*/); //增加一行
		bool GetRow(LVROWINFO &info, int nRow);
		bool SetRow(LVROWINFO &info, int nRow);
		bool DelRow(int nRow = -1);//删除行
		//或许你发现怎么没有AddCol[添加列]等函数，其实IControlArray接口就是用来添加列的

		//首先AddRow新增一行，然后再SetSubItem
		bool SetSubItem(HDROWINFO &info, int nCol, int nRow = -1);
		bool GetSubItem(HDROWINFO &info, int nCol, int nRow = -1);
		bool ExportExcel(LPCTSTR strFilePath);//将数据导出到execl
		
	public:
		void SetHeaderBkColor(ARGBREF refColor);
		ARGBREF GetHeaderBkColor() const;
		void SetHeaderBkImage(LPCTSTR strImage);
		LPCTSTR GetHeaderBkImage() const;
		void SetHeaderHeight(int nHeight);
		int GetHeaderHeight() const;
		void SetInfoBarWidth(int nWidth);
		int GetInfoBarWidth() const;

	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		bool AddHeaderRow(int nRow = -1);
		bool DelHeaderRow(int nRow = -1);

		virtual void PaintStatusImage( const RECT& rcItem, const RECT& rcPaint);
		void PaintHeader(const RECT& rcItem, const RECT &rcPaint,const RECT &rcUpdate);
		void PaintBody(const RECT& rcItem, const RECT &rcPaint);

		CListHeaderUI * FindHeaderByPoint(POINT &pt);
		CListHeaderUI * FindHeaderByName(LPCTSTR strName);

		bool FindItemByPoint(POINT pt, int &nCol, int &nRow);
		bool SetSelect(POINT *ptBegin = NULL, POINT * ptEnd = NULL);
	protected:
		ARGBREF m_refHeaderBkColor;
		CMuiString m_strHeaderBkImage;


		ARGBREF m_refBetweenLinesColor;//分隔线颜色
		ARGBREF m_refGridLinesColor;//网格线颜色
		ARGBREF m_refBetweenRowColor;//每隔一行，背景色就不一样

		CMuiPtrArray m_Headers;
		int m_nHeaderHeight;
		int m_nInfoBarWidth;

		LISTVIEWROW * m_pLvRow;
		int m_nRowCount;
		int m_nRowAutoHeight;

		CListHeaderUI * m_pHotHeader;
		CListHeaderUI * m_pPushedHeader;

		POINT m_ptSelectBeginItem;//这个不是鼠标，是保存选中的项的index
		POINT m_ptSelectEndItem;//这个不是鼠标，是保存选中的项的index
#define LVDF_NON     0x0000
#define LVDF_ITEM    0x0001
#define LVDF_HEADER  0x0002
		DWORD m_dwFocus;//焦点在哪里
	protected:
	};


	
}

#endif