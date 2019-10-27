
#ifndef _MYUI_CONTROL_H_
#define _MYUI_CONTROL_H_

#include "UIBase.h"
#include "../Render/RenderEngine.h"
#include "UIInterface.h"
#include "../ExtControl/UIToolTip.h"

namespace MYUI
{

    template< class T >
    class CSafeRelease
    {
    public:
        CSafeRelease() : m_p(NULL){};
        CSafeRelease(T* p) : m_p(p) { };
        ~CSafeRelease() { if (m_p != NULL) m_p->Release(); };
        T* Attach(T* p) { m_p = p; return m_p; };
        T* Detach() { T* t = m_p; m_p = NULL; return t; };
    private:
        T* m_p;
    };

#define STATE_DEFAULT    0x0000//默认
#define STATE_SELECT     0x0001//选中
#define STATE_HOT        0x0002//热点
#define STATE_FOCUS      0x0004//焦点
#define STATE_PUSHED     0x0008//按下
#define STATE_CHECKED    0x0010//选中
#define STATE_UNKNOW     0x0080//未确定

	//该结构体CWindowUI设置，提供给所有控件共享
	class CControlUI;

    typedef struct __SHAREINFO
	{
		HWND hWnd;
		HDC  hPaintDC;
		CRenderEngine * pRenderEngine;//渲染引擎
		
//一旦使用了某种模式，就不能更改，所以要么是同步模式，要么宿主模式(作为申请和释放资源指针的主窗口)
//TRUE = 宿主模式，不需要释放资源，FALSE = 使用同步资源信息，不需要释放资源指针，由宿主释放
		BOOL bHostType;
		TCHAR * strSkinFolder;//皮肤
		CMuiIdArray * FontArray;//字体数组
		INotify * pNotify;
		CToolTipUI * pToolTip;
	}TSHAREINFO, * PSHAREINFO;

#define GETHWND(_this)  ((_this)->m_pShareInfo ? (_this)->m_pShareInfo->hWnd : NULL)

	class MYUI_API CControlUI :  public CItemViewInfo , public CItemPosition 
	{
	public:
		CControlUI();
		virtual ~CControlUI();

		bool IsControl();
		//设置共享信息，共享信息由窗口传递给始祖控件，始祖控件再递归传给每一个子控件
		//共享信息就是CControlUI和CWindowUI通信的桥梁，可作为一个信息管理器
		virtual void SetShareInfo(TSHAREINFO * pShareInfo);
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		virtual CMuiString GetClassName() const;
		
		//有关这个CallWndProc/WndProc/SendMessage/PostMessage我跟大家说一下，
		//这四个函数我是写到一半的时候折腾出来，所以很多操作没放WndProc里面
		//比如SetText()函数，应该将消息投递到WndProc(WM_SETTEXT)里面处理的。
		//但是因为已经写到一半了，这样工作量太大，所以偶尔有空才会整理这些函数
		//目前到这里，这个界面库已经高度模仿了WIN32窗口和控件，如果大家有精力
		//和兴趣的话，可以自己整理一下。我写这套界面的目的，只是为了给大家整理
		//一套思路，至于细节的问题，还需要大家自己调整一下，当然，如果我的项目
		//用到某个功能的话，我也会进行测试和调整
		
		//先调用Hook，再调用WndProc
		virtual LRESULT CallWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
		virtual CControlUI* GetParent() const;
		virtual VOID SetParent(CControlUI* pParentContrl);
		virtual CControlUI * GetAncestorControl();//始祖控件，处于最顶层的父控件
		HWND GetWindow() const;
		bool SendNotify(BOOL bChild, EnumNotifyMsg dwType, WPARAM wParam = 0L, LPARAM lParam = NULL);
		LRESULT SendMessage(UINT message, WPARAM wParam = 0L, LPARAM lParam = NULL);
		LRESULT PostMessage(UINT message, WPARAM wParam = 0L, LPARAM lParam = NULL);
		BOOL SetTimer(UINT nIDEvnet, UINT uElapse);
		BOOL KillTimer(UINT nIDEvnet);
        void SetMenu(IMenuPopup * pMenu);
        IMenuPopup * GetMenu() const;

		BOOL SetCapture();
		BOOL ReleaseCapture();
		BOOL SetHooker(IControlHooker * pHooker, int index = -1);
		BOOL UnHooker(IControlHooker * pHooker);
		IControlHooker * UnHooker(int index);
	public:
		//绘制
		//因为是布局总是相对的，一个控件的更改，可能会影响到兄弟控件的布局
		//所以Update函数是调用父窗口的SetItem和Invalidate(),这样就能连带调整兄弟控件的布局
		virtual bool Update();//我会更新父布局的区域，所以兄弟控件也会调整
        virtual bool Renewal();//我会将子窗口的位置都更新一次，并刷新视图
		virtual bool Invalidate();//我只更新自己的区域，所以其他控件不会受到影响
		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);

		virtual void SetVisible(bool bVisible);
		virtual bool IsVisible();
		virtual void SetEnabled(bool bEnabled);
		virtual bool IsEnabled();
		virtual void SetPenetrated(bool bPenetrated);
		virtual bool IsPenetrated();

		virtual CControlUI * FindControlByPoint(POINT &pt);
		virtual CControlUI * FindControlByName(LPCTSTR strName);

		//一些高级控件将内部的控件隐藏起来了，使用FindControl并不能找到他们的内部控件指针
		//所以要用FindSubControl进行查找，FindSubControl默认返回NULL，高级控件中都
		//额外为FindSubControl提供了不同的方法
		virtual CControlUI * FindSubControlByPoint(POINT &pt);
		virtual CControlUI * FindSubControlByName(LPCTSTR strName);

		//GetItemDisplayRect和GetItemFixedRect，都是通过客户区跳转，得出
		//控件最终显示的位置，也就是说，只有控件处于父控件的客户区，这个
		//函数最终才能正确执行，有些特殊的控件，他们是在m_rcRawRect区域
		//进行绘制的，那么这个函数最终不能取得正确的显示区域
		virtual bool GetItemDisplayRect(RECT &rcDisplay);
		virtual bool GetClientDisplayRect(RECT &rcDisplay);

		//控件在窗口中的绝对位置，跟GetDisplayRect不同，如果控件发生遮挡GetDisplayRect会减去遮挡部分
		//而GetFixedRect会只计算滚动条偏移与父控件是否隐藏，并不计算遮挡部分，
		//所以只有父控件或自身隐藏的时候，才会返回失败，否则返回控件在窗口中的绝对位置
		virtual bool GetItemFixedRect(RECT &rcFixed);
		virtual bool GetClientFixedRect(RECT &rcFixed);

		virtual CMuiString GetName() const;
		virtual void SetName(LPCTSTR pstrName);
		virtual bool SetFocus();
		virtual bool IsFocued();

		//状态
		virtual void SetState(DWORD dwState);
		virtual void AddState(DWORD dwState);
		virtual void RemoveState(DWORD dwState);
		virtual DWORD GetState() const;

		//
		void SetTag(UINT_PTR pTag);
		UINT_PTR GetTag() const;
		void SetTipText(LPCTSTR strTipText);
		LPCTSTR GetTipText() const;
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		//控件在窗口中的绝对位置，跟GetDisplayRect不同，如果控件发生遮挡GetDisplayRect会减去遮挡部分
		//而GetFixedRect会只计算滚动条偏移与父控件是否隐藏，并不计算遮挡部分，
		//所以只有父控件或自身隐藏的时候，才会返回失败，否则返回控件在窗口中的绝对位置
		virtual bool GetFixedRect(RECT &rcFixed, const RECT &rc);

		//控件在窗口中显示的位置，有显示位置则返回true
		//注意GetDisplayRect只考虑父控件是否遮挡了它，没有考虑兄弟控件和外层控件是否遮挡了它
		//如果兄弟控件或外层控件间发生重叠，GetDisplayRect取得的区域，是没有将这些情况计算进去的
		//但大部分情况下，即使发生了重叠，我们调用GetDisplayRect也能满足我们的需求
		//如果需要更加准确的显示位置信息，请调用GetDisplayRectEx
		virtual bool GetDisplayRect(RECT &rcDisplay,const RECT &rc);

		//GetDisplayRectEx将兄弟控件的遮挡区域也计算了进去，
		//当然，GetDisplayRectEx 比 GetDisplayRect 更消耗效率，
		//因为GetDisplayRect只递归父控件，GetDisplayRectEx将递归所有的同级和上层控件
		//virtual bool GetDisplayRectEx(RECT &rcDisplay);

		//绘制
		virtual void PaintBkColor(const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintBkImage(const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintStatusImage( const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintText(const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintBorder(const RECT& rcItem, const RECT& rcPaint);
		
		//通知
		virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数
		virtual void OnDetach(HWND hOldWnd);//当控件离开一个窗口时，会调用一次该函数 
		virtual void OnPosChange(LPVOID pElement);//当位置属性发生变化
		virtual void OnViewChange(LPVOID pElement);//当界面属性发生变化
		
	private:
		
	protected:
		TSHAREINFO * m_pShareInfo;
		CMuiString m_strToolTip;
		UINT_PTR m_pTag;
        IMenuPopup * m_pMenu;//子菜单句柄，如果该句柄不为空，那么表明该元素为父菜单
		CControlUI * m_pParentContrl;//父控件

		CMuiString m_strName;
		//CMuiString m_strClassName;
		bool m_bControl;//true=这个一个控件， false=不是控件（可能是布局)
		bool m_bEnabled;//是否启用
		bool m_bVisible;//是否显示
		bool m_bPenetrated;//鼠标点击时，直接穿过，或点击到后面的控件

		DWORD m_dwState;
		CMuiPtrArray m_pHookers;

		RECT m_rcSizeBox;

	private:
		

	};

	//用来添加子项的接口
	class MYUI_API IControlArray
	{
	public:
		virtual bool Add(CControlUI* pControl) = 0;
		virtual bool AddAt(CControlUI* pControl, int nIndex) = 0;
		virtual bool Remove(CControlUI* pControl) = 0;
		virtual bool RemoveAt(int nIndex) = 0;
		virtual void RemoveAll() = 0;
		virtual CControlUI * Find(int nIndex) = 0;
		virtual int Find(CControlUI * pControl) = 0;
		virtual int GetCount() const = 0;
	};

}


#endif