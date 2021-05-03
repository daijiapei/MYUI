
#include "LanguageEdition.h"


namespace MYUI
{

	int __stdcall CLanguageEdition::RBTreeCompare(const void* pObject1, const void* pObject2)
	{
		return _tcscmp((LPCTSTR)pObject1, (LPCTSTR)pObject2);
	}

	MUIEDITION* CLanguageEdition::AddLanguage(LPCTSTR strFileName, LPCTSTR strEdition, const char* strFileDate, int nSize)
	{
		MUILANGUAGE* pLanguage = NULL;
		MUIEDITION* pEdition = NULL;
		for (int i = 0; s_LanguageArray.GetSize() > i; i++)
		{
			MUILANGUAGE* p = (MUILANGUAGE*)s_LanguageArray[i];

			if (0 == _tcsicmp(p->strName, strFileName))
			{
				pLanguage = p;
			}
		}

		if (NULL == pLanguage)
		{
			pLanguage = new MUILANGUAGE;
			_tcscpy(pLanguage->strName, strFileName);
			pLanguage->Document.Parse(strFileDate);

			if (pLanguage->Document.Error())
			{
				delete pLanguage;
				return NULL;
			}

			if (stricmp(pLanguage->Document.RootElement()->Value(), "language"))
			{
				delete pLanguage;
				return NULL;
			}
			s_LanguageArray.Add(pLanguage);
		}


		for (int i = 0; pLanguage->EditionArray.GetSize(); i++)
		{
			MUIEDITION* p = (MUIEDITION *)pLanguage->EditionArray[i];
			if (0 == _tcsicmp(p->strName, strFileName))
			{
				pEdition = p;
			}
		}

		if (NULL == pEdition)
		{
			pEdition = new MUIEDITION;
			memset(pEdition, 0, sizeof(MUIEDITION));
			_tcscpy(pEdition->strName, strFileName);
			pEdition->pLanguage = pLanguage;
			pEdition->Root.cmpfn = RBTreeCompare;

			pLanguage->EditionArray.Add(pEdition);
			//对象已经初始化完成，开始加载字典

			SetDictionary(pEdition, pLanguage->Document);
		}

		return pEdition;
	}

	void CLanguageEdition::SetDictionary(MUIEDITION* pEdition, const TiXmlDocument& Document)
	{
		char strEdition[MAX_PATH] = { 0 };
		const char* strKey = NULL;
		const char* strValue = NULL;
		const TiXmlElement* pNode = Document.RootElement();
		const TiXmlAttribute* pAttribute = NULL;
		RBNODE* pTreeNode = NULL;

		pNode = pNode->FirstChildElement();

		while (pNode)
		{
			if (0 == stricmp(pNode->Value(), "text"))
			{
				pAttribute = pNode->FirstAttribute();


				strKey = pNode->Attribute("name");
				strValue = pNode->Attribute(strEdition);

				//做一下文字转换

				while (pAttribute)
				{
					if (0 == stricmp(pAttribute->Value(), "name"))
					{
					}
					else if (0 == stricmp(pAttribute->Value(), strEdition))
					{
					}

					pAttribute = pAttribute->Next();
				}

				if (strKey && strValue)
				{
					if (0 != rbtree_insert(&pEdition->Root, pTreeNode))
					{
						rbtree_free_node(pTreeNode);
					}
				}
			}

			pNode = pNode->NextSiblingElement();
		}
	}

	LPCTSTR CLanguageEdition::GetString(LPCTSTR strFileName, LPCTSTR strEdition, LPCTSTR strKey)
	{
		MUILANGUAGE* pLanguage = NULL;
		MUIEDITION* pEdition = NULL;
		for (int i = 0; s_LanguageArray.GetSize() > i; i++)
		{
			MUILANGUAGE* p = (MUILANGUAGE*)s_LanguageArray[i];

			if (0 == _tcsicmp(p->strName, strFileName))
			{
				pLanguage = p;
			}
		}

		if (NULL == pLanguage) return NULL;

		for (int i = 0; pLanguage->EditionArray.GetSize(); i++)
		{
			MUIEDITION* p = (MUIEDITION*)pLanguage->EditionArray[i];
			if (0 == _tcsicmp(p->strName, strFileName))
			{
				pEdition = p;
			}
		}

		if (NULL == pEdition) return NULL;

		return GetString(pEdition, strKey);
	}

	LPCTSTR CLanguageEdition::GetString(MUIEDITION* pEdition, LPCTSTR strKey)
	{
		if (NULL == pEdition || NULL == strKey) return NULL;

		RBNODE* pNode = rbtree_find(&pEdition->Root, strKey);

		if (pNode)
		{
			return (LPCTSTR)rbtree_node_data(pNode);
		}
		return NULL;
	}

	RBNODE* CLanguageEdition::RBTreeCreateNode(const char* strKeyA, const char* strValueA)
	{
		int nKeySize = 0;
		int nValueSize = 0;
		TCHAR* strKey;
		void* pData;

		int nSize = _tcslen(strKey) + 1;

		if (nSize == 0) return NULL;

		nSize = sizeof(RBNODE) + sizeof(TCHAR) * nSize;

		RBNODE* node = (RBNODE*)malloc(sizeof(RBNODE) + sizeof(TCHAR) * nSize);

		if (node)
		{
			memset(node, 0, nSize);
			node->key = (char*)node + sizeof(RBNODE);
			node->data = pData;
			//_tcscpy((LPTSTR)node->key, strKey);
		}

		return node;
	}

};