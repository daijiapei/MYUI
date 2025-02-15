
#include "base.h"
#include <Shlwapi.h>

//SetLayeredWindowAttributes
__declspec(dllexport) void __Mui_Trace__(LPCTSTR pstrFormat, ...)
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
		argb |= (0x000000FF & _tcstol(strColor + 4, &pstr, 10)) << 16;	MUIASSERT(pstr);
		argb |= (0x000000FF & _tcstol(pstr + 1, &pstr, 10)) << 8;      MUIASSERT(pstr);
		argb |= 0x000000FF & _tcstol(pstr + 1, &pstr, 10);      MUIASSERT(pstr);
	}
	else if((_T('a') == strColor[0] || _T('A') == strColor[0])
		&& (_T('r') == strColor[1] || _T('R') == strColor[1])
		&& (_T('g') == strColor[2] || _T('G') == strColor[2])
		&& (_T('b') == strColor[3] || _T('B') == strColor[3]))
	{
		//ARGB(255,255,255,255)
		argb = 0x00000000;
		argb |= (0x000000FF & _tcstol(strColor + 5, &pstr, 10)) << 24;	MUIASSERT(pstr);
		argb |= (0x000000FF & _tcstol(pstr + 1, &pstr, 10)) << 16;      MUIASSERT(pstr);
		argb |= (0x000000FF & _tcstol(pstr + 1, &pstr, 10)) << 8;      MUIASSERT(pstr);
		argb |= 0x000000FF & _tcstol(pstr + 1, &pstr, 10);      MUIASSERT(pstr);
	}
	else
	{
		//不知道是什么，抛出一个错误看看
		MUIASSERT(!strColor);
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


POINT CalcPopupPoint(const RECT * pRect, const SIZE * pSize, DWORD dwPopupType)
{
    POINT ptPopup = { 0 };
    SIZE szScreen = { 0 };

    szScreen.cx = GetSystemMetrics(SM_CXSCREEN);
    szScreen.cy = GetSystemMetrics(SM_CYSCREEN);

    switch (dwPopupType)
    {
    case CPOT_TOP:
    {

    }break;
    case CPOT_LEFT:
    {

    }break;
    case CPOT_RIGHT:
    {
        //从右边弹出
        if (pSize->cx + pRect->right > szScreen.cx && pRect->left - pSize->cx > 0)
        {
            ptPopup.x = pRect->left - pSize->cx;
        }
        else
        {
            ptPopup.x = pRect->right;
        }

        if (pSize->cy + pRect->top > szScreen.cy && pRect->top - pSize->cy > 0)
        {
            ptPopup.y = pRect->top - pSize->cy;
        }
        else
        {
            ptPopup.y = pRect->top;
        }
    }break;
    case CPOT_BOTTOM:
    default:
    {
        //否则从底部弹出
        if (pSize->cx + pRect->left > szScreen.cx && pRect->right - pSize->cx > 0)
        {
            ptPopup.x = pRect->right - pSize->cx;
        }
        else
        {
            ptPopup.x = pRect->left;
        }

        if (pSize->cy + pRect->bottom > szScreen.cy && pRect->top - pSize->cy > 0)
        {
            ptPopup.y = pRect->top - pSize->cy;
        }
        else
        {
            ptPopup.y = pRect->bottom;
        }
    }break;
    }

    return ptPopup;
}


/**********************************************************************************************
//rcRawItem是原始的，相对于父布局的位置，要通过布局偏移计算，才能得出正确的，要显示的绝对位置
//GetItemFixed函数通过rcPaintItem计算出rcRawItem在HDC中要显示的绝对位置，保存到rcFixedItem中
//返回true说明rcFixedItem有效， rcRawItem的绝对位置与rcPaintItem相交，可以显示
//返回false说明rcFixedItem无效，rcRawItem的绝对位置与rcPaintItem不相交，不能够显示
//@rcFixedItem：计算得出rcRawItem，在HDC中的绝对位置
//@rcRawItem：在布局中的原始位置
//@rcPaintItem：HDC提供给rcRawItem的显示范围，rcRawItem的绝对范围与rcPaintItem不相交则返回true
//@bFloat：是否用绝对布局计算
**********************************************************************************************/
BOOL GetItemFixed(RECT& rcFixedItem, RECT& rcRawItem, RECT& rcPaintItem, BOOL bFloat)
{
	RECT rect = rcRawItem;
	SIZE OffsetSize = { 0 };

	if (FALSE == bFloat)//相对位置计算与显示
	{
		//相对位置显示则尽量居中rect
		OffsetSize.cx = ((rcPaintItem.right - rcPaintItem.left) - (rect.right - rect.left)) / 2;
		OffsetSize.cy = ((rcPaintItem.bottom - rcPaintItem.top) - (rect.bottom - rect.top)) / 2;

		//如果Item大于绝对位置，则从0,0位置开始显示
		OffsetSize.cx = OffsetSize.cx > 0 ? OffsetSize.cx : 0;
		OffsetSize.cy = OffsetSize.cy > 0 ? OffsetSize.cy : 0;
	}

	//计算rcItem 和 rcPaintFixed的交集位置, 用来计算复制的宽和高
	OffsetRect(&rect, rcPaintItem.left + OffsetSize.cx, rcPaintItem.top + OffsetSize.cy);
	return IntersectRect(&rcFixedItem, &rect, &rcPaintItem);
}

//判断两个矩形是否相交
