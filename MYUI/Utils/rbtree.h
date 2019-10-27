#ifndef __RED_BLACK_TREE_H__
#define __RED_BLACK_TREE_H__


//typedef struct __RBKEY
//{
//    unsigned __int64 LowPart;
//    unsigned __int64 HeightPart;
//}RBKEY;

typedef struct __RBKEY
{
#ifdef _UNICODE
    wchar_t strText[260];
#else
    char strText[260];
#endif
}RBKEY;

// 红黑树的节点
typedef struct __RBNODE{
    RBKEY   key;                    // 关键字(键值)
    unsigned char color;        // 颜色(RED 或 BLACK)
    struct __RBNODE *left;    // 左孩子
    struct __RBNODE *right;    // 右孩子
    struct __RBNODE *parent;    // 父结点
    void * data;
}RBNODE;

// 红黑树的根
typedef struct __RBTREEROOT{
    RBNODE *node;
}RBTREEROOT;

#ifdef __cplusplus
extern "C" {
#endif

    RBNODE *  rbtree_find(RBTREEROOT *root, const RBKEY * key);

#define rbtree_node_data(node)  ((node) ? (node)->data : 0)

    //创建Node的时候会进行init
    RBNODE * rbtree_create_node(const RBKEY * key, void * data);

    //自己new一个Node的时候可以使用该函数
    void rbtree_init_node(RBNODE * node, const RBKEY * key, void * data);

    void rbtree_free_node(RBNODE * node);

    // 销毁红黑树
    void rbtree_destroy(RBTREEROOT *root);

    RBNODE * rbtree_get_min(RBTREEROOT * root);

    RBNODE * rbtree_get_max(RBTREEROOT * root);


    // 将结点插入到红黑树中。插入成功，返回0；失败返回-1。
    int rbtree_insert(RBTREEROOT *root, RBNODE *node);

    // 删除结点(key为节点的值)
    RBNODE * rbtree_remove(RBTREEROOT *root, const RBKEY * key);


#define RBTREE_ENUM_PRE  0x01//前序
#define RBTREE_ENUM_MID  0x02//中序
#define RBTREE_ENUM_AFT  0x04//后序
    //默认应返回0，不为 0 将会结束枚举
    typedef int(__stdcall * rbtree_enum_fnptr)(RBNODE * node, void * param);
    void rbtree_enum(RBTREEROOT * root, int etype, rbtree_enum_fnptr eproc, void * eparam);

    void rbtree_print(RBTREEROOT *root);

#ifdef __cplusplus
};
#endif

#endif