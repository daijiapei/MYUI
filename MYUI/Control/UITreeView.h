
#ifndef _MYUI_TREEVIEW_CONTROL_H_
#define _MYUI_TREEVIEW_CONTROL_H_

#include "../Layout/UIVerticalLayout.h"

namespace MYUI
{
	class CTreeViewUI;

	class MYUI_API CNodeInterface
	{
	public:
		CNodeInterface()
		{
		}

#define TINE_DRAWNODEBK    0x0001//���Ʊ���
#define TINE_SETSELECT     0x0002//����SELECT״̬//(bool)wParam
#define TINE_SETCHECK      0x0003//����CHECK״̬//(bool)wParam
#define TINE_GETNODECOUNT  0x0004//ȡ�ýڵ�����
#define TINE_GETNODEINDEX  0x0005//ȡ�ÿؼ�����,����������ڽڵ�������˵���Ҳ�������
	public:
		//void SetParentControl
		virtual bool OnEvent(DWORD dwEvent, LPVOID wParam, LPVOID lParam)  = 0;
		//virtual void OnDrawNodeBk(const RECT &rcItem, const RECT &rcPaint) = 0;
	};

#define MTNS_READONLY   0x0100//ֻ��
#define MTNS_SHOWCHECK  0x0200//Ĭ�ϲ���ʾcheck

//AUTO = FALSE =������û���ӽڵ㣬����ʾEXPAND���ţ�
//TRUE = ���ӽڵ��ʱ��EXPAND���ŲŻ���ʾ
#define MTNS_HIDEEXPAND 0x0400
#define MTNS_CHECKCUR   0x0800//ֻ�ܹ�ѡ��ǰ��

/***************************************************************************
 * CTreeNodeUI ��Ƹ�ʽ
 * ע�⣺������SetItem��ʱ������Expandƫ�ƣ���OnPaint��ʱ��������ƫ��
 * �����SetItem������Expandƫ�ƣ�����տ���Expand����ʱ��
 * FindControlByPoint ���Ҳ���ƥ��Ŀؼ�
 *    _________________________________
 *    | > | �� |  text                |  = CTreeNodeUI/CTreeNodeReinUI
 *    - E -----------------------------
 *    | x |   �ӽڵ� 1                |  = CTreeNodeUI::m_Nodes[0]
 *    | p -----------------------------
 *    | a |   �ӽڵ� 2                |  = CTreeNodeUI::m_Nodes[1]
 *    | n -----------------------------
 *    | d |   �ӽڵ� 3                |  = CTreeNodeUI::m_Nodes[2]
 *    ---------------------------------
***************************************************************************/
	class MYUI_API CTreeNodeUI :  public IControlArray, public CControlUI , public CNodeInterface
	{
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
	public:
		CTreeNodeUI();
		virtual ~CTreeNodeUI();

		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual SIZE GetValidSize();

		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
		virtual void PaintBkColor( const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintBkImage( const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintStatusImage( const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintText(const RECT& rcItem, const RECT& rcPaint);
		virtual VOID SetShareInfo(TSHAREINFO * pShareInfo);
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		virtual bool Update();//�һ���¸����ֵ����������ֵܿؼ�Ҳ�����
        virtual bool Renewal();//�һὫ�Ӵ��ڵ�λ�ö�����һ�Σ���ˢ����ͼ
		virtual bool Invalidate();//��ֻ�����Լ����������������ؼ������ܵ�Ӱ��

		virtual CControlUI * FindControlByPoint(POINT &pt);
		

		//CNodeInterface
		virtual void OnDrawNodeBk(const RECT &rcItem, const RECT &rcPaint);
		virtual bool OnEvent(DWORD dwEvent, LPVOID wParam, LPVOID lParam);
	public:

		//CControlUI * GetNextNode() const;//��һ��
		//CControlUI * GetPreNode() const;//��һ��
		//CControlUI * GetFirstNode() const;//��һ��
		//CControlUI * GetLastNode() const;//���һ��

		//CControlUI * GetContainer() const;

		//CTreeNodeUI * GetRootNode() const;//CTreeViewUI�ĵ�һ���ڵ�
		//CTreeNodeUI * GetParentNode() const;//ParentControl��CTreeViewUI��ParentNode��CTreeNodeUI
		//CTreeNodeUI * GetChildNode() const;//��һ���ӽڵ㣬�ǵ�һ����

		//CTreeNodeUI * GetSelectNode() const;//��ǰѡ����

		//void SetReinHeight(int nHeight);

		void SetTextPadding(const RECT &rect);
		const RECT &GetTextPadding() const;
		void SetExpandOffset(int nOffset);
		int GetExpandOffset() const;
		void SetExpanded(bool bExpand);
		bool GetExpanded() const;//�ڵ��Ƿ�չ��
		void SetChecked(bool bCheck);
		bool GetChecked() const;

		void SetExpandNormalImage(LPCTSTR strImage);
		LPCTSTR GetExpandNormalImage() const;
		void SetExpandHotImage(LPCTSTR strImage);
		LPCTSTR GetExpandHotImage() const ;
		void SetUnExpandNormalImage(LPCTSTR strImage);
		LPCTSTR GetUnExpandNormalImage() const;
		void SetUnExpandHotImage(LPCTSTR strImage);
		LPCTSTR GetUnExpandHotImage() const ;

		void SetCheckNormalImage(LPCTSTR strImage);
		LPCTSTR GetCheckNormalImage() const;
		void SetCheckHotImage(LPCTSTR strImage);
		LPCTSTR GetCheckHotImage() const ;
		void SetUnCheckNormalImage(LPCTSTR strImage);
		LPCTSTR GetUnCheckNormalImage() const;
		void SetUnCheckHotImage(LPCTSTR strImage);
		LPCTSTR GetUnCheckHotImage() const ;

		void SetHotTextColor(ARGBREF argb);
		ARGBREF GetHotTextColor();
		void SetSelectTextColor(ARGBREF argb);
		ARGBREF GetSelectTextColor();
		void SetPushedTextColor(ARGBREF argb);
		ARGBREF GetPushedTextColor();
		
		void SetNormalSelectColor(ARGBREF refColor);
		ARGBREF GetNormalSelectColor() const;
		void SetFocusSelectColor(ARGBREF refColor);
		ARGBREF GetFocusSelectColor() const;
		void SetHotColor(ARGBREF refColor);
		ARGBREF GetHotColor() const;
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		DWORD CalcPointIn(const POINT& pt);

	protected:
		CMuiPtrArray m_Nodes;
		//��Ϊһ����Expand���л��ˣ�����Expand������pushed״̬
		//�����Expand����(DOWN)���л�Expand״̬�����ı�˫����Ҳ�л�Expand״̬
		CMuiString m_strExpandNormalImage;
		CMuiString m_strExpandHotImage;
		CMuiString m_strUnExpandNormalImage;
		CMuiString m_strUnExpandHotImage;

		//VS��Դ���� |expand|�ļ�ͼ��| �ڵ����� |
		//��������һ�£����������VS�Ľ����������Դ��������Ч������Ӧ����ôŪ�أ�
		//��ʵ���ǽ�checkImage���ó��ļ�ͼ�꣬Ȼ��ؼ�״̬Ϊֻ���Ϳ�����
		//���������ֻ���Ļ������check��ֻ������check��״̬������չ���ڵ�

		//Ĭ�Ͽ�д��check����(CLICK)�л�check״̬��
		//ֻ����˫���л�Expand״̬��Ч�����ı�һ��
		CMuiString m_strCheckNormalImage;
		CMuiString m_strCheckHotImage;
		CMuiString m_strUnCheckNormalImage;
		CMuiString m_strUnCheckHotImage;

		ARGBREF m_refHotTextColor;
		ARGBREF m_refSelectTextColor;
		ARGBREF m_refPushedTextColor;

		ARGBREF m_refNormalSelectColor;
		ARGBREF m_refFocusSelectColor;
		ARGBREF m_refHotColor;

#define TNPI_EXPANDED   0x0001
#define TNPI_CHECKED    0x0002
#define TNPI_TEXT       0x0004
		DWORD m_dwPointIn;
		bool m_bExpanded;//auto false
		RECT m_rcTextPadding;
		int m_nExpandOffset;
	};


#define MTVS_SINGLESELECT  0x0001
	class MYUI_API CTreeViewUI : public CVerticalLayoutUI
	{
	public:
		CTreeViewUI();
		virtual ~CTreeViewUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
		
	public:
		
		void CallNodeEvent(DWORD dwEvent,LPVOID wParam, LPVOID lParam);
		static bool HandleNodeEvent(CControlUI *pControl, DWORD dwEvent,LPVOID wParam, LPVOID lParam);
	protected:
		CControlUI * GetNodeByPoint(POINT &pt);

	protected:
		
		CControlUI * m_pSelectCcontrol;
		CControlUI * m_pHotControl;
	};

}

#endif