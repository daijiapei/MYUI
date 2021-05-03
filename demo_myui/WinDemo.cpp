
#include "WinDemo.h"
#include <atlbase.h>
//CComModule _Module;
#include <atlwin.h>


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR szCmdLine, int iCmdShow)
{
	TCHAR strFloder[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, strFloder);

	CoInitialize(NULL);
	//_Module.Init( 0, hInstance );

	MYUI::CUIThread::Init(hInstance);

	CFrameWindow * window = new CFrameWindow();
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = 800;
	rect.bottom = 640;
	window->Create(hInstance,NULL,WS_OVERLAPPEDWINDOW , TEXT("HelloWin"), TEXT("HelloWin"), &rect);
	window->SetIcon(IDI_ICON1);
	window->CenterWindow();
	window->ShowModal();
	delete window;

	MYUI::CUIThread::Uninit();

	//_Module.Term();
	CoUninitialize();
    return 0;
}