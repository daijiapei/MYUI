
#include "UIRender.h"
#include "..\\Utils\\stb_image.h"
#include "..\\Utils\\XUnzip.h"

#ifndef _UNICODE
#define _pathcmp  stricmp
#define _pathcpy  strcmp
#else 
#define _pathcmp _wcsicmp
#define _pathcpy wcscpy
#endif

namespace MYUI
{
	CRenderEngine::CRenderEngine()
	{
	}

	CRenderEngine::~CRenderEngine()
	{
	}

	CMuiPtrArray CRenderEngine::g_SkinArray;
	CMuiPtrArray CRenderEngine::g_Resource;
	RSRCID CRenderEngine::g_ResourceIndex = 1;

	HRGN CRenderEngine::BeginPaint(HDC hPaintDc, const RECT &rcItem)
	{
		RECT rcClip;
		RECT rcPaint;
		::GetClipBox(hPaintDc, &rcClip);
		if(false == ::IntersectRect(&rcPaint,&rcClip, &rcItem))
		{
			return NULL;
		}
		HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
		HRGN hRgnItem = ::CreateRectRgnIndirect(&rcPaint);

		(HRGN)SelectClipRgn(hPaintDc, hRgnItem);

		return hOldRgn;
	}

	void CRenderEngine::EndPaint(HDC hPaintDc, HRGN hOldRgn)
	{
		SelectClipRgn(hPaintDc, hOldRgn);
	}

	int CRenderEngine::AddSkin(LPCTSTR strSkin)
	{
		int nCount = 0;
		SkinFolderNode * pSkinNode;
		for(int i=0; g_SkinArray.GetSize() > i; i++)
		{
			pSkinNode = (SkinFolderNode*)g_SkinArray.GetAt(i);
			if(!_pathcmp(strSkin, pSkinNode->strSkinFolder))
			{
				//�Ѵ���
				nCount = ++pSkinNode->nCount;
				break;
			}
		}

		if(0 == nCount)
		{
			//������
			pSkinNode = new SkinFolderNode;
			nCount = pSkinNode->nCount = 1;
			pSkinNode->ImageCache = new CAvlTree();
			_pathcpy(pSkinNode->strSkinFolder, strSkin);
			g_SkinArray.Add(pSkinNode);
		}
		return nCount;
	}

	int CRenderEngine::RemoveSkin(LPCTSTR strSkin)
	{
		int nCount = -1;
		SkinFolderNode * pSkinNode = NULL;
		for(int i=0; g_SkinArray.GetSize() > i; i++)
		{
			pSkinNode = (SkinFolderNode*)g_SkinArray.GetAt(i);
			if(!_pathcmp(strSkin, pSkinNode->strSkinFolder))
			{
				//�Ѵ���
				nCount = --pSkinNode->nCount;
				break;
			}
		}
		if(0 == nCount)
		{
			pSkinNode->ImageCache->enum_node(CAvlTree::EnumNodeType::preType, ClearSkin);
			delete pSkinNode->ImageCache;
			g_SkinArray.Remove(pSkinNode);
		}
		return nCount;
	}

	bool CRenderEngine::ClearSkin(AvlTreeNode * node)
	{
		TImageInfo * pImage = (TImageInfo *)node->pData;
		DeleteObject(pImage->hBitmap);
		delete pImage;
		return false;//����true ��������
	}

	RSRCID CRenderEngine::AddResource(HINSTANCE hInstatce, int nID, LPCTSTR lpType)
	{
		TResourceInfo * pResInfo = NULL;
		RSRCID index = 0;
		for(int i=0; g_Resource.GetSize() > i; i++)
		{
			TResourceInfo * pInfo = (TResourceInfo * )g_Resource[i];
			if(pInfo->hInstance == hInstatce && pInfo->nID == nID
				&& !_pathcmp(lpType,pInfo->lpType))
			{
				pResInfo = pInfo;
				++pResInfo->nCount;
				index = pResInfo->index;
				break;
			}

		}

		while(!pResInfo)
		{
			//����Դȡ
			HRSRC hResource = ::FindResource(hInstatce , MAKEINTRESOURCE(nID), lpType);
			if( hResource == NULL ) break;
			HGLOBAL hGlobal = ::LoadResource(hInstatce, hResource);
			if( hGlobal == NULL ) {
				FreeResource(hResource);
				break;
			}

			DWORD dwSize = ::SizeofResource(hInstatce , hResource);
			if( dwSize == 0 ) break;
			PBYTE pData = new BYTE[ dwSize ];
			::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
			::FreeResource(hResource);


			pResInfo = new TResourceInfo;
			pResInfo->hInstance = hInstatce;
			pResInfo->nID = nID;
			pResInfo->buffet = pData;
			pResInfo->bufsize = dwSize;
			index = pResInfo->index = ++ g_ResourceIndex ;
			pResInfo->nCount = 1;
			_pathcmp(pResInfo->lpType, lpType);
			g_Resource.Add(pResInfo);
			break;
		}

		if(g_ResourceIndex == -1)//g_ResourceID����Ϊ0
		{
			g_ResourceIndex = 1;
		}

		return index;
	}

	void CRenderEngine::RemoveResource(RSRCID id)
	{
		for(int i=0; g_Resource.GetSize() > i; i++)
		{
			TResourceInfo * pInfo = (TResourceInfo * )g_Resource[i];
			if(pInfo->index ==  id)
			{
				-- pInfo->nCount;

				if(0 == pInfo->nCount)
				{
					g_Resource.Remove(pInfo);
					delete pInfo->buffet;
					delete pInfo;
				}
				break;
			}

		}
	}

	void CRenderEngine::RemoveAllResource()
	{
	}

	TImageInfo * CRenderEngine::GetImageInfo(LPCTSTR strSkin, LPCTSTR strImageName)
	{
		SkinFolderNode * pSkinNode = NULL;
		TImageInfo * pImage = NULL;

		for(int i=0; g_SkinArray.GetSize() > i; i++)
		{
			SkinFolderNode * node = (SkinFolderNode*)g_SkinArray.GetAt(i);
			if(!_pathcmp(strSkin, pSkinNode->strSkinFolder))
			{
				pSkinNode = node;
				break;
			}
		}

		//û�����Ƥ��
		if(NULL == pSkinNode) return NULL;

		pImage = (TImageInfo*)pSkinNode->ImageCache->find(strImageName);

		//�ҵ���Ӧ��ͼƬ������
		if(pImage) return pImage;

		//û�ҵ���Ӧ��ͼƬ��������
		pImage = CRenderEngine::LoadImage(strSkin,strImageName);
		if(pImage)
		{
			//���سɹ������뵽����
			pSkinNode->ImageCache->insert(strImageName, pImage);
		}
		
		return pImage;

	}

	bool CRenderEngine::LoadImageData(LPCTSTR strSkin, LPCTSTR strImageName,BYTE ** pImageData, DWORD &dwDataSize)
	{
		//�ȴ���Դ�ļ��е�ZIP�ļ�����
		//ʧ���ٵ������е�ZIP�ļ�����
		//ʧ���ٵ�����Ŀ¼���ļ��в���
		//���߶�ʧ�ܷ���NULL��
		//strSkin ������Ŀ¼��Ҳ������һ��ZIP�ļ�
		TCHAR strCompletePath[MAX_PATH] = {0};
		TImageInfo * pImageInfo = NULL;
		HZIP hz = NULL;
		PBYTE pData = NULL;
		DWORD dwSize = 0;
		int nItem = 0;

#ifndef _UNICODE
		ZIPENTRY ze; 
		strcat(strCompletePath, strSkin);
		strcat(strCompletePath, strImageName);
#else
		ZIPENTRYW ze;
		wcscat(strCompletePath, strSkin);
		wcscat(strCompletePath, strImageName);
#endif
		//�ȴ���Դ�ļ��е�ZIP�ļ�����
		for(int i=0; g_Resource.GetSize() > i && NULL == pData; i++)
		{
			TResourceInfo * pResInfo = (TResourceInfo*) g_Resource[i];
			hz = OpenZipU(pResInfo->buffet, pResInfo->bufsize, ZIP_MEMORY);
			if( hz == NULL ) continue;//��ʧ��
			
			if( FindZipItem(hz, strCompletePath , true, &nItem, &ze) != 0 )
			{
				//û�ҵ��ļ�
				continue;
			}

			dwSize = ze.unc_size;
			if( dwSize == 0 ) continue;
			pData = new BYTE[ dwSize ];
			int res = UnzipItem(hz, nItem, pData, dwSize, ZIP_MEMORY);
			CloseZip(hz);
			if( res != 0x00000000 && res != 0x00000600) 
			{
				//��ѹʧ��
				delete[] pData;
				pData = NULL;
			}
		}

		//ʧ���ٵ������е�ZIP�ļ�����
		while(NULL == pData)
		{
			hz = OpenZipU((void*)strSkin, 0 , ZIP_FILENAME);
			if( hz == NULL ) break;//��ʧ��
			
			if( FindZipItem(hz, strCompletePath , true, &nItem, &ze) != 0 )
			{
				//û�ҵ��ļ�
				break;
			}

			dwSize = ze.unc_size;
			if( dwSize == 0 ) break;
			pData = new BYTE[ dwSize ];
			int res = UnzipItem(hz, nItem, pData, dwSize, ZIP_FILENAME);
			CloseZip(hz);
			if( res != 0x00000000 && res != 0x00000600) 
			{
				//��ѹʧ��
				delete[] pData;
				pData = NULL;
			}
			break;//ֻѭ��һ��
		}
		
		//ʧ���ٵ�����Ŀ¼���ļ��в���
		while(NULL == pData)
		{
			HANDLE hFile = ::CreateFile(strCompletePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) break;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) break;

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				pData = NULL;
			}
			break;
		}

		if(pData)
		{
			*pImageData = pData;
			dwDataSize = dwSize;
			return true;
		}

		return false;
	}

	TImageInfo * CRenderEngine::LoadImage(LPCTSTR strSkin, LPCTSTR strImageName)
	{
		TImageInfo * pImageInfo = NULL;
		BYTE * pData = NULL;
		DWORD dwSize = 0;
		LPBYTE pImage = NULL;
		int x,y,n;
		bool bAlpha = false;
		//���ļ��м���ͼƬ����
		if(false == LoadImageData(strSkin, strImageName, &pData, dwSize))
		{
			return NULL;
		}
		
		pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
		delete[] pData;
		//ͼƬ����ʧ��
		if( NULL == pImage ) return NULL;

		BITMAPINFO bmi;
		::ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = x;
		bmi.bmiHeader.biHeight = -y;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = x * y * 4;

		
		LPBYTE pDest = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
		if( NULL == hBitmap ) return NULL;

		for( int i = 0; i < x * y; i++ ) 
		{
			pDest[i*4 + 3] = pImage[i*4 + 3];
			//���ص���ͼƬ�Ƿ������ģ����Խ���λ�öԵ�
			// ARGB��  0λ=B; 1=G; 2=R; 3=A(͸��)
			if( pDest[i*4 + 3] < 255 ) 
			{
				pDest[i*4] = (BYTE)(DWORD(pImage[i*4 + 2])*pImage[i*4 + 3]/255);
				pDest[i*4 + 1] = (BYTE)(DWORD(pImage[i*4 + 1])*pImage[i*4 + 3]/255);
				pDest[i*4 + 2] = (BYTE)(DWORD(pImage[i*4])*pImage[i*4 + 3]/255); 
				bAlpha = true;//����͸����־
			}
			else
			{
				pDest[i*4] = pImage[i*4 + 2];
				pDest[i*4 + 1] = pImage[i*4 + 1];
				pDest[i*4 + 2] = pImage[i*4]; 
			}

			////��ɫ = mask ������Ϊ0 ����������Ϊ͸��
			//if( *(DWORD*)(&pDest[i*4]) == mask ) {
			//	pDest[i*4] = (BYTE)0;
			//	pDest[i*4 + 1] = (BYTE)0;
			//	pDest[i*4 + 2] = (BYTE)0; 
			//	pDest[i*4 + 3] = (BYTE)0;
			//	bAlphaChannel = true;
			//}
		}

		stbi_image_free(pImage);

		pImageInfo = new TImageInfo;
		pImageInfo->hBitmap = hBitmap;
		pImageInfo->bmpSize.cx = x;
		pImageInfo->bmpSize.cy = y;
		pImageInfo->bAlpha = bAlpha;
		_pathcmp(pImageInfo->strFile, strImageName);
		return pImageInfo;
		
	}

	bool CRenderEngine::DrawImage(HDC hMemDc,const RECT * rcItem, LPCTSTR strSkin, LPCTSTR strImageName, 
			const RECT * rcSource, COLORREF mask)
	{
		bool bRet = false;
		TImageInfo * pImageInfo = CRenderEngine::LoadImage(strSkin, strImageName);
		if(NULL == pImageInfo) return bRet;

		HDC hTempDc = CreateCompatibleDC(hMemDc);
		if(NULL == hTempDc) return bRet;
		SelectObject(hTempDc, pImageInfo->hBitmap);

		if(NULL == rcSource)
		{
			bRet = StretchBlt(hMemDc, rcItem->left, rcItem->top, \
				rcItem->right - rcItem->left, rcItem->bottom - rcItem->top,
				hTempDc, 0,0,pImageInfo->bmpSize.cx ,pImageInfo->bmpSize.cy, SRCCOPY);
		}
		else
		{
			bRet = StretchBlt(hMemDc, rcItem->left, rcItem->top, \
				rcItem->right - rcItem->left, rcItem->bottom - rcItem->top,
				hTempDc, rcSource->left, rcSource->top, \
				rcSource->right - rcSource->left, rcSource->bottom - rcSource->top,
				SRCCOPY);
		}
		

		DeleteObject(hTempDc);
		return bRet;
	}

	void CRenderEngine::DrawColor(HDC hMenDc, HBRUSH hBrush,const RECT &rcItem)
	{
		if(hBrush) FillRect(hMenDc, &rcItem, hBrush);
		/* ::SetBkColor(hDC, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));*/
        //::ExtTextOut(hMenDc, 0, 0, ETO_OPAQUE, &rcItem, NULL, 0, NULL);
	};

	void CRenderEngine::DrawText(HDC hMenDc,HFONT hFont, ARGBREF argb,const RECT &rcItem, CMuiString strText, DWORD dwType)
	{
		HFONT hOldFont = NULL;
		int bkMode = SetBkMode(hMenDc,TRANSPARENT);
		if(hFont) hOldFont = (HFONT)SelectObject(hMenDc, hFont);
		COLORREF OldTextColor = SetTextColor(hMenDc,GetRGBValue(argb));
		::DrawText(hMenDc, strText.GetData(), -1,  (LPRECT)&rcItem, dwType);
		SetTextColor(hMenDc,OldTextColor);
		if(hFont) SelectObject(hMenDc, hOldFont);
		SetBkMode(hMenDc,bkMode);
	}

	void CRenderEngine::DrawBroder(HDC hMenDc, HPEN hPen,const RECT &rcItem)
	{
		HPEN hPenOld = NULL;
		if(hPen) hPenOld = (HPEN) SelectObject(hMenDc,hPen);
		Rectangle(hMenDc, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
		if(hPen) SelectObject(hMenDc,hPenOld);
	}

	void CRenderEngine::DrawRoundRect(HDC hMenDc,HPEN hPen, const RECT& rcItem, int width, int height)
	{
		ASSERT(::GetObjectType(hMenDc)==OBJ_DC || ::GetObjectType(hMenDc)==OBJ_MEMDC);
		//HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, GetRGBValue(dwPenColor));
		HPEN hOldPen = (HPEN)::SelectObject(hMenDc, hPen);
		::SelectObject(hMenDc, ::GetStockObject(HOLLOW_BRUSH));
		//::RoundRect(hMenDc, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom, width, height);
		::Rectangle(hMenDc,rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
		::SelectObject(hMenDc, hOldPen);
		//::DeleteObject(hPen);

	}

	BOOL CRenderEngine::CloneMemoryToPaint(HDC hPaintDc,const RECT & rcPaintFixed, 
		HDC hMenDc,const RECT &rcItem ,DWORD alpha)
	{
		//bFloat : TRUE ����λ����ʾ

		BLENDFUNCTION blend = {0};//����͸��Ч��
		blend.BlendOp= AC_SRC_OVER;
		blend.SourceConstantAlpha= (BYTE)alpha; // ͸���� ���255L 

		//RECT rcClip = {0};
		//::GetClipBox(hPaintDc, &rcClip);
		//HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);

		/*HRGN hRgnItem = ::CreateRoundRectRgn(rcPaintFixed.left + rcItem.left + OffsetSize.cx,
			rcPaintFixed.top + rcItem.top + OffsetSize.cy, 
			rcPaintFixed.left + rcItem.left + OffsetSize.cx + (rect.right - rect.left) ,
			rcPaintFixed.top + rcItem.top + OffsetSize.cy + (rect.bottom - rect.top), 
			30, 30);*/


		//HRGN hOldRgn = (HRGN)SelectObject(hPaintDc, hRgnItem);
		//SelectClipRgn(hPaintDc,hRgnItem);
		//::ExtSelectClipRgn(hMenDc, hRgnItem , RGN_AND);
		
		//return BitBlt(hPaintDc, 
		//	rcPaintFixed.left, rcPaintFixed.top, 
		//	rcPaintFixed.right - rcPaintFixed.left ,rcPaintFixed.bottom - rcPaintFixed.top, 
		//	hMenDc, rcItem.left , rcItem.top, SRCCOPY);
		AlphaBlend(hPaintDc ,
			rcPaintFixed.left, rcPaintFixed.top, 
			rcPaintFixed.right - rcPaintFixed.left ,rcPaintFixed.bottom - rcPaintFixed.top, 
			hMenDc ,
			rcItem.left, rcItem.top, 
			rcItem.right - rcItem.left ,rcItem.bottom - rcItem.top,
			blend);
		 
		 //DeleteObject(hRgnItem);
		 //SelectObject(hPaintDc, hOldRgn);
		 //SelectClipRgn(hPaintDc,hOldRgn);
		 return true;
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