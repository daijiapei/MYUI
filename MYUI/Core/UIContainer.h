
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
		virtual VOID SetShareInfo(MUISHAREINFO * pShareInfo);
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		virtual LPVOID GetInterface(LPCTSTR strName) override;

	public:
		//绘制
		virtual bool SetItem(RECT rcItem, bool bMustUpdate) override;
		virtual bool OnPaint(const RECT& rcUpdate) override;

		virtual CControlUI * FindControlByPoint(POINT &pt);
		virtual void OnScrollBarMove(CScrollBarUI* pSender, int nShift);
		virtual const SIZE &GetScrollBarShift() const;
		//滚动条
		//取得需要显示必须的最小尺寸，再对比打印的尺寸，就知道是否要使用滚动条
		virtual SIZE GetContentSize() = 0;

		virtual bool SetHorizontalScrollBarActive(bool bActive);//是否一直显示滚动条
		virtual bool GetHorizontalScrollBarActive();
		virtual void SetHorizontalScrollBar(CScrollBarUI * pScrollBar);
		virtual CScrollBarUI * GetHorizontalScrollBar() const;

		virtual bool SetVerticalScrollBarActive(bool bActive);//是否一直显示滚动条
		virtual bool GetVerticalScrollBarActive();
		virtual void SetVerticalScrollBar(CScrollBarUI * pScrollBar);
		virtual CScrollBarUI * GetVerticalScrollBar() const;
		
	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;
		virtual void PaintText(const RECT& rcUpdate) override;
		virtual void PaintContent(const RECT& rcUpdate);

	protected:

		CScrollBarUI * m_pVerticalScrollBar;//垂直滚动条
		CScrollBarUI * m_pHorizontalScrollBar;//水平滚动条
		SIZE m_szContent;//内容区尺寸
		RECT m_rcContent;//内容区
		SIZE m_szScrollOffset;
		
	private:

	};
}



#endif