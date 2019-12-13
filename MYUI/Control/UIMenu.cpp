
#include "UIMenu.h"
#include "../Core/UIBuilder.h"
#include "../Core/UserHandle.h"

#define WMU_SHOWCHILDMENU          (WM_APP + 1)
#define WMU_CHILDMENUHIDE          (WM_APP + 2)

namespace MYUI
{

    CMenuUI::CMenuUI()
        : m_pNotify(NULL)
    {

    }

    CMenuUI::~CMenuUI()
    {

    }

    HWND CMenuUI::Create(HINSTANCE hInstance, LPCTSTR strWindowText, LPCTSTR strSkinFolder, LPCTSTR strXmlFile)
    {
        HWND hWnd = NULL;
        CControlUI * pControl = NULL;
        hWnd = CWindowUI::Create(hInstance, NULL, WS_OVERLAPPEDWINDOW & (~WS_THICKFRAME),
            TEXT("CMenuUI"), strWindowText, &CMuiRect(0, 0, 160, 30));

        if (NULL == hWnd) return NULL;

        if (!strSkinFolder || _T('\0') == strSkinFolder[0] || !strXmlFile || _T('\0') == strXmlFile[0])
        {
            return hWnd;
        }

        SetSkin(strSkinFolder);
        CBuilder * pBuilder = new CBuilder(this, dynamic_cast<IBuilderCallback*>(this));
        pControl = pBuilder->Create(strSkinFolder, strXmlFile);
        delete pBuilder;

        this->AttachFrameView(pControl);
        return hWnd;
    }

    void CMenuUI::SetMenuLayout(CControlUI * pControl)
    {
        this->AttachFrameView(pControl);
    }

    bool CMenuUI::Add(CControlUI* pControl)
    {
        CBaseLayoutUI * pLayout = dynamic_cast<CBaseLayoutUI*>(m_pViewInfo ? m_pViewInfo->pRootControl : NULL);

        if (pLayout)
        {
            return pLayout->Add(pControl);
        }
        return false;
    }

    bool CMenuUI::AddAt(CControlUI* pControl, int nIndex)
    {
        CBaseLayoutUI * pLayout = dynamic_cast<CBaseLayoutUI*>(m_pViewInfo ? m_pViewInfo->pRootControl : NULL);

        if (pLayout)
        {
            return pLayout->AddAt(pControl, nIndex);
        }
        return false;
    }

    bool CMenuUI::Remove(CControlUI* pControl)
    {
        CBaseLayoutUI * pLayout = this->GetMenuUI();

        if (pLayout)
        {
            return pLayout->Remove(pControl);
        }
        return false;
    }

    bool CMenuUI::RemoveAt(int nIndex)
    {
        CBaseLayoutUI * pLayout = this->GetMenuUI();

        if (pLayout)
        {
            return pLayout->RemoveAt(nIndex);
        }
        return false;
    }

    void CMenuUI::RemoveAll()
    {
        CBaseLayoutUI * pLayout = this->GetMenuUI();

        if (pLayout)
        {
            return pLayout->RemoveAll();
        }
    }

    CControlUI * CMenuUI::Find(int nIndex)
    {
        CBaseLayoutUI * pLayout = this->GetMenuUI();

        if (pLayout)
        {
            return pLayout->Find(nIndex);
        }
        return false;
    }

    int CMenuUI::Find(CControlUI * pControl)
    {
        CBaseLayoutUI * pLayout = this->GetMenuUI();

        if (pLayout)
        {
            return pLayout->Find(pControl);
        }
        return false;
    }

    int CMenuUI::GetCount() const
    {
        CBaseLayoutUI * pLayout = this->GetMenuUI();

        if (pLayout)
        {
            return pLayout->GetCount();
        }
        return -1;
    }

    CControlUI * CMenuUI::CreateControl(const char* strClass)
    {
        return NULL;
    }

    CBaseLayoutUI * CMenuUI::GetMenuUI() const
    {
        return dynamic_cast<CBaseLayoutUI*>(m_pViewInfo ? m_pViewInfo->pRootControl : NULL);
    }

    LRESULT CMenuUI::Popup(INotify *pNotify, POINT ptPopup)
    {
        LRESULT lResult = NULL;
        SIZE Size = { 180, 30 };
        SIZE szScreen = { 0 };
        RECT rcPos = { 0 };
        TNOTIFYUI Notify = { 0 };
        MSG Msg = { 0 };

        ASSERT(pNotify != m_pShareInfo->pNotify && "CMenuUI::Popup 形成了一个死环");
        
        Size = this->GetSize();

        rcPos.left = ptPopup.x;
        rcPos.top = ptPopup.y;
        rcPos.right = rcPos.left + Size.cx;
        rcPos.bottom = rcPos.top + Size.cy;

        m_pNotify = pNotify;

        lResult = CWindowUI::Popup(rcPos);

        m_pNotify = NULL;

        return lResult;
    }

    SIZE CMenuUI::GetSize()
    {
        SIZE Size = { 180, 30 };
        CBaseLayoutUI * pLayout = dynamic_cast<CBaseLayoutUI*>(m_pViewInfo ? m_pViewInfo->pRootControl : NULL);

        if (pLayout->GetCount() > 0)
        {
            Size = pLayout->GetContentSize();
            //Size.cx += 2;//边框
            //Size.cy += 2;//边框
        }

        return Size;
    }

    LRESULT CMenuUI::DoModal()
    {
        ASSERT(::IsWindow(m_hWnd));
        LRESULT lResult = 0;
        MSG Msg = { 0 };
        IUserHandle * pHandle = CUserHandleTable::GetThreadHandle(NULL);

        RECT rcPos = { 0 };
        ::SetWindowPos(m_hWnd, HWND_TOPMOST, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
            rcPos.bottom - rcPos.top, SWP_SHOWWINDOW);

        while (TRUE)
        {
            if (::IsWindow(m_hWnd) && PeekMessage(&Msg, 0, 0, 0, PM_NOREMOVE))
            {
                switch (Msg.hwnd ? Msg.message : 0)
                {
                case WM_POPUPDIALOG:
                {
                    if (m_hWnd != Msg.hwnd) break;
                }break;
                case WM_POPUPMENU:
                {
                    if (static_cast<IMenuPopup*>(this) == reinterpret_cast<IMenuPopup*>(Msg.wParam))
                    {
                        //同一个菜单, 不需要重复弹出
                        PeekMessage(&Msg, 0, Msg.message, Msg.message, PM_REMOVE);
                        continue;
                    }

                    if (m_hWnd != Msg.hwnd) break;
                }break;
                case WM_BREAKLOOP:
                {
                    if (NULL == Msg.lParam || m_hWnd == (HWND)Msg.lParam) break;
                }break;
                default:
                    break;
                }

                PeekMessage(&Msg, 0, Msg.message, Msg.message, PM_REMOVE);

                if (NULL == Msg.hwnd)
                {
                    if (pHandle)
                    {
                        if (Msg.message >= WM_USER && WM_USER + 0x7FFF >= Msg.message)
                        {
                            pHandle->Callback(Msg.message - WM_USER, Msg.wParam, Msg.lParam);
                            continue;
                        }
                    }
                }
#ifdef ENABLE_TIMER_LPARAM
                else
                {
                    if (WM_TIMER == Msg.message)
                    {
                        ::SendMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
                        continue;
                    }
                }
#endif

                ::TranslateMessage(&Msg);
                ::DispatchMessage(&Msg);
            }
            else
            {
                WaitMessage();
            }
        }

        if (::IsWindow(m_hWnd)) ShowWindow(FALSE);

        return Msg.message;
    }

    LPVOID CMenuUI::GetInterface(LPCTSTR strName)
    {
        if (0 == _tcsicmp(strName, _T("IMenuPopup")))
        {
            return static_cast<IMenuPopup*>(this);
        }
        else if (0 == _tcsicmp(strName, _T("IControlArray")))
        {
            return static_cast<IControlArray*>(this);
        }
        else if (0 == _tcsicmp(strName, _T("CMenuUI")))
        {
            return static_cast<CMenuUI*>(this);
        }

        return CWindowUI::GetInterface(strName);
    }

    void CMenuUI::OnNotify(TNOTIFYUI &Notify)
    {
        switch (Notify.dwType)
        {
        case EnumNotifyMsg::CheckItem:
        {
            if (Notify.pSender->GetClassName() == CMenuElementUI::g_strClassName && m_pNotify)
            {
                m_pNotify->SendNotify(Notify);
                //this->Hide(true);
            }
        }break;
        }
    }

    LRESULT CMenuUI::OnEvent(TEVENT &Event)
    {
        switch (Event.dwType)
        {
        case EnumEventType::WindowInit:
        {
            CVerticalLayoutUI * pRootLayout = new CVerticalLayoutUI();

            pRootLayout->SetBorderPen(1, ARGB(255, 100, 100, 100));
            pRootLayout->SetBkColor(ARGB(255, 255, 255, 255));
            pRootLayout->SetInset(CMuiRect(1, 1, 1, 1));

            this->AttachFrameView(pRootLayout);
        }break;
        case EnumEventType::OnTimer:
        {

        }break;
        case EnumEventType::WindowShow:
        {
            CMenuUI * pParentMenu = dynamic_cast<CMenuUI *>(m_pNotify);
            if (TRUE == Event.wParam && pParentMenu)
            {
                pParentMenu->SendMessage(WMU_CHILDMENUHIDE, NULL, (LPARAM)this);
            }
        }break;
        case EnumEventType::SetFocued:
        {
            TRACE(_T("EnumEventType::SetFocued %s"), this->GetWindowText());
        }break;
        case EnumEventType::KillFocued:
        {
            //if (NULL == m_pChildMenu)
            //{
            //    this->Hide(false);
            //}
            //
            
            ::PostMessage(m_hWnd, WM_BREAKLOOP, TRUE, NULL);
            
            TRACE(_T("EnumEventType::KillFocued %s"), this->GetWindowText());
        }break;
        case EnumEventType::WindowDestroy:
        {
        }break;
        default:
            break;
        }

        return 0;
    }

    /***************************************************************************
     * CMenuElementUI 类 CMenuUI中的节点
     * CMenuElementUI 设计格式
     *    ______________________________
     *    | 口 |  text             | > |
     *    ------------------------------
     * 符号 口 ：菜单图标
     * text   : 菜单说明
     * 符号 >：表示有子菜单
    ***************************************************************************/
    CMuiString CMenuElementUI::g_strClassName(_T("MenuElementUI"));

    CMenuElementUI::CMenuElementUI()
        : m_refHotColor(0)
        , m_refHotTextColor(0)
        , m_refSelectTextColor(0)
        , m_refLineColor(0)
        , m_nLineSize(1)
    {
        EmptyRect(m_rcTextPadding);
        this->SetHeight(28);
    }

    CMenuElementUI::~CMenuElementUI()
    {

    }

    CMuiString CMenuElementUI::GetClassName() const
    {
        return CMenuElementUI::g_strClassName;
    }

    void CMenuElementUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
    {
        if (0 == _tcsicmp(strItem, _T("DisabledLogo")))
        {
            SetDisabledLogo(strValue);
        }
        else if (0 == _tcsicmp(strItem, _T("NormalLogo")))
        {
            SetNormalLogo(strValue);
        }
        else if (0 == _tcsicmp(strItem, _T("HotLogo")))
        {
            SetHotLogo(strValue);
        }
        else if (0 == _tcsicmp(strItem, _T("SelectLogo")))
        {
            SetSelectLogo(strValue);
        }
        else if (0 == _tcsicmp(strItem, _T("DisabledGuide")))
        {
            SetDisabledGuide(strValue);
        }
        else if (0 == _tcsicmp(strItem, _T("NormalGuide")))
        {
            SetNormalGuide(strValue);
        }
        else if (0 == _tcsicmp(strItem, _T("HotGuide")))
        {
            SetHotLGuide(strValue);
        }
        else if (0 == _tcsicmp(strItem, _T("HotColor")))
        {
            SetHotColor(CMuiColor(strValue));
        }
        else if (0 == _tcsicmp(strItem, _T("HotTextColor")))
        {
            SetHotTextColor(CMuiColor(strValue));
        }
        else if (0 == _tcsicmp(strItem, _T("SelectTextColor")))
        {
            SetSelectTextColor(CMuiColor(strValue));
        }
        else if (0 == _tcsicmp(strItem, _T("SelectMode")))
        {
#ifdef _DEBUG
            ASSERT(CheckBoer(strValue));
#endif
            SetSelectMode(CheckTrue(strValue));
        }
        else if (0 == _tcsicmp(strItem, _T("LineMode")))
        {
#ifdef _DEBUG
            ASSERT(CheckBoer(strValue));
#endif
            SetLineMode(CheckTrue(strValue));
        }
        else
        {
            __super::SetAttribute(strItem, strValue);
        }

    }

    void CMenuElementUI::SetSelectMode(bool b)
    {
        if (b)
        {
            m_dwStyle |= MMUS_SELECTMODE;
        }
        else
        {
            m_dwStyle &= ~MMUS_SELECTMODE;
        }
    }

    bool CMenuElementUI::IsSelectMode()
    {
        return MMUS_SELECTMODE & m_dwStyle;
    }

    void CMenuElementUI::SetLineMode(bool b)
    {
        if (b)
        {
            m_dwStyle |= MMUS_LINEMODE;
        }
        else
        {
            m_dwStyle &= ~MMUS_LINEMODE;
        }
    }

    bool CMenuElementUI::IsLineMode()
    {
        return MMUS_LINEMODE & m_dwStyle;
    }

    LRESULT CMenuElementUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        CControlUI * pControl = NULL;
        RECT rcPos = { 0 };
        SIZE Size = { 0 };
        POINT Point = { 0, 0 };
        switch (message)
        {
        case WM_LBUTTONDOWN:
        {

        }break;
        case WM_LBUTTONUP:
        {

            //要先post,免得处理SendNotify的时候掺杂其他消息在前面,比如WM_CLOSE
            ::PostMessage(GETHWND(this), WM_BREAKLOOP, FALSE, NULL);
            this->SendNotify(FALSE, EnumNotifyMsg::CheckItem, NULL, (LPARAM)this);

        }break;
        case WM_MOUSEMOVE:
        {
            Point.x = (short)LOWORD(lParam);
            Point.y = (short)HIWORD(lParam);
        }break;
        case WM_MOUSEENTER://鼠标第一次进入
        {
            if (m_pMenu && TRUE == this->GetItemFixedRect(rcPos))
            {
                CBaseLayoutUI* pLayout = dynamic_cast<CBaseLayoutUI*>(m_pParentContrl);

                if (pLayout)
                {
                    rcPos.left += pLayout->GetInset().left;
                    rcPos.right += pLayout->GetInset().right;
                }

                Size = m_pMenu->GetSize();
                ::MapWindowPoints(m_pShareInfo->hWnd, NULL, (LPPOINT)&rcPos, 2);
                Point = CalcPopupPoint(&rcPos, &Size, CPOT_RIGHT);

                ASSERT(GETHWND(this));
            }

            ::PostMessage(GETHWND(this), WM_POPUPMENU, (WPARAM)m_pMenu, MAKELONG(Point.x, Point.y));
        }//流入WM_MOUSELEAVE
        case WM_MOUSELEAVE://鼠标离开
        {
            //需要有对应的Hot设置才会更新
            if (true == m_bEnabled)
            {
                //if (NULL != m_refHotColor || !m_strHotLogo.IsEmpty() ||
                //    NULL != m_refHotTextColor)
                //{
                    this->Invalidate();
                    break;//产生一次刷新即可
                //}
            }
        }break;
        case WM_MOUSEHOVER://鼠标停留
        {
        }break;
        default:
            break;
        }

        return __super::WndProc(hWnd, message, wParam, lParam);
    }

    void CMenuElementUI::PaintBkColor(const RECT& rcItem, const RECT& rcPaint)
    {
        ARGBREF refColor = m_refBkColor;

        //如果没有图片，那就绘制颜色
        if (m_dwState & STATE_HOT && NULL != m_refHotColor)
        {
            refColor = m_refHotColor;
        }

        if (NULL != refColor)
        {
            m_pShareInfo->pRenderEngine->OnDrawColor(rcItem, refColor);
            return;
        }
        return;
    }

    void CMenuElementUI::PaintStatusImage(const RECT& rcItem, const RECT& rcPaint)
    {
        LPCTSTR strLogoImage = m_strNormalLogo;
        LPCTSTR strGuideImage = m_strNormalGuide;
        RECT rcLogoDraw = rcItem, rcGuideDraw = rcItem;
        RECT rcIndent = { 0 };
        INT nIndent = ((rcItem.bottom - rcItem.top) / 10) * 3;

        if (MMUS_LINEMODE & m_dwStyle) return;//分隔线

        rcLogoDraw.right = rcLogoDraw.left + (rcItem.bottom - rcItem.top);
        rcGuideDraw.left = rcGuideDraw.right - (rcItem.bottom - rcItem.top);
        rcIndent.left = rcIndent.right = rcIndent.top = rcIndent.bottom = nIndent;

        if (false == m_bEnabled)
        {
            if (MMUS_SELECTMODE & m_dwStyle && STATE_SELECT & m_dwState)
            {
                if (false == m_strSelectLogo.IsEmpty())
                {
                    strLogoImage = m_strSelectLogo;
                }
            }
            else if (false == m_strDisabledLogo.IsEmpty())
            {
                strLogoImage = m_strDisabledLogo;
            }

            if (_T('\0') != strLogoImage[0])
            {
                m_pShareInfo->pRenderEngine->OnDrawImage(rcLogoDraw, strLogoImage);
            }

            if (NULL == m_pMenu) return;//没有子菜单

            if (false == m_strDisabledGuide.IsEmpty())
            {
                strGuideImage = m_strDisabledGuide;
            }

            if (_T('\0') != strGuideImage[0])
            {
                m_pShareInfo->pRenderEngine->OnDrawImage(rcGuideDraw, strGuideImage);
            }

            return;
        }

        if (MMUS_SELECTMODE & m_dwStyle && STATE_SELECT & m_dwState)
        {
            if (false == m_strSelectLogo.IsEmpty())
            {
                strLogoImage = m_strSelectLogo;
            }
        }
        else
        {
            if (m_dwState & STATE_HOT && false == m_strHotLogo.IsEmpty())
            {
                strLogoImage = m_strHotLogo;
            }
        }

        if (_T('\0') != strLogoImage[0] && TRUE == IndentRect(&rcLogoDraw, &rcIndent))
        {
            m_pShareInfo->pRenderEngine->OnDrawImage(rcLogoDraw, strLogoImage);
        }

        if (NULL == m_pMenu) return;//没有子菜单

        if (m_dwState & STATE_HOT && false == m_strHotGuide.IsEmpty())
        {
            strGuideImage = m_strHotGuide;
        }

        if (_T('\0') != strGuideImage[0] && TRUE == IndentRect(&rcGuideDraw, &rcIndent))
        {
            m_pShareInfo->pRenderEngine->OnDrawImage(rcGuideDraw, strGuideImage);
        }

    }

    void CMenuElementUI::PaintText(const RECT& rcItem, const RECT& rcPaint)
    {
        RECT rcDraw = rcItem;
        ARGBREF refTextColor = m_refTextColor;
        HFONT hFont = (HFONT)m_pShareInfo->FontArray->Select(m_nFontId);

        rcDraw.left += rcItem.bottom - rcItem.top;
        rcDraw.right -= rcItem.bottom - rcItem.top;

        if (FALSE == IndentRect(&rcDraw, &m_rcTextPadding)) return;

        if (MMUS_LINEMODE & m_dwStyle)
        {
            POINT ptBegin = { 0 };
            POINT ptEnd = { 0 };

            if (0 >= m_nLineSize) return;

            ptBegin.x = rcItem.left + 2;
            ptEnd.x = rcItem.right - 2;

            ptBegin.y = ptEnd.y = rcDraw.top + ((rcDraw.bottom - rcDraw.top) / 2 - m_nLineSize / 2) + 1;

            m_pShareInfo->pRenderEngine->OnDrawLine(ptBegin, ptEnd, m_nLineSize, m_refLineColor);
            return;
        }

        if (m_strText.IsEmpty()) return;

        while (false == m_bEnabled)
        {
            if (MMUS_SELECTMODE & m_dwStyle && STATE_SELECT & m_dwState)
            {
                refTextColor = m_refSelectTextColor;
                break;
            }
            //注意判断顺序
            if (m_dwState & STATE_HOT && NULL != m_refHotTextColor)
            {
                refTextColor = m_refHotTextColor;
            }

            break;
        }

        m_pShareInfo->pRenderEngine->OnDrawText(rcDraw, m_strText,
            refTextColor, hFont, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
    }

    SIZE CMenuElementUI::GetValidSize()//取得真正有效的尺寸
    {
        SIZE Size = { 0 };
        RECT rcItem = { 0 };
        HFONT hFont = (HFONT)m_pShareInfo->FontArray->Select(m_nFontId);

        Size.cy = this->GetHeight();

        if (EnumFloatType::FloatSizeCx & m_FloatType)
        {
            Size.cx = this->GetWidth();
        }
        else
        {
            Size.cx = Size.cy * 2;
            
            if (m_pShareInfo && m_pShareInfo->pRenderEngine)
            {
                Size.cx += m_pShareInfo->pRenderEngine->GetTextSize(rcItem, m_strText,
                    m_strText.GetLength(), hFont, 0, TOS_SINGLELINE).cx;
            }
        }

        Size.cx += m_rcTextPadding.left + m_rcTextPadding.right;
        Size.cx = MAX(m_SizeMin.cx, MIN(Size.cx, m_SizeMax.cx));
        Size.cy = MAX(m_SizeMin.cy, MIN(Size.cy, m_SizeMax.cy));
        Size.cx += m_rcMargin.left + m_rcMargin.right;
        Size.cy += m_rcMargin.top + m_rcMargin.bottom;
        return Size;
    }

    //logo
    void CMenuElementUI::SetDisabledLogo(LPCTSTR strImage)
    {
        m_strDisabledLogo = strImage;
    }

    LPCTSTR CMenuElementUI::GetDisabledLogo() const
    {
        return m_strDisabledLogo;
    }

    void CMenuElementUI::SetNormalLogo(LPCTSTR strImage)
    {
        m_strNormalLogo = strImage;
    }

    LPCTSTR CMenuElementUI::GetNormalLogo() const
    {
        return m_strNormalLogo;
    }

    void CMenuElementUI::SetHotLogo(LPCTSTR strImage)
    {
        m_strHotLogo = strImage;
    }

    LPCTSTR CMenuElementUI::GetHotLogo() const
    {
        return m_strHotLogo;
    }

    void CMenuElementUI::SetSelectLogo(LPCTSTR strImage)
    {
        m_strSelectLogo = strImage;
    }

    LPCTSTR CMenuElementUI::GetSelectLogo() const
    {
        return m_strSelectLogo;
    }

    //guide
    void CMenuElementUI::SetDisabledGuide(LPCTSTR strImage)
    {
        m_strDisabledGuide = strImage;
    }

    LPCTSTR CMenuElementUI::GetDisabledGuide() const
    {
        return m_strDisabledGuide;
    }

    void CMenuElementUI::SetNormalGuide(LPCTSTR strImage)
    {
        m_strNormalGuide = strImage;
    }

    LPCTSTR CMenuElementUI::GetNormalGuide() const
    {
        return m_strNormalGuide;
    }

    void CMenuElementUI::SetHotLGuide(LPCTSTR strImage)
    {
        m_strHotGuide = strImage;
    }

    LPCTSTR CMenuElementUI::GetHotGuide() const
    {
        return m_strHotGuide;
    }

    //bkcolor
    void CMenuElementUI::SetHotColor(ARGBREF argb)
    {
        m_refHotColor = argb;
    }

    ARGBREF CMenuElementUI::GetHotColor()
    {
        return m_refHotColor;
    }


    //textcolor
    void CMenuElementUI::SetHotTextColor(ARGBREF argb)
    {
        m_refHotTextColor = argb;
    }

    ARGBREF CMenuElementUI::GetHotTextColor()
    {
        return m_refHotTextColor;
    }

    void CMenuElementUI::SetSelectTextColor(ARGBREF argb)
    {
        m_refSelectTextColor = argb;
    }

    ARGBREF CMenuElementUI::GetSelectTextColor()
    {
        return m_refSelectTextColor;
    }

}