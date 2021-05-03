
#ifndef __MYUI_SKIN_MANAGER_H__
#define __MYUI_SKIN_MANAGER_H__

#include "../Utils/Utils.h"
#include "../Utils/rbtree.h"

namespace MYUI
{
	typedef  DWORD  MUIRESID;
    //ͼƬ��Ϣ
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
		TCHAR strSkinFolder[MAX_PATH];//��һ����Ŀ¼��Ҳ�п�����ZIP�ļ�
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

	//��Ϊ��������Key+value����ʽ����ͼƬ��key���ļ�·����value���ļ���Ϣ�����ļ�·����������ȡ����xml���ڲ�����
	//�������xml�ڲ�ͬ�����Ŀ¼�·���ͬһ��ͼƬ����ô���Ǹ�����·�����ǲ�ͬ�ģ�·����ͬ��ζ��key��ͬ��Ҳ����ͬһ��
	//ͼƬ�����������ڴ棬����ֻ��key��ͬ���ѣ����Խ��龡����xml�ļ�������һ��Ŀ¼

	//����ģʽ���̲߳���ȫ
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

		//����ǿ��ų�ȥ�Ĺ�����Ա
		static bool LoadFile(LPCTSTR strSkin,LPCTSTR strFile, BYTE ** pFileData, DWORD * pdwDataSize);
	private:
		static int __stdcall RBTreeCompare(const void* pObject1, const void* pObject2);
		static RBNODE* RBTreeCreateNode(LPCTSTR strKey, void * pData);
        static MUIIMAGEINFO* LoadImage(LPCTSTR strSkinFolder, MUIRESID resIndex, LPCTSTR strImageFile);

		//������˽�г�Ա
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