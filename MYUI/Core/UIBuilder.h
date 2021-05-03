
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

		//��ʹ��<?xml version="1.0" encoding="gb2312"?>
		CControlUI * Create(LPCTSTR strSkinFolder, LPCTSTR strXmlFile);

	private:

        bool SetWindow(TiXmlElement *pNode);//����CWindowUI������
		
        bool AddGroup(TiXmlElement * pNode);//��ӷ���ؼ��ķ���
        bool AddFont(TiXmlElement * pNode);//�����屣�浽CWindowUI�У���ҪCWindowUI��Ϊ��

		void SaveTheme(TiXmlElement * pNode);//���ؼ������Ᵽ��������������ǿؼ���Ĭ����������
		void SetTheme(CControlUI *pControl, LPCTSTR strThemeName);//�ڱ���õ������У����������ҳ���Ӧ�����⣬�����õ��ؼ���

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