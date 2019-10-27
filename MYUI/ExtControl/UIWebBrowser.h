
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
			//返回 E_NOTIMPL 正常弹出系统右键菜单
			return S_OK;
			//返回S_OK 则可屏蔽系统右键菜单
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
		// 构造函数
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
        Internet Explorer的BHO的对象必须实现IObjectWithSite接口.
        该接口是IE用来对插件进行管理和通讯的一个接口，其有SetSite和GetSite两个方法，
        当IE加载和卸载Band对象的时候,调用该接口的SetSite方法,
        我们可以通过SetSite传进来的IUnknown指针来获取我们所需要的接口:
        */
        STDMETHOD(SetSite)(IUnknown *pUnkSite);//允许容器传递对象指向其站点的接口的指针。
        STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);//检索使用SETSITE传递的最新站点。

        // IOleWindow
        STDMETHOD(GetWindow)(HWND* phwnd);//检索参与就地激活的一个窗口（框架、文档、父窗口或就地对象窗口）的句柄。
        STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);//确定是否应在就地激活会话期间输入上下文敏感的帮助模式。

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
        //通知容器控件的ControlINFO结构已经更改，并且容器应该调用控件的IOleControl::GetControlInfo进行更新。
        STDMETHOD(OnControlInfoChanged)(void);
        //指示控件是否应保持在活动状态。此方法的调用通常嵌套一个事件，以确保对象的激活状态在事件的整个处理过程中保持稳定。
        STDMETHOD(LockInPlaceActive)(BOOL bLock);
        //检索指向容器用于包装实际控件的扩展控件的IDispatch指针。
        STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);
        //将用HIMETRIC单位（如OLE中的标准）表示的坐标转换为容器指定的单位。
        STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);
        //将击键传递到控制站点进行处理。
        STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
        //指示此控制站点管理的控件是否已获得焦点或已失去焦点。
        STDMETHOD(OnFocus)(BOOL bGotFocus);
        //指示容器显示嵌入在此站点中的控件的属性表。
        STDMETHOD(ShowPropertyFrame)(void);

        // IOleClientSite
        STDMETHOD(SaveObject)(void);  //保存嵌入对象     
        STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);//请求组件名
        STDMETHOD(GetContainer)(IOleContainer** ppContainer);   //请求组件包容器指针
        STDMETHOD(ShowObject)(void);        //要求包容器显示对象
        STDMETHOD(OnShowWindow)(BOOL fShow);        //在组件变为可视或不可视时通知包容器
        STDMETHOD(RequestNewObjectLayout)(void);//要求包容器更改显示位置

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
        STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID) override;	//浏览器消息过滤
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
		// 消息分发给WebBrowser
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
        virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数
        virtual void OnDetach(HWND hOldWnd);//当控件离开一个窗口时，会调用一次该函数 
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
		
        LONG m_dwRef;//引用计数
        IWebBrowser2*	m_pWebBrowser; //浏览器指针
        IHTMLWindow2*	m_pHtmlWnd2;
        //IViewObject * m_pViewObject;
        IOleObject * m_pOleObject;
        IUnknown * m_pUnkSite;
        IOleInPlaceObject* m_pOleInPlaceObject;
        CWebBrowserEventHandler* m_pWebBrowserEventHandler;	//浏览器事件处理
        
		DWORD m_dwCookie;
		CMuiString m_strHomePage;	// 默认页面
        CMuiString m_strUrl;
		bool m_bAutoNavi;	// 是否启动时打开默认页面
        RECT m_rcPos;//绝对位置
	};
} // namespace MYUI
#endif // __UIWEBBROWSER_H__