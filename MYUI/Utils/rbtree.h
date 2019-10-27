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

// ������Ľڵ�
typedef struct __RBNODE{
    RBKEY   key;                    // �ؼ���(��ֵ)
    unsigned char color;        // ��ɫ(RED �� BLACK)
    struct __RBNODE *left;    // ����
    struct __RBNODE *right;    // �Һ���
    struct __RBNODE *parent;    // �����
    void * data;
}RBNODE;

// ������ĸ�
typedef struct __RBTREEROOT{
    RBNODE *node;
}RBTREEROOT;

#ifdef __cplusplus
extern "C" {
#endif

    RBNODE *  rbtree_find(RBTREEROOT *root, const RBKEY * key);

#define rbtree_node_data(node)  ((node) ? (node)->data : 0)

    //����Node��ʱ������init
    RBNODE * rbtree_create_node(const RBKEY * key, void * data);

    //�Լ�newһ��Node��ʱ�����ʹ�øú���
    void rbtree_init_node(RBNODE * node, const RBKEY * key, void * data);

    void rbtree_free_node(RBNODE * node);

    // ���ٺ����
    void rbtree_destroy(RBTREEROOT *root);

    RBNODE * rbtree_get_min(RBTREEROOT * root);

    RBNODE * rbtree_get_max(RBTREEROOT * root);


    // �������뵽������С�����ɹ�������0��ʧ�ܷ���-1��
    int rbtree_insert(RBTREEROOT *root, RBNODE *node);

    // ɾ�����(keyΪ�ڵ��ֵ)
    RBNODE * rbtree_remove(RBTREEROOT *root, const RBKEY * key);


#define RBTREE_ENUM_PRE  0x01//ǰ��
#define RBTREE_ENUM_MID  0x02//����
#define RBTREE_ENUM_AFT  0x04//����
    //Ĭ��Ӧ����0����Ϊ 0 �������ö��
    typedef int(__stdcall * rbtree_enum_fnptr)(RBNODE * node, void * param);
    void rbtree_enum(RBTREEROOT * root, int etype, rbtree_enum_fnptr eproc, void * eparam);

    void rbtree_print(RBTREEROOT *root);

#ifdef __cplusplus
};
#endif

#endif