
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
#define BLWF_UNCHECK         0x00 //����Ҫ�����κμ�飬ֱ���˳�
#define BLWF_CHECKFOCUS      0x01 //�˳�ǰ��鸸�ӿؼ��Ƿ�ӵ�н���

		WMU_LBUTTONCLICK,
		WMU_MBUTTONCLICK,
		WMU_RBUTTONCLICK,


		WMU_MYUI = WM_USER + 128,
		WMU_USER = WM_APP,
	};







#define WM_MYUIMSG			(WM_USER + 128)//������ؼ�ͬ��
#define WM_MYCONTROLMSG     (WM_USER + 256)//�ؼ�˽��
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
//wParamʹ�� STATE_XXXXXX������BOOL����wParam = NULL, ��������״̬
/*WM_XETSTATE user WPARAM return BOOL*/


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


	typedef struct __MUICARET_INFO
	{
		HBITMAP Bitmap;
		ARGBREF Color;
		POINT Point;
		SIZE Size;
		BYTE Visible;
		BYTE Showing;
	} MUICARETINFO, * LPMUICARETINFO;

	//���
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

		ShowTip,//չʾtip
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
		//����ӿ���������notify��������ɺ��ٵ��� IControlNotify
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
        //��hNotifyWnd����Ϣ, �������Ļ�е�Pointλ��
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
		OnFinal,//���һ����Ϣ��һ��������ִ��delete this

		SetFocued,//WM_SETFOCUS
		KillFocued,//WM_KILLFOCUS
		OnTimer ,//WM_TIMER
		DragOver,//һ�����϶�����ʱ����������Ϣ
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
		//��Ϣ����֮ǰ����OnBefore������true����Ϣ��������ؼ�����
		virtual bool OnBefore(CControlUI * pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule) = 0;

		//��Ϣ�����������OnAfter
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