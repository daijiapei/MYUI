
#ifndef __MYUI_LANGUAGE_EDITION_H__
#define __MYUI_LANGUAGE_EDITION_H__

#include "../tinyxml/tinyxml.h"
#include "../Utils/rbtree.h"
#include "../Utils/Utils.h"
#include <Windows.h>
namespace MYUI
{
	typedef struct __MUILANGUAGE
	{
		TiXmlDocument Document;
		CMuiPtrArray EditionArray;
		TCHAR strName[MAX_PATH];
	}MUILANGUAGE;

	typedef struct __MUIEDITION
	{
		RBTREEROOT Root;
		MUILANGUAGE* pLanguage;
		TCHAR strName[MAX_PATH];
	}MUIEDITION;

	class CLanguageEdition
	{
	public:
		MUIEDITION* AddLanguage(LPCTSTR strFileName, LPCTSTR strEdition, const char* strFileDate, int nSize);
		LPCTSTR GetString(LPCTSTR strFileName, LPCTSTR strEdition, LPCTSTR strKey);
		LPCTSTR GetString(MUIEDITION* pEdition, LPCTSTR strKey);
	private:
		static int __stdcall RBTreeCompare(const void* pObject1, const void* pObject2);
		static RBNODE* RBTreeCreateNode(const char * strKeyA, const char * strValueA);

		void SetDictionary(MUIEDITION* pEdition, const TiXmlDocument &Document);
	private:
		CMuiPtrArray s_LanguageArray;
	};
}

#endif