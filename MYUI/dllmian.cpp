
#include <Windows.h>


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: //�������ó�ʼ��
		{
			DisableThreadLibraryCalls((HMODULE)hModule);
		}break;
	case DLL_THREAD_ATTACH: //�߳�ÿ������ʱ
		{
			DisableThreadLibraryCalls((HMODULE)hModule);
		}break;
	case DLL_THREAD_DETACH: //�߳��˳�ʱ
		{

		}break;
	case DLL_PROCESS_DETACH: //��̬��ӳ����
		{
		}break;
	}
    return TRUE;

}