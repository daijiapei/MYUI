
#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_

enum KeyType
{
	Not = 0,
	Pointer = 1,
	String,
	Int32,
	Int64
};

#ifdef _UNICODE
		typedef wchar_t KEYCHAR;
#else
		typedef char KEYCHAR;
#endif

typedef struct _NodeKey
{
	KeyType type;
	union
	{
		__int32  int32;
		__int64  int64;
		void * pointer;
		KEYCHAR string[128];
	};
}NODEKEY;

typedef struct _AvlTreeNode{
    NODEKEY key;                    // �ؼ���(��ֵ)
    int height;
    struct _AvlTreeNode *left;    // ����
    struct _AvlTreeNode *right;    // �Һ���
	void * pData;
}AvlTreeNode;

int keycmp(NODEKEY * key1, NODEKEY * key2);

typedef bool (__cdecl* EnumAvlNodeProcPtr)(AvlTreeNode *);

class CAvlTree
{
public:
	CAvlTree();
	~CAvlTree();
	AvlTreeNode * GetRoot();
	enum EnumNodeType
	{
		preType = 1,//����
		midType,//����
		aftType //����
	};

public:
	bool insert(__int32 key, void * pData);
	bool insert(__int64 key, void * pData);
	bool insert(void * key, void *pData);
	bool insert(const KEYCHAR * key, void *pData);
	void * remove(__int32 key);
	void * remove(__int64 key);
	void * remove(void * key);
	void * remove(const KEYCHAR * key);
	void * find(__int32 key);
	void * find(__int64 key);
	void * find(void * key);
	void * find(const KEYCHAR * key);

	bool insert(NODEKEY * key, void * pData);
	void * remove(NODEKEY * key);
	void * find(NODEKEY * key);

	int height();
	AvlTreeNode * get_min_node();
	AvlTreeNode * get_max_node();
	void enum_node(EnumNodeType type, EnumAvlNodeProcPtr pFunc);
protected:

	//����true��������
	virtual bool pre_enum(AvlTreeNode * node, EnumAvlNodeProcPtr pFunc);//����
	virtual bool mid_enum(AvlTreeNode * node, EnumAvlNodeProcPtr pFunc);//����
	virtual bool aft_enum(AvlTreeNode * node, EnumAvlNodeProcPtr pFunc);//����

private:
	AvlTreeNode * left_left_rotation(AvlTreeNode * k2);
	AvlTreeNode * right_right_rotation(AvlTreeNode * k1);
	AvlTreeNode * left_right_rotation(AvlTreeNode * k3);
	AvlTreeNode * right_left_rotation(AvlTreeNode * k1);
	AvlTreeNode * avltree_maximum(AvlTreeNode * node);
	AvlTreeNode * avltree_minimum(AvlTreeNode * node);
	
	AvlTreeNode * avltree_search(AvlTreeNode *x, NODEKEY * key);
	AvlTreeNode * iterative_avltree_search(AvlTreeNode * x, NODEKEY * key);

	AvlTreeNode * avltree_insert(AvlTreeNode * node, NODEKEY * key, void * pData);
	AvlTreeNode * avltree_create_node(NODEKEY * key, AvlTreeNode *left, AvlTreeNode* right);
	AvlTreeNode * delete_node(AvlTreeNode * tree, AvlTreeNode *z);
	
	void destroy_avltree(AvlTreeNode * node);
private:
	AvlTreeNode * m_pRootNode;
	KeyType m_keyType;
};

#endif