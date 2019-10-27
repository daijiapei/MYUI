
#ifndef _MYUI_EDIT_CONTROL_H_
#define _MYUI_EDIT_CONTROL_H_

#include "..\\Core\\UIContainer.h"

namespace MYUI
{
//��ȻEdit�̳�Lable�ؼ�����Lable�ؼ���Style����������Edit

#define MES_SINGLE         0x00000000//����
#define MES_MULTILINE      0x00000001//���б༭
#define MES_LINEFREE       0x00000002//�Զ�����
#define MES_CENTER         0x00000004//ˮƽ����[��Ҫ���в���Ч]
#define MES_READONLY       0x00000008//ֻ��

#define MES_PASSWORD       0x00000010//����
#define MES_NUMBER         0x00000020//ֻ��������
#define MES_LOWERCASE      0x00000040//�Զ�Сд
#define MES_UPPERCASE      0x00000080//�Զ���д


	//typedef struct _TextBuffer{
	//	int lenght;//�ַ�������
	//	int size;//���峤��
	//	TCHAR * buffer;
	//}TEXTBUFFER, *PTEXTBUFFER;

	//����һ��������Ҫռ2���ַ�����ô�����յ�����WM_CHAR��CTextCheck���������ж��Ƿ��������յ���һ������
	class CTextCheck
	{
	public:
		CTextCheck()
		{
			Clean();
		}

		operator LPCTSTR () const
		{
			return m_char;
		}

		operator bool () const
		{
			return IsText();
		}

		bool Insert(TCHAR c)
		{
			if(2 > m_index)
			{
				m_char[m_index++] = c;
				return true;
			}
			return false;
		}

		void Clean()
		{
			memset(m_char, 0, sizeof(m_char));
			m_index = 0;
		}

		bool IsEmpty() const
		{
			return 0 == m_index;
		}

		bool IsText() const//�Ƿ������һ������������
		{
			return m_index == check_encode_lenght(m_char);
		}

	private:
		TCHAR m_char[3];
		int m_index;
	};

	class MYUI_API CEditUI : public CContainerUI
	{
	public:
		CEditUI();
		virtual ~CEditUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		virtual CMuiString GetText() const;
		virtual void SetText(LPCTSTR pstrText);

		void SetHint(LPCTSTR strHint);
		LPCTSTR GetHint() const;
		void SetHintColor(ARGBREF refColor);
		ARGBREF GetHintColor() const;
		int GetRowSpace();
		void SetRowSpace(int nRowSpace);
		
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual void SetStyle(DWORD dwStyle);
		void SetSelect(int nSelect, int nLenght = 0);
		/*int GetSelect();
		int GetSelCount();*/
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual SIZE GetContentSize();
		virtual void OnScrollBarMove(LPCVOID pSender, int nShift);

		virtual void PaintText(const RECT& rcItem, const RECT& rcPaint);
		void PaintCaret(const RECT& rcItem, const RECT& rcPaint);


		LRESULT OnKeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT OnChat(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT OnLeftButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT OnLeftButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool SetCaret(POINT * ptMouse);
	protected:
		DWORD m_dwTextStyle;
		
		CTextBuffer m_TextBuffer;
		int m_nSelStart;
		int m_nSelLenght;
		int m_nRowSpace;//�о�
		ARGBREF m_refSelBkColor;
		ARGBREF m_refSelTextColor;

		POINT m_ptCaret;

		CMuiString m_strHint;//��û���ı�ʱ����ʾ��Ĭ����ʾ
		ARGBREF m_refHintColor;
		HIMC m_hIMC;
		bool m_bShowCaret;
	private:

	};

}

#endif