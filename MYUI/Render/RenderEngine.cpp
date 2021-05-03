
#include "RenderEngine.h"
#include "GdiEngine.h"

namespace MYUI
{
    //typedef struct __MUICARET_INFO
    //{
    //    HWND hWnd;
    //    HBITMAP Bitmap;
    //    POINT Pos;
    //    SIZE Size;
    //    BYTE Visible;
    //    BYTE Showing;
    //} MUICARETINFO, *LPMUICARETINFO;

    //void DrawCaret(HWND hWnd, LPMUICARETINFO CaretInfo)
    //{
    //    HDC hdc, hdcMem;
    //    HBITMAP hbmOld;
    //    BOOL bDone = FALSE;

    //    hdc = GetDC(hWnd);
    //    if (!hdc) return;

    //    if (CaretInfo->Bitmap && GetBitmapDimensionEx(CaretInfo->Bitmap, &CaretInfo->Size))
    //    {
    //        hdcMem = CreateCompatibleDC(hdc);
    //        if (hdcMem)
    //        {
    //            hbmOld = (HBITMAP)SelectObject(hdcMem, CaretInfo->Bitmap);
    //            bDone = BitBlt(hdc,
    //                CaretInfo->Pos.x,
    //                CaretInfo->Pos.y,
    //                CaretInfo->Size.cx,
    //                CaretInfo->Size.cy,
    //                hdcMem,
    //                0,
    //                0,
    //                SRCINVERT);
    //            SelectObject(hdcMem, hbmOld);
    //            DeleteDC(hdcMem);
    //        }
    //    }

    //    if (!bDone)
    //    {
    //        PatBlt(hdc,
    //            CaretInfo->Pos.x,
    //            CaretInfo->Pos.y,
    //            CaretInfo->Size.cx,
    //            CaretInfo->Size.cy,
    //            DSTINVERT);
    //    }

    //    ReleaseDC(hWnd, hdc);
    //}

    CRenderEngine::CRenderEngine(HWND hWnd)
	{
		//在Window平台中，这是一个窗口句柄
        m_hWnd = hWnd;
		m_ptDrawPos.x = m_ptDrawPos.y = 0;
		memset(m_strSkin, 0, sizeof(m_strSkin));
	}

	CRenderEngine::~CRenderEngine()
	{
        m_hWnd = NULL;
        SetSkinFolder(NULL);
	}

	CRenderEngine* CRenderEngine::Create(LPCTSTR strName, HWND hWnd)
	{
		CRenderEngine* pRenderEngine = NULL;
		if (0 == _tcsicmp(strName, _T("UnKown")))
		{
			//新添加的引擎
		}
		else
		{
			//默认引擎
			pRenderEngine = new CGdiRenderEngine(hWnd);
		}

		return pRenderEngine;
	}

    bool CRenderEngine::SetSkinFolder(LPCTSTR strSkin)
    {
        //如果strSkin传入空，将会卸载原来的皮肤
        if (strSkin && 0 == _tcsicmp(m_strSkin, strSkin)) return true;

        if (_T('\0') != m_strSkin[0])
        {
            //已存在皮肤
            CSkinManager::RemoveSkin(m_strSkin);
            memset(m_strSkin, 0, sizeof(m_strSkin));
        }

        if (NULL == strSkin || _T('\0') == strSkin[0])
        {
            return true;
        }

        if (strSkin && _T('\0') != strSkin[0] && CSkinManager::AddSkin(strSkin) > 0)
        {
            _tcscpy(m_strSkin, strSkin);
            return true;
        }
        return false;
    }

	LPCTSTR CRenderEngine::GetSkinFolder() const
	{
		return m_strSkin;
	}

	bool CRenderEngine::OffsetDrawPoint(int nX, int nY)
	{
		m_ptDrawPos.x += nX;
		m_ptDrawPos.y += nY;
		return true;
	}

	POINT CRenderEngine::GetDrawPoint()
	{
		return m_ptDrawPos;
	}

	HCLIP CRenderEngine::EnterClip(const RECT& rcPaint, SIZE& szRound)
	{
		RECT rcPaint2 = rcPaint;
		::OffsetRect(&rcPaint2, m_ptDrawPos.x, m_ptDrawPos.y);
		return EnterClipImp(rcPaint2, szRound);
	}

	void  CRenderEngine::LeaveClip(HCLIP hOldClip)
	{
		return LeaveClipImp(hOldClip);
	}

	bool CRenderEngine::OnDrawImage(const RECT& rcDraw, LPCTSTR strImage)
	{
		RECT rcDraw2 = rcDraw;
		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		return DrawImageImp(rcDraw2, strImage);
	}

	bool CRenderEngine::OnDrawFrame(const RECT& rcDraw, ARGBREF refColor)
	{
		RECT rcDraw2 = rcDraw;
		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		return DrawFrameImp(rcDraw2, refColor);
	}

	bool CRenderEngine::OnDrawLine(const POINT& ptBegin, const POINT& ptEnd, int nPenSize, ARGBREF refColor)
	{
		POINT ptBegin2 = ptBegin;
		POINT ptEnd2 = ptEnd;

		ptBegin2.x += m_ptDrawPos.x;
		ptBegin2.y += m_ptDrawPos.y;
		ptEnd2.x += m_ptDrawPos.x;
		ptEnd2.y += m_ptDrawPos.y;
		return DrawLineImp(ptBegin2, ptEnd2, nPenSize, refColor);
	}

	bool CRenderEngine::OnDrawColor(const RECT& rcDraw, ARGBREF refColor)
	{
		RECT rcDraw2 = rcDraw;
		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		return DrawColorImp(rcDraw2, refColor);
	}

	bool CRenderEngine::OnDrawText(const RECT& rcDraw, LPCTSTR strText, ARGBREF refTextColor, HFONT hFont, DWORD dwType)
	{
		RECT rcDraw2 = rcDraw;
		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		return DrawTextImp(rcDraw2, strText, refTextColor, hFont, dwType);
	}

	bool CRenderEngine::OnDrawBroder(const RECT& rcDraw, ARGBREF refColor, int nBroderSize, SIZE& szRound)
	{
		RECT rcDraw2 = rcDraw;
		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		return DrawBroderImp(rcDraw2, refColor, nBroderSize, szRound);
	}

	bool CRenderEngine::OnDrawImage(const RECT& rcDraw, MUIIMAGEINFO* pImageInfo, const RECT& rcSource,
		const RECT& rcCorner, ARGBREF refMask, DWORD dwFade, BOOL bHole)
	{
		RECT rcDraw2 = rcDraw;
		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		return DrawImageImp(rcDraw2, pImageInfo, rcSource, rcCorner, refMask, dwFade, bHole);
	}

	int CRenderEngine::GetTextHeight(HFONT hFont)
	{
		return GetTextHeightImp(hFont);
	}

	SIZE CRenderEngine::GetTextSize(const RECT& rcItem, LPCTSTR strText, int nLenght,
		HFONT hFont, int nRowSpace, DWORD dwTextStyle)
	{
		return GetTextSizeImp(rcItem, strText, nLenght, hFont, nRowSpace, dwTextStyle);
	}

	int CRenderEngine::TestTextIndex(const RECT& rcItem, POINT& ptMouse,
		LPCTSTR strText, int nLenght, HFONT hFont, int nRowSpace, DWORD dwTextStyle)
	{
		return TestTextIndexImp(rcItem, ptMouse, strText, nLenght, hFont, nRowSpace, dwTextStyle);
	}

	POINT CRenderEngine::GetTextPos(const RECT& rcItem, int nTextIndex, LPCTSTR strText,
		HFONT hFont, int nRowSpace, DWORD dwTextStyle)
	{
		return GetTextPosImp(rcItem, nTextIndex, strText, hFont, nRowSpace, dwTextStyle);
	}

	bool CRenderEngine::OnTextOut(const RECT& rcDraw, POINT& ptOutput, LPCTSTR strText, int nStrLenght,
		ARGBREF refTextColor, HFONT hFont, int nRowSpace, DWORD dwTextStyle, ARGBREF refTextBkColor)
	{
		bool bRet = false;
		RECT rcDraw2 = rcDraw;
		POINT ptOutput2 = ptOutput;

		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		ptOutput.x += m_ptDrawPos.x;
		ptOutput.y += m_ptDrawPos.y;

		bRet = TextOutImp(rcDraw2, ptOutput, strText, nStrLenght, refTextColor, hFont, nRowSpace,
			dwTextStyle, refTextBkColor);

		ptOutput.x -= m_ptDrawPos.x;
		ptOutput.y -= m_ptDrawPos.y;

		return bRet;
	}

	bool CRenderEngine::SetCaret(const RECT& rcDraw, const POINT ptCaret, HFONT hFont)
	{
		RECT rcDraw2 = rcDraw;
		POINT ptCaret2 = ptCaret;

		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		ptCaret2.x += m_ptDrawPos.x;
		ptCaret2.y += m_ptDrawPos.y;

		return SetCaretImp(rcDraw2, ptCaret2, hFont);
	}

	bool CRenderEngine::OnDrawOleObject(const RECT& rcDraw, IViewObject* pViewObject)
	{
		RECT rcDraw2 = rcDraw;
		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		return DrawOleObjectImp(rcDraw2, pViewObject);
	}

	bool CRenderEngine::OnDrawHtmlText(const RECT& rcDraw, LPCTSTR strText,
		CMuiIdArray* FontArray, ARGBREF refTextColor)
	{
		RECT rcDraw2 = rcDraw;
		::OffsetRect(&rcDraw2, m_ptDrawPos.x, m_ptDrawPos.y);
		return DrawHtmlTextImp(rcDraw2, strText, FontArray, refTextColor);
	}

	//SIZE CRenderEngine::GetTextSize( HDC hDC, CPaintManagerUI* pManager , LPCTSTR pstrText, int iFont, UINT uStyle )
	//{
	//	SIZE size = {0,0};
	//	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	//	if( pstrText == NULL || pManager == NULL ) return size;
	//	::SetBkMode(hDC, TRANSPARENT);
	//	HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
	//	GetTextExtentPoint32(hDC, pstrText, _tcslen(pstrText) , &size);
	//	::SelectObject(hDC, hOldFont);
	//	return size;
	//}

	bool CRenderEngine::DrawImageImp(const RECT& rcDraw, LPCTSTR strImage)
	{
		int nSize = _tcslen(strImage);
		LPCTSTR pstr = strImage;
		int nRead = 0;
		TCHAR strItem[128];
		TCHAR strValue[128];

		RECT rcDest = { 0 }, rcSource = { 0 }, rcCorner = { 0 };

		bool bXtiled = false, bYtiled = false;
		ARGBREF refMask = 0xFF000000;
		DWORD dwFade = 255;
		bool bHole = false;
		TCHAR strImageFile[128];
		MUIIMAGEINFO* pImageInfo = NULL;

		strItem[0] = _T('\0'); strValue[0] = _T('\0'); strImageFile[0] = _T('\0');
		int iCount = 0;
		int nIndex = 0;
		int nStrLen = _tcslen(strImage);
		//for(int i =0; nStrLen > i ; i++)
		while (TRUE == ExtractInfo(pstr, strItem, strValue, &nRead))
		{
			//已经成功取得其中一个strItem与strValue，开始处理
			if (0 == _tcsicmp(strItem, _T("file")))
			{
				//file='filepath'
				//图片路径
				_tcscpy(strImageFile, strValue);
			}
			else if (0 == _tcsicmp(strItem, _T("corner")))
			{
				//conrner='0,0,0,0'
				//九格切图
				rcCorner = CMuiRect(strValue);
			}
			else if (0 == _tcsicmp(strItem, _T("dest")))
			{
				//dest='0,0,0,0'
				//图片相对于贴图区域的位置
				//空表示整个区域
				rcDest = CMuiRect(strValue);
			}
			else if (0 == _tcsicmp(strItem, _T("source")))
			{
				//source='0,0,0,0'
				//取源图片的哪个区域
				rcSource = CMuiRect(strValue);
			}
			else if (0 == _tcsicmp(strItem, _T("fade")))
			{
				//fade='255'
				//透明度,0表示完全透明
				dwFade = _tcstol(strValue, NULL, 10);
			}
			else if (0 == _tcsicmp(strItem, _T("mask")))
			{
				//屏蔽颜色，屏蔽后为透明
				refMask = CMuiColor(strValue);
			}
			else if (0 == _tcsicmp(strItem, _T("hole")))
			{
				//hole='false'
				//九格切图是否绘制中心点
				if (0 == _tcsicmp(strValue, _T("true")))
				{
					bHole = true;
				}
			}
			else if (0 == _tcsicmp(strItem, _T("xtiled")))
			{
				//xtiled='false'
				//横向拉伸图片
				//true = 不拉伸图片， false = 拉伸图片
				if (0 == _tcsicmp(strValue, _T("true")))
				{
					bXtiled = true;
				}
			}
			else if (0 == _tcsicmp(strItem, _T("ytiled")))
			{
				//ytiled='false'
				//纵向拉伸图片
				//true = 不拉伸图片， false = 拉伸图片
				if (0 == _tcsicmp(strValue, _T("true")))
				{
					bYtiled = true;
				}
			}
			else
			{
				//不知道是什么，抛出一个错误看看
				MUIASSERT(!strItem);
			}

			//重新取值，置零
			strItem[0] = strValue[0] = nIndex = 0;
			pstr += nRead;
		}/*while end*/

		if (_T('\0') == strImageFile[0])
		{
			_tcscpy(strImageFile, strImage);
		}

		pImageInfo = CSkinManager::GetImageInfo(m_strSkin, strImageFile);

		if (NULL == pImageInfo)
		{
			return false;
		}

		//先处理好位图的拉伸需求，再传递给绘制函数
		if (TRUE == IsEmptyRect(rcSource))
		{
			rcSource.right = pImageInfo->szBitmap.cx;
			rcSource.bottom = pImageInfo->szBitmap.cy;
		}

		if (TRUE == IsEmptyRect(rcDest))
		{
			rcDest = rcDraw;
		}
		else
		{
			OffsetRect(&rcDest, rcDraw.left, rcDraw.top);
		}

		if (true == bXtiled)
		{
			if (rcDest.right - rcDest.left > rcSource.right - rcSource.left)
			{
				rcDest.right = rcDest.left + rcSource.right - rcSource.left;
			}
			else
			{
				rcSource.right = rcSource.left + rcDest.right - rcDest.left;
			}
		}

		if (true == bYtiled)
		{
			if (rcDest.bottom - rcDest.top > rcSource.bottom - rcSource.top)
			{
				rcDest.bottom = rcDest.top + rcSource.bottom - rcSource.top;
			}
			else
			{
				rcSource.bottom = rcSource.top + rcDest.bottom - rcDest.top;
			}
		}

		return DrawImageImp(rcDest, pImageInfo, rcSource, rcCorner, refMask,
			dwFade, bHole);

	}

}



