
#include "AvlTree.h"
#include <memory.h>

#define HEIGHT(p)    ( (p==nullptr) ? -1 : (((AvlTreeNode *)(p))->height) )

#ifndef MAX
#define MAX(a, b)    ( (a) > (b) ? (a) : (b) )
#endif


int keycmp(NODEKEY * key1, NODEKEY * key2)
{
	int ret = 0 ;
	if((key1->type & key2->type) == KeyType::Int32)
	{
		ret = key1->int32 - key2->int32;
	}
	else if((key1->type & key2->type) == KeyType::String)
	{
		KEYCHAR * src = key1->string;
		KEYCHAR * dst = key2->string;
		while(!(ret=*src-*dst) && *dst)   //�����û�н���
			++src, ++dst;
	}
	else if((key1->type & key2->type) == KeyType::Int64)
	{
		ret = key1->int64 - key2->int64;
	}
	else if((key1->type & key2->type) == KeyType::Pointer)
	{
		ret = (char *)key1->pointer - (char *)key2->pointer;
	}

	return ret;
}

void initkey(NODEKEY * key, void * value)
{
	memset(key, 0, sizeof(NODEKEY));
	key->type = KeyType::Pointer;
	key->pointer = value;
}

void initkey(NODEKEY * key, __int32 value)
{
	memset(key, 0, sizeof(NODEKEY));
	key->type = KeyType::Int32;
	key->int32 = value;
}

void initkey(NODEKEY * key, __int64 value)
{
	memset(key, 0, sizeof(NODEKEY));
	key->type = KeyType::Int64;
	key->int64 = value;
}

void initkey(NODEKEY * key,const KEYCHAR * value)
{
	memset(key, 0, sizeof(NODEKEY));
	key->type = KeyType::String;
	
	for(int i=0; 0 != value[i]; i++)
	{
		key->string[i] = value[i];
	}
}

CAvlTree::CAvlTree()
	: m_pRootNode(nullptr)
	, m_keyType(KeyType::Not)
{
}

CAvlTree::~CAvlTree()
{
	destroy_avltree(m_pRootNode);
}

//���÷����Ķ�̬
bool CAvlTree::insert(void * key, void * pData)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return insert(&_Key, pData);
}

bool CAvlTree::insert(__int32 key, void * pData)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return insert(&_Key, pData);
}

bool CAvlTree::insert(__int64 key, void * pData)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return insert(&_Key, pData);
}

bool CAvlTree::insert(const KEYCHAR * key, void *pData)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return insert(&_Key, pData);
}

void * CAvlTree::remove(void * key)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return remove(&_Key);
}

void * CAvlTree::remove(__int32 key)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return remove(&_Key);
}

void * CAvlTree::remove(__int64 key)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return remove(&_Key);
}

void * CAvlTree::remove(const KEYCHAR * key)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return remove(&_Key);
}

void * CAvlTree::find(void * key)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return find(&_Key);
}

void * CAvlTree::find(__int32 key)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return find(&_Key);
}

void * CAvlTree::find(__int64 key)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return find(&_Key);
}

void * CAvlTree::find(const KEYCHAR * key)
{
	NODEKEY _Key;
	initkey(&_Key, key);
	return find(&_Key);
}


 //���÷���
bool CAvlTree::insert(NODEKEY * key, void * pData)
{
	if(m_keyType != KeyType::Not && key->type != m_keyType) return false;
	m_keyType = key->type;
	m_pRootNode = avltree_insert(m_pRootNode, key, pData);

	return false;
}

void * CAvlTree::remove(NODEKEY * key)
{

    AvlTreeNode * z = nullptr;
	void * pData = nullptr;
	if(m_keyType != KeyType::Not && key->type != m_keyType) return nullptr;
	if ((z = avltree_search(m_pRootNode, key)) != nullptr)
	{
		pData = z->pData;
		m_pRootNode = delete_node(m_pRootNode, z);
	}
    
    return pData;
}

void * CAvlTree::find(NODEKEY * key)
{
    AvlTreeNode * z = nullptr;
	void * pData = nullptr;
	if(m_keyType != KeyType::Not && key->type != m_keyType) return nullptr;
	if ((z = avltree_search(m_pRootNode, key)) != nullptr)
	{
		pData = z->pData;
	}
    
    return pData;
}


int CAvlTree::height()
{
	return HEIGHT(m_pRootNode);
}

AvlTreeNode * CAvlTree::GetRoot()
{
	return m_pRootNode;
}

// LL�������Ӧ�����(����ת)��
AvlTreeNode * CAvlTree::left_left_rotation(AvlTreeNode * k2)
{
	AvlTreeNode * k1;

    k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;

    k2->height = MAX( HEIGHT(k2->left), HEIGHT(k2->right)) + 1;
    k1->height = MAX( HEIGHT(k1->left), k2->height) + 1;

    return k1;
}

// RR�����Ҷ�Ӧ�����(�ҵ���ת)��
AvlTreeNode * CAvlTree::right_right_rotation(AvlTreeNode * k1)
{
    AvlTreeNode * k2;

    k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;

    k1->height = MAX( HEIGHT(k1->left), HEIGHT(k1->right)) + 1;
    k2->height = MAX( HEIGHT(k2->right), k1->height) + 1;

    return k2;

}

// LR�����Ҷ�Ӧ�����(��˫��ת)��
AvlTreeNode * CAvlTree::left_right_rotation(AvlTreeNode * k3)
{
    k3->left = right_right_rotation(k3->left);

    return left_left_rotation(k3);
}

// RL�������Ӧ�����(��˫��ת)��
AvlTreeNode * CAvlTree::right_left_rotation(AvlTreeNode * k1)
{
    k1->right = left_left_rotation(k1->right);

    return right_right_rotation(k1);
}


AvlTreeNode * CAvlTree::avltree_create_node(NODEKEY * key, AvlTreeNode *left, AvlTreeNode* right)
{
    AvlTreeNode * p = new AvlTreeNode;

	if (p) 
	{
		p->key = *key;
		p->height = 0;
		p->left = left;
		p->right = right;
	}
    return p;
}


// (�ݹ�ʵ��)����"AVL��x"�м�ֵΪkey�Ľڵ�
AvlTreeNode * CAvlTree::avltree_search(AvlTreeNode * x, NODEKEY * key)
{
	if (x== nullptr || !keycmp(&x->key,key))
        return x;

    if (keycmp(&x->key, key) > 0)
        return avltree_search(x->left, key);
    else
        return avltree_search(x->right, key);
}


// (�ǵݹ�ʵ��)����"AVL��x"�м�ֵΪkey�Ľڵ�
AvlTreeNode * iterative_avltree_search(AvlTreeNode * x, NODEKEY * key)
{
	while ((x!= nullptr) && keycmp(&x->key, key))
    {
        if (keycmp(&x->key,key) > 0)
            x = x->left;
        else
            x = x->right;
    }

    return x;
}

AvlTreeNode* CAvlTree::avltree_insert(AvlTreeNode * node, NODEKEY * key, void * pData)
{
	if (node == nullptr) //��������ڵ�
    {
        // �½��ڵ�
        node = avltree_create_node(key, nullptr, nullptr);
        if (node == nullptr)
        {
            return nullptr;
        }
		node->pData = pData;
    }
	else if (keycmp(&node->key, key) > 0 ) // Ӧ�ý�key���뵽"tree��������"�����
    {
        node->left = avltree_insert(node->left, key, pData);//�ص�
        // ����ڵ����AVL��ʧȥƽ�⣬�������Ӧ�ĵ��ڡ�
        if (HEIGHT(node->left) - HEIGHT(node->right) == 2)
        {
            if (keycmp(&node->left->key, key) > 0)//key�嵽�����
                node = left_left_rotation(node);//˳ʱ����תһ��
            else//key�嵽�ұ���
                node = left_right_rotation(node);
        }
    }
    else if (0 > keycmp(&node->key, key)) // Ӧ�ý�key���뵽"tree��������"�����
    {
        node->right = avltree_insert(node->right, key, pData);
        // ����ڵ����AVL��ʧȥƽ�⣬�������Ӧ�ĵ��ڡ�
        if (HEIGHT(node->right) - HEIGHT(node->left) == 2)
        {
            if (keycmp(key, &node->right->key) > 0)
                node = right_right_rotation(node);
            else
                node = right_left_rotation(node);
        }
    }
    else //(key == node->key)
    {

    }

    node->height = MAX( HEIGHT(node->left), HEIGHT(node->right)) + 1;

    return node;
}

AvlTreeNode * CAvlTree::delete_node(AvlTreeNode * tree, AvlTreeNode *z)
{
    // ��Ϊ�� ���� û��Ҫɾ���Ľڵ㣬ֱ�ӷ���NULL��
	if (tree== nullptr || z == nullptr)
        return nullptr;

	if (keycmp(&tree->key, &z->key) > 0)        // ��ɾ���Ľڵ���"tree��������"��
    {
        tree->left = delete_node(tree->left, z);
        // ɾ���ڵ����AVL��ʧȥƽ�⣬�������Ӧ�ĵ��ڡ�
        if (HEIGHT(tree->right) - HEIGHT(tree->left) == 2)
        {
            AvlTreeNode *r =  tree->right;
            if (HEIGHT(r->left) > HEIGHT(r->right))
                tree = right_left_rotation(tree);
            else
                tree = right_right_rotation(tree);
        }
    }
	else if (keycmp(&z->key,&tree->key) > 0)// ��ɾ���Ľڵ���"tree��������"��
    {
        tree->right = delete_node(tree->right, z);
        // ɾ���ڵ����AVL��ʧȥƽ�⣬�������Ӧ�ĵ��ڡ�
        if (HEIGHT(tree->left) - HEIGHT(tree->right) == 2)
        {
            AvlTreeNode *l =  tree->left;
            if (HEIGHT(l->right) > HEIGHT(l->left))
                tree = left_right_rotation(tree);
            else
                tree = left_left_rotation(tree);
        }
    }
    else    // tree�Ƕ�ӦҪɾ���Ľڵ㡣
    {
        // tree�����Һ��Ӷ��ǿ�
        if ((tree->left) && (tree->right))
        {
            if (HEIGHT(tree->left) > HEIGHT(tree->right))
            {
                // ���tree�����������������ߣ�
                // ��(01)�ҳ�tree���������е����ڵ�
                //   (02)�������ڵ��ֵ��ֵ��tree��
                //   (03)ɾ�������ڵ㡣
                // ����������"tree�������������ڵ�"��"tree"������
                // �������ַ�ʽ�ĺô��ǣ�ɾ��"tree�������������ڵ�"֮��AVL����Ȼ��ƽ��ġ�
                AvlTreeNode *max = avltree_maximum(tree->left);
                tree->key = max->key;
                tree->left = delete_node(tree->left, max);
            }
            else
            {
                // ���tree��������������������(��������ȣ�������������������1)
                // ��(01)�ҳ�tree���������е���С�ڵ�
                //   (02)������С�ڵ��ֵ��ֵ��tree��
                //   (03)ɾ������С�ڵ㡣
                // ����������"tree������������С�ڵ�"��"tree"������
                // �������ַ�ʽ�ĺô��ǣ�ɾ��"tree������������С�ڵ�"֮��AVL����Ȼ��ƽ��ġ�
                AvlTreeNode *min = avltree_maximum(tree->right);
                tree->key = min->key;
                tree->right = delete_node(tree->right, min);
            }
        }
        else
        {
            AvlTreeNode *tmp = tree;
            tree = tree->left ? tree->left : tree->right;
            delete tmp;
        }
    }

    return tree;
}

void CAvlTree::destroy_avltree(AvlTreeNode * node)
{
	if (node == nullptr)
        return ;

    if (node->left != nullptr)
        destroy_avltree(node->left);
    if (node->right != nullptr)
        destroy_avltree(node->right);

    delete(node);
}

AvlTreeNode * CAvlTree::get_min_node()
{
	return avltree_minimum(m_pRootNode);
}
 
AvlTreeNode * CAvlTree::get_max_node()
{
	return avltree_maximum(m_pRootNode);
}

AvlTreeNode * CAvlTree::avltree_minimum(AvlTreeNode * node)
{
    if (node == nullptr) return nullptr;
    while(node->left != nullptr)
        node = node->left;
    return node;
}
 
AvlTreeNode * CAvlTree::avltree_maximum(AvlTreeNode * node)
{
    if (node == nullptr) return nullptr;
    while(node->right != nullptr)
        node = node->right;
    return node;
}

void CAvlTree::enum_node(EnumNodeType type, EnumAvlNodeProcPtr pFunc)
{
	switch (type)
	{
	case EnumNodeType::preType:
		{
			pre_enum(m_pRootNode,pFunc);
		}break;
	case EnumNodeType::midType:
		{
			mid_enum(m_pRootNode,pFunc);
		}break;
	case EnumNodeType::aftType:
		{
			aft_enum(m_pRootNode,pFunc);
		}break;
	default:
		break;
	}
}

bool CAvlTree::pre_enum(AvlTreeNode * node,EnumAvlNodeProcPtr pFunc)//����
{
	if(node != nullptr)
    {
        if(true == pFunc(node))
		{
			return true;
		}
        if(true == pre_enum(node->left,pFunc))
		{
			return true;
		}
        if(true == pre_enum(node->right,pFunc))
		{
			return true;
		}
    }
	return false;
}

bool CAvlTree::mid_enum(AvlTreeNode * node,EnumAvlNodeProcPtr pFunc)//����
{
	if(node != nullptr)
    {
        if(true == pFunc(node))
		{
			return true;
		}
		if(true == mid_enum(node->right,pFunc))
		{
			return true;
		}
        if(true == mid_enum(node->left,pFunc))
		{
			return true;
		}
    }
	return false;
}

bool CAvlTree::aft_enum(AvlTreeNode * node,EnumAvlNodeProcPtr pFunc)//����
{
	if(node != nullptr)
    {
        if(true == aft_enum(node->left,pFunc))
		{
			return true;
		}
        if(true == aft_enum(node->right,pFunc))
		{
			return true;
		}
		if(true == pFunc(node))
		{
			return true;
		}
    }
	return false;
}

