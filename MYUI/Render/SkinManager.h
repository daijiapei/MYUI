
#ifndef _MYUI_SKIN_MANAGER_H_
#define _MYUI_SKIN_MANAGER_H_

#include "../Utils/Utils.h"
#include "../Utils/rbtree.h"
typedef  DWORD  RSRCID;
namespace MYUI
{
    //ͼƬ��Ϣ
	typedef struct __IMAGEINFO
	{
		TCHAR strFile[MAX_PATH];
		HBITMAP hBitmap;
		SIZE szBitmap;
		bool bAlpha;
    }IMAGEINFO, *LPIMAGEINFO;
    

	typedef struct __SKINFOLDERNODE
	{
		TCHAR strSkin[MAX_PATH];
		TCHAR strSkinFolder[MAX_PATH];//��һ����Ŀ¼��Ҳ�п�����ZIP�ļ�
		int resId;
		TCHAR resType[MAX_PATH];
		RSRCID resIndex;
		volatile int nCount;
        RBTREEROOT ImageCache;
    }SKINFOLDERNODE, *LPSKINFOLDERNODE;

	typedef struct __RESOURCEINFO
	{
		RSRCID index;
		int nID;
		TCHAR lpType[MAX_PATH];
		LPBYTE buffet;
		int bufsize;
		volatile int nCount;
    }RESOURCEINFO, *LPRESOURCEINFO;

	//����ģʽ���̲߳���ȫ
	class CSkinManager
	{
	public:
		static void SetInstance(HINSTANCE hInstance);

		static int AddSkin(LPCTSTR strSkin);
		static int RemoveSkin(LPCTSTR strSkin);
        static IMAGEINFO * GetImageInfo(LPCTSTR strSkin, LPCTSTR strImageFile);
        static bool AddImage(LPCTSTR strSkin, IMAGEINFO * pImageInfo);
        static bool RemoveImage(LPCTSTR strSkin, LPCTSTR strImageFile);
		static RSRCID AddResource(int nID, LPCTSTR lpType);
        static RESOURCEINFO * GetResource(RSRCID resid);
		static void RemoveResource(RSRCID resIndex);
		static void RemoveAllResource();

		//����ǿ��ų�ȥ�Ĺ�����Ա
		static bool LoadFile(LPCTSTR strSkin,LPCTSTR strFile, BYTE ** pFileData, DWORD * pdwDataSize);
	private:
        static IMAGEINFO * LoadImage(LPCTSTR strSkinFolder, RSRCID resIndex, LPCTSTR strImageFile);

		//������˽�г�Ա
		static bool LoadFileData(LPCTSTR strSkinFolder, RSRCID resIndex, LPCTSTR strFile, 
            BYTE ** pFileData, DWORD *pdwDataSize);
        static int __stdcall ClearImage(RBNODE * node, void * param);
	private:
		static RSRCID s_ResourceIndex;
		static CMuiPtrArray s_Resource;
		static CMuiPtrArray s_SkinArray;
		static HINSTANCE s_hInstance;
	};

}

#endif