
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
        //��Ϊ����ϢС��WM_USER ʱ, ϵͳ�ڲ����ܻ����Ϣ����һЩ����Ĵ���, 
        //���������������message ���� WM_USER, ��֪ͨCallback��ʱ��, 
        //�Ὣmessage ��ȥ�� WM_USER ��
        //����Ҳ���Ѵ��, ��ϢҪ�� WM_USER����Ч��Χ��, ̫����, �ᷢ��δ֪����

        assert(0x7FFF > message && "message ȡֵ��Χ����, ��ʹ��С��0x7FFF��ֵ!");

        return PostThreadMessage(m_dwHandleAttachThreadId__, WM_USER + message, wParam, lParam);
    }


    LRESULT IUserHandle::SendMessage(UINT message, WPARAM wParam , LPARAM lParam)
    {
        //��ͬһ�̵߳���IUserHandle::SendMessage �п��������̰߳�ȫ����, ��������Ӹ���ȫ��ʾ
        //�������Ϊ����Ҫ���ǰ�ȫ����Ļ�, �Ͱ��������ȥ����
        assert(m_dwHandleAttachThreadId__ == GetCurrentThreadId() && "����IUserHandle::SendMessage�̵߳ĺ�������IUserHandle���������߳�");

        return this->Callback(message, wParam, lParam);
    }

    BOOL IUserHandle::SetTimer(UINT message, DWORD dwSecond, LPVOID lpArg)
    {
        //һ������ȷ�Ķ�ʱ��������WM_TIMER��ͬ�� WM_TIMER��ͬ���첽�� ���WndProcδ����Ϣ
        //�����꣬WM_TIMER������ʱ��������SetTimer��ͬ��ֻҪʱ��һ�������ϾͻὫ��Ϣ
        //Ͷ�뵽��Ϣ���У�Ȼ����ˮ��ִ��

        TIMERDATA * pData = new TIMERDATA;
        assert(0x7FFF > message && "message ȡֵ��Χ����, ��ʹ��С��0x7FFF��ֵ!");
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

        //�����Ƿ�����ͨ��, ʹ�����Լ����ǰ�
        if(pData->pHandle != this) return FALSE;//Σ�ղ���, ʧ��

        BOOL bRet = !!timeKillEvent(pData->uTimerId);
        delete pData;
        return TRUE;
    }

    void CALLBACK IUserHandle::TimerRoutine(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
    {
        //����ص��Ѿ�����������һ���߳���
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
        InitializeCriticalSection(&CUserHandleTable::s_Lock);//�ٽ���
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


