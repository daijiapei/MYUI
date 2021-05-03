
#ifndef __MYUI_BUILDER_H__
#define __MYUI_BUILDER_H__

#include "UIControl.h"
#include "UIWindow.h"
#include "../tinyxml/tinyxml.h"
#include "../Utils/rbtree.h"
namespace MYUI
{
	__interface IBuilderCallback
	{
	public:
		virtual CControlUI * CreateControl(const char* strClass) = 0;
	};

	class MYUI_API CBuilder
	{
	public:
		CBuilder(CWindowUI * pWindow, IBuilderCallback * pCallback = NULL);
		~CBuilder();

		//请使用<?xml version="1.0" encoding="gb2312"?>
		CControlUI * Create(LPCTSTR strSkinFolder, LPCTSTR strXmlFile);

	private:

        bool SetWindow(TiXmlElement *pNode);//设置CWindowUI的属性
		
        bool AddGroup(TiXmlElement * pNode);//添加分组控件的分组
        bool AddFont(TiXmlElement * pNode);//将字体保存到CWindowUI中，需要CWindowUI不为空

		void SaveTheme(TiXmlElement * pNode);//将控件的主题保存起来，主题就是控件的默认属性设置
		void SetTheme(CControlUI *pControl, LPCTSTR strThemeName);//在保存好的主题中，根据名字找出对应的主题，并设置到控件中

		CControlUI * BuilderLayout(TiXmlElement *pNode, CControlUI * pParent);
		CControlUI * CreateControl(const char * strClass);
		CControlUI * CreateControl(const wchar_t * strClass);
	private:
		CWindowUI * m_pWindow;
		IBuilderCallback * m_pCallback;
		CControlUI * m_pCaptionControl;
		CMuiPtrArray m_ThemeItem;
        CMuiStringPtrMap m_MenuMap;
        CMuiString m_strText;
	};
}

#endif