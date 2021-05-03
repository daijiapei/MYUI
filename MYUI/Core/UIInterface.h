
#ifndef __MYUI_INTERFACE_H__
#define __MYUI_INTERFACE_H__

#include "../base.h"

namespace MYUI
{
	class CControlUI;

	enum
	{
		WMU_MOUSEENTER = WM_USER + 1,
		WMU_POPUPDIALOG,
		WMU_POPUPMENU,
		WMU_BREAKLOOP,
		//break loop wparam flag
#define BLWF_UNCHECK         0x00 //不需要进行任何检查，直接退出
#define BLWF_CHECKFOCUS      0x01 //退出前检查父子控件是否拥有焦点

		WMU_LBUTTONCLICK,
		WMU_MBUTTONCLICK,
		WMU_RBUTTONCLICK,


		WMU_MYUI = WM_USER + 128,
		WMU_USER = WM_APP,
	};







#define WM_MYUIMSG			(WM_USER + 128)//窗口与控件同用
#define WM_MYCONTROLMSG     (WM_USER + 256)//控件私有
#define WM_MYCM             WM_MYCONTROLMSG




/*WM_REQUESTINFO user WPARAM return InfoPtr*/
#define WM_REQUESTINFO      (WM_MYUIMSG + 5)
#define MRQF_SHAREINFO   0x01
#define MRQF_VIEWINFO    0x02
#define MRQF_GETFOCUS    0x03
/*WM_REQUESTINFO*/



/*WM_XETSTATE user WPARAM return DWORD or BOOL*/
#define WM_SETSTATE          (WM_MYUIMSG + 23)
#define WM_GETSTATE          (WM_MYUIMSG + 24)
//wParam使用 STATE_XXXXXX，返回BOOL，若wParam = NULL, 返回所有状态
/*WM_XETSTATE user WPARAM return BOOL*/


//WM_MYCM 控件私有
#define WMU_GETGROUP           (WM_MYCM + 1)
#define WMU_SETCHECK           (WM_MYCM + 2)
#define WMU_GETCHECK           (WM_MYCM + 3)
#define WMU_NOTIFY             (WM_MYCM + 4)

//进度条以及它的派生控件
#define WMU_SETVALUE           (WM_MYCM + 30)
#define WMU_GETVALUE           (WM_MYCM + 31)
#define WMU_SETMAXVALUE        (WM_MYCM + 32)
#define WMU_GETMAXVALUE        (WM_MYCM + 33)
#define WMU_SETMINVALUE        (WM_MYCM + 34)
#define WMU_GETMINVALUE        (WM_MYCM + 35)


	typedef struct __MUICARET_INFO
	{
		HBITMAP Bitmap;
		ARGBREF Color;
		POINT Point;
		SIZE Size;
		BYTE Visible;
		BYTE Showing;
	} MUICARETINFO, * LPMUICARETINFO;

	//光标
	class MYUI_API CUICaret
	{
	public:
		CUICaret()
		{
			memset(&m_CaretInfo, 0, sizeof(MUICARETINFO));
		}
		virtual ~CUICaret()
		{
			if (m_CaretInfo.Bitmap)
			{
				::DeleteObject(m_CaretInfo.Bitmap);
			}
		}

	protected:
		virtual void OnDrawCaret(const RECT& rcUpdate) = 0;
	protected:
		MUICARETINFO m_CaretInfo;
	};

	enum EnumNotify
	{
		//处理消息后，如果返回false，表明不需要调用OnNotify
		NonMessage = 0,
		ClickItem,//控件单击消息
		DbClickItem,//控件双击消息
		SelectChange,//滚动条

		CheckItem, //用法跟SelectItem一样，主要是STATE_CHECK状态会配合特殊标志，如：STATE_UNKNOW
        
		//存在多项的控件，选择(左击或右击)了某一项，wParam是控件的状态，
		//通过判断wParam，来判断选中还是取消选中
		SelectItem,
		ActiveItem,//存在多项的控件，左双击了某一项
		TextChange,//文本发生了改变，一般关联编辑框
		SetFocus,//控件获得焦点
		KillFocus,//控件失去焦点
		TimerCall,//消息通知

		ShowTip,//展示tip
	};

    

    typedef struct __MUINOTIFY
	{
		CControlUI* pSender;
		
		EnumNotify   dwType;
		WPARAM      wParam;
		LPARAM      lParam;
		DWORD       dwTime;
		POINT       ptMouse;
	}MUINOTIFY;

	class INotify
	{
	public:
		//这个接口用来过滤notify，过滤完成后，再调用 IControlNotify
        virtual void SendNotify(MUINOTIFY& Notify) = 0;
	};

	class IControlNotify
	{
	public:
        virtual void OnNotify(MUINOTIFY& Notify) = 0;
	};

    class IMenuPopup
    {
    public:
        //往hNotifyWnd发消息, 鼠标在屏幕中的Point位置
        virtual LRESULT Popup(INotify *pNotify, POINT ptPopup) = 0;
        virtual SIZE GetSize() = 0;
    };

    class IDialogPopup
    {
    public:
        virtual LRESULT Popup(LPARAM lParam) = 0;
    };

	enum EnumEvent
	{
		WindowInit = 0,//WM_NCCREATE
		RequestRenderEngine,//WM_CREATE
		WindowReady,//WM_CREATE
		WindowDestroy,//WM_NCDESTROY
		WindowShow,//WM_SHOWWINDOW
		OnFinal,//最后一条消息，一般在里面执行delete this

		SetFocued,//WM_SETFOCUS
		KillFocued,//WM_KILLFOCUS
		OnTimer ,//WM_TIMER
		DragOver,//一般是拖动结束时，触发的消息
	};

	typedef struct __MUIEVENT
	{
		DWORD	  dwType;
		WPARAM    wParam;
		LPARAM    lParam;
	}MUIEVENT;

	class IWindowEvent
	{
	public:
		virtual LRESULT OnEvent(MUIEVENT & Event) = 0;
	};

	

	class IControlHooker
	{
	public:
		//消息处理之前流入OnBefore，返回true则消息不会流入控件处理
		virtual bool OnBefore(CControlUI * pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule) = 0;

		//消息处理后再流入OnAfter
		virtual bool OnAfter(CControlUI * pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule) = 0;
	};

	class IListElemenDraw
	{
	public:
		virtual DWORD OnDraw(int nIndex, DWORD dwState, LPVOID InfoPtr);
	};

	class IListGetItemInfo
	{
	public:
		virtual DWORD GetItemInfo(int nItem, int nSubItem, DWORD dwState, LPVOID InfoPtr);
	};

	class ITranslateAccelerator
	{
	public:
		virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
	};


}

#endif