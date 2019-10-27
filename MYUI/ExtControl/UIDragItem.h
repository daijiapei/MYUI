
#ifndef _MYUI_DRAGITEM_CONTROL_H_
#define _MYUI_DRAGITEM_CONTROL_H_

#include "../Core/UIWindow.h"
#include "../Core/UIControl.h"

namespace MYUI
{

	//���ؼ�����Ļλ�ô�������CDragItemUI�����λ�û���һ�����棬Ȼ��
	//�϶����ڣ�����ͷŻ��յ�֪ͨ
	//�������ģ�� :
	//CDragItemUI * pDragItem = new CDragItemUI();
	//pDragItem->Create(hInstance, pWindow, rect, WM_LBUTTONUP, 220, &szRound);
	class MYUI_API CDragItemUI 
	{
	public:
		CDragItemUI();
		virtual ~CDragItemUI();
		static CMuiString g_strClassName;


		//��������, 
		//rcDrag=Ҫ���Ƶ���Ļ����
		//dwFlag ����͸���ȣ�0 ~ 255, 255���ڲ�͸��
		HWND Create(HINSTANCE hInstance, CWindowUI * pPointWnd, RECT &rcDrag, 
			UINT uOverMessage, int nAlpha = 255, SIZE * szRound = NULL);
		BOOL Close(LONG nRet = 0);//�첽�رգ��������κεط�����
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
		UINT m_uOverMessage;//����һ�������϶�����Ϣ
		SIZE m_szRound;
	};

}

#endif