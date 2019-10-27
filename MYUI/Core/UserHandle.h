
#ifndef __MYUI_USER_HANDLE_H__
#define __MYUI_USER_HANDLE_H__

#include "../base.h"
#include "../Utils/Utils.h"

namespace MYUI
{

DECLARE_HANDLE            (HTIMER);

    //实际的业务当中, 我们需要一个消息处理体系, 这个体系跟UI处于同一个线程, 但它又不是一个窗口
    //于是产生了UserHandle接口, 在对应的线程中继承IUserHandle进行开发, 可使你的Callback函数与
    //UI处于同一个线程中.
    class MYUI_API IUserHandle
    {
        friend class CWindowUI;
        friend class CMenuUI;
        friend class CUserHandleTable;
    public:
        IUserHandle();
        virtual ~IUserHandle();

        DWORD GetAttachThreadId();

        //将消息放进线程对应的队列
        BOOL PostMessage(UINT message, WPARAM wParam = NULL, LPARAM lParam = NULL);
        LRESULT SendMessage(UINT message, WPARAM wParam = NULL, LPARAM lParam = NULL);
        


        //这是一个很危险的函数, 即使当前线程阻塞,它依然会不停地向队列中拋送消息, 
        //其实我试过好几种定时器的实现了, 比如WM_TIMER, WaitableTimer, 效果都不
        //理想, 所以现在内部用了timeSetEvent, 使用这个函数, 要注意队列中的消息
        //是否能够得到及时处理, 如果能够处理及时, 这个函数不会引发队列疯狂增长
        //如果实在不行, 建议使用窗口的WM_TIMER 进行额外处理
        BOOL SetTimer(UINT message, DWORD dwSecond, LPVOID lpArg);
        BOOL KillTimer(HTIMER hTimer);
        
    protected:
        virtual LRESULT Callback(UINT message, WPARAM wParam , LPARAM lParam) = 0;
       
        static void CALLBACK TimerRoutine(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
        virtual void OnAttach() = 0;//附加到CUserHandleTable时的通知
		virtual void OnDetach() = 0;//离开CUserHandleTable时的通知

    protected:
        HINSTANCE m_hInstance;
    private:
        DWORD m_dwHandleAttachThreadId__;
    };


    class MYUI_API CUserHandleTable
    {
    public:
        //一个线程只能有一个IUserHandle, 新的IUserHandle 会将旧的IUserHandle替换出来
        //需要在对应的ShowModal前调用, 才能生效
        static IUserHandle * AddHandle(IUserHandle * pHandle);

        static IUserHandle * RemoveHandle(DWORD dwThreadId);

        static IUserHandle * GetThreadHandle(DWORD dwThreadId);

        static void InitHandleTable();//建议要在主线程中初始化
        static void FreeHandleTable();
        static DWORD GetMainThreadId();

        //ClearHandleTable 会对IUserHandle执行delete操作
        //因为主线程窗口退出,其他线程的窗口往往是不知道情况的,
        //所以这里帮助IUserHandle释放
        static void ClearHandleTable();

    private:
        static CMuiPtrArray s_Items;
        static DWORD s_dwMainThreadId;
        static CRITICAL_SECTION s_Lock;
    };

};


#endif