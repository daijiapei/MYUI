
#ifndef _MY_WIN_FRAME_DEMO_WINDOW_H_
#define _MY_WIN_FRAME_DEMO_WINDOW_H_

#ifndef _MY_WIN_FRAME_H_
#include "../MYUI/myui.h"
using namespace MYUI;
#endif


class CBuilderControl : public IBuilderCallback
{
public:
	virtual CControlUI * CreateControl(const char* strClass);
};

class CFrameWindow : public CWindowUI , public IControlHooker
{
public:
	CFrameWindow();
	~CFrameWindow();

	void OnNotify(TNOTIFYUI &notify);
	LRESULT OnEvent(TEVENT &event);

public:
	virtual bool OnBefore(PVOID pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule);
	virtual bool OnAfter(PVOID pControl, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResule);

protected:
	virtual LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT OnCreate2(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	LRESULT OnReady(WPARAM wParam, LPARAM lParam);

	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);

private:
    CMenuUI * m_pMenUI;
};



#endif