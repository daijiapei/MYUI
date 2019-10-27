
#ifndef _MYUI_GDI_RENDER_ENGINE_H_
#define _MYUI_GDI_RENDER_ENGINE_H_

#include "RenderEngine.h"

namespace MYUI
{
	class CGdiRenderEngine : public CRenderEngine
	{
	public:
		CGdiRenderEngine(HWND hWnd);
		virtual ~CGdiRenderEngine();

		//���ڵ���
		virtual bool BeginPaint(const RECT &rcScreen);
		virtual bool EndPaint(const RECT &rcUpdate);

		//�ؼ��ڲ�����
		virtual HCLIP EnterClip(const RECT &rcClient, SIZE &szRound);
		virtual void  LeaveClip(HCLIP hOldClip);

		//�����Ի棬������DC��ȡ�������Լ����������
		virtual HDC GetMemDC();
		virtual void  ReleaseMemDC(HDC hMemDc);
		
		virtual bool OnDrawFrame(const RECT &rcDraw, ARGBREF refColor);
		virtual bool OnDrawLine(const POINT &ptBegin,const POINT& ptEnd, int nPenSize, ARGBREF refColor);
		virtual bool OnDrawText(const RECT &rcDraw, LPCTSTR strText,ARGBREF refTextColor, HFONT hFont, DWORD dwType);
		virtual bool OnDrawBroder(const RECT &rcDraw, ARGBREF refColor, int nBroderSize, SIZE &szRound);
		virtual bool OnDrawColor(const RECT &rcDraw, ARGBREF refColor);
		virtual bool OnDrawImage(const RECT &rcDraw,IMAGEINFO * pImageInfo, const RECT &rcSource,
			const RECT &rcCorner, ARGBREF refMask, DWORD dwFade, bool bHole);

		virtual bool OnDrawHtmlText(const RECT &rcDraw, LPCTSTR strText, 
			  CMuiIdArray * FontArray, ARGBREF refTextColor);

		virtual int GetTextHeight(HFONT hFont);
		virtual SIZE GetTextSize(const RECT &rcItem, LPCTSTR strText, int nLenght, HFONT hFont, 
			int nRowSpace, DWORD dwTextStyle);
		virtual bool OnTextOut(const RECT &rcItem, POINT &ptOutput, LPCTSTR strText, int nStrLenght, 
			ARGBREF refTextColor,HFONT hFont, int nRowSpace,  DWORD dwTextStyle,  ARGBREF refTextBkColor);

		virtual POINT GetTextPos(const RECT &rcItem, int nTextIndex, LPCTSTR strText,
			HFONT hFont, int nRowSpace, DWORD dwTextStyle);
		virtual int TestTextIndex(const RECT &rcItem, POINT &ptMouse, 
			LPCTSTR strText, int nLenght, HFONT hFont, int nRowSpace, DWORD dwTextStyle);

		//virtual void KissCaret();
		virtual bool SetCaret(const RECT &rcItem, const POINT ptCaret, HFONT hFont);

		virtual bool DrawOleObject(const RECT &rcItem, IViewObject * pViewObject);

	private:

		//ʹ��rcLineҪע�⣬rcLine��������left���룬ÿ��ʹ��rcLine��Ҫ��rcLine.leftƫ�Ƶ����µ�λ��
        bool DrawHtmlTextLine(RECT &rcLine, LPCTSTR strText, int nLenght, int &index, CMuiIdArray * FontArray,
			int nFontId, ARGBREF refTextColor, ARGBREF refTextBkColor, DWORD dwTextStyle);

		//ȡ�ø��ı�һ�еĸ߶�, 
		//nHeight ���ı���Ҫ�ĸ߶ȣ�
		//nRowSpace ���о�
		//return ��һ����TCHAR�ĸ���
		int GetHtmlTextLineHeight(LPCTSTR strText, CMuiIdArray * FontArray, int &nHeight, int &nRowSpace);

		//������������ʾ�ı�
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