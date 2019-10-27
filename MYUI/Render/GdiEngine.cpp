
#include "GdiEngine.h"


namespace MYUI
{
	//���ű���
//	int check_encode_lenght(LPCTSTR pstr_c)
//	{
//		int c1 = 0x00FF & pstr_c[0];
//		int c2 = 0x00FF & pstr_c[1];
//#ifdef _UNICODE
//		return (pstr_c[0] >= 0xd800 && 0xdbff >= pstr_c[0] && pstr_c[1] >= 0xdc00 && 0xdfff >= pstr_c[1]) ? 2 : 1;
//#else 
//		//UTF-8
//		/*if(0x00 == (0x80 & (char)pstr_c[0])) return 1;
//		else if(0xC0 == (0xE0 & (char)pstr_c[0])) return 2;
//		else if(0xE0 == (0xF0 & (char)pstr_c[0])) return 3;
//		else if(0xF0 == (0xF8 & (char)pstr_c[0])) return 4;
//		else if(0xF8 == (0xFC & (char)pstr_c[0])) return 5;
//		else if(0xFC == (0xFE & (char)pstr_c[0])) return 6;
//		else return 1;*/
//		
//		return (c1 >= 0x81 && 0xFE >= c1 && c2>= 0x40 && 0xFE >= c2) ? 2 : 1;
//#endif
//
//	}

	static bool DrawImage(HDC hdcDest, const RECT &rcDest, HDC hdcSrc, const RECT &rcSrc,
		bool bAlpha, DWORD dwFade = 255)
	{
		if(FALSE == IsValidRect(rcDest) || FALSE == IsValidRect(rcSrc))
		{
			//rect����Ҫ������Ч�Ŀռ���ܽ��л���
			//�ռ���Ч������Ҫ���ƣ���������ʧ�ܣ�����TRUE
			return true;
		}
		
		//::SetStretchBltMode(hdcDest, HALFTONE);
		if(false == bAlpha && 255 == dwFade)//û��͸����
		{
			if(rcSrc.right - rcSrc.left == rcDest.right - rcDest.left
				&& rcSrc.bottom - rcSrc.top == rcDest.bottom - rcDest.top)
			{
				//û��λ��ƫ��, ͬλ����BitBlt����
				return BitBlt(hdcDest, rcDest.left, rcDest.top,
					rcDest.right - rcDest.left, rcDest.bottom - rcDest.top,
					hdcSrc, rcSrc.left, rcSrc.top, SRCCOPY);
			}
			else
			{
				//���츴��StretchBlt����
				//::SetStretchBltMode(hdcDest, HALFTONE);
				return StretchBlt(hdcDest, rcDest.left, rcDest.top, \
				rcDest.right - rcDest.left, rcDest.bottom - rcDest.top,
				hdcSrc, rcSrc.left, rcSrc.top, \
				rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top,
				SRCCOPY);
			}
			return false;
		}
		else//��Ҫ͸����
		{
			ARGBREF refMask = NULL;
			//�����TransparentBlt��������ɫ���ǲ���ִ�еģ���ֻ����ʱ������������
			//��������ִ�е�ԭ���ǣ��û���������ĳ����ɫ��������͸���ȣ���ô�ú�����ʵ�ֲ���
			//����������ɫ�������ڻ���ͼƬ֮ǰ
			if(NULL != GetRGBValue(refMask))
			{
				return TransparentBlt(hdcDest, rcDest.left, rcDest.top, \
					rcDest.right - rcDest.left, rcDest.bottom - rcDest.top,
					hdcSrc, rcSrc.left, rcSrc.top, \
					rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top,
					GetRGBValue(refMask));
			}
			else
			{
				BLENDFUNCTION blend = {0};//����͸��Ч��
				blend.BlendOp= AC_SRC_OVER;
				blend.BlendFlags = 0;
				blend.AlphaFormat = (bAlpha ? AC_SRC_ALPHA : 0);
				//blend.AlphaFormat=0;
				blend.SourceConstantAlpha= (BYTE)dwFade; // ͸���� ���255L 

				return AlphaBlend(hdcDest, rcDest.left, rcDest.top, \
					rcDest.right - rcDest.left, rcDest.bottom - rcDest.top,
					hdcSrc, rcSrc.left, rcSrc.top, \
					rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top,
					blend);
			}
		}
		return false;
	}

	/*******************************************************************************************
	 * �Ÿ���ͼ��
	 * @hdcDest Ŀ��DC
	 * @rcDest Ŀ������
	 * @hdcSrc ԴDC
	 * @rcSrc Դ����
	 * @rcCorner �и�����ÿ��ֵ����Դ�����������·ֱ��и���پ���
	 * @bAlpha ͼƬ�Ƿ��Դ�͸������
	 * @dwFade ͸���� (BYTE)255 Ϊ��͸��
	 * @bHole �����Ƿ�Ϊ�գ���ЩͼƬ�����ǿհ׵ģ��治���ƶ�һ����TRUE = �����ƿ������Ч��
	*******************************************************************************************/
	static bool NineGridsDrawImage(HDC hdcDest, const RECT &rcDest, HDC hdcSrc, const RECT &rcSrc,
		const RECT &rcCorner,bool bAlpha, DWORD dwFade, bool bHole)
	{
		/***********************************************************************************
		 *  �Ÿ���ͼ����ԭ���Լ����������ܼ򵥵ģ����Ǵ���ȽϷ��ӣ���������Ͳ�����̫����
		 *  �����ԭ���ٿ�����ͻᷢ�ֺܼ�
		 *  --------------------------------
		 *  |      |                |      |
		 *  |����  |      ����      |  ����|
		 *  |������x����������������x������|
		 *  |      |                |      |
		 *  |      |                |      |
		 *  |����  |      ����      |  ����|
		 *  |      |                |      |
		 *  |      |                |      |
		 *  |������x����������������x������|
		 *  |����  |      ����      |  ����|
		 *  |______|________________|______|
		 *  �ĸ�x ����rcCorner���ĸ��㣬�����hdcDest��rcSrc���м���
		 *  left�������Ҽ��㣬right����������㣬top�������¼��㣬bottom�������ϼ���
		 *  ע��:
		 *  ĳ��xֵΪ0�����
		 *  ���ϣ����ϣ����£������ĸ����򲻻����죬ֻ������в�����Ż�����
		 *  rcDest��ñ�rcSrc��,rcCorner���и�����Ҫ����rcSrc�ͳ��ָ���
		***********************************************************************************/
		if(rcDest.right - rcDest.left == rcSrc.right - rcSrc.left
			&& rcDest.bottom - rcDest.top == rcSrc.bottom - rcSrc.top
			&& false == bHole)
		{
			//��Сһ������������Ҳ��Ҫ���ƣ��������иֱ��COPY
			return DrawImage(hdcDest, rcDest, hdcSrc, rcSrc, bAlpha, dwFade);
		}

		//��ʼ����
		bool bResult = true;
		RECT rcMaster = {0}, rcSlave = {0};

		//����
		rcMaster = rcDest; rcSlave = rcSrc;
		rcMaster.right = rcMaster.left + rcCorner.left;
		rcMaster.bottom = rcMaster.top + rcCorner.top;
		rcSlave.right = rcSlave.left + rcCorner.left;
		rcSlave.bottom = rcSlave.top + rcCorner.top;
		bResult = DrawImage(hdcDest, rcMaster, hdcSrc, rcSlave, bAlpha, dwFade);
		if(false == bResult) return bResult;

		//����
		rcMaster = rcDest; rcSlave = rcSrc;
		rcMaster.left += rcCorner.left;
		rcMaster.right -= rcCorner.right;
		rcMaster.bottom = rcMaster.top + rcCorner.top;
		rcSlave.left += rcCorner.left;
		rcSlave.right -= rcCorner.right;
		rcSlave.bottom = rcSlave.top + rcCorner.top;
		bResult = DrawImage(hdcDest, rcMaster, hdcSrc, rcSlave, bAlpha, dwFade);
		if(false == bResult) return bResult;

		//����
		rcMaster = rcDest; rcSlave = rcSrc;
		rcMaster.left = rcMaster.right - rcCorner.right;
		rcMaster.bottom = rcMaster.top + rcCorner.top;
		rcSlave.left = rcSlave.right - rcCorner.right;
		rcSlave.bottom = rcSlave.top + rcCorner.top;
		bResult = DrawImage(hdcDest, rcMaster, hdcSrc, rcSlave, bAlpha, dwFade);
		if(false == bResult) return bResult;

		//���� 
		rcMaster = rcDest; rcSlave = rcSrc;
		rcMaster.top += rcCorner.top;
		rcMaster.right = rcMaster.left + rcCorner.left;
		rcMaster.bottom -= rcCorner.bottom;
		rcSlave.top += rcCorner.top;
		rcSlave.right = rcSlave.left + rcCorner.left;
		rcSlave.bottom -= rcCorner.bottom;
		bResult = DrawImage(hdcDest, rcMaster, hdcSrc, rcSlave, bAlpha, dwFade);
		if(false == bResult) return bResult;

		//����
		if(false == bHole)//��������Ϊ�գ���Ҫ����
		{
			rcMaster = rcDest; rcSlave = rcSrc;
			rcMaster.left +=  rcCorner.left;
			rcMaster.top +=  rcCorner.top;
			rcMaster.right -= rcCorner.right;
			rcMaster.bottom -= rcCorner.bottom;

			rcSlave.left +=  rcCorner.left;
			rcSlave.top +=  rcCorner.top;
			rcSlave.right -= rcCorner.right;
			rcSlave.bottom -= rcCorner.bottom;

			bResult = DrawImage(hdcDest, rcMaster, hdcSrc, rcSlave, bAlpha, dwFade);
			if(false == bResult) return bResult;
		}
		
		//����
		rcMaster = rcDest; rcSlave = rcSrc;
		rcMaster.left = rcMaster.right - rcCorner.right;
		rcMaster.top += rcCorner.top;
		rcMaster.bottom -= rcCorner.bottom;
		rcSlave.left = rcSlave.right - rcCorner.right;
		rcSlave.top += rcCorner.top;
		rcSlave.bottom -= rcCorner.bottom;
		bResult = DrawImage(hdcDest, rcMaster, hdcSrc, rcSlave, bAlpha, dwFade);
		if(false == bResult) return bResult;

		//����
		rcMaster = rcDest; rcSlave = rcSrc;
		rcMaster.top = rcMaster.bottom - rcCorner.bottom;
		rcMaster.right = rcMaster.left + rcCorner.left;
		rcSlave.top = rcSlave.bottom - rcCorner.bottom;
		rcSlave.right = rcSlave.left + rcCorner.left;
		bResult = DrawImage(hdcDest, rcMaster, hdcSrc, rcSlave, bAlpha, dwFade);
		if(false == bResult) return bResult;

		//����
		rcMaster = rcDest; rcSlave = rcSrc;
		rcMaster.left += rcCorner.left;
		rcMaster.top = rcMaster.bottom - rcCorner.bottom;
		rcMaster.right -= rcCorner.right;
		rcSlave.left += rcCorner.left;
		rcSlave.top = rcSlave.bottom - rcCorner.bottom;
		rcSlave.right -= rcCorner.right;
		bResult = DrawImage(hdcDest, rcMaster, hdcSrc, rcSlave, bAlpha, dwFade);
		if(false == bResult) return bResult;

		//����
		rcMaster = rcDest; rcSlave = rcSrc;
		rcMaster.left = rcMaster.right - rcCorner.right;
		rcMaster.top = rcMaster.bottom - rcCorner.bottom;
		rcSlave.left = rcSlave.right - rcCorner.right;
		rcSlave.top = rcSlave.bottom - rcCorner.bottom;
		bResult = DrawImage(hdcDest, rcMaster, hdcSrc, rcSlave, bAlpha, dwFade);
		if(false == bResult) return bResult;

		//���ƽ���
		return bResult;
	}

    CGdiRenderEngine::CGdiRenderEngine(HWND hWnd)
        :CRenderEngine(hWnd)
		,m_hMemBitmap(NULL)
		,m_hMemDc(NULL)
		,m_hTmpDc(NULL)
		,m_hWindowFont(NULL)
	{
		m_hPaintDc = ::GetDC((HWND)m_hWnd);
	}

	CGdiRenderEngine::~CGdiRenderEngine()
	{
		if(m_hMemBitmap) DeleteObject(m_hMemBitmap);
		if(m_hMemDc) DeleteDC(m_hMemDc);
		if(m_hTmpDc) DeleteDC(m_hTmpDc);
		if(m_hPaintDc) ReleaseDC(m_hWnd,m_hPaintDc);
	}

	bool CGdiRenderEngine::BeginPaint(const RECT &rcScreen)
	{
		//int nSave = SaveDC(ps.hdc);
		//TRACE(_T("BeginPaint"));

		//��ʼ�����ˣ������û���
		m_hMemDc = CreateCompatibleDC(m_hPaintDc);
		//��ʱ����Ҫ��m_hTmpDc���ƺú��ٸ��Ƶ�m_hMemDc
		m_hTmpDc = CreateCompatibleDC(m_hPaintDc);

		m_hMemBitmap = CreateCompatibleBitmap(m_hPaintDc, 
			rcScreen.right - rcScreen.left , rcScreen.bottom - rcScreen.top);
		SelectObject(m_hMemDc, m_hMemBitmap);
		m_hWindowFont = (HFONT)::SendMessage((HWND)m_hWnd, WM_GETFONT, NULL, NULL);
		return true;//������飬�򵥷���TRUE
	}

	bool CGdiRenderEngine::EndPaint(const RECT &rcUpdate)
	{
		//TRACE(_T("EndPaint"));
		bool bResult = false;
		//���ƽ��������ƻ��嵽����DC
		bResult =  DrawImage(m_hPaintDc, rcUpdate, m_hMemDc, rcUpdate, false, 255);
		if(m_hMemBitmap) DeleteObject(m_hMemBitmap);
		if(m_hMemDc) DeleteDC(m_hMemDc);
		if(m_hTmpDc) DeleteDC(m_hTmpDc);

		m_hMemBitmap = NULL;
		m_hMemDc = NULL;
		m_hTmpDc = NULL;
		//m_hWindowFont = NULL; //keep
		//RestoreDC(ps.hdc,nSave);
		return bResult;
	}

	HCLIP CGdiRenderEngine::EnterClip(const RECT &rcClient, SIZE &szRound)
	{
		//���ؾɵ���Ч������û��Լ�����
		RECT rcClip = {0,0,1,1};
		HRGN hOldRgn ,hRgnItem;

		::GetClipBox(m_hMemDc, &rcClip);//��ǰ��Ч��ͼ����
		hOldRgn = ::CreateRectRgnIndirect(&rcClip);
		::GetClipRgn(m_hMemDc, hOldRgn);//ȡ����Ч��ͼ����
		if(szRound.cx > 0 || szRound.cy > 0)
		{
			hRgnItem = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right + 1,
				rcClient.bottom + 1, szRound.cx, szRound.cy);
		}
		else
		{
			hRgnItem = ::CreateRectRgn(rcClient.left, rcClient.top, rcClient.right + 1,
				rcClient.bottom + 1);
		}

		::CombineRgn(hRgnItem, hOldRgn, hRgnItem, RGN_AND);
		//::SelectClipRgn(m_hMemDc, hRgnItem);
		::ExtSelectClipRgn(m_hMemDc, hRgnItem, RGN_AND);
		::DeleteObject(hRgnItem);
		return hOldRgn;
	}

	void  CGdiRenderEngine::LeaveClip(HCLIP hOldClip)
	{
		if(NULL == hOldClip) return;
		::SelectClipRgn(m_hMemDc, (HRGN)hOldClip);
		::DeleteObject(hOldClip);
	}

	HDC CGdiRenderEngine::GetMemDC()
	{
		return m_hMemDc;
	}

	void CGdiRenderEngine::ReleaseMemDC(HDC hMemDc)
	{
		return;
	}

	bool CGdiRenderEngine::OnDrawFrame(const RECT &rcDraw, ARGBREF refColor)
	{
		RECT rcFrame = rcDraw;
		if(0 == GetAValue(refColor)) return true;

		rcFrame.right += 1;
		rcFrame.bottom += 1;
		HBRUSH hBrush = ::CreateSolidBrush(0x00FFFFFF & refColor);
		::FrameRect(m_hMemDc, &rcFrame, hBrush);
		::DeleteObject(hBrush);
		return true;
	}

	bool CGdiRenderEngine::OnDrawLine(const POINT &ptBegin, const POINT &ptEnd, int nPenSize, ARGBREF refColor)
	{
		HPEN hPen = CreatePen(PS_SOLID,nPenSize ,0x00FFFFFF & refColor);
		HPEN hPenOld = (HPEN) SelectObject(m_hMemDc, hPen);

		MoveToEx(m_hMemDc, ptBegin.x, ptBegin.y,NULL);
		LineTo(m_hMemDc, ptEnd.x, ptEnd.y);

		SelectObject(m_hMemDc, hPenOld);
		DeleteObject(hPen);
		return true;
	}

	bool CGdiRenderEngine::OnDrawColor(const RECT &rcDraw, ARGBREF refColor)
	{
		//FillRect ʹ�û�ˢ����rect�ڲ�
		//FrameRect ʹ�û�ˢ����rect�߿�
		//Rectangle = FillRect + FrameRect, 
		//��Rectangle�������û�ˢ������ͨ�������Ļ������л��Ƶģ�Ч�ʸ���
		ARGBREF refOldColor = NULL;
		bool bResult = false;
		if(TRUE == IsEmptyRect(rcDraw)) return true;
		if(0 == GetAValue(refColor))
		{ 
			//��ȫ͸��,û�б�Ҫ���л�����
			return true;
		}
		if(255 == GetAValue(refColor))
		{
			//������͸����
			refOldColor = ::SetBkColor(m_hMemDc,GetRGBValue(refColor));
			::ExtTextOut(m_hMemDc, 0, 0, ETO_OPAQUE, &rcDraw, NULL, 0, NULL);
			::SetBkColor(m_hMemDc,refOldColor);
			return true;
		}
		//����͸���ȣ����⴦��
		//��͸��Ч��
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = 1;
		bmi.bmiHeader.biHeight = 1;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 1 * 1 * sizeof(ARGBREF);
		ARGBREF * pDest = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(m_hPaintDc, &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
		if( !hBitmap ) return false;

		*pDest = ARGB(GetAValue(refColor),GetBValue(refColor),GetGValue(refColor),GetRValue(refColor));//��ɫ��ֵ

		//��1X1����ɫͼƬ���츴�Ƶ�rcDraw
		HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hTmpDc, hBitmap);
		//::SetStretchBltMode(m_hTmpDc, HALFTONE);

		//bResult = DrawImage(m_hMemDc,rcDraw, m_hTmpDc
		bResult = StretchBlt(m_hMemDc, rcDraw.left, rcDraw.top, 
			rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
			m_hTmpDc, 0, 0, 1, 1, SRCCOPY);

		SelectObject(m_hTmpDc, hOldBitmap);
		DeleteObject(hBitmap);
		return bResult;
	}

	bool CGdiRenderEngine::OnDrawText(const RECT &rcDraw, LPCTSTR strText, ARGBREF refTextColor, HFONT hFont, DWORD dwType)
	{
		//��ȫ͸����û�б�Ҫ����
		if(0 == GetAValue(refTextColor)) return true;

		if(NULL == hFont) hFont = m_hWindowFont;

		if(255 == GetAValue(refTextColor))
		{
			//û��͸��Ч����ֱ��д��
			HFONT hOldFont = NULL;

			int nBkMode = SetBkMode(m_hMemDc,TRANSPARENT);
			if(hFont) hOldFont = (HFONT)SelectObject(m_hMemDc, hFont);
			COLORREF OldTextColor = SetTextColor(m_hMemDc,GetRGBValue(refTextColor));
			::DrawText(m_hMemDc, strText, -1,  (LPRECT)&rcDraw, dwType);
			SetTextColor(m_hMemDc ,OldTextColor);
			if(hFont) SelectObject(m_hMemDc, hOldFont);
			SetBkMode(m_hMemDc,nBkMode);
		}
		else
		{
			//����͸��Ч��
			//���ְ�͸������Ŀ���ͦ��
			ASSERT(FALSE);
			HFONT hOldFont = NULL;

			int nWidth = rcDraw.right - rcDraw.left;
			int nHeight = rcDraw.bottom - rcDraw.top;
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = nWidth;
			bmi.bmiHeader.biHeight = nHeight;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = nWidth * nHeight * sizeof(ARGBREF);
			ARGBREF * pDest = NULL;
			HBITMAP hTmpBitmap = ::CreateDIBSection(m_hPaintDc, &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
			if( !hTmpBitmap ) return false;

			HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hTmpDc, hTmpBitmap);
			int bkMode = SetBkMode(m_hTmpDc,TRANSPARENT);
			if(hFont) hOldFont = (HFONT)SelectObject(m_hTmpDc, hFont);
			COLORREF OldTextColor = SetTextColor(m_hTmpDc,GetRGBValue(refTextColor));
			::DrawText(m_hTmpDc, strText, -1,  (LPRECT)&rcDraw, dwType);

			//��͸������
			ARGBREF abgr = ARGB(GetAValue(refTextColor),GetBValue(refTextColor),
				GetGValue(refTextColor),GetRValue(refTextColor));
			for(int i = 0; nWidth * nHeight > i; i++)
			{
				if(pDest[i])//ֻ������λ�õ������ǲ�͸����
				{
					//����͸������
					pDest[i] = abgr;
				}
			}

			//д����ɣ��ϲ���ͼ
			BitBlt(m_hMemDc, rcDraw.left ,rcDraw.top,
				rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
				m_hTmpDc, 0 ,0 ,SRCPAINT);//��ɫ�ϲ�
			
			SetTextColor(m_hTmpDc ,OldTextColor);
			if(hFont) SelectObject(m_hTmpDc, hOldFont);
			SetBkMode(m_hTmpDc, bkMode);
			DeleteObject(hTmpBitmap);
		}

		return true;
	}

	bool CGdiRenderEngine::OnDrawBroder(const RECT &rcDraw, ARGBREF refColor, int nBroderSize, SIZE &szRound)
	{
		if(0 == GetAValue(refColor) || 0 == nBroderSize) return true;//��ȫ͸����������

		HDC hCache = m_hMemDc;
		if(255 > GetAValue(refColor))//��͸��Ч��
		{
			hCache = m_hTmpDc;
		}
		
		HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nBroderSize ,GetRGBValue(refColor));
		HPEN hOldPen = (HPEN)::SelectObject(hCache, hPen);
		HBRUSH hOldBrush = (HBRUSH)::SelectObject(hCache, ::GetStockObject(HOLLOW_BRUSH));

		if(255 == GetAValue(refColor))
		{
			if(szRound.cx > 0 || szRound.cy > 0)//��Բ��
			{
				::RoundRect(m_hMemDc, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,\
					szRound.cx, szRound.cy);
			}
			else//������
			{
				::Rectangle(m_hMemDc, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom);
			}
		}
		else
		{
			//��͸��Ч��
			ASSERT(FALSE);
			int nWidth = rcDraw.right - rcDraw.left;
			int nHeight = rcDraw.bottom - rcDraw.top;
			BITMAPINFO bmi = { 0 };
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = nWidth;
			bmi.bmiHeader.biHeight = nHeight;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = nWidth * nHeight * sizeof(ARGBREF);
			ARGBREF * pDest = NULL;
			HBITMAP hTmpBitmap = ::CreateDIBSection(m_hPaintDc, &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
			if( !hTmpBitmap ) return false;

			HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hTmpDc, hTmpBitmap);

			if(szRound.cx > 0 || szRound.cy > 0)//��Բ��
			{
				::RoundRect(m_hTmpDc, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,\
					szRound.cx, szRound.cy);
			}
			else//������
			{
				::Rectangle(m_hTmpDc, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom);
			}

			ARGBREF abgr = ARGB(GetAValue(refColor),GetBValue(refColor),
				GetGValue(refColor),GetRValue(refColor));
			for(int i = 0; nWidth * nHeight > i; i++)
			{
				if(pDest[i])//ֻ�б߿�λ�õ������ǲ�͸����
				{
					//����͸������
					pDest[i] = abgr;
				}
			}

			BitBlt(m_hMemDc, rcDraw.left ,rcDraw.top,
				rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
				m_hTmpDc, 0 ,0 ,SRCPAINT);//��ɫ�ϲ�
			DeleteObject(hTmpBitmap);
		}

		::SelectObject(hCache, hOldBrush);
		::SelectObject(hCache, hOldPen);
		::DeleteObject(hPen);
		return true;
	}

	bool CGdiRenderEngine::OnDrawImage(const RECT &rcDraw, IMAGEINFO * pImageInfo, const RECT &rcSource,
		const RECT &rcCorner, ARGBREF refMask, DWORD dwFade, bool bHole)
	{
		//TransparentBlt������ɫ
		//AlphaBlendָ��͸����
		//BitBlt����ͼ��
		//CreateDIBSectionֱ�Ӷ�HDC��BITMAP���ж�д
		bool bResult = false;

		RECT rcSrc = {0};
		HBITMAP hTmpBitmap = NULL, hOldBitmap = NULL;
		HDC hCacheDc = NULL;
		BITMAPINFO bmi = { 0 };
		ARGBREF * pDest = NULL;
		bool bAlpha = pImageInfo->bAlpha;
		//ʹ��refMask�����Ӷ��⿪����������ĺ�����֪���ˣ�����Ҫ�����µĶ���
		if(GetRGBValue(refMask))
		{
			//��ȫ͸������������ɫ��Ϊ��ɫ
			hCacheDc = CreateCompatibleDC(m_hPaintDc);
			SelectObject(hCacheDc, pImageInfo->hBitmap);
			rcSrc.left = rcSrc.top = 0;
			rcSrc.right = rcSource.right - rcSource.left;
			rcSrc.bottom = rcSource.bottom - rcSource.top;

			//����TransparentBlt֮����AlphaBlend�����ֻ�ʲô��û����ʾ�������������Լ�������ɫ���δ���
			/*bResult = TransparentBlt(m_hTmpDc, rcSrc.left, rcSrc.top, rcSrc.right, rcSrc.bottom,
				hCacheDc, rcSource.left, rcSource.top, rcSrc.right, rcSrc.bottom,
				GetRGBValue(refMask));*/
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = rcSource.right - rcSource.left;
			bmi.bmiHeader.biHeight = rcSource.bottom - rcSource.top;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;//ֻ֧��32λͼƬ
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * sizeof(ARGBREF);
			
			hTmpBitmap = ::CreateDIBSection(m_hPaintDc, &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
			hOldBitmap = (HBITMAP)SelectObject(m_hTmpDc, hTmpBitmap);

			if(hOldBitmap)
			{
				//��ͼƬ���Ƶ�m_hTmpDc
				refMask = ARGB(0 ,GetBValue(refMask),GetGValue(refMask),GetRValue(refMask));//��ɫ��ֵ
				bResult = BitBlt(m_hTmpDc,0,0, rcSrc.right, rcSrc.bottom, hCacheDc,0,0,SRCCOPY);
				//������ɫ
				for(int i = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight - 1; i >= 0; i--)
				{
					if((pDest[i] & 0x00FFFFFF) == refMask)
					{
						pDest[i] = NULL;
					}
				}
				bAlpha = true;
			}
		}
		else
		{
			rcSrc = rcSource;
			hOldBitmap = (HBITMAP)SelectObject(m_hTmpDc, pImageInfo->hBitmap);
			if(hOldBitmap) bResult = true;
		}

		if(false == bResult)
		{
			goto end;
		}

		if(FALSE == IsEmptyRect(rcCorner))
		{
			//��ͼ����Ϊ�գ�Ҫ������ͼ
			//���оŸ��и��ͼ
			bResult = NineGridsDrawImage(m_hMemDc, rcDraw, m_hTmpDc, rcSrc, rcCorner,
				bAlpha, dwFade, bHole);
		}
		else//����Ҫ��ͼ
		{
			bResult = DrawImage(m_hMemDc, rcDraw, m_hTmpDc, rcSrc, bAlpha, dwFade);
		}

		//��ɲ������ͷŶ���
end:
		if(hOldBitmap) SelectObject(m_hTmpDc, hOldBitmap);
		if(hTmpBitmap) DeleteObject(hTmpBitmap);
		if(hCacheDc) DeleteDC(hCacheDc);
		return bResult;
	}


	bool CGdiRenderEngine::OnDrawHtmlText(const RECT &rcDraw, LPCTSTR strText, CMuiIdArray * FontArray, ARGBREF refTextColor)
	{
		//��ʼ������
		bool bRet = false;
		int index = 0;
		ARGBREF refOldTextColor = ::SetTextColor(m_hMemDc, refTextColor);
		ARGBREF refOldBkColor = ::GetBkColor(m_hMemDc);
		HFONT hOldFont = (HFONT)::GetCurrentObject(m_hMemDc, OBJ_FONT);
		int nBkMode = ::SetBkMode(m_hMemDc,TRANSPARENT);

		//���н��л���
		int nStrIndex = 0;
		int nLineLen = 0;
		int nRowSpace = 0;
		int nLineHeight = 0;
		int nOffset = 0;
		RECT rcLine = {0};
		
		while(nLineLen = CGdiRenderEngine::GetHtmlTextLineHeight(&strText[nStrIndex],
			FontArray, nLineHeight, nRowSpace))
		{
			rcLine.left = rcDraw.left;
			rcLine.right = rcDraw.right;
			rcLine.top = rcDraw.top + nOffset;
			rcLine.bottom = rcLine.top + nLineHeight;
			index = 0;

			bRet = DrawHtmlTextLine(rcLine, &strText[nStrIndex], nLineLen, index, 
                FontArray, -1, refTextColor, NULL, DT_SINGLELINE | DT_LEFT | DT_BOTTOM);

			nOffset += nLineHeight + nRowSpace;
			nStrIndex += nLineLen + 1;
		}
		
		//��ԭ
		::SetBkMode(m_hMemDc,nBkMode);
		::SelectObject(m_hMemDc, hOldFont);
		::SetBkColor(m_hMemDc, refOldBkColor);
		::SetTextColor(m_hMemDc, refTextColor);
		return bRet;
	}

/*ƫ�Ƶ�ָ��β��*/
#ifdef _DEBUG
#define OFFSET_TO_END \
	while(_T('}')!=strText[index] && _T('\0')!=strText[index]){++index;}\
	TRACE(_T("strText[%d] = %c"), index, strText[index])
#else
	while(_T('}')!=strText[index] && _T('\0')!=strText[index]){++index;}
#endif
/*ƫ�Ƶ�ָ��β��*/

/*��ָ����ȡ��������strParam��*/
#ifdef _DEBUG
#define GET_PARAM_STRING \
	i =0;\
	index++;while(_T(' ')==strText[index]){++index;};\
	while(_T(' ') != strText[index] && _T('}') != strText[index])\
	{strParam[i++] = strText[index++];}\
	strParam[i] = _T('\0');\
	i=0;\
	TRACE(_T("strText[%d] = %c, strParam = %s"), index, strText[index], strParam)
#else
#define GET_PARAM_STRING \
	i =0;\
	index++;while(_T(' ')==strText[index]){++index;};\
	while(_T(' ') != strText[index] && _T('}') != strText[index])\
	{strParam[i++] = strText[index++];}\
	strParam[i] = _T('\0');\
	i=0;\

#endif

/*��ָ����ȡ��������strParam��*/
	bool CGdiRenderEngine::DrawHtmlTextLine(RECT &rcLine, LPCTSTR strText, int nLenght, int &index,
			CMuiIdArray * FontArray, int nFontId, ARGBREF refTextColor, ARGBREF refTextBkColor, DWORD dwTextStyle)
	{
		//ע��ָ�����ִ�Сд, ���Ҳ��ܳ���30����ĸ
		//{p x}text{/p} ���У�x��ʾ�˶����о�. 
		//��{p}ָ����ڣ�{p}������Ƕ�׵ĵ�һ�㣬����{p}��ֹǶ������!!

		//  #{ = _T('{')    #} = _T('}')   ## = _T('#')
		//{f id} text {/f}  id����fontid
		//{i x} ��ʾͼƬ x = ·��
		//{c ref}text{/c}������ɫ
		//{b ref}text{/b} ��ʾѡ����ɫ����ʵ�����Ϊ������ɫ
		//{x i} ��ʾ�Ӵ˴�������ƶ�i������ֵ
		//{a xx}text{/a} �ı����뷽ʽ����top �� mid, Ĭ�� = ��bottom;
		//�ı�ģʽ,����ȡtextʱ��ΪTRUE, ��ȡ{xxx}ʱ��ΪFALSE
		RECT rcDraw = {0};
		BOOL bCommandMode = FALSE;
		TCHAR cmd = _T('\0');
		int i =0;
		
		int nBufIndex = 0;

		TCHAR * strStartCmd = NULL;
		TCHAR * strEndCmd = NULL;
		TCHAR * strParam = NULL;
		TCHAR * strBuffer = NULL;
#define COMMANDSIZE  128
		strStartCmd = (TCHAR *) new char[4096];
		memset(strStartCmd, 0, 4096);
		strEndCmd = strStartCmd + COMMANDSIZE;
		strParam = strEndCmd + COMMANDSIZE;
		strBuffer = strParam + COMMANDSIZE;

		while(nLenght > index)
		{
			//TRACE(_T("check strText[%d] = %c"), index, strText[index]);
			if(_T('{') == strText[index])
			{
				if(TRUE == bCommandMode)
				{
					i = index + 1;
					while(_T(' ')==strText[i]){++i;};

					if(_T('/') != strText[i])
					{
						//�Ѿ��ڷ���һ��ָ���ˣ��ڶ���ָ���������ô���ύ���ݹ鴦��

						//�ȴ��������ı�����

						CGdiRenderEngine::ShowHtmlText(rcLine, strBuffer, nBufIndex,FontArray,
							nFontId, refTextColor, refTextBkColor,  dwTextStyle);

						nBufIndex = 0;//�Ѿ����ƹ��ˣ�����
                        CGdiRenderEngine::DrawHtmlTextLine(rcLine, strText, nLenght, index, FontArray,
							 nFontId, refTextColor, refTextBkColor, dwTextStyle);

						TRACE(_T("goback strText[%d] = %c"), index, strText[index]);
						++index;
						continue;
					}
				}

				//����ָ��ģʽ
				bCommandMode = TRUE;
				++index; while(_T(' ')==strText[index]){++index;};
				TRACE(_T("strText[%d] = %c"), index, strText[index]);
				if(_T('/') != strText[index])
				{
					cmd = strText[index];
				}

				switch (strText[index])
				{
				case _T('/'):
					{
						//���л���
						++index; while(_T(' ')==strText[index]){++index;};
						TRACE(_T("cmd=%c \t strText[%d] = %c"),cmd, index, strText[index]);
						ASSERT(cmd == strText[index] && "{?}ָ����β����Ӧ");

						CGdiRenderEngine::ShowHtmlText(rcLine, strBuffer, nBufIndex,FontArray,
							nFontId, refTextColor, refTextBkColor,  dwTextStyle);
						OFFSET_TO_END;
						goto finish;
					}break;
				case _T('f'):
					{
						GET_PARAM_STRING;
						nFontId = _tcstol(strParam, NULL, 10);
						OFFSET_TO_END;
					}break;
				case _T('i'):
					{
						GET_PARAM_STRING;

                        IMAGEINFO * pImageInfo = CSkinManager::GetImageInfo(m_strSkin, strParam);
						if(pImageInfo)
						{
							rcDraw = rcLine;
							rcDraw.right = rcDraw.left + pImageInfo->szBitmap.cx;
							
							int nOffset = (rcDraw.bottom - rcDraw.top - pImageInfo->szBitmap.cy) / 2;
							nOffset = MAX(0, nOffset);
							rcDraw.top += nOffset;
							rcDraw.bottom -= nOffset;
							rcLine.left = rcDraw.right;
							CRenderEngine::OnDrawImage(rcDraw,strParam);
						}
						OFFSET_TO_END;
						goto finish;
					}break;
				case _T('c'):
					{
						GET_PARAM_STRING;
						refTextColor = GetArgbFromString(strParam);
						OFFSET_TO_END;
						TRACE(_T("ָ��{c} param=%s, color=0x%x"), strParam, refTextColor);
					}break;
				case _T('b'):
					{
						GET_PARAM_STRING;
						refTextBkColor = GetArgbFromString(strParam);
						OFFSET_TO_END;
					}break;
				case _T('a'):
					{
						GET_PARAM_STRING;

						if(0 == _tcsicmp(_T("top"), strParam))
						{
							dwTextStyle = DT_SINGLELINE | DT_LEFT | DT_TOP;
						}
						else if(0 == _tcsicmp(_T("middle"), strParam))
						{
							dwTextStyle = DT_SINGLELINE | DT_LEFT | DT_VCENTER;
						}
						else if(0 == _tcsicmp(_T("bottom"), strParam))
						{
							dwTextStyle = DT_SINGLELINE | DT_LEFT | DT_BOTTOM;
						}
						else
						{
							TRACE(_T("{a} ָ���У���⵽δ��ʶ��Ĳ���: %s"), strParam);
							ASSERT(0 && "{a} ָ���У���⵽δ��ʶ��Ĳ���");
						}

						OFFSET_TO_END;
					}break;
				case _T('p'):
					{
						//�ϲ��Ѿ������Ǵ�����ˣ�ƫ��index����β����
						OFFSET_TO_END;
					}break;
				case _T('x'):
					{
						GET_PARAM_STRING;
						rcLine.left += _tcstol(strParam, NULL, 10);
						OFFSET_TO_END;
						goto finish;
					}break;
				default:
					{
						TRACE(_T("DrawHtmlTextLine�У���⵽δ��ʶ���ָ��: %c"), strText[index]);
						ASSERT(0 && "CGdiRenderEngine::DrawHtmlTextLine����⵽δ��ʶ���ָ��");
					}break;
				}
			}
			//else if(_T('\r') == strText[index] || _T('\n') == strText[index])
			//{
			//	//����
			//}
			else
			{
				if(_T('#') == strText[index])
				{
					//ת��ģʽ����Ϊ����ģʽ�Ǵ�{}�ж�ȡ�ģ������ó���ĸ�ʽ����ʾ{}������Ҫ����һ��ת��
					//�ַ�������{}����ת�壬�磺#{ = {      #} = }   ## = #
					if(_T('{') == strText[index + 1])
					{
						strBuffer[nBufIndex++] = strText[++index];
					}
					else if((_T('{') == strText[index + 1]))
					{
						strBuffer[nBufIndex++] = strText[++index];
					}
					else if((_T('#') == strText[index + 1]))
					{
						strBuffer[nBufIndex++] = strText[++index];
					}
					else
					{
						strBuffer[nBufIndex++] = strText[index];
					}
				}
				else
				{
					strBuffer[nBufIndex++] = strText[index];
				}
			}
			index ++;
		}

finish:
		delete strStartCmd;

		return true;
	}

	int CGdiRenderEngine::GetHtmlTextLineHeight(LPCTSTR strText, CMuiIdArray * FontArray, 
		int &nHeight, int &nRowSpace)
	{
		TCHAR * strBuffer = new TCHAR[1024];
		IMAGEINFO * pImageInfo = NULL;
		int i =0;
		nRowSpace = 0;
		int index = 0;
		BOOL bParagraph = FALSE;
		BOOL bTextMode = FALSE;

		while(strText[index])
		{
			if(_T('{') == strText[index])
			{
				index++; while(_T(' ')==strText[index]){++index;};

				if(_T('p') == strText[index] && (_T(' ') == strText[index+1] || _T('}') == strText[index+1]))
				{
					if(TRUE == bTextMode)
					{
						while(_T('{') != strText[index--]);
						return index;
					}
					else
					{
						i =0;
						index++; while(_T(' ')==strText[index]){++index;};
						while(_T(' ') != strText[index] && _T('}') != strText[index])
						{
							strBuffer[i++] = strText[index++];
						}
						strBuffer[i] = _T('\0');
						nRowSpace = _tcstol(strBuffer, NULL, 10);
						bParagraph = TRUE;//ӵ�л��б�־
					}
				}
				else if(_T('/') == strText[index] && _T('p') == strText[index+1] && 
					(_T(' ') == strText[index+2] || _T('}') == strText[index+2]))
				{
					//�ҵ�һ�е�ת��
					ASSERT(bParagraph && "{/p}ǰ�������{p}�����Ӧ");
					while(_T('}') != strText[++index]);
					return index;
				}
				else if(_T('f') == strText[index] && _T(' ') == strText[index+1])
				{
					i =0;
					index++; while(_T(' ')==strText[index]){++index;};
					while(_T(' ') != strText[index] && _T('}') != strText[index])
					{
						strBuffer[i++] = strText[index++];
					}
					strBuffer[i] = _T('\0');

					HFONT hFont = (HFONT)FontArray->Select(_tcstol(strBuffer, NULL, 10));

					int nFontHeight = CGdiRenderEngine::GetTextHeight(hFont);
					nHeight = nFontHeight > nHeight ? nFontHeight : nHeight;
				}
				else if(_T('i') == strText[index] && _T(' ') == strText[index+1])
				{
					strBuffer[0] = _T('\0');
					i =0;

					index++; while(_T(' ')==strText[index]){++index;};
					while(_T(' ') != strText[index] && _T('}') != strText[index])
					{
						strBuffer[i++] = strText[index++];
					}
					strBuffer[i] = _T('\0');

					pImageInfo = CSkinManager::GetImageInfo(m_strSkin, strBuffer);

					ASSERT(pImageInfo && "CGdiRenderEngine::GetHtmlTextLineHeight pImageInfo Is NULL");
					if(pImageInfo && pImageInfo->szBitmap.cy > nHeight)
					{
						nHeight = pImageInfo->szBitmap.cy;
					}
				}
				else
				{
					
				}

				while(_T('}') != strText[index]) {++index;};//ƫ�Ƶ�_T('}');
				TRACE(_T("strText[%d] = %c"), index, strText[index]);
			}
			else
			{
				bTextMode = TRUE;
				if(_T('#') == strText[index])
				{
					if(_T('#') == strText[index + 1] ||
						_T('{') == strText[index + 1] || _T('}') == strText[index + 1])
					{
						index++;
					}
				}
			}

			index++;
		}

		return index;
	}

	bool CGdiRenderEngine::ShowHtmlText(RECT &rcLine,LPCTSTR strText, int nLenght,CMuiIdArray * FontArray,
			int nFontId, ARGBREF refTextColor, ARGBREF refTextBkColor, DWORD dwTextStyle)
	{
		SIZE szText = {0};
		RECT rcDraw;
		HFONT hFont = NULL ,hOldFont = NULL;
		ARGBREF refOldTextColor = NULL;
		ARGBREF refOldTextBkColor = NULL;
		if(FALSE == IsValidRect(rcLine) || 0 == nLenght) return false;
		TRACE(_T("ShowHtmlText ={f %d}{c #%x}{b #%x} %s {/f}{/c}{/b}"),
			nFontId, refTextColor, refTextBkColor, strText);
		//��ʼ������

		//���fָ���id������Ч��ֵ����ô���������������
		hFont = (HFONT)FontArray->Select(nFontId);
		if(hFont) hOldFont = (HFONT)::SelectObject(m_hMemDc, hFont);

		//Ҫע�⵱����б��ʱ��������������Ȳ�һ���ʺ�ʹ��
		GetTextExtentPoint(m_hMemDc,strText ,nLenght, &szText);
		if(0 != GetAValue(refTextBkColor))
		{
			rcDraw = rcLine;
			rcDraw.right = MIN(rcLine.right, rcDraw.left + szText.cx);

			if(DT_TOP & dwTextStyle)
			{
				rcDraw.bottom = rcDraw.top + szText.cy;
			}
			else if(DT_BOTTOM & dwTextStyle)
			{
				rcDraw.top = rcDraw.bottom - szText.cy;
			}
			else if(DT_VCENTER & dwTextStyle)
			{
				int nOffset = (rcDraw.bottom - rcDraw.top - szText.cy) / 2;
				nOffset = MAX(0, nOffset);
				rcDraw.bottom -= nOffset;
				rcDraw.top += nOffset;
			}
			//���Ʊ���
			HBRUSH hBrush = ::CreateSolidBrush(GetRGBValue(refTextBkColor));
			//refOldTextBkColor = ::SetBkColor(m_hMemDc,GetRGBValue(refTextBkColor));
			//::ExtTextOut(m_hMemDc, 0, 0, ETO_OPAQUE, &rcDraw, NULL, 0, NULL);
			::FillRect(m_hMemDc, &rcDraw, hBrush);
			::DeleteObject(hBrush);
		}
		refOldTextColor = ::SetTextColor(m_hMemDc, GetRGBValue(refTextColor));

		//����
		::DrawText(m_hMemDc, strText, nLenght,  (LPRECT)&rcLine, dwTextStyle);

		//��ԭ
		::SetTextColor(m_hMemDc, refOldTextColor);
		if(hOldFont) ::SelectObject(m_hMemDc, hOldFont);

		rcLine.left += szText.cx;
		return true;
	}

	int CGdiRenderEngine::GetTextHeight(HFONT hFont)
	{
		int nHeight = 0;
		TEXTMETRIC tm;
		HFONT hOldFont = NULL;
		if(NULL == hFont) hFont = m_hWindowFont;

		if(hFont) hOldFont = (HFONT)SelectObject(m_hPaintDc, hFont);
		if(TRUE == GetTextMetrics(m_hPaintDc, &tm))
		{
			nHeight = tm.tmHeight;
		}
		if(hFont) SelectObject(m_hPaintDc, hOldFont);
		return nHeight;
	}

	SIZE CGdiRenderEngine::GetTextSize(const RECT &rcItem, LPCTSTR strText, int nLenght, 
		HFONT hFont, int nRowSpace, DWORD dwTextStyle)
	{
		SIZE szText = {0};
		SIZE szTmp = {0};
		HFONT hOldFont = NULL;
		TEXTMETRIC tm = {0};
		BOOL bHasNext = FALSE;
		int nStrStart = 0;
		int nWidth = 0, nHeight = 0;
		int nStart = 0, nCount = 0, nTabCount = 0;
		int nEncodeLenght = 0, nTextWidth = 0;
		if(NULL == strText || 0 == nLenght) return szText;

		if(NULL == hFont) hFont = m_hWindowFont;
		if(hFont) hOldFont = (HFONT)SelectObject(m_hPaintDc, hFont);

		if(FALSE == ((TOS_AUTOLINEFREE | TOS_MULTILLINE) & dwTextStyle))
		{
			GetTextExtentPoint(m_hPaintDc,strText ,nLenght, &szText);
			goto finish;
		}

		GetTextMetrics(m_hPaintDc, &tm);
		GetTextExtentPoint(m_hPaintDc, _T("\t"), 2, &szTmp);
		int nHasTabSize = szTmp.cx ? 1 : 0;
		int nTabWidth = szTmp.cx ? szTmp.cx : (tm.tmAveCharWidth + 1) / 2;

		int nTextRow = 0;
		if(TOS_AUTOLINEFREE)
		{
			nStrStart = 0;
			if(nLenght) szText.cy = tm.tmHeight + nRowSpace;
			for(int nIndex = 0; nLenght > nIndex; nIndex)
			{
				nEncodeLenght = check_encode_lenght(&strText[nIndex]);
				GetTextExtentPoint(m_hPaintDc, &strText[nIndex], nEncodeLenght, &szTmp);
				
				if(_T('\t') == strText[nIndex])
				{
					szTmp.cx = TABSPACE * nTabWidth;
				}
				nTextWidth += szTmp.cx;
				
				//ע���ж����ȼ�
				if(_T('\r') == strText[nIndex])
				{
					if(_T('\n') == strText[nIndex + 1])
					{
						nIndex += nEncodeLenght;
					}
					nTextWidth = 0;
					szText.cy += tm.tmHeight + nRowSpace;
				}
				else if(_T('\n') == strText[nIndex])
				{
					if(_T('\r') == strText[nIndex + 1])
					{
						nIndex += nEncodeLenght;
					}
					nTextWidth = 0;
					szText.cy += tm.tmHeight + nRowSpace;
				}
				else if(nTextWidth > rcItem.right)
				{
					if(_T('\t') == strText[nIndex])
					{
					}
					else if(nTextWidth == szTmp.cx)
					{
					}
					else 
					{
						nEncodeLenght = 0;
					}
					nTextWidth = 0;
					szText.cy += tm.tmHeight + nRowSpace;
				}

				nIndex += nEncodeLenght;

			}
			szText.cx = rcItem.right - rcItem.left;
		}
		else
		{
			do
			{
				nTabCount = 0;
				bHasNext = GetStringLine(strText, nLenght, nStart, &nCount, &nTabCount);
				//ע�⣬���з�Ҳ����������ڣ������������ǿ��������з�ռ�ݵ�λ�õ�
				//���ǵ�������ѡ��ʱ��ѡ������Ҫ�����з�ռ�ݵ�����Ҳ���Ƴ���
				GetTextExtentPoint(m_hPaintDc, strText + nStart, nCount, &szTmp);
				szTmp.cx += nTabCount * (TABSPACE - nHasTabSize) * nTabWidth;
				
				szText.cx = MAX(szText.cx, szTmp.cx);
				szText.cy += szTmp.cy + nRowSpace;
				nStart += nCount;
			}while (bHasNext);
		}

finish:
		if(hFont) SelectObject(m_hPaintDc, hOldFont);
		return szText;
	}

	int CGdiRenderEngine::TestTextIndex(const RECT &rcItem, POINT &ptMouse, 
		LPCTSTR strText, int nLenght, HFONT hFont, int nRowSpace, DWORD dwTextStyle)
	{
		HFONT hOldFont = NULL;
		POINT ptPos = {0};
		int nStart = 0, nCount = 0;
		bool bHasNext = false;
		SIZE szText = {0};
		TEXTMETRIC tm = {0};
		int index = 0;
		int nTabCount = 0;
		int nTabWidth = 0;
		int nPointRow = 0;
		int nTextWidth = 0;
		int nEncodeLenght = 0;

		//׼������
		if(NULL == hFont) hFont = m_hWindowFont;
		if(hFont) hOldFont = (HFONT)SelectObject(m_hPaintDc, hFont);
		GetTextMetrics(m_hPaintDc, &tm);
		GetTextExtentPoint(m_hPaintDc, _T("\t"), 2, &szText);

		nTabWidth = szText.cx ? szText.cx : (tm.tmAveCharWidth + 1) / 2;

		ptPos.x = rcItem.left;
		ptPos.y = rcItem.top;
		if(FALSE == ((TOS_AUTOLINEFREE | TOS_MULTILLINE) & dwTextStyle))//����
		{
			ptPos.y += MAX(0, (rcItem.bottom - rcItem.top - tm.tmHeight) / 2);//��ֱ����

			if(TOS_CENTER & dwTextStyle)//ˮƽ����
			{
				GetTextExtentPoint(m_hPaintDc,strText ,nLenght, &szText);
				ptPos.x += MAX(0, ((rcItem.right - rcItem.left) - szText.cx) / 2);
			}

			for(index = 0; nLenght > index; index)
			{
				if(ptPos.x + POINTDIFF > ptMouse.x)
				{
					//�û������ܰٷְٻ���һ�����أ�һ����Ϊ4����
					break;
				}
				nEncodeLenght = check_encode_lenght(&strText[index]);
				GetTextExtentPoint(m_hPaintDc,strText + index ,nEncodeLenght, &szText);
				ptPos.x += szText.cx;
				index += nEncodeLenght;
			}
			ptMouse = ptPos;
			goto finish;
		}

		if(TOS_AUTOLINEFREE & dwTextStyle)
		{
			nStart = 0;

			//�ҳ�yֵ
			for(index = 0; nLenght > index; index)
			{
				if(ptPos.x == rcItem.left && ptPos.y + tm.tmHeight + nRowSpace > ptMouse.y) break;

				nEncodeLenght = check_encode_lenght(&strText[index]);
				if(nLenght == index + nEncodeLenght) break;
				GetTextExtentPoint(m_hPaintDc, &strText[index], nEncodeLenght, &szText);

				if(_T('\t') == strText[index])
				{
					szText.cx = TABSPACE * nTabWidth;
				}
				ptPos.x += szText.cx;

				//ע���ж����ȼ�
				if(_T('\r') == strText[index])
				{
					if(_T('\n') == strText[index + 1])
					{
						index += nEncodeLenght;
					}
					nStart = index + nEncodeLenght;
					ptPos.x = rcItem.left;
					ptPos.y += tm.tmHeight + nRowSpace;
				}
				else if(_T('\n') == strText[index])
				{
					if(_T('\r') == strText[index + 1])
					{
						index += nEncodeLenght;
					}
					nStart = index + nEncodeLenght;
					ptPos.x = rcItem.left;
					ptPos.y += tm.tmHeight + nRowSpace;
				}
				else if(ptPos.x > rcItem.right)
				{
					if(_T('\t') == strText[index])
					{
					}
					else if(0 == index - nStart)
					{
					}
					else 
					{
						nEncodeLenght = 0;
					}
					nStart = index + nEncodeLenght;
					ptPos.x = rcItem.left;
					ptPos.y += tm.tmHeight + nRowSpace;
				}
				index += nEncodeLenght;
			}
			//�ҳ�xֵ

			szText.cx = szText.cy = 0;
			ptPos.x = rcItem.left;
			for(index = 0; nLenght - nStart > index; index)
			{
				if(_T('\n') == strText[nStart + index] ||
					_T('\r') == strText[nStart + index] ||
					_T('\0') == strText[nStart + index] ||
					ptPos.x + POINTDIFF > ptMouse.x)
				{
					break;
				}

				nEncodeLenght = check_encode_lenght(&strText[nStart + index]);
				GetTextExtentPoint(m_hPaintDc, &strText[nStart + index], nEncodeLenght, &szText);

				if(_T('\t') == strText[nStart + index])
				{
					szText.cx = TABSPACE * nTabWidth;
				}

				if(ptPos.x + szText.cx > rcItem.right) break;

				ptPos.x += szText.cx;
				index += nEncodeLenght;
			}
			index +=nStart;
			ptMouse = ptPos;
		}
		else
		{
			do
			{
				bHasNext = GetStringLine(strText, nLenght, nStart, &nCount, NULL);
				if(false == bHasNext
					|| ptPos.y + tm.tmHeight + nRowSpace > ptMouse.y)
				{
					break;
				}

				ptPos.y += tm.tmHeight + nRowSpace;
				nStart += nCount;
			}while (TRUE);
			
			nTabCount = nTextWidth = 0;
			for(index = 0; nCount > index; index)
			{
				if(_T('\n') == strText[nStart + index] ||
					_T('\r') == strText[nStart + index] ||
					_T('\0') == strText[nStart + index] ||
					ptPos.x + POINTDIFF > ptMouse.x)
				{
					break;
				}

				nEncodeLenght = check_encode_lenght(&strText[nStart + index]);
				GetTextExtentPoint(m_hPaintDc, &strText[nStart + index], nEncodeLenght, &szText);

				if(_T('\t') == strText[nStart + index])
				{
					szText.cx = TABSPACE * nTabWidth;
				}
				ptPos.x += szText.cx;
				index += nEncodeLenght;
			}
			index += nStart;
			ptMouse = ptPos;
		}

finish:

		if(hFont) SelectObject(m_hPaintDc, hOldFont);
		ptMouse.x -= 1;
		return index;
	}

	POINT CGdiRenderEngine::GetTextPos(const RECT &rcItem, int nTextIndex, LPCTSTR strText,
			HFONT hFont, int nRowSpace, DWORD dwTextStyle)
	{
		HFONT hOldFont = NULL;
		POINT ptOutput;
		SIZE szText = {0};
		ptOutput.x = rcItem.left;
		ptOutput.y = rcItem.top;
		
		if(NULL == hFont) hFont = m_hWindowFont;
		if(hFont) hOldFont = (HFONT)SelectObject(m_hPaintDc, hFont);

		//������׼���ã���ʼ����

		BOOL bHasNext = FALSE;
		int nStart = 0, nCount = 0, nStrStrat = 0;
		
		bool bFreeLine = false;//����

		int nTabWidth = 0;
		int nTabCount = 0;
		int nIndex = 0;
		int nEncodeLenght = 0;
		int nHasTabSize = 0;
		TEXTMETRIC tm = {0};
		GetTextMetrics(m_hPaintDc, &tm);
		GetTextExtentPoint(m_hPaintDc, _T("\t"),  2, &szText);
		nHasTabSize = szText.cx ? 1 : 0;
		nTabWidth = szText.cx ? szText.cx : (tm.tmAveCharWidth + 1) / 2;

		if(FALSE == ((TOS_AUTOLINEFREE | TOS_MULTILLINE) & dwTextStyle))//����
		{
			GetTextExtentPoint(m_hPaintDc, strText, nTextIndex, &szText);
			if(TOS_CENTER & dwTextStyle)//ˮƽ����
			{
				ptOutput.x += MAX(0, ((rcItem.right - rcItem.left) - szText.cx) / 2);
			}
			ptOutput.x += szText.cx;
			ptOutput.y = rcItem.top + MAX(0, (rcItem.bottom - rcItem.top - tm.tmHeight) / 2);//��ֱ����
			goto finish;
		}

		//���������Ķ��Ƕ��д���
		if(TOS_AUTOLINEFREE & dwTextStyle)//�Զ�����
		{
			nStrStrat = 0;
			//��ѯ�߼�������������뱳����λ�ã���bOutput = true�н��д���
			//bOutput = true��������Ϻ���ִ�к�������
			for(nIndex = 0; nTextIndex > nIndex; nIndex)
			{
				nEncodeLenght = check_encode_lenght(&strText[nIndex]);
				GetTextExtentPoint(m_hPaintDc, &strText[nIndex], nEncodeLenght, &szText);
				
				if(_T('\t') == strText[nIndex])
				{
					szText.cx = nTabWidth * TABSPACE;
				}

				//ע���ж����ȼ�
				if(_T('\r') == strText[nIndex])
				{
					if(_T('\n') == strText[nIndex + 1])
					{
						++nIndex;
					}
					bFreeLine = true;
				}
				else if(_T('\n') == strText[nIndex])
				{
					if(_T('\r') == strText[nIndex + 1])
					{
						++nIndex;
					}
					bFreeLine = true;
				}
				else if(ptOutput.x + szText.cx > rcItem.right)
				{
					if(_T('\t') == strText[nIndex])
					{
					}
					else if(ptOutput.x == rcItem.left)
					{
					}
					else 
					{
						szText.cx = 0;
						nEncodeLenght = 0;
					}
					
					bFreeLine = true;
				}

				ptOutput.x += szText.cx;
				if(bFreeLine)
				{
					bFreeLine = false;
					ptOutput.x = rcItem.left;
					ptOutput.y += tm.tmHeight + nRowSpace;
				}

				nStrStrat = nIndex + nEncodeLenght;
				nIndex += nEncodeLenght;
			}
		}
		else//�����з����л���
		{
			do
			{
				bHasNext = GetStringLine(strText, nTextIndex, nStart, &nCount, &nTabCount);
				if(false == bHasNext)
				{
					break;
				}
				ptOutput.y += tm.tmHeight + nRowSpace;
				nStart += nCount;
			}while (TRUE);
			
			if(_T('\r') == strText[nTextIndex - 1] || _T('\n') == strText[nTextIndex - 1])
			{
				ptOutput.y += tm.tmHeight + nRowSpace;
			}
			else
			{
				//nEncodeLenght = check_pre_text_encode_lenght(&strText[nStart], nCount);
				GetTextExtentPoint(m_hPaintDc, &strText[nStart], nCount, &szText);
				szText.cx += nTabCount * (TABSPACE - nHasTabSize) * nTabWidth;
				ptOutput.x += szText.cx;
			}
		}

finish:
		//���ƽ���
		if(hFont) SelectObject(m_hPaintDc, hOldFont);
		return ptOutput;
	}

	bool CGdiRenderEngine::OnTextOut(const RECT &rcItem, POINT &ptOutput, 
		LPCTSTR strText, int nStrLenght, ARGBREF refTextColor, HFONT hFont, 
		int nRowSpace, DWORD dwTextStyle, ARGBREF refTextBkColor)
	{
		HFONT hOldFont = NULL;
		COLORREF refOldTextColor = NULL;
		int bkMode = 0;
		bkMode = SetBkMode(m_hMemDc,TRANSPARENT);
		if(NULL == hFont) hFont = m_hWindowFont;
		if(hFont) hOldFont = (HFONT)SelectObject(m_hMemDc, hFont);
		refOldTextColor = SetTextColor(m_hMemDc,GetRGBValue(refTextColor));
		//������׼���ã���ʼ����

		BOOL bHasNext = FALSE;
		int nStart = 0, nCount = 0, nStrStrat = 0;
		
		bool bFreeLine = false;//����
		bool bOutput = false;//��������¼�
		int nOutputCount = 0;//������ָ���
		RECT rcBackdrop = {0};

		SIZE szText = {0};
		int nTabWidth = 0;
		int nTabCount = 0;
		int nTextWidth = 0;
		int nIndex = 0;
		int nEncodeLenght = 0;
		TEXTMETRIC tm = {0};
		GetTextMetrics(m_hMemDc, &tm);

		GetTextExtentPoint(m_hMemDc, _T("\t"), 2, &szText);
		int nHasTabSize = szText.cx ? 1 : 0 ;
		nTabWidth = szText.cx ? szText.cx : (tm.tmAveCharWidth + 1) / 2;
		
		if(FALSE == ((TOS_AUTOLINEFREE | TOS_MULTILLINE) & dwTextStyle))//����
		{
			GetTextExtentPoint(m_hMemDc, strText, nStrLenght, &szText);

			/*
			if((TOS_CENTER & dwTextStyle))//ˮƽ����
			{
				//���е�λ���Ѿ�������ˣ����Ի��Ʋ��ؽ��ж�������
				//ptOutput.x += MAX(0, ((rcItem.right - rcItem.left) - szText.cx) / 2);
			}
			*/
			ptOutput.y = rcItem.top + MAX(0, (rcItem.bottom - rcItem.top - tm.tmHeight) / 2);//��ֱ����
			if(0x00 != GetAValue(refTextBkColor))
			{
				rcBackdrop.left = ptOutput.x;
				rcBackdrop.top = ptOutput.y;
				rcBackdrop.right = rcBackdrop.left + szText.cx;
				rcBackdrop.bottom = rcBackdrop.top + szText.cy;
				OnDrawColor(rcBackdrop, refTextBkColor);
			}

			TextOut(m_hMemDc, ptOutput.x, ptOutput.y, strText, nStrLenght);

			ptOutput.x += szText.cx;
			goto finish;
		}

		//���������Ķ��Ƕ��д���
		
		if(TOS_AUTOLINEFREE & dwTextStyle)//�Զ�����
		{
			nStrStrat = 0;
			nOutputCount = 0;
			bOutput = false;
			//��ѯ�߼�������������뱳����λ�ã���bOutput = true�н��д���
			//bOutput = true��������Ϻ���ִ�к�������
			for(nIndex = 0; nStrLenght > nIndex; nIndex)
			{
				nEncodeLenght = check_encode_lenght(&strText[nIndex]);
				GetTextExtentPoint(m_hMemDc, &strText[nIndex], nEncodeLenght, &szText);
				
				if(_T('\t') == strText[nIndex])
				{
					szText.cx = TABSPACE * nTabWidth;
				}
				nTextWidth += szText.cx;

				//ע���ж����ȼ�
				if(_T('\r') == strText[nIndex])
				{
					if(_T('\n') == strText[nIndex + 1])
					{
						++nIndex;
						nTextWidth += szText.cx;
					}
					bFreeLine = bOutput = true;
				}
				else if(_T('\n') == strText[nIndex])
				{
					if(_T('\r') == strText[nIndex + 1])
					{
						++nIndex;
						nTextWidth += szText.cx;
					}
					bFreeLine = bOutput = true;
				}
				else if(ptOutput.x + nTextWidth > rcItem.right)
				{
					if(_T('\t') == strText[nIndex])
					{
					}
					else if(ptOutput.x == rcItem.left && 0 == nOutputCount)
					{
						nOutputCount = nEncodeLenght;
					}
					else 
					{
						nTextWidth -= szText.cx;
						nEncodeLenght = 0;
					}
					
					bFreeLine = bOutput = true;
				}
				else if(_T('\t') == strText[nIndex])
				{
					bOutput = true;
				}
				else if(nStrLenght == nIndex + nEncodeLenght)
				{
					nOutputCount += nEncodeLenght;
					bOutput = true;
				}
				else
				{
					szText.cx = 0;
					nOutputCount += nEncodeLenght;
				}

				if(true == bOutput)
				{
					//�Ȼ��Ʊ���
					if(0x00 != GetAValue(refTextBkColor))
					{
						rcBackdrop.left = ptOutput.x;
						rcBackdrop.top = ptOutput.y;
						rcBackdrop.bottom = rcBackdrop.top + tm.tmHeight;
						rcBackdrop.right = MIN(rcBackdrop.left + nTextWidth, rcItem.right);
						OnDrawColor(rcBackdrop, refTextBkColor);
					}

					//�ٻ�������
					if(nOutputCount)
					{
						TextOut(m_hMemDc, ptOutput.x, ptOutput.y, &strText[nStrStrat], nOutputCount);
					}

					if(bFreeLine)
					{
						ptOutput.x = rcItem.left;
						ptOutput.y += tm.tmHeight + nRowSpace;
					}
					else
					{
						ptOutput.x += nTextWidth;
					}

					nStrStrat = nIndex + nEncodeLenght;
					nTextWidth = nOutputCount = 0;
					bFreeLine = bOutput = false;
				}
				nIndex += nEncodeLenght;
			}
		}
		else//�����з����л���
		{
			nStart = 0;
			do
			{
				bHasNext = GetStringLine(strText, nStrLenght, nStart, &nCount, &nTabCount);
			
				nStrStrat = nStart;
				nOutputCount = 0;
				bOutput = false;
				nEncodeLenght = nIndex = 0;
				//���������
				if(0x00 != GetAValue(refTextBkColor))
				{
					GetTextExtentPoint(m_hMemDc, &strText[nStart], nCount, &szText);

					szText.cx += nTabCount * (TABSPACE - nHasTabSize) * nTabWidth;

					rcBackdrop.left = ptOutput.x;
					rcBackdrop.top = ptOutput.y;
					rcBackdrop.right = rcBackdrop.left + szText.cx;
					rcBackdrop.bottom = rcBackdrop.top + szText.cy;
					OnDrawColor(rcBackdrop, refTextBkColor);
				}

				//���������
				for(nIndex = 0; nCount > nIndex; nIndex++)
				{
					//ע���ж����ȼ�
					if(_T('\t') == strText[nStart + nIndex] || 
						_T('\r') == strText[nStart + nIndex] ||
						_T('\n') == strText[nStart + nIndex])
					{
						bOutput = true;
					}
					else if (nCount == nIndex + 1)
					{
						++nOutputCount;
						bOutput = true;
					}
					else 
					{
						++nOutputCount;
					}

					if(true == bOutput)
					{
						bOutput = false;
						
						if(nOutputCount)
						{
							TextOut(m_hMemDc, ptOutput.x, ptOutput.y, &strText[nStrStrat],
								nOutputCount);
							GetTextExtentPoint(m_hMemDc, &strText[nStrStrat], 
								nOutputCount, &szText);
							ptOutput.x += szText.cx;
							nOutputCount = 0;
						}

						if(_T('\t') == strText[nStart + nIndex])
						{
							ptOutput.x += nTabWidth * 8;
							nStrStrat = nStart + nIndex + 1;
						}
						else if(_T('\r') == strText[nStart + nIndex] || 
							_T('\n') == strText[nStart + nIndex])
						{
							ptOutput.x = rcItem.left;
							ptOutput.y += tm.tmHeight + nRowSpace;
							break;
						}
						else if(nCount == nIndex + 1)
						{
							break;
						}
					}
				}
				nStart += nCount;
			}while(bHasNext);
		}

finish:
		//���ƽ���
		::SetTextColor(m_hMemDc ,refOldTextColor);
		if(hFont) SelectObject(m_hMemDc, hOldFont);
		SetBkMode(m_hMemDc, bkMode);
		return true;
	}

	//bool CGdiRenderEngine::OnDrawText(const RECT &rcItem, LPCTSTR strText, int nStrLenght, ARGBREF refTextColor, HFONT hFont, 
	//	int nRowSpace, DWORD dwTextStyle, bool bFocus, int nSelStart, int nSelLenght, ARGBREF refSelTextColor)
	//{
	//	return true;
	//}

	bool CGdiRenderEngine::SetCaret(const RECT &rcItem, const POINT ptCaret, HFONT hFont)
	{
		//�㷨���
		//����굱��һ�����Σ�Ȼ�����������rcItem���ཻ����
		//�ٽ��ཻ������Ϊ���������ݸ�CreateCaret��SetCaretPos
		TRACE(_T("CGdiRenderEngine::SetCaret"));
		TEXTMETRIC tm = {0};
		RECT rcCaret = {0};
		HFONT hOldFont = NULL;
		//DestroyCaret();
		if(ptCaret.x > rcItem.right || ptCaret.y > rcItem.bottom) return false;

		if(NULL == hFont) hFont = m_hWindowFont;
		if(hFont) hOldFont = (HFONT) SelectObject(m_hPaintDc, hFont);
		GetTextMetrics(m_hPaintDc, &tm);
		if(hFont) SelectObject(m_hPaintDc, hOldFont);

		rcCaret.left = ptCaret.x;
		rcCaret.top = ptCaret.y;
		rcCaret.right = rcCaret.left + MAX(1,tm.tmHeight / 10);
		rcCaret.bottom = rcCaret.top + tm.tmHeight;

		if(FALSE == IntersectRect(&rcCaret, &rcItem, &rcCaret))
		{
			HideCaret((HWND)m_hWnd);
			return false;
		}

		::CreateCaret((HWND)m_hWnd, NULL, rcCaret.right - rcCaret.left ,rcCaret.bottom - rcCaret.top);
		//SetCaretBlinkTime(800);
		::SetCaretPos(rcCaret.left , rcCaret.top);
		::ShowCaret((HWND)m_hWnd);
		return true;
	}

	bool CGdiRenderEngine::DrawOleObject(const RECT &rcItem, IViewObject * pViewObject)
	{
		HRESULT hr = -1;
		hr = pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, 
				m_hMemDc, (RECTL*)&rcItem, (RECTL*)&rcItem, NULL,NULL);
		return !FAILED(hr);
	}
}