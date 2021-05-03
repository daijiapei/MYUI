
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
        //需要缩小
        //由于是缩小, 这里是一定成功的
        pBinaryHeap->lpArrayElement = (HEAPELEMENT*)BHREALLOC(pBinaryHeap->lpArrayElement, pBinaryHeap->uIncrement * sizeof(HEAPELEMENT));
        assert(pBinaryHeap->lpArrayElement && "binaryheap_clear 发生不可预料的情况!");
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
    assert(pBinaryHeap && pElem && "binaryheap_top 传入无效的指针, 请检查!");

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
    
    assert(pBinaryHeap && pElem && "binaryheap_top 传入无效的指针, 请检查!");

    if (pBinaryHeap->uCount > 0 && 0 == pBinaryHeap->uCount % pBinaryHeap->uIncrement)
    {
        //内存刚好够用, 现在还要插入一个, 那就需要重新分配空间
        uBytes = sizeof(HEAPELEMENT) * (pBinaryHeap->uCount + pBinaryHeap->uIncrement);
        if (0 == (lpArray = (HEAPELEMENT*)BHREALLOC(pBinaryHeap->lpArrayElement, uBytes)))
        {
            return -1;
        }

        pBinaryHeap->lpArrayElement = lpArray;
        memset(pBinaryHeap->lpArrayElement + pBinaryHeap->uCount, 0, sizeof(HEAPELEMENT) * pBinaryHeap->uIncrement);
    }

    //内存问题已处理好, 下面开始正式插入

    lpArray = pBinaryHeap->lpArrayElement;
    uIndex = pBinaryHeap->uCount;//nIndex指向待调整元素的位置，即其数组下标，初始指向新元素所在的堆尾位置

    lpArray[pBinaryHeap->uCount++] = *pElem;//向堆尾添加新元素, 并增加数组长度

    while (0 != uIndex)
    {
        j = (uIndex - 1) / 2; //j指向下标为uIndex的元素的双亲
        //if (pElem->dwValue >= lpArray[j].dwValue) //若新元素大于待调整元素的双亲，则比较调整结束，退出循环
        if (pBinaryHeap->pfnCompare(pElem->pObject, lpArray[j].pObject) >= 0)
            break;

        lpArray[uIndex] = lpArray[j]; //将双亲元素下移到待调整元素的位置

        pBinaryHeap->pfnUpdate(lpArray[uIndex].pObject, uIndex);

        uIndex = j; //使待调整位置变为其双亲位置，进行下一次循环
    }
    lpArray[uIndex] = *pElem;//把新元素调整到最终位置
    pBinaryHeap->pfnUpdate(pElem->pObject, uIndex);
    return 0;
}

int binaryheap_erase(BINARYHEAP * pBinaryHeap, unsigned int uIndex)
{
    unsigned int uBytes = 0;
    HEAPELEMENT * lpArray = 0;
    //unsigned long uIndex = 0;//用nIndex指向待调整元素的位置，初始指向堆顶位置
    unsigned long j = 0;//j是默认指向左边, 如果左边比右边大,那么指向右边
    HEAPELEMENT Element = { 0 };
    int bReAlloc = 0;

    assert(pBinaryHeap && "binaryheap_erase 传入无效的指针, 请检查!");

    if (uIndex > pBinaryHeap->uCount) return -1;

    lpArray = pBinaryHeap->lpArrayElement;

    pBinaryHeap->pfnUpdate(lpArray[uIndex].pObject, -1);//已经被移除

    if (0 == --pBinaryHeap->uCount) return 0;

    Element = lpArray[pBinaryHeap->uCount]; //将待调整的原堆尾元素暂存Element中，以便放入最终位置
    j = 2 * uIndex + 1;//用j指向uIndex的左孩子位置，初始指向下标为1的位置

    while (pBinaryHeap->uCount - 1 >= j)//寻找待调整元素的最终位置，每次使孩子元素上移一层，调整到孩子为空时止
    {
        //若存在右孩子且较小，使j指向右孩子
        //if (pMinHeap->dwCount - 1 > j && lpArray[j].dwValue > lpArray[j + 1].dwValue)//左比右大

        if (pBinaryHeap->uCount - 1 > j && pBinaryHeap->pfnCompare(lpArray[j].pObject, lpArray[j + 1].pObject) > 0)//左比右大
            j++;//指向右边, 指向小的位置

        //if (lpArray[j].dwValue >= Element.dwValue) 
        if (pBinaryHeap->pfnCompare(lpArray[j].pObject, Element.pObject) >= 0)//若Element比其较小的孩子还小，则调整结束，退出循环
            break;

        lpArray[uIndex] = lpArray[j];//否则，将孩子元素移到双亲位置
        pBinaryHeap->pfnUpdate(lpArray[uIndex].pObject, uIndex);

        uIndex = j; //将待调整位置变为其较小的孩子位置
        j = 2 * uIndex + 1;//将j变为新的待调整位置的左孩子位置，继续下一次循环
    }

    lpArray[uIndex] = Element;
    pBinaryHeap->pfnUpdate(Element.pObject, uIndex);

    if (pBinaryHeap->uCount >= pBinaryHeap->uIncrement && 0 == pBinaryHeap->uCount % pBinaryHeap->uIncrement)
    {
        //此时内存刚好多出一个, 现在还移除一个, 那就重新分配空间, 将空间压缩到最小
        uBytes = sizeof(HEAPELEMENT) * (pBinaryHeap->uCount);

        //因为是缩小空间, 所以realloc是不会失败的
        pBinaryHeap->lpArrayElement = (HEAPELEMENT*)BHREALLOC(pBinaryHeap->lpArrayElement, uBytes);

        assert(lpArray == pBinaryHeap->lpArrayElement && "binaryheap_erase 发生不可预料的情况!");
    }
    return 0;
}
