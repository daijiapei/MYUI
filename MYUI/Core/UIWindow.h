
#ifndef _MYUI_WINDOW_H_
#define _MYUI_WINDOW_H_

#include "UIContainer.h"
#include "UIInterface.h"

namespace MYUI
{

    typedef struct __VIEWINFO
	{
		BOOL bFocusWnd;//����ӵ�н���
		CControlUI * pRootControl;//�������ڣ���Ҫ���ǻ�������ؼ�/����
		CControlUI * pHotControl;//��꾭��, �����ڲ���Ĭ�ϴ���
		CControlUI * pFocusControl;//���뽹��, ����׽������Ϣ,����CControlUI::SetFocus��������
		CControlUI * pCaptionControl;//������
		CControlUI * pPushedControl;//��갴��δ����,����׽�����Ϣ���ؼ��ڲ���Ĭ�ϴ���

		//��Ϊû��ʹ��ϵͳ�ļ��ټ����ܣ�����ʹ��Ctrl + ��ĸ����ݼ���ʱ�������һ��
		//�����WM_CHAR�ַ������Ե�WM_KEYDOWNʱ�����ַ�������������WM_CHAR���ж��ַ�
		//�Ƿ���ȣ������ȣ��򷵻ء��Դ˱��������ַ�

		TCHAR tSurplus;
		POINT ptLeftMouse;

		SIZE SizeMin;
		SIZE SizeMax;

	}VIEWINFO, *PVIEWINFO;

	//ע�⣺
	//1. �̳�CWindowUI���������new���������
	//2. �����յ�WM_NCDESTROY[��������]�󣬻��Զ�delete CWindowUI�������û��ֶ�����
	//   �����벻Ҫ��CWindowUI����������ִ��delete����
	class MYUI_API CWindowUI : public INotify, public IControlNotify, public IWindowEvent
	{
        friend class CBuilder;
	public:
		CWindowUI();
		virtual ~CWindowUI();

        virtual LPVOID GetInterface(LPCTSTR strName);
		operator HWND() const;

		HWND Create(HINSTANCE hInstance,HWND hWndParent, DWORD dwStyle, 
			LPCTSTR strClassName ,LPCTSTR strWindowText, RECT * pRect = NULL);//��������
		BOOL Close(LONG nRet = 0);//�첽�رգ��������κεط�����

		//ͬ�������Ѵ��һ�����⣬����ڴ����ڲ����ô˷�����������WindowDestroy�¼���delete thisָ��
		BOOL Destroy();
		
		void ShowWindow(bool bShow = true, bool bTakeFocus = false);//��ʾ����
        LRESULT ShowModal(bool bShow = true, bool bEnableParent = true, MSG * pMsg = NULL);//��ʾģ̬���ڣ�����������
		HWND GetHandle();

		void CenterWindow();
		void SetIcon(UINT nRes);

        virtual void SendNotify(TNOTIFYUI &notify);
		LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0L);
		LRESULT PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0L);
		void SetMinSize(SIZE size);
		const SIZE &GetMinSize() const;
		void SetMaxSize(SIZE size);
		const SIZE &GetMaxSize() const;
		void SetTextFont(int FontId);
		int GetTextFont() const;
		void SetHoverTime(DWORD dwHoverTime);
		DWORD GetHoverTimer() const;

		CMuiString GetClassName();
		CMuiString GetWindowText();
		
		//ÿ����������ֻ����һ��������
		void AttachFrameView(CControlUI * pControl);
		CControlUI * GetFrameView() const;

        //�����ļ�
        void SetLanguageFile(LPCTSTR strFile);
        LPCTSTR GetLanguageFile() const;

		//����������ʹ��ͬһ��Դ
        bool SetSyncResource(CWindowUI *pHostWindow);

		bool SetSkin(LPCTSTR strSkin);
		LPCTSTR GetSkin() const;

		bool AddFont(int Fontid, HFONT hFont);
		HFONT GetFont(int Fontid);

		bool AddGroup(LPCTSTR strGroup);

		void SetCaption(CControlUI * pControl);
		CControlUI * GetCaption() const;
		CControlUI * GetFocusControl();
	protected:
		//void SetCapture(bool bCapture);
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	private:
		bool RegisterClass();//ע�ᴰ�� 
		LRESULT MouseProc(UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT KeyBoardProc(UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		//�����Ǳ�������
        

	protected:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		int m_nFontId;
		DWORD m_dwHoverTime;
		CMuiString m_strClassName;
		CMuiString m_strWindowText;

		CMuiStringPtrMap m_GroupArray;//������¼�ؼ��ķ�������
		CAvlTree m_atrControlTimer;
		TSHAREINFO * m_pShareInfo;
		VIEWINFO * m_pViewInfo;
		BOOL m_bShowInScreen;
        CMuiPtrArray m_MenuArray;
        CMuiString m_strLanguageFile;
	};
}

#endif