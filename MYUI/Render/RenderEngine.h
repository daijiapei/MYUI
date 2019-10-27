
#ifndef _MYUI_RENDER_ENGINE_H_
#define _MYUI_RENDER_ENGINE_H_

#include "SkinManager.h"

namespace MYUI
{
	//���һ�������࣬ϵͳĬ���ṩ��GDI���棬�����ʹ���Զ�������
	//��̳д��࣬ʵ����Ӧ���麯��
	//�û������עUI����ε������棬ֻ��ʵ������ӿڼ���
	//��ϸ�ɲο�GDI����
	typedef HANDLE  HCLIP;
	typedef HANDLE  HOBJECT;
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

        //����Ƥ��
        bool SetSkinFolder(LPCTSTR strSkin);
        LPCTSTR GetSkinFolder() const;

		//���ڵ���
		virtual bool BeginPaint(const RECT &rcScreen) = 0;
		virtual bool EndPaint(const RECT &rcUpdate) = 0;

		//�ؼ��ڲ�����
		virtual HCLIP EnterClip(const RECT &rcPaint, SIZE &szRound) = 0;
		virtual void  LeaveClip(HCLIP hOldClip) = 0;

		//�����Ի棬������DC��ȡ�������Լ����������
		virtual HDC GetMemDC() = 0;
		virtual void  ReleaseMemDC(HDC hMemDc) = 0;

		//�Զ���ļ��׸��ı���ʽ
		virtual bool OnDrawHtmlText(const RECT &rcDraw, LPCTSTR strText,
			  CMuiIdArray * FontArray, ARGBREF refTextColor) = 0;

		//strImage�а�����ͼƬ���ƵĲ�����OnDrawImage�����������������ٵ������غ���
		virtual bool OnDrawImage(const RECT &rcDraw, LPCTSTR strImage);
		
		//��������Ҫ������ɵĳ�����
		virtual bool OnDrawFrame(const RECT &rcDraw, ARGBREF refColor) = 0;
		virtual bool OnDrawLine(const POINT &ptBegin, const POINT &ptEnd, int nPenSize, ARGBREF refColor) = 0;
		virtual bool OnDrawColor(const RECT &rcDraw, ARGBREF refColor) = 0;
		virtual bool OnDrawText(const RECT &rcDraw,  LPCTSTR strText, ARGBREF refTextColor, HFONT hFont, DWORD dwType) = 0;
		virtual bool OnDrawBroder(const RECT &rcDraw, ARGBREF refColor, int nBroderSize, SIZE &szRound) = 0;


        //@rcDraw Ҫ���Ƶ�����
        //@pImageInfo ͼƬ��Ϣ
        //@rcSource ȡλͼ�ĸ�����
        //@rcCorner �Ź���ָ�
        //@refMask ������ɫ
        //@dwFade ͸����
        //@bHole �м䲻����
		virtual bool OnDrawImage(const RECT &rcDraw,IMAGEINFO * pImageInfo, const RECT &rcSource,
			const RECT &rcCorner, ARGBREF refMask, DWORD dwFade, bool bHole) = 0;

		//ȡ���ı��߶�
		virtual int GetTextHeight(HFONT hFont) = 0;

		//ȡ���ı��ߴ�
		virtual SIZE GetTextSize(const RECT &rcItem, LPCTSTR strText, int nLenght, 
			HFONT hFont, int nRowSpace, DWORD dwTextStyle) = 0;

		//ͨ��ptMouseλ��ȡ���ַ����ж�Ӧ��������������ΪptMouse��λ�ÿ�����ƫ�
		//���Է���ʱ�����ptMouse����ȷ������λ��
		virtual int TestTextIndex(const RECT &rcItem, POINT &ptMouse, 
			LPCTSTR strText, int nLenght, HFONT hFont, int nRowSpace, DWORD dwTextStyle) = 0;

		//�����ı�������nTextIndex��ȡ��TextOut������ֵ��
		//���ص�POINTҲ������Ϊ���λ��
		virtual POINT GetTextPos(const RECT &rcItem, int nTextIndex, LPCTSTR strText, 
			HFONT hFont, int nRowSpace, DWORD dwTextStyle) = 0;

		//����ı���ptOutput�����ı��������λ��
		virtual bool OnTextOut(const RECT &rcItem, POINT &ptOutput, LPCTSTR strText, int nStrLenght, 
			ARGBREF refTextColor, HFONT hFont, int nRowSpace, DWORD dwTextStyle, ARGBREF refTextBkColor) = 0;

		//���ù��λ�ã������곬��rcItem�����᲻��ʾ
		virtual bool SetCaret(const RECT &rcItem, const POINT ptCaret, HFONT hFont) = 0;

		//����OLE����
		virtual bool DrawOleObject(const RECT &rcItem, IViewObject * pViewObject) = 0;

	protected:
		HWND m_hWnd;
        TCHAR m_strSkin[MAX_PATH];
	};
}

#endif