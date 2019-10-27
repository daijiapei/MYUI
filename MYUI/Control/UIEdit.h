
#ifndef _MYUI_EDIT_CONTROL_H_
#define _MYUI_EDIT_CONTROL_H_

#include "..\\Core\\UIContainer.h"

namespace MYUI
{
//虽然Edit继承Lable控件，但Lable控件的Style并不适用于Edit

#define MES_SINGLE         0x00000000//单行
#define MES_MULTILINE      0x00000001//多行编辑
#define MES_LINEFREE       0x00000002//自动换行
#define MES_CENTER         0x00000004//水平居中[需要单行才生效]
#define MES_READONLY       0x00000008//只读

#define MES_PASSWORD       0x00000010//密码
#define MES_NUMBER         0x00000020//只允许数字
#define MES_LOWERCASE      0x00000040//自动小写
#define MES_UPPERCASE      0x00000080//自动大写


	//typedef struct _TextBuffer{
	//	int lenght;//字符串长度
	//	int size;//缓冲长度
	//	TCHAR * buffer;
	//}TEXTBUFFER, *PTEXTBUFFER;

	//假设一个文字需要占2个字符，那么将会收到两个WM_CHAR，CTextCheck可以用来判断是否完整地收到了一个文字
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

		bool IsText() const//是否包含了一个完整的文字
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
		int m_nRowSpace;//行距
		ARGBREF m_refSelBkColor;
		ARGBREF m_refSelTextColor;

		POINT m_ptCaret;

		CMuiString m_strHint;//当没有文本时，显示的默认提示
		ARGBREF m_refHintColor;
		HIMC m_hIMC;
		bool m_bShowCaret;
	private:

	};

}

#endif