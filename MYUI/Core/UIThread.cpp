
#include "UIThread.h"
#include "UIControl.h"
#include "../Render/SkinManager.h"

namespace MYUI
{

    CUITimer::CUITimer(HWND hTimerWnd)
        :m_pMinHeap(NULL)
        ,m_hTimerWnd(hTimerWnd)
    {
        m_pMinHeap = binaryheap_new(256, CUITimer::TimeCompare, CUITimer::UpdateIndex);
        m_pRegditFirst = m_pRegditLast = NULL;
    }

    CUITimer::~CUITimer()
    {
        binaryheap_free(m_pMinHeap);
    }

    int __stdcall CUITimer::TimeCompare(void* pObject1, void* pObject2)
    {
        MUITIMERITEM* pItem1 = (MUITIMERITEM*)pObject1;
        MUITIMERITEM* pItem2 = (MUITIMERITEM*)pObject2;

        if (pItem1->tiAbsoluteTimeOut > pItem2->tiAbsoluteTimeOut)
        {
            return 1;
        }
        else if (pItem1->tiAbsoluteTimeOut < pItem2->tiAbsoluteTimeOut)
        {
            return -1;
        }

        return 0;
    }

    void __stdcall CUITimer::UpdateIndex(void* pObject, unsigned int uIndex)
    {
        MUITIMERITEM* pItem = (MUITIMERITEM*)pObject;
        pItem->uIndex = uIndex;
    }

    LPMUITIMERITEM CUITimer::SetTimer(CUIMessage* pMessage, DWORD dwTimerId, DWORD dwTimeOut, LPVOID lParam)
    {
        if (FALSE == ::IsWindow(m_hTimerWnd)) return NULL;

        MUITIMERITEM* pItem = new MUITIMERITEM;
        memset(pItem, 0, sizeof(MUITIMERITEM));
        pItem->pMessage = pMessage;
        pItem->dwTimerId = dwTimerId;
        pItem->dwTimeOut = dwTimeOut;
        pItem->lParam = (LPARAM)lParam;
        pItem->uIndex = -1;

        RegditQueuePush(pItem);
        //发出这条命令会使Timer再次注册
        ::PostMessage(m_hTimerWnd, WM_TIMER, CUITimer::TimerId, NULL);
        return pItem;
    }

    BOOL CUITimer::KillTimer(LPMUITIMERITEM pItem)
    {
        if (NULL == pItem) return FALSE;

        if (-1 == pItem->uIndex)
        {
            //不在小根堆，那一定是在注册队列, 从队列中移除
            RegditQueueErase(pItem);
        }
        else
        {
            //当前放在小根堆中, 从小根堆移除
            binaryheap_erase(m_pMinHeap, pItem->uIndex);
        }

        return TRUE;
    }

    BOOL CUITimer::RegditTimer(HWND hWnd)
    {
        //重置定时器，并放到小根堆中
        DWORD dwTimeOut = 0;
        if (FALSE == ::IsWindow(hWnd)) return FALSE;
        ::KillTimer(hWnd, CUITimer::TimerId);

        MUITIMERITEM* pItem = NULL;
        HEAPELEMENT Elem = { 0 };
        time_t tiCurrent = GetTickCount();

        while (pItem = RegditQueuePopup())
        {
            Elem.pObject = pItem;
            pItem->tiAbsoluteTimeOut = pItem->dwTimeOut + tiCurrent;
            binaryheap_push(m_pMinHeap, &Elem);
        }

        if (0 == binaryheap_top(m_pMinHeap, &Elem))
        {
            pItem = (MUITIMERITEM*)Elem.pObject;

            if (pItem->tiAbsoluteTimeOut - tiCurrent > 0)
            {
                dwTimeOut = pItem->tiAbsoluteTimeOut - tiCurrent;
                return !!::SetTimer(m_hTimerWnd, CUITimer::TimerId, dwTimeOut, NULL);
            }
            else
            {
                //已经超时了，直接触发定时器
                return ::PostMessage(m_hTimerWnd, WM_TIMER, CUITimer::TimerId, NULL);
            }
            
        }
        return TRUE;
    }

    DWORD CUITimer::GetWaitCount()
    {
        return binaryheap_get_count(m_pMinHeap);
    }

    DWORD CUITimer::ActiveTimer(HWND hWnd)
    {
        DWORD dwCount = 0;
        HEAPELEMENT Elem = { 0 };
        MUITIMERITEM* pItem = NULL;
        time_t tiCurrent = GetTickCount();
        CUIMessage* pMessage = NULL;

        if (m_hTimerWnd != hWnd) return -1;
        

        while (0 == binaryheap_top(m_pMinHeap, &Elem))
        {
            pItem = (MUITIMERITEM*)Elem.pObject;
            if (pItem->tiAbsoluteTimeOut > tiCurrent)
            {
                //没有发生超时
                break;
            }

            //超时已触发,剔除这个超时对象
            binaryheap_erase(m_pMinHeap, pItem->uIndex);

            //因为要重新触发，先放到注册队列中，事件完成后会再次放到小根堆
            RegditQueuePush(pItem);

            pItem->pMessage->CallWndProc(WM_TIMER, pItem->dwTimerId, pItem->lParam);

            dwCount++;
        }

        if (FALSE == RegditTimer(hWnd))
        {
            return -2;
        }
        return dwCount;
    }


    BOOL CUITimer::RegditQueuePush(LPMUITIMERITEM pItem)
    {
        if (NULL == pItem) return FALSE;

        if (NULL == m_pRegditFirst)
        {
            pItem->pPrev = pItem->pNext = NULL;
            m_pRegditFirst = m_pRegditLast = pItem;
        }
        else
        {
            pItem->pPrev = m_pRegditLast;
            pItem->pNext = NULL;
            m_pRegditLast->pNext = pItem;
            m_pRegditLast = pItem;
        }

        return TRUE;
    }

    LPMUITIMERITEM CUITimer::RegditQueuePopup()
    {
        LPMUITIMERITEM pItem = NULL;
        if (NULL == m_pRegditFirst) return pItem;

        if (m_pRegditFirst == m_pRegditLast)
        {
            pItem = m_pRegditFirst;
            m_pRegditFirst = m_pRegditLast = NULL;
        }
        else
        {
            pItem = m_pRegditFirst;
            m_pRegditFirst = m_pRegditFirst->pNext;
            m_pRegditFirst->pPrev = NULL;
        }

        pItem->pPrev = NULL;
        pItem->pNext = NULL;
        return pItem;
    }

    BOOL CUITimer::RegditQueueErase(LPMUITIMERITEM pItem)
    {
        if (NULL == pItem) return FALSE;

        if (NULL == pItem->pPrev && NULL == pItem->pNext) return FALSE;

        if (pItem->pPrev)
        {
            pItem->pPrev->pNext = pItem->pNext;
        }
        else
        {
            //前面没有对象，那他肯定是头部
            //m_pRegditFirst = pItem->pNext;
        }

        if (pItem->pNext)
        {
            pItem->pNext->pPrev = pItem->pPrev;
        }
        else
        {
            //后面没有对象，那他肯定是在尾部
            //m_pRegditLast = pItem->pPrev;
        }

        if (pItem == m_pRegditFirst)
        {
            m_pRegditFirst = pItem->pNext;
        }

        if (pItem == m_pRegditLast)
        {
            m_pRegditLast = pItem->pPrev;
        }

        pItem->pPrev = pItem->pNext = NULL;
        return TRUE;
    }


/*************************************************************************
*
*
*
*
*************************************************************************/

    CUIMessage::CUIMessage()
    {
        m_pNodeFrist = m_pNodeLast = NULL;
    }

    CUIMessage::~CUIMessage()
    {

    }

    LRESULT CUIMessage::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
    {
        return this->CallWndProc(message, wParam, lParam);
    }

    BOOL CUIMessage::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
    {
        MUIMESSAGENODE* pNode = CUIThread::PostMessage(this, message, wParam, lParam);

        if (pNode)
        {
            AddMessage(pNode);
        }
        return !!pNode;
    }

    void CUIMessage::OnAttach(HWND hNewWnd)//当控件附加到新的窗口时，会调用一次该函数
    {
        return;
    }

    void CUIMessage::OnDetach(HWND hOldWnd)//当控件离开一个窗口时，会调用一次该函数 
    {
        int nCount = 0;
        //将所有消息设置为无效，这样就未触发的消息就不会被调用
        MUIMESSAGENODE* pNode = m_pNodeFrist;
        while (pNode)
        {
            //将所有消息置为无效
            pNode->bValid = FALSE;
            pNode = pNode->pNext;
        }

        m_pNodeFrist = m_pNodeLast = NULL;

        //清除所有定时器
        LPMUITIMERITEM pItem = NULL;
        while (nCount = m_Timers.GetSize())
        {
            //从尾到头移除，减少内存迁移
            pItem = (LPMUITIMERITEM)m_Timers[nCount-1];
            m_Timers.Remove(nCount - 1);
            CUIThread::KillTimer(pItem);
        }

        return;
    }

    BOOL CUIMessage::SetTimer(UINT nIDEvnet, UINT uElapse)
    {
        LPMUITIMERITEM pItem = NULL;
        pItem = CUIThread::SetTimer(this, nIDEvnet, uElapse, NULL);
        m_Timers.Add(pItem);
        return TRUE;
    }

    BOOL CUIMessage::KillTimer(UINT nIDEvnet)
    {
        LPMUITIMERITEM pItem = NULL;

        for (int i = 0; m_Timers.GetSize() > i; i++)
        {
            pItem = (LPMUITIMERITEM)m_Timers[i];

            if (nIDEvnet == pItem->dwTimerId)
            {
                m_Timers.Remove(pItem);
                break;
            }
        }
         
        if (NULL == pItem) return FALSE;

        return CUIThread::KillTimer(pItem);
    }

    BOOL CUIMessage::AddMessage(MUIMESSAGENODE* pNode)
    {
        if (pNode)
        {
            pNode->pNext = NULL;

            if (NULL == m_pNodeLast)
            {
                m_pNodeFrist = pNode;
            }
            else
            {
                m_pNodeLast->pNext = pNode;
            }
            
            m_pNodeLast = pNode;
            return TRUE;
        }
        return FALSE;
    }

    BOOL CUIMessage::RemoveMessage(MUIMESSAGENODE* pNode)
    {
        //因为post的消息都是按顺序到达的，所以必然能匹配上第一条
        //如果不为第一条，肯定出现了某些逻辑错误
        if (pNode && pNode == m_pNodeFrist)
        {
            m_pNodeFrist = m_pNodeFrist->pNext;

            if (NULL == m_pNodeFrist)
            {
                m_pNodeLast = NULL;
            }
            return TRUE;
        }
        return FALSE;
    }

/*************************************************************************
*
*
*
*
*************************************************************************/
    HINSTANCE CUIThread::s_hInstance = NULL;
    HWND CUIThread::s_hThreadProc = NULL;
    MUITHREADINFO CUIThread::s_ThreadInfo = { 0 };

#define THREADPROC_CLASSNAME TEXT("THREADPROC_CLASSNAME")
    BOOL CUIThread::Init(HINSTANCE hInstance)
    {
        s_hInstance = hInstance;
        CSkinManager::SetInstance(hInstance);

        WNDCLASS WndClass = { 0 };
        //wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
        WndClass.style = CS_DBLCLKS;
        WndClass.lpfnWndProc = CUIThread::ThreadProc;
        WndClass.cbClsExtra = sizeof(void*);
        WndClass.cbWndExtra = sizeof(void*);
        WndClass.hInstance = s_hInstance;
        WndClass.hIcon = NULL;
        WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        WndClass.hbrBackground = CreateSolidBrush(NULL_BRUSH);
        WndClass.lpszMenuName = NULL;
        WndClass.lpszClassName = THREADPROC_CLASSNAME;

        ATOM ret = ::RegisterClass(&WndClass);
        MUIASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);

        

        s_hThreadProc = ::CreateWindow(THREADPROC_CLASSNAME, THREADPROC_CLASSNAME,
            WS_OVERLAPPEDWINDOW, 0, 0, 0, 0,
            NULL, NULL, s_hInstance, &s_ThreadInfo);

        if (NULL == s_hThreadProc)
        {
            return FALSE;
        }

        s_ThreadInfo.dwThreadId = ::GetCurrentThreadId();
        s_ThreadInfo.pTimer = new CUITimer(s_hThreadProc);
        s_ThreadInfo.WorkPool.cmpfn = WorkTreeCompare;
        return TRUE;
    }

    void CUIThread::Uninit()
    {
        if (s_hThreadProc)
        {
            DestroyWindow(s_hThreadProc);
            s_hThreadProc = NULL;
        }
        rbtree_enum(&s_ThreadInfo.WorkPool, RBTREE_ENUM_PRE, CUIThread::WorkerDelete, NULL);
        rbtree_destroy(&s_ThreadInfo.WorkPool);

        if (s_ThreadInfo.pTimer)
        {
            delete s_ThreadInfo.pTimer;
        }
        memset(&s_ThreadInfo, 0, sizeof(MUITHREADINFO));
    }


    MUIMESSAGENODE* CUIThread::PostMessage(CUIMessage* pMessage, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        MUIMESSAGENODE* pNode = MallocMQNode();
        pNode->bValid = TRUE;
        pNode->Message.pMessage = pMessage;
        pNode->Message.uMsg = uMsg;
        pNode->Message.wParam = wParam;
        pNode->Message.lParam = lParam;

        ::PostMessage(s_hThreadProc, CUIThread::UWM_MESSAGE, NULL, (LPARAM)pNode);
        return pNode;
    }


    LPMUITIMERITEM CUIThread::SetTimer(CUIMessage* pMessage, DWORD dwTimerId, DWORD dwTimeOut, LPVOID lParam)
    {
        CUITimer* pTimer = s_ThreadInfo.pTimer;
        if (NULL == pTimer) return NULL;

        return pTimer->SetTimer(pMessage, dwTimerId, dwTimeOut, lParam);
    }

    BOOL CUIThread::KillTimer(LPMUITIMERITEM pItem)
    {
        CUITimer* pTimer = s_ThreadInfo.pTimer;
        if (NULL == pItem || NULL == pTimer) return FALSE;

        return pTimer->KillTimer(pItem);
    }

    BOOL CUIThread::WorkerAdd(DWORD dwWorkerId, IWorker* pWorker)
    {
        if (NULL == pWorker) return FALSE;

        RBNODE* pNode = rbtree_find(&s_ThreadInfo.WorkPool, (const void *)dwWorkerId);

        if (pNode) return FALSE;//已经存在

        pNode = rbtree_create_node((const void*)dwWorkerId, pWorker);

        if (NULL == pNode) return TRUE;

        return !rbtree_insert(&s_ThreadInfo.WorkPool, pNode);
    }

    RBNODE* CUIThread::WorkerRemove(DWORD dwWorkerId)
    {
        return rbtree_find(&s_ThreadInfo.WorkPool, (const void*)dwWorkerId);
    }

    BOOL CUIThread::WorkerActive(DWORD dwWorkerId, LPVOID lParam)//放到队列中异步执行
    {
        RBNODE* pNode = rbtree_find(&s_ThreadInfo.WorkPool, (const void*)dwWorkerId);
        if (NULL == pNode || NULL == pNode->data) return FALSE;

        if (!::IsWindow(s_hThreadProc)) return FALSE;

        return ::PostMessage(s_hThreadProc, CUIThread::UWM_WORKER, (WPARAM)pNode->data, (LPARAM)lParam);
    }

    BOOL CUIThread::WorkerExecute(DWORD dwWorkerId, LPVOID lParam, LRESULT* lResult)//立即执行
    {
        RBNODE* pNode = rbtree_find(&s_ThreadInfo.WorkPool, (const void*)dwWorkerId);
        if (NULL == pNode) return FALSE;

        IWorker* pWorker = (IWorker *)pNode->data;
        LRESULT lRet = pWorker->Exec(lParam);

        if (lResult) *lResult = lRet;

        return TRUE;
    }


    BOOL CUIThread::BuildMQCache()
    {
        return TRUE;
    }

    void CUIThread::FreeMQcache()
    {
        return;
    }

    MUIMESSAGENODE* CUIThread::MallocMQNode()
    {
        MUIMESSAGENODE* pNode = new MUIMESSAGENODE;
        memset(pNode, 0, sizeof(MUIMESSAGENODE));
        return 0;
    }

    void CUIThread::FreeMQNode(MUIMESSAGENODE* pNode)
    {
        delete pNode;
    }

    int __stdcall CUIThread::WorkTreeCompare(const void* object1, const void* object2)
    {
        return (int)((long long)object1 - (long long)object2);
    }

    int __stdcall CUIThread::WorkerDelete(RBNODE* node, void* param)
    {
        if (node && node->data)
        {
            delete node->data;
        }
        return 0;//返回true 结束遍历
    }

    LRESULT CALLBACK CUIThread::ThreadProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        MUITHREADINFO* pInfo = NULL;
        IWorker* pWorker = NULL;
        MUIMESSAGENODE* pNode = NULL;
        BOOL bRet = TRUE;
        switch (uMsg)
        {
        case WM_NCCREATE://第二条消息
        {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            pInfo = static_cast<MUITHREADINFO*>(lpcs->lpCreateParams);
            ::SetWindowLongPtr(hwnd, 0, reinterpret_cast<LPARAM>(pInfo));

            return TRUE;
        }break;
        case WM_NCDESTROY://能够收到的，最后一条消息，倒数第二条
        {
            pInfo = reinterpret_cast<MUITHREADINFO*>(::GetWindowLongPtr(hwnd, 0));

        }break;
        case CUIThread::UWM_TIMER:
        {
            pInfo = reinterpret_cast<MUITHREADINFO*>(::GetWindowLongPtr(hwnd, 0));
            CUITimer* pTimer = pInfo ? pInfo->pTimer :  NULL;
            if (CUITimer::TimerId == wParam && pTimer)
            {
                pTimer->ActiveTimer(hwnd);
            }
            return 0;
        }break;
        case CUIThread::UWM_WORKER:
        {
            if (wParam)
            {
                pWorker = (IWorker*)wParam;
                pWorker->Exec((LPVOID)lParam);
            }
            return !!wParam;
        }break;
        case CUIThread::UWM_MESSAGE:
        {
            bRet = TRUE;
            pNode = (MUIMESSAGENODE*)lParam;

            CUIMessage* pMessage = pNode ? pNode->Message.pMessage : NULL;
            MUIMESSAGE* pMsg = pNode ? &pNode->Message : NULL;
            if (pMessage && TRUE == pNode->bValid)
            {
                //如果bRet为FALSE, 必然是出现了某种逻辑错误
                bRet = pMessage->RemoveMessage(pNode);
                pMessage->CallWndProc(pMsg->uMsg, pMsg->wParam, pMsg->lParam);
            }

            MUIASSERT(TRUE == bRet && "RemoveMessage失败，界面库必然出现了逻辑错误!");
            return bRet;
        }break;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
};
