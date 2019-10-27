
#include "UserHandle.h"

#pragma comment(lib, "Winmm.lib")

namespace MYUI
{

    typedef struct _timer_data_
    {
        IUserHandle * pHandle;
        UINT uTimerId;
        UINT uMsg;
        DWORD dwSecond;
        LPVOID lpArg;
    }TIMERDATA, * PTIMERDATA;

    IUserHandle::IUserHandle()
    {
        m_dwHandleAttachThreadId__ = ::GetCurrentThreadId();
        m_hInstance = GetModuleHandle(NULL);
    }

    IUserHandle::~IUserHandle()
    {
    }

    DWORD IUserHandle::GetAttachThreadId()
    {
        return m_dwHandleAttachThreadId__;
    }

    BOOL IUserHandle::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
    {
        //因为当消息小于WM_USER 时, 系统内部可能会对消息进行一些额外的处理, 
        //所以这里在这里对message 加上 WM_USER, 在通知Callback的时候, 
        //会将message 减去的 WM_USER 的
        //这里也提醒大家, 消息要在 WM_USER的有效范围内, 太大了, 会发生未知错误

        assert(0x7FFF > message && "message 取值范围过大, 请使用小于0x7FFF的值!");

        return PostThreadMessage(m_dwHandleAttachThreadId__, WM_USER + message, wParam, lParam);
    }


    LRESULT IUserHandle::SendMessage(UINT message, WPARAM wParam , LPARAM lParam)
    {
        //非同一线程调用IUserHandle::SendMessage 有可能引起线程安全问题, 所以这里加个安全提示
        //如果你认为不需要考虑安全问题的话, 就把这个断言去掉吧
        assert(m_dwHandleAttachThreadId__ == GetCurrentThreadId() && "调用IUserHandle::SendMessage线程的函数并非IUserHandle所监听的线程");

        return this->Callback(message, wParam, lParam);
    }

    BOOL IUserHandle::SetTimer(UINT message, DWORD dwSecond, LPVOID lpArg)
    {
        //一个不精确的定时器，它跟WM_TIMER不同， WM_TIMER是同步异步， 如果WndProc未将消息
        //处理完，WM_TIMER将会延时产生，而SetTimer则不同，只要时间一到，马上就会将消息
        //投入到消息队列，然后按流水线执行

        TIMERDATA * pData = new TIMERDATA;
        assert(0x7FFF > message && "message 取值范围过大, 请使用小于0x7FFF的值!");
        memset(pData, 0, sizeof(TIMERDATA));
        pData->pHandle = this;
        pData->dwSecond = dwSecond;
        pData->uMsg = message;
        pData->lpArg = lpArg;

        pData->uTimerId = timeSetEvent(pData->dwSecond * 1000, 10, IUserHandle::TimerRoutine, 
                        (DWORD_PTR)pData, TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);

        //return PostThreadMessage(m_dwThreadId, WM_USER + 0x7FFF + WM_TIMER, dwSecond, (LPARAM)pData);
        return !!pData->uTimerId;
    }

    BOOL IUserHandle::KillTimer(HTIMER hTimer)
    {
        TIMERDATA * pData = reinterpret_cast<TIMERDATA *>(hTimer);

        //这里是否允许通过, 使用者自己考虑吧
        if(pData->pHandle != this) return FALSE;//危险操作, 失败

        BOOL bRet = !!timeKillEvent(pData->uTimerId);
        delete pData;
        return TRUE;
    }

    void CALLBACK IUserHandle::TimerRoutine(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
    {
        //这个回调已经进入了另外一个线程中
        TIMERDATA * pData = reinterpret_cast<TIMERDATA *>(dwUser);
        //pData->uTimerId = uTimerID;
        pData->pHandle->PostMessage(pData->uMsg, dwUser, (LPARAM)pData->lpArg);

        TRACE(_T("IUserHandle::TimerRoutine::ThreadId=%ld /t this::ThreadId=%ld"), 
            GetCurrentThreadId(), pData->pHandle->m_dwHandleAttachThreadId__);
    }

/*************************************************************************
 *
 *
 *
 *
*************************************************************************/

    DWORD CUserHandleTable::s_dwMainThreadId = NULL;
    CMuiPtrArray CUserHandleTable::s_Items;
    CRITICAL_SECTION CUserHandleTable::s_Lock = { 0 };

    IUserHandle * CUserHandleTable::AddHandle(IUserHandle * pHandle)
    {
        IUserHandle * pTemp = NULL;

        DWORD dwThreadId = pHandle->GetAttachThreadId();
        EnterCriticalSection(&CUserHandleTable::s_Lock);
        for(int i = 0 ; s_Items.GetSize() > i; i++)
        {
            pTemp = static_cast<IUserHandle *>(s_Items[i]);

            if(dwThreadId == pTemp->GetAttachThreadId())
            {
                s_Items.Remove(i);
                pTemp->OnDetach();
                break;
            }

            pTemp = NULL;
        }


        s_Items.Add(pHandle);
        pHandle->OnAttach();

        LeaveCriticalSection(&CUserHandleTable::s_Lock);

        return pTemp;
    }

    IUserHandle * CUserHandleTable::RemoveHandle(DWORD dwThreadId)
    {
        IUserHandle * pTemp = NULL;

        if(NULL == dwThreadId)
        {
            dwThreadId = ::GetCurrentThreadId();
        }
        EnterCriticalSection(&CUserHandleTable::s_Lock);
        for(int i = 0 ; s_Items.GetSize() > i; i++)
        {
            pTemp = static_cast<IUserHandle *>(s_Items[i]);

            if(dwThreadId == pTemp->GetAttachThreadId())
            {
                s_Items.Remove(i);
                pTemp->OnDetach();
                break;
            }

            pTemp = NULL;
        }
        LeaveCriticalSection(&CUserHandleTable::s_Lock);
        return pTemp;
    }

    IUserHandle * CUserHandleTable::GetThreadHandle(DWORD dwThreadId)
    {
        IUserHandle * pTemp = NULL;

        if(NULL == dwThreadId)
        {
            dwThreadId = ::GetCurrentThreadId();
        }
        EnterCriticalSection(&CUserHandleTable::s_Lock);
        for(int i = 0 ; s_Items.GetSize() > i; i++)
        {
            pTemp = static_cast<IUserHandle *>(s_Items[i]);

            if(dwThreadId == pTemp->GetAttachThreadId() )
            {
                break;
            }

            pTemp = NULL;
        }
        LeaveCriticalSection(&CUserHandleTable::s_Lock);
        return pTemp;
    }

    void CUserHandleTable::InitHandleTable()
    {
        CUserHandleTable::s_dwMainThreadId = ::GetCurrentThreadId();
        InitializeCriticalSection(&CUserHandleTable::s_Lock);//临界区
    }

    void CUserHandleTable::FreeHandleTable()
    {
        CUserHandleTable::s_dwMainThreadId  = NULL;
        DeleteCriticalSection(&CUserHandleTable::s_Lock);
    }

    DWORD CUserHandleTable::GetMainThreadId()
    {
        return CUserHandleTable::s_dwMainThreadId;
    }

    void CUserHandleTable::ClearHandleTable()
    {
        EnterCriticalSection(&CUserHandleTable::s_Lock);
        for(int i = 0 ; s_Items.GetSize() > i; i++)
        {
            IUserHandle * pHandle = static_cast<IUserHandle *>(s_Items[i]);
            pHandle->OnDetach();
            delete pHandle;
        }
        s_Items.Empty();
        LeaveCriticalSection(&CUserHandleTable::s_Lock);
    }

};


