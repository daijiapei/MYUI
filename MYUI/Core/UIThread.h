
#ifndef __MYUI_UITHREAD_MANAGEER_H__
#define __MYUI_UITHREAD_MANAGEER_H__

#include "../base.h"
#include "../Utils/Utils.h"
#include "../Utils/binaryheap.h"
#include "../Utils/rbtree.h"

namespace MYUI
{
    class CUIMessage;
    typedef struct __MUIMESSAGE
    {
        HWND        hWnd;
        UINT        uMsg;
        WPARAM      wParam;
        LPARAM      lParam;
        DWORD       dwTime;
        POINT       Point;
        CUIMessage* pMessage;
    }MUIMESSAGE, * LPMUIMESSAGE;

    typedef struct __MUIMESSAGE_NODE {
        BOOL bCache;
        BOOL bValid;
        struct __MUIMESSAGE_NODE* pPrev;
        struct __MUIMESSAGE_NODE* pNext;
        MUIMESSAGE Message;
        
    } MUIMESSAGENODE, LPMUIMESSAGENODE;

    class MYUI_API CUIMessage
    {
        friend class CUIThread;
    public:
        CUIMessage();
        virtual ~CUIMessage();

        virtual LRESULT CallWndProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
        virtual LRESULT SendMessage(UINT message, WPARAM wParam = 0L, LPARAM lParam = NULL);
        virtual BOOL PostMessage(UINT message, WPARAM wParam = 0L, LPARAM lParam = NULL);

        BOOL SetTimer(UINT nIDEvnet, UINT uElapse);
        BOOL KillTimer(UINT nIDEvnet);

    protected:
        virtual void OnAttach(HWND hNewWnd);//当控件附加到新的窗口时，会调用一次该函数
        virtual void OnDetach(HWND hOldWnd);//当控件离开一个窗口时，会调用一次该函数 
    private:
        BOOL RemoveMessage(MUIMESSAGENODE* pNode);
        BOOL AddMessage(MUIMESSAGENODE* pNode);

    private:
        CMuiPtrArray m_Timers;
        MUIMESSAGENODE* m_pNodeFrist;
        MUIMESSAGENODE* m_pNodeLast;
    };

/*************************************************************************
*
*
*
*
*************************************************************************/

    class MYUI_API IWorker
    {
    public:
        IWorker() {};
        virtual ~IWorker() {};

        LRESULT Exec(LPVOID lParam = NULL)
        {
            return Callback(lParam);
        }
    protected:
        virtual LRESULT Callback(LPVOID lParam) = 0;
    };

/*************************************************************************
*
*
*
*
*************************************************************************/
    typedef struct __MUITIMER_ITEM
    {
        UINT        uIndex;
        CUIMessage* pMessage;
        DWORD       dwType;
        DWORD       dwTimerId;
        DWORD       dwTimeOut;
        time_t      tiAbsoluteTimeOut;
        WPARAM      wParam;
        LPARAM      lParam;
        struct __MUITIMER_ITEM* pPrev;
        struct __MUITIMER_ITEM* pNext;
    }MUITIMERITEM, * LPMUITIMERITEM;

    class CUITimer
    {
    public:
        CUITimer(HWND hTimerWnd);
        ~CUITimer();
        enum
        {
            TimerId = -1,
        };
        

    public:
        DWORD GetWaitCount();
        DWORD ActiveTimer(HWND hWnd);
        
        LPMUITIMERITEM SetTimer(CUIMessage* pMessage, DWORD dwTimerId, DWORD dwTimeOut, LPVOID lParam);
        BOOL KillTimer(LPMUITIMERITEM pItem);
        
    private:
        BOOL RegditTimer(HWND hWnd);
        BOOL RegditQueuePush(LPMUITIMERITEM pItem);
        LPMUITIMERITEM RegditQueuePopup();
        BOOL RegditQueueErase(LPMUITIMERITEM pItem);

        static int __stdcall TimeCompare(void* pObject1, void* pObject2);
        static void __stdcall UpdateIndex(void* pObject, unsigned int uIndex);
    private:
        BINARYHEAP* m_pMinHeap;
        LPMUITIMERITEM m_pRegditFirst;
        LPMUITIMERITEM m_pRegditLast;
        HWND m_hTimerWnd;
    };
/*************************************************************************
*
*
*
*
*************************************************************************/

    typedef struct __MUITHREADINFO
    {
        DWORD dwThreadId;
        CUITimer* pTimer;
        RBTREEROOT WorkPool;
    }MUITHREADINFO;

    class MYUI_API CUIThread
    {
        friend class CMYTimer;
        friend class CUIMessage;
    public:
        enum
        {
            UWM_TIMER = WM_TIMER,
            UWM_WORKER = WM_USER + 1,
            UWM_MESSAGE = WM_USER + 2,
        };
    public:
        static BOOL Init(HINSTANCE hInstance);
        static void Uninit();

        static BOOL WorkerAdd(DWORD dwWorkerId, IWorker* pWorker);
        static RBNODE* WorkerRemove(DWORD dwWorkerId);
        static BOOL WorkerActive(DWORD dwWorkId, LPVOID lParam);//放到队列中异步执行
        static BOOL WorkerExecute(DWORD dwWorkId, LPVOID lParam, LRESULT* lResult = NULL);//立即执行

    private:
        static LPMUITIMERITEM SetTimer(CUIMessage* pMessage, DWORD dwTimerId, DWORD dwTimeOut, LPVOID lParam);
        static BOOL KillTimer(LPMUITIMERITEM pItem);
        static MUIMESSAGENODE* PostMessage(CUIMessage* pMessage, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        static int __stdcall WorkTreeCompare(const void* object1, const void* object2);
        static int __stdcall WorkerDelete(RBNODE* node, void* param);
        static LRESULT CALLBACK ThreadProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

        
        static BOOL BuildMQCache();
        static void FreeMQcache();
        static MUIMESSAGENODE* MallocMQNode();
        static void FreeMQNode(MUIMESSAGENODE* pNode);
        

    private:
        static HINSTANCE s_hInstance;
        static HWND s_hThreadProc;
        static MUITHREADINFO s_ThreadInfo;
    };


};



#endif