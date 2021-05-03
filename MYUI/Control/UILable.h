
#ifndef _MYUI_LABLE_CONTROL_H_
#define _MYUI_LABLE_CONTROL_H_

#include "..\\Core\\UIControl.h"

namespace MYUI
{
//Lable�ؼ���style������̳���Lable�Ŀؼ�û���ر�˵��
//��Lable���������ɿ���ʹ����Щ����

#define MLBES_LEFT     0x00000001 //�ı�����
#define MLBES_RIGHT    0x00000002 //�ı�����
#define MLBES_CENTER   0X00000004 //ˮƽ����
#define MLBES_VCENTER  0x00000008 //��ֱ����


	class MYUI_API CLableUI : public CControlUI
	{
	public:
		CLableUI();
		virtual ~CLableUI();
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		
		void SetTextPadding(const RECT &rect);
		const RECT &GetTextPadding() const;

		//��������
		void SetNormalColor(ARGBREF argb);
		ARGBREF GetNormalColor();
		void SetHotColor(ARGBREF argb);
		ARGBREF GetHotColor();
		void SetSelectColor(ARGBREF argb);
		ARGBREF GetSelectColor();
		void SetPushedColor(ARGBREF argb);
		ARGBREF GetPushedColor();
		void SetFocusedColor(ARGBREF argb);
		ARGBREF GetFocusedColor();

		void SetNormalImage(LPCTSTR strImage);
		LPCTSTR GetNormalImage() const;
		void SetHotImage(LPCTSTR strImage);
		LPCTSTR GetHotImage() const;
		void SetSelectImage(LPCTSTR strImage);
		LPCTSTR GetSelectImage() const;
		void SetPushedImage(LPCTSTR strImage);
		LPCTSTR GetPushedImage() const;
		void SetFocusedImage(LPCTSTR strImage);
		LPCTSTR GetFocusedImage() const;

		void SetHotTextColor(ARGBREF argb);
		ARGBREF GetHotTextColor();
		void SetSelectTextColor(ARGBREF argb);
		ARGBREF GetSelectTextColor();
		void SetPushedTextColor(ARGBREF argb);
		ARGBREF GetPushedTextColor();
		void SetFocusedTextColor(ARGBREF argb);
		ARGBREF GetFocusedTextColor();

		int GetHotBorderSize();
		void SetHotBorderSize(int size);
		ARGBREF GetHotBorderColor() const;
		void SetHotBorderColor(ARGBREF refColor);

        int GetFocusedBorderSize();
		void SetFocusedBorderSize(int size);
		ARGBREF GetFocusedBorderColor() const;
		void SetFocusedBorderColor(ARGBREF refColor);

	protected:
		virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam) override;
		virtual void PaintStatusImage( const RECT& rcUpdate) override;
		virtual void PaintText(const RECT& rcUpdate) override;
		virtual void PaintBorder(const RECT& rcUpdate) override;
		virtual void SetState(DWORD dwState);
	protected:
		
		//ͼƬ��·�� + ͼƬ����
		CMuiString m_strNormalImage;//��ͨ״̬
		CMuiString m_strHotImage;//��꾭���ؼ�״̬
		CMuiString m_strSelectImage;//ѡ��״̬��
		CMuiString m_strPushedImage;//æµ״̬���������ڵ���ؼ�
		CMuiString m_strFocusedImage;//����״̬

		ARGBREF m_refNormalColor;
		ARGBREF m_refHotColor;
		ARGBREF m_refSelectColor;
		ARGBREF m_refPushedColor;
		ARGBREF m_refFocusedColor;

		ARGBREF m_refHotTextColor;
		ARGBREF m_refSelectTextColor;
		ARGBREF m_refPushedTextColor;
		ARGBREF m_refFocusedTextColor;

		RECT m_rcTextPadding;//����������

		int m_nHotBorderSize;
		ARGBREF m_refHotBorderColor;

        int m_nFocusedBorderSize;
        ARGBREF m_refFocusedBorderColor;
	private:

	};

}

#endif