
#include "UIWebBrowser.h"
#include <atlconv.h>
#include <atlcomcli.h>
#include "DownLoadMgr.h"
#include <mshtml.h>


namespace MYUI
{
	CWebBrowserUI::CWebBrowserUI()
        : m_dwRef(1)
		, m_pWebBrowser(NULL)
		, m_pHtmlWnd2(NULL)
        //, m_pViewObject(NULL)
        , m_pOleObject(NULL)
        , m_pUnkSite(NULL)
        , m_pOleInPlaceObject(NULL)
		, m_pWebBrowserEventHandler(NULL)
        , m_dwCookie(0)
		, m_bAutoNavi(false)
	{
		m_strHomePage.Empty();
        EmptyRect(m_rcPos);
	}

	CWebBrowserUI::~CWebBrowserUI()
	{
	}

	IID CWebBrowserUI::OBJCLSID = CLSID_WebBrowser;
	CMuiString CWebBrowserUI::g_strClassName = _T("WebBrowserUI");

	CMuiString CWebBrowserUI::GetClassName() const
	{
		return CWebBrowserUI::g_strClassName;
	}

	void CWebBrowserUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("url")))
		{
			Navigate2(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("AutoNavi")))
		{
#ifdef _DEBUG
			MUIASSERT(CheckBoer(strValue));
#endif
			SetAutoNavigation(CheckTrue(strValue));
		}
		else if(0 == _tcsicmp(strItem, _T("HomePage")))
		{
			SetHomePage(strValue);
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
		return;
	}

    HRESULT CWebBrowserUI::GetControl(const IID iid, LPVOID *ppRet)
    {
        MUIASSERT(ppRet != NULL);
        MUIASSERT(*ppRet == NULL);

        if (NULL == ppRet) return E_POINTER;
        if (NULL == m_pOleObject) return E_PENDING;

        return m_pOleObject->QueryInterface(iid, (LPVOID*)ppRet);
    }

	bool CWebBrowserUI::CreateControl()
	{
        HRESULT hr = -1;
        IOleControl * pOleControl = NULL;
        
        hr = ::OleCreate(CLSID_WebBrowser,IID_IOleObject, OLERENDER_DRAW, 0, this, this, (void**)&m_pOleObject);
        /*
        hr = ::CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*)&pOleControl);

        ASSERT(SUCCEEDED(hr) && "CWebBrowserUI::CreateControl");
        if (FAILED(hr)) return false;

        pOleControl->QueryInterface(IID_IOleObject, (LPVOID*)&m_pOleObject);
        pOleControl->Release();
        */

        if (NULL == m_pOleObject) return false;

        
        //hr = m_pOleObject->SetClientSite(this);
        
        IObjectWithSite * pSite = NULL;
        m_pOleObject->QueryInterface(IID_IObjectWithSite, (LPVOID*)&pSite);
        if (pSite)
        {
            pSite->SetSite(static_cast<IOleClientSite*>(this));
            pSite->Release();
        }
        hr = OleSetContainedObject(m_pOleObject, TRUE);

        DWORD dwMiscStatus = 0;
        m_pOleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
        IOleClientSite * pOleClientSite = NULL;
        this->QueryInterface(IID_IOleClientSite, (LPVOID*)&pOleClientSite);
        CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;

        //init
        if ((OLEMISC_SETCLIENTSITEFIRST & dwMiscStatus))
        {
            m_pOleObject->SetClientSite(pOleClientSite);
        }

        IPersistStreamInit * pPersistStreamInit = NULL;
        m_pOleObject->QueryInterface(IID_IPersistStreamInit, (LPVOID*)&pPersistStreamInit);
        if (pPersistStreamInit)
        {
            hr = pPersistStreamInit->InitNew();
            pPersistStreamInit->Release();
        }
        
        if (FAILED(hr)) return false;
        
        //hr = m_pOleObject->QueryInterface(IID_IViewObjectEx, (LPVOID*)&m_pViewObject);
        //if (FAILED(hr)) hr = m_pOleObject->QueryInterface(IID_IViewObject2, (LPVOID*)&m_pViewObject);
        //if (FAILED(hr)) hr = m_pOleObject->QueryInterface(IID_IViewObject, (LPVOID*)&m_pViewObject);

        m_pOleObject->SetHostNames(OLESTR("CWebBrowserUI"), NULL);
        
        if (OLEMISC_INVISIBLEATRUNTIME & dwMiscStatus)
        {
            hr = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pOleClientSite, 0,
                m_pShareInfo->hWnd, &m_rcPos);
        }
        
        IOleObject * pOleObject = NULL;
        this->GetControl(IID_IOleObject, (LPVOID*) &pOleObject);

        while(pOleObject)
        {
            CSafeRelease<IOleObject> RefOleObject = pOleObject;
            DWORD dwMiscStatus = 0;
            pOleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
            if((dwMiscStatus & OLEMISC_NOUIACTIVATE)) break;
            if (!m_pOleInPlaceObject) DoVerb(OLEIVERB_INPLACEACTIVATE);
            break;
        }


        //hr = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0,GETHWND(this), &m_rcPos);

        if (SUCCEEDED(hr))
		{
            m_pOleObject->QueryInterface(IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser);
			RegisterEventHandler(TRUE);
			return true;
		}
		
        return false;
	}

	void CWebBrowserUI::ReleaseControl()
	{
		RegisterEventHandler(FALSE);
        IObjectWithSite * pSite = NULL;

        if (m_pWebBrowser)
        {
            m_pWebBrowser->Release();
            m_pWebBrowser = NULL;
        }

        if (m_pHtmlWnd2)
        {
            m_pHtmlWnd2->Release();
            m_pHtmlWnd2 = NULL;
        }

        if (m_pOleInPlaceObject)
        {
            m_pOleInPlaceObject->Release();
            m_pOleInPlaceObject = NULL;
        }

        //if (m_pViewObject)
        //{
        //    m_pViewObject->Release();
        //    m_pViewObject = NULL;
        //}

        if (m_pOleObject)
        {
            m_pOleObject->QueryInterface(IID_IObjectWithSite, (LPVOID *)&pSite);
            if (pSite != NULL)
            {
                pSite->SetSite(NULL);
                pSite->Release();
            }
            m_pOleObject->Close(OLECLOSE_NOSAVE);
            m_pOleObject->SetClientSite(NULL);
            m_pOleObject->Release();
            m_pOleObject = NULL;
        }

        if (m_pUnkSite)
        {
            m_pUnkSite->Release();
            m_pUnkSite = NULL;
        }
	}

    HRESULT CWebBrowserUI::RegisterEventHandler(BOOL inAdvise)
    {
        MUITRACE(_T("CWebBrowserUI::RegisterEventHandler"));
        CComPtr<IWebBrowser2> pWebBrowser;
        CComPtr<IConnectionPointContainer>  pCPC;
        CComPtr<IConnectionPoint> pCP;
        HRESULT hr = this->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
        if (FAILED(hr)) return hr;

        hr = pWebBrowser->QueryInterface(IID_IConnectionPointContainer, (void **)&pCPC);

        if (FAILED(hr)) return hr;

        hr = pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &pCP);

        if (FAILED(hr)) return hr;

        if (inAdvise)
        {
            hr = pCP->Advise((IDispatch*)this, &m_dwCookie);
        }
        else
        {
            hr = pCP->Unadvise(m_dwCookie);
        }
        return hr;
    }

    bool CWebBrowserUI::OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate)
    {
        //if (false == m_bVisible || TRUE == IsEmptyRect(rcPaint)) return false;
        //PaintBkColor(rcItem, rcPaint);
        //PaintBkImage(rcItem, rcPaint);
        return true;
    }

    LRESULT CWebBrowserUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        POINT point;
        LRESULT lResult = 0;
        SIZEL hmSize = { 0 };
        SIZEL pxSize = { 0 };
        RECT rcItem = { 0 };
        //if(NULL == m_bInPlaceActive) return false;
        MUITRACE(_T("AX: CWebBrowserUI::WndProc"));

        //if (message >= WM_MOUSEFIRST && WM_MOUSELAST >= message)
        //{
        //    point.x = GET_X_LPARAM(lParam);
        //    point.y = GET_Y_LPARAM(lParam);
        //    DWORD dwHitResult = m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;

        //    if (HITRESULT_OUTSIDE == dwHitResult && NULL != m_pViewObject)
        //    {
        //        IViewObjectEx * pViewEx = NULL;
        //        m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*)&pViewEx);
        //        if (pViewEx)
        //        {
        //            pViewEx->QueryHitPoint(DVASPECT_CONTENT, &m_rcPos, point, 0, &dwHitResult);
        //            pViewEx->Release();
        //        }
        //    }
        //}

        //if (NULL == m_pOleInPlaceObject) return lResult;

        //HRESULT hr = m_pOleInPlaceObject->OnWindowMessage(message, wParam, lParam, &lResult);
        return lResult;
    }

    void CWebBrowserUI::OnAttach(HWND hNewWnd)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OnAttach"));
        this->CreateControl();

        if (!m_strUrl.IsEmpty())
        {
            this->Navigate2(m_strUrl);
        }
        else if (m_bAutoNavi && !m_strHomePage.IsEmpty())
        {
            this->Navigate2(m_strHomePage);
        }
    }

    void CWebBrowserUI::OnDetach(HWND hOldWnd)
    {
        MUITRACE(_T("AX: CActiveXUI::OnDetach"));
        //我老实告诉大家，这样写我也不知道对不对，因为我对COM不了解，这里可能需要大家测试改正
        //先跟大家说一说OnAttach跟OnDetach函数是干嘛用的。一般我们都是先创建UI对象，再附加到
        //窗口中的，而有些控件在new命令中是无法初始化的，因为那个时候还不知道宿主窗口是谁，
        //所以当UI对象附加到一个窗口时，就会执行OnAttach回调，这个时候我们就可以根据宿主窗口
        //来创建我们的一些特殊对象了。而OnDetach也很明显的，就是当UI对象离开一个窗口时，它就
        //会被调用，这个时候我们就应该释放我们在OnAttach创建的特殊对象了。那OnDetach一般在什
        //么情况下发生呢？答案是，窗口关闭和控件移动到另一个窗口的时候，窗口关闭WM_CLOSE,想
        //必大家都知道怎么回事了，那移动到另一个窗口是怎么回事呢？因为在本界面的设计中，控件
        //(UI对象)可作为独立存在的对象, 客人(UI对象)可以随意在各个房间(窗口)内走动，只要遵守
        //先离开一个房间，才能进入一个新的房间的规范便可。
        //下面的方法之所以可能是错误的，UI库也能经常能运行的原因是因为，我们很少把A窗口的控件
        //移动到B窗口，如果真的发生这种情况的话，就不能100%保证CActiveXUI正确，所以还请大家改
        //正和指教，有问题别忘记联系我的QQ ： 492927689

        DoVerb(OLEIVERB_HIDE);
        this->ReleaseControl();
    }

    HRESULT CWebBrowserUI::DoVerb(LONG nVerb)
    {
        /*********************************************************************************
        * DoVerb ：执行指定的动作
        *	1L	显示编辑的对象。			OLEIVERB_SHOW
        *	0L	主谓词						OLEIVERB_PRIMARY
        *	-1	辅助谓词					（无）
        *	-2	编辑在单独的窗口中的项。	OLEIVERB_OPEN
        *	-3	隐藏对象。					OLEIVERB_HIDE
        *	-4	激活就地的控件。			OLEIVERB_UIACTIVATE
        *	-5	激活就地控件，而无需其他用户界面元素。		OLEIVERB_INPLACEACTIVATE
        *	-7	显示该控件的属性。			OLEIVERB_PROPERTIES
        **********************************************************************************/
        HRESULT hr = S_FALSE;
        if (NULL == m_pOleObject) return hr;

        hr = m_pOleObject->DoVerb(nVerb, NULL, this, -1, GETHWND(this), &m_rcPos);
        return hr;
    }

    /************************************************************************
    * WEBBROWSER 静态函数
    *************************************************************************/
    DISPID CWebBrowserUI::FindId(IDispatch *pObj, LPOLESTR pName)
    {
        MUITRACE(_T("CWebBrowserUI::FindId"));
        DISPID id = 0;
        if (FAILED(pObj->GetIDsOfNames(IID_NULL, &pName, 1, LOCALE_SYSTEM_DEFAULT, &id))) id = -1;
        return id;
    }

    HRESULT CWebBrowserUI::InvokeMethod(IDispatch *pObj, LPOLESTR pMehtod, VARIANT *pVarResult, VARIANT *ps, int cArgs)
    {
        MUITRACE(_T("CWebBrowserUI::InvokeMethod"));
        DISPID dispid = FindId(pObj, pMehtod);
        if (dispid == -1) return E_FAIL;

        DISPPARAMS dispparams;
        dispparams.cArgs = cArgs;
        dispparams.rgvarg = ps;
        dispparams.cNamedArgs = 0;
        dispparams.rgdispidNamedArgs = NULL;

        return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispparams, pVarResult, NULL, NULL);
    }

    HRESULT CWebBrowserUI::SetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
    {
        MUITRACE(_T("CWebBrowserUI::SetProperty"));
        DISPID dispid = FindId(pObj, pName);
        if (dispid == -1) return E_FAIL;

        DISPPARAMS ps;
        ps.cArgs = 1;
        ps.rgvarg = pValue;
        ps.cNamedArgs = 0;
        ps.rgdispidNamedArgs = NULL;

        return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &ps, NULL, NULL, NULL);
    }

    HRESULT CWebBrowserUI::GetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
    {
        MUITRACE(_T("CWebBrowserUI::GetProperty"));
        DISPID dispid = FindId(pObj, pName);
        if (dispid == -1) return E_FAIL;

        DISPPARAMS ps;
        ps.cArgs = 0;
        ps.rgvarg = NULL;
        ps.cNamedArgs = 0;
        ps.rgdispidNamedArgs = NULL;

        return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &ps, pValue, NULL, NULL);
    }


    /************************************************************************
    * WEBBROWSER 对外接口
    *************************************************************************/
    void CWebBrowserUI::SetHomePage(LPCTSTR lpszUrl)
    {
        MUITRACE(_T("CWebBrowserUI::SetHomePage"));
        m_strHomePage.Format(_T("%s"), lpszUrl);
    }

    LPCTSTR CWebBrowserUI::GetHomePage()
    {
        MUITRACE(_T("CWebBrowserUI::GetHomePage"));
        return m_strHomePage;
    }

    void CWebBrowserUI::SetAutoNavigation(bool bAuto /*= TRUE*/)
    {
        MUITRACE(_T("CWebBrowserUI::SetAutoNavigation"));
        if (m_bAutoNavi == bAuto)	return;

        m_bAutoNavi = bAuto;
    }

    bool CWebBrowserUI::IsAutoNavigation()
    {
        MUITRACE(_T("CWebBrowserUI::IsAutoNavigation"));
        return m_bAutoNavi;
    }

    void CWebBrowserUI::Navigate2(LPCTSTR lpszUrl)
    {
        MUITRACE(_T("CWebBrowserUI::Navigate2"));
        if (lpszUrl == NULL) return;

        if (m_pWebBrowser)
        {
            CVariant url;
            url.vt = VT_BSTR;
            url.bstrVal = T2BSTR(lpszUrl);
            HRESULT hr = m_pWebBrowser->Navigate2(&url, NULL, NULL, NULL, NULL);
            m_strUrl.Empty();
        }
        else
        {
            m_strUrl = lpszUrl;
        }
    }

    void CWebBrowserUI::Refresh()
    {
        MUITRACE(_T("CWebBrowserUI::Refresh"));
        if (m_pWebBrowser)
        {
            m_pWebBrowser->Refresh();
        }
    }

    void CWebBrowserUI::Refresh2(int Level)
    {
        MUITRACE(_T("CWebBrowserUI::Refresh2"));
        CVariant vLevel;
        vLevel.vt = VT_I4;
        vLevel.intVal = Level;
        m_pWebBrowser->Refresh2(&vLevel);
    }

    void CWebBrowserUI::GoBack()
    {
        MUITRACE(_T("CWebBrowserUI::GoBack"));
        if (m_pWebBrowser)
        {
            m_pWebBrowser->GoBack();
        }
    }

    void CWebBrowserUI::GoForward()
    {
        MUITRACE(_T("CWebBrowserUI::GoForward"));
        if (m_pWebBrowser)
        {
            m_pWebBrowser->GoForward();
        }
    }

    void CWebBrowserUI::NavigateHomePage()
    {
        MUITRACE(_T("CWebBrowserUI::NavigateHomePage"));
        if (!m_strHomePage.IsEmpty())
            this->NavigateUrl(m_strHomePage);
    }

    void CWebBrowserUI::NavigateUrl(LPCTSTR lpszUrl)
    {
        MUITRACE(_T("CWebBrowserUI::NavigateUrl"));
        if (m_pWebBrowser && lpszUrl)
        {
            m_pWebBrowser->Navigate((BSTR)SysAllocString(T2BSTR(lpszUrl)), NULL, NULL, NULL, NULL);
        }
    }

    IWebBrowser2* CWebBrowserUI::GetWebBrowser2(void)
    {
        MUITRACE(_T("CWebBrowserUI::GetWebBrowser2"));
        return m_pWebBrowser;
    }

    IDispatch* CWebBrowserUI::GetHtmlWindow()
    {
        MUITRACE(_T("CWebBrowserUI::GetHtmlWindow"));
        IDispatch* pDp = NULL;
        HRESULT hr = S_FALSE;
        if (m_pWebBrowser)
            hr = m_pWebBrowser->get_Document(&pDp);

        if (FAILED(hr))
            return NULL;

        CComQIPtr<IHTMLDocument2> pHtmlDoc2 = pDp;

        if (pHtmlDoc2 == NULL)
            return NULL;

        hr = pHtmlDoc2->get_parentWindow(&m_pHtmlWnd2);

        if (FAILED(hr))
            return NULL;

        IDispatch *pHtmlWindown = NULL;
        hr = m_pHtmlWnd2->QueryInterface(IID_IDispatch, (void**)&pHtmlWindown);
        if (FAILED(hr))
            return NULL;

        return pHtmlWindown;
    }

    void CWebBrowserUI::SetVisible(bool bVisible)
    {
        if (bVisible == m_bVisible) return;
        __super::SetVisible(bVisible);

        if (true == m_bVisible)
        {
            DoVerb(OLEIVERB_SHOW);
        }
        else
        {
            DoVerb(OLEIVERB_HIDE);
        }
    }

    bool CWebBrowserUI::SetItem(RECT rcItem, bool bMustUpdate)
    {
        SIZEL hmSize = { 0 };
        SIZEL pxSize = { 0 };

        if (false == __super::SetItem(rcItem, bMustUpdate)) return false;

        GetItemFixedRect(rcItem);

        if (TRUE == IsSameRect(rcItem, m_rcPos)) return true;
        this->OnPosRectChange(&rcItem);

        return true;
    }

    /************************************************************************
     * COM 组件接口实例化 BEGIN
    *************************************************************************/

    // IUnknown
    STDMETHODIMP_(ULONG) CWebBrowserUI::AddRef()
    {
        //TRACE(_T("AX: CWebBrowserUI::AddRef %ld"), m_dwRef);
        InterlockedIncrement(&m_dwRef);
        return m_dwRef;
    }

    STDMETHODIMP_(ULONG) CWebBrowserUI::Release()
    {
        //TRACE(_T("AX: CWebBrowserUI::Release %ld"), m_dwRef);
        ULONG ulRefCount = InterlockedDecrement(&m_dwRef);
        //if (ulRefCount == 0) delete this;
        return ulRefCount;
    }

    STDMETHODIMP CWebBrowserUI::QueryInterface(REFIID riid, LPVOID *ppvObject)
    {
        //TRACE(_T("AX: CWebBrowserUI::QueryInterface"));
        *ppvObject = NULL;

        if (riid == IID_IUnknown) *ppvObject = (this);
        else if (riid == IID_IObjectWithSite) *ppvObject = static_cast<IObjectWithSite*>(this);
        else if (riid == IID_IOleClientSite) *ppvObject = static_cast<IOleClientSite*>(this);
        else if (riid == IID_IOleInPlaceSite) *ppvObject = static_cast<IOleInPlaceSite*>(this);
        else if (riid == IID_IOleControlSite) *ppvObject = static_cast<IOleControlSite*>(this);
        else if (riid == IID_IStorage) *ppvObject = static_cast<IStorage*>(this);
        else if (riid == IID_IOleCommandTarget) *ppvObject = static_cast<IOleCommandTarget*>(this);
        else if (riid == IID_IDocHostUIHandler) *ppvObject = static_cast<IDocHostUIHandler*>(this);
        else if (riid == IID_IServiceProvider) *ppvObject = static_cast<IServiceProvider*>(this);
        else if (riid == IID_IDispatch) *ppvObject = static_cast<IDispatch*>(this);
        else if (riid == IID_IOleWindow) *ppvObject = static_cast<IOleWindow*>(this);
        else { return E_NOINTERFACE; }

        this->AddRef();
        return S_OK;
    }

    // IObjectWithSite
    STDMETHODIMP CWebBrowserUI::SetSite(IUnknown *pUnkSite)
    {
        MUITRACE(_T("AX: CActiveXCtrl::SetSite"));
        if (NULL != m_pUnkSite)
        {
            m_pUnkSite->Release();
            m_pUnkSite = NULL;
        }
        if (NULL != pUnkSite)
        {
            m_pUnkSite = pUnkSite;
            m_pUnkSite->AddRef();
        }
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::GetSite(REFIID  riid, LPVOID * ppvSite)
    {
        MUITRACE(_T("AX: CActiveXCtrl::GetSite"));
        if (NULL == ppvSite) return E_POINTER;
        *ppvSite = NULL;
        if (NULL == m_pUnkSite) return E_FAIL;
        return m_pUnkSite->QueryInterface(riid, ppvSite);
    }

    // IOleWindow
    STDMETHODIMP CWebBrowserUI::GetWindow(HWND *phWnd)
    {
        MUITRACE(_T("AX: CWebBrowserUI::GetWindow"));
        if (NULL == m_pShareInfo || NULL == m_pShareInfo->hWnd) return E_FAIL;
        *phWnd = m_pShareInfo->hWnd;
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::ContextSensitiveHelp(BOOL bEnterMode)
    {
        MUITRACE(_T("AX: CWebBrowserUI::ContextSensitiveHelp"));
        return S_OK;
    }

    //IOleInPlaceSite
    HRESULT STDMETHODCALLTYPE CWebBrowserUI::CanInPlaceActivate(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::CanInPlaceActivate"));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::OnInPlaceActivate(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OnInPlaceActivate"));
        OleLockRunning(m_pOleObject, TRUE, FALSE);
        m_pOleObject->QueryInterface(&m_pOleInPlaceObject);
        m_pOleInPlaceObject->SetObjectRects(&m_rcPos, &m_rcPos);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::OnUIActivate(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OnUIActivate"));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::GetWindowContext(
        __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
        __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
        __RPC__out LPRECT lprcPosRect,
        __RPC__out LPRECT lprcClipRect,
        __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)
    {
        MUITRACE(_T("AX: CWebBrowserUI::GetWindowContext"));
        HWND hwnd = GETHWND(this);

        if (NULL == ppDoc) return E_POINTER;
        if (NULL == ppFrame) return E_POINTER;
        if (NULL == lprcPosRect) return E_POINTER;
        if (NULL == lprcClipRect) return E_POINTER;

        (*ppFrame) = NULL;
        (*ppDoc) = NULL;
        *lprcPosRect = m_rcPos;

        lpFrameInfo->fMDIApp = false;
        lpFrameInfo->hwndFrame = hwnd;
        lpFrameInfo->haccel = NULL;
        lpFrameInfo->cAccelEntries = 0;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::Scroll(SIZE scrollExtant)
    {
        MUITRACE(_T("AX: CWebBrowserUI::Scroll"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::OnUIDeactivate(BOOL fUndoable)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OnUIDeactivate"));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::OnInPlaceDeactivate(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OnInPlaceDeactivate"));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::DiscardUndoState(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::DiscardUndoState"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::DeactivateAndUndo(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::DeactivateAndUndo"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::OnPosRectChange(__RPC__in LPCRECT lprcPosRect)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OnPosRectChange"));
        SIZEL hmSize = { 0 };
        SIZEL pxSize = { 0 };

        m_rcPos = *lprcPosRect;

        if (m_pOleObject)
        {
            pxSize.cx = m_rcPos.right - m_rcPos.left;
            pxSize.cy = m_rcPos.bottom - m_rcPos.top;
            if (GETHWND(this)) ::PixelToHiMetric(GETHWND(this), &pxSize, &hmSize);
            
            m_pOleObject->SetExtent(DVASPECT_CONTENT, &hmSize);
        }

        if (m_pOleInPlaceObject)
        {
            m_pOleInPlaceObject->SetObjectRects(&m_rcPos, &m_rcPos);
        }
        return E_NOTIMPL;
    }

    // IOleControlSite
    STDMETHODIMP CWebBrowserUI::OnControlInfoChanged(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OnControlInfoChanged"));
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::LockInPlaceActive(BOOL bLock)
    {
        MUITRACE(_T("AX: CWebBrowserUI::LockInPlaceActive"));
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::GetExtendedControl(IDispatch** ppDisp)
    {
        MUITRACE(_T("AX: CWebBrowserUI::GetExtendedControl"));
        if (NULL == ppDisp) return E_POINTER;
        if (NULL == m_pOleObject) return E_UNEXPECTED;
        return m_pOleObject->QueryInterface(IID_IDispatch, (LPVOID*)ppDisp);
    }

    STDMETHODIMP CWebBrowserUI::TransformCoords(POINTL * pPtlHimetric, POINTF *pPtfContainer, DWORD dwFlags)
    {
        MUITRACE(_T("AX: CWebBrowserUI::TransformCoords"));
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
    {
        MUITRACE(_T("AX: CWebBrowserUI::TranslateAccelerator"));
        return S_FALSE;
    }

    STDMETHODIMP CWebBrowserUI::OnFocus(BOOL bGotFocus)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OnFocus"));

        if (bGotFocus)
        {
            AddState(STATE_FOCUS);
        }
        else
        {
            RemoveState(STATE_FOCUS);
        }

        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::ShowPropertyFrame(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::ShowPropertyFrame"));
        return E_NOTIMPL;
    }

    //IOleClientSite
    STDMETHODIMP CWebBrowserUI::SaveObject(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::SaveObject"));
        return E_NOTIMPL;
    }

    STDMETHODIMP CWebBrowserUI::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk)
    {
        MUITRACE(_T("AX: CWebBrowserUI::GetMoniker"));
        if ((dwAssign == OLEGETMONIKER_ONLYIFTHERE) && (dwWhichMoniker == OLEWHICHMK_CONTAINER))
            return E_FAIL;
        //if (ppmk) *ppmk = NULL;
        return E_NOTIMPL;
    }

    STDMETHODIMP CWebBrowserUI::GetContainer(IOleContainer ** ppContainer)
    {
        MUITRACE(_T("AX: CWebBrowserUI::GetContainer"));
        *ppContainer = NULL;
        return E_NOINTERFACE;
    }

    STDMETHODIMP CWebBrowserUI::ShowObject(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::ShowObject"));
        if (NULL == m_pShareInfo || NULL == m_pShareInfo->hWnd) return E_UNEXPECTED;

        /*HDC hdc = ::GetDC(m_hHostWindow);
        if(NULL == hdc) return E_FAIL;
        if(NULL != m_pViewObject) m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL,NULL,
        hdc, (RECTL*) &m_rcPos, (RECTL*) &m_rcPos, NULL, NULL);
        ::ReleaseDC(m_hHostWindow, hdc);*/

        //::InvalidateRect(m_pShareInfo->hWnd, &m_rcPos, FALSE);
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::OnShowWindow(BOOL bShow)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OnShowWindow"));
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::RequestNewObjectLayout(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::RequestNewObjectLayout"));
        return E_NOTIMPL;
    }

    // IStorage
    HRESULT STDMETHODCALLTYPE CWebBrowserUI::CreateStream(const OLECHAR *pwcsName,
        DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm)
    {
        MUITRACE(_T("AX: CWebBrowserUI::CreateStream"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::OpenStream(const OLECHAR *pwcsName,
        void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OpenStream"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::CreateStorage(const OLECHAR *pwcsName,
        DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg)
    {
        MUITRACE(_T("AX: CWebBrowserUI::CreateStorage"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::OpenStorage(const OLECHAR *pwcsName,
        IStorage *pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg)
    {
        MUITRACE(_T("AX: CWebBrowserUI::OpenStorage"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::CopyTo(DWORD ciidExclude,
        const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest)
    {
        MUITRACE(_T("AX: CWebBrowserUI::CopyTo"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::MoveElementTo(const OLECHAR *pwcsName,
        IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags)
    {
        MUITRACE(_T("AX: CWebBrowserUI::MoveElementTo"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::Commit(DWORD grfCommitFlags)
    {
        MUITRACE(_T("AX: CWebBrowserUI::Commit"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::Revert(void)
    {
        MUITRACE(_T("AX: CWebBrowserUI::Revert"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::EnumElements(DWORD reserved1,
        void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum)
    {
        MUITRACE(_T("AX: CWebBrowserUI::EnumElements"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::DestroyElement(const OLECHAR *pwcsName)
    {
        MUITRACE(_T("AX: CWebBrowserUI::DestroyElement"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::RenameElement(const OLECHAR *pwcsOldName, const OLECHAR *pwcsNewName)
    {
        MUITRACE(_T("AX: CWebBrowserUI::RenameElement"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::SetElementTimes(const OLECHAR *pwcsName,
        const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime)
    {
        MUITRACE(_T("AX: CWebBrowserUI::SetElementTimes"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::SetClass(REFCLSID clsid)
    {
        MUITRACE(_T("AX: CWebBrowserUI::SetClass"));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::SetStateBits(DWORD grfStateBits, DWORD grfMask)
    {
        MUITRACE(_T("AX: CWebBrowserUI::SetStateBits"));
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
    {
        MUITRACE(_T("AX: CWebBrowserUI::Stat"));
        return E_NOTIMPL;
    }

    // IOleCommandTarget
    HRESULT STDMETHODCALLTYPE CWebBrowserUI::QueryStatus(__RPC__in_opt const GUID *pguidCmdGroup, 
        ULONG cCmds, __RPC__inout_ecount_full(cCmds) OLECMD prgCmds[], __RPC__inout_opt OLECMDTEXT *pCmdText)
    {
        MUITRACE(_T("CWebBrowserUI::QueryStatus"));
        //HRESULT hr = OLECMDERR_E_NOTSUPPORTED;
        //return hr;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CWebBrowserUI::Exec(__RPC__in_opt const GUID *pguidCmdGroup, 
        DWORD nCmdID, DWORD nCmdexecopt, __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut)
    {
        //TRACE(_T("CWebBrowserUI::Exec")); 这个调用非常频繁，所以先屏蔽掉
        HRESULT hr = OLECMDERR_E_UNKNOWNGROUP;
        if (!pguidCmdGroup || !IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler))
            return OLECMDERR_E_UNKNOWNGROUP;

        switch (nCmdID)
        {
        case OLECMDID_SHOWSCRIPTERROR:// 屏蔽脚本错误的对话框
        {
            IHTMLDocument2*             pDoc = NULL;
            IHTMLWindow2*               pWindow = NULL;
            IHTMLEventObj*              pEventObj = NULL;
            BSTR                        rgwszNames[5] =
            {
                SysAllocString(L"errorLine"),
                SysAllocString(L"errorCharacter"),
                SysAllocString(L"errorCode"),
                SysAllocString(L"errorMessage"),
                SysAllocString(L"errorUrl")
            };
            DISPID                      rgDispIDs[5];
            VARIANT                     rgvaEventInfo[5];
            DISPPARAMS                  params;
            BOOL                        bContinueRunningScripts = true;
            int                         i;

            params.cArgs = 0;
            params.cNamedArgs = 0;

            // Get the document that is currently being viewed.
            hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, (void **)&pDoc);
            // Get document.parentWindow.
            hr = pDoc->get_parentWindow(&pWindow);
            pDoc->Release();
            // Get the window.event object.
            hr = pWindow->get_event(&pEventObj);
            // Get the error info from the window.event object.
            for (i = 0; i < 5; i++)
            {
                // Get the property's dispID.
                hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1,
                    LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);
                // Get the value of the property.
                hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL,
                    LOCALE_SYSTEM_DEFAULT,
                    DISPATCH_PROPERTYGET, &params, &rgvaEventInfo[i],
                    NULL, NULL);
                SysFreeString(rgwszNames[i]);
            }

            // At this point, you would normally alert the user with 
            // the information about the error, which is now contained
            // in rgvaEventInfo[]. Or, you could just exit silently.

            (*pvaOut).vt = VT_BOOL;
            if (bContinueRunningScripts)
            {
                // Continue running scripts on the page.
                (*pvaOut).boolVal = VARIANT_TRUE;
            }
            else
            {
                // Stop running scripts on the page.
                (*pvaOut).boolVal = VARIANT_FALSE;
            }
            break;
        }
        //case OLECMDID_SHOWSCRIPTERROR:
        //{
        //    // 屏蔽脚本错误的对话框
        //    (*pvaOut).vt = VT_BOOL;
        //    (*pvaOut).boolVal = VARIANT_TRUE;
        //    return S_OK;
        //}break;
        case OLECMDID_FIND:// 不弹出查找框
        {
            hr = S_OK;
        }break;
        case OLECMDID_SAVEAS:// 不要弹保存对话框
        {
            hr = S_OK;
        }break;
        case OLECMDID_SHOWMESSAGE:
        {
            (*pvaOut).vt = VT_BOOL;
            (*pvaOut).boolVal = VARIANT_TRUE;
            hr = S_OK;
        }break;
        default:
        {
            hr = OLECMDERR_E_NOTSUPPORTED;
        }break;
        }

        return hr;
    }

    // IDocHostUIHandler
    STDMETHODIMP CWebBrowserUI::ShowContextMenu(DWORD dwID, POINT* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit)
    {
        MUITRACE(_T("CWebBrowserUI::ShowContextMenu"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->ShowContextMenu(dwID, pptPosition, pCommandTarget, pDispatchObjectHit);
        }
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::GetHostInfo(DOCHOSTUIINFO* pInfo)
    {
        //选中文本
        MUITRACE(_T("CWebBrowserUI::GetHostInfo"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->GetHostInfo(pInfo);
        }
        else
        {
            pInfo->cbSize = sizeof(DOCHOSTUIINFO);
            pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
            pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_THEME | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE;
            return S_OK;
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP CWebBrowserUI::ShowUI(DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc)
    {
        MUITRACE(_T("CWebBrowserUI::ShowUI"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->ShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
        }
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::HideUI()
    {
        MUITRACE(_T("CWebBrowserUI::HideUI"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->HideUI();
        }
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::UpdateUI()
    {
        MUITRACE(_T("CWebBrowserUI::UpdateUI"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->UpdateUI();
        }
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::EnableModeless(BOOL fEnable)
    {
        MUITRACE(_T("CWebBrowserUI::EnableModeless"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->EnableModeless(fEnable);
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP CWebBrowserUI::OnDocWindowActivate(BOOL fActivate)
    {
        MUITRACE(_T("CWebBrowserUI::OnDocWindowActivate"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->OnDocWindowActivate(fActivate);
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP CWebBrowserUI::OnFrameWindowActivate(BOOL fActivate)
    {
        MUITRACE(_T("CWebBrowserUI::OnFrameWindowActivate"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->OnFrameWindowActivate(fActivate);
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP CWebBrowserUI::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fFrameWindow)
    {
        MUITRACE(_T("CWebBrowserUI::ResizeBorder"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->ResizeBorder(prcBorder, pUIWindow, fFrameWindow);
        }
        return S_OK;
    }

    STDMETHODIMP CWebBrowserUI::TranslateAccelerator(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID)
    {
        MUITRACE(_T("CWebBrowserUI::TranslateAccelerator"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
        }
        return S_FALSE;
    }

    STDMETHODIMP CWebBrowserUI::GetOptionKeyPath(LPOLESTR* pchKey, DWORD dwReserved)
    {
        MUITRACE(_T("CWebBrowserUI::GetOptionKeyPath"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->GetOptionKeyPath(pchKey, dwReserved);
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP CWebBrowserUI::GetDropTarget(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
    {
        MUITRACE(_T("CWebBrowserUI::GetDropTarget"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->GetDropTarget(pDropTarget, ppDropTarget);
        }
        return E_NOTIMPL;	// 使用系统拖拽
    }

    STDMETHODIMP CWebBrowserUI::GetExternal(IDispatch** ppDispatch)
    {
        MUITRACE(_T("CWebBrowserUI::GetExternal"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->GetExternal(ppDispatch);
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP CWebBrowserUI::TranslateUrl(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
    {
        MUITRACE(_T("CWebBrowserUI::TranslateUrl"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
        }

        return E_NOTIMPL;
    }

    STDMETHODIMP CWebBrowserUI::FilterDataObject(IDataObject* pDO, IDataObject** ppDORet)
    {
        MUITRACE(_T("CWebBrowserUI::FilterDataObject"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->FilterDataObject(pDO, ppDORet);
        }

        *ppDORet = 0;
        return E_NOTIMPL;
    }

    // IServiceProvider
    STDMETHODIMP CWebBrowserUI::QueryService(REFGUID guidService, REFIID riid, void** ppvObject)
    {
        MUITRACE(_T("CWebBrowserUI::QueryService"));
        HRESULT hr = E_NOINTERFACE;
        *ppvObject = NULL;

        if (guidService == SID_SDownloadManager && riid == IID_IDownloadManager)
        {
            *ppvObject = this;
            return S_OK;
        }

        return hr;
    }

    // IDownloadManager
    STDMETHODIMP CWebBrowserUI::Download( /* [in] */ IMoniker *pmk, /* [in] */ IBindCtx *pbc,
        /* [in] */ DWORD dwBindVerb, /* [in] */ LONG grfBINDF, /* [in] */ BINDINFO *pBindInfo, 
        /* [in] */ LPCOLESTR pszHeaders, /* [in] */ LPCOLESTR pszRedir, /* [in] */ UINT uiCP)
    {
        MUITRACE(_T("CWebBrowserUI::Download"));
        if (m_pWebBrowserEventHandler)
        {
            return m_pWebBrowserEventHandler->Download(pmk, pbc, dwBindVerb, grfBINDF, pBindInfo, pszHeaders, pszRedir, uiCP);
        }
        return S_OK;
    }

    // ITranslateAccelerator
    // 消息分发给WebBrowser
    LRESULT CWebBrowserUI::TranslateAccelerator(MSG *pMsg)
    {
        if (pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST)
            return S_FALSE;

        if (m_pWebBrowser == NULL)
            return E_NOTIMPL;

        // 当前Web窗口不是焦点,不处理加速键
        BOOL bIsChild = FALSE;
        HWND hTempWnd = NULL;
        HWND hWndFocus = ::GetFocus();

        hTempWnd = hWndFocus;
        while (hTempWnd != NULL)
        {
            if (hTempWnd == (m_pShareInfo ? m_pShareInfo->hWnd : NULL))
            {
                bIsChild = TRUE;
                break;
            }
            hTempWnd = ::GetParent(hTempWnd);
        }
        if (!bIsChild)
            return S_FALSE;

        IOleInPlaceActiveObject *pObj;
        if (FAILED(m_pWebBrowser->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID *)&pObj)))
            return S_FALSE;

        HRESULT hResult = pObj->TranslateAccelerator(pMsg);
        pObj->Release();
        return hResult;
    }

    // IDispatch
	STDMETHODIMP CWebBrowserUI::GetTypeInfoCount(UINT *iTInfo )
	{
		MUITRACE(_T("CWebBrowserUI::GetTypeInfoCount"));
        return S_OK;
	}

	STDMETHODIMP CWebBrowserUI::GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
	{
		MUITRACE(_T("CWebBrowserUI::GetTypeInfo"));
        return S_OK;
	}

	STDMETHODIMP CWebBrowserUI::GetIDsOfNames( REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid,DISPID *rgDispId )
	{
		MUITRACE(_T("CWebBrowserUI::GetIDsOfNames"));
        return DISP_E_UNKNOWNNAME;
	}

	STDMETHODIMP CWebBrowserUI::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid,WORD wFlags, DISPPARAMS* pDispParams,VARIANT* pVarResult, EXCEPINFO* pExcepInfo,UINT* puArgErr )
	{
		MUITRACE(_T("CWebBrowserUI::Invoke"));
        if (IID_NULL != riid) return E_INVALIDARG;

		switch(dispIdMember)
		{
		case DISPID_BEFORENAVIGATE2:
			BeforeNavigate2(
				pDispParams->rgvarg[6].pdispVal,
				pDispParams->rgvarg[5].pvarVal,
				pDispParams->rgvarg[4].pvarVal,
				pDispParams->rgvarg[3].pvarVal,
				pDispParams->rgvarg[2].pvarVal,
				pDispParams->rgvarg[1].pvarVal,
				pDispParams->rgvarg[0].pboolVal);
			break;
		case DISPID_COMMANDSTATECHANGE:
			CommandStateChange(
				pDispParams->rgvarg[1].lVal,
				pDispParams->rgvarg[0].boolVal);
			break;
		case DISPID_NAVIGATECOMPLETE2:
			NavigateComplete2(
				pDispParams->rgvarg[1].pdispVal,
				pDispParams->rgvarg[0].pvarVal);
			break;
        case DISPID_DOCUMENTCOMPLETE:
            DocumentComplete2(
                pDispParams->rgvarg[1].pdispVal,
                pDispParams->rgvarg[0].pvarVal);
            break;
		case DISPID_NAVIGATEERROR:
			NavigateError(
				pDispParams->rgvarg[4].pdispVal,
				pDispParams->rgvarg[3].pvarVal,
				pDispParams->rgvarg[2].pvarVal,
				pDispParams->rgvarg[1].pvarVal,
				pDispParams->rgvarg[0].pboolVal);
			break;
		case DISPID_STATUSTEXTCHANGE:
			break;
			//  	case DISPID_NEWWINDOW2:
			//  		break;
		case DISPID_NEWWINDOW3:
			NewWindow3(
				pDispParams->rgvarg[4].ppdispVal,
				pDispParams->rgvarg[3].pboolVal,
				pDispParams->rgvarg[2].uintVal,
				pDispParams->rgvarg[1].bstrVal,
				pDispParams->rgvarg[0].bstrVal);
			break;
	 	case DISPID_PROPERTYCHANGE:
	 		if (pDispParams->cArgs>0 && pDispParams->rgvarg[0].vt == VT_BSTR) 
            {
	 			MUITRACE(_T("PropertyChange(%s)\n"), pDispParams->rgvarg[0].bstrVal);
	 		}
	 		break;
		default:
			return DISP_E_MEMBERNOTFOUND;
		}
		return S_OK;
	}


    /************************************************************************
     * COM 组件 Invoke自动化接口封装 BEGIN
    *************************************************************************/
    // DWebBrowserEvents2
    void CWebBrowserUI::SetWebBrowserEventHandler(CWebBrowserEventHandler* pEventHandler)
    {
        MUITRACE(_T("CWebBrowserUI::SetWebBrowserEventHandler"));
        if (pEventHandler != NULL && m_pWebBrowserEventHandler != pEventHandler)
            m_pWebBrowserEventHandler = pEventHandler;
    }

    void CWebBrowserUI::BeforeNavigate2(IDispatch *pDisp, VARIANT *&url, VARIANT *&Flags, 
        VARIANT *&TargetFrameName, VARIANT *&PostData, VARIANT *&Headers, VARIANT_BOOL *&Cancel)
    {
        MUITRACE(_T("CWebBrowserUI::BeforeNavigate2"));
        if (m_pWebBrowserEventHandler)
        {
            m_pWebBrowserEventHandler->BeforeNavigate2(pDisp, url, Flags, TargetFrameName, PostData, Headers, Cancel);
        }
    }

    void CWebBrowserUI::CommandStateChange(long Command, VARIANT_BOOL Enable)
    {
        MUITRACE(_T("CWebBrowserUI::CommandStateChange"));
        if (m_pWebBrowserEventHandler)
        {
            m_pWebBrowserEventHandler->CommandStateChange(Command, Enable);
        }
    }

    void CWebBrowserUI::NavigateComplete2(IDispatch *pDisp, VARIANT *&url)
    {
        MUITRACE(_T("CWebBrowserUI::NavigateComplete2"));
        CComPtr<IDispatch> spDoc;
        m_pWebBrowser->get_Document(&spDoc);

        if (spDoc)
        {
            CComQIPtr<ICustomDoc, &IID_ICustomDoc> spCustomDoc(spDoc);
            if (spCustomDoc)
                spCustomDoc->SetUIHandler(this);
        }

        if (m_pWebBrowserEventHandler)
        {
            m_pWebBrowserEventHandler->NavigateComplete2(pDisp, url);
        }
    }

    void CWebBrowserUI::DocumentComplete2(IDispatch *pDisp, VARIANT *&url)
    {
        MUITRACE(_T("CWebBrowserUI::DocumentComplete2"));
        if (m_pWebBrowserEventHandler)
        {
            m_pWebBrowserEventHandler->DocumentComplete2(pDisp, url);
        }
    }

    void CWebBrowserUI::NavigateError(IDispatch *pDisp, VARIANT * &url, VARIANT *&TargetFrameName, 
        VARIANT *&StatusCode, VARIANT_BOOL *&Cancel)
    {
        MUITRACE(_T("CWebBrowserUI::NavigateError"));
        if (m_pWebBrowserEventHandler)
        {
            m_pWebBrowserEventHandler->NavigateError(pDisp, url, TargetFrameName, StatusCode, Cancel);
        }
    }

    void CWebBrowserUI::ProgressChange(LONG nProgress, LONG nProgressMax)
    {
        MUITRACE(_T("CWebBrowserUI::ProgressChange"));
        if (m_pWebBrowserEventHandler)
        {
            m_pWebBrowserEventHandler->ProgressChange(nProgress, nProgressMax);
        }
    }

    void CWebBrowserUI::NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
    {
        MUITRACE(_T("CWebBrowserUI::NewWindow3"));
        if (m_pWebBrowserEventHandler)
        {
            m_pWebBrowserEventHandler->NewWindow3(pDisp, Cancel, dwFlags, bstrUrlContext, bstrUrl);
        }
    }


}