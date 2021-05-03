#include <atlbase.h>
#include <atlwin.h>
#include "UIActiveX.h"
#include "..//Core//UIWindow.h"

namespace MYUI
{
/***************************************************************************************
 *
 *
 *
 *
 *
***************************************************************************************/
	class CActiveXEnum : public IEnumUnknown
	{
	public:
		CActiveXEnum(IUnknown * pUnk)
			: m_pUnk(pUnk)
			, m_dwRef(1)
			, m_iPos(0)
		{
			m_pUnk->AddRef();
		}

		~CActiveXEnum()
		{
			m_pUnk->Release();
		}

		STDMETHOD_(ULONG, AddRef)()
		{
			return ++m_dwRef;
		}

		STDMETHOD_(ULONG , Release)()
		{
			LONG lRef = --m_dwRef;
			if(lRef == 0) delete this;
			return lRef;
		}

		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
		{
			*ppvObject = NULL;
			if( riid == IID_IUnknown) *ppvObject = static_cast<IEnumUnknown *>(this);
			else if(riid == IID_IEnumUnknown) *ppvObject = static_cast<IEnumUnknown *>(this);
			if(*ppvObject != NULL) AddRef();
			return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
		}

		STDMETHOD(Next)(ULONG celt, IUnknown ** rgelt, ULONG *pceltFetched)
		{
			IID_ICallFactory; IID_IClassFactory;
			//celt ： 需要枚举多少个值给rgelt
			//rgelt ：转载枚举值的动态数组
			//pceltFetched ：rgelt返回是，携带了多少个值
			//这个函数很明显了，只是简单把m_pUnk复制给rgelt
			if(pceltFetched != NULL) *pceltFetched = 0;
			if(++m_iPos > 1) return S_FALSE;
			*rgelt = m_pUnk;
			(*rgelt)->AddRef();
			if(pceltFetched != NULL) *pceltFetched = 1;
			return S_OK;
		}

		STDMETHOD(Skip)(ULONG celt)
		{
			//枚举时，跳过celt个值
			m_iPos += celt;
			return S_OK;
		}

		STDMETHOD(Reset)(void)
		{
			//从0开始
			m_iPos = 0;
			return S_OK;
		}

		STDMETHOD(Clone)(IEnumUnknown **ppEnum)
		{
			return E_NOTIMPL;
		}

	protected:
		LONG m_iPos;
		ULONG m_dwRef;
		IUnknown * m_pUnk;

	};

/***************************************************************************************
 *
 *
 *
 *
 *
***************************************************************************************/

	class CActiveXInPlaceFrame : public IOleInPlaceFrame
	{
	public:
		CActiveXInPlaceFrame(HWND pHostWindow)
			: m_dwRef(1)
			, m_hHostWindow(pHostWindow)
			, m_pActiveObject(NULL)
		{
		}

		~CActiveXInPlaceFrame()
		{
			if(NULL != m_pActiveObject) m_pActiveObject->Release();
		}

		//IUnknown
		STDMETHOD_(ULONG, AddRef)()
		{
			return ++m_dwRef;
		}

		STDMETHOD_(ULONG, Release)()
		{
			ULONG lRef = --m_dwRef;
			if(0 == lRef) delete this;
			return lRef;
		}

		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
		{
			*ppvObject = NULL;
			if(IID_IUnknown == riid) *ppvObject =static_cast<IOleInPlaceFrame*>(this);
			else if(IID_IOleWindow == riid) *ppvObject = static_cast<IOleWindow*>(this);
			else if(IID_IOleInPlaceFrame == riid) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
			else if(IID_IOleInPlaceUIWindow == riid) *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);
			if(NULL != *ppvObject) AddRef();
			return *ppvObject == NULL ? E_NOINTERFACE : S_OK;

			IOleWindow * p = NULL;
		}

		//IOleInPlaceFrameWindow
		STDMETHOD(InsertMenus)(HMENU /*hMenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
		{
			return S_OK;
		}
		STDMETHOD(SetMenu)(HMENU /*hMenuShared*/, HOLEMENU /*hOleMenu*/, HWND /*hWndActiveObject*/)
		{
			return S_OK;
		}
		STDMETHOD(RemoveMenus)(HMENU /*hMenuShared*/)
		{
			return S_OK;
		}
		STDMETHOD(SetStatusText)(LPCOLESTR /*strStatusText*/)
		{
			return S_OK;
		}
		STDMETHOD(EnableModeless)(BOOL /*bEnable*/)
		{
			return S_OK;
		}
		STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
		{
			return S_FALSE;
		}
		//IOleWindow
		STDMETHOD(GetWindow)(HWND *phWnd)
		{
			if(NULL == m_hHostWindow) return E_UNEXPECTED;
			*phWnd = m_hHostWindow;
			return S_OK;
		}
		STDMETHOD(ContextSensitiveHelp)(BOOL /*bEnterMode*/)
		{
			return S_OK;
		}
		//IOleInPlaceUIWindow
		STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
		{
			return S_OK;
		}
		STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pBorderWidths*/)
		{
			return INPLACE_E_NOTOOLSPACE;
		}
		STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pBorderWidths*/)
		{
			return S_OK;
		}
		STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR /*strObjectName*/)
		{
			if(pActiveObject != NULL) pActiveObject->AddRef();
			if(m_pActiveObject != NULL) m_pActiveObject->Release();
			m_pActiveObject = pActiveObject;

			return S_OK;
		}

	protected:
		ULONG m_dwRef;
		HWND m_hHostWindow;
		IOleInPlaceActiveObject * m_pActiveObject;
	};

/***************************************************************************************
 *
 *
 *
 *
 *
***************************************************************************************/
	

	CActiveXCtrl::CActiveXCtrl()
		: m_hHostWindow(NULL)
		, m_pOleObject(NULL)
		, m_dwRef(1)
		, m_pUnkSite(NULL)
		, m_pViewObject(NULL)
		, m_pInPlaceObject(NULL)
		, m_bLocked(false)
		, m_bFocused(false)
		, m_bCaptured(false)
		, m_bWindowless(true)
		, m_bUIActivated(false)
	{
		EmptyRect(m_rcPos);
		//ASSERT(m_hHostWindow);
	}

	CActiveXCtrl::~CActiveXCtrl()
	{
		MUITRACE(_T("AX: CActiveXCtrl::构析"));
		//if(m_pUnkSite != NULL) m_pUnkSite->Release();
		//if(m_pViewObject != NULL) m_pViewObject->Release();
		//if(m_pInPlaceObject != NULL) m_pInPlaceObject->Release();
	}

	STDMETHODIMP CActiveXCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
	{
		//TRACE(_T("AX: CActiveXCtrl::QueryInterface"));
		*ppvObject = NULL;
		if(riid == IID_IUnknown) *ppvObject = this;
		else if(riid == IID_IOleClientSite) *ppvObject = static_cast<IOleClientSite *>(this);
		else if(riid == IID_IOleInPlaceSiteWindowless) *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
		else if(riid == IID_IOleInPlaceSiteEx) *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
		else if(riid == IID_IOleInPlaceSite) *ppvObject = static_cast<IOleInPlaceSite*>(this);
		else if(riid == IID_IOleWindow) *ppvObject = static_cast<IOleWindow*>(this);
		else if(riid == IID_IOleControlSite) *ppvObject = static_cast<IOleControlSite*>(this);
		else if(riid == IID_IOleContainer) *ppvObject = static_cast<IOleContainer*>(this);
		else if(riid == IID_IObjectWithSite) *ppvObject = static_cast<IObjectWithSite*>(this);
		if(*ppvObject != NULL) AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}

	STDMETHODIMP_(ULONG) CActiveXCtrl::AddRef()
	{
		MUITRACE(_T("AX: CActiveXCtrl::AddRef %ld"), m_dwRef);
		InterlockedIncrement(&m_dwRef);
		return m_dwRef;
	}

	STDMETHODIMP_(ULONG) CActiveXCtrl::Release()
	{
		MUITRACE(_T("AX: CActiveXCtrl::Release %ld"), m_dwRef);
		ULONG ulRefCount = InterlockedDecrement(&m_dwRef);
		if(ulRefCount == 0) delete this;
		return ulRefCount;
	}

	STDMETHODIMP CActiveXCtrl::SetSite(IUnknown *pUnkSite)
	{
		MUITRACE(_T("AX: CActiveXCtrl::SetSite"));
		if(NULL != m_pUnkSite)
		{
			m_pUnkSite->Release();
			m_pUnkSite = NULL;
		}
		if(NULL != pUnkSite)
		{
			m_pUnkSite = pUnkSite;
			m_pUnkSite->AddRef();
		}
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::GetSite(REFIID  riid, LPVOID * ppvSite)
	{
		MUITRACE(_T("AX: CActiveXCtrl::GetSite"));
		if(NULL == ppvSite) return E_POINTER;
		*ppvSite = NULL;
		if(NULL == m_pUnkSite ) return E_FAIL;
		return m_pUnkSite->QueryInterface(riid, ppvSite);
	}

	STDMETHODIMP CActiveXCtrl::SaveObject(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::SaveObject"));
		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk)
	{
		MUITRACE(_T("AX: CActiveXCtrl::GetMoniker"));
		if(ppmk) *ppmk = NULL;
		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::GetContainer(IOleContainer ** ppContainer)
	{
		MUITRACE(_T("AX: CActiveXCtrl::GetContainer"));
		*ppContainer = NULL;
		HRESULT hr = E_NOTIMPL;
		if(NULL != m_pUnkSite) hr = m_pUnkSite->QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
		if(FAILED(hr)) hr = QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
		return hr;
	}

	STDMETHODIMP CActiveXCtrl::ShowObject(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::ShowObject"));
		if(NULL == m_hHostWindow) return E_UNEXPECTED;
		/*HDC hdc = ::GetDC(m_hHostWindow);
		if(NULL == hdc) return E_FAIL;
		if(NULL != m_pViewObject) m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL,NULL,
			hdc, (RECTL*) &m_rcPos, (RECTL*) &m_rcPos, NULL, NULL);
		::ReleaseDC(m_hHostWindow, hdc);*/

		InvalidateRect(&m_rcPos, FALSE);
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::OnShowWindow(BOOL bShow)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnShowWindow"));
		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::RequestNewObjectLayout(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::RequestNewObjectLayout"));
		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::CanWindowlessActivate(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::CanWindowlessActivate"));
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::GetCapture(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::GetCapture"));
        if(NULL == m_hHostWindow) return E_UNEXPECTED;
		return m_bCaptured ? S_OK : S_FALSE;
	}

	STDMETHODIMP CActiveXCtrl::SetCapture(BOOL bCapture)
	{
		MUITRACE(_T("AX: CActiveXCtrl::SetCapture"));
		if(NULL == m_hHostWindow) return E_UNEXPECTED;
		m_bCaptured = (TRUE == bCapture);
		if(m_bCaptured) ::SetCapture(m_hHostWindow); else ::ReleaseCapture();
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::GetFocus(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::GetFocus"));
		if(NULL == m_hHostWindow) return E_UNEXPECTED;
		return m_bFocused ? S_OK : S_FALSE;
	}

	STDMETHODIMP CActiveXCtrl::SetFocus(BOOL bFocus)
	{
		MUITRACE(_T("AX: CActiveXCtrl::SetFocus"));
		if(NULL == m_hHostWindow) return E_UNEXPECTED;
		m_bFocused = (bFocus == TRUE);
        if(m_bFocused) ::SetFocus(m_hHostWindow);
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::GetDC(LPCRECT pRect, DWORD grfFlags, HDC * phdc)
	{
		MUITRACE(_T("AX: CActiveXCtrl::GetDC"));
		if(NULL == phdc) return E_POINTER;
		if(NULL == m_hHostWindow) return E_UNEXPECTED;
		*phdc = ::GetDC(m_hHostWindow);
		if((grfFlags & OLEDC_PAINTBKGND)!= 0 )
		{
			//CMuiRect rcItem = m_pOwner->GetPos();
			//if(!m_bWindowless) OffsetRect(&m_rcItem, -m_rcItem.left, -m_rcItem.top);
			//::FillRect(*phdc, &m_rcPos, (HBRUSH)(COLOR_WINDOW + 1));
		}
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::ReleaseDC(HDC hdc)
	{
		MUITRACE(_T("AX: CActiveXCtrl::ReleaseDC"));
		if(NULL == m_hHostWindow) return E_UNEXPECTED;
		::ReleaseDC(m_hHostWindow, hdc);
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::InvalidateRect(LPCRECT pRect, BOOL bErase)
	{
		MUITRACE(_T("AX: CActiveXCtrl::InvalidateRect"));
		if(NULL == m_hHostWindow) return E_UNEXPECTED;
		return ::InvalidateRect(m_hHostWindow, pRect, bErase) ? S_OK : E_FAIL;
	}

	STDMETHODIMP CActiveXCtrl::InvalidateRgn(HRGN hRgn, BOOL bErase)
	{
		MUITRACE(_T("AX: CActiveXCtrl::InvalidateRgn"));
		if(NULL == m_hHostWindow) return E_UNEXPECTED;
		return ::InvalidateRgn(m_hHostWindow, hRgn, bErase) ? S_OK : E_FAIL;
	}

	STDMETHODIMP CActiveXCtrl::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
	{
		MUITRACE(_T("AX: CActiveXCtrl::ScrollRect"));
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::AdjustRect(LPRECT prc)
	{
		MUITRACE(_T("AX: CActiveXCtrl::AdjustRect"));
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::OnDefWindowMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * plResult)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnDefWindowMessage"));
		if(NULL == m_hHostWindow) return E_UNEXPECTED;
		*plResult = ::DefWindowProc(m_hHostWindow, message, wParam, lParam);
		return S_OK;
	}
		
	//当控件激活时，是否需要刷新
	STDMETHODIMP CActiveXCtrl::OnInPlaceActivateEx(BOOL * pbNoRedraw, DWORD dwFlags)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnInPlaceActivateEx"));
		MUIASSERT(NULL == m_pInPlaceObject);
		if(NULL == m_pOleObject) return E_UNEXPECTED;
		::OleLockRunning(m_pOleObject, TRUE, FALSE);
		//HWND hWndFrame = m_pOwner->GetManager()->GetPaintWindow();

		HRESULT hr = E_FAIL;
		if((dwFlags & ACTIVATE_WINDOWLESS) != 0 )
		{
			m_bWindowless = true;
			hr = m_pOleObject->QueryInterface(IID_IOleInPlaceObjectWindowless,(LPVOID *)m_pInPlaceObject);

		}

		if(FAILED(hr))
		{
			m_bWindowless = false;

			hr = m_pOleObject->QueryInterface(IID_IOleInPlaceObject, (LPVOID*) &m_pInPlaceObject);
		}

		if(m_pInPlaceObject != NULL)
		{
			m_pInPlaceObject->SetObjectRects(&m_rcPos, &m_rcPos);
		}
		return hr;
	}

	//通知容器对象是否需要重新绘制在停用。
	STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivateEx(BOOL bNoRedraw)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnInPlaceDeactivateEx"));
		if(m_pInPlaceObject != NULL)
		{
			m_pInPlaceObject->Release();
			m_pInPlaceObject = NULL;
		}
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::RequestUIActivate(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::RequestUIActivate"));
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::CanInPlaceActivate(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::CanInPlaceActivate"));
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::OnInPlaceActivate(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnInPlaceActivate"));
		BOOL bDummy = FALSE;
		return OnInPlaceActivateEx(&bDummy, 0);
	}

	STDMETHODIMP CActiveXCtrl::OnUIActivate(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnUIActivate"));
		m_bUIActivated = true;
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow ** ppDoc,
		LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{

		MUITRACE(_T("AX: CActiveXCtrl::GetWindowContext"));
		if(NULL == ppDoc) return E_POINTER;
		if(NULL == ppFrame) return E_POINTER;
		if(NULL == lprcPosRect) return E_POINTER;
		if(NULL == lprcClipRect) return E_POINTER;

		ACCEL ac = {0};
		HACCEL hac = ::CreateAcceleratorTable(&ac, 1);

		*ppDoc =  NULL;
		*lprcPosRect = *lprcClipRect = m_rcPos;
		
		*ppFrame = new CActiveXInPlaceFrame(m_hHostWindow);
		lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
		lpFrameInfo->fMDIApp = FALSE;
		lpFrameInfo->hwndFrame = m_hHostWindow;
		lpFrameInfo->haccel = hac;
		lpFrameInfo->cAccelEntries = 1;
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::Scroll(SIZE scrollExtant)
	{
		MUITRACE(_T("AX: CActiveXCtrl::Scroll"));
		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::OnUIDeactivate(BOOL bUndoable)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnUIDeactivate"));
		m_bUIActivated = false;
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivate(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnInPlaceDeactivate"));
		return OnInPlaceDeactivateEx(TRUE);
	}

	STDMETHODIMP CActiveXCtrl::DeactivateAndUndo(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::DeactivateAndUndo"));
		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::DiscardUndoState(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::DiscardUndoState"));
		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::OnPosRectChange(LPCRECT lprcPosRect)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnPosRectChange"));
        SIZEL hmSize = {0};
		SIZEL pxSize = {0};

		m_rcPos = *lprcPosRect;

        //if(true == m_bWindowless) return E_NOTIMPL;

        if(m_pOleObject)
        {
            pxSize.cx = m_rcPos.right - m_rcPos.left;
		    pxSize.cy = m_rcPos.bottom - m_rcPos.top;
		    ::PixelToHiMetric(m_hHostWindow, &pxSize, &hmSize);

		    m_pOleObject->SetExtent(DVASPECT_CONTENT, &hmSize);
        }
        
		if(m_pInPlaceObject)
		{
			m_pInPlaceObject->SetObjectRects(&m_rcPos, &m_rcPos);
		}

		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::GetWindow(HWND *phWnd)
	{
		MUITRACE(_T("AX: CActiveXCtrl::GetWindow"));
		if(NULL == m_hHostWindow) return E_FAIL;
		*phWnd = m_hHostWindow;
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::ContextSensitiveHelp(BOOL bEnterMode)
	{
		MUITRACE(_T("AX: CActiveXCtrl::ContextSensitiveHelp"));
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::OnControlInfoChanged(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnControlInfoChanged"));
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::LockInPlaceActive(BOOL bLock)
	{
		MUITRACE(_T("AX: CActiveXCtrl::LockInPlaceActive"));
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::GetExtendedControl(IDispatch** ppDisp)
	{
		MUITRACE(_T("AX: CActiveXCtrl::GetExtendedControl"));
		if(NULL == ppDisp) return E_POINTER;
		if(NULL == m_pOleObject) return E_UNEXPECTED;
		return m_pOleObject->QueryInterface(IID_IDispatch, (LPVOID*) ppDisp);
	}

	STDMETHODIMP CActiveXCtrl::TransformCoords(POINTL * pPtlHimetric, POINTF *pPtfContainer, DWORD dwFlags)
	{
		MUITRACE(_T("AX: CActiveXCtrl::TransformCoords"));
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
	{
		MUITRACE(_T("AX: CActiveXCtrl::TranslateAccelerator"));
		return S_FALSE;
	}

	STDMETHODIMP CActiveXCtrl::OnFocus(BOOL bGotFocus)
	{
		MUITRACE(_T("AX: CActiveXCtrl::OnFocus"));
		m_bFocused = (bGotFocus == TRUE);
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::ShowPropertyFrame(void)
	{
		MUITRACE(_T("AX: CActiveXCtrl::ShowPropertyFrame"));
		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::EnumObjects(DWORD grfFlags, IEnumUnknown ** ppEnum)
	{
		MUITRACE(_T("AX: CActiveXCtrl::EnumObjects"));
		if(NULL == ppEnum) return E_POINTER;
		if(NULL == m_pOleObject) return E_UNEXPECTED;
		*ppEnum = new CActiveXEnum(m_pOleObject);
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::LockContainer(BOOL bLock)
	{
		MUITRACE(_T("AX: CActiveXCtrl::ParseDisplayName"));
		m_bLocked = (FALSE != bLock);
		return S_OK;
	}

	STDMETHODIMP CActiveXCtrl::ParseDisplayName(IBindCtx *pbc, LPOLESTR strDisplayName, ULONG * pchEaten, IMoniker **ppmkOut)
	{
		MUITRACE(_T("AX: CActiveXCtrl::ParseDisplayName"));
		return E_NOTIMPL;
	}

	STDMETHODIMP CActiveXCtrl::SetWindow(HWND hWnd)
	{
		MUITRACE(_T("AX: CActiveXCtrl::SetWindow"));
		m_hHostWindow = hWnd;
		return S_OK;
	}
	
/***************************************************************************************
 *
 *
 *
 *
 *
***************************************************************************************/
	CActiveXUI::CActiveXUI()
	{
		m_clsid = IID_NULL;
	}

	CActiveXUI::~CActiveXUI()
	{
		MUITRACE(_T("AX: CActiveXUI::构析"));
		ReleaseControl();
	}

	CMuiString CActiveXUI::g_strClassName(_T("ActiveXUI"));

	CMuiString CActiveXUI::GetClassName() const
	{
		return CActiveXUI::g_strClassName;
	}

	void CActiveXUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0 == _tcsicmp(strItem, _T("clsid")))
		{
			CreateControl(strValue);
		}
		else if(0 == _tcsicmp(strItem, _T("ModuleName")))
		{
			SetModuleName(strValue);
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
		return;
	}

	void CActiveXUI::OnAttach(HWND hNewWnd)
	{
		MUITRACE(_T("AX: CActiveXUI::OnAttach"));

		IOleObject * pOleObject = NULL;
		DWORD dwMiscStatus = 0;
		CActiveXCtrl::SetWindow(hNewWnd);
		this->GetControl(IID_IOleObject, (LPVOID*) &pOleObject);
		if(pOleObject)
		{
			CSafeRelease<IOleObject> RefOleObject = pOleObject;
			
			pOleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
			if((dwMiscStatus & OLEMISC_NOUIACTIVATE)) return;
			if(NULL == m_pInPlaceObject) DoVerb(OLEIVERB_INPLACEACTIVATE);
		}
	}

	void CActiveXUI::OnDetach(HWND hOldWnd)
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

		if(m_pInPlaceObject) m_pInPlaceObject->Release();
		if(m_pUnkSite) m_pUnkSite->Release();
		if(m_pViewObject) m_pViewObject->Release();

		m_pInPlaceObject = NULL;
		m_pUnkSite = NULL;
		m_pViewObject = NULL;
		m_hHostWindow = NULL;
		
		CActiveXCtrl::SetWindow(NULL);
		DoVerb(OLEIVERB_HIDE);
		m_bWindowless = true;
	}

	bool CActiveXUI::CreateControl(LPCTSTR strCLSID)
	{
		CLSID clsid = {0};
		OLECHAR szCLSID[100] = {0};
#ifndef _UNICODE
		::MultiByteToWideChar(::GetACP(), 0, (LPCCH)strCLSID, -1, szCLSID, lengthof(szCLSID) -1);
#else
		::_tcsncpy(szCLSID, strCLSID, lengthof(szCLSID) - 1);
#endif
		if(_T('{') == strCLSID[0]) ::CLSIDFromString(szCLSID, &clsid);
		else ::CLSIDFromProgID(szCLSID, &clsid);

		return CreateControl(clsid);
	}

typedef HRESULT (__stdcall *DllGetClassObjectFunc)(REFCLSID rclsid, REFIID riid, LPVOID* ppv); 

	bool CActiveXUI::CreateControl(const CLSID clsid /*= IID_NULL*/)
	{
		if(IID_NULL == clsid && IID_NULL == m_clsid)
		{
			MUIASSERT(0 && "没有为CreateControl::m_clsid设置IID！！");
			return false;
		}
		if(IID_NULL != clsid && clsid != m_clsid)
		{
			MUIASSERT(0 && "CreateControl::m_clsid已经初始化，不能再重新赋值！！");
			return false;
		}
		if(IID_NULL != clsid)
		{
			m_clsid = clsid;
		}

		IOleControl * pOleControl = NULL;

		HRESULT hr = -1;

		if(!m_strModuleName.IsEmpty())
		{
			HMODULE hModule = ::LoadLibrary((LPCTSTR) m_strModuleName);
			if(hModule)
			{
				IClassFactory * pClassFactory = NULL;
				//从对应的模块加载DllGetClassObject函数和IID_IClassFactory指针
				DllGetClassObjectFunc pfnDllGetClassObject = (DllGetClassObjectFunc)::GetProcAddress(hModule, "DllGetClassObject");
				hr = pfnDllGetClassObject(m_clsid, IID_IClassFactory, (LPVOID*)&pClassFactory);
				if(SUCCEEDED(hr))
				{
					hr = pClassFactory->CreateInstance(NULL, IID_IOleObject, (LPVOID *)&pOleControl);
				}
				pClassFactory->Release();
			}
		}

		if(FAILED(hr))
		{
			hr = ::CoCreateInstance(m_clsid, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*)&pOleControl);
		}

		MUIASSERT(SUCCEEDED(hr));
		if(FAILED(hr)) return false;
		pOleControl->QueryInterface(IID_IOleObject,(LPVOID*) &m_pOleObject);
		pOleControl->Release();
		if(NULL == m_pOleObject) return false;
		DWORD dwMiscStatus = 0;
		m_pOleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
		IOleClientSite * pOleClientSite = NULL;
		CActiveXCtrl::QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
		CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;

		//init
		if((OLEMISC_SETCLIENTSITEFIRST & dwMiscStatus))
		{
			m_pOleObject->SetClientSite(pOleClientSite);
		}

		IPersistStreamInit * pPersistStreamInit = NULL;
		m_pOleObject->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersistStreamInit);
		if(pPersistStreamInit)
		{
			hr = pPersistStreamInit->InitNew();
			pPersistStreamInit->Release();
		}

		if(FAILED(hr)) return false;

		if((OLEMISC_SETCLIENTSITEFIRST & dwMiscStatus))
		{
			m_pOleObject->SetClientSite(pOleClientSite);
		}

		hr = m_pOleObject->QueryInterface(IID_IViewObjectEx, (LPVOID*)&m_pViewObject);
		if(FAILED(hr)) hr = m_pOleObject->QueryInterface(IID_IViewObject2, (LPVOID*) &m_pViewObject);
		if(FAILED(hr)) hr = m_pOleObject->QueryInterface(IID_IViewObject, (LPVOID*) &m_pViewObject);

		m_pOleObject->SetHostNames(OLESTR("UIActiveX"), NULL);
		
		if(OLEMISC_INVISIBLEATRUNTIME & dwMiscStatus)
		{
			hr = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pOleClientSite, 0,
				CActiveXCtrl::m_hHostWindow, &m_rcPos);
		}

		IObjectWithSite * pSite = NULL;
		m_pOleObject->QueryInterface(IID_IObjectWithSite, (LPVOID*)&pSite);
		if(pSite)
		{
			pSite->SetSite(static_cast<IOleClientSite*>(this));
			pSite->Release();
		}

		/*IOleObject * pOleObject = NULL;
		this->GetControl(IID_IOleObject, (LPVOID*) &pOleObject);

		while(pOleObject)
		{
			CSafeRelease<IOleObject> RefOleObject = pOleObject;
			DWORD dwMiscStatus = 0;
			pOleObject->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
			if((dwMiscStatus & OLEMISC_NOUIACTIVATE)) break;
			if(!m_bInPlaceActive) DoVerb(OLEIVERB_INPLACEACTIVATE);
			break;
		}*/
		
		return SUCCEEDED(hr);
	}

	void CActiveXUI::ReleaseControl()
	{
		IObjectWithSite * pSite = NULL;
		if(m_pOleObject)
		{
			m_pOleObject->QueryInterface(IID_IObjectWithSite, (LPVOID *) &pSite);
			if(pSite != NULL)
			{
				pSite->SetSite(NULL);
				pSite->Release();
			}
			m_pOleObject->Close(OLECLOSE_NOSAVE);
			m_pOleObject->SetClientSite(NULL);
			m_pOleObject->Release();
			m_pOleObject = NULL;
		}
	}

	HRESULT CActiveXUI::GetControl(const IID iid, LPVOID *ppRet)
	{
		MUIASSERT(ppRet != NULL);
		MUIASSERT(*ppRet == NULL);

		if(NULL == ppRet) return E_POINTER;
		if(NULL == m_pOleObject) return E_PENDING;

		return m_pOleObject->QueryInterface(iid, (LPVOID*) ppRet);
	}

	CLSID CActiveXUI::GetClisd() const
	{
		return m_clsid;
	}

	CMuiString CActiveXUI::GetModuleName() const
	{
		return m_strModuleName;
	}

	void CActiveXUI::SetModuleName(LPCTSTR strText)
	{
		m_strModuleName = strText;
	}

	void CActiveXUI::DoVerb(LONG nVerb)
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
		IOleObject * pOleObject = NULL;
		this->GetControl(IID_IOleObject, (LPVOID*) &pOleObject);
		if(NULL == pOleObject) return ;
		CSafeRelease<IOleObject> RefOleObject = pOleObject;
		
		IOleClientSite * pOleClientSite = NULL;

		CActiveXCtrl::QueryInterface(IID_IOleClientSite, (LPVOID *) &pOleClientSite);
		CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
		pOleObject->DoVerb(nVerb, NULL, pOleClientSite, 0, CActiveXCtrl::m_hHostWindow, &m_rcPos);

	}

	void CActiveXUI::SetVisible(bool bVisible)
	{
		if(bVisible == m_bVisible) return;
		__super::SetVisible(bVisible);

		if(true == m_bVisible)
		{
			DoVerb(OLEIVERB_SHOW);
		}
		else
		{
			DoVerb(OLEIVERB_HIDE);
		}
	}

	bool CActiveXUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		SIZEL hmSize = {0};
		SIZEL pxSize = {0};

		if(false == __super::SetItem(rcItem, bMustUpdate)) return false;

		if(FALSE == IsValidRect(m_rcRawItem) && true == m_bWindowless)
		{
			return true;
		}

		GetItemFixedRect(rcItem);

		if(TRUE == IsSameRect(rcItem, CActiveXCtrl::m_rcPos)) return true;
		CActiveXCtrl::OnPosRectChange(&rcItem);

        //代码移动到OnPosRectChange中
		//if(!m_pOleObject || !m_pShareInfo || !m_pShareInfo->hWnd) return true;

		//pxSize.cx = m_rcItem.right - m_rcItem.left;
		//pxSize.cy = m_rcItem.bottom - m_rcItem.top;
		//::PixelToHiMetric(m_pShareInfo->hWnd, &pxSize, &hmSize);

		//m_pOleObject->SetExtent(DVASPECT_CONTENT, &hmSize);

		//if(m_pInPlaceObject)
		//{
		//	m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
		//}
		return true;
	}

	bool CActiveXUI::OnPaint(RECT rcItem,RECT rcPaint, RECT rcUpdate)
	{
		HDC hdc = NULL;

		if(false == m_bVisible || TRUE == IsEmptyRect(rcPaint)) return false;
		PaintBkColor(rcPaint);
		PaintBkImage(rcPaint);

		if(m_bWindowless && m_pViewObject)
		{
			//TRACE(_T("left=%d, top=%d, right=%d, bottom=%d"), rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
			return m_pShareInfo->pRenderEngine->OnDrawOleObject(rcItem, m_pViewObject);
		}

		/*hdc = ::GetDC(CActiveXCtrl::m_hHostWindow);
		FillRect(hdc, &rcItem, (HBRUSH) (COLOR_WINDOW + 1));
		m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc,
			(RECTL*)&rcItem, (RECTL*)&rcItem, NULL,NULL);
		::ReleaseDC(CActiveXCtrl::m_hHostWindow, hdc);*/
		return true;
	}

	LRESULT CActiveXUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{

		POINT point;
		LRESULT lResult = 0;
		SIZEL hmSize = {0};
		SIZEL pxSize = {0};
		RECT rcItem = {0};
		//if(NULL == m_bInPlaceActive) return false;
		//不是无窗模式，它有自己的实体窗口和消息流，如果不返回的话，将会进行二次处理
		MUITRACE(_T("AX: CActiveXUI::WndProc"));

		if(true == m_bWindowless) return 0;

		if(message >= WM_MOUSEFIRST && WM_MOUSELAST >= message)
		{
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
			DWORD dwHitResult = m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;

			if(HITRESULT_OUTSIDE == dwHitResult && NULL != m_pViewObject)
			{
				IViewObjectEx * pViewEx = NULL;
				m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*) &pViewEx);
				if(pViewEx)
				{
					pViewEx->QueryHitPoint(DVASPECT_CONTENT, &m_rcPos, point, 0, &dwHitResult);
					pViewEx->Release();
				}
			}
		}

		if(NULL == m_pInPlaceObject) return lResult;
		
		HRESULT hr = m_pInPlaceObject->OnWindowMessage(message, wParam, lParam, &lResult);
		return lResult;
	}
}