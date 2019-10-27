
#include "Helper.h"

/**********************************************************************************************
//rcRawItem��ԭʼ�ģ�����ڸ����ֵ�λ�ã�Ҫͨ������ƫ�Ƽ��㣬���ܵó���ȷ�ģ�Ҫ��ʾ�ľ���λ��
//GetItemFixed����ͨ��rcPaintItem�����rcRawItem��HDC��Ҫ��ʾ�ľ���λ�ã����浽rcFixedItem��
//����true˵��rcFixedItem��Ч�� rcRawItem�ľ���λ����rcPaintItem�ཻ��������ʾ
//����false˵��rcFixedItem��Ч��rcRawItem�ľ���λ����rcPaintItem���ཻ�����ܹ���ʾ
//@rcFixedItem������ó�rcRawItem����HDC�еľ���λ��
//@rcRawItem���ڲ����е�ԭʼλ��
//@rcPaintItem��HDC�ṩ��rcRawItem����ʾ��Χ��rcRawItem�ľ��Է�Χ��rcPaintItem���ཻ�򷵻�true
//@bFloat���Ƿ��þ��Բ��ּ���
**********************************************************************************************/
BOOL GetItemFixed(RECT & rcFixedItem, RECT & rcRawItem, RECT & rcPaintItem, BOOL bFloat)
{
	RECT rect = rcRawItem;
	SIZE OffsetSize = {0};

	if(FALSE == bFloat)//���λ�ü�������ʾ
	{
		//���λ����ʾ��������rect
		OffsetSize.cx = ((rcPaintItem.right - rcPaintItem.left) - (rect.right - rect.left)) / 2;
		OffsetSize.cy = ((rcPaintItem.bottom - rcPaintItem.top) - (rect.bottom - rect.top)) / 2;

		//���Item���ھ���λ�ã����0,0λ�ÿ�ʼ��ʾ
		OffsetSize.cx = OffsetSize.cx > 0 ? OffsetSize.cx : 0;
		OffsetSize.cy = OffsetSize.cy > 0 ? OffsetSize.cy : 0;
	}

	//����rcItem �� rcPaintFixed�Ľ���λ��, �������㸴�ƵĿ�͸�
	OffsetRect(&rect , rcPaintItem.left + OffsetSize.cx , rcPaintItem.top + OffsetSize.cy);
	return IntersectRect(&rcFixedItem, &rect, &rcPaintItem);
}


//�ж����������Ƿ��ཻ
