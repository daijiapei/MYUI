
#ifndef __MYUI_INTERFACE_H__
#define __MYUI_INTERFACE_H__

#include "../base.h"

namespace MYUI
{
#define WM_HITTEST          WM_NCHITTEST
#define WM_BREAK            (WM_USER + 1)
#define WM_MYUIMSG			(WM_USER + 128)//������ؼ�ͬ��
#define WM_MYCONTROLMSG     (WM_USER + 256)//�ؼ�˽��
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
//wParamʹ�� STATE_XXXXXX������BOOL����wParam = NULL, ��������״̬
/*WM_XETSTATE user WPARAM return BOOL*/

/*WM_GETSCROLL user WPARAM return   CScrollUI*/
#define WM_GETSCROLL         (WM_MYUIMSG + 25)
#define GSL_VERTICAL         1
#define GSL_HORIZONTAIL      2

//WM_MYCM �ؼ�˽��
#define WMU_GETGROUP           (WM_MYCM + 1)
#define WMU_SETCHECK           (WM_MYCM + 2)
#define WMU_GETCHECK           (WM_MYCM + 3)
#define WMU_NOTIFY             (WM_MYCM + 4)

//�������Լ����������ؼ�
#define WMU_SETVALUE           (WM_MYCM + 30)
#define WMU_GETVALUE           (WM_MYCM + 31)
#define WMU_SETMAXVALUE        (WM_MYCM + 32)
#define WMU_GETMAXVALUE        (WM_MYCM + 33)
#define WMU_SETMINVALUE        (WM_MYCM + 34)
#define WMU_GETMINVALUE        (WM_MYCM + 35)

//ĳЩ�ؼ������Ӵ��ڣ��Ӵ���˽����Ϣ����
#define WM_CHECKHIDE    (WM_APP + 1)//����Ƿ���Ҫ�ر�,ʹ��PostMessage�����鿴��Ϣ����

    typedef struct __MSGCALL
	{
		BOOL bRelease;//TRUE = �Զ��ͷ�MSGCALL�� FALSE = �ֶ��ͷ�MSGCALL
		LPVOID pSender;
		MSG msg;
	}MSGCALL, *PMSGCALL;

	enum EnumNotifyMsg
	{
		//������Ϣ���������false����������Ҫ����OnNotify
		NonMessage = 0,
		ClickItem,//�ؼ�������Ϣ
		DbClickItem,//�ؼ�˫����Ϣ
		SelectChange,//������

		CheckItem, //�÷���SelectItemһ������Ҫ��STATE_CHECK״̬����������־���磺STATE_UNKNOW
        
		//���ڶ���Ŀؼ���ѡ��(������һ�)��ĳһ�wParam�ǿؼ���״̬��
		//ͨ���ж�wParam�����ж�ѡ�л���ȡ��ѡ��
		SelectItem,
		ActiveItem,//���ڶ���Ŀؼ�����˫����ĳһ��
		TextChange,//�ı������˸ı䣬һ������༭��
		SetFocus,//�ؼ���ý���
		KillFocus,//�ؼ�ʧȥ����
		TimerCall,//��Ϣ֪ͨ
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
		//����ӿ���������notify��������ɺ��ٵ��� IControlNotify
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
        //��hNotifyWnd����Ϣ, �������Ļ�е�Pointλ��
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
		OnFinal,//���һ����Ϣ��һ��������ִ��delete this

		SetFocued,//WM_SETFOCUS
		KillFocued,//WM_KILLFOCUS
		OnTimer ,//WM_TIMER
		DragOver,//һ�����϶�����ʱ����������Ϣ
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
		//��Ϣ����֮ǰ����OnBefore������true����Ϣ��������ؼ�����
		virtual bool OnBefore(PVOID pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule) = 0;

		//��Ϣ�����������OnAfter
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
		//pSender = Ŀ�������
	public:
		virtual void OnScrollBarMove(LPCVOID pSender, int nShift) = 0;
		virtual const SIZE &GetScrollBarShift() const = 0 ;
	};

	typedef struct _tag_msg
	{
		HWND hWnd;//����Ϣ���ǴӴ��ڴ�������Ҫ����Ϊ��
		UINT message;
		WPARAM wParam;
		LPARAM lParam;
		POINT point;
	}TMSG,* PTMSG;

	
}

#endif