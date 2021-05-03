
#include "UIGif.h"

namespace MYUI
{
#define IDT_GIT_TIMER   1
	CGifUI::CGifUI()
		: m_pGifImage(NULL)
		, m_pPropertyItem(NULL)
		, m_nFrameCount(0)
		, m_nFramePosition(0)
		, m_bIsAutoPlay(true)
		, m_bIsAutoSize(false)
		, m_gdiplusToken(0)
		, m_bIsPlaying(false)
	{
		GdiplusStartup( &m_gdiplusToken, &m_gdiplusStartupInput, NULL);
	}

	CGifUI::~CGifUI()
	{
		GdiplusShutdown(m_gdiplusToken);
	}

	const CMuiString CGifUI::g_strClassName = _T("GifUI");

	CMuiString CGifUI::GetClassName() const
	{
		return CGifUI::g_strClassName;
	}

	void CGifUI::OnAttach(HWND hNewWnd)
	{
		if(m_pGifImage) return;

		m_pGifImage = LoadGif(m_strBkImage);
		if (m_pGifImage == NULL) return;

		UINT nCount = 0;
		//返回层 每层可以包含很多帧
		nCount = m_pGifImage->GetFrameDimensionsCount();
		GUID* pDimensionIDs = new GUID[nCount];
		//获取层的GUID列表
		m_pGifImage->GetFrameDimensionsList(pDimensionIDs, nCount);
		//获取帧数
		m_nFrameCount	= m_pGifImage->GetFrameCount(&pDimensionIDs[0]);
		//获取属性结构体大小
		int nSize		= m_pGifImage->GetPropertyItemSize(PropertyTagFrameDelay);
		m_pPropertyItem = (Gdiplus::PropertyItem*)malloc(nSize);
		//获取属性信息
		m_pGifImage->GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
		delete pDimensionIDs;
		pDimensionIDs = NULL;
		
		if (IsAutoPlay()) 
		{
			PlayGif();
		}
	}

	void CGifUI::OnDetach(HWND hOldWnd)
	{
		this->KillTimer(IDT_GIT_TIMER);
	}

	void CGifUI::SetAutoPlay(bool bIsAuto) {
		m_bIsAutoPlay = bIsAuto;
	}

	bool CGifUI::IsAutoPlay() const {
		return m_bIsAutoPlay;
	}

	void CGifUI::SetAutoSize(bool bIsAuto) {
		m_bIsAutoSize = bIsAuto;
	}
	 
	bool CGifUI::IsAutoSize() const {
		return m_bIsAutoSize;
	}

	void CGifUI::PlayGif() 
	{
		if (m_bIsPlaying || NULL == m_pGifImage) return;

		//获取gif帧之间的时间间隔
		long lPause = ((long*)m_pPropertyItem->value)[m_nFramePosition] * 10;
		if (lPause == 0) 
		{
			lPause = 100;
		}
		this->SetTimer(IDT_GIT_TIMER, lPause);
		m_bIsPlaying = true;
	}
	 
	void CGifUI::PauseGif() 
	{
		if (!m_bIsPlaying ||  NULL == m_pGifImage) return;

		this->KillTimer(IDT_GIT_TIMER);
		this->Invalidate();
		m_bIsPlaying = false;
	}

	void CGifUI::StopGif() 
	{
		if (!m_bIsPlaying) 
		{
			return;
		}

		this->KillTimer(IDT_GIT_TIMER);
		m_nFrameCount = 0; 
		this->Invalidate();
		m_bIsPlaying = false;
	}

	void CGifUI::DeleteGif() {
		if (NULL == m_pGifImage) 
		{
			delete m_pGifImage;
			m_pGifImage = NULL;
		}

		if (m_pPropertyItem) 
		{
			free(m_pPropertyItem);
			m_pPropertyItem = NULL;
		}

		m_nFrameCount = 0;
		m_nFramePosition = 0;
	}

	Gdiplus::Image*	CGifUI::LoadGif(LPCTSTR strGifPath) 
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;
		Gdiplus::Image * pImage = NULL;
		IStream* pStream = NULL; 

		CSkinManager::LoadFile(m_pShareInfo->strSkinFolder, strGifPath, &pData, &dwSize);

		if(NULL == pData || 0 == dwSize) return NULL;

		HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, dwSize);
		BYTE* pMem = (BYTE*)GlobalLock(hMem);

		memcpy(pMem, pData, dwSize); 
		
		CreateStreamOnHGlobal(hMem, TRUE, &pStream); 
		pImage = Gdiplus::Image::FromStream(pStream);
		if (!pImage || Gdiplus::Ok != pImage->GetLastStatus() ) 
		{
			pStream->Release(); 
			GlobalUnlock(hMem);
			MUITRACE(_T("CGifUI::LoadGif GetLastError = %d"), GetLastError());
		}

		delete pData;
		return pImage;
	} 

	void CGifUI::SetBkImage(LPCTSTR strImage)
	{
		//StopGif();
		//DeleteGif();
		//Invalidate();
		__super::SetBkImage(strImage);
	}

	void CGifUI::SetVisible(bool bVisible) 
	{
		CControlUI::SetVisible(bVisible);
		if (true == bVisible) 
		{
			if(m_bIsAutoPlay) PlayGif();
		} 
		else 
		{
			StopGif();
		}
	}

	bool CGifUI::OnPaint(const RECT& rcUpdate)
	{
		RECT rcClient = m_rcClient;
		
		if(FALSE == IsContainRect(rcClient, rcUpdate)) return false;
		if (NULL == m_pGifImage) return true;

		if(m_bIsAutoSize)
		{
			rcClient.right = rcClient.left + m_pGifImage->GetWidth();
			rcClient.bottom = rcClient.top + m_pGifImage->GetHeight();
		}

		if(0 == (MGIFS_FLAG & m_dwStyle))
		{

		}

		HDC hMemDc = m_pShareInfo->pRenderEngine->GetMemDC();
		POINT ptDraw = m_pShareInfo->pRenderEngine->GetDrawPoint();
		GUID pageGuid = Gdiplus::FrameDimensionTime;
		Gdiplus::Graphics graphics(hMemDc);

		//选择当前活动帧
		m_pGifImage->SelectActiveFrame(&pageGuid, m_nFramePosition);

		//绘制当前帧
		OffsetRect(&rcClient, ptDraw.x, ptDraw.y);
		graphics.DrawImage(m_pGifImage, rcClient.left, rcClient.top, 
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top );
		

		m_pShareInfo->pRenderEngine->ReleaseMemDC(hMemDc);
		return true;
	}

	LRESULT CGifUI::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_TIMER:
			{
				//this->KillTimer(IDT_GIT_TIMER);
				
				m_nFramePosition = (++m_nFramePosition) % m_nFrameCount;

				long lPause = ((long*)m_pPropertyItem->value)[m_nFramePosition] * 10;
				if (lPause == 0) 
				{
					lPause = 100;
				}
				this->Invalidate();
				//this->SetTimer(IDT_GIT_TIMER, lPause);
				return 0;
			}break;
		default:
			break;
		}

		return __super::WndProc(message, wParam, lParam);
	}


}