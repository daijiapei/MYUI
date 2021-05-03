
#include "Utils.h"

namespace MYUI
{

/*****************************************
//类：CResource  开始
******************************************/
	CResource::CResource(LPCTSTR lpString) : m_lpstr(lpString)
	{
		m_type = ResourceType::UnKown;
	}

	CResource::CResource(unsigned int nID) : m_lpstr(MAKEINTRESOURCE(nID))
	{
		m_type = ResourceType::UnKown;
	}

	CResource::operator LPCTSTR() const
	{
		return m_lpstr;
	}

	ResourceType CResource::GetType()
	{
		return m_type;
	}

	void CResource::SetType(ResourceType type)
	{
		m_type = type;
	}

/*****************************************
//类：CTextBuffer  开始
******************************************/
	CTextBuffer::CTextBuffer()
		: m_buffer(NULL)
		, m_lenght(0)
		, m_size(0)
		, m_pwd_buffer(NULL)
		, m_pwd_size(0)
		, m_chpwd(_T('*'))
		, m_Ispwd(false)
	{
		m_buffer = new TCHAR[512];
		m_size = 512 * sizeof(TCHAR);
	}

	CTextBuffer::~CTextBuffer()
	{
		if(m_buffer) delete[] m_buffer;
		if(m_pwd_buffer) delete[] m_pwd_buffer;
	}

	CTextBuffer::operator LPCTSTR() const
	{
		if(false == m_Ispwd) return m_buffer;

		//这里就是属于password了
		//m_pwd_buffer 的定义： TCHAR * m_pwd_buffer;
		if(m_size > m_pwd_size)
		{
			if(m_pwd_buffer) delete m_pwd_buffer;

			m_pwd_buffer = (TCHAR *)new char[m_size];

            m_pwd_buffer[0] = _T('\0');
		}

        if(NULL == m_pwd_buffer[0])
        {
            for(int i = 0; m_lenght > i ; i++)
		    {
			    m_pwd_buffer[i] = m_chpwd;
		    }

		    m_pwd_buffer[m_lenght] = _T('\0');
        }

		return m_pwd_buffer;
	}

	void CTextBuffer::SetPwdText(TCHAR chpwd)
	{
		m_chpwd = chpwd;
	}

	TCHAR CTextBuffer::GetPwdText() const
	{
		return m_chpwd;
	}

	void CTextBuffer::SetPwdStyle(bool b)
	{
		m_Ispwd = b;
	}

	bool CTextBuffer::IsPwdStyle()
	{
		return m_Ispwd;
	}

	LPCTSTR CTextBuffer::GetSrcText() const
	{
		return m_buffer;
	}

	bool CTextBuffer::InsterText(int nSelStart, int nLenght, TCHAR c)
	{
		TCHAR str[2];
		str[0] = c;
		str[1] = _T('\0');
		return InsterText(nSelStart, nLenght, str);
	}

	bool CTextBuffer::InsterText(int nSelStart, int nLenght, LPCTSTR strText)
	{
		MUIASSERT(m_buffer);
		MUIASSERT(m_size > m_lenght * sizeof(TCHAR));
		MUIASSERT(strText);

		if(NULL == strText) return false;

		if(0 != nLenght)
		{
			//选择了内容，strText替换选择的内容，
			//替换的方法是先删除，再插入
			DeleteText(nSelStart, nLenght);
		}

		int nTextLenght = _tcslen(strText);
		if(0 == nTextLenght) return true;

		int nNeedLen = m_lenght + nTextLenght + sizeof(TCHAR);//TCHAR = _T('\0');

		if(nNeedLen * sizeof(TCHAR) > m_size)
		{
			//长度不足，扩容
			int nDiff = ((nNeedLen * sizeof(TCHAR) - m_size) / m_size) + 2;
			TCHAR * buffer = new TCHAR[m_size * nDiff];
			_tcscpy(buffer, m_buffer);
			delete[] m_buffer;
			m_buffer = buffer;
		}

		if(nSelStart != m_lenght)//在文字中间插入
		{
			//在文字中间腾出空间
			memmove(&m_buffer[nSelStart + nTextLenght], &m_buffer[nSelStart],
				(m_lenght - nSelStart) * sizeof(TCHAR));
		}
		
		//插入文字
		memcpy(&m_buffer[nSelStart], strText, nTextLenght *sizeof(TCHAR));
		m_lenght += nTextLenght;
		m_buffer[m_lenght] = _T('\0');

		MUITRACE(_T("CTextBuffer::m_buffer = %s"), m_buffer);
		return true;
	}

	bool CTextBuffer::DeleteText(int nSelStart, int nLenght)
	{
		MUIASSERT(m_buffer);
		MUIASSERT(m_size > m_lenght * sizeof(TCHAR));

		MUIASSERT(m_lenght >= nSelStart);

		if(0 == nLenght)//无选择
		{
			//nLenght = comeback_check_encode_lenght(&m_buffer[nSelStart - 1]);
			MUITRACE(_T("nLenght = %d"), nLenght);
			nSelStart -= nLenght;
		}
		else if(0 > nLenght) //逆向选择
		{
			nSelStart += nLenght;
			nLenght = -nLenght;
		}
		else if(nLenght > 0)
		{
		}

		MUIASSERT(nSelStart >= 0);
		if(0 > nSelStart) return false;

		memmove(&m_buffer[nSelStart], &m_buffer[nSelStart + nLenght], 
			(m_lenght - (nSelStart + nLenght)) * sizeof(TCHAR));
		m_lenght -= nLenght;
		m_buffer[m_lenght] = _T('\0');
		return true;
	}

	bool CTextBuffer::ClearText()
	{
		MUIASSERT(m_buffer);
		MUIASSERT(m_size > m_lenght * sizeof(TCHAR));
		m_buffer[0] = _T('\0');
		m_lenght = 0;
		return true;
	}

	//返回false表示字符串已结束
	bool CTextBuffer::GetLine(int nStart, int &nLenght)
	{
		//结束符\0也计算在内
		nLenght = 0;
		int i = 0;
		for(i = nStart; m_lenght >= i; i++)
		{
			++nLenght;
			switch(m_buffer[i])
			{
			case _T('\r'):
				{
					if(_T('\n') == m_buffer[i + 1])
					{
						++nLenght;
					}
					return true;
				}break;
			case _T('\n'):
				{
					if(_T('\r') == m_buffer[i + 1])
					{
						++nLenght;
					}
					return true;
				}break;
			case _T('\0'):
				{
					++nLenght;
					return false;
				}break;
			default:
				{
				}break;
			}
		}
		return false;
	}

	bool CTextBuffer::IsEmpty()
	{
		return 0 == m_lenght;
	}

	int CTextBuffer::GetLenght()
	{
		return m_lenght;
	}

	int CTextBuffer::GetLineStart(int nIndex)
	{
		int nStart = 0;

		MUIASSERT(nIndex >= 0 && m_lenght >= nIndex);
		if(0 == nIndex) return 0;
		if(nIndex > m_lenght) return m_lenght;
		
		nStart = nIndex;
		if(_T('\r') == m_buffer[nStart])
		{
			--nStart;
			if(nStart > 0 && _T('\n') == m_buffer[nStart])
			{
				--nStart;
			}
		}
		else if(_T('\n') == m_buffer[nStart])
		{
			--nStart;
			if(nStart > 0 && _T('\r') == m_buffer[nStart])
			{
				--nStart;
			}
		}

		while(nStart)
		{
			if(_T('\n') == m_buffer[nStart] || _T('\r') == m_buffer[nStart])
			{
				++nStart;
				break;
			}
			--nStart;
		}

		return nStart;
	}

/*****************************************
//类：CMuiColor  开始
******************************************/
	CMuiColor::CMuiColor()
	{
		m_argb = NULL;
	}

	CMuiColor::CMuiColor(ARGBREF argb)
	{
		m_argb = argb;
	}

	CMuiColor::CMuiColor(BYTE r, BYTE g, BYTE b)
	{
		m_argb = ARGB(255,r,g,b);
	}

	CMuiColor::CMuiColor(BYTE a, BYTE r, BYTE g, BYTE b)
	{
		m_argb = ARGB(a,r,g,b);
	}

	CMuiColor::CMuiColor(LPCTSTR strColor)
	{
		m_argb = GetArgbFromString(strColor);
	}

	DWORD CMuiColor::GetARGB()
	{
		return m_argb;
	}

	DWORD CMuiColor::GetABGR()
	{
		return ARGB(GetAValue(m_argb),GetBValue(m_argb),GetGValue(m_argb),GetRValue(m_argb));
	}

	CMuiColor::operator ARGBREF() const
	{
		return m_argb;
	}

	void CMuiColor::operator=(LPCTSTR strColor)
	{
		CMuiColor::CMuiColor(strColor);
	}

	void CMuiColor::operator=(ARGBREF argb)
	{
		CMuiColor::CMuiColor(argb);
	}

	CMuiString CMuiColor::ToString()
    {
        CMuiString sColor;
		sColor.SmallFormat(_T("ARGB(%d,%d,%d,%d)"), GetAValue(m_argb), GetRValue(m_argb),
			GetGValue(m_argb),GetBValue(m_argb));
        return sColor;
    }
/*****************************************
//类：CMuiPoint  开始
******************************************/
	CMuiPoint::CMuiPoint()
	{
		x = y = 0;
	}

	CMuiPoint::CMuiPoint(const POINT & src)
	{
		x = src.x;
		y = src.y;
	}

	CMuiPoint::CMuiPoint(long _x, long _y)
	{
		x = _x;
		y = _y;
	}

	CMuiPoint::CMuiPoint(LPARAM lParam)
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
	}

    CMuiPoint::CMuiPoint(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) x = y = 0;
        LPTSTR pstr = NULL;
        x = y = _tcstol(pstrValue, &pstr, 10); MUIASSERT(pstr);
        y = _tcstol(pstr + 1, &pstr, 10);      MUIASSERT(pstr);
    }

    CMuiString CMuiPoint::ToString()
    {
        CMuiString sPoint;
        sPoint.SmallFormat(_T("%ld,%ld"), x, y);
        return sPoint;
    }

/*****************************************
//类：CDuiSize  开始
******************************************/
	CMuiSize::CMuiSize()
	{
		cx = cy = 0;
	}

	CMuiSize::CMuiSize(const SIZE& src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	CMuiSize::CMuiSize(const RECT rc)
	{
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
	}

	CMuiSize::CMuiSize(long _cx, long _cy)
	{
		cx = _cx;
		cy = _cy;
	}

    CMuiSize::CMuiSize(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) cx = cy = 0;
        LPTSTR pstr = NULL;
        cx = cy = _tcstol(pstrValue, &pstr, 10); MUIASSERT(pstr);
        cy = _tcstol(pstr + 1, &pstr, 10);       MUIASSERT(pstr);
    }

    CMuiString CMuiSize::ToString()
    {
        CMuiString sSize;
        sSize.SmallFormat(_T("%ld,%ld"), cx, cy);
        return sSize;
    }

/*****************************************
//类：CDuiRect  开始
******************************************/
	CMuiRect::CMuiRect()
	{
		left = top = right = bottom = 0;
	}

	CMuiRect::CMuiRect(const RECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
	}

	CMuiRect::CMuiRect(long iLeft, long iTop, long iRight, long iBottom)
	{
		left = iLeft;
		top = iTop;
		right = iRight;
		bottom = iBottom;
	}

    CMuiRect::CMuiRect(LPCTSTR pstrValue)
    {
        if (pstrValue == NULL || *pstrValue == _T('\0')) left = top = right = bottom = 0;
        LPTSTR pstr = NULL;
        left = top = right = bottom = _tcstol(pstrValue, &pstr, 10); MUIASSERT(pstr);
        top = bottom = _tcstol(pstr + 1, &pstr, 10);                 MUIASSERT(pstr);
        right = _tcstol(pstr + 1, &pstr, 10);                        MUIASSERT(pstr);
        bottom = _tcstol(pstr + 1, &pstr, 10);                       MUIASSERT(pstr);
    }

    CMuiString CMuiRect::ToString()
    {
        CMuiString sRect;
        sRect.SmallFormat(_T("%ld,%ld,%ld,%ld"), left, top, right, bottom);
        return sRect;
    }

	int CMuiRect::GetWidth() const
	{
		return right - left;
	}

	int CMuiRect::GetHeight() const
	{
		return bottom - top;
	}

	void CMuiRect::Empty()
	{
		left = top = right = bottom = 0;
	}

	bool CMuiRect::IsNull() const
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0); 
	}

	//扩容
	void CMuiRect::Join(const RECT& rc)
	{
		if( rc.left < left ) left = rc.left;
		if( rc.top < top ) top = rc.top;
		if( rc.right > right ) right = rc.right;
		if( rc.bottom > bottom ) bottom = rc.bottom;
	}

	//将x,y置零
	void CMuiRect::ResetOffset()
	{
		::OffsetRect(this, -left, -top);
	}

	//x,y的位置可能翻转，将其调整为正确的值
	void CMuiRect::Normalize()
	{
		if( left > right ) { int iTemp = left; left = right; right = iTemp; }
		if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
	}

	void CMuiRect::Offset(int cx, int cy)
	{
		//移动cx\cy个点
		::OffsetRect(this, cx, cy);
	}

	void CMuiRect::Inflate(int cx, int cy)
	{
		//增大矩形
		::InflateRect(this, cx, cy);
	}

	void CMuiRect::Deflate(int cx, int cy)
	{
		//缩小矩形
		::InflateRect(this, -cx, -cy);
	}

	//求相交
	void CMuiRect::Intersect(CMuiRect &rc)
	{
		::IntersectRect(this, this, &rc);
	}

	void CMuiRect::Union(CMuiRect& rc)
	{
		//求两个矩形合集矩形
		::UnionRect(this, this, &rc);
	}

/*****************************************
//类：CMuiPtrArray  开始
******************************************/
	CMuiPtrArray::CMuiPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
	{
		MUIASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
	}

	CMuiPtrArray::CMuiPtrArray(const CMuiPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
	{
		for(int i=0; i<src.GetSize(); i++)
			Add(src.GetAt(i));
	}

	CMuiPtrArray::~CMuiPtrArray()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
	}

	void CMuiPtrArray::Empty()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
		m_ppVoid = NULL;
		m_nCount = m_nAllocated = 0;
	}

	void CMuiPtrArray::Resize(int iSize)
	{
		Empty();
		m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
		::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
		m_nAllocated = iSize;
		m_nCount = iSize;
	}

	bool CMuiPtrArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CMuiPtrArray::Add(LPVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		m_ppVoid[m_nCount - 1] = pData;
		return true;
	}

	bool CMuiPtrArray::InsertAt(int iIndex, LPVOID pData)
	{
		if( iIndex == m_nCount ) return Add(pData);
		if( iIndex < 0 || iIndex > m_nCount ) return false;
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
		m_ppVoid[iIndex] = pData;
		return true;
	}

	bool CMuiPtrArray::SetAt(int iIndex, LPVOID pData)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		m_ppVoid[iIndex] = pData;
		return true;
	}

    bool CMuiPtrArray::Remove(int iIndex, int iCount)
    {
        if( iIndex < 0 || iCount <= 0 || iIndex + iCount > m_nCount ) return false;
        if (iIndex + iCount < m_nCount) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + iCount, (m_nCount - iIndex - iCount) * sizeof(LPVOID));
        m_nCount -= iCount;
        return true;
    }

	bool CMuiPtrArray::Remove(LPVOID pData)
	{
		return Remove(Find(pData));
	}

	int CMuiPtrArray::Find(LPVOID pData) const
	{
		for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
		return -1;
	}

	int CMuiPtrArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID* CMuiPtrArray::GetData()
	{
		return m_ppVoid;
	}

	LPVOID CMuiPtrArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_ppVoid[iIndex];
	}

	LPVOID CMuiPtrArray::operator[] (int iIndex) const
	{
		MUIASSERT(iIndex>=0 && iIndex < m_nCount);
		return m_ppVoid[iIndex];
	}


/*****************************************
//类：CMuiIdArray  开始
******************************************/

	CMuiIdArray::CMuiIdArray(int iPreallocSize) 
		: m_ppVoid(NULL)
		, m_nCount(0)
		, m_nAllocated(iPreallocSize)
	{
		MUIASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_ppVoid = static_cast<IDITEM*>(malloc(iPreallocSize * sizeof(IDITEM)));
	}

	CMuiIdArray::~CMuiIdArray()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
	}

	bool CMuiIdArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	int CMuiIdArray::GetSize() const
	{
		return m_nCount;
	}

	bool CMuiIdArray::Add(int id, LPVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			IDITEM * ppVoid = static_cast<IDITEM*>(realloc(m_ppVoid, nAllocated * sizeof(IDITEM)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		m_ppVoid[m_nCount - 1].id = id;
		m_ppVoid[m_nCount - 1].data = pData;
		return true;
	}

	LPVOID CMuiIdArray::Select(int id) const
	{
		for( int i = 0; i < m_nCount; i++ )
		{
			if( m_ppVoid[i].id == id )
			{
				return m_ppVoid[i].data;
			}
		}
		MUIASSERT(-1 == id);
		return NULL;
	}

	LPVOID CMuiIdArray::GetAt(int index) const
	{
		MUIASSERT(index>=0 && index < m_nCount);
		if( index < 0 || index >= m_nCount ) return NULL;
		return m_ppVoid[index].data;
	}

	LPVOID CMuiIdArray::operator[] (int index) const
	{
		return GetAt(index);
	}

/*****************************************
//类：CMuiValArray  开始
******************************************/
	CMuiValArray::CMuiValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
	m_pVoid(NULL), 
		m_nCount(0), 
		m_iElementSize(iElementSize), 
		m_nAllocated(iPreallocSize)
	{
		MUIASSERT(iElementSize>0);
		MUIASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
	}

	CMuiValArray::~CMuiValArray()
	{
		if( m_pVoid != NULL ) free(m_pVoid);
	}

	void CMuiValArray::Empty()
	{   
		m_nCount = 0;  // NOTE: We keep the memory in place
	}

	bool CMuiValArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CMuiValArray::Add(LPCVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
			if( pVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_pVoid = pVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
		return true;
	}

	bool CMuiValArray::Remove(int iIndex, int iCount)
	{
        if( iIndex < 0 || iCount <= 0 || iIndex + iCount > m_nCount ) return false;
        if (iIndex + iCount < m_nCount) ::CopyMemory(m_pVoid + (iIndex * m_iElementSize), m_pVoid + (iIndex + iCount) * m_iElementSize, (m_nCount - iIndex - iCount) * m_iElementSize);
        m_nCount -= iCount;
		return true;
	}

	int CMuiValArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID CMuiValArray::GetData()
	{
		return static_cast<LPVOID>(m_pVoid);
	}

	LPVOID CMuiValArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_pVoid + (iIndex * m_iElementSize);
	}

	LPVOID CMuiValArray::operator[] (int iIndex) const
	{
		MUIASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_pVoid + (iIndex * m_iElementSize);
	}


/*****************************************
//类：CMuiString  开始
******************************************/
	CMuiString::CMuiString() : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = _T('\0');
	}

	CMuiString::CMuiString(const TCHAR ch) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = ch;
		m_szBuffer[1] = _T('\0');
	}

	CMuiString::CMuiString(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer)
	{      
		MUIASSERT(!::IsBadStringPtr(lpsz,-1) || lpsz==NULL);
		m_szBuffer[0] = _T('\0');
		Assign(lpsz, nLen);
	}

	CMuiString::CMuiString(const CMuiString& src) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = _T('\0');
		Assign(src.m_pstr);
	}

	CMuiString::~CMuiString()
	{
		if( m_pstr != m_szBuffer ) free(m_pstr);
	}

    CMuiString CMuiString::ToString()
    {
        return m_pstr;
    }

	int CMuiString::GetLength() const
	{ 
		return (int) _tcslen(m_pstr); 
	}

	CMuiString::operator LPCTSTR() const 
	{ 
		return m_pstr; 
	}

	void CMuiString::Append(LPCTSTR pstr)
	{
		int nNewLength = GetLength() + (int) _tcslen(pstr);
		if( nNewLength >= MAX_LOCAL_STRING_LEN ) {
			if( m_pstr == m_szBuffer ) {
				m_pstr = static_cast<LPTSTR>(malloc((nNewLength + 1) * sizeof(TCHAR)));
				_tcscpy(m_pstr, m_szBuffer);
				_tcscat(m_pstr, pstr);
			}
			else {
				m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (nNewLength + 1) * sizeof(TCHAR)));
				_tcscat(m_pstr, pstr);
			}
		}
		else {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
			_tcscat(m_szBuffer, pstr);
		}
	}

	void CMuiString::Assign(LPCTSTR pstr, int cchMax)
	{
		if( pstr == NULL ) pstr = _T("");
		cchMax = (cchMax < 0 ? (int) _tcslen(pstr) : cchMax);
		if( cchMax < MAX_LOCAL_STRING_LEN ) {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
		}
		else if( cchMax > GetLength() || m_pstr == m_szBuffer ) {
			if( m_pstr == m_szBuffer ) m_pstr = NULL;
			m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (cchMax + 1) * sizeof(TCHAR)));
		}
		_tcsncpy(m_pstr, pstr, cchMax);
		m_pstr[cchMax] = _T('\0');
	}

	bool CMuiString::IsEmpty() const 
	{ 
		return m_pstr[0] == _T('\0'); 
	}

	void CMuiString::Empty() 
	{ 
		if( m_pstr != m_szBuffer ) free(m_pstr);
		m_pstr = m_szBuffer;
		m_szBuffer[0] = _T('\0'); 
	}

	LPCTSTR CMuiString::GetData() const
	{
		return m_pstr;
	}

	TCHAR CMuiString::GetAt(int nIndex) const
	{
		return m_pstr[nIndex];
	}

	TCHAR CMuiString::operator[] (int nIndex) const
	{ 
		return m_pstr[nIndex];
	}   

	const CMuiString& CMuiString::operator=(const CMuiString& src)
	{      
		Assign(src);
		return *this;
	}

	const CMuiString& CMuiString::operator=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			MUIASSERT(!::IsBadStringPtr(lpStr,-1));
			Assign(lpStr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

#ifdef _UNICODE

	const CMuiString& CMuiString::operator=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			MUIASSERT(!::IsBadStringPtrA(lpStr,-1));
			int cchStr = (int) strlen(lpStr) + 1;
			LPWSTR pwstr = (LPWSTR) _alloca(cchStr);
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Assign(pwstr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

	const CMuiString& CMuiString::operator+=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			MUIASSERT(!::IsBadStringPtrA(lpStr,-1));
			int cchStr = (int) strlen(lpStr) + 1;
			LPWSTR pwstr = (LPWSTR) _alloca(cchStr);
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Append(pwstr);
		}
		
		return *this;
	}

#else

	const CMuiString& CMuiString::operator=(LPCWSTR lpwStr)
	{      
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Assign(pstr);
		}
		else
		{
			Empty();
		}
		
		return *this;
	}

	const CMuiString& CMuiString::operator+=(LPCWSTR lpwStr)
	{
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Append(pstr);
		}
		
		return *this;
	}

#endif // _UNICODE

	const CMuiString& CMuiString::operator=(const TCHAR ch)
	{
		Empty();
		m_szBuffer[0] = ch;
		m_szBuffer[1] = _T('\0');
		return *this;
	}

	CMuiString CMuiString::operator+(const CMuiString& src) const
	{
		CMuiString sTemp = *this;
		sTemp.Append(src);
		return sTemp;
	}

	CMuiString CMuiString::operator+(LPCTSTR lpStr) const
	{
		if ( lpStr )
		{
			MUIASSERT(!::IsBadStringPtr(lpStr,-1));
			CMuiString sTemp = *this;
			sTemp.Append(lpStr);
			return sTemp;
		}

		return *this;
	}

	const CMuiString& CMuiString::operator+=(const CMuiString& src)
	{      
		Append(src);
		return *this;
	}

	const CMuiString& CMuiString::operator+=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			MUIASSERT(!::IsBadStringPtr(lpStr,-1));
			Append(lpStr);
		}
		
		return *this;
	}

	const CMuiString& CMuiString::operator+=(const TCHAR ch)
	{      
		TCHAR str[] = { ch, _T('\0') };
		Append(str);
		return *this;
	}

	bool CMuiString::operator == (LPCTSTR str) const { return (Compare(str) == 0); };
	bool CMuiString::operator != (LPCTSTR str) const { return (Compare(str) != 0); };
	bool CMuiString::operator <= (LPCTSTR str) const { return (Compare(str) <= 0); };
	bool CMuiString::operator <  (LPCTSTR str) const { return (Compare(str) <  0); };
	bool CMuiString::operator >= (LPCTSTR str) const { return (Compare(str) >= 0); };
	bool CMuiString::operator >  (LPCTSTR str) const { return (Compare(str) >  0); };

	void CMuiString::SetAt(int nIndex, TCHAR ch)
	{
		MUIASSERT(nIndex>=0 && nIndex<GetLength());
		m_pstr[nIndex] = ch;
	}

	int CMuiString::Compare(LPCTSTR lpsz) const 
	{ 
		return _tcscmp(m_pstr, lpsz); 
	}

	int CMuiString::CompareNoCase(LPCTSTR lpsz) const 
	{ 
		return _tcsicmp(m_pstr, lpsz); 
	}

	void CMuiString::MakeUpper() 
	{
		//大写
		_tcsupr(m_pstr); 
	}

	void CMuiString::MakeLower() 
	{
		//小写
		_tcslwr(m_pstr); 
	}

	CMuiString CMuiString::Left(int iLength) const
	{
		if( iLength < 0 ) iLength = 0;
		if( iLength > GetLength() ) iLength = GetLength();
		return CMuiString(m_pstr, iLength);
	}

	CMuiString CMuiString::Mid(int iPos, int iLength) const
	{
		if( iLength < 0 ) iLength = GetLength() - iPos;
		if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
		if( iLength <= 0 ) return CMuiString();
		return CMuiString(m_pstr + iPos, iLength);
	}

	CMuiString CMuiString::Right(int iLength) const
	{
		int iPos = GetLength() - iLength;
		if( iPos < 0 ) {
			iPos = 0;
			iLength = GetLength();
		}
		return CMuiString(m_pstr + iPos, iLength);
	}

	int CMuiString::Find(TCHAR ch, int iPos /*= 0*/) const
	{
		MUIASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
		LPCTSTR p = _tcschr(m_pstr + iPos, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CMuiString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
	{
		MUIASSERT(!::IsBadStringPtr(pstrSub,-1));
		MUIASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
		LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CMuiString::ReverseFind(TCHAR ch) const
	{
		LPCTSTR p = _tcsrchr(m_pstr, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CMuiString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
	{
		CMuiString sTemp;
		int nCount = 0;
		int iPos = Find(pstrFrom);
		if( iPos < 0 ) return 0;
		int cchFrom = (int) _tcslen(pstrFrom);
		int cchTo = (int) _tcslen(pstrTo);
		while( iPos >= 0 ) {
			sTemp = Left(iPos);
			sTemp += pstrTo;
			sTemp += Mid(iPos + cchFrom);
			Assign(sTemp);
			iPos = Find(pstrFrom, iPos + cchTo);
			nCount++;
		}
		return nCount;
	}

	int CMuiString::Format(LPCTSTR pstrFormat, ...)
	{
		LPTSTR szSprintf = NULL;
		va_list argList;
        int nLen;
		va_start(argList, pstrFormat);
        nLen = _vsntprintf(NULL, 0, pstrFormat, argList);
        szSprintf = (TCHAR*)malloc((nLen + 1) * sizeof(TCHAR));
        ZeroMemory(szSprintf, (nLen + 1) * sizeof(TCHAR));
		int iRet = _vsntprintf(szSprintf, nLen + 1, pstrFormat, argList);
		va_end(argList);
		Assign(szSprintf);
        free(szSprintf);
		return iRet;
	}

	int CMuiString::SmallFormat(LPCTSTR pstrFormat, ...)
	{
		CMuiString sFormat = pstrFormat;
		TCHAR szBuffer[64] = { 0 };
		va_list argList;
		va_start(argList, pstrFormat);
		int iRet = ::wvsprintf(szBuffer, sFormat, argList);
		va_end(argList);
		Assign(szBuffer);
		return iRet;
	}

/*****************************************
//结构体：TITEM  开始
******************************************/
	struct TITEM
    {
        CMuiString Key;
        LPVOID Data;
        struct TITEM* pPrev;
        struct TITEM* pNext;
    };

	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		SIZE_T len = _tcslen(Key);
		while( len-- > 0 ) i = (i << 5) + i + Key[len];
		return i;
	}

	static UINT HashKey(const CMuiString& Key)
	{
		return HashKey((LPCTSTR)Key);
	};

/*****************************************
//类：CMuiStringPtrMap  开始
******************************************/
	CMuiStringPtrMap::CMuiStringPtrMap(int nSize) : m_nCount(0)
	{
		if( nSize < 16 ) nSize = 16;
		m_nBuckets = nSize;
		m_aT = new TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(TITEM*));
	}

	CMuiStringPtrMap::~CMuiStringPtrMap()
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}
	}

	void CMuiStringPtrMap::RemoveAll()
	{
		this->Resize(m_nBuckets);
	}

	void CMuiStringPtrMap::Resize(int nSize)
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}

		if( nSize < 0 ) nSize = 0;
		if( nSize > 0 ) {
			m_aT = new TITEM*[nSize];
			memset(m_aT, 0, nSize * sizeof(TITEM*));
		} 
		m_nBuckets = nSize;
		m_nCount = 0;
	}

	LPVOID CMuiStringPtrMap::Find(LPCTSTR key, bool optimize) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		UINT slot = HashKey(key) % m_nBuckets;
		for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) 
		{
			if( pItem->Key == key ) 
			{
				if (optimize && pItem != m_aT[slot]) 
				{
					if (pItem->pNext) 
					{
						pItem->pNext->pPrev = pItem->pPrev;
					}
					pItem->pPrev->pNext = pItem->pNext;
					pItem->pPrev = NULL;
					pItem->pNext = m_aT[slot];
					pItem->pNext->pPrev = pItem;
					//将item移动至链条头部
					m_aT[slot] = pItem;
				}
				return pItem->Data;
			}        
		}

		return NULL;
	}

	bool CMuiStringPtrMap::Insert(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return false;
		if( Find(key) ) return false;

		// Add first in bucket
		UINT slot = HashKey(key) % m_nBuckets;
		TITEM* pItem = new TITEM;
		pItem->Key = key;
		pItem->Data = pData;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
			pItem->pNext->pPrev = pItem;
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	}

	LPVOID CMuiStringPtrMap::Set(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return pData;

		if (GetSize()>0) {
			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
				if( pItem->Key == key ) {
					LPVOID pOldData = pItem->Data;
					pItem->Data = pData;
					return pOldData;
				}
			}
		}

		Insert(key, pData);
		return NULL;
	}

	bool CMuiStringPtrMap::Remove(LPCTSTR key)
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		UINT slot = HashKey(key) % m_nBuckets;
		TITEM** ppItem = &m_aT[slot];
		while( *ppItem ) {
			if( (*ppItem)->Key == key ) {
				TITEM* pKill = *ppItem;
				*ppItem = (*ppItem)->pNext;
				if (*ppItem)
					(*ppItem)->pPrev = pKill->pPrev;
				delete pKill;
				m_nCount--;
				return true;
			}
			ppItem = &((*ppItem)->pNext);
		}

		return false;
	}

	int CMuiStringPtrMap::GetSize() const
	{
#if 0//def _DEBUG
		int nCount = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) nCount++;
		}
		ASSERT(m_nCount==nCount);
#endif
		return m_nCount;
	}

	LPCTSTR CMuiStringPtrMap::GetAt(int iIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == iIndex ) {
					return pItem->Key.GetData();
				}
			}
		}

		return NULL;
	}

	LPCTSTR CMuiStringPtrMap::operator[] (int nIndex) const
	{
		return GetAt(nIndex);
	}


/*****************************************
//类：CWaitCursor  开始
******************************************/
	CWaitCursor::CWaitCursor()
	{
		m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	}

	CWaitCursor::~CWaitCursor()
	{
		::SetCursor(m_hOrigCursor);
	}
}