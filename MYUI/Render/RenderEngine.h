
#ifndef _MYUI_RENDER_ENGINE_H_
#define _MYUI_RENDER_ENGINE_H_

#include "SkinManager.h"

namespace MYUI
{
	//这个一个抽象类，系统默认提供了GDI引擎，如果想使用自定义引擎
	//请继承此类，实现相应的虚函数
	//用户无需关注UI库如何调用引擎，只需实现引擎接口即可
	//详细可参考GDI引擎
	typedef HANDLE  HCLIP;
	typedef HANDLE  HOBJECT;
#define POINTDIFF  4//用户并不能百分百击中一个像素，一般差距为4个点
//*************************************************************
//test out style 属性说明：
//默认为属性为 TS_SINGLELINE + TS_LEFT
//自动换行必须为多行
//自动换行只支持TS_LEFT
//TOS_CENTER 只支持单行
//*************************************************************
#define TOS_CENTER         0x00000001//居中，只有单行的时候生效
#define TOS_SINGLELINE     0x00000010//单行
#define TOS_MULTILLINE     0x00000020//多行
#define TOS_AUTOLINEFREE   0x00000040//自动换行


//#define TOS_LEFT           0x00000000//靠左
//#define TOS_RIGHT          0x00000001//靠右
//#define TOS_CENTER         0x00000002//水平居中
//#define TOS_VENTER         0x00000004//垂直居中

#define TABSPACE   8  //制表符占据的空格
	class MYUI_API CRenderEngine 
	{
	public:
        CRenderEngine(HWND hWnd);
		virtual ~CRenderEngine();

        //设置皮肤
        bool SetSkinFolder(LPCTSTR strSkin);
        LPCTSTR GetSkinFolder() const;

		//窗口调用
		virtual bool BeginPaint(const RECT &rcScreen) = 0;
		virtual bool EndPaint(const RECT &rcUpdate) = 0;

		//控件内部调用
		virtual HCLIP EnterClip(const RECT &rcPaint, SIZE &szRound) = 0;
		virtual void  LeaveClip(HCLIP hOldClip) = 0;

		//打算自绘，将缓冲DC提取出来，自己在上面绘制
		virtual HDC GetMemDC() = 0;
		virtual void  ReleaseMemDC(HDC hMemDc) = 0;

		//自定义的简易富文本格式
		virtual bool OnDrawHtmlText(const RECT &rcDraw, LPCTSTR strText,
			  CMuiIdArray * FontArray, ARGBREF refTextColor) = 0;

		//strImage中包含了图片绘制的参数，OnDrawImage将参数解析出来后，再调用重载函数
		virtual bool OnDrawImage(const RECT &rcDraw, LPCTSTR strImage);
		
		//下面是需要独立完成的抽象函数
		virtual bool OnDrawFrame(const RECT &rcDraw, ARGBREF refColor) = 0;
		virtual bool OnDrawLine(const POINT &ptBegin, const POINT &ptEnd, int nPenSize, ARGBREF refColor) = 0;
		virtual bool OnDrawColor(const RECT &rcDraw, ARGBREF refColor) = 0;
		virtual bool OnDrawText(const RECT &rcDraw,  LPCTSTR strText, ARGBREF refTextColor, HFONT hFont, DWORD dwType) = 0;
		virtual bool OnDrawBroder(const RECT &rcDraw, ARGBREF refColor, int nBroderSize, SIZE &szRound) = 0;


        //@rcDraw 要绘制的区域
        //@pImageInfo 图片信息
        //@rcSource 取位图哪个区域
        //@rcCorner 九宫格分隔
        //@refMask 屏蔽颜色
        //@dwFade 透明度
        //@bHole 中间不绘制
		virtual bool OnDrawImage(const RECT &rcDraw,IMAGEINFO * pImageInfo, const RECT &rcSource,
			const RECT &rcCorner, ARGBREF refMask, DWORD dwFade, bool bHole) = 0;

		//取得文本高度
		virtual int GetTextHeight(HFONT hFont) = 0;

		//取得文本尺寸
		virtual SIZE GetTextSize(const RECT &rcItem, LPCTSTR strText, int nLenght, 
			HFONT hFont, int nRowSpace, DWORD dwTextStyle) = 0;

		//通过ptMouse位置取得字符串中对应的文字索引，因为ptMouse的位置可能有偏差，
		//所以返回时会调整ptMouse到正确的索引位置
		virtual int TestTextIndex(const RECT &rcItem, POINT &ptMouse, 
			LPCTSTR strText, int nLenght, HFONT hFont, int nRowSpace, DWORD dwTextStyle) = 0;

		//根据文本索引（nTextIndex）取得TextOut的坐标值，
		//返回的POINT也可以作为光标位置
		virtual POINT GetTextPos(const RECT &rcItem, int nTextIndex, LPCTSTR strText, 
			HFONT hFont, int nRowSpace, DWORD dwTextStyle) = 0;

		//输出文本，ptOutput返回文本最后的输出位置
		virtual bool OnTextOut(const RECT &rcItem, POINT &ptOutput, LPCTSTR strText, int nStrLenght, 
			ARGBREF refTextColor, HFONT hFont, int nRowSpace, DWORD dwTextStyle, ARGBREF refTextBkColor) = 0;

		//设置光标位置，如果光标超出rcItem，将会不显示
		virtual bool SetCaret(const RECT &rcItem, const POINT ptCaret, HFONT hFont) = 0;

		//绘制OLE对象
		virtual bool DrawOleObject(const RECT &rcItem, IViewObject * pViewObject) = 0;

	protected:
		HWND m_hWnd;
        TCHAR m_strSkin[MAX_PATH];
	};
}

#endif