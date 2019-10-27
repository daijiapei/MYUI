
#include "Helper.h"

/**********************************************************************************************
//rcRawItem是原始的，相对于父布局的位置，要通过布局偏移计算，才能得出正确的，要显示的绝对位置
//GetItemFixed函数通过rcPaintItem计算出rcRawItem在HDC中要显示的绝对位置，保存到rcFixedItem中
//返回true说明rcFixedItem有效， rcRawItem的绝对位置与rcPaintItem相交，可以显示
//返回false说明rcFixedItem无效，rcRawItem的绝对位置与rcPaintItem不相交，不能够显示
//@rcFixedItem：计算得出rcRawItem，在HDC中的绝对位置
//@rcRawItem：在布局中的原始位置
//@rcPaintItem：HDC提供给rcRawItem的显示范围，rcRawItem的绝对范围与rcPaintItem不相交则返回true
//@bFloat：是否用绝对布局计算
**********************************************************************************************/
BOOL GetItemFixed(RECT & rcFixedItem, RECT & rcRawItem, RECT & rcPaintItem, BOOL bFloat)
{
	RECT rect = rcRawItem;
	SIZE OffsetSize = {0};

	if(FALSE == bFloat)//相对位置计算与显示
	{
		//相对位置显示则尽量居中rect
		OffsetSize.cx = ((rcPaintItem.right - rcPaintItem.left) - (rect.right - rect.left)) / 2;
		OffsetSize.cy = ((rcPaintItem.bottom - rcPaintItem.top) - (rect.bottom - rect.top)) / 2;

		//如果Item大于绝对位置，则从0,0位置开始显示
		OffsetSize.cx = OffsetSize.cx > 0 ? OffsetSize.cx : 0;
		OffsetSize.cy = OffsetSize.cy > 0 ? OffsetSize.cy : 0;
	}

	//计算rcItem 和 rcPaintFixed的交集位置, 用来计算复制的宽和高
	OffsetRect(&rect , rcPaintItem.left + OffsetSize.cx , rcPaintItem.top + OffsetSize.cy);
	return IntersectRect(&rcFixedItem, &rect, &rcPaintItem);
}


//判断两个矩形是否相交
