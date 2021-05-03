
#include "SkinManager.h"
#include "..\\Utils\\stb_image.h"
#include "..\\Utils\\XUnzip.h"

namespace MYUI
{
	CMuiPtrArray CSkinManager::s_SkinArray;
	CMuiPtrArray CSkinManager::s_Resource;
	MUIRESID CSkinManager::s_ResourceIndex = 1;
	HINSTANCE CSkinManager::s_hInstance = NULL;

	void CSkinManager::SetInstance(HINSTANCE hInstance)
	{
		s_hInstance = hInstance;
	}

	int CSkinManager::AddSkin(LPCTSTR strSkin)
	{
		int nCount = 0;
		MUISKINFOLDERNODE* pSkinNode = NULL;
		TCHAR strItem[128];
		TCHAR strValue[128];
		LPCTSTR pstr = strSkin;
		int nRead = 0;

		for(int i=0; s_SkinArray.GetSize() > i; i++)
		{
            pSkinNode = (MUISKINFOLDERNODE*)s_SkinArray.GetAt(i);

			if(!_tcsicmp(strSkin, pSkinNode->strSkin))
			{
				//已存在,增加引用计数
				nCount = ++pSkinNode->nCount;
				return nCount;
			}
		}

		//不存在
        pSkinNode = new MUISKINFOLDERNODE;
        memset(pSkinNode, 0, sizeof(MUISKINFOLDERNODE));
		rbtree_set_cmpfn(pSkinNode->ImageCache, RBTreeCompare); 
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
				MUIASSERT(0 == strItem);
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
		return nCount;
	}

	int CSkinManager::RemoveSkin(LPCTSTR strSkin)
	{
		int nCount = -1;
		MUISKINFOLDERNODE* pSkinNode = NULL;
		for(int i=0; s_SkinArray.GetSize() > i; i++)
		{
            pSkinNode = (MUISKINFOLDERNODE*)s_SkinArray.GetAt(i);
			if(!_tcscmp(strSkin, pSkinNode->strSkinFolder))
			{
				//已存在
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
			rbtree_destroy(&pSkinNode->ImageCache);
			s_SkinArray.Remove(pSkinNode);
		}
		return nCount;
	}

    int __stdcall CSkinManager::ClearImage(RBNODE * node, void * param)
    {
		MUIIMAGEINFO* pImage = (MUIIMAGEINFO *)rbtree_node_data(node);
        DeleteObject(pImage->hBitmap);
        delete pImage;
        return 0;//返回true 结束遍历
    }

	MUIRESID CSkinManager::AddResource(int nID, LPCTSTR lpType)
	{
		MUIRESOURCEINFO* pResInfo = NULL;
		MUIRESID index = 0;
		for(int i=0; s_Resource.GetSize() > i; i++)
		{
			MUIRESOURCEINFO* pInfo = (MUIRESOURCEINFO*)s_Resource[i];
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
			//从资源取
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
			UnlockResource(hGlobal);//其实可以不要这句
			::FreeResource(hResource);


            pResInfo = new MUIRESOURCEINFO;
			pResInfo->nID = nID;
			pResInfo->buffet = pData;
			pResInfo->bufsize = dwSize;
			index = pResInfo->index = s_ResourceIndex ++ ;
			pResInfo->nCount = 1;
			_tcscpy(pResInfo->lpType, lpType);
			s_Resource.Add(pResInfo);
			break;
		}

		if(s_ResourceIndex == -1)//s_ResourceID永不为0
        {
            s_ResourceIndex = 1;
        }

return index;
    }

    void CSkinManager::RemoveResource(MUIRESID id)
    {
        for (int i = 0; s_Resource.GetSize() > i; i++)
        {
			MUIRESOURCEINFO* pInfo = (MUIRESOURCEINFO*)s_Resource[i];
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

    //往皮肤中插入一张自定义图片，一般用于添加内存中的图片
    //@strSkin 图片放到那个皮肤中
    //@pImageInfo 图片信息
    bool CSkinManager::AddImage(LPCTSTR strSkin, MUIIMAGEINFO* pImageInfo)
    {
		MUISKINFOLDERNODE* pSkinNode = NULL;
        if (NULL == pImageInfo || _T('\0') == pImageInfo->strFile[0]) return false;

        for (int i = 0; s_SkinArray.GetSize() > i; i++)
        {
			MUISKINFOLDERNODE* node = (MUISKINFOLDERNODE*)s_SkinArray.GetAt(i);
            if (!_tcsicmp(strSkin, node->strSkin))
            {
                pSkinNode = node;
                break;
            }
        }

        if (NULL == pSkinNode) return false;

		RBNODE* pTreeNode = RBTreeCreateNode(pImageInfo->strFile, pImageInfo);

        if (0 != rbtree_insert(&pSkinNode->ImageCache, pTreeNode))
        {
            MUITRACE(_T("CSkinManager::AddImage : %s 文件已经存在！"), pImageInfo->strFile);
            MUIASSERT(0 && "CSkinManager::AddImage : 文件已经存在,请定义合适的文件名称！");
            rbtree_free_node(pTreeNode);
            return false;
        }

        return true;
    }

    bool CSkinManager::RemoveImage(LPCTSTR strSkin, LPCTSTR strImageFile)
    {
        MUISKINFOLDERNODE * pSkinNode = NULL;

        if (NULL == strImageFile || _T('\0') == strImageFile[0]) return false;

        for (int i = 0; s_SkinArray.GetSize() > i; i++)
        {
			MUISKINFOLDERNODE* pNode = (MUISKINFOLDERNODE*)s_SkinArray.GetAt(i);
            if (!_tcsicmp(strSkin, pNode->strSkin))
            {
                pSkinNode = pNode;
                break;
            }
        }

        if (NULL == pSkinNode) return false;

        RBNODE * pTreeNode = rbtree_remove(&pSkinNode->ImageCache, strImageFile);

        if (pTreeNode)
        {
            MUIIMAGEINFO * pImage = (MUIIMAGEINFO*)pTreeNode->data;
            DeleteObject(pImage->hBitmap);
            delete pImage;
            rbtree_free_node(pTreeNode);
        }
        return !!pTreeNode;
    }

    MUIIMAGEINFO * CSkinManager::GetImageInfo(LPCTSTR strSkin, LPCTSTR strImageFile)
    {
		MUISKINFOLDERNODE * pSkinNode = NULL;
		MUIIMAGEINFO* pImage = NULL;
        for (int i = 0; s_SkinArray.GetSize() > i; i++)
        {
			MUISKINFOLDERNODE* pNode = (MUISKINFOLDERNODE*)s_SkinArray.GetAt(i);
            if (!_tcscmp(strSkin, pNode->strSkin))
            {
                pSkinNode = pNode;
                break;
            }
        }

        //没有这个皮肤
        if (NULL == pSkinNode) return NULL;

        RBNODE* pTreeNode = rbtree_find(&pSkinNode->ImageCache, strImageFile);

        if (pTreeNode)
        {
            pImage = (MUIIMAGEINFO*)rbtree_node_data(pTreeNode);
            return pImage;
        }

        //没找到对应的图片，加载它
        pImage = CSkinManager::LoadImage(pSkinNode->strSkinFolder, pSkinNode->resIndex, strImageFile);

        if (NULL == pImage)
        {
            MUITRACE(_T("CSkinManager::GetImageInfo : %s 文件加载失败！"), strImageFile);
        }

        //不管加载成功与失败，仍然放到树中，为了避免缓存击穿
		pTreeNode = RBTreeCreateNode(strImageFile, pImage);

        if (0 != rbtree_insert(&pSkinNode->ImageCache, pTreeNode))
        {
            rbtree_free_node(pTreeNode);
        }
		
		return pImage;
	}

	MUIIMAGEINFO* CSkinManager::LoadImage(LPCTSTR strSkinFolder, MUIRESID resIndex, LPCTSTR strImageFile)
	{
		MUIIMAGEINFO* pImageInfo = NULL;
		BYTE * pData = NULL;
		DWORD dwSize = 0;
		LPBYTE pImage = NULL;
		int x,y,n;
		bool bAlpha = false;
		//从文件中加载图片数据
		if(false == CSkinManager::LoadFileData(strSkinFolder, resIndex , strImageFile, &pData, &dwSize))
		{
			return NULL;
		}
		
		pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
		delete[] pData;
		//图片解析失败
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
			//加载到的图片是反过来的，所以进行位置对调
			// ARGB；  0位=B; 1=G; 2=R; 3=A(透明)
			if( pDest[i*4 + 3] < 255 ) 
			{
				pDest[i*4] = (BYTE)(DWORD(pImage[i*4 + 2])*pImage[i*4 + 3]/255);
				pDest[i*4 + 1] = (BYTE)(DWORD(pImage[i*4 + 1])*pImage[i*4 + 3]/255);
				pDest[i*4 + 2] = (BYTE)(DWORD(pImage[i*4])*pImage[i*4 + 3]/255); 
				bAlpha = true;//带有透明标志
			}
			else
			{
				pDest[i*4] = pImage[i*4 + 2];
				pDest[i*4 + 1] = pImage[i*4 + 1];
				pDest[i*4 + 2] = pImage[i*4]; 
			}

			////颜色 = mask ，设置为0 ，就是设置为透明
			//if( *(DWORD*)(&pDest[i*4]) == mask ) {
			//	pDest[i*4] = (BYTE)0;
			//	pDest[i*4 + 1] = (BYTE)0;
			//	pDest[i*4 + 2] = (BYTE)0; 
			//	pDest[i*4 + 3] = (BYTE)0;
			//	bAlphaChannel = true;
			//}
		}

		stbi_image_free(pImage);

        pImageInfo = new MUIIMAGEINFO;
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
		MUIRESID nIndex = 0;

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
				MUIASSERT(0 == strItem);
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
                MUISKINFOLDERNODE * pSkinNode = (MUISKINFOLDERNODE*)s_SkinArray.GetAt(i);
				if(!_tcscmp(strSkin, pSkinNode->strSkin))
				{
					nIndex = pSkinNode->resIndex;
				}
			}
		}

		return LoadFileData(strFolder, nIndex, strFile, pFileData, pdwDataSize);

	}

	int __stdcall CSkinManager::RBTreeCompare(const void* pObject1, const void* pObject2)
	{
		return _tcscmp((LPCTSTR)pObject1, (LPCTSTR)pObject2);
	}

	RBNODE* CSkinManager::RBTreeCreateNode(LPCTSTR strKey, void* pData)
	{
		int nSize = _tcslen(strKey) + 1;

		if (nSize == 0) return NULL;

		nSize = sizeof(RBNODE) + sizeof(TCHAR) * nSize;

		RBNODE* node = (RBNODE*)malloc(sizeof(RBNODE) + sizeof(TCHAR) * nSize);

		if (node)
		{
			memset(node, 0, nSize);
			node->key = (char*)node + sizeof(RBNODE);
			node->data = pData;
			_tcscpy((LPTSTR)node->key, strKey);
		}

		return node;
	}

	BOOL CSkinManager::LoadFileData(LPCTSTR strSkinFolder, MUIRESID resIndex, LPCTSTR strFile,BYTE ** pFileData, DWORD *pdwDataSize)
	{
		//先从资源文件中的ZIP文件查找
		//失败再到磁盘中的ZIP文件查找
		//失败再到磁盘目录的文件中查找
		//三者都失败返回NULL；
		//strSkin 可以是目录，也可以是一个ZIP文件
		TCHAR strCompletePath[MAX_PATH] = {0};
		HZIP hz = NULL;
		PBYTE pData = NULL;
		DWORD dwSize = 0;
		int nItem = 0;
        MUIRESOURCEINFO * pResInfo = NULL;

#ifndef _UNICODE
		ZIPENTRY ze; 
#else
		ZIPENTRYW ze;
#endif
		_tcscat(strCompletePath, strSkinFolder);
		_tcscat(strCompletePath, strFile);
		//zip文件不支持..\\[上层目录],所以要对strCompletePath进行上层目录处理
		//skin/folder/../file.txt 转成 skin/file.txt
		int nPathLen = _tcslen(strCompletePath);
		for(int i = 0; _T('\0') != strCompletePath[i]; i++)
		{
			//将 \\ 替换成 /
			if(_T('\\') == strCompletePath[i]) strCompletePath[i] = _T('/');

			if(_T('.') == strCompletePath[i] && _T('.') == strCompletePath[i + 1] &&
				_T('/') == strCompletePath[i + 2])
			{
				//发现上层目录，进行合并
				if(0 == i) break; //已经是第一层，没法合并
				int k = i, nCount = 0;
				while(strCompletePath[k] && k >= 0)//找出上层合并点
				{
					if(_T('/') == strCompletePath[k]) nCount ++;

					if(2 == nCount) break;

					k --;
				}
				//合并
				memcpy(&strCompletePath[k], &strCompletePath[i + 2], (nPathLen - (i + 2) + 1) * sizeof(TCHAR));
				nPathLen -= (i + 2) - k;
				i = k;
			}
		}

		//先从资源文件中的ZIP文件查找
		for(int i=0; s_Resource.GetSize() > i && resIndex; i++)
		{
            MUIRESOURCEINFO * pTmp = (MUIRESOURCEINFO*)s_Resource[i];
			if(pTmp->index == resIndex) 
			{
				pResInfo = pTmp;
				break;
			}
		}
		
		while(pResInfo)
		{
			hz = OpenZipU(pResInfo->buffet, pResInfo->bufsize, ZIP_MEMORY);
			if( hz == NULL ) break;//打开失败
			
			if(NULL != FindZipItem(hz, strCompletePath , true, &nItem, &ze))
			{
				//没找到文件
				break;
			}

			dwSize = ze.unc_size;
			if( dwSize == 0 ) break;
			pData = new BYTE[ dwSize ];
			int res = UnzipItem(hz, nItem, pData, dwSize, ZIP_MEMORY);
			CloseZip(hz);
			if( res != ZR_OK && res != ZR_MORE) 
			{
				//解压失败
				delete[] pData;
				pData = NULL;
			}
			break;//只循环一次
			//当皮肤隶属资源文件，会先在资源中查找，资源中没有找到，
			//会继续向磁盘查找，如果不想继续想磁盘查找，请将break替换成return
		}

		//失败再到磁盘中的ZIP文件查找
		while(NULL == pData)
		{
			hz = OpenZipU((void*)strSkinFolder, 0 , ZIP_FILENAME);
			if(NULL == hz) break;//打开失败
			
			if( FindZipItem(hz, strCompletePath , true, &nItem, &ze) != 0 )
			{
				//没找到文件
				break;
			}

			dwSize = ze.unc_size;
			if( dwSize == 0 ) break;
			pData = new BYTE[ dwSize ];
			int res = UnzipItem(hz, nItem, pData, dwSize, ZIP_FILENAME);
			CloseZip(hz);
			if( res != 0x00000000 && res != 0x00000600) 
			{
				//解压失败
				delete[] pData;
				pData = NULL;
			}
			break;//只循环一次
		}
		
		//失败再到磁盘目录的文件中查找
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
			break;//只循环一次
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