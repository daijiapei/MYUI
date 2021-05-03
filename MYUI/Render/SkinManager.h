
#ifndef __MYUI_SKIN_MANAGER_H__
#define __MYUI_SKIN_MANAGER_H__

#include "../Utils/Utils.h"
#include "../Utils/rbtree.h"

namespace MYUI
{
	typedef  DWORD  MUIRESID;
    //图片信息
	typedef struct __MUIIMAGE_INFO
	{
		TCHAR strFile[MAX_PATH];
		HBITMAP hBitmap;
		SIZE szBitmap;
		BOOL bAlpha;
    }MUIIMAGEINFO, *LPMUIIMAGEINFO;
    

	typedef struct __MUISKINFOLDER_NODE
	{
		TCHAR strSkin[MAX_PATH];
		TCHAR strSkinFolder[MAX_PATH];//不一定是目录，也有可能是ZIP文件
		int resId;
		TCHAR resType[MAX_PATH];
		MUIRESID resIndex;
		volatile int nCount;
        RBTREEROOT ImageCache;
    }MUISKINFOLDERNODE, *LPMUISKINFOLDERNODE;

	typedef struct __MUIRESOURCE_INFO
	{
		MUIRESID index;
		int nID;
		TCHAR lpType[MAX_PATH];
		LPBYTE buffet;
		int bufsize;
		volatile int nCount;
    }MUIRESOURCEINFO, *LPMUIRESOURCEINFO;

	//因为这里是用Key+value的形式管理图片，key是文件路径，value是文件信息，而文件路径的命名有取决于xml的内部命名
	//如果两个xml在不同级别的目录下访问同一张图片，那么他们给出的路径将是不同的，路径不同意味着key不同，也就是同一张
	//图片将会有两份内存，他们只是key不同而已，所以建议尽量将xml文件都放在一个目录

	//单例模式，线程不安全
	class CSkinManager
	{
	public:
		static void SetInstance(HINSTANCE hInstance);

		static int AddSkin(LPCTSTR strSkin);
		static int RemoveSkin(LPCTSTR strSkin);
        static MUIIMAGEINFO* GetImageInfo(LPCTSTR strSkin, LPCTSTR strImageFile);
        static bool AddImage(LPCTSTR strSkin, MUIIMAGEINFO* pImageInfo);
        static bool RemoveImage(LPCTSTR strSkin, LPCTSTR strImageFile);
		static MUIRESID AddResource(int nID, LPCTSTR lpType);
        static MUIRESOURCEINFO* GetResource(MUIRESID resid);
		static void RemoveResource(MUIRESID resIndex);
		static void RemoveAllResource();

		//这个是开放出去的公共成员
		static bool LoadFile(LPCTSTR strSkin,LPCTSTR strFile, BYTE ** pFileData, DWORD * pdwDataSize);
	private:
		static int __stdcall RBTreeCompare(const void* pObject1, const void* pObject2);
		static RBNODE* RBTreeCreateNode(LPCTSTR strKey, void * pData);
        static MUIIMAGEINFO* LoadImage(LPCTSTR strSkinFolder, MUIRESID resIndex, LPCTSTR strImageFile);

		//这里是私有成员
		static BOOL LoadFileData(LPCTSTR strSkinFolder, MUIRESID resIndex, LPCTSTR strFile,
            BYTE ** pFileData, DWORD *pdwDataSize);
        static int __stdcall ClearImage(RBNODE * node, void * param);
	private:
		static MUIRESID s_ResourceIndex;
		static CMuiPtrArray s_Resource;
		static CMuiPtrArray s_SkinArray;
		static HINSTANCE s_hInstance;
	};

}

#endif