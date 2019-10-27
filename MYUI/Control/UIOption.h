
#ifndef _MYUI_OPTION_CONTROL_H_
#define _MYUI_OPTION_CONTROL_H_

#include "UIButton.h"

namespace MYUI
{
	//解释一下COptionUI的作用，简单来说，如果Option不加入(group)分组的话，那么它就
	//相当于一个CheckBox（复选框），如果加入了分组Option就相当于一个radioBox（单选框）
	//复选框的功效是，如果你有十个复选框，那么十个你可以任意选择N个Option
	//单选框的功效是，如果你有十个单选框，那么十个你只能选择其中一个，
	//复选被点击时，一般会收到一个SelectItem通知,(BOOL)wParam表示是否选中
	//单选被点击时，一般会收到两个SelectItem通知，一个是被选中的控件通知
	//一个是取消选中的控件通知，同样是通过同样(BOOL)wParam判断是否选中
	class MYUI_API COptionUI : public CLableUI
	{
	public:
		COptionUI();
		virtual ~COptionUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		
	public:
		void SetGroupName(LPCTSTR strGroup);
		LPCTSTR GetGroupName() const;

		void SetForeUnknowImage(LPCTSTR strImage);
		LPCTSTR GetForeUnknowImage() const;
		void SetForeNormalImage(LPCTSTR strImage);
		LPCTSTR GetForeNormalImage() const;
		void SetForeCheckImage(LPCTSTR strImage);
		LPCTSTR GetForeCheckImage() const;

		bool SetCheck(bool bCheck);
		bool GetCheck();
		
		virtual void SetState(DWORD dwState);
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void PaintStatusImage( const RECT& rcItem, const RECT& rcPaint);
		virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数
		virtual void OnDetach(HWND hOldWnd);//当控件离开一个窗口时，会调用一次该函数 

		bool AttachToGroup(bool bAttach);
	
	protected:
		CMuiString m_strGroupName;
		//Fore，前景图片，先绘制Lable的状态图片，再绘制前景图片
		//前面说过了，Option相当于单选，多选，如果没有选中，那么一般就是显示Normal了，
		//如果选中了，那么就是显示Check，那么UnKnow是什么呢？如果你用过数据库，相信你
		//会明白，BOOL类型其实有三种状态，分别是: FALSE/TRUE/NULL，NULL是数据库的一种
		//特殊状态，这个时候可以用Unknow表示，当然你也可以不用，Unknow一般是初始化的
		//时候设置，以后每次修改STATE_CHECK状态，都会把STATE_UNKNOW去掉
		CMuiString m_strForeUnknowImage;
		CMuiString m_strForeNormalImage;
		CMuiString m_strForeCheckImage;
	};

}

#endif