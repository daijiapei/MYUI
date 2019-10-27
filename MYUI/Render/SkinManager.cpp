
#include "SkinManager.h"
#include "..\\Utils\\stb_image.h"
#include "..\\Utils\\XUnzip.h"

namespace MYUI
{
	CMuiPtrArray CSkinManager::s_SkinArray;
	CMuiPtrArray CSkinManager::s_Resource;
	RSRCID CSkinManager::s_ResourceIndex = 1;
	HINSTANCE CSkinManager::s_hInstance = NULL;

	void CSkinManager::SetInstance(HINSTANCE hInstance)
	{
		s_hInstance = hInstance;
	}

	int CSkinManager::AddSkin(LPCTSTR strSkin)
	{
		int nCount = 0;
        SKINFOLDERNODE * pSkinNode = NULL;
		TCHAR strItem[128];
		TCHAR strValue[128];
		LPCTSTR pstr = strSkin;
		int nRead = 0;

		for(int i=0; s_SkinArray.GetSize() > i; i++)
		{
            pSkinNode = (SKINFOLDERNODE*)s_SkinArray.GetAt(i);

			if(!_tcsicmp(strSkin, pSkinNode->strSkin))
			{
				//�Ѵ���,�������ü���
				nCount = ++pSkinNode->nCount;
				break;
			}
		}

		if(0 == nCount)
		{
			//������
            pSkinNode = new SKINFOLDERNODE;
            memset(pSkinNode, 0, sizeof(SKINFOLDERNODE));
			nCount = pSkinNode->nCount = 1;
			_tcscpy(pSkinNode->strSkin, strSkin);

			while(TRUE == ExtractInfo(pstr, strItem, strValue, &nRead))
			{
				if(0 == _tcsicmp(strItem,_T("file")) ||
					0 == _tcsicmp(strItem,_T("folder")))
				{
					_tcscpy(pSkinNode->strSkinFolder, strValue);
				}
				else if(0 == _tcsicmp(strItem,_T("resid")))
				{
					pSkinNode->resId = _tcstol(strValue,NULL, 10);
				}
				else if(0 == _tcsicmp(strItem,_T("restype")))
				{
					_tcscpy(pSkinNode->resType, strValue);
				}
				else
				{
					ASSERT(0 == strItem);
				}
				pstr += nRead;
			}

			if(_T('\0') == pSkinNode->strSkinFolder[0] &&
				0 == pSkinNode->resId && _T('\0') == pSkinNode->resType[0])
			{
				_tcscpy(pSkinNode->strSkinFolder, strSkin);
			}

			pSkinNode->resIndex = 0;
			if(pSkinNode->resId && pSkinNode->resType[0])
			{
				pSkinNode->resIndex = CSkinManager::AddResource(pSkinNode->resId, pSkinNode->resType);
			}

			s_SkinArray.Add(pSkinNode);
		}
		return nCount;
	}

	int CSkinManager::RemoveSkin(LPCTSTR strSkin)
	{
		int nCount = -1;
        SKINFOLDERNODE * pSkinNode = NULL;
		for(int i=0; s_SkinArray.GetSize() > i; i++)
		{
            pSkinNode = (SKINFOLDERNODE*)s_SkinArray.GetAt(i);
			if(!_tcscmp(strSkin, pSkinNode->strSkinFolder))
			{
				//�Ѵ���
				nCount = --pSkinNode->nCount;
				break;
			}
		}
		if(0 == nCount)
		{
			if(pSkinNode->resIndex)
			{
				CSkinManager::RemoveResource(pSkinNode->resIndex);
			}

            rbtree_enum(&pSkinNode->ImageCache, RBTREE_ENUM_MID, CSkinManager::ClearImage, NULL);

			s_SkinArray.Remove(pSkinNode);
		}
		return nCount;
	}

    int __stdcall CSkinManager::ClearImage(RBNODE * node, void * param)
    {
        IMAGEINFO * pImage = (IMAGEINFO *)node->data;
        DeleteObject(pImage->hBitmap);
        delete pImage;
        return 0;//����true ��������
    }

	RSRCID CSkinManager::AddResource(int nID, LPCTSTR lpType)
	{
        RESOURCEINFO * pResInfo = NULL;
		RSRCID index = 0;
		for(int i=0; s_Resource.GetSize() > i; i++)
		{
            RESOURCEINFO * pInfo = (RESOURCEINFO *)s_Resource[i];
			if(pInfo->nID == nID && !_tcsicmp(lpType,pInfo->lpType))
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
			HRSRC hResource = ::FindResource(CSkinManager::s_hInstance , MAKEINTRESOURCE(nID), lpType);
			if( hResource == NULL ) break;
			HGLOBAL hGlobal = ::LoadResource(CSkinManager::s_hInstance, hResource);
			if( hGlobal == NULL ) {
				::FreeResource(hResource);
				break;
			}

			DWORD dwSize = ::SizeofResource(CSkinManager::s_hInstance , hResource);
			if( dwSize == 0 ) break;
			PBYTE pData = new BYTE[ dwSize ];
			::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
			UnlockResource(hGlobal);//��ʵ���Բ�Ҫ���
			::FreeResource(hResource);


            pResInfo = new RESOURCEINFO;
			pResInfo->nID = nID;
			pResInfo->buffet = pData;
			pResInfo->bufsize = dwSize;
			index = pResInfo->index = s_ResourceIndex ++ ;
			pResInfo->nCount = 1;
			_tcscpy(pResInfo->lpType, lpType);
			s_Resource.Add(pResInfo);
			break;
		}

		if(s_ResourceIndex == -1)//s_ResourceID����Ϊ0
        {
            s_ResourceIndex = 1;
        }

return index;
    }

    void CSkinManager::RemoveResource(RSRCID id)
    {
        for (int i = 0; s_Resource.GetSize() > i; i++)
        {
            RESOURCEINFO * pInfo = (RESOURCEINFO *)s_Resource[i];
            if (pInfo->index == id)
            {
                --pInfo->nCount;

                if (0 == pInfo->nCount)
                {
                    s_Resource.Remove(pInfo);
                    delete pInfo->buffet;
                    delete pInfo;
                }
                break;
            }

        }
    }

    void CSkinManager::RemoveAllResource()
    {
    }

    //��Ƥ���в���һ���Զ���ͼƬ��һ����������ڴ��е�ͼƬ
    //@strSkin ͼƬ�ŵ��Ǹ�Ƥ����
    //@pImageInfo ͼƬ��Ϣ
    bool CSkinManager::AddImage(LPCTSTR strSkin, IMAGEINFO * pImageInfo)
    {
        SKINFOLDERNODE * pSkinNode = NULL;
        RBKEY Key = { 0 };
        if (NULL == pImageInfo || _T('\0') == pImageInfo->strFile[0]) return false;

        for (int i = 0; s_SkinArray.GetSize() > i; i++)
        {
            SKINFOLDERNODE * node = (SKINFOLDERNODE*)s_SkinArray.GetAt(i);
            if (!_tcsicmp(strSkin, node->strSkin))
            {
                pSkinNode = node;
                break;
            }
        }

        if (NULL == pSkinNode) return false;

        _tcscpy(Key.strText, pImageInfo->strFile);

        RBNODE * pTreeNode = rbtree_create_node(&Key, pImageInfo);

        if (0 != rbtree_insert(&pSkinNode->ImageCache, pTreeNode))
        {
            TRACE(_T("CSkinManager::AddImage : %s �ļ��Ѿ����ڣ�"), pImageInfo->strFile);
            ASSERT(0 && "CSkinManager::AddImage : �ļ��Ѿ�����,�붨����ʵ��ļ����ƣ�");
            rbtree_free_node(pTreeNode);
            return false;
        }

        return true;
    }

    bool CSkinManager::RemoveImage(LPCTSTR strSkin, LPCTSTR strImageFile)
    {
        SKINFOLDERNODE * pSkinNode = NULL;
        RBKEY Key = { 0 };
        if (NULL == strImageFile || _T('\0') == strImageFile[0]) return false;

        for (int i = 0; s_SkinArray.GetSize() > i; i++)
        {
            SKINFOLDERNODE * node = (SKINFOLDERNODE*)s_SkinArray.GetAt(i);
            if (!_tcsicmp(strSkin, node->strSkin))
            {
                pSkinNode = node;
                break;
            }
        }

        if (NULL == pSkinNode) return false;

        _tcscpy(Key.strText, strImageFile);

        RBNODE * pTreeNode = rbtree_remove(&pSkinNode->ImageCache, &Key);

        if (pTreeNode)
        {
            IMAGEINFO * pImage = (IMAGEINFO *)pTreeNode->data;
            DeleteObject(pImage->hBitmap);
            delete pImage;
            rbtree_free_node(pTreeNode);
        }
        return !!pTreeNode;
    }

    IMAGEINFO * CSkinManager::GetImageInfo(LPCTSTR strSkin, LPCTSTR strImageFile)
    {
        SKINFOLDERNODE * pSkinNode = NULL;
        IMAGEINFO * pImage = NULL;
        RBKEY Key = { 0 };
        for (int i = 0; s_SkinArray.GetSize() > i; i++)
        {
            SKINFOLDERNODE * node = (SKINFOLDERNODE*)s_SkinArray.GetAt(i);
            if (!_tcscmp(strSkin, node->strSkin))
            {
                pSkinNode = node;
                break;
            }
        }

        //û�����Ƥ��
        if (NULL == pSkinNode) return NULL;

        _tcscpy(Key.strText, strImageFile);

        RBNODE* pTreeNode = rbtree_find(&pSkinNode->ImageCache, &Key);

        if (pTreeNode)
        {
            pImage = (IMAGEINFO*)rbtree_node_data(pTreeNode);
            return pImage;
        }

        //û�ҵ���Ӧ��ͼƬ��������
        pImage = CSkinManager::LoadImage(pSkinNode->strSkinFolder, pSkinNode->resIndex, strImageFile);

        if (NULL == pImage)
        {
            TRACE(_T("CSkinManager::GetImageInfo : %s �ļ�����ʧ�ܣ�"), strImageFile);
        }

        //���ܼ��سɹ���ʧ�ܣ���Ȼ�ŵ����У�Ϊ�˱��⻺�����
        pTreeNode = rbtree_create_node(&Key, pImage);

        if (0 != rbtree_insert(&pSkinNode->ImageCache, pTreeNode))
        {
            rbtree_free_node(pTreeNode);
        }
		
		return pImage;
	}

    IMAGEINFO * CSkinManager::LoadImage(LPCTSTR strSkinFolder, RSRCID resIndex, LPCTSTR strImageFile)
	{
        IMAGEINFO * pImageInfo = NULL;
		BYTE * pData = NULL;
		DWORD dwSize = 0;
		LPBYTE pImage = NULL;
		int x,y,n;
		bool bAlpha = false;
		//���ļ��м���ͼƬ����
		if(false == CSkinManager::LoadFileData(strSkinFolder, resIndex , strImageFile, &pData, &dwSize))
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

        pImageInfo = new IMAGEINFO;
		pImageInfo->hBitmap = hBitmap;
		pImageInfo->szBitmap.cx = x;
		pImageInfo->szBitmap.cy = y;
		pImageInfo->bAlpha = bAlpha;
		_tcscpy(pImageInfo->strFile, strImageFile);
		return pImageInfo;
		
	}

	bool CSkinManager::LoadFile(LPCTSTR strSkin,LPCTSTR strFile,BYTE ** pFileData, DWORD * pdwDataSize)
	{
		TCHAR strItem[128] = {0};
		TCHAR strValue[128] = {0};
		LPCTSTR pstr = strSkin;
		int nRead = 0;

		TCHAR strFolder[128] = {0};
		TCHAR resType[128] = {0};
		int resId = 0;
		RSRCID nIndex = 0;

		while(TRUE == ExtractInfo(pstr, strItem, strValue, &nRead))
		{
			if(0 == _tcsicmp(strItem,_T("file")) ||
				0 == _tcsicmp(strItem,_T("folder")))
			{
				_tcscpy(strFolder, strValue);
			}
			else if(0 == _tcsicmp(strItem,_T("resid")))
			{
				resId = _tcstol(strValue,NULL, 10);
			}
			else if(0 == _tcsicmp(strItem,_T("restype")))
			{
				_tcscpy(resType, strValue);
			}
			else
			{
				ASSERT(0 == strItem);
			}
			pstr += nRead;
		}

		if(_T('\0') == strFolder[0] && 0 == resId && _T('\0') == resType[0])
		{
			_tcscpy(strFolder, strSkin);
		}

		if(resId || resType[0])
		{
			for(int i=0; s_SkinArray.GetSize() > i; i++)
			{
                SKINFOLDERNODE * pSkinNode = (SKINFOLDERNODE*)s_SkinArray.GetAt(i);
				if(!_tcscmp(strSkin, pSkinNode->strSkin))
				{
					nIndex = pSkinNode->resIndex;
				}
			}
		}

		return LoadFileData(strFolder, nIndex, strFile, pFileData, pdwDataSize);

	}

	bool CSkinManager::LoadFileData(LPCTSTR strSkinFolder, RSRCID resIndex, LPCTSTR strFile,BYTE ** pFileData, DWORD *pdwDataSize)
	{
		//�ȴ���Դ�ļ��е�ZIP�ļ�����
		//ʧ���ٵ������е�ZIP�ļ�����
		//ʧ���ٵ�����Ŀ¼���ļ��в���
		//���߶�ʧ�ܷ���NULL��
		//strSkin ������Ŀ¼��Ҳ������һ��ZIP�ļ�
		TCHAR strCompletePath[MAX_PATH] = {0};
		HZIP hz = NULL;
		PBYTE pData = NULL;
		DWORD dwSize = 0;
		int nItem = 0;
        RESOURCEINFO * pResInfo = NULL;

#ifndef _UNICODE
		ZIPENTRY ze; 
#else
		ZIPENTRYW ze;
#endif
		_tcscat(strCompletePath, strSkinFolder);
		_tcscat(strCompletePath, strFile);
		//zip�ļ���֧��..\\[�ϲ�Ŀ¼],����Ҫ��strCompletePath�����ϲ�Ŀ¼����
		//skin/folder/../file.txt ת�� skin/file.txt
		int nPathLen = _tcslen(strCompletePath);
		for(int i = 0; _T('\0') != strCompletePath[i]; i++)
		{
			//�� \\ �滻�� /
			if(_T('\\') == strCompletePath[i]) strCompletePath[i] = _T('/');

			if(_T('.') == strCompletePath[i] && _T('.') == strCompletePath[i + 1] &&
				_T('/') == strCompletePath[i + 2])
			{
				//�����ϲ�Ŀ¼�����кϲ�
				if(0 == i) break; //�Ѿ��ǵ�һ�㣬û���ϲ�
				int k = i, nCount = 0;
				while(strCompletePath[k] && k >= 0)//�ҳ��ϲ�ϲ���
				{
					if(_T('/') == strCompletePath[k]) nCount ++;

					if(2 == nCount) break;

					k --;
				}
				//�ϲ�
				memcpy(&strCompletePath[k], &strCompletePath[i + 2], (nPathLen - (i + 2) + 1) * sizeof(TCHAR));
				nPathLen -= (i + 2) - k;
				i = k;
			}
		}

		//�ȴ���Դ�ļ��е�ZIP�ļ�����
		for(int i=0; s_Resource.GetSize() > i && resIndex; i++)
		{
            RESOURCEINFO * pTmp = (RESOURCEINFO*)s_Resource[i];
			if(pTmp->index == resIndex) 
			{
				pResInfo = pTmp;
				break;
			}
		}
		
		while(pResInfo)
		{
			hz = OpenZipU(pResInfo->buffet, pResInfo->bufsize, ZIP_MEMORY);
			if( hz == NULL ) break;//��ʧ��
			
			if(NULL != FindZipItem(hz, strCompletePath , true, &nItem, &ze))
			{
				//û�ҵ��ļ�
				break;
			}

			dwSize = ze.unc_size;
			if( dwSize == 0 ) break;
			pData = new BYTE[ dwSize ];
			int res = UnzipItem(hz, nItem, pData, dwSize, ZIP_MEMORY);
			CloseZip(hz);
			if( res != ZR_OK && res != ZR_MORE) 
			{
				//��ѹʧ��
				delete[] pData;
				pData = NULL;
			}
			break;//ֻѭ��һ��
			//��Ƥ��������Դ�ļ�����������Դ�в��ң���Դ��û���ҵ���
			//���������̲��ң���������������̲��ң��뽫break�滻��return
		}

		//ʧ���ٵ������е�ZIP�ļ�����
		while(NULL == pData)
		{
			hz = OpenZipU((void*)strSkinFolder, 0 , ZIP_FILENAME);
			if(NULL == hz) break;//��ʧ��
			
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
			if( dwSize == 0 )
			{
				::CloseHandle( hFile );
				break;
			}

			DWORD dwRead = 0;
			pData = new BYTE[dwSize];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				pData = NULL;
			}
			break;//ֻѭ��һ��
		}

		if(pData)
		{
			*pFileData = pData;
			*pdwDataSize = dwSize;
			return true;
		}

        *pFileData = NULL;
        *pdwDataSize = 0;
		return false;
	}

}