
#ifndef _MYUI_ACTIVEX_EXT_CONTROL_H_
#define _MYUI_ACTIVEX_EXT_CONTROL_H_

#include "..\\Core\\UIControl.h"

struct IOleObject;

namespace MYUI
{
	//c++�ֲ�������ʹ��new�����ڴ����create*����COM�ӿ�,
	//���ڳ�������ں�����������жϴ���ʧ���˳�, ����ÿ�ζ���Ҫdelete�ڴ����release, 
	//�����鷳���ҿ�����������ĳ��,�����Դû�м�ʱ�ͷ�.�����ͨ�õ�����Խ����Щ����:
	

	class MYUI_API CActiveXCtrl : public IOleClientSite, public IOleInPlaceSiteWindowless,
		public IOleControlSite, public IObjectWithSite, public IOleContainer
	{
	public:
		CActiveXCtrl();
		~CActiveXCtrl();

		// IUnknown
		STDMETHOD_(ULONG,AddRef)();//���Ӽ���
		STDMETHOD_(ULONG,Release)();//�������ü���
		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);//����ӿ�

		/*
		// IObjectWithSite
		Internet Explorer��BHO�Ķ������ʵ��IObjectWithSite�ӿ�.
		�ýӿ���IE�����Բ�����й����ͨѶ��һ���ӿڣ�����SetSite��GetSite����������
		��IE���غ�ж��Band�����ʱ��,���øýӿڵ�SetSite����,
		���ǿ���ͨ��SetSite��������IUnknownָ������ȡ��������Ҫ�Ľӿ�:
		*/
		STDMETHOD(SetSite)(IUnknown *pUnkSite);//�����������ݶ���ָ����վ��Ľӿڵ�ָ�롣
		STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);//����ʹ��SETSITE���ݵ�����վ�㡣

		// IOleClientSite
		STDMETHOD(SaveObject)(void);  //����Ƕ�����     
		STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);//���������
		STDMETHOD(GetContainer)(IOleContainer** ppContainer);   //�������������ָ��
		STDMETHOD(ShowObject)(void);        //Ҫ���������ʾ����
		STDMETHOD(OnShowWindow)(BOOL fShow);        //�������Ϊ���ӻ򲻿���ʱ֪ͨ������
		STDMETHOD(RequestNewObjectLayout)(void);//Ҫ�������������ʾλ��

		// IOleInPlaceSiteWindowless - �޴����¼�����ʵ�����ʾ͸�CControl��һ����
		STDMETHOD(CanWindowlessActivate)(void);//֪ͨ�����������Ƿ����֧��Ϊ���Ծ͵ؼ����ĩ���ڵĶ���
		STDMETHOD(GetCapture)(void);//�����ɾ͵ػ���޴��ڵĶ���ȷ�����Ƿ��Ծ�����겶��
		STDMETHOD(SetCapture)(BOOL fCapture);//ʹһ�͵ػ���޴��ڵĶ����ȡ���������Ϣ
		STDMETHOD(GetFocus)(void);//�����ɾ͵ػ���޴��ڵĶ���ȷ�����Ƿ��Ծ��м��̽���
		STDMETHOD(SetFocus)(BOOL fFocus);//����һ���û�������Ч����Ч���ڵĶ���ļ��̽���
		STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);//�ṩ����ľ���豸�����ĵ���Ļ������������������
		STDMETHOD(ReleaseDC)(HDC hDC);//�ͷ���ǰ���û���豸�����ĵ�GetDC
		STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);//ʹ������Ч������Ļ��ʹ�þ͵�ͼ��ָ����һ������
		STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);//ʹ������Ч������Ļ��ʹ�þ͵�ͼ���ָ������
		STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);//ʹ�����ƶ�����͵ػ��ӳ���ڵ���������Ļ��
		STDMETHOD(AdjustRect)(LPRECT prc);//����һ��ָ���ľ��Σ���ͨ���ص���ȫ�򲿷ָ��ǲ�͸���Ķ���
		STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);//���ô����������Ϣ��Ĭ�ϴ��׸�����

		// IOleInPlaceSiteEx
		STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);  //��Ƕ�������ã���ȷ���Ƿ���Ҫ�ڼ���ʱ���»�������      
		STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);  //�����ж�Ŀ���ʾ�����֪ͨ��
		STDMETHOD(RequestUIActivate)(void);//֪ͨ�����ö��󼴽�����UI�״̬��

		// IOleInPlaceSite
		STDMETHOD(CanInPlaceActivate)(void);//ȷ�������Ƿ���Ծ͵ؼ������ 
		STDMETHOD(OnInPlaceActivate)(void); //֪ͨ��������һ���������ڱ����  
		STDMETHOD(OnUIActivate)(void);//֪ͨ�������󼴽����ʵ�λ�ñ�������Ҹö���ʹ�þ͵ظ��ϲ˵��滻���������˵���
		//ʹ�͵ض����ܹ������γɴ��ڶ����νṹ�Ĵ��ڽӿڣ��Լ��ö���ľ͵ؼ����Ӧ��λ�ڸ������е�λ�á�
		STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
		STDMETHOD(Scroll)(SIZE scrollExtant);//ָʾ������ָ���������������������ͼ��
		STDMETHOD(OnUIDeactivate)(BOOL fUndoable);//֪ͨ����Ӧ�����°�װ���û����沢����ע�������Լ������Ƿ���пɳ���״̬��
		STDMETHOD(OnInPlaceDeactivate)(void);//֪ͨ�����ö����ٴ��ڻ״̬��
		STDMETHOD(DiscardUndoState)( void);//ָʾ���������䳷��״̬��������Ӧ�õ���IOLIN StaseObjult:��ReActivateAndUndo��
		STDMETHOD(DeactivateAndUndo)( void);//���ö��󣬽����͵ػỰ�����ָ�����������ĳ���״̬��
		STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);//֪ͨ����������չ�Ѹ��ġ�

		// IOleWindow
		STDMETHOD(GetWindow)(HWND* phwnd);//��������͵ؼ����һ�����ڣ���ܡ��ĵ��������ڻ�͵ض��󴰿ڣ��ľ����
		STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);//ȷ���Ƿ�Ӧ�ھ͵ؼ���Ự�ڼ��������������еİ���ģʽ��

		// IOleControlSite
		//֪ͨ�����ؼ���ControlINFO�ṹ�Ѿ����ģ���������Ӧ�õ��ÿؼ���IOleControl::GetControlInfo���и��¡�
		STDMETHOD(OnControlInfoChanged)(void);      
		//ָʾ�ؼ��Ƿ�Ӧ�����ڻ״̬���˷����ĵ���ͨ��Ƕ��һ���¼�����ȷ������ļ���״̬���¼���������������б����ȶ���
		STDMETHOD(LockInPlaceActive)(BOOL fLock);       
		 //����ָ���������ڰ�װʵ�ʿؼ�����չ�ؼ���IDispatchָ�롣
		STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);
		//����HIMETRIC��λ����OLE�еı�׼����ʾ������ת��Ϊ����ָ���ĵ�λ��
		STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);     
		//���������ݵ�����վ����д���
		STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
		//ָʾ�˿���վ�����Ŀؼ��Ƿ��ѻ�ý������ʧȥ���㡣
		STDMETHOD(OnFocus)(BOOL fGotFocus);
		//ָʾ������ʾǶ���ڴ�վ���еĿؼ������Ա�
		STDMETHOD(ShowPropertyFrame)(void);

		// IOleContainer
		STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);//ö�ٵ�ǰ�����еĶ���
		STDMETHOD(LockContainer)(BOOL fLock);//����Ƕ�������������У�ֱ����ʽ�ͷš�

		// IParseDisplayName
		//����ָ������ʾ���Ʋ�������Ӧ�����֡�
		STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);

		//MYUI
		STDMETHOD(SetWindow)(HWND hWnd);
	protected:

		
	protected:
		LONG m_dwRef;

		HWND m_hHostWindow;//���������Ǹ�����
		IUnknown * m_pUnkSite;
		IViewObject * m_pViewObject;
		IOleObject * m_pOleObject;
		IOleInPlaceObjectWindowless * m_pInPlaceObject;
		bool m_bLocked;
		bool m_bFocused;
		bool m_bCaptured;
		bool m_bUIActivated;
		bool m_bWindowless;
		RECT m_rcPos;//�ؼ��ľ���λ��
		
	};

	class MYUI_API CActiveXUI : public CControlUI , public CActiveXCtrl
	{
	public:
		CActiveXUI();
		virtual ~CActiveXUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		
	public:
		virtual bool CreateControl(const CLSID clsid = IID_NULL);
		virtual bool CreateControl(LPCTSTR strCLSID);
		HRESULT GetControl(const IID iid, LPVOID *ppRet);
		CLSID GetClisd() const;
		CMuiString GetModuleName() const;
		void SetModuleName(LPCTSTR strText);

		virtual void SetVisible(bool bVisible = true);

		//����
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual bool OnPaint(RECT rcItem,RECT rcPaint, RECT rcUpdate);
		
	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
		virtual void OnAttach(HWND hNewWnd);//���ؼ����ӵ��µĴ���ʱ�������һ�θú���
		virtual void OnDetach(HWND hOldWnd);//���ؼ��뿪һ������ʱ�������һ�θú��� 
		virtual void ReleaseControl();
		void DoVerb(LONG nVerb);
	protected:
		CLSID m_clsid;
		CMuiString m_strModuleName;

	};

}

#endif