
#ifndef _MYUI_GIF_CONTROL_H_
#define _MYUI_GIF_CONTROL_H_

#include "../Core/UIControl.h"
#include<gdiplus.h>
using namespace Gdiplus;

namespace MYUI
{
//显示的位置
#define MGIFS_LEFT      0x0001
#define MGIFS_RIGHT     0x0002
#define MGIFS_CENTER    0x0004
#define MGIFS_VCENTER   0x0008

#define MGIFS_FLAG      0x000F//用来内部判断的，不要用于旗标设置

	class MYUI_API CGifUI : public CControlUI
	{
	public:
		CGifUI();
		~CGifUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual void SetBkImage(LPCTSTR strImage);
		virtual void SetVisible(bool bVisible);
		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
		

		void	PlayGif();//播放gif图片
		void	PauseGif();//暂停不重置
		void	StopGif();//暂停重置
		void	DeleteGif();//析构gif图片
		void	SetAutoPlay(bool bAuto = true );
		bool	IsAutoPlay() const;
		void	SetAutoSize(bool bAuto = true );
		bool	IsAutoSize() const;

	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数
		virtual void OnDetach(HWND hOldWnd);//当控件离开一个窗口时，会调用一次该函数
		
	private:
		Gdiplus::Image*	CGifUI::LoadGif(LPCTSTR strGifPath);

	private:
		Gdiplus::Image	*m_pGifImage;				// gif图片存储
		UINT			m_nFrameCount;				// gif图片总帧数
		UINT			m_nFramePosition;			// 当前放到第几帧
		Gdiplus::PropertyItem*	m_pPropertyItem;	// 帧与帧之间间隔时间

		bool			m_bIsAutoPlay;				// 是否自动播放gif
		bool			m_bIsAutoSize;				// 是否自动根据图片设置大小
		bool			m_bIsPlaying;				// 标记图片是否在放映
		ULONG_PTR		m_gdiplusToken;				// 初始化gdi+函数库暂存句柄
		GdiplusStartupInput		m_gdiplusStartupInput;//gdi+函数库信息

	};
};

#endif