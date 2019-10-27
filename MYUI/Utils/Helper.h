
#ifndef _MYUI_HELPER_H_
#define _MYUI_HELPER_H_
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL GetItemFixed(RECT & rcFixedItem, RECT & rcRawItem, RECT & rcPaintItem, BOOL bFloat);

#ifdef __cplusplus
}
#endif

#endif