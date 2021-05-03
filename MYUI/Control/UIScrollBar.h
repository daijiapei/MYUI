
#ifndef _MYUI_SCROLLBAR_CONTROL_H_
#define _MYUI_SCROLLBAR_CONTROL_H_

#include "..\\Core\\UIControl.h"

namespace MYUI
{
	enum ScrollBarStyle
	{
		NonStyle = 0, //���κη�񣬽������κδ�����������ʾ������
		LeftVerticalStyle = 0x01, //��ֱ������
		RightVerticalStyle = 0x02, //�Ҵ�ֱ������
		AutoVerticalStyle = RightVerticalStyle, //Ĭ�ϴ�ֱ������
		TopHorizontalStyle = 0x04, //��ˮƽ������
		BottomHorizontalStyle = 0x08, //��ˮƽ������
		AutoHorizontalStyle = BottomHorizontalStyle, //Ĭ��ˮƽ������

		//���������ж��Ƿ�ӵ�й��������
		BarVertical = 0x03,
		BarHorizontal = 0x0C,
		BarHas = 0x0F 
	};

	enum ScrollShiftStyle
	{
		SameDirection = 1,
		ContraDirection = -1,
		Auto = ContraDirection
	};

	

	class MYUI_API CScrollBarUI : public CControlUI
	{
	public:
		//һ����������ǰ����ť���󵼰�ť����ק��ť��������ĸ��������
		//ǰ����ť���󵼰�ť���ϳ��򵼰�ť
		//ע���ڴ�ֱ�������У��򵼰�ť�Ŀ��ɹ������Ŀ����
		//ע����ˮƽ�������У��򵼰�ť�ĸ��ɹ������ĸ߾���
		CScrollBarUI(ScrollBarStyle style);
		virtual ~CScrollBarUI();
		
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual void SetFilterFloatType(EnumFloatType type);
		ScrollBarStyle GetStyle() const;
		void SetActive(bool bActive);
		bool IsActive();
		bool IsShowing();//�Ƿ�������ʾ

		void SetShiftStyle(ScrollShiftStyle style);
		ScrollShiftStyle GetShiftStyle();
		void SetDragBarMaxShift(int nShift);
		int GetDragBarMaxShift();//�������������ƶ��ľ���,һ���ǡ��ؼ���С-�ͻ�����
		void SetDragBarShift(int nShift);//��ק��ť��λ��
		int GetDragBarShift();

		void SetBarIntersect(RECT &rect);
		//�򵼰�ť��������ǰ�����������ť������ʹ����ͬ�Ĳ���
		//ע�⣬����������Ǵ�ֱ���������򵼰�ť�Ŀ��Ǹ��ݴ�ֱ�������Ŀ��趨��
		//�����ڴ�ֱ�������У��򵼰�ť�Ŀ���Ч
		//ע�⣬�����������ˮƽ���������򵼰�ť�ĸߣ��Ǹ���ˮƽ�������ĸ��趨��
		//������ˮƽ�������У��򵼰�ť�ĸ���Ч
		bool CalcDragBarRect();
		virtual bool SetItem(RECT &rcItem, bool bMustUpdate);
		virtual bool OnPaint(const RECT& rcUpdate);

		//ͼƬ
		void SetFrontNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontNormalImage();
		void SetFrontHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontHotImage();
		void SetFrontPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontPushedImage();
		void SetFrontDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetFrontDisabledImage();

		void SetFinalNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetFinalNormalImage();
		void SetFinalHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFinalHotImage();
		void SetFinalPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFinalPushedImage();
		void SetFinalDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetFinalDisabledImage();

		void SetTrackNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetTrackNormalImage();
		void SetTrackHotImage(LPCTSTR pStrImage);
		LPCTSTR GetTrackHotImage();
		void SetTrackPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetTrackPushedImage();
		void SetTrackDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetTrackDisabledImage();

		void SetDragBtnNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetDragBtnNormalImage();
		void SetDragBtnHotImage(LPCTSTR pStrImage);
		LPCTSTR GetDragBtnHotImage();
		void SetDragBtnPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetDragBtnPushedImage();
		void SetDragBtnDisabledImage(LPCTSTR pStrImage);
		LPCTSTR GetDragBtnDisabledImage();

	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
		virtual void SetState(DWORD dwState);
		int GetDragBarOffset();

		DWORD CalcPointIn(const POINT& pt);

		void PaintTrackButton(const RECT& rcUpdate);//������ק��ť�Ĺ��
		void PaintFrontButton(const RECT& rcUpdate);//��ʼ�İ�ť
		void PaintFinalButton(const RECT& rcUpdate);//ĩβ�İ�ť
		void PaintDragButton(const RECT& rcUpdate);//��ק��ť
		void PaintIntersect(const RECT& rcUpdate);//��������

	private:
		POINT m_ptOldMousePos;

#define SBPI_DRAGBTN   0x0001
#define SBPI_FRONTBTN  0x0002
#define SBPI_FINALBTN  0x0004
#define SBPI_TRACKBTN  0x0008
#define SBPI_INTERSECT 0x0010
		DWORD m_dwPointIn;//�����Ǹ�����
		ScrollBarStyle m_dwStyle;
		ScrollShiftStyle m_dwShiftStyle;
		bool m_bActive;//�Ƿ�һֱ��ʾ
		int m_nMaxShift;//���ƶ��ģ����λ��
		int m_nNowShift;//��ǰλ��

		RECT m_rcBarIntersect;//��ֱ��������ˮƽ�������Ľ����
		RECT m_rcFrontButton;//��ʼ�İ�ť
		RECT m_rcFinalButton;//ĩβ�İ�ť
		RECT m_rcDragButton;//��ק��ť
		RECT m_rcTrackButton;//������ק��ť�Ĺ��

		//һ������£��������ǲ���Ҫ��ý����
		CMuiString m_strFrontNormalImage;//��ͨ״̬
		CMuiString m_strFrontHotImage;//��꾭���ؼ�״̬
		CMuiString m_strFrontPushedImage;//æµ״̬���������ڵ���ؼ�
		CMuiString m_strFrontDisabledImage;//����״̬

		CMuiString m_strFinalNormalImage;//��ͨ״̬
		CMuiString m_strFinalHotImage;//��꾭���ؼ�״̬
		CMuiString m_strFinalPushedImage;//æµ״̬���������ڵ���ؼ�
		CMuiString m_strFinalDisabledImage;//����״̬

		CMuiString m_strTrackNormalImage;//��ͨ״̬
		CMuiString m_strTrackHotImage;//��꾭���ؼ�״̬
		CMuiString m_strTrackPushedImage;//æµ״̬���������ڵ���ؼ�
		CMuiString m_strTrackDisabledImage;//����״̬

		CMuiString m_strDragBtnNormalImage;//��ͨ״̬
		CMuiString m_strDragBtnHotImage;//��꾭���ؼ�״̬
		CMuiString m_strDragBtnPushedImage;//æµ״̬���������ڵ���ؼ�
		CMuiString m_strDragBtnDisabledImage;//����״̬
	};

	//����������һ��Ĺ�����������ʹ��
	class MYUI_API CHorizontailScrollBarUI : public CScrollBarUI
	{
	public:
		CHorizontailScrollBarUI()
			: CScrollBarUI(AutoHorizontalStyle)
		{  0;  }

		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const
		{
			return CHorizontailScrollBarUI::g_strClassName;
		}
	};
	
	class MYUI_API CVerticalScrollBarUI : public CScrollBarUI
	{
	public:
		CVerticalScrollBarUI()
			: CScrollBarUI(AutoVerticalStyle)
		{  0;  }

		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const
		{
			return CVerticalScrollBarUI::g_strClassName;
		}
	};
	
	class IScrollBarMove
	{
		//pSender = Ŀ�������
	public:
		virtual void OnScrollBarMove(CScrollBarUI* pSender, int nShift) = 0;
		virtual const SIZE& GetScrollBarShift() const = 0;
	};
}

#endif