
#ifndef _MYUI_RENDER_H_
#define _MYUI_RENDER_H_

#include "..\\Utils\\Utils.h"

typedef  int RSRCID;

namespace MYUI
{
	typedef struct _ImageInfo
	{
		TCHAR strFile[MAX_PATH];
		HBITMAP hBitmap;
		SIZE bmpSize;
		bool bAlpha;
	}TImageInfo ,* PTImageInfo;

	typedef struct _SkinFolderNode
	{
		TCHAR strSkinFolder[MAX_PATH];
		enum FolderType
		{
			UnKown = 0,
			ResourceFile,//资源文件
			DiskFile,//磁盘文件
			DiskFolder //磁盘目录
		};
		FolderType type;
		volatile int nCount;
		CAvlTree * ImageCache;
	}SkinFolderNode, *PSkinFolderNode;

	typedef struct _ResourceInfo
	{
		RSRCID index;
		HINSTANCE hInstance;
		int nID;
		TCHAR lpType[MAX_PATH];
		LPBYTE buffet;
		int bufsize;
		volatile int nCount;
	}TResourceInfo, * PTResourceInfo;

	

	class MYUI_API CRenderEngine 
	{
	public:
		//渲染引擎，单例模式，线程不安全
		CRenderEngine();
		~CRenderEngine();
		//有些地方用CMuiPtrArray是因为感觉这个地方的数据量不会太多
		//如果有大量的资源和皮肤，请选择红黑树或查找树
		static HPEN CreatePen();
		static void ReleasePen();

		static HRGN BeginPaint(HDC hPaintDc, const RECT &rcItem);
		static void EndPaint(HDC hPaintDc, HRGN hOldRgn);
		//static void SetDcSkin(HDC hDc, CMuiString &strSkin);
		//static const CMuiString &GetDcSkin(HDC hDc);
		
		static int AddSkin(LPCTSTR strSkin);
		static int RemoveSkin(LPCTSTR strSkin);
		static TImageInfo * GetImageInfo(LPCTSTR strSkin, LPCTSTR strImageName);
		static TImageInfo * LoadImage(LPCTSTR strSkin, LPCTSTR strImageName);
		static bool LoadImageData(LPCTSTR strSkin, LPCTSTR strImageName,BYTE ** pData, DWORD &dwSize);
		
		//static SIZE GetTextSize();
		//static bool CloneMemoryToPaint();

		static RSRCID AddResource(HINSTANCE hInstatce, int nID, LPCTSTR lpType);
		static void RemoveResource(RSRCID id);
		static void RemoveAllResource();
		
		static bool DrawImage(HDC hMemDc,const RECT * rcItem, LPCTSTR strSkin, LPCTSTR strImageFile, 
			const RECT * rcSource, COLORREF mask);
		static void DrawColor(HDC hMemDc, HBRUSH hBrush,const RECT &rcItem);
		static void DrawText(HDC hMemDc,HFONT hFont, ARGBREF argb,const RECT &rcItem, 
			 CMuiString strText, DWORD dwType);
		static void DrawBroder(HDC hMemDc, HPEN hPen,const RECT &rcItem);
		static void DrawRoundRect(HDC hMemDc,HPEN hPen, const RECT& rcItem, int width, int height);
		static BOOL MoveMemoryToPaint(HDC hPaintDc,const RECT & rcPaintFixed,  HDC hMemDc,const RECT &rcItem,DWORD alpha);
		static BOOL CloneMemoryToPaint(HDC hPaintDc,const RECT & rcPaintFixed,  HDC hMemDc,const RECT &rcItem,DWORD alpha);
	private:
		static bool ClearSkin(AvlTreeNode * node);
		static RSRCID g_ResourceIndex;
		static CMuiPtrArray g_Resource;
		static CMuiPtrArray g_SkinArray;
	};
}

#endif