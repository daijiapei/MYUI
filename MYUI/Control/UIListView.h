
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
		LINKED link;//�������ָ����һ����һ��
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
		LINKED link;//�������ָ��ͬһ����һ����һ�У���ͬһ�У���
		HDROWINFO info;
	}HEADERROW, * PHEADERROW;

/*********************************************************************************************
 * CListHeaderUI ��Ƹ�ʽ
 * ���ø�ʽ��
 *   ______________________________________________________________________________
 *   |          |          |          |          |          |          |          |
 *   |  head 1  |  head 2  |  head 3  |  head 4  |  head 5  |  head 6  |  head 7  |
 *   |__________|__________|__________|__________|__________|__________|__________|
 *
 * ���ͷ��ʽ��
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

#define MLHS_NOLYREAD       0x0001//����ֻ�����޷������󶨵Ŀؼ�
#define MLHS_UNCHENKSHOW    0x0002//ListView��һ��InfoBar������ѡ���ĸ�������/��ʾ�������־��ʾ����infobar����ʾ����
#define MLHS_SHOWCALC           0x0004//�������������У�����ʾ�ϼƣ�Ҫע������̫���������

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

//��ͷ�󶨵��������ͣ�Ĭ��Ϊ���֣���ͬ���������ͣ�������ɸѡ���ϼ�ʱ������ò�ͬ���㷨
#define LHDT_TEXT  0x0000//����
#define LHDT_INT   0x0001//����
#define LHDT_FLOAT 0x0002//������
		DWORD m_dwDateType;//��������
		//CMuiPtrArray m_Nodes;
		HEADERROW * m_pHdRow;//CListHeaderUI������m_pHdRow�����ݣ���ListView�������
		int m_nRowCount;
	};


/*********************************************************************************************
 * CListViewUI ��Ƹ�ʽ
 *    _____________________________________________________________
 *    |     |          |          |          |          |          |
 *    |     |  head 1  |  head 2  |  head 3  |  head 4  |  head 5  |    //��ͷ
 *    |_____|__________|__________|__________|__________|__________|
 *    |     |          |          |          |          |          |
 *    |     | filter 1 | filter 2 | filter 3 | filter 4 | filter 5 |    //���ڹ��ˣ�Ĭ�ϲ���ʾ
 *    |_____|__________|__________|__________|__________|__________|
 *    |     |          |          |          |          |          |
 *    | ��  |  item 1  |  item 2  |  item 3  |  item 4  |  item 5  |    //���壬Ĭ�ϲ���ʾ�к�
 *    | ʾ  |__________|__________|__________|__________|__________|
 *    | ��  |          |          |          |          |          |
 *    | ��  |  item 1  |  item 2  |  item 3  |  item 4  |  item 5  |    //���壬Ĭ�ϲ���ʾ�к�
 *    |_____|__________|__________|__________|__________|__________|
 *
 *
 *
*********************************************************************************************/

#define MLVS_NOLYREAD        0x0001//ȫ��ֻ�����޷������ؼ�
#define MLVS_FULLROWSELECT   0x0002//����ѡ�У�Ĭ��ѡ��ĳһ�е�ĳһ��
#define MLVS_SHOWFILTERBAR   0x0010//��ʾhead��body֮���ɸѡ��
#define MLVS_SHOWINFOBAR     0x0020//��ʾ������Ϣ��
#define MLVS_SHOWROWCODE     0x0040//��ʾ�кţ�ҪMLVS_SHOWBAR���
#define MLVS_SHOWCALCBAR     0x0080//��ʾ�ײ��ĺϼ���
#define MLVS_SELECTCOL       0x0100//��ɸѡ�����û���ѡ��Ҫ��ʾ/���ص���
#define MLVS_SELECTROW       0x0200//��ɸѡ�����û���ѡ��Ҫ��ʾ/���ص���(ͨ������ƥ��)

	class MYUI_API CListViewUI : public CContainerUI, public IControlArray
	{
	public:
		CListViewUI();
		virtual ~CListViewUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		//�����Add���������listviewͷ���ģ�body����ֻ��ͨ���������
		virtual bool Add(CControlUI* pControl);
		virtual bool AddAt(CControlUI* pControl, int nIndex);
		virtual bool Remove(CControlUI* pControl);
		virtual bool RemoveAt(int nIndex);
		virtual void RemoveAll();
		virtual CControlUI * Find(int nIndex);
		virtual int Find(CControlUI * pControl);
		virtual int GetCount() const;

		int GetColumnCount();//�ж�����
		int GetRowCount();//�ж�����

		virtual void SetShareInfo(TSHAREINFO * pShareInfo);
		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual SIZE GetContentSize();

		bool AddRow(LVROWINFO &info,int nRow = -1/* -1 ���һ��*/); //����һ��
		bool GetRow(LVROWINFO &info, int nRow);
		bool SetRow(LVROWINFO &info, int nRow);
		bool DelRow(int nRow = -1);//ɾ����
		//�����㷢����ôû��AddCol[�����]�Ⱥ�������ʵIControlArray�ӿھ�����������е�

		//����AddRow����һ�У�Ȼ����SetSubItem
		bool SetSubItem(HDROWINFO &info, int nCol, int nRow = -1);
		bool GetSubItem(HDROWINFO &info, int nCol, int nRow = -1);
		bool ExportExcel(LPCTSTR strFilePath);//�����ݵ�����execl
		
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


		ARGBREF m_refBetweenLinesColor;//�ָ�����ɫ
		ARGBREF m_refGridLinesColor;//��������ɫ
		ARGBREF m_refBetweenRowColor;//ÿ��һ�У�����ɫ�Ͳ�һ��

		CMuiPtrArray m_Headers;
		int m_nHeaderHeight;
		int m_nInfoBarWidth;

		LISTVIEWROW * m_pLvRow;
		int m_nRowCount;
		int m_nRowAutoHeight;

		CListHeaderUI * m_pHotHeader;
		CListHeaderUI * m_pPushedHeader;

		POINT m_ptSelectBeginItem;//���������꣬�Ǳ���ѡ�е����index
		POINT m_ptSelectEndItem;//���������꣬�Ǳ���ѡ�е����index
#define LVDF_NON     0x0000
#define LVDF_ITEM    0x0001
#define LVDF_HEADER  0x0002
		DWORD m_dwFocus;//����������
	protected:
	};


	
}

#endif