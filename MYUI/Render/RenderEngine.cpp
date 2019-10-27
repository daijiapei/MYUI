
#include "RenderEngine.h"


namespace MYUI
{
    typedef struct __MYCARETINFO
    {
        HWND hWnd;
        HBITMAP Bitmap;
        POINT Pos;
        SIZE Size;
        BYTE Visible;
        BYTE Showing;
    } MYCARETINFO, *PMYCARETINFO;

    void DrawCaret(HWND hWnd, PMYCARETINFO CaretInfo)
    {
        HDC hdc, hdcMem;
        HBITMAP hbmOld;
        BOOL bDone = FALSE;

        hdc = GetDC(hWnd);
        if (!hdc) return;

        if (CaretInfo->Bitmap && GetBitmapDimensionEx(CaretInfo->Bitmap, &CaretInfo->Size))
        {
            hdcMem = CreateCompatibleDC(hdc);
            if (hdcMem)
            {
                hbmOld = (HBITMAP)SelectObject(hdcMem, CaretInfo->Bitmap);
                bDone = BitBlt(hdc,
                    CaretInfo->Pos.x,
                    CaretInfo->Pos.y,
                    CaretInfo->Size.cx,
                    CaretInfo->Size.cy,
                    hdcMem,
                    0,
                    0,
                    SRCINVERT);
                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);
            }
        }

        if (!bDone)
        {
            PatBlt(hdc,
                CaretInfo->Pos.x,
                CaretInfo->Pos.y,
                CaretInfo->Size.cx,
                CaretInfo->Size.cy,
                DSTINVERT);
        }

        ReleaseDC(hWnd, hdc);
    }

    CRenderEngine::CRenderEngine(HWND hWnd)
	{
		//��Windowƽ̨�У�����һ�����ھ��
        m_hWnd = hWnd;
	}

	CRenderEngine::~CRenderEngine()
	{
        m_hWnd = NULL;
        SetSkinFolder(NULL);
	}

    bool CRenderEngine::SetSkinFolder(LPCTSTR strSkin)
    {
        //���strSkin����գ�����ж��ԭ����Ƥ��
        if (strSkin && 0 == _tcsicmp(m_strSkin, strSkin)) return true;

        if (_T('\0') != m_strSkin[0])
        {
            //�Ѵ���Ƥ��
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
            if (m_hWnd && ::IsWindow(m_hWnd))
            {
                InvalidateRect(m_hWnd, NULL, FALSE);
            }
            return true;
        }
        return false;
    }

    LPCTSTR CRenderEngine::GetSkinFolder() const
    {
        return m_strSkin;
    }

	bool CRenderEngine::OnDrawImage(const RECT &rcDraw, LPCTSTR strImage)
	{
		int nSize = _tcslen(strImage);
		LPCTSTR pstr = strImage;
		int nRead = 0;
		TCHAR strItem[128];
		TCHAR strValue[128];

		RECT rcDest = {0}, rcSource = {0}, rcCorner = {0};

		bool bXtiled = false ,bYtiled = false;
		ARGBREF refMask = 0xFF000000; 
		DWORD dwFade = 255; 
		bool bHole = false;
		TCHAR strImageFile[128];
		IMAGEINFO * pImageInfo = NULL;

		strItem[0] = _T('\0'); strValue[0] = _T('\0'); strImageFile[0] = _T('\0');
		int iCount =0;
		int nIndex = 0;
		int nStrLen = _tcslen(strImage);
		//for(int i =0; nStrLen > i ; i++)
		while(TRUE == ExtractInfo(pstr, strItem, strValue, &nRead))
		{
			//�Ѿ��ɹ�ȡ������һ��strItem��strValue����ʼ����

			if(0 == _tcsicmp(strItem,_T("file")))
			{
				//file='filepath'
				//ͼƬ·��
				_tcscpy(strImageFile, strValue);
			}
			else if(0 == _tcsicmp(strItem,_T("corner")))
			{
				//conrner='0,0,0,0'
				//�Ÿ���ͼ
				rcCorner = CMuiRect(strValue);
			}
			else if(0 == _tcsicmp(strItem,_T("dest")))
			{
				//dest='0,0,0,0'
				//ͼƬ�������ͼ�����λ��
				//�ձ�ʾ��������
				rcDest = CMuiRect(strValue);
			}
			else if(0 == _tcsicmp(strItem,_T("source")))
			{
				//source='0,0,0,0'
				//ȡԴͼƬ���ĸ�����
				rcSource = CMuiRect(strValue);
			}
			else if(0 == _tcsicmp(strItem,_T("fade")))
			{
				//fade='255'
				//͸����,0��ʾ��ȫ͸��
				dwFade = _tcstol(strValue,NULL, 10);
			}
			else if(0 == _tcsicmp(strItem,_T("mask")))
			{
				//������ɫ�����κ�Ϊ͸��
				refMask = CMuiColor(strValue);
			}
			else if(0 == _tcsicmp(strItem,_T("hole")))
			{
				//hole='false'
				//�Ÿ���ͼ�Ƿ�������ĵ�
				if(0 == _tcsicmp(strValue, _T("true")))
				{
					bHole = true;
				}
			}
			else if(0 == _tcsicmp(strItem, _T("xtiled")))
			{
				//xtiled='false'
				//��������ͼƬ
				//true = ������ͼƬ�� false = ����ͼƬ
				if(0 == _tcsicmp(strValue, _T("true")))
				{
					bXtiled = true;
				}
			}
			else if(0 == _tcsicmp(strItem, _T("ytiled")))
			{
				//ytiled='false'
				//��������ͼƬ
				//true = ������ͼƬ�� false = ����ͼƬ
				if(0 == _tcsicmp(strValue, _T("true")))
				{
					bYtiled = true;
				}
			}
			else 
			{
				//��֪����ʲô���׳�һ�����󿴿�
				ASSERT(!strItem);
			}

			//����ȡֵ������
			strItem[0] = strValue[0] = nIndex = 0;
			pstr += nRead;
		}/*while end*/

		if(_T('\0') == strImageFile[0])
		{
			_tcscpy(strImageFile, strImage);
		}

		pImageInfo = CSkinManager::GetImageInfo(m_strSkin, strImageFile);

		if(NULL == pImageInfo)
		{
			return false;
		}

		//�ȴ����λͼ�����������ٴ��ݸ����ƺ���
		if(TRUE == IsEmptyRect(rcSource))
		{
			rcSource.right = pImageInfo->szBitmap.cx;
			rcSource.bottom = pImageInfo->szBitmap.cy;
		}

		if(TRUE == IsEmptyRect(rcDest))
		{
			rcDest = rcDraw;
		}
		else
		{
			OffsetRect(&rcDest, rcDraw.left, rcDraw.top);
		}

		if(true == bXtiled)
		{
			if(rcDest.right - rcDest.left > rcSource.right - rcSource.left)
			{
				rcDest.right = rcDest.left + rcSource.right - rcSource.left;
			}
			else
			{
				rcSource.right = rcSource.left + rcDest.right - rcDest.left;
			}
		}

		if(true == bYtiled)
		{
			if(rcDest.bottom - rcDest.top > rcSource.bottom - rcSource.top)
			{
				rcDest.bottom = rcDest.top + rcSource.bottom - rcSource.top;
			}
			else
			{
				rcSource.bottom = rcSource.top + rcDest.bottom - rcDest.top;
			}
		}
		
		return OnDrawImage(rcDest, pImageInfo, rcSource, rcCorner, refMask,
			dwFade, bHole);
		
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

	
}