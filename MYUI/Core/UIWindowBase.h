
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
			LPCTSTR strClassName, LPCTSTR strWindowText, RECT* pRect = NULL);//��������

		HWND CreateEx(HINSTANCE hInstance, HWND hWndParent, DWORD dwStyleEx, DWORD dwStyle,
			LPCTSTR strClassName, LPCTSTR strWindowText, RECT* pRect = NULL);//��������

		BOOL SetLayeredWindowAttributes(COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

		BOOL Close(LONG nRet = 0);//�첽�رգ��������κεط�����

		//ͬ�������Ѵ��һ�����⣬����ڴ����ڲ����ô˷�����������WindowDestroy�¼���delete thisָ��
		BOOL Destroy();

		BOOL ShowWindow(bool bShow = true, bool bTakeFocus = false);//��ʾ����
		LRESULT ShowModal(bool bShow = true, bool bEnableParent = true);//��ʾģ̬���ڣ�����������
		
		HWND GetHandle();

		void CenterWindow();
		void SetIcon(UINT nRes);

		LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0L);
		LRESULT PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0L);

	protected:
		//void SetCapture(bool bCapture);
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;

	private:
		bool RegisterClass(LPCTSTR strClassName);//ע�ᴰ�� 
		static LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		//�����Ǳ�������

	protected:
		HWND m_hWnd;
		HINSTANCE m_hInstance;
		
		TCHAR m_strWindowText[64];
		TCHAR m_strClassName[64];
	};
}

#endif