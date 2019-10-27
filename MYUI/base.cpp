
#include "base.h"
#include <Shlwapi.h>

//SetLayeredWindowAttributes
__declspec(dllexport) void __Trace__(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
    TCHAR szBuffer[300] = { 0 };
    va_list args;
    va_start(args, pstrFormat);
    ::wvnsprintf(szBuffer, lengthof(szBuffer) - 2, pstrFormat, args);
    _tcscat(szBuffer, _T("\n"));
    va_end(args);
    ::OutputDebugString(szBuffer);
#endif
}

BOOL ExpandRect(LPRECT lprcDst, CONST RECT *lprcExpand)
{
	__ExpandRect(*lprcDst,*lprcExpand);
	if(FALSE == IsValidRect(*lprcDst) || FALSE == __CheckAdjust(*lprcExpand))
	{
		//lprcDst 无效，或者参数无效
		EmptyRect(*lprcDst);
		return FALSE;
	}
	return TRUE;
}

BOOL IndentRect(LPRECT lprcDst, CONST RECT *lprcIndect)
{
	__IndentRect(*lprcDst,*lprcIndect);
	if(FALSE == IsValidRect(*lprcDst) || FALSE == __CheckAdjust(*lprcIndect))
	{
		//lprcDst 无效，或者参数无效
		EmptyRect(*lprcDst);
		return FALSE;
	}
	return TRUE;
}

//提取颜色
ARGBREF GetArgbFromString(LPCTSTR strColor)
{
	ARGBREF argb = NULL;
	LPTSTR pstr = NULL;

	if(NULL == strColor) return argb;

	if(_T('#') == strColor[0])
	{
		if(7 == _tcslen(strColor))
		{
			//mask='#FF00FF'
			argb = 0xFF000000 | _tcstoul(strColor + 1, NULL, 16);
		}
		else if(9 == _tcslen(strColor))
		{
			//mask='#FF00FF00'
			argb = _tcstoul(strColor + 1, NULL, 16);
		}
		else
		{
		}
	}
	else if(_T('0') == strColor[0] && (_T('x') == strColor[1] || _T('X') == strColor[1]))
	{
		if(8 == _tcslen(strColor))
		{
			//mask='0xFF00FF'
			argb = 0xFF000000 | _tcstoul(strColor + 2, NULL, 16);
		}
		else if(10 == _tcslen(strColor))
		{
			//mask='0xFF00FF00'
			argb = _tcstoul(strColor + 2, NULL, 16);
		}
		else
		{
		}
	}
	else if((_T('r') == strColor[0] || _T('R') == strColor[0])
		&& (_T('g') == strColor[1] || _T('G') == strColor[1])
		&& (_T('b') == strColor[2] || _T('B') == strColor[2]))
	{
		//RGB(255,255,255) 

		argb = 0xFF000000;
		argb |= (0x000000FF & _tcstol(strColor + 4, &pstr, 10)) << 16;	ASSERT(pstr);
		argb |= (0x000000FF & _tcstol(pstr + 1, &pstr, 10)) << 8;      ASSERT(pstr);
		argb |= 0x000000FF & _tcstol(pstr + 1, &pstr, 10);      ASSERT(pstr);
	}
	else if((_T('a') == strColor[0] || _T('A') == strColor[0])
		&& (_T('r') == strColor[1] || _T('R') == strColor[1])
		&& (_T('g') == strColor[2] || _T('G') == strColor[2])
		&& (_T('b') == strColor[3] || _T('B') == strColor[3]))
	{
		//ARGB(255,255,255,255)
		argb = 0x00000000;
		argb |= (0x000000FF & _tcstol(strColor + 5, &pstr, 10)) << 24;	ASSERT(pstr);
		argb |= (0x000000FF & _tcstol(pstr + 1, &pstr, 10)) << 16;      ASSERT(pstr);
		argb |= (0x000000FF & _tcstol(pstr + 1, &pstr, 10)) << 8;      ASSERT(pstr);
		argb |= 0x000000FF & _tcstol(pstr + 1, &pstr, 10);      ASSERT(pstr);
	}
	else
	{
		//不知道是什么，抛出一个错误看看
		ASSERT(!strColor);
	}

	argb = ARGB(GetAValue(argb) , GetBValue(argb), GetGValue(argb), GetRValue(argb));

	return argb;
}

//返回false表示字符串已结束
BOOL GetStringLine(LPCTSTR strText, int nStrLenght, int nStart,__out int * nCount, __out int * nTabCount)
{
	//结束符\0也计算在内
		*nCount = 0;
		if(nTabCount) *nTabCount = 0;
		for(int index = nStart; nStrLenght > index; index++)
		{
			++(*nCount);
			switch(strText[index])
			{
			case _T('\t'):
				{
					if(nTabCount) ++(*nTabCount);
				}break;
			case _T('\r'):
				{
					if(_T('\n') == strText[index + 1])
					{
						++(*nCount);
					}
					return TRUE;
				}break;
			case _T('\n'):
				{
					if(_T('\r') == strText[index + 1])
					{
						++(*nCount);
					}
					return TRUE;
				}break;
			default:
				{
					
				}break;
			}
		}
		return FALSE;
}

BOOL ContainRect(LPRECT lprcDst, CONST RECT *lprcSrc1, CONST RECT *lprcSrc2)
{
	RECT rcDest = {0};
	BOOL bRet = FALSE;
	do
	{
		//lprcSrc1 的某个坐标落实lprcSrc2里面
		if(PointInRect2(lprcSrc1->left, lprcSrc1->top, *lprcSrc2))
		{
			rcDest.left = lprcSrc1->left;
			rcDest.top = lprcSrc1->top;
			bRet = TRUE;
		}
		if (PointInRect2(lprcSrc1->right, lprcSrc1->bottom, *lprcSrc2))
		{
			rcDest.right = lprcSrc1->right;
			rcDest.bottom = lprcSrc1->bottom;
			bRet = TRUE;
		}

		if (PointInRect2(lprcSrc1->right, lprcSrc1->top, *lprcSrc2))
		{
			rcDest.right = lprcSrc1->right;
			rcDest.top = lprcSrc1->top;
			bRet = TRUE;
		}

		if (PointInRect2(lprcSrc1->left, lprcSrc1->bottom, *lprcSrc2))
		{
			rcDest.left = lprcSrc1->left;
			rcDest.bottom = lprcSrc1->bottom;
			bRet = TRUE;
		}

		//lprcSrc2 的某个坐标落实lprcSrc1里面
		if(PointInRect2(lprcSrc2->left, lprcSrc2->top, *lprcSrc1))
		{
			rcDest.left = lprcSrc2->left;
			rcDest.top = lprcSrc2->top;
			bRet = TRUE;
		}
		if (PointInRect2(lprcSrc2->right, lprcSrc2->bottom, *lprcSrc1))
		{
			rcDest.right = lprcSrc2->right;
			rcDest.bottom = lprcSrc2->bottom;
			bRet = TRUE;
		}

		if (PointInRect2(lprcSrc2->right, lprcSrc2->top, *lprcSrc1))
		{
			rcDest.right = lprcSrc2->right;
			rcDest.top = lprcSrc2->top;
			bRet = TRUE;
		}

		if (PointInRect2(lprcSrc2->left, lprcSrc2->bottom, *lprcSrc1))
		{
			rcDest.left = lprcSrc2->left;
			rcDest.bottom = lprcSrc2->bottom;
			bRet = TRUE;
		}
	}while(0);
	
	*lprcDst = rcDest;
	return bRet;
}


int check_pre_text_encode_lenght(LPCTSTR strText, int nIndex)
{
	int nBeginIndex = nIndex - 1;
	int nLenght = 0;

	while(nBeginIndex > 0)
	{
		if(_T('\r') == strText[nBeginIndex] || _T('\n') == strText[nBeginIndex])
		{
			break;
		}
		--nBeginIndex;
	}

	while(nIndex > nBeginIndex)
	{
		nLenght = check_encode_lenght(&strText[nBeginIndex]);
		nBeginIndex += nLenght;
	}

	return nLenght;
}

//提取字符串"file='skin\hello' name='btn_close'"
//strSrc 必须以_T('\0')结尾
//strItem 与 strValue 必须提供足够的字符串长度,
BOOL ExtractInfo(LPCTSTR strSrc, TCHAR * strItem, TCHAR * strValue, int *nReadLen)
{
	WORD wState = 0x0000;
	int nIndex = 0;

	if(_T('\0') == strSrc[0]) return FALSE;

	while(_T(' ') == strSrc[nIndex]) ++nIndex;
	//strItem
	while(_T(' ') != strSrc[nIndex] && _T('\0') != strSrc[nIndex] && _T('=') != strSrc[nIndex])
	{
		*(strItem++) = strSrc[nIndex++];
		wState |= 0x01;
	}
	*strItem = _T('\0');

	//find _T('=')
	while(_T(' ') == strSrc[nIndex]) ++nIndex;
	if(_T('=') != strSrc[nIndex++]) return FALSE;
	while(_T(' ') == strSrc[nIndex]) ++nIndex;
	if(_T('\'') == strSrc[nIndex])  ++nIndex;

	//strValue
	while(_T(' ') != strSrc[nIndex] && _T('\0') != strSrc[nIndex] && _T('\'') != strSrc[nIndex])
	{
		*(strValue++) = strSrc[nIndex++];
		wState |= 0x02;
	}
	*strValue = _T('\0');

	if(_T('\'') == strSrc[nIndex])  ++nIndex;
	*nReadLen = nIndex;
	return (0x01 | 0x02) == wState;
}

void PixelToHiMetric(HWND hWnd, const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric)
{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
    int nPixelsPerInchX;    // Pixels per logical inch along width
    int nPixelsPerInchY;    // Pixels per logical inch along height
    HDC hDCScreen = ::GetDC(hWnd);
    nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
    nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
    ::ReleaseDC(NULL, hDCScreen);
    lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, nPixelsPerInchX);
    lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, nPixelsPerInchY);
}