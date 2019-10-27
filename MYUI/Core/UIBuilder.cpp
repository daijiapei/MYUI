
#include "UIBuilder.h"


#include "../Layout/UIVerticalLayout.h"
#include "../Layout/UIHorizontalLayout.h"
#include "../Layout/UIAbsoluteLayout.h"
#include "../Layout/UITabLayout.h"
#include "../Layout/UITileLayout.h"


#include "../Control/UIScrollBar.h"
#include "../Control/UIButton.h"
#include "../Control/UICombo.h"
#include "../Control/UIComboBox.h"
#include "../Control/UIEdit.h"
#include "../Control/UIGif.h"
#include "../Control/UILable.h"
#include "../Control/UIListBox.h"
#include "../Control/UIListView.h"
#include "../Control/UIMenu.h"
#include "../Control/UIOption.h"
#include "../Control/UIProgress.h"
#include "../Control/UIPicture.h"
#include "../Control/UIRichEdit.h"
#include "../Control/UISlider.h"
#include "../Control/UIText.h"
#include "../Control/UITreeView.h"

#include "..//ExtControl//UIActiveX.h"
#include "..//ExtControl//UIFlash.h"
#include "..//ExtControl//UIWebBrowser.h"
#include "..//ExtControl//UICalendar.h"
//#include "..//ExtControl//UIDragItem.h"
namespace MYUI
{
	CBuilder::CBuilder( CWindowUI * pWindow, IBuilderCallback * pCallback)
		: m_pWindow(NULL)
		, m_pCallback(NULL)
		, m_pCaptionControl(NULL)
	{
		m_pWindow = pWindow;
		m_pCallback = pCallback;
	}

	CBuilder::~CBuilder()
	{
	}

    bool CBuilder::SetWindow(TiXmlElement *pNode)
    {
        TiXmlAttribute * pAttribute = NULL;
        const char * strItem = NULL;
        char * strValue = NULL;
        SIZE Size = { 0 };

        if (NULL == m_pWindow || NULL == pNode) return false;

        pAttribute = pNode->FirstAttribute();
        while (pAttribute)
        {
            strItem = pAttribute->Name();

            if (0 == stricmp(strItem, "size"))
            {
                Size.cx = strtol(pAttribute->Value(), &strValue, 10);
                ASSERT(strValue);
                Size.cy = strtol(strValue + 1, &strValue, 10);
                ASSERT(strValue);
                ::MoveWindow(*m_pWindow, 0, 0, Size.cx, Size.cy, FALSE);
            }
            else if (0 == stricmp(strItem, "mininfo"))
            {
                Size.cx = strtol(pAttribute->Value(), &strValue, 10);
                ASSERT(strValue);
                Size.cy = strtol(strValue + 1, &strValue, 10);
                ASSERT(strValue);
                m_pWindow->SetMinSize(Size);
            }
            else if (0 == stricmp(strItem, "maxinfo"))
            {
                Size.cx = strtol(pAttribute->Value(), &strValue, 10);
                ASSERT(strValue);
                Size.cy = strtol(strValue + 1, &strValue, 10);
                ASSERT(strValue);
                m_pWindow->SetMaxSize(Size);
            }
            else if (0 == stricmp(strItem, "font"))
            {
                m_pWindow->SetTextFont(strtol(pAttribute->Value(), NULL, 10));
            }
            else if (0 == stricmp(strItem, "zoomed"))
            {
#ifdef _DEBUG
                CheckBoer(strValue);
#endif
                if (TRUE == CheckTrue(strValue))
                {
                    SIZE szScreen = { 0 };
                    szScreen.cx = GetSystemMetrics(SM_CXSCREEN);
                    szScreen.cy = GetSystemMetrics(SM_CYSCREEN);
                    MoveWindow(*m_pWindow, 0, 0, szScreen.cx, szScreen.cy, FALSE);
                }
            }
            else
            {
                ASSERT(0 && "CBuilder::SetWindow: û�ж�Ӧ�����Դ���");
            }
            pAttribute = pAttribute->Next();
        }

        return true;
    }

    bool CBuilder::AddGroup(TiXmlElement * pNode)
	{
        bool bRet = false;
#ifdef _UNICODE
		wchar_t * strItem = new wchar_t[MAX_PATH];
		wchar_t * strValue = new wchar_t[MAX_PATH * 2];
#else
		const char * strItem = NULL;
		const char * strValue = NULL;
#endif
        if (NULL == m_pWindow) goto end;

		TiXmlAttribute * pAttribute = pNode->FirstAttribute();
		while(pAttribute)
		{
#ifdef _UNICODE
			strValue[0] = strItem[0] = _T('\0');
			::MultiByteToWideChar(::GetACP(), 0, pAttribute->Name(), -1, strItem, MAX_PATH) ;
			::MultiByteToWideChar(::GetACP(), 0, pAttribute->Value(), -1, strValue, MAX_PATH * 2) ;
#else
			strItem = pAttribute->Name();
			strValue = pAttribute->Value();
#endif
				
			if(0 == _tcsicmp(strItem,_T("name")))
			{
				//ʲôҲ����
                m_pWindow->AddGroup(strValue);
			}
			pAttribute = pAttribute->Next();
		}
        bRet = true;
end:
#ifdef _UNICODE

		delete strItem;
		delete strValue;
#endif
        return bRet;
	}

	//<font id="0" name="΢���ź�" size="14" bold="true" italic="false" underline="false"/>
    bool CBuilder::AddFont(TiXmlElement * pNode)
	{
        if (NULL == m_pWindow || NULL == pNode) return false;
		
		const char * strItem = NULL;
		const char * strValue = NULL;
		int id = -1;
		int height = 0;
		int width = 0;
		
		int weight = 400;//Ȩֵ��400 = ��׼����
		bool italic = false;//б��
		bool underline = false;//�»���
		bool strikeOut = false;//ɾ����
		char name[64] = {0};

		TiXmlAttribute * pAttribute =  pNode->FirstAttribute();
		while(pAttribute)
		{
			strItem = pAttribute->Name();

			if(0 == stricmp(strItem, "id"))
			{
				id = atoi(pAttribute->Value());
			}
			else if(0 == stricmp(strItem, "name"))
			{
				strcpy(name, pAttribute->Value());
			}
			else if(0 == stricmp(strItem, "size"))
			{
				height = atoi(pAttribute->Value());
			}
			else if(0 == stricmp(strItem, "weight"))
			{
				weight = atoi(pAttribute->Value());
			}
			else if(0 == stricmp(strItem, "bold"))//�Ӵ�
			{
#ifdef _DEBUG
				CheckBoer(pAttribute->Value());
#endif
				if(TRUE == CheckTrue(pAttribute->Value()))
				{
					weight = 700;
				}
			}
			else if(0 == stricmp(strItem, "italic"))
			{
#ifdef _DEBUG
				CheckBoer(pAttribute->Value());
#endif
				italic = CheckTrue(pAttribute->Value());
			}
			else if(0 == stricmp(strItem, "underline"))
			{
#ifdef _DEBUG
				CheckBoer(pAttribute->Value());
#endif
				underline = CheckTrue(pAttribute->Value());
			}
			else if(0 == stricmp(strItem, "strikeOut"))
			{
#ifdef _DEBUG
				CheckBoer(pAttribute->Value());
#endif
				strikeOut = CheckTrue(pAttribute->Value());
			}
			else
			{
				ASSERT(0 && strItem);
			}

			pAttribute = pAttribute->Next();
		}

		ASSERT(-1 != id);
		if(-1 == id) return false;

		HFONT hFont = CreateFontA(height,0,0,0,weight,italic,underline,strikeOut,
			CHINESEBIG5_CHARSET,OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,
			FF_MODERN,name);
        m_pWindow->AddFont(id, hFont);
        return true;
	}

	void CBuilder::SaveTheme(TiXmlElement * pNode)
	{
		TiXmlElement * pCopy = new TiXmlElement(pNode->Value());
		*pCopy = *pNode;
        m_ThemeItem.Add(pCopy);
	}

	void CBuilder::SetTheme(CControlUI *pControl, LPCTSTR strThemeName)
	{
#ifdef _UNICODE
		wchar_t * strItem = new wchar_t[MAX_PATH];
		wchar_t * strValue = new wchar_t[MAX_PATH * 2];
#else
		const char * strItem = NULL;
		const char * strValue = NULL;
#endif
		TiXmlElement * pTheme = NULL;
        for (int i = 0; m_ThemeItem.GetSize() > i; i++)
		{
            TiXmlElement * pTmp = (TiXmlElement*)m_ThemeItem.GetAt(i);
#ifdef _UNICODE
			strItem[0] = _T('\0');
			::MultiByteToWideChar(::GetACP(), 0, pTmp->Attribute("name"), -1, strItem, MAX_PATH) ;
#else
			strItem = pTmp->Attribute("name");
#endif
			if(0 == _tcscmp(strItem, strThemeName))
			{
				pTheme = pTmp;
				break;
			}
		}

		if(NULL == pTheme) goto end ;
		//�ҵ��˶�Ӧ������

		TiXmlAttribute * pAttribute = pTheme->FirstAttribute();
		while(pAttribute)
		{
#ifdef _UNICODE
			strValue[0] = strItem[0] = _T('\0');
			::MultiByteToWideChar(::GetACP(), 0, pAttribute->Name(), -1, strItem, MAX_PATH) ;
			::MultiByteToWideChar(::GetACP(), 0, pAttribute->Value(), -1, strValue, MAX_PATH * 2) ;
#else
			strItem = pAttribute->Name();
			strValue = pAttribute->Value();
#endif
				
			if(0 == _tcsicmp(strItem,_T("name")))
			{
				//ʲôҲ����
			}
			else
			{
				pControl->SetAttribute(strItem, strValue);
			}
			pAttribute = pAttribute->Next();
		}

end:
#ifdef _UNICODE

		delete strItem;
		delete strValue;
#endif
		return;
	}

	CControlUI * CBuilder::BuilderLayout(TiXmlElement *pNode, CControlUI * pParent)
	{
#ifdef _UNICODE
		wchar_t * strItem = new wchar_t[MAX_PATH];
		wchar_t * strValue = new wchar_t[MAX_PATH * 2];
#else
		const char * strItem = NULL;
		const char * strValue = NULL;
#endif
	
        CMenuUI * pMenuDialog = NULL;
		TiXmlElement * pChildNode = NULL;
		SIZE size = {0};
		CControlUI * pControl = NULL;
		IControlArray * pControlArray = NULL;
		TiXmlAttribute * pAttribute = NULL;
        IControlArray * pControlAddray = dynamic_cast<IControlArray *>(pParent);

        if (pParent && NULL == pControlAddray)
        {
            ASSERT(0 && "CBuilder::BuilderLayout pParent������һ���ؼ�����");
            return NULL;
        }

		while(pNode)
		{
			//
            pControl = CBuilder::CreateControl(pNode->Value());

            if (NULL == pControl) goto next;//������������һ��

            if (pControlAddray) pControlAddray->Add(pControl);//��ӵ�������

			//���ÿؼ�����
			pAttribute =  pNode->FirstAttribute();

			while(pAttribute)
			{
#ifdef _UNICODE
				strValue[0] = strItem[0] = _T('\0');
				::MultiByteToWideChar(::GetACP(), 0, pAttribute->Name(), -1, strItem, MAX_PATH) ;
				::MultiByteToWideChar(::GetACP(), 0, pAttribute->Value(), -1, strValue, MAX_PATH * 2) ;
#else
				strItem = pAttribute->Name();
				strValue = pAttribute->Value();
#endif
				
				if(0 == _tcsicmp(strItem,_T("theme")))
				{
					SetTheme(pControl, strValue);
					//continue;
					//��continue����ע�͵�����ôtheme֮������ԣ��ͻḲ��theme���õ�����
					//�����費��Ҫע�ͣ����⿴ҵ�������Ҿ���ע�͵���������
				}
				else if(0 == _tcsicmp(strItem,_T("vScrollTheme")))
				{
					//vScrollTheme ����Ҫж�� vScroll="true" ����, ������Ч
					CScrollBarUI * pScroll = (CScrollBarUI*)pControl->CallWndProc(NULL, WM_GETSCROLL, GSL_VERTICAL, 0);
					ASSERT(pScroll && "������Ϊ�գ��Ƿ�û����ǰ����vScroll=\"true\"����");
					if(pScroll)
					{
						SetTheme(pScroll, strValue);
					}
				}
				else if(0 == _tcsicmp(strItem,_T("hScrollTheme")))
				{
					//hScrollTheme ����Ҫж�� hScroll="true" ����, ������Ч
					CScrollBarUI * pScroll = (CScrollBarUI*)pControl->CallWndProc(NULL, WM_GETSCROLL, GSL_HORIZONTAIL, 0);
					ASSERT(pScroll && "������Ϊ�գ��Ƿ�û����ǰ����hScroll=\"true\"����");
					if(pScroll)
					{
						SetTheme(pScroll, strValue);
					}
				}
				else if(0 == _tcsicmp(strItem,_T("caption")))
				{
#ifdef _DEBUG
					CheckBoer(strValue);
#endif
					if(TRUE == CheckTrue(strValue))
					{
						m_pCaptionControl = pControl;
					}
				}
                else if (0 == _tcsicmp(strItem, _T("menu")))
                {
                    pMenuDialog = reinterpret_cast<CMenuUI*>(m_MenuMap.Find(strValue));

                    if (pMenuDialog)
                    {
                        pControl->SetMenu(dynamic_cast<IMenuPopup*>(pMenuDialog));
                    }
                    else
                    {
                        ASSERT(0 && "CBuilder::BuilderLayout û��Ϊ�ؼ��ҵ���Ӧ�Ĳ˵�");
                    }
                }
				else
				{
					pControl->SetAttribute(strItem, strValue);
				}
				pAttribute = pAttribute->Next();
			}

			if(pChildNode = pNode->FirstChildElement())//ӵ���ӽڵ�
			{
                if (0 == stricmp("Menu", pChildNode->Value()))
                {
                    pMenuDialog = new CMenuUI;
                    pMenuDialog->SetSyncResource(m_pWindow);
                    
                    CControlUI * pMenuUI = BuilderLayout(pChildNode, NULL);

                    pMenuDialog->Create(m_pWindow ? m_pWindow->m_hInstance : NULL, pMenuUI->GetName());
                    pMenuDialog->AttachFrameView(pMenuUI);

                    if (pControl)
                    {
                        pControl->SetMenu(dynamic_cast<IMenuPopup*>(pMenuDialog));
                    }

                    if (pControl->GetName() != _T(""))
                    {
                        m_MenuMap.Insert(pControl->GetName(), pMenuDialog);
                    }

                    if (m_pWindow)
                    {
                        m_pWindow->m_MenuArray.Add(pMenuDialog);
                    }
                }
                else
                {
                    BuilderLayout(pChildNode, pControl);
                }
			}
			
next:
			pNode = pNode->NextSiblingElement();//��һ���ֵܽڵ�
		}

		
#ifdef _UNICODE
		delete strItem;
		delete strValue;
#endif

		return pControl;
	}

	CControlUI * CBuilder::CreateControl(const wchar_t * strClass)
	{
		CControlUI * pControl = NULL;
		char * strClassA = NULL;
		int nSize = 0;
		nSize = ::WideCharToMultiByte(CP_ACP, 0, strClass, -1, NULL, 0, NULL, NULL);

		ASSERT(nSize && "CBuilder::CreateControl ת���ַ�����ʧ��");
		strClassA = new char[nSize];
		nSize =::WideCharToMultiByte(CP_ACP, 0, strClass, -1, strClassA, nSize, NULL, NULL);

		ASSERT(nSize && "CBuilder::CreateControl ת���ַ�����ʧ��");
		pControl = CBuilder::CreateControl(strClassA);

		delete strClassA;
		return pControl;
	}

	CControlUI * CBuilder::CreateControl(const char * strClass)
	{
		CControlUI * pControl = NULL;
		if(0 == stricmp(strClass, "placeholder"))
		{
			//˵������ؼ�ֻ������ռλ��
			pControl = new CControlUI();
			pControl->SetPenetrated(true);
		}
		else if(0 == stricmp(strClass, "Button"))
		{
			pControl = new CButtonUI();
		}
		else if(0 == stricmp(strClass, "Combo"))
		{
			pControl = new CComboUI();
		}
		else if(0 == stricmp(strClass, "ComboBox"))
		{
			pControl = new CComboBoxUI();
		}
		else if(0 == stricmp(strClass, "Edit"))
		{
			pControl = new CEditUI();
		}
		else if(0 == stricmp(strClass, "Gif"))
		{
			pControl = new CGifUI();
		}
		else if(0 == stricmp(strClass, "Lable"))
		{
			pControl = new CLableUI();
		}
		else if(0 == stricmp(strClass, "ListBox"))
		{
			pControl = new CListBoxUI();
		}
		else if(0 == stricmp(strClass, "ListHeader"))
		{
			pControl = new CListHeaderUI();
		}
		else if(0 == stricmp(strClass, "ListView"))
		{
			pControl = new CListViewUI();
		}
        else if (0 == stricmp(strClass, "menu"))
        {
            pControl = new CVerticalLayoutUI();
        }
        else if (0 == stricmp(strClass, "MenuElement"))
        {
            pControl = new CMenuElementUI();
        }
		else if(0 == stricmp(strClass, "Option"))
		{
			pControl = new COptionUI();
		}
		else if(0 == stricmp(strClass, "Picture"))
		{
			pControl = new CPictureUI();
		}
		else if(0 == stricmp(strClass, "Progress"))
		{
			pControl = new CProgressUI();
		}
		else if(0 == stricmp(strClass, "richEdit"))
		{
			pControl = new CRichEditUI();
		}
		else if(0 == stricmp(strClass, "vScroll"))
		{
			pControl = new CVerticalScrollBarUI();
		}
		else if(0 == stricmp(strClass, "hScroll"))
		{
			pControl = new CHorizontailScrollBarUI();
		}
		else if(0 == stricmp(strClass, "Slider"))
		{
			pControl = new CSliderUI();
		}
		else if(0 == stricmp(strClass, "Text"))
		{
			pControl = new CTextUI();
		}
		else if(0 == stricmp(strClass, "TreeNode"))
		{
			pControl = new CTreeNodeUI();
		}
		else if(0 == stricmp(strClass, "TreeView"))
		{
			pControl = new CTreeViewUI();
		}
		//ExtControl
		else if(0 == stricmp(strClass, "Calendar"))
		{
			pControl = new CCalendarUI();
		}
		else if(0 == stricmp(strClass, "DragItem"))
		{
			//pControl = new CDragItemUI();
		}
		else if(0 == stricmp(strClass, "ActiveX"))
		{
			pControl = new CActiveXUI();
		}
		else if(0 == stricmp(strClass, "WebBrowser"))
		{
			pControl = new CWebBrowserUI();
		}
		//layout
		else if(0 == stricmp(strClass, "AbsoluteLayout"))
		{
			pControl = new CAbsoluteLayoutUI();
		}
		else if(0 == stricmp(strClass, "HorizontalLayout"))
		{
			pControl = new CHorizontalLayoutUI();
		}
		else if(0 == stricmp(strClass, "TabLayout"))
		{
			pControl = new CTabLayoutUI();
		}
		else if(0 == stricmp(strClass, "TileLayout"))
		{
			pControl = new CTileLayoutUI();
		}
		else if(0 == stricmp(strClass, "VerticalLayout"))
		{
			pControl = new CVerticalLayoutUI();
		}
		else if(0 == stricmp(strClass, "Control"))
		{
			pControl = new CControlUI();
		}
		else
		{
			if(m_pCallback)
			{
				pControl = m_pCallback->CreateControl(strClass);
			}
			else
			{
				ASSERT(0 && "CBuilder::CreateControl : �Ҳ�����Ӧ�Ŀؼ�����, �Զ���ؼ��ص�ҲΪ��");
			}
		}

		ASSERT(pControl && "CBuilder::CreateControl : �ؼ�����ʧ�ܣ�û���ҵ���Ӧ�Ŀؼ��ӿ�");
		return pControl;
	}

	CControlUI * CBuilder::Create(LPCTSTR strSkinFolder, LPCTSTR strXmlFile)
	{
        bool bRet = false;
		CControlUI * pControl = NULL;
        CMenuUI * pMenu = NULL;
		TiXmlDocument * pDocument = new TiXmlDocument();
		BYTE * pData = NULL;
		DWORD dwSize = 0;
		CSkinManager::LoadFile(strSkinFolder, strXmlFile, &pData, &dwSize);

		pDocument->Parse((char *)pData);
		if(pDocument->Error())
		{
			 ASSERT(0 && "CBuilder::Create ����������Ǳ�׼��xml��ʽ");
			 goto end;
		}

        TiXmlElement * pChildNode = NULL;
        TiXmlElement * pNode = pDocument->RootElement();
        if (stricmp(pNode->Value(), "UIFrame")) goto end;

        pNode = pNode->FirstChildElement();
		while(pNode)
		{
			pNode->Value();

			if(0 == stricmp(pNode->Value(), "theme"))
			{
                SaveTheme(pNode);
			}
			else if(0 == stricmp(pNode->Value(), "font"))
			{
                AddFont(pNode);
			}
			else if(0 == stricmp(pNode->Value(), "group"))
			{
                AddGroup(pNode);
			}
            else if (0 == stricmp(pNode->Value(), "language"))
            {
                //AddLanguage(pNode->Attribute("file"), pNode->Attribute("script"));
            }
            else if (0 == stricmp(pNode->Value(), "Menu"))
            {
                CMenuUI * pMenuDialog = new CMenuUI;
                pMenuDialog->SetSyncResource(m_pWindow);

                pControl = BuilderLayout(pNode, NULL);
                pMenuDialog->Create(m_pWindow ? m_pWindow->m_hInstance : NULL, pControl->GetName());
                pMenuDialog->AttachFrameView(pControl);

                if (pControl->GetName() == _T(""))
                {
                    //���û�����ֵĻ�����ô�������ط���������������˵�����ô����˵�����Ҳû��������
                    ASSERT(0 && "CBuilder::Create û�����ֵĲ˵����ᱻɾ��");
                    delete pMenuDialog;
                }
                else
                {
                    m_MenuMap.Insert(pControl->GetName(), pMenuDialog);

                    if (m_pWindow)
                    {
                        m_pWindow->m_MenuArray.Add(pMenuDialog);
                    }
                }
            }
			else if(0 == stricmp(pNode->Value(), "window"))
			{
                SetWindow(pNode);

                if (pChildNode = pNode->FirstChildElement())
				{
                    pControl = BuilderLayout(pChildNode, NULL);
				}
			}
			else//Control
			{
				pControl = BuilderLayout(pNode, NULL);
			}

			pNode = pNode->NextSiblingElement();
		}

		if(m_pCaptionControl && m_pWindow) m_pWindow->SetCaption(m_pCaptionControl);

        bRet = true;
end:
		delete pDocument;
		return pControl;

	}

}