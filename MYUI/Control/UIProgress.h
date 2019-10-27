
#ifndef _MYUI_PROGRESS_CONTROL_H_
#define _MYUI_PROGRESS_CONTROL_H_

#include "UILable.h"

namespace MYUI
{

#define MPGS_HORIZONTAL 0x0000
#define MPGS_VERTICAL   0x0001


	class MYUI_API CProgressUI : public CLableUI
	{
	public:
		CProgressUI();
		virtual ~CProgressUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		bool IsHorizontal();//是否水平显示进度条（默认=true)
		void SetHorizontal(bool bHorizontal = true);

		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		int GetValue() const;
		void SetValue(int nValue);
		
		//绘制
		virtual void PaintStatusImage( const RECT& rcItem, const RECT& rcPaint);

	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	protected:
		
		int m_nMinValue;
		int m_nMaxValue;
		int m_nValue;

	private:

	};

}

#endif