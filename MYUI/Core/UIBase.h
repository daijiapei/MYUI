
#ifndef __MYUI_UI_BASE_ATTRIBUTE_H__
#define __MYUI_UI_BASE_ATTRIBUTE_H__

#include "..//Utils//Utils.h"

namespace MYUI
{
	//���ⴰ�ڵĽ���
	class MYUI_API CItemViewInfo
	{
	public:
		CItemViewInfo();
		virtual ~CItemViewInfo();

	public:
		virtual void SetStyle(DWORD dwStyle);
		virtual DWORD GetStyle();
		//����ͼ��
		virtual void SetBkColor(ARGBREF argb);
		virtual ARGBREF GetBkColor();

		virtual void SetTextFont(int FontId);
		virtual int GetTextFont();

		virtual void SetTextColor(ARGBREF argb);
		virtual ARGBREF GetTextColor();

		virtual void SetText(LPCTSTR pstrText);
		virtual CMuiString GetText() const;

		virtual void SetBorderSize(int nSize);
		virtual int GetBorderSize();

		virtual void SetBorderColor(ARGBREF argb);
		virtual ARGBREF GetBorderColor();

		virtual void SetBorderPen(int nSize, ARGBREF argb);

		virtual void SetBorderRound(SIZE &size);
		virtual SIZE &GetBorderRound();

		virtual void SetBkImage(LPCTSTR strImage);
		virtual LPCTSTR GetBkImage();
		virtual void SetDisabledImage(LPCTSTR strImage);
		virtual LPCTSTR GetDisabledImage();

	protected:
		//����֪ͨ
		virtual void OnViewChange(LPVOID pElement) = 0;
		//virtual void OnFontChange(LPVOID pElement) = 0;
	protected:

//MCS_CONTROLUI��Ҫ���ڲ�����
//#define MCS_CONTROLUI 0x80000000//����һ���ؼ�
		DWORD m_dwStyle;
		DWORD m_dwStyleEx;
		ARGBREF m_refTextColor;
		int m_nFontId;
		HFONT m_hFont;
		CMuiString m_strText;

		ARGBREF m_refBkColor;

		ARGBREF m_refBorderColor;
		SIZE   m_szBorderRound;
		int m_nBorderSize;

		//ͼƬ��·��+ͼƬ����
		CMuiString m_strBkImage;//����
		CMuiString m_strDisabledImage;//����״̬
	};

	enum EnumFloatType//�жϿؼ������У��ĸ�ֵ����Ч��
	{
		FloatNon = 0x00, //��Ч
		FloatPointX = 0x01,
		FloatPointY = 0x02,
		//ע�����е�type�ж��У�������Ϊxy����һ��ʹ�õģ�xy��Ҫ����ʹ��
		FloatPoint = 0x03 , //x,y��Ч,
		FloatSizeCx = 0x04,
		FloatSizeCy = 0x08,
		FloatSize = 0x0C, //cx,cy��Ч
		FloatAll = 0x0F //����Ч
	};

	class MYUI_API CItemPosition//���ⴰ�ڵ�λ��
	{
	public:
		//�޸�CItemPosition��ֵ�����ᵼ��ҳ�����
		//���Լǵõ��ø����update����
		CItemPosition();
		virtual ~CItemPosition();
		
	public:
		
		//δƫ������
		virtual const RECT &GetRawRect() const;
		virtual const RECT &GetClientRect() const;

		//��ƫ������
		virtual RECT GetItemClient();
		//virtual const RECT &GetItemRaw() const;

		//return true = �������˱���� false = ����û�з������
		//bMustUpdate true = ���������Ƿ���ͬ����һ��Ҫ��������������
		//bMustUpdate false = ���������ͬ���򲻻ᷢ���������
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual const RECT &GetItem() const;
		virtual SIZE GetValidSize();//ȡ��������Ч�ĳߴ�
		//�ڲ�����
		virtual EnumFloatType GetFloatType();
		virtual void SetFloatType(EnumFloatType type);
		virtual void AddFloatType(EnumFloatType type);
		virtual void RemoveFloatType(EnumFloatType type);
		
		//rect

		//SetPos��SetItem������Ƚ����׻���������˵��һ��
		//SetPos��ֵ���ǿؼ�����λ��ʱ�Ĳο�ֵ��SetItem�������ÿؼ�������λ��
		//��SetItemʱ�����ֻ���ȡGetPos��ֵ���ο�����
		//������SetPos��ֵ x=10,y=10,width=100,height=100, �����ڴ�ֱ�����У�
		//x,y��ֵ����Ч�ģ����Դ�ֱ������SetItemʱ��ֻ����ȡwidth,height������ֵ���м��㣬������x,yֵ
		//����SetWight,SetPoint�Ⱥ������ʸ�SetPos��һ�µģ�
		void SetPos(RECT &rect);
		RECT GetPos();

		//size
		const SIZE & GetSize() const;
		void SetSize(SIZE &size);
		int GetHeight();
		void SetHeight(int nHeight);
		int GetWidth();
		void SetWidth(int nWidth);

		//point
		void GetPoint(POINT &pt);
		void SetPoint(POINT &pt);
		int GetPointX();
		void SetPointX(int x);
		int GetPointY();
		void SetPointY(int y);

		//���ڴ�С
		int GetMinCx();
		void SetMinCx(int cx);
		int GetMinCy();
		void SetMinCy(int cy);
		int GetMaxCx();
		void SetMaxCx(int cx);
		int GetMaxCy();
		void SetMaxCy(int cy);
		void GetMinSize(SIZE &size);
		void SetMinSize(SIZE &size);
		void GetMaxSize(SIZE &size);
		void SetMaxSize(SIZE &size);
		

		void SetMargin(RECT &rcMargin);
		const RECT &GetMargin() const;

	protected:
		virtual void OnPosChange(LPVOID pElement) = 0;

	protected:

		RECT m_rcItem;//�ؼ����λ�ã�����ڸ��ؼ�
		//��߾������Ϊ����m_rcItem�������γ�m_rcRawItem��
		//Ҳ������Ϊ����m_rcRawItem��չ���γ�m_rcItem
		RECT m_rcMargin;//��߾�
		//��������򣬶��������m_rcItem ��
		RECT m_rcRawItem;//ԭʼItem��m_rcItem��ȥ��߾�
		RECT m_rcClient;//�ͻ�����m_rcRawItem��ȥ������
		
		POINT m_ItemPoint;//xy
		SIZE m_ItemSize;//�����
		SIZE m_SizeMin;
		SIZE m_SizeMax;
		//m_ItemPoint �� m_ItemSize���ĸ�ֵ��Ч
		EnumFloatType m_FloatType;
	};

}

#endif