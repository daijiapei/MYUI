
#ifndef _MYUI_ACTIVEX_EXT_CONTROL_H_
#define _MYUI_ACTIVEX_EXT_CONTROL_H_

#include "..\\Core\\UIControl.h"

struct IOleObject;

namespace MYUI
{
	//c++局部函数中使用new分配内存或者create*创建COM接口,
	//由于程序可能在函数内下面的判断代码失败退出, 这样每次都需要delete内存或者release, 
	//不仅麻烦而且可能忘记其中某个,造成资源没有及时释放.下面的通用的类可以解决这些问题:
	

	class MYUI_API CActiveXCtrl : public IOleClientSite, public IOleInPlaceSiteWindowless,
		public IOleControlSite, public IObjectWithSite, public IOleContainer
	{
	public:
		CActiveXCtrl();
		~CActiveXCtrl();

		// IUnknown
		STDMETHOD_(ULONG,AddRef)();//增加计数
		STDMETHOD_(ULONG,Release)();//减少引用计数
		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);//请求接口

		/*
		// IObjectWithSite
		Internet Explorer的BHO的对象必须实现IObjectWithSite接口.
		该接口是IE用来对插件进行管理和通讯的一个接口，其有SetSite和GetSite两个方法，
		当IE加载和卸载Band对象的时候,调用该接口的SetSite方法,
		我们可以通过SetSite传进来的IUnknown指针来获取我们所需要的接口:
		*/
		STDMETHOD(SetSite)(IUnknown *pUnkSite);//允许容器传递对象指向其站点的接口的指针。
		STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);//检索使用SETSITE传递的最新站点。

		// IOleClientSite
		STDMETHOD(SaveObject)(void);  //保存嵌入对象     
		STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);//请求组件名
		STDMETHOD(GetContainer)(IOleContainer** ppContainer);   //请求组件包容器指针
		STDMETHOD(ShowObject)(void);        //要求包容器显示对象
		STDMETHOD(OnShowWindow)(BOOL fShow);        //在组件变为可视或不可视时通知包容器
		STDMETHOD(RequestNewObjectLayout)(void);//要求包容器更改显示位置

		// IOleInPlaceSiteWindowless - 无窗口事件，其实处理本质就跟CControl类一样，
		STDMETHOD(CanWindowlessActivate)(void);//通知对象其容器是否可以支持为可以就地激活的末窗口的对象
		STDMETHOD(GetCapture)(void);//条用由就地活动，无窗口的对象确定它是否仍具有鼠标捕获
		STDMETHOD(SetCapture)(BOOL fCapture);//使一就地活动，无窗口的对象获取所有鼠标消息
		STDMETHOD(GetFocus)(void);//调用由就地活动，无窗口的对象确定它是否仍具有键盘焦点
		STDMETHOD(SetFocus)(BOOL fFocus);//设置一个用户界面有效，无效窗口的对象的键盘焦点
		STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);//提供对象的句柄设备上下文的屏幕或兼容虚拟机从其容器
		STDMETHOD(ReleaseDC)(HDC hDC);//释放以前对用活动的设备上下文到GetDC
		STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);//使对象无效其在屏幕上使用就地图像指定的一个矩形
		STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);//使对象无效其在屏幕上使用就地图像的指定区域
		STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);//使对象移动到其就地活动的映像内的区域在屏幕上
		STDMETHOD(AdjustRect)(LPRECT prc);//调整一个指定的矩形，则通过重叠完全或部分覆盖不透明的对象
		STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);//调用处理的所有消息的默认床底给对象

		// IOleInPlaceSiteEx
		STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);  //由嵌入对象调用，以确定是否需要在激活时重新绘制自身。      
		STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);  //报告中对目标标示对象的通知。
		STDMETHOD(RequestUIActivate)(void);//通知容器该对象即将进入UI活动状态。

		// IOleInPlaceSite
		STDMETHOD(CanInPlaceActivate)(void);//确定容器是否可以就地激活对象。 
		STDMETHOD(OnInPlaceActivate)(void); //通知容器它的一个对象正在被激活。  
		STDMETHOD(OnUIActivate)(void);//通知容器对象即将在适当位置被激活，并且该对象将使用就地复合菜单替换容器的主菜单。
		//使就地对象能够检索形成窗口对象层次结构的窗口接口，以及该对象的就地激活窗口应该位于父窗口中的位置。
		STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
		STDMETHOD(Scroll)(SIZE scrollExtant);//指示容器按指定的像素数滚动对象的视图。
		STDMETHOD(OnUIDeactivate)(BOOL fUndoable);//通知容器应该重新安装其用户界面并集中注意力，以及对象是否具有可撤销状态。
		STDMETHOD(OnInPlaceDeactivate)(void);//通知容器该对象不再处于活动状态。
		STDMETHOD(DiscardUndoState)( void);//指示容器丢弃其撤消状态。容器不应该调用IOLIN StaseObjult:：ReActivateAndUndo。
		STDMETHOD(DeactivateAndUndo)( void);//禁用对象，结束就地会话，并恢复到容器保存的撤消状态。
		STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);//通知容器对象扩展已更改。

		// IOleWindow
		STDMETHOD(GetWindow)(HWND* phwnd);//检索参与就地激活的一个窗口（框架、文档、父窗口或就地对象窗口）的句柄。
		STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);//确定是否应在就地激活会话期间输入上下文敏感的帮助模式。

		// IOleControlSite
		//通知容器控件的ControlINFO结构已经更改，并且容器应该调用控件的IOleControl::GetControlInfo进行更新。
		STDMETHOD(OnControlInfoChanged)(void);      
		//指示控件是否应保持在活动状态。此方法的调用通常嵌套一个事件，以确保对象的激活状态在事件的整个处理过程中保持稳定。
		STDMETHOD(LockInPlaceActive)(BOOL fLock);       
		 //检索指向容器用于包装实际控件的扩展控件的IDispatch指针。
		STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);
		//将用HIMETRIC单位（如OLE中的标准）表示的坐标转换为容器指定的单位。
		STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);     
		//将击键传递到控制站点进行处理。
		STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
		//指示此控制站点管理的控件是否已获得焦点或已失去焦点。
		STDMETHOD(OnFocus)(BOOL fGotFocus);
		//指示容器显示嵌入在此站点中的控件的属性表。
		STDMETHOD(ShowPropertyFrame)(void);

		// IOleContainer
		STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);//枚举当前容器中的对象。
		STDMETHOD(LockContainer)(BOOL fLock);//保持嵌入对象的容器运行，直到显式释放。

		// IParseDisplayName
		//解析指定的显示名称并创建相应的名字。
		STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);

		//MYUI
		STDMETHOD(SetWindow)(HWND hWnd);
	protected:

		
	protected:
		LONG m_dwRef;

		HWND m_hHostWindow;//宿主，就是父窗口
		IUnknown * m_pUnkSite;
		IViewObject * m_pViewObject;
		IOleObject * m_pOleObject;
		IOleInPlaceObjectWindowless * m_pInPlaceObject;
		bool m_bLocked;
		bool m_bFocused;
		bool m_bCaptured;
		bool m_bUIActivated;
		bool m_bWindowless;
		RECT m_rcPos;//控件的绝对位置
		
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

		//绘制
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual bool OnPaint(RECT rcItem,RECT rcPaint, RECT rcUpdate);
		
	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
		virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数
		virtual void OnDetach(HWND hOldWnd);//当控件离开一个窗口时，会调用一次该函数 
		virtual void ReleaseControl();
		void DoVerb(LONG nVerb);
	protected:
		CLSID m_clsid;
		CMuiString m_strModuleName;

	};

}

#endif