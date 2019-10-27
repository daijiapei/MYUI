
#ifndef _MYUI_CONTAINER_H_
#define _MYUI_CONTAINER_H_

#include "UIInterface.h"
#include "..//Control//UIScrollBar.h"
#include "..//Control//UILable.h"

namespace MYUI
{
	class MYUI_API CContainerUI : public CLableUI, public IScrollBarMove
	{
	public:
		CContainerUI();
		virtual ~CContainerUI();
		virtual VOID SetShareInfo(TSHAREINFO * pShareInfo);
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		
	public:
		//����
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);

		virtual CControlUI * FindControlByPoint(POINT &pt);
		virtual void OnScrollBarMove(LPCVOID pSender, int nShift);
		virtual const SIZE &GetScrollBarShift() const;
		//������
		//ȡ����Ҫ��ʾ�������С�ߴ磬�ٶԱȴ�ӡ�ĳߴ磬��֪���Ƿ�Ҫʹ�ù�����
		virtual SIZE GetContentSize() = 0;

		virtual bool SetHorizontalScrollBarActive(bool bActive);//�Ƿ�һֱ��ʾ������
		virtual bool GetHorizontalScrollBarActive();
		virtual void SetHorizontalScrollBar(CScrollBarUI * pScrollBar);
		virtual CScrollBarUI * GetHorizontalScrollBar() const;

		virtual bool SetVerticalScrollBarActive(bool bActive);//�Ƿ�һֱ��ʾ������
		virtual bool GetVerticalScrollBarActive();
		virtual void SetVerticalScrollBar(CScrollBarUI * pScrollBar);
		virtual CScrollBarUI * GetVerticalScrollBar() const;
		
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void PaintText(const RECT& rcItem, const RECT& rcPaint);

	protected:

		CScrollBarUI * m_pVerticalScrollBar;//��ֱ������
		CScrollBarUI * m_pHorizontalScrollBar;//ˮƽ������
		SIZE m_szContent;//�������ߴ�
		RECT m_rcContent;//������
		SIZE m_szScrollOffset;
		
	private:

	};
}



#endif