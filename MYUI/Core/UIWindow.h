
#ifndef _MYUI_WINDOW_H_
#define _MYUI_WINDOW_H_

#include "UIContainer.h"
#include "UIInterface.h"
#include "UIWindowBase.h"

namespace MYUI
{

    typedef struct __MUIVIEW_INFO
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

	}MUIVIEWINFO, *LPMUIVIEWINFO;

	//注意：
	//1. 继承CWindowUI的类必须用new命令构建对象
	//2. 窗口收到WM_NCDESTROY[窗口销毁]后，会自动delete CWindowUI，无需用户手动回收
	//   所以请不要对CWindowUI和它的子类执行delete操作
	class MYUI_API CWindowUI : public CWindowBase, public INotify, public IControlNotify, public IWindowEvent
	{
        friend class CBuilder;
	public:
		CWindowUI();
		virtual ~CWindowUI();

        virtual LPVOID GetInterface(LPCTSTR strName);

        BOOL UpdateLayeredWindow(COLORREF refKey, DWORD dwFlags = ULW_ALPHA);

        LRESULT Popup(RECT rcPos);

        virtual void SendNotify(MUINOTIFY &notify);

		void SetMinSize(SIZE size);
		const SIZE &GetMinSize() const;
		void SetMaxSize(SIZE size);
		const SIZE &GetMaxSize() const;
		void SetTextFont(int FontId);
		int GetTextFont() const;
		void SetHoverTime(DWORD dwHoverTime);
		DWORD GetHoverTimer() const;


		//每个窗口有且只能有一个主布局
		void AttachFrameView(CControlUI * pControl);
		CControlUI * GetFrameView() const;

        //语言文件
        void SetLanguageFile(LPCTSTR strFile);
        LPCTSTR GetLanguageFile() const;

		bool SetSkin(LPCTSTR strSkin);
		LPCTSTR GetSkin() const;

		bool AddFont(int Fontid, HFONT hFont);
		HFONT GetFont(int Fontid);

		bool AddGroup(LPCTSTR strGroup);

		void SetCaption(CControlUI * pControl);
		CControlUI * GetCaption() const;
		CControlUI * GetFocusControl();

		CControlUI* FindControl(LPCTSTR strName);
		CControlUI* FindControl(POINT& Point);

		CControlUI* GetRootControl();

		bool SetHost(CWindowUI * pHost);
		

        BOOL PostBreakMessage(BOOL bCheck, CControlUI * pParent);
	protected:
		//void SetCapture(bool bCapture);
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
        
	private:
		bool AddParasite(CWindowUI* pParasite);
		bool RemoveParasite(CWindowUI* pParasite);
		LRESULT MouseProc(UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT KeyBoardProc(UINT message, WPARAM wParam, LPARAM lParam);
		//下面是变量定义

	private:
		int m_nFontId;
		DWORD m_dwHoverTime;

		CMuiStringPtrMap m_GroupArray;//用来记录控件的分组名称
		MUISHAREINFO * m_pShareInfo;
		MUIVIEWINFO* m_pViewInfo;
		BOOL m_bShowInScreen;
        CMuiPtrArray m_MenuArray;
		CMuiString m_strLanguageFile;
		CWindowUI* m_pHost;
		CMuiPtrArray m_Parasite;
	};
}

#endif