
#ifndef _MYUI_WINDOW_H_
#define _MYUI_WINDOW_H_

#include "UIContainer.h"
#include "UIInterface.h"
#include "UIWindowBase.h"

namespace MYUI
{

    typedef struct __MUIVIEW_INFO
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

	}MUIVIEWINFO, *LPMUIVIEWINFO;

	//ע�⣺
	//1. �̳�CWindowUI���������new���������
	//2. �����յ�WM_NCDESTROY[��������]�󣬻��Զ�delete CWindowUI�������û��ֶ�����
	//   �����벻Ҫ��CWindowUI����������ִ��delete����
	class MYUI_API CWindowUI : public CWindowBase, public INotify, public IControlNotify, public IWindowEvent
	{
        friend class CBuilder;
	public:
		CWindowUI();
		virtual ~CWindowUI();

        virtual LPVOID GetInterface(LPCTSTR strName);

        BOOL UpdateLayeredWindow(COLORREF refKey, DWORD dwFlags = ULW_ALPHA);

        LRESULT Popup(RECT rcPos);

        virtual void SendNotify(MUINOTIFY &notify);

		void SetMinSize(SIZE size);
		const SIZE &GetMinSize() const;
		void SetMaxSize(SIZE size);
		const SIZE &GetMaxSize() const;
		void SetTextFont(int FontId);
		int GetTextFont() const;
		void SetHoverTime(DWORD dwHoverTime);
		DWORD GetHoverTimer() const;


		//ÿ����������ֻ����һ��������
		void AttachFrameView(CControlUI * pControl);
		CControlUI * GetFrameView() const;

        //�����ļ�
        void SetLanguageFile(LPCTSTR strFile);
        LPCTSTR GetLanguageFile() const;

		bool SetSkin(LPCTSTR strSkin);
		LPCTSTR GetSkin() const;

		bool AddFont(int Fontid, HFONT hFont);
		HFONT GetFont(int Fontid);

		bool AddGroup(LPCTSTR strGroup);

		void SetCaption(CControlUI * pControl);
		CControlUI * GetCaption() const;
		CControlUI * GetFocusControl();

		CControlUI* FindControl(LPCTSTR strName);
		CControlUI* FindControl(POINT& Point);

		CControlUI* GetRootControl();

		bool SetHost(CWindowUI * pHost);
		

        BOOL PostBreakMessage(BOOL bCheck, CControlUI * pParent);
	protected:
		//void SetCapture(bool bCapture);
		virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
        
	private:
		bool AddParasite(CWindowUI* pParasite);
		bool RemoveParasite(CWindowUI* pParasite);
		LRESULT MouseProc(UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT KeyBoardProc(UINT message, WPARAM wParam, LPARAM lParam);
		//�����Ǳ�������

	private:
		int m_nFontId;
		DWORD m_dwHoverTime;

		CMuiStringPtrMap m_GroupArray;//������¼�ؼ��ķ�������
		MUISHAREINFO * m_pShareInfo;
		MUIVIEWINFO* m_pViewInfo;
		BOOL m_bShowInScreen;
        CMuiPtrArray m_MenuArray;
		CMuiString m_strLanguageFile;
		CWindowUI* m_pHost;
		CMuiPtrArray m_Parasite;
	};
}

#endif