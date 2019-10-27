
#ifndef _MYUI_WINDOW_H_
#define _MYUI_WINDOW_H_

#include "UIContainer.h"
#include "UIInterface.h"

namespace MYUI
{

    typedef struct __VIEWINFO
	{
		BOOL bFocusWnd;//窗口拥有焦点
		CControlUI * pRootControl;//整个窗口，主要就是绘制这个控件/布局
		CControlUI * pHotControl;//鼠标经过, 窗口内部已默认处理
		CControlUI * pFocusControl;//输入焦点, 负责捕捉键盘消息,调用CControlUI::SetFocus函数设置
		CControlUI * pCaptionControl;//标题栏
		CControlUI * pPushedControl;//鼠标按下未弹起,负责捕捉鼠标消息，控件内部已默认处理

		//因为没有使用系统的加速键功能，所以使用Ctrl + 字母（快捷键）时，会产生一个
		//多余的WM_CHAR字符，所以当WM_KEYDOWN时，将字符保存下来，在WM_CHAR中判断字符
		//是否相等，如果相等，则返回。以此避免多余的字符

		TCHAR tSurplus;
		POINT ptLeftMouse;

		SIZE SizeMin;
		SIZE SizeMax;

	}VIEWINFO, *PVIEWINFO;

	//注意：
	//1. 继承CWindowUI的类必须用new命令构建对象
	//2. 窗口收到WM_NCDESTROY[窗口销毁]后，会自动delete CWindowUI，无需用户手动回收
	//   所以请不要对CWindowUI和它的子类执行delete操作
	class MYUI_API CWindowUI : public INotify, public IControlNotify, public IWindowEvent
	{
        friend class CBuilder;
	public:
		CWindowUI();
		virtual ~CWindowUI();

        virtual LPVOID GetInterface(LPCTSTR strName);
		operator HWND() const;

		HWND Create(HINSTANCE hInstance,HWND hWndParent, DWORD dwStyle, 
			LPCTSTR strClassName ,LPCTSTR strWindowText, RECT * pRect = NULL);//创建窗口
		BOOL Close(LONG nRet = 0);//异步关闭，可以在任何地方调用

		//同步，提醒大家一个问题，如果在窗口内部调用此方法，则不能再WindowDestroy事件中delete this指针
		BOOL Destroy();
		
		void ShowWindow(bool bShow = true, bool bTakeFocus = false);//显示窗口
        LRESULT ShowModal(bool bShow = true, bool bEnableParent = true, MSG * pMsg = NULL);//显示模态窗口，即阻塞窗口
		HWND GetHandle();

		void CenterWindow();
		void SetIcon(UINT nRes);

        virtual void SendNotify(TNOTIFYUI &notify);
		LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0L);
		LRESULT PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0L);
		void SetMinSize(SIZE size);
		const SIZE &GetMinSize() const;
		void SetMaxSize(SIZE size);
		const SIZE &GetMaxSize() const;
		void SetTextFont(int FontId);
		int GetTextFont() const;
		void SetHoverTime(DWORD dwHoverTime);
		DWORD GetHoverTimer() const;

		CMuiString GetClassName();
		CMuiString GetWindowText();
		
		//每个窗口有且只能有一个主布局
		void AttachFrameView(CControlUI * pControl);
		CControlUI * GetFrameView() const;

        //语言文件
        void SetLanguageFile(LPCTSTR strFile);
        LPCTSTR GetLanguageFile() const;

		//与宿主窗口使用同一资源
        bool SetSyncResource(CWindowUI *pHostWindow);

		bool SetSkin(LPCTSTR strSkin);
		LPCTSTR GetSkin() const;

		bool AddFont(int Fontid, HFONT hFont);
		HFONT GetFont(int Fontid);

		bool AddGroup(LPCTSTR strGroup);

		void SetCaption(CControlUI * pControl);
		CControlUI * GetCaption() const;
		CControlUI * GetFocusControl();
	protected:
		//void SetCapture(bool bCapture);
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	private:
		bool RegisterClass();//注册窗口 
		LRESULT MouseProc(UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT KeyBoardProc(UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		//下面是变量定义
        

	protected:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		int m_nFontId;
		DWORD m_dwHoverTime;
		CMuiString m_strClassName;
		CMuiString m_strWindowText;

		CMuiStringPtrMap m_GroupArray;//用来记录控件的分组名称
		CAvlTree m_atrControlTimer;
		TSHAREINFO * m_pShareInfo;
		VIEWINFO * m_pViewInfo;
		BOOL m_bShowInScreen;
        CMuiPtrArray m_MenuArray;
        CMuiString m_strLanguageFile;
	};
}

#endif