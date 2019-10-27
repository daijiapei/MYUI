
#ifndef __MYUI_MENU_CONTROL_H__
#define __MYUI_MENU_CONTROL_H__

#include "../Core/UIWindow.h"
#include "UIListBox.h"

namespace MYUI
{
   
#define MMUS_SELECTMODE  0x0001
#define MMUS_LINEMODE    0x0002

    //��������һ�����⣬�ɴ��ڲ������������Ĳ˵���������洰��һ������
    //�������û��Լ�����ʹ��new ������Ĳ˵��������û��Լ���������delete��������
    class MYUI_API CMenuUI : public IControlArray, public IMenuPopup, protected CWindowUI
    {
        friend class CBuilder;
    public:
        CMenuUI();
        virtual ~CMenuUI();
        //CControlArray
        virtual bool Add(CControlUI* pControl);
        virtual bool AddAt(CControlUI* pControl, int nIndex);
        virtual bool Remove(CControlUI* pControl);
        virtual bool RemoveAt(int nIndex);
        virtual void RemoveAll();
        virtual CControlUI * Find(int nIndex);
        virtual int Find(CControlUI * pControl);
        virtual int GetCount() const;

    public:
        

        HWND Create(HINSTANCE hInstance, LPCTSTR strWindowText, LPCTSTR strSkinFolder = NULL, LPCTSTR strXmlFile = NULL);
        void SetMenuLayout(CControlUI * pControl);
        virtual LRESULT Popup(INotify *pNotify, RECT rcAbsolute, bool bRightPopup = true);//��hNotifyWnd����Ϣ
        
    protected:
        virtual void Hide(bool bHideChild);
        //virtual void SendNotify(TNOTIFYUI &notify);

        virtual LPVOID GetInterface(LPCTSTR strName);
        virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
        virtual void OnNotify(TNOTIFYUI &notify);
        virtual LRESULT OnEvent(TEVENT &event);
        virtual CControlUI * CreateControl(const char* strClass);

        LRESULT DoModal(MSG &msg);
        CBaseLayoutUI * GetMenuUI() const;
    protected:
        CMenuUI * m_pChildMenu;
        INotify * m_pNotify;
    };

    /***************************************************************************
     * CMenuElementUI �� CMenuUI�еĽڵ�
     * CMenuElementUI ��Ƹ�ʽ
     *    ______________________________
     *    | �� |  text             | > |
     *    ------------------------------
     * ���� �� ���˵�ͼ��
     * text   : �˵�˵��
     * ���� >����ʾ���Ӳ˵�
    ***************************************************************************/
    class MYUI_API CMenuElementUI : public CControlUI
    {
    public:
        CMenuElementUI();
        ~CMenuElementUI();
        static CMuiString g_strClassName;

        virtual CMuiString GetClassName() const;

        virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

        void SetSelectMode(bool b);
        bool IsSelectMode();
        void SetLineMode(bool b);
        bool IsLineMode();
    protected:
        virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        virtual void PaintBkColor(const RECT& rcItem, const RECT& rcPaint);
        virtual void PaintStatusImage(const RECT& rcItem, const RECT& rcPaint);
        virtual void PaintText(const RECT& rcItem, const RECT& rcPaint);
        virtual SIZE GetValidSize();//ȡ��������Ч�ĳߴ�
    public:
        //��������
        void SetDisabledLogo(LPCTSTR strImage);
        LPCTSTR GetDisabledLogo() const;
        void SetNormalLogo(LPCTSTR strImage);
        LPCTSTR GetNormalLogo() const;
        void SetHotLogo(LPCTSTR strImage);
        LPCTSTR GetHotLogo() const;
        void SetSelectLogo(LPCTSTR strImage);
        LPCTSTR GetSelectLogo() const;

        void SetDisabledGuide(LPCTSTR strImage);
        LPCTSTR GetDisabledGuide() const;
        void SetNormalGuide(LPCTSTR strImage);
        LPCTSTR GetNormalGuide() const;
        void SetHotLGuide(LPCTSTR strImage);
        LPCTSTR GetHotGuide() const;

        void SetHotColor(ARGBREF argb);
        ARGBREF GetHotColor();

        void SetHotTextColor(ARGBREF argb);
        ARGBREF GetHotTextColor();
        void SetSelectTextColor(ARGBREF argb);
        ARGBREF GetSelectTextColor();

        
    protected:
        CMuiString m_strDisabledLogo;//����״̬
        CMuiString m_strNormalLogo;//��ͨ״̬
        CMuiString m_strHotLogo;//��꾭���ؼ�״̬
        CMuiString m_strSelectLogo;//Checkģʽ��,ѡ��״̬��
        CMuiString m_strEnalbledSelectLogo;//������ѡ��״̬��

        CMuiString m_strDisabledGuide;
        CMuiString m_strNormalGuide;
        CMuiString m_strHotGuide;


        ARGBREF m_refHotColor;

        ARGBREF m_refHotTextColor;
        ARGBREF m_refSelectTextColor;

        ARGBREF m_refLineColor;
        ARGBREF m_nLineSize;

        int m_nLogoArea;//Logo�Ŀ�ȣ�Ĭ��-1���������һ��
        int m_nGuideArea;
        RECT m_rcTextPadding;
    };


    
}


#endif