
#ifndef __MYUI_GDI_RENDER_ENGINE_H__
#define __MYUI_GDI_RENDER_ENGINE_H__

#include "RenderEngine.h"

namespace MYUI
{
	class CGdiRenderEngine : public CRenderEngine
	{
	public:
		CGdiRenderEngine(HWND hWnd);
		virtual ~CGdiRenderEngine();

	
		//引擎名称
		virtual LPCTSTR GetName() const;

		//窗口调用
		virtual bool BeginPaint(const RECT &rcScreen);
		virtual bool EndPaint(const RECT &rcUpdate);

	
		//打算自绘，将缓冲DC提取出来，自己在上面绘制
		virtual HDC GetMemDC();
		virtual void  ReleaseMemDC(HDC hMemDc);
	
	protected:
		virtual HCLIP EnterClipImp(const RECT& rcClient, SIZE& szRound);
		virtual void  LeaveClipImp(HCLIP hOldClip);
		
		virtual bool DrawFrameImp(const RECT &rcDraw, ARGBREF refColor);
		virtual bool DrawLineImp(const POINT &ptBegin,const POINT& ptEnd, int nPenSize, ARGBREF refColor);
		virtual bool DrawTextImp(const RECT &rcDraw, LPCTSTR strText,ARGBREF refTextColor, HFONT hFont, DWORD dwType);
		virtual bool DrawBroderImp(const RECT &rcDraw, ARGBREF refColor, int nBroderSize, SIZE &szRound);
		virtual bool DrawColorImp(const RECT &rcDraw, ARGBREF refColor);
		virtual bool DrawImageImp(const RECT &rcDraw, MUIIMAGEINFO * pImageInfo, const RECT &rcSource,
			const RECT &rcCorner, ARGBREF refMask, DWORD dwFade, BOOL bHole);

		virtual bool DrawHtmlTextImp(const RECT &rcDraw, LPCTSTR strText,
			  CMuiIdArray * FontArray, ARGBREF refTextColor);

		virtual int GetTextHeightImp(HFONT hFont);

		virtual int TestTextIndexImp(const RECT& rcItem, POINT& ptMouse,
			LPCTSTR strText, int nLenght, HFONT hFont, int nRowSpace, DWORD dwTextStyle);

		virtual SIZE GetTextSizeImp(const RECT &rcItem, LPCTSTR strText, int nLenght, HFONT hFont,
			int nRowSpace, DWORD dwTextStyle);

		virtual bool TextOutImp(const RECT &rcItem, POINT &ptOutput, LPCTSTR strText, int nStrLenght,
			ARGBREF refTextColor,HFONT hFont, int nRowSpace,  DWORD dwTextStyle,  ARGBREF refTextBkColor);

		virtual POINT GetTextPosImp(const RECT &rcItem, int nTextIndex, LPCTSTR strText,
			HFONT hFont, int nRowSpace, DWORD dwTextStyle);
		
		//virtual void KissCaret();
		virtual bool SetCaretImp(const RECT &rcItem, const POINT ptCaret, HFONT hFont);

		virtual bool DrawOleObjectImp(const RECT &rcItem, IViewObject * pViewObject);

	private:

		//使用rcLine要注意，rcLine绘制总是left对齐，每次使用rcLine后，要将rcLine.left偏移到最新的位置
        bool DrawHtmlTextLine(RECT &rcLine, LPCTSTR strText, int nLenght, int &index, CMuiIdArray * FontArray,
			int nFontId, ARGBREF refTextColor, ARGBREF refTextBkColor, DWORD dwTextStyle);

		//取得富文本一行的高度, 
		//nHeight 是文本需要的高度，
		//nRowSpace 是行距
		//return 是一行中TCHAR的个数
		int GetHtmlTextLineHeight(LPCTSTR strText, CMuiIdArray * FontArray, int &nHeight, int &nRowSpace);

		//触发条件后，显示文本
		bool ShowHtmlText(RECT &rcLine,LPCTSTR strText, int nLenght,CMuiIdArray * FontArray,
			int nFontId, ARGBREF refTextColor, ARGBREF refTextBkColor, DWORD dwTextStyle);
	protected:
		HDC m_hMemDc;
		HBITMAP m_hMemBitmap;
		HDC m_hTmpDc;
		HDC m_hPaintDc;
		HCLIP m_hClip;
		WCHAR * m_strBuffer;
		HFONT m_hWindowFont;
	private:

	};

}

#endif