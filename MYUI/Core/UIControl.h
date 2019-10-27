
#ifndef _MYUI_CONTROL_H_
#define _MYUI_CONTROL_H_

#include "UIBase.h"
#include "../Render/RenderEngine.h"
#include "UIInterface.h"
#include "../ExtControl/UIToolTip.h"

namespace MYUI
{

    template< class T >
    class CSafeRelease
    {
    public:
        CSafeRelease() : m_p(NULL){};
        CSafeRelease(T* p) : m_p(p) { };
        ~CSafeRelease() { if (m_p != NULL) m_p->Release(); };
        T* Attach(T* p) { m_p = p; return m_p; };
        T* Detach() { T* t = m_p; m_p = NULL; return t; };
    private:
        T* m_p;
    };

#define STATE_DEFAULT    0x0000//Ĭ��
#define STATE_SELECT     0x0001//ѡ��
#define STATE_HOT        0x0002//�ȵ�
#define STATE_FOCUS      0x0004//����
#define STATE_PUSHED     0x0008//����
#define STATE_CHECKED    0x0010//ѡ��
#define STATE_UNKNOW     0x0080//δȷ��

	//�ýṹ��CWindowUI���ã��ṩ�����пؼ�����
	class CControlUI;

    typedef struct __SHAREINFO
	{
		HWND hWnd;
		HDC  hPaintDC;
		CRenderEngine * pRenderEngine;//��Ⱦ����
		
//һ��ʹ����ĳ��ģʽ���Ͳ��ܸ��ģ�����Ҫô��ͬ��ģʽ��Ҫô����ģʽ(��Ϊ������ͷ���Դָ���������)
//TRUE = ����ģʽ������Ҫ�ͷ���Դ��FALSE = ʹ��ͬ����Դ��Ϣ������Ҫ�ͷ���Դָ�룬�������ͷ�
		BOOL bHostType;
		TCHAR * strSkinFolder;//Ƥ��
		CMuiIdArray * FontArray;//��������
		INotify * pNotify;
		CToolTipUI * pToolTip;
	}TSHAREINFO, * PSHAREINFO;

#define GETHWND(_this)  ((_this)->m_pShareInfo ? (_this)->m_pShareInfo->hWnd : NULL)

	class MYUI_API CControlUI :  public CItemViewInfo , public CItemPosition 
	{
	public:
		CControlUI();
		virtual ~CControlUI();

		bool IsControl();
		//���ù�����Ϣ��������Ϣ�ɴ��ڴ��ݸ�ʼ��ؼ���ʼ��ؼ��ٵݹ鴫��ÿһ���ӿؼ�
		//������Ϣ����CControlUI��CWindowUIͨ�ŵ�����������Ϊһ����Ϣ������
		virtual void SetShareInfo(TSHAREINFO * pShareInfo);
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		virtual CMuiString GetClassName() const;
		
		//�й����CallWndProc/WndProc/SendMessage/PostMessage�Ҹ����˵һ�£�
		//���ĸ���������д��һ���ʱ�����ڳ��������Ժܶ����û��WndProc����
		//����SetText()������Ӧ�ý���ϢͶ�ݵ�WndProc(WM_SETTEXT)���洦���ġ�
		//������Ϊ�Ѿ�д��һ���ˣ�����������̫������ż���пղŻ�������Щ����
		//Ŀǰ��������������Ѿ��߶�ģ����WIN32���ںͿؼ����������о���
		//����Ȥ�Ļ��������Լ�����һ�¡���д���׽����Ŀ�ģ�ֻ��Ϊ�˸��������
		//һ��˼·������ϸ�ڵ����⣬����Ҫ����Լ�����һ�£���Ȼ������ҵ���Ŀ
		//�õ�ĳ�����ܵĻ�����Ҳ����в��Ժ͵���
		
		//�ȵ���Hook���ٵ���WndProc
		virtual LRESULT CallWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		
		virtual CControlUI* GetParent() const;
		virtual VOID SetParent(CControlUI* pParentContrl);
		virtual CControlUI * GetAncestorControl();//ʼ��ؼ����������ĸ��ؼ�
		HWND GetWindow() const;
		bool SendNotify(BOOL bChild, EnumNotifyMsg dwType, WPARAM wParam = 0L, LPARAM lParam = NULL);
		LRESULT SendMessage(UINT message, WPARAM wParam = 0L, LPARAM lParam = NULL);
		LRESULT PostMessage(UINT message, WPARAM wParam = 0L, LPARAM lParam = NULL);
		BOOL SetTimer(UINT nIDEvnet, UINT uElapse);
		BOOL KillTimer(UINT nIDEvnet);
        void SetMenu(IMenuPopup * pMenu);
        IMenuPopup * GetMenu() const;

		BOOL SetCapture();
		BOOL ReleaseCapture();
		BOOL SetHooker(IControlHooker * pHooker, int index = -1);
		BOOL UnHooker(IControlHooker * pHooker);
		IControlHooker * UnHooker(int index);
	public:
		//����
		//��Ϊ�ǲ���������Եģ�һ���ؼ��ĸ��ģ����ܻ�Ӱ�쵽�ֵܿؼ��Ĳ���
		//����Update�����ǵ��ø����ڵ�SetItem��Invalidate(),�����������������ֵܿؼ��Ĳ���
		virtual bool Update();//�һ���¸����ֵ����������ֵܿؼ�Ҳ�����
        virtual bool Renewal();//�һὫ�Ӵ��ڵ�λ�ö�����һ�Σ���ˢ����ͼ
		virtual bool Invalidate();//��ֻ�����Լ����������������ؼ������ܵ�Ӱ��
		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);

		virtual void SetVisible(bool bVisible);
		virtual bool IsVisible();
		virtual void SetEnabled(bool bEnabled);
		virtual bool IsEnabled();
		virtual void SetPenetrated(bool bPenetrated);
		virtual bool IsPenetrated();

		virtual CControlUI * FindControlByPoint(POINT &pt);
		virtual CControlUI * FindControlByName(LPCTSTR strName);

		//һЩ�߼��ؼ����ڲ��Ŀؼ����������ˣ�ʹ��FindControl�������ҵ����ǵ��ڲ��ؼ�ָ��
		//����Ҫ��FindSubControl���в��ң�FindSubControlĬ�Ϸ���NULL���߼��ؼ��ж�
		//����ΪFindSubControl�ṩ�˲�ͬ�ķ���
		virtual CControlUI * FindSubControlByPoint(POINT &pt);
		virtual CControlUI * FindSubControlByName(LPCTSTR strName);

		//GetItemDisplayRect��GetItemFixedRect������ͨ���ͻ�����ת���ó�
		//�ؼ�������ʾ��λ�ã�Ҳ����˵��ֻ�пؼ����ڸ��ؼ��Ŀͻ��������
		//�������ղ�����ȷִ�У���Щ����Ŀؼ�����������m_rcRawRect����
		//���л��Ƶģ���ô����������ղ���ȡ����ȷ����ʾ����
		virtual bool GetItemDisplayRect(RECT &rcDisplay);
		virtual bool GetClientDisplayRect(RECT &rcDisplay);

		//�ؼ��ڴ����еľ���λ�ã���GetDisplayRect��ͬ������ؼ������ڵ�GetDisplayRect���ȥ�ڵ�����
		//��GetFixedRect��ֻ���������ƫ���븸�ؼ��Ƿ����أ����������ڵ����֣�
		//����ֻ�и��ؼ����������ص�ʱ�򣬲Ż᷵��ʧ�ܣ����򷵻ؿؼ��ڴ����еľ���λ��
		virtual bool GetItemFixedRect(RECT &rcFixed);
		virtual bool GetClientFixedRect(RECT &rcFixed);

		virtual CMuiString GetName() const;
		virtual void SetName(LPCTSTR pstrName);
		virtual bool SetFocus();
		virtual bool IsFocued();

		//״̬
		virtual void SetState(DWORD dwState);
		virtual void AddState(DWORD dwState);
		virtual void RemoveState(DWORD dwState);
		virtual DWORD GetState() const;

		//
		void SetTag(UINT_PTR pTag);
		UINT_PTR GetTag() const;
		void SetTipText(LPCTSTR strTipText);
		LPCTSTR GetTipText() const;
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		//�ؼ��ڴ����еľ���λ�ã���GetDisplayRect��ͬ������ؼ������ڵ�GetDisplayRect���ȥ�ڵ�����
		//��GetFixedRect��ֻ���������ƫ���븸�ؼ��Ƿ����أ����������ڵ����֣�
		//����ֻ�и��ؼ����������ص�ʱ�򣬲Ż᷵��ʧ�ܣ����򷵻ؿؼ��ڴ����еľ���λ��
		virtual bool GetFixedRect(RECT &rcFixed, const RECT &rc);

		//�ؼ��ڴ�������ʾ��λ�ã�����ʾλ���򷵻�true
		//ע��GetDisplayRectֻ���Ǹ��ؼ��Ƿ��ڵ�������û�п����ֵܿؼ������ؼ��Ƿ��ڵ�����
		//����ֵܿؼ������ؼ��䷢���ص���GetDisplayRectȡ�õ�������û�н���Щ��������ȥ��
		//���󲿷�����£���ʹ�������ص������ǵ���GetDisplayRectҲ���������ǵ�����
		//�����Ҫ����׼ȷ����ʾλ����Ϣ�������GetDisplayRectEx
		virtual bool GetDisplayRect(RECT &rcDisplay,const RECT &rc);

		//GetDisplayRectEx���ֵܿؼ����ڵ�����Ҳ�����˽�ȥ��
		//��Ȼ��GetDisplayRectEx �� GetDisplayRect ������Ч�ʣ�
		//��ΪGetDisplayRectֻ�ݹ鸸�ؼ���GetDisplayRectEx���ݹ����е�ͬ�����ϲ�ؼ�
		//virtual bool GetDisplayRectEx(RECT &rcDisplay);

		//����
		virtual void PaintBkColor(const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintBkImage(const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintStatusImage( const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintText(const RECT& rcItem, const RECT& rcPaint);
		virtual void PaintBorder(const RECT& rcItem, const RECT& rcPaint);
		
		//֪ͨ
		virtual void OnAttach(HWND hNewWnd);//���ؼ����ӵ��µĴ���ʱ�������һ�θú���
		virtual void OnDetach(HWND hOldWnd);//���ؼ��뿪һ������ʱ�������һ�θú��� 
		virtual void OnPosChange(LPVOID pElement);//��λ�����Է����仯
		virtual void OnViewChange(LPVOID pElement);//���������Է����仯
		
	private:
		
	protected:
		TSHAREINFO * m_pShareInfo;
		CMuiString m_strToolTip;
		UINT_PTR m_pTag;
        IMenuPopup * m_pMenu;//�Ӳ˵����������þ����Ϊ�գ���ô������Ԫ��Ϊ���˵�
		CControlUI * m_pParentContrl;//���ؼ�

		CMuiString m_strName;
		//CMuiString m_strClassName;
		bool m_bControl;//true=���һ���ؼ��� false=���ǿؼ��������ǲ���)
		bool m_bEnabled;//�Ƿ�����
		bool m_bVisible;//�Ƿ���ʾ
		bool m_bPenetrated;//�����ʱ��ֱ�Ӵ���������������Ŀؼ�

		DWORD m_dwState;
		CMuiPtrArray m_pHookers;

		RECT m_rcSizeBox;

	private:
		

	};

	//������������Ľӿ�
	class MYUI_API IControlArray
	{
	public:
		virtual bool Add(CControlUI* pControl) = 0;
		virtual bool AddAt(CControlUI* pControl, int nIndex) = 0;
		virtual bool Remove(CControlUI* pControl) = 0;
		virtual bool RemoveAt(int nIndex) = 0;
		virtual void RemoveAll() = 0;
		virtual CControlUI * Find(int nIndex) = 0;
		virtual int Find(CControlUI * pControl) = 0;
		virtual int GetCount() const = 0;
	};

}


#endif