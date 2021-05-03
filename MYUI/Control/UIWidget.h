
#ifndef __MYUI_WIDGET_CONTROL_H__
#define __MYUI_WIDGET_CONTROL_H__

#include "../Core/UIContainer.h"


//���һ����ʵ�ģ��ᴴ��һ�����ڵĿؼ�
namespace MYUI
{
	class MYUI_API CWidgetUI : public CContainerUI
	{
	public:
		CWidgetUI();
		virtual ~CWidgetUI();
		const static CMuiString g_strClassName;
		const static CMuiString g_strPropName;

		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		//����
		virtual void SetVisible(bool bVisible) override;
		virtual bool SetItem(RECT rcItem, bool bMustUpdate) override;
		virtual bool OnPaint(const RECT& rcUpdate) override;
		virtual void OnScrollBarMove(CScrollBarUI* pSender, int nShift) override;
		virtual SIZE GetContentSize();
	protected:
		
		virtual void OnAttach(HWND hNewWnd) override;//���ؼ����ӵ��µĴ���ʱ�������һ�θú���
		virtual void OnDetach(HWND hOldWnd) override;//���ؼ��뿪һ������ʱ�������һ�θú��� 
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;

		static LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		CMuiString m_strClassName;
		HWND m_hWidget;
		WNDPROC m_pfnSrcWndProc;

	};

}

#endif