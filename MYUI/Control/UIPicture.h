
#ifndef _MYUI_PICTURE_CONTROL_H_
#define _MYUI_PICTURE_CONTROL_H_

#include "..//Core//UIControl.h"

#define MPICS_QRCODE   0x01
namespace MYUI
{

	class MYUI_API CPictureUI : public CControlUI
	{
	public:
		CPictureUI();
		virtual ~CPictureUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

        virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
        virtual void SetText(LPCTSTR pstrText);

        void ShowQrCode(bool bShow);
        bool QrCodeVisible();
	protected:
        virtual void PaintStatusImage(const RECT& rcItem, const RECT& rcPaint);
	private:

    protected:
        IMAGEINFO m_QrCode;
	};

}

#endif