//
//#ifndef _MYUI_FLASH_CONTROL_H_
//#define _MYUI_FLASH_CONTROL_H_
//
//#include "UIActiveX.h"
//#include "flash11.tlh"
//
//namespace MYUI
//{
//	class CFlashEventHandler
//	{
//	public:
//		CFlashEventHandler() {}
//		~CFlashEventHandler() {}
//
//		virtual ULONG STDMETHODCALLTYPE Release( void ) { return S_OK;}
//		virtual ULONG STDMETHODCALLTYPE AddRef( void ) { return S_OK;}
//
//		virtual HRESULT OnReadyStateChange ( long newState )
//		{
//			return S_OK;
//		}
//
//		virtual HRESULT OnProgress (long percentDone )
//		{
//			return S_OK;
//		}
//
//		virtual HRESULT FSCommand ( LPCTSTR command, LPCTSTR args )
//		{
//			return S_OK;
//		}
//		
//		virtual HRESULT FlashCall ( LPCTSTR request )
//		{
//			return S_OK;
//		}
//	};
//
//	class MYUI_API CFlashUI
//		: public CActiveXUI
//		, public _IShockwaveFlashEvents
//	{
//	public:
//		CFlashUI(void);
//		~CFlashUI(void);
//
//		void SetFlashEventHandler(CFlashEventHandler* pHandler);
//		virtual bool CreateControl(const CLSID clsid = IID_NULL);
//
//		static CMuiString g_strClassName;
//		CMuiString GetClassName() const;
//
//	private:
//
//		virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( __RPC__out UINT *pctinfo );
//		virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo );
//		virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);
//		virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );
//
//		virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject );
//		virtual ULONG STDMETHODCALLTYPE AddRef( void );
//		virtual ULONG STDMETHODCALLTYPE Release( void );
//
//		HRESULT OnReadyStateChange (long newState);
//		HRESULT OnProgress(long percentDone );
//		HRESULT FSCommand (_bstr_t command, _bstr_t args);
//		HRESULT FlashCall (_bstr_t request );
//
//		virtual void ReleaseControl();
//		HRESULT RegisterEventHandler(BOOL inAdvise);
//
//		virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数
//		virtual void OnDetach(HWND hOldWnd);//当控件离开一个窗口时，会调用一次该函数 
//	private:
//		IShockwaveFlash* m_pFlash;
//		DWORD m_dwCookie;
//		CFlashEventHandler* m_pFlashEventHandler;
//	};
//};
//#endif