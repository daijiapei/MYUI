
#ifndef _MYUI_LABLE_CONTROL_H_
#define _MYUI_LABLE_CONTROL_H_

#include "..\\Core\\UIControl.h"

namespace MYUI
{
//Lable控件的style，如果继承自Lable的控件没有特别说明
//则Lable的子类依旧可以使用这些属性

#define MLBES_LEFT     0x00000001 //文本靠左
#define MLBES_RIGHT    0x00000002 //文本靠右
#define MLBES_CENTER   0X00000004 //水平居中
#define MLBES_VCENTER  0x00000008 //垂直居中


	class MYUI_API CLableUI : public CControlUI
	{
	public:
		CLableUI();
		virtual ~CLableUI();
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		
		void SetTextPadding(const RECT &rect);
		const RECT &GetTextPadding() const;

		//绘制属性
		void SetNormalColor(ARGBREF argb);
		ARGBREF GetNormalColor();
		void SetHotColor(ARGBREF argb);
		ARGBREF GetHotColor();
		void SetSelectColor(ARGBREF argb);
		ARGBREF GetSelectColor();
		void SetPushedColor(ARGBREF argb);
		ARGBREF GetPushedColor();
		void SetFocusedColor(ARGBREF argb);
		ARGBREF GetFocusedColor();

		void SetNormalImage(LPCTSTR strImage);
		LPCTSTR GetNormalImage() const;
		void SetHotImage(LPCTSTR strImage);
		LPCTSTR GetHotImage() const;
		void SetSelectImage(LPCTSTR strImage);
		LPCTSTR GetSelectImage() const;
		void SetPushedImage(LPCTSTR strImage);
		LPCTSTR GetPushedImage() const;
		void SetFocusedImage(LPCTSTR strImage);
		LPCTSTR GetFocusedImage() const;

		void SetHotTextColor(ARGBREF argb);
		ARGBREF GetHotTextColor();
		void SetSelectTextColor(ARGBREF argb);
		ARGBREF GetSelectTextColor();
		void SetPushedTextColor(ARGBREF argb);
		ARGBREF GetPushedTextColor();
		void SetFocusedTextColor(ARGBREF argb);
		ARGBREF GetFocusedTextColor();

		int GetHotBorderSize();
		void SetHotBorderSize(int size);
		ARGBREF GetHotBorderColor() const;
		void SetHotBorderColor(ARGBREF refColor);

        int GetFocusedBorderSize();
		void SetFocusedBorderSize(int size);
		ARGBREF GetFocusedBorderColor() const;
		void SetFocusedBorderColor(ARGBREF refColor);

	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;
		virtual void PaintStatusImage( const RECT& rcUpdate) override;
		virtual void PaintText(const RECT& rcUpdate) override;
		virtual void PaintBorder(const RECT& rcUpdate) override;
		virtual void SetState(DWORD dwState);
	protected:
		
		//图片的路径 + 图片参数
		CMuiString m_strNormalImage;//普通状态
		CMuiString m_strHotImage;//鼠标经过控件状态
		CMuiString m_strSelectImage;//选中状态，
		CMuiString m_strPushedImage;//忙碌状态，比如正在点击控件
		CMuiString m_strFocusedImage;//焦点状态

		ARGBREF m_refNormalColor;
		ARGBREF m_refHotColor;
		ARGBREF m_refSelectColor;
		ARGBREF m_refPushedColor;
		ARGBREF m_refFocusedColor;

		ARGBREF m_refHotTextColor;
		ARGBREF m_refSelectTextColor;
		ARGBREF m_refPushedTextColor;
		ARGBREF m_refFocusedTextColor;

		RECT m_rcTextPadding;//文字内缩进

		int m_nHotBorderSize;
		ARGBREF m_refHotBorderColor;

        int m_nFocusedBorderSize;
        ARGBREF m_refFocusedBorderColor;
	private:

	};

}

#endif