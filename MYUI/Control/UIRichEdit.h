
#ifndef _MYUI_RICHEDIT_CONTROL_H_
#define _MYUI_RICHEDIT_CONTROL_H_


#include "..\\Core\\UIContainer.h"
#include <textserv.h>

namespace MYUI
{
	class MYUI_API CRichEditUI : public CContainerUI, public ITextHost
	{
	public:
		CRichEditUI();
		virtual ~CRichEditUI();
		static CMuiString g_strClassName;

		virtual CMuiString GetClassName() const;

		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		virtual SIZE GetContentSize();

	public:
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);

	protected:
		BOOL Init();
		HRESULT InitDefaultCharFormat(CHARFORMAT2W* pcf, HFONT hFont);
		virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数
		virtual void OnDetach(HWND hOldWnd);//当控件离开一个窗口时，会调用一次该函数 

	public://ITextHost Interface

		BOOL GetWordWrap(void);
		void SetWordWrap(BOOL fWordWrap);
		BOOL GetReadOnly();
		void SetReadOnly(BOOL fReadOnly);
		void SetFont(HFONT hFont);
		void SetColor(DWORD dwColor);
		SIZEL* GetExtent();
		void SetExtent(SIZEL *psizelExtent);
		void LimitText(LONG nChars);
		BOOL IsCaptured();

		BOOL GetAllowBeep();
		void SetAllowBeep(BOOL fAllowBeep);
		WORD GetDefaultAlign();
		void SetDefaultAlign(WORD wNewAlign);
		BOOL GetRichTextFlag();
		void SetRichTextFlag(BOOL fNew);
		LONG GetDefaultLeftIndent();
		void SetDefaultLeftIndent(LONG lNewIndent);
		BOOL SetSaveSelection(BOOL fSaveSelection);
		HRESULT OnTxInPlaceDeactivate();
		HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
		BOOL GetActiveState(void);
		BOOL DoSetCursor(RECT *prc, POINT *pt);
		void SetTransparent(BOOL fTransparent);

		LONG SetAccelPos(LONG laccelpos);
		WCHAR SetPasswordChar(WCHAR chPasswordChar);
		void SetDisabled(BOOL fOn);
		LONG SetSelBarWidth(LONG lSelBarWidth);
		BOOL GetTimerState();

		void SetCharFormat(CHARFORMAT2W &cf);
		void SetParaFormat(PARAFORMAT2 &pf);

		// -----------------------------
		//	IUnknown interface
		// -----------------------------
		virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
		virtual ULONG _stdcall AddRef(void);
		virtual ULONG _stdcall Release(void);

		// -----------------------------
		//	ITextHost interface
		// -----------------------------
		virtual HDC TxGetDC();
		virtual INT TxReleaseDC(HDC hdc);
		virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
		virtual BOOL TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
		virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
		virtual BOOL TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
		virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
		virtual void TxViewChange(BOOL fUpdate);
		virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
		virtual BOOL TxShowCaret(BOOL fShow);
		virtual BOOL TxSetCaretPos(INT x, INT y);
		virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
		virtual void TxKillTimer(UINT idTimer);
		virtual void TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
		virtual void TxSetCapture(BOOL fCapture);
		virtual void TxSetFocus();
		virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
		virtual BOOL TxScreenToClient (LPPOINT lppt);
		virtual BOOL TxClientToScreen (LPPOINT lppt);
		virtual HRESULT TxActivate( LONG * plOldState );
		virtual HRESULT TxDeactivate( LONG lNewState );
		virtual HRESULT TxGetClientRect(LPRECT prc);
		virtual HRESULT TxGetViewInset(LPRECT prc);
		virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF );
		virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
		virtual COLORREF TxGetSysColor(int nIndex);
		virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
		virtual HRESULT TxGetMaxLength(DWORD *plength);
		virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
		virtual HRESULT TxGetPasswordChar(TCHAR *pch);
		virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
		virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
		virtual HRESULT OnTxCharFormatChange (const CHARFORMATW * pcf);
		virtual HRESULT OnTxParaFormatChange (const PARAFORMAT * ppf);
		virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
		virtual HRESULT TxNotify(DWORD iNotify, void *pv);
		virtual HIMC TxImmGetContext(void);
		virtual void TxImmReleaseContext(HIMC himc);
		virtual HRESULT TxGetSelectionBarWidth (LONG *lSelBarWidth);

	protected://ITextHost Interface
		HMODULE m_hDll;
		LONG m_dwRef;
		ITextServices	*m_pServices;		    // pointer to Text Services object
		// Properties

		//DWORD		m_dwTxStyle;				// style bits

		unsigned	m_fEnableAutoWordSel	:1;	// enable Word style auto word selection?
		unsigned	m_fWordWrap				:1;	// Whether control should word wrap
		unsigned	m_fAllowBeep			:1;	// Whether beep is allowed
		unsigned	m_fRich					:1;	// Whether control is rich text
		unsigned	m_fSaveSelection		:1;	// Whether to save the selection when inactive
		unsigned	m_fInplaceActive		:1; // Whether control is inplace active
		unsigned	m_fTransparent			:1; // Whether control is transparent
		unsigned	m_fTimer				:1;	// A timer is set
		unsigned    m_fCaptured				:1;

		LONG		m_lSelBarWidth;			// Width of the selection bar
		LONG  		m_cchTextMost;			// maximum text size
		DWORD		m_dwEventMask;			// DoEvent mask to pass on to parent window
		LONG		m_icf;
		LONG		m_ipf;
		RECT		m_rcTxClient;				// Client Rect for this control
		SIZEL		m_sizelExtent;			// Extent array
		CHARFORMAT2W m_CharFormat;					// Default character format
		PARAFORMAT2	m_ParaFormat;			// Default paragraph format
		LONG		m_laccelpos;				// Accelerator position
		WCHAR		m_chPasswordChar;		    // Password character

	protected://标准CControl实现
		POINT m_ptMouse;
	};

}

#endif