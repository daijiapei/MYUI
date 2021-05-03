
#ifndef __MYUI_WINDOW_BASE_H__
#define __MYUI_WINDOW_BASE_H__

#include "../base.h"

namespace MYUI
{

	class MYUI_API CWindowBase
	{
	public:
		CWindowBase();
		virtual ~CWindowBase();
		operator HWND() const;

		HWND Create(HINSTANCE hInstance, HWND hWndParent, DWORD dwStyle,
			LPCTSTR strClassName, LPCTSTR strWindowText, RECT* pRect = NULL);//创建窗口

		HWND CreateEx(HINSTANCE hInstance, HWND hWndParent, DWORD dwStyleEx, DWORD dwStyle,
			LPCTSTR strClassName, LPCTSTR strWindowText, RECT* pRect = NULL);//创建窗口

		BOOL SetLayeredWindowAttributes(COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

		BOOL Close(LONG nRet = 0);//异步关闭，可以在任何地方调用

		//同步，提醒大家一个问题，如果在窗口内部调用此方法，则不能再WindowDestroy事件中delete this指针
		BOOL Destroy();

		BOOL ShowWindow(bool bShow = true, bool bTakeFocus = false);//显示窗口
		LRESULT ShowModal(bool bShow = true, bool bEnableParent = true);//显示模态窗口，即阻塞窗口
		
		HWND GetHandle();

		void CenterWindow();
		void SetIcon(UINT nRes);

		LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0L);
		LRESULT PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0L);

	protected:
		//void SetCapture(bool bCapture);
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;

	private:
		bool RegisterClass(LPCTSTR strClassName);//注册窗口 
		static LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		//下面是变量定义

	protected:
		HWND m_hWnd;
		HINSTANCE m_hInstance;
		
		TCHAR m_strWindowText[64];
		TCHAR m_strClassName[64];
	};
}

#endif