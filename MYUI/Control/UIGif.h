
#ifndef _MYUI_GIF_CONTROL_H_
#define _MYUI_GIF_CONTROL_H_

#include "../Core/UIControl.h"
#include<gdiplus.h>
using namespace Gdiplus;

namespace MYUI
{
//��ʾ��λ��
#define MGIFS_LEFT      0x0001
#define MGIFS_RIGHT     0x0002
#define MGIFS_CENTER    0x0004
#define MGIFS_VCENTER   0x0008

#define MGIFS_FLAG      0x000F//�����ڲ��жϵģ���Ҫ�����������

	class MYUI_API CGifUI : public CControlUI
	{
	public:
		CGifUI();
		~CGifUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual void SetBkImage(LPCTSTR strImage);
		virtual void SetVisible(bool bVisible);
		virtual bool OnPaint(RECT rcItem, RECT rcPaint, RECT rcUpdate);
		

		void	PlayGif();//����gifͼƬ
		void	PauseGif();//��ͣ������
		void	StopGif();//��ͣ����
		void	DeleteGif();//����gifͼƬ
		void	SetAutoPlay(bool bAuto = true );
		bool	IsAutoPlay() const;
		void	SetAutoSize(bool bAuto = true );
		bool	IsAutoSize() const;

	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void OnAttach(HWND hNewWnd);//���ؼ����ӵ��µĴ���ʱ�������һ�θú���
		virtual void OnDetach(HWND hOldWnd);//���ؼ��뿪һ������ʱ�������һ�θú���
		
	private:
		Gdiplus::Image*	CGifUI::LoadGif(LPCTSTR strGifPath);

	private:
		Gdiplus::Image	*m_pGifImage;				// gifͼƬ�洢
		UINT			m_nFrameCount;				// gifͼƬ��֡��
		UINT			m_nFramePosition;			// ��ǰ�ŵ��ڼ�֡
		Gdiplus::PropertyItem*	m_pPropertyItem;	// ֡��֮֡����ʱ��

		bool			m_bIsAutoPlay;				// �Ƿ��Զ�����gif
		bool			m_bIsAutoSize;				// �Ƿ��Զ�����ͼƬ���ô�С
		bool			m_bIsPlaying;				// ���ͼƬ�Ƿ��ڷ�ӳ
		ULONG_PTR		m_gdiplusToken;				// ��ʼ��gdi+�������ݴ���
		GdiplusStartupInput		m_gdiplusStartupInput;//gdi+��������Ϣ

	};
};

#endif