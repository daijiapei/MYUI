
#ifndef __MYUI_RENDER_ENGINE_H__
#define __MYUI_RENDER_ENGINE_H__

#include "SkinManager.h"

namespace MYUI
{
	//���һ�������࣬ϵͳĬ���ṩ��GDI���棬�����ʹ���Զ�������
	//��̳д��࣬ʵ����Ӧ���麯��
	//�û������עUI����ε������棬ֻ��ʵ������ӿڼ���
	//��ϸ�ɲο�GDI����
	typedef HANDLE  HCLIP;

#define POINTDIFF  4//�û������ܰٷְٻ���һ�����أ�һ����Ϊ4����
//*************************************************************
//test out style ����˵����
//Ĭ��Ϊ����Ϊ TS_SINGLELINE + TS_LEFT
//�Զ����б���Ϊ����
//�Զ�����ֻ֧��TS_LEFT
//TOS_CENTER ֻ֧�ֵ���
//*************************************************************
#define TOS_CENTER         0x00000001//���У�ֻ�е��е�ʱ����Ч
#define TOS_SINGLELINE     0x00000010//����
#define TOS_MULTILLINE     0x00000020//����
#define TOS_AUTOLINEFREE   0x00000040//�Զ�����


//#define TOS_LEFT           0x00000000//����
//#define TOS_RIGHT          0x00000001//����
//#define TOS_CENTER         0x00000002//ˮƽ����
//#define TOS_VENTER         0x00000004//��ֱ����

#define TABSPACE   8  //�Ʊ��ռ�ݵĿո�
	class MYUI_API CRenderEngine 
	{
	public:
		CRenderEngine(HWND hWnd);
		virtual ~CRenderEngine();
		static CRenderEngine* Create(LPCTSTR strName, HWND hWnd);

		//��������
		virtual LPCTSTR GetName() const = 0;

		//���ڵ���
		virtual bool BeginPaint(const RECT& rcScreen) = 0;
		virtual bool EndPaint(const RECT& rcUpdate) = 0;

		//�����Ի棬������DC��ȡ�������Լ����������
		virtual HDC GetMemDC() = 0;
		virtual void  ReleaseMemDC(HDC hMemDc) = 0;

		//����Ƥ��
		bool SetSkinFolder(LPCTSTR strSkin);
		LPCTSTR GetSkinFolder() const;

		//ƫ�Ƶ��ؼ��ڴ����ж�Ӧ�Ļ���λ��
		bool OffsetDrawPoint(int nX, int nY);
		POINT GetDrawPoint();

		//�ؼ��ڲ�����
		HCLIP EnterClip(const RECT& rcPaint, SIZE& szRound);
		void  LeaveClip(HCLIP hOldClip);

		//strImage�а�����ͼƬ���ƵĲ�����OnDrawImage�����������������ٵ������غ���
		bool OnDrawImage(const RECT& rcDraw, LPCTSTR strImage);

		bool OnDrawFrame(const RECT& rcDraw, ARGBREF refColor);
		bool OnDrawLine(const POINT& ptBegin, const POINT& ptEnd, int nPenSize, ARGBREF refColor);
		bool OnDrawColor(const RECT& rcDraw, ARGBREF refColor);
		bool OnDrawText(const RECT& rcDraw, LPCTSTR strText, ARGBREF refTextColor, HFONT hFont, DWORD dwType);
		bool OnDrawBroder(const RECT& rcDraw, ARGBREF refColor, int nBroderSize, SIZE& szRound);

		//@rcDraw Ҫ���Ƶ�����
		//@pImageInfo ͼƬ��Ϣ
		//@rcSource ȡλͼ�ĸ�����
		//@rcCorner �Ź���ָ�
		//@refMask ������ɫ
		//@dwFade ͸����
		//@bHole �м䲻����
		bool OnDrawImage(const RECT& rcDraw, MUIIMAGEINFO* pImageInfo, const RECT& rcSource,
			const RECT& rcCorner, ARGBREF refMask, DWORD dwFade, BOOL bHole);

		//ȡ���ı��߶�
		int GetTextHeight(HFONT hFont);

		//ȡ���ı��ߴ�
		SIZE GetTextSize(const RECT& rcItem, LPCTSTR strText, int nLenght,
			HFONT hFont, int nRowSpace, DWORD dwTextStyle);

		//ͨ��ptMouseλ��ȡ���ַ����ж�Ӧ��������������ΪptMouse��λ�ÿ�����ƫ�
		//���Է���ʱ�����ptMouse����ȷ������λ��
		int TestTextIndex(const RECT& rcItem, POINT& ptMouse,
			LPCTSTR strText, int nLenght, HFONT hFont, int nRowSpace, DWORD dwTextStyle);

		//�����ı�������nTextIndex��ȡ��TextOut������ֵ��
		//���ص�POINTҲ������Ϊ���λ��
		POINT GetTextPos(const RECT& rcItem, int nTextIndex, LPCTSTR strText,
			HFONT hFont, int nRowSpace, DWORD dwTextStyle);

		//����ı���ptOutput�����ı��������λ��
		bool OnTextOut(const RECT& rcItem, POINT& ptOutput, LPCTSTR strText, int nStrLenght,
			ARGBREF refTextColor, HFONT hFont, int nRowSpace, DWORD dwTextStyle, ARGBREF refTextBkColor);

		//���ù��λ�ã������곬��rcItem�����᲻��ʾ
		bool SetCaret(const RECT& rcItem, const POINT ptCaret, HFONT hFont);

		//����OLE����
		bool OnDrawOleObject(const RECT& rcItem, IViewObject* pViewObject);

		//�Զ���ļ��׸��ı���ʽ
		bool OnDrawHtmlText(const RECT& rcDraw, LPCTSTR strText,
			CMuiIdArray* FontArray, ARGBREF refTextColor);
	protected:
		//strImage�а�����ͼƬ���ƵĲ�����OnDrawImage�����������������ٵ������غ���
		bool DrawImageImp(const RECT& rcDraw, LPCTSTR strImage);

		//�ؼ��ڲ�����
		virtual HCLIP EnterClipImp(const RECT &rcPaint, SIZE &szRound) = 0;
		virtual void  LeaveClipImp(HCLIP hOldClip) = 0;

		//��������Ҫ������ɵĳ�����
		virtual bool DrawFrameImp(const RECT &rcDraw, ARGBREF refColor) = 0;
		virtual bool DrawLineImp(const POINT &ptBegin, const POINT &ptEnd, int nPenSize, ARGBREF refColor) = 0;
		virtual bool DrawColorImp(const RECT &rcDraw, ARGBREF refColor) = 0;
		virtual bool DrawTextImp(const RECT &rcDraw,  LPCTSTR strText, ARGBREF refTextColor, HFONT hFont, DWORD dwType) = 0;
		virtual bool DrawBroderImp(const RECT &rcDraw, ARGBREF refColor, int nBroderSize, SIZE &szRound) = 0;
        //@rcDraw Ҫ���Ƶ�����
        //@pImageInfo ͼƬ��Ϣ
        //@rcSource ȡλͼ�ĸ�����
        //@rcCorner �Ź���ָ�
        //@refMask ������ɫ
        //@dwFade ͸����
        //@bHole �м䲻����
		virtual bool DrawImageImp(const RECT &rcDraw,MUIIMAGEINFO * pImageInfo, const RECT &rcSource,
			const RECT &rcCorner, ARGBREF refMask, DWORD dwFade, BOOL bHole) = 0;

		//�Զ���ļ��׸��ı���ʽ
		virtual bool DrawHtmlTextImp(const RECT& rcDraw, LPCTSTR strText,
			CMuiIdArray* FontArray, ARGBREF refTextColor) = 0;

		//ȡ���ı��߶�
		virtual int GetTextHeightImp(HFONT hFont) = 0;

		//ͨ��ptMouseλ��ȡ���ַ����ж�Ӧ��������������ΪptMouse��λ�ÿ�����ƫ�
		//���Է���ʱ�����ptMouse����ȷ������λ��
		virtual int TestTextIndexImp(const RECT& rcItem, POINT& ptMouse,
			LPCTSTR strText, int nLenght, HFONT hFont, int nRowSpace, DWORD dwTextStyle) = 0;

		//ȡ���ı��ߴ�
		virtual SIZE GetTextSizeImp(const RECT &rcItem, LPCTSTR strText, int nLenght,
			HFONT hFont, int nRowSpace, DWORD dwTextStyle) = 0;

		//����ı���ptOutput�����ı��������λ��
		virtual bool TextOutImp(const RECT& rcItem, POINT& ptOutput, LPCTSTR strText, int nStrLenght,
			ARGBREF refTextColor, HFONT hFont, int nRowSpace, DWORD dwTextStyle, ARGBREF refTextBkColor) = 0;

		//�����ı�������nTextIndex��ȡ��TextOut������ֵ��
		//���ص�POINTҲ������Ϊ���λ��
		virtual POINT GetTextPosImp(const RECT &rcItem, int nTextIndex, LPCTSTR strText,
			HFONT hFont, int nRowSpace, DWORD dwTextStyle) = 0;

		//���ù��λ�ã������곬��rcItem�����᲻��ʾ
		virtual bool SetCaretImp(const RECT &rcItem, const POINT ptCaret, HFONT hFont) = 0;

		//����OLE����
		virtual bool DrawOleObjectImp(const RECT &rcItem, IViewObject * pViewObject) = 0;

	protected:
		HWND m_hWnd;
        TCHAR m_strSkin[MAX_PATH];
		POINT m_ptDrawPos;
	};
}

#endif