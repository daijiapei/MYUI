
#ifndef __MYUI_INTERFACE_H__
#define __MYUI_INTERFACE_H__

#include "../base.h"

namespace MYUI
{
#define WM_HITTEST          WM_NCHITTEST
#define WM_BREAK            (WM_USER + 1)
#define WM_MYUIMSG			(WM_USER + 128)//窗口与控件同用
#define WM_MYCONTROLMSG     (WM_USER + 256)//控件私有
#define WM_MYCM             WM_MYCONTROLMSG

#define WM_NOTIFYEX			(WM_MYUIMSG + 1)

/*WM_SETTIMER */
#define WM_SETTIMER         (WM_MYUIMSG + 2)

	typedef struct _tagControlTimer
	{
		PVOID pControl;
		UINT nIDEvnet;
		UINT uElapse;
	}CONTROLTIMER;
/*WM_SETTIMER */

#define WM_MOUSEENTER		(WM_MYUIMSG + 3)
#define WM_DEPENDCHANGE		(WM_MYUIMSG + 4)


/*WM_REQUESTINFO user WPARAM return InfoPtr*/
#define WM_REQUESTINFO      (WM_MYUIMSG + 5)
#define MRQF_SHAREINFO   0x01
#define MRQF_VIEWINFO    0x02
#define MRQF_GETFOCUS    0x03
/*WM_REQUESTINFO*/

#define WM_LBUTTONCLICK          (WM_MYUIMSG + 11)
#define WM_MBUTTONCLICK          (WM_MYUIMSG + 12)
#define WM_RBUTTONCLICK          (WM_MYUIMSG + 13)

#define WM_CONTROLMSG_SEND   (WM_MYUIMSG + 21)
#define WM_CONTROLMSG_POST   (WM_MYUIMSG + 22)

/*WM_XETSTATE user WPARAM return DWORD or BOOL*/
#define WM_SETSTATE          (WM_MYUIMSG + 23)
#define WM_GETSTATE          (WM_MYUIMSG + 24)
//wParam使用 STATE_XXXXXX，返回BOOL，若wParam = NULL, 返回所有状态
/*WM_XETSTATE user WPARAM return BOOL*/

/*WM_GETSCROLL user WPARAM return   CScrollUI*/
#define WM_GETSCROLL         (WM_MYUIMSG + 25)
#define GSL_VERTICAL         1
#define GSL_HORIZONTAIL      2

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

//某些控件包含子窗口，子窗口私有消息处理
#define WM_CHECKHIDE    (WM_APP + 1)//检查是否需要关闭,使用PostMessage，详情看消息处理

    typedef struct __MSGCALL
	{
		BOOL bRelease;//TRUE = 自动释放MSGCALL； FALSE = 手动释放MSGCALL
		LPVOID pSender;
		MSG msg;
	}MSGCALL, *PMSGCALL;

	enum EnumNotifyMsg
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
	};

    class CControlUI;
    typedef struct __TNOTIFYUI
	{
		CControlUI *  pSender;
		EnumNotifyMsg   dwType;
		WPARAM      wParam;
		LPARAM      lParam;
		//DWORD       time;
		//POINT       pt;
	}TNOTIFYUI;

	class INotify
	{
	public:
		//这个接口用来过滤notify，过滤完成后，再调用 IControlNotify
        virtual void SendNotify(TNOTIFYUI &Notify) = 0;
	};

	class IControlNotify
	{
	public:
        virtual void OnNotify(TNOTIFYUI &notify) = 0;
	};

    class IMenuPopup
    {
    public:
        //往hNotifyWnd发消息, 鼠标在屏幕中的Point位置
        virtual LRESULT Popup(INotify *pNotify, RECT rcAbsolute, bool bRightPopup = true) = 0;
        virtual void Hide(bool bHideChild) = 0;
    };

	enum EnumEventType
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

	typedef struct __TEVENT
	{
		DWORD	  dwType;
		WPARAM    wParam;
		LPARAM    lParam;
	}TEVENT;

	class IWindowEvent
	{
	public:
		virtual LRESULT OnEvent(TEVENT &event) = 0;
	};

	class IControlHooker
	{
	public:
		//消息处理之前流入OnBefore，返回true则消息不会流入控件处理
		virtual bool OnBefore(PVOID pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule) = 0;

		//消息处理后再流入OnAfter
		virtual bool OnAfter(PVOID pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule) = 0;
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

	class IBaseWindow
	{
	public:
		virtual LRESULT SendMessage(UINT message, WPARAM wParam, LPARAM lParam);
		virtual LRESULT PostMessage(UINT message, WPARAM wParam, LPARAM lParam);
		virtual BOOL Update();
	};

	class IScrollBarMove
	{
		//pSender = 目标滚动条
	public:
		virtual void OnScrollBarMove(LPCVOID pSender, int nShift) = 0;
		virtual const SIZE &GetScrollBarShift() const = 0 ;
	};

	typedef struct _tag_msg
	{
		HWND hWnd;//当消息不是从窗口传出，需要设置为空
		UINT message;
		WPARAM wParam;
		LPARAM lParam;
		POINT point;
	}TMSG,* PTMSG;

	
}

#endif