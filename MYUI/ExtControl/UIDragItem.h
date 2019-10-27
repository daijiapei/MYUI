
#ifndef _MYUI_DRAGITEM_CONTROL_H_
#define _MYUI_DRAGITEM_CONTROL_H_

#include "../Core/UIWindow.h"
#include "../Core/UIControl.h"

namespace MYUI
{

	//将控件的屏幕位置传进来，CDragItemUI会根据位置绘制一个画面，然后
	//拖动窗口，鼠标释放会收到通知
	//常规调用模板 :
	//CDragItemUI * pDragItem = new CDragItemUI();
	//pDragItem->Create(hInstance, pWindow, rect, WM_LBUTTONUP, 220, &szRound);
	class MYUI_API CDragItemUI 
	{
	public:
		CDragItemUI();
		virtual ~CDragItemUI();
		static CMuiString g_strClassName;


		//创建窗口, 
		//rcDrag=要绘制的屏幕区域
		//dwFlag 窗口透明度，0 ~ 255, 255等于不透明
		HWND Create(HINSTANCE hInstance, CWindowUI * pPointWnd, RECT &rcDrag, 
			UINT uOverMessage, int nAlpha = 255, SIZE * szRound = NULL);
		BOOL Close(LONG nRet = 0);//异步关闭，可以在任何地方调用
		BOOL Destroy();

		void SetTag(LPVOID pTag);
		LPVOID GetTag() const;

	protected:
		
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	private:
		static LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		bool RegisterClass(HINSTANCE hInstance);

	protected:
		LPVOID m_pTag;
		HWND m_hWnd;
		RECT m_rcDrag;
		BYTE m_nAlpha;

		CWindowUI * m_pPointWnd;
		HBITMAP m_hViewBmp;
		HDC m_hViewDc;
		POINT m_ptMouse;
		UINT m_uOverMessage;//保存一个结束拖动的消息
		SIZE m_szRound;
	};

}

#endif