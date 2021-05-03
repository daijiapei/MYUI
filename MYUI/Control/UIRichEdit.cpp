
#include "UIRichEdit.h"


EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

namespace MYUI
{
	const LONG g_cInitTextMax = (32 * 1024) - 1;

	CRichEditUI::CRichEditUI()
		: m_dwRef(1)
		, m_hDll(NULL)
		, m_pServices(NULL)
		, m_lSelBarWidth(0)
		, m_cchTextMost(g_cInitTextMax)
		, m_dwEventMask(NULL)
		, m_icf(0)
		, m_ipf(0)
		, m_laccelpos(-1)
		, m_chPasswordChar(_T('*'))
	{
		//m_hBorderPen = CreatePen(PS_SOLID,1 ,RGB(0,0,0));
		EmptyRect(m_rcTxClient);
		m_sizelExtent.cx = m_sizelExtent.cy = 0;
		m_ptMouse.x = m_ptMouse.y = 0;
		memset(&m_CharFormat, 0, sizeof(CHARFORMAT2W));
		memset(&m_ParaFormat, 0, sizeof(PARAFORMAT2));
	}

	CRichEditUI::~CRichEditUI()
	{
		m_pServices->OnTxInPlaceDeactivate();
		m_pServices->Release();
		if(m_hDll) ::FreeLibrary(m_hDll);
	}

	const CMuiString CRichEditUI::g_strClassName(_T("RichEditUI"));

	CMuiString CRichEditUI::GetClassName() const
	{
		return CRichEditUI::g_strClassName;
	}

/********************************************************************************
 * CRichEditUI 富文本控件
 * ITextHost 接口实现
 *
 *
********************************************************************************/

////////////////////// Create/Init/Destruct Commands ///////////////////////

	HRESULT CRichEditUI::InitDefaultCharFormat(CHARFORMAT2W* pcf, HFONT hFont) 
	{
		MUITRACE(_T("CRichEditUI::InitDefaultCharFormat"));
		LOGFONT lf;
		memset(pcf, 0, sizeof(CHARFORMAT2W));
		
		if(NULL == hFont)
		{
			hFont = (HFONT)m_pShareInfo->FontArray.Select(m_nFontId);
		}

		if(NULL == hFont)
		{
			hFont = (HFONT)::SendMessage(m_pShareInfo->hWnd, WM_GETFONT, 0, 0);
		}

		if(NULL == hFont && m_pShareInfo->hPaintDC)
		{
			hFont = (HFONT)GetCurrentObject(m_pShareInfo->hPaintDC, OBJ_FONT);
		}

		if(NULL == hFont) return E_FAIL;

		::GetObject(hFont, sizeof(LOGFONT), &lf);

		DWORD dwColor = ARGB(0, GetRValue(m_refTextColor), GetGValue(m_refTextColor), GetBValue(m_refTextColor));
		pcf->cbSize = sizeof(CHARFORMAT2W);
		pcf->crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
		LONG yPixPerInch = GetDeviceCaps(m_pShareInfo->hPaintDC, LOGPIXELSY);
		pcf->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
		pcf->yOffset = 0;
		pcf->dwEffects = 0;
		pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
		if(lf.lfWeight >= FW_BOLD)
			pcf->dwEffects |= CFE_BOLD;
		if(lf.lfItalic)
			pcf->dwEffects |= CFE_ITALIC;
		if(lf.lfUnderline)
			pcf->dwEffects |= CFE_UNDERLINE;
		pcf->bCharSet = lf.lfCharSet;
		pcf->bPitchAndFamily = lf.lfPitchAndFamily;
	#ifdef _UNICODE
		_tcscpy(pcf->szFaceName, lf.lfFaceName);
	#else
		//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
		MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE) ;
	#endif

		return S_OK;
	}

	BOOL CRichEditUI::Init()
	{
		IUnknown *pUnk;
		HRESULT hr;
		PCreateTextServices TextServicesProc;

		// Create and cache CHARFORMAT for this control
		if(FAILED(InitDefaultCharFormat(&m_CharFormat, NULL)))
		{
			goto err;
		}

		// Create and cache PARAFORMAT for this control
		memset(&m_ParaFormat, 0, sizeof(PARAFORMAT2));
		m_ParaFormat.cbSize = sizeof(PARAFORMAT2);
		m_ParaFormat.dwMask = PFM_ALL;
		m_ParaFormat.wAlignment = PFA_LEFT;
		m_ParaFormat.cTabCount = 1;
		m_ParaFormat.rgxTabs[0] = lDefaultTab;


		// edit controls created without a window are multiline by default
		// so that paragraph formats can be
		m_dwStyle = ES_MULTILINE;

		if ( !(m_dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL)) )
		{
			m_fWordWrap = TRUE;
		}

		if( !(m_dwStyle & ES_LEFT) )
		{
			if(m_dwStyle & ES_CENTER)
				m_ParaFormat.wAlignment = PFA_CENTER;
			else if(m_dwStyle & ES_RIGHT)
				m_ParaFormat.wAlignment = PFA_RIGHT;
		}

		m_fInplaceActive = TRUE;

		// Create Text Services component
		//if(FAILED(CreateTextServices(NULL, this, &pUnk)))
		//    goto err;

		if(NULL == m_hDll) m_hDll = ::LoadLibrary(_T("msftedit.dll"));

		if (m_hDll && (TextServicesProc = (PCreateTextServices)GetProcAddress(m_hDll,"CreateTextServices")))
		{
			hr = TextServicesProc(NULL, dynamic_cast<ITextHost*>(this), &pUnk);
		}

		hr = pUnk->QueryInterface(IID_ITextServices,(void **)&m_pServices);

		// Whether the previous call succeeded or failed we are done
		// with the private interface.
		pUnk->Release();

		if(FAILED(hr))
		{
			goto err;
		}

		// Set window text
		if(!m_strName.IsEmpty())
		{
	#ifdef _UNICODE		
			if(FAILED(m_pServices->TxSetText(m_strName)))
				goto err;
	#else
			size_t iLen = m_strName.GetLength();
			LPWSTR lpText = new WCHAR[iLen + 1];
			::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
			::MultiByteToWideChar(CP_ACP, 0, pcs->lpszName, -1, (LPWSTR)lpText, iLen) ;
			if(FAILED(m_pServices->TxSetText((LPWSTR)lpText))) {
				delete[] lpText;
				goto err;
			}
			delete[] lpText;
	#endif
		}

		return TRUE;

err:

		if(m_hDll) ::FreeLibrary(m_hDll);
		m_hDll = NULL;
		return FALSE;
	}

/////////////////////////////////  IUnknown ////////////////////////////////

	HRESULT CRichEditUI::QueryInterface(REFIID riid, void **ppvObject)
	{
		MUITRACE(_T("CRichEditUI::QueryInterface"));
		HRESULT hr = E_NOINTERFACE;
		*ppvObject = NULL;

		if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextHost)) 
		{
			AddRef();
			*ppvObject = dynamic_cast<ITextHost *>(this);
			hr = S_OK;
		}

		return hr;
	}

	ULONG CRichEditUI::AddRef(void)
	{
		MUITRACE(_T("CRichEditUI::AddRef"));
		InterlockedIncrement(&m_dwRef);
		return m_dwRef;
	}

	ULONG CRichEditUI::Release(void)
	{
		MUITRACE(_T("CRichEditUI::Release"));

		ULONG ulRefCount = InterlockedDecrement(&m_dwRef);
		//if(ulRefCount == 0) delete this;
		return ulRefCount;
	}

	BOOL CRichEditUI::GetWordWrap(void)
	{
		MUITRACE(_T("CRichEditUI::GetWordWrap"));
		return m_fWordWrap;
	}

	BOOL CRichEditUI::GetActiveState(void)
	{
		MUITRACE(_T("CRichEditUI::GetActiveState"));
		return m_fInplaceActive;
	}

/////////////////////////////////  Far East Support  //////////////////////////////////////

	HIMC CRichEditUI::TxImmGetContext(void)
	{
		MUITRACE(_T("CRichEditUI::TxImmGetContext"));
		return NULL;
	}

	void CRichEditUI::TxImmReleaseContext(HIMC himc)
	{
		MUITRACE(_T("CRichEditUI::TxImmReleaseContext"));
		//::ImmReleaseContext( hwnd, himc );
	}

//////////////////////////// ITextHost Interface  ////////////////////////////

	HDC CRichEditUI::TxGetDC()
	{
		MUITRACE(_T("CRichEditUI::TxGetDC"));
		return m_pShareInfo->hPaintDC;
	}

	int CRichEditUI::TxReleaseDC(HDC hdc)
	{
		MUITRACE(_T("CRichEditUI::TxReleaseDC"));
		return 1;
	}

	BOOL CRichEditUI::TxShowScrollBar(INT fnBar, BOOL fShow)
	{
		MUITRACE(_T("CRichEditUI::TxShowScrollBar"));
		if( fnBar == SB_VERT && m_pVerticalScrollBar ) 
		{
			m_pVerticalScrollBar->SetVisible(fShow == TRUE);
		}
		else if( fnBar == SB_HORZ && m_pHorizontalScrollBar ) 
		{
			m_pHorizontalScrollBar->SetVisible(fShow == TRUE);
		}
		else if( fnBar == SB_BOTH ) 
		{
			if( m_pVerticalScrollBar ) m_pVerticalScrollBar->SetVisible(fShow == TRUE);
			if( m_pHorizontalScrollBar ) m_pHorizontalScrollBar->SetVisible(fShow == TRUE);
		}
		return TRUE;
	}

	BOOL CRichEditUI::TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags)
	{
		MUITRACE(_T("CRichEditUI::TxEnableScrollBar"));
		if( fuSBFlags == SB_VERT || fuSBFlags == SB_BOTH) 
		{
			if(m_pVerticalScrollBar)
			{
				m_pVerticalScrollBar->SetEnabled(true);
				m_pVerticalScrollBar->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
			}
		}

		if( fuSBFlags == SB_HORZ || fuSBFlags == SB_BOTH) 
		{
			if(m_pHorizontalScrollBar)
			{
				m_pHorizontalScrollBar->SetEnabled(true);
				m_pHorizontalScrollBar->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
			}
		}

		return TRUE;
	}

	BOOL CRichEditUI::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
	{
		MUITRACE(_T("CRichEditUI::TxSetScrollRange"));
		//设置滚动范围，这里的代码我也不知道对不对，因为我没用过这个控件
		if( fnBar == SB_VERT && m_pVerticalScrollBar ) 
		{
			if( nMaxPos - nMinPos - m_rcTxClient.bottom + m_rcTxClient.top <= 0 ) 
			{
				m_pVerticalScrollBar->SetVisible(false);
			}
			else 
			{
				m_pVerticalScrollBar->SetVisible(true);
				m_pVerticalScrollBar->SetDragBarMaxShift(nMaxPos - nMinPos - m_rcTxClient.bottom + m_rcTxClient.top);
			}
		}

		if( fnBar == SB_HORZ && m_pHorizontalScrollBar ) 
		{
			if( nMaxPos - nMinPos - m_rcTxClient.right + m_rcTxClient.left <= 0 ) 
			{
				m_pHorizontalScrollBar->SetVisible(false);
			}
			else 
			{
				m_pHorizontalScrollBar->SetVisible(true);
				m_pHorizontalScrollBar->SetDragBarMaxShift(nMaxPos - nMinPos - m_rcTxClient.right + m_rcTxClient.left);
			}   
		}
		return TRUE;
	}

	BOOL CRichEditUI::TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw)
	{
		MUITRACE(_T("CRichEditUI::TxSetScrollPos"));

		if( fnBar == SB_VERT && m_pVerticalScrollBar ) 
		{
			//设置滚动条位置偏移的位置(滚动到哪里)
			m_pVerticalScrollBar->SetDragBarShift(nPos);
		}
		else if( fnBar == SB_HORZ && m_pHorizontalScrollBar ) 
		{
			//设置滚动条位置偏移的位置(滚动到哪里)
			m_pHorizontalScrollBar->SetDragBarShift(nPos);
		}
		return TRUE;
	}

	void CRichEditUI::TxInvalidateRect(LPCRECT prc, BOOL fMode)
	{
		MUITRACE(_T("CRichEditUI::TxCreateCaret"));
		this->Invalidate();
	}

	void CRichEditUI::TxViewChange(BOOL fUpdate) 
	{
		MUITRACE(_T("CRichEditUI::TxViewChange"));
		this->Invalidate();
	}

	BOOL CRichEditUI::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
	{
		MUITRACE(_T("CRichEditUI::TxCreateCaret"));
		return ::CreateCaret(m_pShareInfo->hWnd, hbmp, xWidth, yHeight);
	}

	BOOL CRichEditUI::TxShowCaret(BOOL fShow)
	{
		MUITRACE(_T("CRichEditUI::TxShowCaret"));
		if(fShow)
			return ::ShowCaret(m_pShareInfo->hWnd);
		else
			return ::HideCaret(m_pShareInfo->hWnd);
	}

	BOOL CRichEditUI::TxSetCaretPos(INT x, INT y)
	{
		MUITRACE(_T("CRichEditUI::TxSetCaretPos"));
		return ::SetCaretPos(x, y);
	}

	BOOL CRichEditUI::TxSetTimer(UINT idTimer, UINT uTimeout)
	{
		MUITRACE(_T("CRichEditUI::TxSetTimer"));
		m_fTimer = TRUE;
		return this->SetTimer(idTimer, uTimeout);
	}

	void CRichEditUI::TxKillTimer(UINT idTimer)
	{
		MUITRACE(_T("CRichEditUI::TxKillTimer"));
		this->KillTimer(idTimer);
		m_fTimer = FALSE;
	}

	void CRichEditUI::TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll,	LPCRECT lprcClip,	HRGN hrgnUpdate, LPRECT lprcUpdate,	UINT fuScroll)	
	{
		MUITRACE(_T("CRichEditUI::TxScrollWindowEx"));
		return;
	}

	void CRichEditUI::TxSetCapture(BOOL fCapture)
	{
		MUITRACE(_T("CRichEditUI::TxSetFocus"));
		if (fCapture)
		{
			this->SetCapture();
		}
		else
		{
			this->ReleaseCapture();
		}
		m_fCaptured = fCapture;
	}

	void CRichEditUI::TxSetFocus()
	{
		MUITRACE(_T("CRichEditUI::TxSetFocus"));
		this->SetFocus();
	}

	void CRichEditUI::TxSetCursor(HCURSOR hcur,	BOOL fText)
	{
		MUITRACE(_T("CRichEditUI::TxSetCursor"));
		::SetCursor(hcur);
	}

	BOOL CRichEditUI::TxScreenToClient(LPPOINT lppt)
	{
		MUITRACE(_T("CRichEditUI::TxGetClientRect"));
		return ::ScreenToClient(m_pShareInfo->hWnd, lppt);	
	}

	BOOL CRichEditUI::TxClientToScreen(LPPOINT lppt)
	{
		MUITRACE(_T("CRichEditUI::TxGetClientRect"));
		return ::ClientToScreen(m_pShareInfo->hWnd, lppt);
	}

	HRESULT CRichEditUI::TxActivate(LONG *plOldState)
	{
		MUITRACE(_T("CRichEditUI::TxActivate"));
		return S_OK;
	}

	HRESULT CRichEditUI::TxDeactivate(LONG lNewState)
	{
		MUITRACE(_T("CRichEditUI::TxDeactivate"));
		return S_OK;
	}

	HRESULT CRichEditUI::TxGetClientRect(LPRECT prc)
	{
		MUITRACE(_T("CRichEditUI::TxGetClientRect"));
		*prc = m_rcTxClient;
		return NOERROR;
	}

	HRESULT CRichEditUI::TxGetViewInset(LPRECT prc) 
	{
		MUITRACE(_T("CRichEditUI::TxGetViewInset"));
		prc->left = prc->right = prc->top = prc->bottom = 0;
		return NOERROR;	
	}

	HRESULT CRichEditUI::TxGetCharFormat(const CHARFORMATW **ppCF)
	{
		MUITRACE(_T("CRichEditUI::TxGetCharFormat"));
		*ppCF = &m_CharFormat;
		return NOERROR;
	}

	HRESULT CRichEditUI::TxGetParaFormat(const PARAFORMAT **ppPF)
	{
		MUITRACE(_T("CRichEditUI::TxGetParaFormat"));
		*ppPF = &m_ParaFormat;
		return NOERROR;
	}

	COLORREF CRichEditUI::TxGetSysColor(int nIndex) 
	{
		MUITRACE(_T("CRichEditUI::TxGetSysColor"));
		return ::GetSysColor(nIndex);
	}

	HRESULT CRichEditUI::TxGetBackStyle(TXTBACKSTYLE *pstyle)
	{
		MUITRACE(_T("CRichEditUI::TxGetBackStyle"));
		*pstyle = !m_fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
		return NOERROR;
	}

	HRESULT CRichEditUI::TxGetMaxLength(DWORD *pLength)
	{
		MUITRACE(_T("CRichEditUI::TxGetMaxLength"));
		*pLength = m_cchTextMost;
		return NOERROR;
	}

	HRESULT CRichEditUI::TxGetScrollBars(DWORD *pdwScrollBar)
	{
		MUITRACE(_T("CRichEditUI::TxGetScrollBars"));
		*pdwScrollBar =  m_dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | 
			ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

		return NOERROR;
	}

	HRESULT CRichEditUI::TxGetPasswordChar(TCHAR *pch)
	{
		MUITRACE(_T("CRichEditUI::TxGetPasswordChar"));
	#ifdef _UNICODE
		*pch = m_chPasswordChar;
	#else
		::WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, NULL, NULL) ;
	#endif
		return NOERROR;
	}

	HRESULT CRichEditUI::TxGetAcceleratorPos(LONG *pcp)
	{
		MUITRACE(_T("CRichEditUI::TxGetAcceleratorPos"));
		*pcp = m_laccelpos;
		return S_OK;
	} 										   

	HRESULT CRichEditUI::OnTxCharFormatChange(const CHARFORMATW *pcf)
	{
		MUITRACE(_T("CRichEditUI::OnTxCharFormatChange"));
		return S_OK;
	}

	HRESULT CRichEditUI::OnTxParaFormatChange(const PARAFORMAT *ppf)
	{
		MUITRACE(_T("CRichEditUI::OnTxParaFormatChange"));
		return S_OK;
	}

	HRESULT CRichEditUI::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) 
	{
		MUITRACE(_T("CRichEditUI::TxGetPropertyBits"));
		DWORD dwProperties = 0;

		if (m_fRich)
		{
			dwProperties = TXTBIT_RICHTEXT;
		}

		if (m_dwStyle & ES_MULTILINE)
		{
			dwProperties |= TXTBIT_MULTILINE;
		}

		if (m_dwStyle & ES_READONLY)
		{
			dwProperties |= TXTBIT_READONLY;
		}

		if (m_dwStyle & ES_PASSWORD)
		{
			dwProperties |= TXTBIT_USEPASSWORD;
		}

		if (!(m_dwStyle & ES_NOHIDESEL))
		{
			dwProperties |= TXTBIT_HIDESELECTION;
		}

		if (m_fEnableAutoWordSel)
		{
			dwProperties |= TXTBIT_AUTOWORDSEL;
		}

		if (m_fWordWrap)
		{
			dwProperties |= TXTBIT_WORDWRAP;
		}

		if (m_fAllowBeep)
		{
			dwProperties |= TXTBIT_ALLOWBEEP;
		}

		if (m_fSaveSelection)
		{
			dwProperties |= TXTBIT_SAVESELECTION;
		}

		*pdwBits = dwProperties & dwMask; 
		return NOERROR;
	}

	HRESULT CRichEditUI::TxNotify(DWORD iNotify, void *pv)
	{
		MUITRACE(_T("CRichEditUI::SetWordWrap"));
		RECT rc;
		REQRESIZE *preqsz = (REQRESIZE *)pv;

		if( iNotify == EN_REQUESTRESIZE ) 
		{
			rc = m_rcTxClient;
			//rc.bottom = rc.top + preqsz->rc.bottom;
			//rc.right  = rc.left + preqsz->rc.right;

			//this->SetItem(

		}

		switch(iNotify)
		{ 
		case EN_DROPFILES:   
		case EN_MSGFILTER:   
		case EN_OLEOPFAILED:   
		case EN_PROTECTED:   
		case EN_SAVECLIPBOARD:   
		case EN_SELCHANGE:   
		case EN_STOPNOUNDO:   
		case EN_LINK:   
		case EN_OBJECTPOSITIONS:   
		case EN_DRAGDROPDONE:   
			{
				if(pv)                        // Fill out NMHDR portion of pv   
				{   
					LONG nId =  GetWindowLong(m_pShareInfo->hWnd, GWL_ID);   
					NMHDR  *phdr = (NMHDR *)pv;   
					phdr->hwndFrom = m_pShareInfo->hWnd;   
					phdr->idFrom = nId;   
					phdr->code = iNotify;  

					if(::SendMessage(m_pShareInfo->hWnd, WM_NOTIFY, (WPARAM) nId, (LPARAM) pv))   
					{   
						//hr = S_FALSE;   
					}
				}    
			}
			break;
		}

		return S_OK;
	}

	HRESULT CRichEditUI::TxGetExtent(LPSIZEL lpExtent)
	{
		MUITRACE(_T("CRichEditUI::TxGetExtent"));
		*lpExtent = m_sizelExtent;
		return S_OK;
	}

	HRESULT	CRichEditUI::TxGetSelectionBarWidth (LONG *plSelBarWidth)
	{
		MUITRACE(_T("CRichEditUI::TxGetSelectionBarWidth"));
		*plSelBarWidth = m_lSelBarWidth;
		return S_OK;
	}

	void CRichEditUI::SetWordWrap(BOOL fWordWrap)
	{
		MUITRACE(_T("CRichEditUI::SetWordWrap"));
		m_fWordWrap = fWordWrap;
		m_pServices->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, m_fWordWrap ? TXTBIT_WORDWRAP : 0);
	}

	BOOL CRichEditUI::GetReadOnly()
	{
		MUITRACE(_T("CRichEditUI::GetReadOnly"));
		return (m_dwStyle & ES_READONLY) != 0;
	}

	void CRichEditUI::SetReadOnly(BOOL fReadOnly)
	{
		MUITRACE(_T("CRichEditUI::SetReadOnly"));
		if (fReadOnly)
		{
			m_dwStyle |= ES_READONLY;
		}
		else
		{
			m_dwStyle &= ~ES_READONLY;
		}

		m_pServices->OnTxPropertyBitsChange(TXTBIT_READONLY, 
			fReadOnly ? TXTBIT_READONLY : 0);
	}

	void CRichEditUI::SetFont(HFONT hFont) 
	{
		MUITRACE(_T("CRichEditUI::SetFont"));
		if( hFont == NULL ) return;
		LOGFONT lf;
		::GetObject(hFont, sizeof(LOGFONT), &lf);
		LONG yPixPerInch = ::GetDeviceCaps(m_pShareInfo->hPaintDC , LOGPIXELSY);
		m_CharFormat.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
		if(lf.lfWeight >= FW_BOLD)
			m_CharFormat.dwEffects |= CFE_BOLD;
		if(lf.lfItalic)
			m_CharFormat.dwEffects |= CFE_ITALIC;
		if(lf.lfUnderline)
			m_CharFormat.dwEffects |= CFE_UNDERLINE;
		m_CharFormat.bCharSet = lf.lfCharSet;
		m_CharFormat.bPitchAndFamily = lf.lfPitchAndFamily;
	#ifdef _UNICODE
		_tcscpy(m_CharFormat.szFaceName, lf.lfFaceName);
	#else
		//need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
		MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, cf.szFaceName, LF_FACESIZE) ;
	#endif

		m_pServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
	}

	void CRichEditUI::SetColor(DWORD dwColor)
	{
		MUITRACE(_T("CRichEditUI::SetColor"));
		m_CharFormat.crTextColor = ARGB(0, GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
		m_pServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
	}

	SIZEL* CRichEditUI::GetExtent() 
	{
		MUITRACE(_T("CRichEditUI::GetExtent"));
		return &m_sizelExtent;
	}

	void CRichEditUI::SetExtent(SIZEL *psizelExtent) 
	{
		MUITRACE(_T("CRichEditUI::SetExtent"));
		m_sizelExtent = *psizelExtent; 
		m_pServices->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
	}

	void CRichEditUI::LimitText(LONG nChars)
	{
		MUITRACE(_T("CRichEditUI::LimitText"));
		m_cchTextMost = nChars;
		if( m_cchTextMost <= 0 ) m_cchTextMost = g_cInitTextMax;
		m_pServices->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
	}

	BOOL CRichEditUI::IsCaptured()
	{
		MUITRACE(_T("CRichEditUI::IsCaptured"));
		return m_fCaptured;
	}

	BOOL CRichEditUI::GetAllowBeep()
	{
		MUITRACE(_T("CRichEditUI::GetAllowBeep"));
		return m_fAllowBeep;
	}

	void CRichEditUI::SetAllowBeep(BOOL fAllowBeep)
	{
		MUITRACE(_T("CRichEditUI::SetAllowBeep"));
		m_fAllowBeep = fAllowBeep;

		m_pServices->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, 
			fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
	}

	WORD CRichEditUI::GetDefaultAlign()
	{
		MUITRACE(_T("CRichEditUI::GetDefaultAlign"));
		return m_ParaFormat.wAlignment;
	}

	void CRichEditUI::SetDefaultAlign(WORD wNewAlign)
	{
		MUITRACE(_T("CRichEditUI::SetDefaultAlign"));
		m_ParaFormat.wAlignment = wNewAlign;

		// Notify control of property change
		m_pServices->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}

	BOOL CRichEditUI::GetRichTextFlag()
	{
		MUITRACE(_T("CRichEditUI::GetRichTextFlag"));
		return m_fRich;
	}

	void CRichEditUI::SetRichTextFlag(BOOL fNew)
	{
		MUITRACE(_T("CRichEditUI::SetRichTextFlag"));
		m_fRich = fNew;

		m_pServices->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, 
			fNew ? TXTBIT_RICHTEXT : 0);
	}

	LONG CRichEditUI::GetDefaultLeftIndent()
	{
		MUITRACE(_T("CRichEditUI::GetDefaultLeftIndent"));
		return m_ParaFormat.dxOffset;
	}

	void CRichEditUI::SetDefaultLeftIndent(LONG lNewIndent)
	{
		MUITRACE(_T("CRichEditUI::SetDefaultLeftIndent"));
		m_ParaFormat.dxOffset = lNewIndent;

		m_pServices->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
	}

	BOOL CRichEditUI::SetSaveSelection(BOOL fSaveSelection)
	{
		MUITRACE(_T("CRichEditUI::SetSaveSelection"));
		BOOL fResult = m_fSaveSelection;

		m_fSaveSelection = fSaveSelection;

		// notify text services of property change
		m_pServices->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, 
			m_fSaveSelection ? TXTBIT_SAVESELECTION : 0);

		return fResult;		
	}

	HRESULT	CRichEditUI::OnTxInPlaceDeactivate()
	{
		MUITRACE(_T("CRichEditUI::OnTxInPlaceDeactivate"));
		HRESULT hr = m_pServices->OnTxInPlaceDeactivate();

		if (SUCCEEDED(hr))
		{
			m_fInplaceActive = FALSE;
		}

		return hr;
	}

	HRESULT	CRichEditUI::OnTxInPlaceActivate(LPCRECT prcClient)
	{
		MUITRACE(_T("CRichEditUI::OnTxInPlaceActivate"));
		m_fInplaceActive = TRUE;

		HRESULT hr = m_pServices->OnTxInPlaceActivate(prcClient);

		if (FAILED(hr))
		{
			m_fInplaceActive = FALSE;
		}

		return hr;
	}

	BOOL CRichEditUI::DoSetCursor(RECT *prc, POINT *pt)
	{
		MUITRACE(_T("CRichEditUI::GetControlRect"));
		RECT rc = prc ? *prc : m_rcTxClient;

		// Is this in our rectangle?
		if (PtInRect(&rc, *pt))
		{
			RECT *prcClient = (!m_fInplaceActive || prc) ? &rc : NULL;
			m_pServices->OnTxSetCursor(DVASPECT_CONTENT,	-1, NULL, NULL, m_pShareInfo->hPaintDC,
				NULL, prcClient, pt->x, pt->y);

			return TRUE;
		}

		return FALSE;
	}

	void CRichEditUI::SetTransparent(BOOL fTransparent)
	{
		MUITRACE(_T("CRichEditUI::SetTransparent"));
		m_fTransparent = fTransparent;

		// notify text services of property change
		m_pServices->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
	}

	LONG CRichEditUI::SetAccelPos(LONG laccelpos)
	{
		MUITRACE(_T("CRichEditUI::SetAccelPos"));
		LONG laccelposOld = m_laccelpos;

		m_laccelpos = laccelpos;

		// notify text services of property change
		m_pServices->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

		return laccelposOld;
	}

	WCHAR CRichEditUI::SetPasswordChar(WCHAR chPasswordChar)
	{
		MUITRACE(_T("CRichEditUI::SetAccelPos"));
		WCHAR chOldPasswordChar = m_chPasswordChar;

		m_chPasswordChar = chPasswordChar;

		// notify text services of property change
		m_pServices->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, 
			(chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

		return chOldPasswordChar;
	}

	void CRichEditUI::SetDisabled(BOOL fOn)
	{
		MUITRACE(_T("CRichEditUI::SetDisabled"));
		m_CharFormat.dwMask	 |= CFM_COLOR | CFM_DISABLED;
		m_CharFormat.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

		if( !fOn )
		{
			m_CharFormat.dwEffects &= ~CFE_DISABLED;
		}

		m_pServices->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, 
			TXTBIT_CHARFORMATCHANGE);
	}

	LONG CRichEditUI::SetSelBarWidth(LONG lSelBarWidth)
	{
		MUITRACE(_T("CRichEditUI::SetSelBarWidth"));
		LONG lOldSelBarWidth = m_lSelBarWidth;

		m_lSelBarWidth = lSelBarWidth;

		if (m_lSelBarWidth)
		{
			m_dwStyle |= ES_SELECTIONBAR;
		}
		else
		{
			m_dwStyle &= (~ES_SELECTIONBAR);
		}
		
		m_pServices->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

		return lOldSelBarWidth;
	}

	BOOL CRichEditUI::GetTimerState()
	{
		MUITRACE(_T("CRichEditUI::GetTimerState"));
		return m_fTimer;
	}

	void CRichEditUI::SetCharFormat(CHARFORMAT2W &cf)
	{
		MUITRACE(_T("CRichEditUI::SetCharFormat"));
		m_CharFormat = cf;
	}

	void CRichEditUI::SetParaFormat(PARAFORMAT2 &pf)
	{
		MUITRACE(_T("CTxtWinHost::SetParaFormat"));
		m_ParaFormat = pf;
	}

/********************************************************************************
 * CRichEditUI 富文本控件
 * CContainerUI 派生实现
 *
 *
********************************************************************************/

	void CRichEditUI::OnAttach(HWND hNewWnd)
	{
		if(NULL == m_hDll) Init();
		return __super::OnAttach(hNewWnd);
	}

	void CRichEditUI::OnDetach(HWND hOldWnd)
	{
		return __super::OnDetach(hOldWnd);
	}

	bool CRichEditUI::OnPaint(const RECT& rcUpdate)
	{
		if(false == __super::OnPaint(rcUpdate)) return false;

		if(NULL == m_pServices) return true;

		RECT rcClient = m_rcClient;
		RECT rcDraw = { 0 };
		if (false == GetItemFixedRect(rcDraw)) return true;

		HDC hdc = m_pShareInfo->pRenderEngine->GetMemDC();
		m_pServices->TxDraw(
			DVASPECT_CONTENT,  		// Draw Aspect
            /*-1*/0,				// Lindex
            NULL,					// Info for drawing optimazation
            NULL,					// target device information
            hdc,			        // Draw device HDC
            NULL, 				   	// Target device HDC
            (RECTL*)&rcClient,		// Bounding client rectangle
            NULL, 		            // Clipping rectangle for metafiles
            (RECT*)&rcDraw,		// Update rectangle
            NULL, 	   				// Call back function
            NULL,					// Call back parameter
            0);				        // What view of the object

		m_pShareInfo->pRenderEngine->ReleaseMemDC(hdc);
		return true;
	}

	void CRichEditUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
	{
		if(0)
		{
		}
		else
		{
			__super::SetAttribute(strItem, strValue);
		}
	}

	SIZE CRichEditUI::GetContentSize()
	{
		SIZE szContent = {0,0};

		return szContent;
	}

	bool CRichEditUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		if(false == __super::SetItem(rcItem, bMustUpdate)) return false;

		m_rcTxClient = m_rcItem;


		if(!m_pShareInfo || !m_pShareInfo->hPaintDC) return true;

		LONG xPerInch = ::GetDeviceCaps(m_pShareInfo->hPaintDC, LOGPIXELSX); 
		LONG yPerInch =	::GetDeviceCaps(m_pShareInfo->hPaintDC, LOGPIXELSY); 
		
		m_sizelExtent.cx = ::MulDiv(m_rcTxClient.right - m_rcTxClient.left, HIMETRIC_PER_INCH, xPerInch);
		m_sizelExtent.cy = ::MulDiv(m_rcTxClient.bottom - m_rcTxClient.top, HIMETRIC_PER_INCH, yPerInch);

		m_pServices->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);

		return true;
	}

	LRESULT CRichEditUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		POINT point;
		bool bRet = false;
		LRESULT lResult = 0;
		HRESULT hr = S_FALSE;
		RECT rect;
		if(m_pServices)
		{
			if( (message >= WM_KEYFIRST && message <= WM_KEYLAST) || message == WM_CHAR || message == WM_IME_CHAR )
			{
				hr = m_pServices->TxSendMessage(message, wParam, lParam, &lResult);
			}
			else if(message >= WM_MOUSEFIRST && message <= WM_MOUSELAST )
			{
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);

				if(true == GetItemFixedRect(rect))
				{
					point.x += rect.left;
					point.y += rect.top;
				}

				hr = m_pServices->TxSendMessage(message, wParam, MAKELONG(point.x,point.y), &lResult);
			}
		}

		switch(message)
		{
		case WM_TIMER:
			{
				m_pServices->TxSendMessage(WM_TIMER, wParam, lParam, 0);
				return 0;
			}break;
		case WM_SETCURSOR:
			{
				this->DoSetCursor(NULL, &m_ptMouse);
				return 0;
			}break;
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			{
				m_pServices->OnTxInPlaceActivate(NULL);
				m_pServices->TxSendMessage(message, 0, 0, 0);
			}break;
		case WMU_MOUSEENTER:
		case WM_MOUSELEAVE:
			{
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);
				m_ptMouse = point;
			}break;
		case WM_MOUSEMOVE:
			{
				point.x = GET_X_LPARAM(lParam);
				point.y = GET_Y_LPARAM(lParam);
				m_ptMouse = point;
			}break;
		default:
			break;
		}
		return __super::WndProc(message, wParam, lParam);
	}
}