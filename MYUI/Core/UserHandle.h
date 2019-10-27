
#ifndef __MYUI_USER_HANDLE_H__
#define __MYUI_USER_HANDLE_H__

#include "../base.h"
#include "../Utils/Utils.h"

namespace MYUI
{

DECLARE_HANDLE            (HTIMER);

    //ʵ�ʵ�ҵ����, ������Ҫһ����Ϣ������ϵ, �����ϵ��UI����ͬһ���߳�, �����ֲ���һ������
    //���ǲ�����UserHandle�ӿ�, �ڶ�Ӧ���߳��м̳�IUserHandle���п���, ��ʹ���Callback������
    //UI����ͬһ���߳���.
    class MYUI_API IUserHandle
    {
        friend class CWindowUI;
        friend class CMenuUI;
        friend class CUserHandleTable;
    public:
        IUserHandle();
        virtual ~IUserHandle();

        DWORD GetAttachThreadId();

        //����Ϣ�Ž��̶߳�Ӧ�Ķ���
        BOOL PostMessage(UINT message, WPARAM wParam = NULL, LPARAM lParam = NULL);
        LRESULT SendMessage(UINT message, WPARAM wParam = NULL, LPARAM lParam = NULL);
        


        //����һ����Σ�յĺ���, ��ʹ��ǰ�߳�����,����Ȼ�᲻ͣ��������В�����Ϣ, 
        //��ʵ���Թ��ü��ֶ�ʱ����ʵ����, ����WM_TIMER, WaitableTimer, Ч������
        //����, ���������ڲ�����timeSetEvent, ʹ���������, Ҫע������е���Ϣ
        //�Ƿ��ܹ��õ���ʱ����, ����ܹ�����ʱ, ������������������з������
        //���ʵ�ڲ���, ����ʹ�ô��ڵ�WM_TIMER ���ж��⴦��
        BOOL SetTimer(UINT message, DWORD dwSecond, LPVOID lpArg);
        BOOL KillTimer(HTIMER hTimer);
        
    protected:
        virtual LRESULT Callback(UINT message, WPARAM wParam , LPARAM lParam) = 0;
       
        static void CALLBACK TimerRoutine(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
        virtual void OnAttach() = 0;//���ӵ�CUserHandleTableʱ��֪ͨ
		virtual void OnDetach() = 0;//�뿪CUserHandleTableʱ��֪ͨ

    protected:
        HINSTANCE m_hInstance;
    private:
        DWORD m_dwHandleAttachThreadId__;
    };


    class MYUI_API CUserHandleTable
    {
    public:
        //һ���߳�ֻ����һ��IUserHandle, �µ�IUserHandle �Ὣ�ɵ�IUserHandle�滻����
        //��Ҫ�ڶ�Ӧ��ShowModalǰ����, ������Ч
        static IUserHandle * AddHandle(IUserHandle * pHandle);

        static IUserHandle * RemoveHandle(DWORD dwThreadId);

        static IUserHandle * GetThreadHandle(DWORD dwThreadId);

        static void InitHandleTable();//����Ҫ�����߳��г�ʼ��
        static void FreeHandleTable();
        static DWORD GetMainThreadId();

        //ClearHandleTable ���IUserHandleִ��delete����
        //��Ϊ���̴߳����˳�,�����̵߳Ĵ��������ǲ�֪�������,
        //�����������IUserHandle�ͷ�
        static void ClearHandleTable();

    private:
        static CMuiPtrArray s_Items;
        static DWORD s_dwMainThreadId;
        static CRITICAL_SECTION s_Lock;
    };

};


#endif