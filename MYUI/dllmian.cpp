
#include <Windows.h>


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: //进程引用初始化
		{
			DisableThreadLibraryCalls((HMODULE)hModule);
		}break;
	case DLL_THREAD_ATTACH: //线程每次引用时
		{
			DisableThreadLibraryCalls((HMODULE)hModule);
		}break;
	case DLL_THREAD_DETACH: //线程退出时
		{

		}break;
	case DLL_PROCESS_DETACH: //动态库映像撤销
		{
		}break;
	}
    return TRUE;

}