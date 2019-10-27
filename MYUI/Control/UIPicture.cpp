
#include "UIPicture.h"
#include "../Utils/QR_Encode.h"

namespace MYUI
{
	CPictureUI::CPictureUI()
	{
        memset(&m_QrCode, 0, sizeof(m_QrCode));
	}

	CPictureUI::~CPictureUI()
	{
        if (m_QrCode.hBitmap)
        {
            DeleteObject(m_QrCode.hBitmap);
            memset(&m_QrCode, 0, sizeof(m_QrCode));
        }
	}

	CMuiString CPictureUI::g_strClassName(_T("PictureUI"));

	CMuiString CPictureUI::GetClassName() const
	{
		return CPictureUI::g_strClassName;
	}

    void CPictureUI::ShowQrCode(bool bShow)
    {
        m_dwStyle &= (~MPICS_QRCODE);
        if (bShow)
        {
            m_dwStyle |= MPICS_QRCODE;
            this->SetText(m_strText);
        }
    }

    bool CPictureUI::QrCodeVisible()
    {
        return MPICS_QRCODE & m_dwStyle;
    }

    void CPictureUI::SetAttribute(LPCTSTR strItem, LPCTSTR strValue)
    {
        if (0 == _tcsicmp(strItem, _T("QrCode")))
        {
#ifdef _DEBUG
            ASSERT(CheckBoer(strValue));
#endif
            ShowQrCode(CheckTrue(strValue));
        }
        else
        {
            __super::SetAttribute(strItem, strValue);
        }
    }

    void CPictureUI::SetText(LPCTSTR pstrText)
    {
        if (MPICS_QRCODE & m_dwStyle)
        {
            if (m_QrCode.hBitmap)
            {
                DeleteObject(m_QrCode.hBitmap);
                memset(&m_QrCode, 0, sizeof(m_QrCode));
            }

            m_QrCode.hBitmap = CreateQrCodeImage(m_pShareInfo ? m_pShareInfo->hPaintDC : NULL,
                pstrText, &m_QrCode.szBitmap);
            m_QrCode.bAlpha = true;
        }

        __super::SetText(pstrText);
    }

    void CPictureUI::PaintStatusImage(const RECT& rcItem, const RECT& rcPaint)
    {
        if (NULL == (MPICS_QRCODE & m_dwStyle) || NULL == m_QrCode.hBitmap) return;

        RECT rcClient = m_rcClient;
        RECT rcSource = { 0 }, rcCorner = { 0 };
        ARGBREF refMask = 0xFF000000;

        OffsetRect(&rcClient, rcItem.left, rcItem.top);

        rcSource.right = m_QrCode.szBitmap.cx;
        rcSource.bottom = m_QrCode.szBitmap.cy;

        m_pShareInfo->pRenderEngine->OnDrawImage(rcClient, &m_QrCode, rcSource, rcCorner,
            refMask, 255, m_QrCode.bAlpha);

        return;
    }
}