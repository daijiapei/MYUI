
#ifndef _MYUI_WEBBROWSER_EXT_CONTROL_H_
#define _MYUI_WEBBROWSER_EXT_CONTROL_H_

#include <ExDisp.h>
#include <ExDispid.h>
#include <mshtmhst.h>
#include <ExDisp.h>
//#include "..//Core//UIControl.h"
#include "UIActiveX.h"

namespace MYUI
{
	class CWebBrowserEventHandler
	{
	public:
		CWebBrowserEventHandler() {}
		~CWebBrowserEventHandler() {}

		virtual void BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel ) {}
		virtual void NavigateError(IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel) {}
		virtual void NavigateComplete2(IDispatch *pDisp,VARIANT *&url){}
        virtual void DocumentComplete2(IDispatch *pDisp, VARIANT *&url){};
		virtual void ProgressChange(LONG nProgress, LONG nProgressMax){}
		virtual void NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl){}
		virtual void CommandStateChange(long Command,VARIANT_BOOL Enable){};

		// interface IDocHostUIHandler
		virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(
			/* [in] */ DWORD dwID,
			/* [in] */ POINT __RPC_FAR *ppt,
			/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
			/* [in] */ IDispatch __RPC_FAR *pdispReserved)
		{
			//return E_NOTIMPL;
			//���� E_NOTIMPL ��������ϵͳ�Ҽ��˵�
			return S_OK;
			//����S_OK �������ϵͳ�Ҽ��˵�
		}

		virtual HRESULT STDMETHODCALLTYPE GetHostInfo(
			/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
		{
			// 		if (pInfo != NULL)
			// 		{
			// 			pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER;
			// 		}
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE ShowUI(
			/* [in] */ DWORD dwID,
			/* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
			/* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
			/* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
			/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc)
		{
			return S_FALSE;
		}

		virtual HRESULT STDMETHODCALLTYPE HideUI( void)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE UpdateUI( void)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE EnableModeless(
			/* [in] */ BOOL fEnable)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(
			/* [in] */ BOOL fActivate)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
			/* [in] */ BOOL fActivate)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE ResizeBorder(
			/* [in] */ LPCRECT prcBorder,
			/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
			/* [in] */ BOOL fRameWindow)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
			/* [in] */ LPMSG lpMsg,
			/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
			/* [in] */ DWORD nCmdID)
		{
			return S_FALSE;
		}

		virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(
			/* [out] */ LPOLESTR __RPC_FAR *pchKey,
			/* [in] */ DWORD dw)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE GetDropTarget(
			/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
			/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE GetExternal(
			/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
		{
			return E_NOTIMPL;
		}

		virtual HRESULT STDMETHODCALLTYPE TranslateUrl(
			/* [in] */ DWORD dwTranslate,
			/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
			/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE FilterDataObject(
			/* [in] */ IDataObject __RPC_FAR *pDO,
			/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
		{
			return S_OK;
		}

		// 	virtual HRESULT STDMETHODCALLTYPE GetOverrideKeyPath( 
		// 		/* [annotation][out] */ 
		// 		__deref_out  LPOLESTR *pchKey,
		// 		/* [in] */ DWORD dw)
		// 	{
		// 		return E_NOTIMPL;
		// 	}

		// IDownloadManager
		virtual HRESULT STDMETHODCALLTYPE Download( 
			/* [in] */ IMoniker *pmk,
			/* [in] */ IBindCtx *pbc,
			/* [in] */ DWORD dwBindVerb,
			/* [in] */ LONG grfBINDF,
			/* [in] */ BINDINFO *pBindInfo,
			/* [in] */ LPCOLESTR pszHeaders,
			/* [in] */ LPCOLESTR pszRedir,
			/* [in] */ UINT uiCP)
		{
			return S_OK;
		}
	};

	class MYUI_API CWebBrowserUI
        : public CControlUI
        , public IObjectWithSite
        , public IOleClientSite
        , public IOleInPlaceSite
        , public IOleControlSite
        , public IStorage
        , public IOleCommandTarget
		, public IDocHostUIHandler
		, public IServiceProvider
		, public ITranslateAccelerator
        , public IDispatch
	{
	public:
		// ���캯��
		CWebBrowserUI();
		virtual ~CWebBrowserUI();
		static IID OBJCLSID;
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
        virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
        void SetWebBrowserEventHandler(CWebBrowserEventHandler* pEventHandler);

		void SetHomePage(LPCTSTR lpszUrl);
		LPCTSTR GetHomePage();

		void SetAutoNavigation(bool bAuto = TRUE);
		bool IsAutoNavigation();

		void Navigate2(LPCTSTR lpszUrl);
		void Refresh();
		void Refresh2(int Level);
		void GoBack();
		void GoForward();
		void NavigateHomePage();
		void NavigateUrl(LPCTSTR lpszUrl);
		IWebBrowser2* GetWebBrowser2(void);
		IDispatch* GetHtmlWindow();
        
        virtual void SetVisible(bool bVisible = true);
        virtual bool SetItem(RECT rcItem, bool bMustUpdate);
    public:
		static DISPID FindId(IDispatch *pObj, LPOLESTR pName);
		static HRESULT InvokeMethod(IDispatch *pObj, LPOLESTR pMehtod, VARIANT *pVarResult, VARIANT *ps, int cArgs);
        static HRESULT SetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue);
		static HRESULT GetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue);

	protected:
        HRESULT GetControl(const IID iid, LPVOID *ppRet);
        bool CreateControl();
		void ReleaseControl();
		HRESULT RegisterEventHandler(BOOL inAdvise);
		
	public:

		// IUnknown
		STDMETHOD_(ULONG,AddRef)();
		STDMETHOD_(ULONG,Release)();
		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

        /*
        // IObjectWithSite
        Internet Explorer��BHO�Ķ������ʵ��IObjectWithSite�ӿ�.
        �ýӿ���IE�����Բ�����й����ͨѶ��һ���ӿڣ�����SetSite��GetSite����������
        ��IE���غ�ж��Band�����ʱ��,���øýӿڵ�SetSite����,
        ���ǿ���ͨ��SetSite��������IUnknownָ������ȡ��������Ҫ�Ľӿ�:
        */
        STDMETHOD(SetSite)(IUnknown *pUnkSite);//�����������ݶ���ָ����վ��Ľӿڵ�ָ�롣
        STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);//����ʹ��SETSITE���ݵ�����վ�㡣

        // IOleWindow
        STDMETHOD(GetWindow)(HWND* phwnd);//��������͵ؼ����һ�����ڣ���ܡ��ĵ��������ڻ�͵ض��󴰿ڣ��ľ����
        STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);//ȷ���Ƿ�Ӧ�ھ͵ؼ���Ự�ڼ��������������еİ���ģʽ��

        //IOleInPlaceSite
        virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void);
        virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void);
        virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void);
        virtual HRESULT STDMETHODCALLTYPE GetWindowContext(
            __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
            __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
            __RPC__out LPRECT lprcPosRect,
            __RPC__out LPRECT lprcClipRect,
            __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo);

        virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant);
        virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
        virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void);
        virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void);
        virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void);
        virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(__RPC__in LPCRECT lprcPosRect);

        // IOleControlSite
        //֪ͨ�����ؼ���ControlINFO�ṹ�Ѿ����ģ���������Ӧ�õ��ÿؼ���IOleControl::GetControlInfo���и��¡�
        STDMETHOD(OnControlInfoChanged)(void);
        //ָʾ�ؼ��Ƿ�Ӧ�����ڻ״̬���˷����ĵ���ͨ��Ƕ��һ���¼�����ȷ������ļ���״̬���¼���������������б����ȶ���
        STDMETHOD(LockInPlaceActive)(BOOL bLock);
        //����ָ���������ڰ�װʵ�ʿؼ�����չ�ؼ���IDispatchָ�롣
        STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);
        //����HIMETRIC��λ����OLE�еı�׼����ʾ������ת��Ϊ����ָ���ĵ�λ��
        STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);
        //���������ݵ�����վ����д���
        STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
        //ָʾ�˿���վ�����Ŀؼ��Ƿ��ѻ�ý������ʧȥ���㡣
        STDMETHOD(OnFocus)(BOOL bGotFocus);
        //ָʾ������ʾǶ���ڴ�վ���еĿؼ������Ա�
        STDMETHOD(ShowPropertyFrame)(void);

        // IOleClientSite
        STDMETHOD(SaveObject)(void);  //����Ƕ�����     
        STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);//���������
        STDMETHOD(GetContainer)(IOleContainer** ppContainer);   //�������������ָ��
        STDMETHOD(ShowObject)(void);        //Ҫ���������ʾ����
        STDMETHOD(OnShowWindow)(BOOL fShow);        //�������Ϊ���ӻ򲻿���ʱ֪ͨ������
        STDMETHOD(RequestNewObjectLayout)(void);//Ҫ�������������ʾλ��

        // IStorage
        virtual HRESULT STDMETHODCALLTYPE CreateStream(const OLECHAR *pwcsName,
            DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm) override;

        virtual HRESULT STDMETHODCALLTYPE OpenStream(const OLECHAR *pwcsName,
            void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm) override;

        virtual HRESULT STDMETHODCALLTYPE CreateStorage(const OLECHAR *pwcsName,
            DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg) override;

        virtual HRESULT STDMETHODCALLTYPE OpenStorage(const OLECHAR *pwcsName,
            IStorage *pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved,IStorage **ppstg) override;

        virtual HRESULT STDMETHODCALLTYPE CopyTo(DWORD ciidExclude,
            const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest) override;

        virtual HRESULT STDMETHODCALLTYPE MoveElementTo(const OLECHAR *pwcsName,
            IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags) override;

        virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) override;

        virtual HRESULT STDMETHODCALLTYPE Revert(void) override;

        virtual HRESULT STDMETHODCALLTYPE EnumElements(DWORD reserved1,
            void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum) override;

        virtual HRESULT STDMETHODCALLTYPE DestroyElement(const OLECHAR *pwcsName) override;

        virtual HRESULT STDMETHODCALLTYPE RenameElement(const OLECHAR *pwcsOldName, const OLECHAR *pwcsNewName) override;

        virtual HRESULT STDMETHODCALLTYPE SetElementTimes(const OLECHAR *pwcsName,
            const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime) override;

        virtual HRESULT STDMETHODCALLTYPE SetClass(REFCLSID clsid) override;

        virtual HRESULT STDMETHODCALLTYPE SetStateBits(DWORD grfStateBits, DWORD grfMask) override;

        virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag) override;

        // IOleCommandTarget
        virtual HRESULT STDMETHODCALLTYPE QueryStatus(__RPC__in_opt const GUID *pguidCmdGroup, 
            ULONG cCmds, __RPC__inout_ecount_full(cCmds) OLECMD prgCmds[], __RPC__inout_opt OLECMDTEXT *pCmdText);

        virtual HRESULT STDMETHODCALLTYPE Exec(__RPC__in_opt const GUID *pguidCmdGroup, 
            DWORD nCmdID, DWORD nCmdexecopt, __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut);

		// IDocHostUIHandler
        STDMETHOD(ShowContextMenu)(DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit) override;
        STDMETHOD(GetHostInfo)(DOCHOSTUIINFO* pInfo) override;
		STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, 
            IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc) override;
        STDMETHOD(HideUI)() override;
        STDMETHOD(UpdateUI)() override;
        STDMETHOD(EnableModeless)(BOOL fEnable) override;
        STDMETHOD(OnDocWindowActivate)(BOOL fActivate) override;
        STDMETHOD(OnFrameWindowActivate)(BOOL fActivate) override;
        STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fFrameWindow) override;
        STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID) override;	//�������Ϣ����
        STDMETHOD(GetOptionKeyPath)(LPOLESTR* pchKey, DWORD dwReserved) override;
        STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget) override;
        STDMETHOD(GetExternal)(IDispatch** ppDispatch) override;
        STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut) override;
        STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet) override;

		// IServiceProvider
		STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void** ppvObject);

		// IDownloadManager
		STDMETHOD(Download)( 
			/* [in] */ IMoniker *pmk,
			/* [in] */ IBindCtx *pbc,
			/* [in] */ DWORD dwBindVerb,
			/* [in] */ LONG grfBINDF,
			/* [in] */ BINDINFO *pBindInfo,
			/* [in] */ LPCOLESTR pszHeaders,
			/* [in] */ LPCOLESTR pszRedir,
			/* [in] */ UINT uiCP);

		// ITranslateAccelerator
		// ��Ϣ�ַ���WebBrowser
		virtual LRESULT TranslateAccelerator( MSG *pMsg );

        // IDispatch
        virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(__RPC__out UINT *pctinfo);
        virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo);
        virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(__RPC__in REFIID riid,
            __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
            DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

    protected:
        virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
        virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        virtual void OnAttach(HWND hNewWnd);//���ؼ����ӵ��µĴ���ʱ�������һ�θú���
        virtual void OnDetach(HWND hOldWnd);//���ؼ��뿪һ������ʱ�������һ�θú��� 
        HRESULT DoVerb(LONG nVerb);


        // DWebBrowserEvents2
        void BeforeNavigate2(IDispatch *pDisp, VARIANT *&url, VARIANT *&Flags, VARIANT *&TargetFrameName, VARIANT *&PostData, VARIANT *&Headers, VARIANT_BOOL *&Cancel);
        void NavigateError(IDispatch *pDisp, VARIANT * &url, VARIANT *&TargetFrameName, VARIANT *&StatusCode, VARIANT_BOOL *&Cancel);
        void NavigateComplete2(IDispatch *pDisp, VARIANT *&url);
        void DocumentComplete2(IDispatch *pDisp, VARIANT *&url);
        void ProgressChange(LONG nProgress, LONG nProgressMax);
        void NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);
        void CommandStateChange(long Command, VARIANT_BOOL Enable);
	protected:
		
        LONG m_dwRef;//���ü���
        IWebBrowser2*	m_pWebBrowser; //�����ָ��
        IHTMLWindow2*	m_pHtmlWnd2;
        //IViewObject * m_pViewObject;
        IOleObject * m_pOleObject;
        IUnknown * m_pUnkSite;
        IOleInPlaceObject* m_pOleInPlaceObject;
        CWebBrowserEventHandler* m_pWebBrowserEventHandler;	//������¼�����
        
		DWORD m_dwCookie;
		CMuiString m_strHomePage;	// Ĭ��ҳ��
        CMuiString m_strUrl;
		bool m_bAutoNavi;	// �Ƿ�����ʱ��Ĭ��ҳ��
        RECT m_rcPos;//����λ��
	};
} // namespace MYUI
#endif // __UIWEBBROWSER_H__