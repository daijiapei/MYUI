
#include "binaryheap.h"
#include <stdlib.h>
#include <assert.h>
#define BHMALLOC   malloc
#define BHREALLOC  realloc
#define BHFREE     free


BINARYHEAP * binaryheap_new(unsigned int uIncrement, PFN_COMPAREOBJECT pfnCompare, PFN_UPDATEINDEX pfnUpdate)
{
    if (0 == uIncrement || 0 == pfnUpdate || 0 == pfnUpdate) return 0;
    BINARYHEAP * pBinaryHeap = (BINARYHEAP *)BHMALLOC(sizeof(BINARYHEAP));
    memset(pBinaryHeap, 0, sizeof(BINARYHEAP));
    pBinaryHeap->uIncrement = uIncrement;
    pBinaryHeap->pfnCompare = pfnCompare;
    pBinaryHeap->pfnUpdate = pfnUpdate;
    pBinaryHeap->lpArrayElement = (HEAPELEMENT *)BHMALLOC(sizeof(HEAPELEMENT) * uIncrement);

    if (0 == pBinaryHeap->lpArrayElement)
    {
        BHFREE(pBinaryHeap);
        pBinaryHeap = 0;
    }
    return pBinaryHeap;
}

void binaryheap_free(BINARYHEAP * pBinaryHeap)
{
    if (pBinaryHeap->lpArrayElement)
    {
        BHFREE(pBinaryHeap->lpArrayElement);
    }
    BHFREE(pBinaryHeap);
}

void binaryheap_clear(BINARYHEAP * pBinaryHeap)
{
    if (pBinaryHeap->uCount > pBinaryHeap->uIncrement)
    {
        //��Ҫ��С
        //��������С, ������һ���ɹ���
        pBinaryHeap->lpArrayElement = (HEAPELEMENT*)BHREALLOC(pBinaryHeap->lpArrayElement, pBinaryHeap->uIncrement * sizeof(HEAPELEMENT));
        assert(pBinaryHeap->lpArrayElement && "binaryheap_clear ��������Ԥ�ϵ����!");
    }

    for (unsigned int i = 0; pBinaryHeap->uCount > i; i++)
    {
        pBinaryHeap->pfnUpdate(pBinaryHeap->lpArrayElement[i].pObject, -1);
    }

    pBinaryHeap->uCount = 0;
    memset(pBinaryHeap->lpArrayElement, 0, pBinaryHeap->uIncrement * sizeof(HEAPELEMENT));
}

int binaryheap_top(BINARYHEAP * pBinaryHeap, HEAPELEMENT * pElem)
{
    assert(pBinaryHeap && pElem && "binaryheap_top ������Ч��ָ��, ����!");

    if (0 == pBinaryHeap->uCount) return -1;

    *pElem = pBinaryHeap->lpArrayElement[0];

    return 0;
}

int binaryheap_pop(BINARYHEAP * pBinaryHeap, HEAPELEMENT * pElem)
{
    if (0 == binaryheap_top(pBinaryHeap, pElem))
    {
        return binaryheap_erase(pBinaryHeap, 0);
    }

    return -1;
}

int binaryheap_push(BINARYHEAP * pBinaryHeap, const HEAPELEMENT * pElem)
{
    unsigned int uBytes = 0;
    HEAPELEMENT * lpArray = 0;
    unsigned int uIndex = 0;
    unsigned int j = 0;
    
    assert(pBinaryHeap && pElem && "binaryheap_top ������Ч��ָ��, ����!");

    if (pBinaryHeap->uCount > 0 && 0 == pBinaryHeap->uCount % pBinaryHeap->uIncrement)
    {
        //�ڴ�պù���, ���ڻ�Ҫ����һ��, �Ǿ���Ҫ���·���ռ�
        uBytes = sizeof(HEAPELEMENT) * (pBinaryHeap->uCount + pBinaryHeap->uIncrement);
        if (0 == (lpArray = (HEAPELEMENT*)BHREALLOC(pBinaryHeap->lpArrayElement, uBytes)))
        {
            return -1;
        }

        pBinaryHeap->lpArrayElement = lpArray;
        memset(pBinaryHeap->lpArrayElement + pBinaryHeap->uCount, 0, sizeof(HEAPELEMENT) * pBinaryHeap->uIncrement);
    }

    //�ڴ������Ѵ����, ���濪ʼ��ʽ����

    lpArray = pBinaryHeap->lpArrayElement;
    uIndex = pBinaryHeap->uCount;//nIndexָ�������Ԫ�ص�λ�ã����������±꣬��ʼָ����Ԫ�����ڵĶ�βλ��

    lpArray[pBinaryHeap->uCount++] = *pElem;//���β�����Ԫ��, ���������鳤��

    while (0 != uIndex)
    {
        j = (uIndex - 1) / 2; //jָ���±�ΪuIndex��Ԫ�ص�˫��
        //if (pElem->dwValue >= lpArray[j].dwValue) //����Ԫ�ش��ڴ�����Ԫ�ص�˫�ף���Ƚϵ����������˳�ѭ��
        if (pBinaryHeap->pfnCompare(pElem->pObject, lpArray[j].pObject) >= 0)
            break;

        lpArray[uIndex] = lpArray[j]; //��˫��Ԫ�����Ƶ�������Ԫ�ص�λ��

        pBinaryHeap->pfnUpdate(lpArray[uIndex].pObject, uIndex);

        uIndex = j; //ʹ������λ�ñ�Ϊ��˫��λ�ã�������һ��ѭ��
    }
    lpArray[uIndex] = *pElem;//����Ԫ�ص���������λ��
    pBinaryHeap->pfnUpdate(pElem->pObject, uIndex);
    return 0;
}

int binaryheap_erase(BINARYHEAP * pBinaryHeap, unsigned int uIndex)
{
    unsigned int uBytes = 0;
    HEAPELEMENT * lpArray = 0;
    //unsigned long uIndex = 0;//��nIndexָ�������Ԫ�ص�λ�ã���ʼָ��Ѷ�λ��
    unsigned long j = 0;//j��Ĭ��ָ�����, �����߱��ұߴ�,��ôָ���ұ�
    HEAPELEMENT Element = { 0 };
    int bReAlloc = 0;

    assert(pBinaryHeap && "binaryheap_erase ������Ч��ָ��, ����!");

    if (uIndex > pBinaryHeap->uCount) return -1;

    lpArray = pBinaryHeap->lpArrayElement;

    pBinaryHeap->pfnUpdate(lpArray[uIndex].pObject, -1);//�Ѿ����Ƴ�

    if (0 == --pBinaryHeap->uCount) return 0;

    Element = lpArray[pBinaryHeap->uCount]; //����������ԭ��βԪ���ݴ�Element�У��Ա��������λ��
    j = 2 * uIndex + 1;//��jָ��uIndex������λ�ã���ʼָ���±�Ϊ1��λ��

    while (pBinaryHeap->uCount - 1 >= j)//Ѱ�Ҵ�����Ԫ�ص�����λ�ã�ÿ��ʹ����Ԫ������һ�㣬����������Ϊ��ʱֹ
    {
        //�������Һ����ҽ�С��ʹjָ���Һ���
        //if (pMinHeap->dwCount - 1 > j && lpArray[j].dwValue > lpArray[j + 1].dwValue)//����Ҵ�

        if (pBinaryHeap->uCount - 1 > j && pBinaryHeap->pfnCompare(lpArray[j].pObject, lpArray[j + 1].pObject) > 0)//����Ҵ�
            j++;//ָ���ұ�, ָ��С��λ��

        //if (lpArray[j].dwValue >= Element.dwValue) 
        if (pBinaryHeap->pfnCompare(lpArray[j].pObject, Element.pObject) >= 0)//��Element�����С�ĺ��ӻ�С��������������˳�ѭ��
            break;

        lpArray[uIndex] = lpArray[j];//���򣬽�����Ԫ���Ƶ�˫��λ��
        pBinaryHeap->pfnUpdate(lpArray[uIndex].pObject, uIndex);

        uIndex = j; //��������λ�ñ�Ϊ���С�ĺ���λ��
        j = 2 * uIndex + 1;//��j��Ϊ�µĴ�����λ�õ�����λ�ã�������һ��ѭ��
    }

    lpArray[uIndex] = Element;
    pBinaryHeap->pfnUpdate(Element.pObject, uIndex);

    if (pBinaryHeap->uCount >= pBinaryHeap->uIncrement && 0 == pBinaryHeap->uCount % pBinaryHeap->uIncrement)
    {
        //��ʱ�ڴ�պö��һ��, ���ڻ��Ƴ�һ��, �Ǿ����·���ռ�, ���ռ�ѹ������С
        uBytes = sizeof(HEAPELEMENT) * (pBinaryHeap->uCount);

        //��Ϊ����С�ռ�, ����realloc�ǲ���ʧ�ܵ�
        pBinaryHeap->lpArrayElement = (HEAPELEMENT*)BHREALLOC(pBinaryHeap->lpArrayElement, uBytes);

        assert(lpArray == pBinaryHeap->lpArrayElement && "binaryheap_erase ��������Ԥ�ϵ����!");
    }
    return 0;
}
