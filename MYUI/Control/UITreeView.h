
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

#define TINE_DRAWNODEBK    0x0001//绘制背景
#define TINE_SETSELECT     0x0002//设置SELECT状态//(bool)wParam
#define TINE_SETCHECK      0x0003//设置CHECK状态//(bool)wParam
#define TINE_GETNODECOUNT  0x0004//取得节点数量
#define TINE_GETNODEINDEX  0x0005//取得控件索引,如果索引等于节点数量，说明找不到索引
	public:
		//void SetParentControl
		virtual bool OnEvent(DWORD dwEvent, LPVOID wParam, LPVOID lParam)  = 0;
		//virtual void OnDrawNodeBk(const RECT &rcItem, const RECT &rcPaint) = 0;
	};

#define MTNS_READONLY   0x0100//只读
#define MTNS_SHOWCHECK  0x0200//默认不显示check

//AUTO = FALSE =不管有没有子节点，都显示EXPAND符号，
//TRUE = 有子节点的时候，EXPAND符号才会显示
#define MTNS_HIDEEXPAND 0x0400
#define MTNS_CHECKCUR   0x0800//只能够选择当前项

/***************************************************************************
 * CTreeNodeUI 设计格式
 * 注意：我们在SetItem的时候不设置Expand偏移，在OnPaint的时候再设置偏移
 * 如果在SetItem中设置Expand偏移，点击空旷的Expand区域时，
 * FindControlByPoint 将找不到匹配的控件
 *    _________________________________
 *    | > | √ |  text                |  = CTreeNodeUI/CTreeNodeReinUI
 *    - E -----------------------------
 *    | x |   子节点 1                |  = CTreeNodeUI::m_Nodes[0]
 *    | p -----------------------------
 *    | a |   子节点 2                |  = CTreeNodeUI::m_Nodes[1]
 *    | n -----------------------------
 *    | d |   子节点 3                |  = CTreeNodeUI::m_Nodes[2]
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

		virtual bool Update();//我会更新父布局的区域，所以兄弟控件也会调整
        virtual bool Renewal();//我会将子窗口的位置都更新一次，并刷新视图
		virtual bool Invalidate();//我只更新自己的区域，所以其他控件不会受到影响

		virtual CControlUI * FindControlByPoint(POINT &pt);
		

		//CNodeInterface
		virtual void OnDrawNodeBk(const RECT &rcItem, const RECT &rcPaint);
		virtual bool OnEvent(DWORD dwEvent, LPVOID wParam, LPVOID lParam);
	public:

		//CControlUI * GetNextNode() const;//下一个
		//CControlUI * GetPreNode() const;//上一个
		//CControlUI * GetFirstNode() const;//第一个
		//CControlUI * GetLastNode() const;//最后一个

		//CControlUI * GetContainer() const;

		//CTreeNodeUI * GetRootNode() const;//CTreeViewUI的第一个节点
		//CTreeNodeUI * GetParentNode() const;//ParentControl是CTreeViewUI，ParentNode是CTreeNodeUI
		//CTreeNodeUI * GetChildNode() const;//第一个子节点，是第一个！

		//CTreeNodeUI * GetSelectNode() const;//当前选择项

		//void SetReinHeight(int nHeight);

		void SetTextPadding(const RECT &rect);
		const RECT &GetTextPadding() const;
		void SetExpandOffset(int nOffset);
		int GetExpandOffset() const;
		void SetExpanded(bool bExpand);
		bool GetExpanded() const;//节点是否展开
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
		//因为一按下Expand就切换了，所以Expand不存在pushed状态
		//鼠标在Expand按下(DOWN)，切换Expand状态，在文本双击，也切换Expand状态
		CMuiString m_strExpandNormalImage;
		CMuiString m_strExpandHotImage;
		CMuiString m_strUnExpandNormalImage;
		CMuiString m_strUnExpandHotImage;

		//VS资源管理 |expand|文件图标| 节点文字 |
		//这里提醒一下，如果想做到VS的解决方案的资源管理器的效果，那应该怎么弄呢？
		//其实就是将checkImage设置成文件图标，然后控件状态为只读就可以了
		//如果不设置只读的话，点击check，只会设置check的状态，不会展开节点

		//默认可写：check单击(CLICK)切换check状态，
		//只读，双击切换Expand状态，效果跟文本一样
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