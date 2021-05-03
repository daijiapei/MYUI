
#ifndef __MYUI_BASE_H__
#define __MYUI_BASE_H__


#ifdef __GNUC__
// 如果没找到min，max的头文件的话-_-
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#endif

#define _CRT_SECURE_NO_DEPRECATE

// Remove pointless warning messages
#ifdef _MSC_VER
#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
#pragma warning (disable : 4786) // identifier was truncated
#pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
#pragma warning (disable : 4251) // using namespace std

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif
#endif // _MSC_VER
#ifdef __BORLANDC__
#pragma option -w-8027		   // function not expanded inline
#endif

// Required for VS 2008 (fails on XP and Win2000 without this fix)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif


#include <olectl.h>

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))


#define ENABLE_CONTROL_HOOK     //启用自带的Hook接口

#ifdef MYUI_STATIC
#	define MYUI_API 
#else
#	if defined(MYUI_EXPORTS)
#		if	defined(_MSC_VER)
#			define MYUI_API __declspec(dllexport)
#		else
#			define MYUI_API 
#		endif
#	else
#		if defined(_MSC_VER)
#			define MYUI_API __declspec(dllimport)
#		else
#			define MYUI_API 
#		endif
#	endif
#endif

#define MYUI_COMDAT __declspec(selectany)

#if defined _M_IX86
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#	pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif



#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <stdio.h>

#pragma comment(lib, "msimg32")     // AlphaBlend,透明化
#pragma comment(lib, "Shlwapi") 
#pragma comment(lib, "gdiplus")
#include <initguid.h>     
#include <comdef.h>

#if defined(UNICODE)
# define _INC_CRT_UNICODE_MACROS 1
# define __MINGW_NAME_AW(func) func##W
# define __MINGW_NAME_AW_EXT(func,ext) func##W##ext
# define __MINGW_NAME_UAW(func) func##_W
# define __MINGW_NAME_UAW_EXT(func,ext) func##_W_##ext
# define __MINGW_STRING_AW(str) L##str	/* same as TEXT() from winnt.h */
# define __MINGW_PROCNAMEEXT_AW "W"
#else
# define _INC_CRT_UNICODE_MACROS 2
# define __MINGW_NAME_AW(func) func##A
# define __MINGW_NAME_AW_EXT(func,ext) func##A##ext
# define __MINGW_NAME_UAW(func) func##_A
# define __MINGW_NAME_UAW_EXT(func,ext) func##_A_##ext
# define __MINGW_STRING_AW(str) str	/* same as TEXT() from winnt.h */
# define __MINGW_PROCNAMEEXT_AW "A"
#endif

#define __MINGW_TYPEDEF_AW(type)	\
    typedef __MINGW_NAME_AW(type) type;
#define __MINGW_TYPEDEF_UAW(type)	\
    typedef __MINGW_NAME_UAW(type) type;


#define MUIASSERT(expr)    _ASSERTE(expr)


#ifndef ARGB
typedef DWORD ARGBREF;
#define ARGB(a,r,g,b)          ((ARGBREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)))
#endif

#ifdef RGB
#undef RGB
#define RGB(r,g,b) ARGB(255,r,g,b)
#endif

#ifdef GetRValue
#undef GetRValue
#undef GetGValue
#undef GetBValue

#define GetRValue(argb)      (LOBYTE(argb))
#define GetGValue(argb)      (LOBYTE(((WORD)(argb)) >> 8))
#define GetBValue(argb)      (LOBYTE((argb)>>16))
#define GetAValue(argb)      (LOBYTE((argb)>>24))
#define GetRGBValue(argb)    ((COLORREF)((argb) & 0x00FFFFFF))
#endif


//提取pt
#define GetPointByLong(pt, l) \
	((pt).x = LOWORD((l)) , (pt).y = HIWORD((l)))

//point 是否在rect 里面
#define PointInRect(pt, rc) \
	((((pt).x >= (rc).left && (rc).right >= (pt).x) && ((pt).y >= (rc).top && (rc).bottom >= (pt).y)) ? TRUE : FALSE)

//x,y 是否在rect里面
#define PointInRect2(x, y, rc) \
	(((x) >= (rc).left && (rc).right >= (x) && (y) >= (rc).top && (rc).bottom >= (y)) ? TRUE : FALSE)

//rc1 的某个角坐标，是否落在rc2里面
#define IsCornerInRect(rc1, rc2) \
	(PointInRect2((rc1).left,(rc1).top, rc2) || PointInRect2((rc1).right,(rc1).bottom, rc2)\
	|| PointInRect2((rc1).right,(rc1).top, rc2) || PointInRect2((rc1).left,(rc1).bottom, rc2))

//置空
#define EmptyRect(rc) \
	((rc).left = 0, (rc).top = 0, (rc).right = 0, (rc).bottom = 0)

//是否空的
#define IsEmptyRect(rc) \
	((0==(rc).left && 0==(rc).top && 0==(rc).right && 0==(rc).bottom) ? TRUE : FALSE)

//rc1 是否与 rc2 相交
#define IsContainRect(rc1, rc2) \
	((IsEmptyRect(rc1) || IsEmptyRect(rc2) || (rc1).left >= (rc2).right || (rc1).top >= (rc2).bottom\
	|| (rc2).left >= (rc1).right || (rc2).top >= (rc1).bottom) ? FALSE : TRUE)

//是否无效的rect， left大于right 或者 top大于bottom，则无效
#define IsValidRect(rc)\
	(((rc).right > (rc).left && (rc).bottom > (rc).top) ? TRUE : FALSE)

//检查布尔值
#define CheckTrue(pstr)\
	((	(_T('1') == (pstr)[0] && _T('\0') == (pstr)[1]	) || \
	(	(_T('T') == (pstr)[0] || _T('t') == (pstr)[0]) && (_T('R') == (pstr)[1] || _T('r') == (pstr)[1]) && \
	(_T('U') == (pstr)[2] || _T('u') == (pstr)[2]) && (_T('E') == (pstr)[3] || _T('e') == (pstr)[3]) && \
	_T('\0') == (pstr)[4])) ? TRUE : FALSE)

#define CheckFalse(pstr)\
	((	(_T('0') == (pstr)[0] && _T('\0') == (pstr)[1]	) || \
	(	(_T('F') == (pstr)[0] || _T('f') == (pstr)[0]) && (_T('A') == (pstr)[1] || _T('a') == (pstr)[1]) && \
	(_T('L') == (pstr)[2] || _T('l') == (pstr)[2]) && (_T('S') == (pstr)[3] || _T('s') == (pstr)[3]) && \
	(_T('E') == (pstr)[4] || _T('e') == (pstr)[4]) && _T('\0') == (pstr)[5])) ? TRUE : FALSE)

//是否正确命名
#define CheckBoer(pstr)  ((CheckTrue(pstr) || CheckFalse(pstr)) ? TRUE : FALSE)

//检查调整的参数是否正确
//禁止采用负数调整
#define __CheckAdjust(rcSize) \
	((0 <= (rcSize).left && 0 <= (rcSize).top && 0 <= (rcSize).right\
	&& 0 <= (rcSize).bottom) ? TRUE : FALSE)

//外扩
#define __ExpandRect(rcDst, rcSize)\
	((rcDst).left -= (rcSize).left, (rcDst).top -= (rcSize).top,\
	(rcDst).right += (rcSize).right, (rcDst).bottom += (rcSize).bottom)

//内缩
#define __IndentRect(rcDst, rcSize)\
	((rcDst).left += (rcSize).left, (rcDst).top += (rcSize).top,\
	(rcDst).right -= (rcSize).right, (rcDst).bottom -= (rcSize).bottom)

//相同的rect
#define IsSameRect(rc1, rc2) \
	(((rc1).left == (rc2).left && (rc1).top == (rc2).top &&\
	(rc1).right == (rc2).right && (rc1).bottom == (rc2).bottom) ? TRUE : FALSE)

//检测一个字符编码的长度
#define check_unicode_lenght(pstr_c) (( (0x0000FFFF & (pstr_c)[0]) >= 0x0000d800 && 0x0000dbff >= (0x0000FFFF & (pstr_c)[0]) \
	&& (0x0000FFFF & (pstr_c)[1]) >= 0x0000dc00 && 0x0000dfff >= (0x0000FFFF & (pstr_c)[1])) ? 2 : 1)

#define check_gbk_lenght(pstr_c) ((	(0x00FF & (pstr_c)[0]) >= 0x0081 && 0x00FE >= (0x00FF & (pstr_c)[0])\
	&& (0x00FF & (pstr_c)[1]) >= 0x0040 && 0x00FE >= (0x00FF & (pstr_c)[1])) ? 2 : 1)

#ifdef _UNICODE
#define check_encode_lenght check_unicode_lenght
#else
#define check_encode_lenght check_gbk_lenght
#endif

//按键是否按下
#define  IsKeyPressed(nVirtKey)     ((0x00008000 & ((DWORD)((SHORT)GetKeyState(nVirtKey)))) != 0)

BOOL GetItemFixed(RECT& rcFixedItem, RECT& rcRawItem, RECT& rcPaintItem, BOOL bFloat);
#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllexport) void __Mui_Trace__(LPCTSTR pstrFormat, ...);
BOOL ContainRect(LPRECT lprcDst, CONST RECT *lprcSrc1, CONST RECT *lprcSrc2);
BOOL ExpandRect(LPRECT lprcDst, CONST RECT *lprcExpand);
BOOL IndentRect(LPRECT lprcDst, CONST RECT *lprcIndent);
ARGBREF GetArgbFromString(LPCTSTR strColor);
BOOL GetStringLine(LPCTSTR strText, int nStrLenght, int nStart,__out int * nCount, __out int * nTabCount);
void PixelToHiMetric(HWND hWnd, const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric);
int check_pre_text_encode_lenght(LPCTSTR strText, int nIndex);
BOOL ExtractInfo(LPCTSTR strSrc, TCHAR * strItem, TCHAR * strValue, int *nReadLen);

#define CPOT_TOP     0x01
#define CPOT_LEFT    0X02
#define CPOT_RIGHT   0x03
#define CPOT_BOTTOM  0X04
POINT CalcPopupPoint(const RECT * pRect, const SIZE * pSize, DWORD dwPopupType);

#ifdef __cplusplus
}
#endif

#ifdef _DEBUG
#define MUITRACE __Mui_Trace__
#else
#define MUITRACE __noop
#endif

#endif