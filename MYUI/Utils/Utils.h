
#ifndef _MYUI_UTILS_H__
#define _MYUI_UTILS_H__

#include "..//base.h"

namespace MYUI
{
/*****************************************
//类：CResource  开始
******************************************/
	enum ResourceType
	{
		UnKown = 0,
		ID ,
		File
	};
	class MYUI_API CResource
	{
	public:
		CResource(LPCTSTR lpString);
		CResource(unsigned int nID);
		operator LPCTSTR() const;

		ResourceType GetType();
		void SetType(ResourceType type);
	private:
		
		TCHAR m_strResource[MAX_PATH];
		ResourceType m_type;
		LPCTSTR m_lpstr;
	};

/*****************************************
//类：CTextBuffer  开始
******************************************/
	class CTextBuffer
	{
	public:
		CTextBuffer();
		~CTextBuffer();

		operator LPCTSTR() const;
		//TCHAR operator[] (int nIndex) const;

		void SetPwdText(TCHAR chpwd);
		TCHAR GetPwdText() const;
		void SetPwdStyle(bool b);
		bool IsPwdStyle();

		LPCTSTR GetSrcText() const;
		bool InsterText(int nSelStart, int nLenght, TCHAR c);
		bool InsterText(int nSelStart, int nLenght, LPCTSTR strText);
		bool DeleteText(int nSelStart, int nLenght);
		bool ClearText();
		bool GetLine(int nStart, int &nLenght);
		int GetLenght();
		int GetLineStart(int nIndex);
		bool IsEmpty();
	private:
		int m_lenght;//字符串长度
		int m_size;//缓冲长度
		TCHAR * m_buffer;

		bool m_Ispwd;
		TCHAR m_chpwd;
		mutable TCHAR * m_pwd_buffer;
		int m_pwd_size;
	};

/*****************************************
//类：CMuiColor  开始
******************************************/

	class CMuiString;
	class MYUI_API CMuiColor
	{
	public:
		CMuiColor();
		CMuiColor(ARGBREF argb);
		CMuiColor(BYTE r, BYTE g, BYTE b);
		CMuiColor(BYTE a, BYTE r, BYTE g, BYTE b);
		CMuiColor(LPCTSTR strColor);

		DWORD GetARGB();
		DWORD GetABGR();

		operator ARGBREF() const;
		void operator=(LPCTSTR strColor);
		void operator=(ARGBREF argb);

		CMuiString ToString();
	private:
		ARGBREF m_argb;
	};

/*****************************************
//类：CMuiPoint  开始
******************************************/
	
	class MYUI_API CMuiPoint : public tagPOINT
	{
	public:
		CMuiPoint();
		CMuiPoint(const POINT &src);
		CMuiPoint(long x, long y);
		CMuiPoint(LPARAM lParam);
		CMuiPoint(LPCTSTR pstrValue);
		CMuiString ToString();
	};

/*****************************************
//类：CMuiSize  开始
******************************************/
	class MYUI_API CMuiSize : public tagSIZE
	{
	public:
		CMuiSize();
		CMuiSize(const SIZE& src);
		CMuiSize(const RECT rc);
		CMuiSize(long cx, long cy);
        CMuiSize(LPCTSTR pstrValue);
        CMuiString ToString();
	};

/*****************************************
//类：CMuiRect  开始
******************************************/
	class MYUI_API CMuiRect : public tagRECT
	{
	public:
		CMuiRect();
		CMuiRect(const RECT& src);
		CMuiRect(long iLeft, long iTop, long iRight, long iBottom);
        CMuiRect(LPCTSTR pstrValue);
        CMuiString ToString();

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Intersect(CMuiRect& rc);
		void Union(CMuiRect& rc);
	};

/*****************************************
//类：CMuiString  开始
******************************************/
	class MYUI_API CMuiString
	{
	public:
		enum{MAX_LOCAL_STRING_LEN = 63};

		CMuiString();
		CMuiString(const TCHAR ch);
		CMuiString(const CMuiString &src);
		CMuiString(LPCTSTR lpsz, int nLen = -1);
		~CMuiString();
		CMuiString ToString();

		void Empty();
		int GetLength() const;
		bool IsEmpty() const;
		TCHAR GetAt(int nIndex) const;
		void Append(LPCTSTR pstr);
		void Assign(LPCTSTR pstr, int nLength = -1);
		LPCTSTR GetData() const;

		void SetAt(int nIndex, TCHAR ch);
		operator LPCTSTR() const;

        TCHAR operator[] (int nIndex) const;
        const CMuiString& operator=(const CMuiString& src);
        const CMuiString& operator=(const TCHAR ch);
        const CMuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
        const CMuiString& operator=(LPCSTR lpStr);
        const CMuiString& operator+=(LPCSTR lpStr);
#else
        const CMuiString& operator=(LPCWSTR lpwStr);
        const CMuiString& operator+=(LPCWSTR lpwStr);
#endif
        CMuiString operator+(const CMuiString& src) const;
        CMuiString operator+(LPCTSTR pstr) const;
        const CMuiString& operator+=(const CMuiString& src);
        const CMuiString& operator+=(LPCTSTR pstr);
        const CMuiString& operator+=(const TCHAR ch);

		bool operator == (LPCTSTR str) const;
        bool operator != (LPCTSTR str) const;
        bool operator <= (LPCTSTR str) const;
        bool operator <  (LPCTSTR str) const;
        bool operator >= (LPCTSTR str) const;
        bool operator >  (LPCTSTR str) const;

		int Compare(LPCTSTR pstr) const;
		int CompareNoCase(LPCTSTR pstr) const;

		void MakeUpper();
		void MakeLower();

		CMuiString Left(int nLength) const;
		CMuiString Mid(int iPos, int nLength = -1) const;
		CMuiString Right(int nLength) const;

		int Find(TCHAR ch, int iPos =0) const;
		int Find(LPCTSTR pstr, int iPos =0) const;
		int ReverseFind(TCHAR ch) const;
		int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

		int __cdecl Format(LPCTSTR pstrFormat, ...);
		int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

	protected:
		LPTSTR m_pstr;
		TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];


	};

/*****************************************
//类：CMuiPtrArray  开始
******************************************/
	class MYUI_API CMuiPtrArray
	{
	public:
		CMuiPtrArray(int iPreallocSize = 0);
		CMuiPtrArray(const CMuiPtrArray & src);
		~CMuiPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID pData) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex, int iCount = 1);
		bool Remove(LPVOID pData);
		int GetSize() const;
		LPVOID * GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID * m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};

/*****************************************
//类：CMuiValArray  开始
******************************************/
	class MYUI_API CMuiValArray
	{
	public:
		CMuiValArray(int iElementSize, int iPreallocSize =0);
		~CMuiValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex, int iCount =1);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};

/*****************************************
//类：CMuiStringPtrMap  开始
******************************************/
	struct TITEM;
	class MYUI_API CMuiStringPtrMap
	{
	public:
		CMuiStringPtrMap(int nSize = 83);
		~CMuiStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM ** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

/*****************************************
//类：CWaitCursor  开始
******************************************/
	class MYUI_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

/*****************************************
//类：CMuiIdArray  开始
******************************************/
typedef struct _IDITEM_
{
	int id;
	LPVOID data;
}IDITEM;

	//根据id找出对应的数据，使用for循环算法，适合小型数据查询，仅支持添加，查找功能
	class MYUI_API CMuiIdArray
	{
	public:
		CMuiIdArray(int iPreallocSize = 0);
		~CMuiIdArray();

		bool IsEmpty() const;
		int GetSize() const;

		LPVOID Select(int id) const;
		bool Add(int id, LPVOID pData);


		LPVOID operator[] (int index) const;
		LPVOID GetAt(int index) const;
	protected:
		IDITEM * m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};
/*****************************************
//类：CVariant  开始
******************************************/
	class CVariant : public VARIANT 
	{
	public:
		CVariant()
		{
			VariantInit(this);
		}
		CVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CVariant() 
		{ 
			VariantClear(this); 
		}
	};

}
#endif