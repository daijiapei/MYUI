
#ifndef __BINARY_HEAP_H__
#define __BINARY_HEAP_H__

//对比两个对象的大小
typedef int(__stdcall *PFN_COMPAREOBJECT)(void * pObject1, void * pObject2);

//通知对象，他在堆的位置改变了
typedef void(__stdcall *PFN_UPDATEINDEX)(void * pObject, unsigned int uIndex);


typedef struct __HEAPELEMENT
{
    void * pObject;
}HEAPELEMENT;

typedef struct __BITHEAP
{
    unsigned int uIncrement;//空间不足时,每次递增多少空间
    unsigned int uCount;//元素数量
    HEAPELEMENT * lpArrayElement;
    PFN_COMPAREOBJECT pfnCompare;
    PFN_UPDATEINDEX pfnUpdate;
}BINARYHEAP;

//这里的算法是大/小根堆操作

#ifdef __cplusplus
extern "C" {
#endif

    __inline unsigned int binaryheap_get_count(BINARYHEAP * pBinaryHeap)
    {
        return pBinaryHeap->uCount;
    }

    BINARYHEAP * binaryheap_new(unsigned int uIncrement, PFN_COMPAREOBJECT pfnCompare, PFN_UPDATEINDEX pfnUpdate);
    void binaryheap_free(BINARYHEAP * pBinaryHeap);
    void binaryheap_clear(BINARYHEAP * pBinaryHeap);
    int binaryheap_top(BINARYHEAP * pBinaryHeap, HEAPELEMENT * pElem);
    int binaryheap_push(BINARYHEAP * pBinaryHeap, const HEAPELEMENT * pElem);
    int binaryheap_erase(BINARYHEAP * pBinaryHeap, unsigned int uIndex);
    int binaryheap_pop(BINARYHEAP * pBinaryHeap, HEAPELEMENT * pElem);

#ifdef __cplusplus
};
#endif


#endif