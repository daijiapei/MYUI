
#include "UIBase.h"

namespace MYUI
{
	/*****************************************************************************************
	//CItemView
	*****************************************************************************************/
	CItemViewInfo::CItemViewInfo()
		:m_nFontId(-1)
		,m_hFont(NULL)
		,m_refTextColor(ARGB(255,0,0,0))
		,m_refBkColor(NULL)
		,m_refBorderColor(NULL)
		,m_nBorderSize(0)
		,m_dwStyle(NULL)
	{
		m_szBorderRound.cx = m_szBorderRound.cy = 0;
	}

	CItemViewInfo::~CItemViewInfo()
	{
		MUITRACE(_T("CItemViewInfo::~CItemViewInfo"));
	}

	void CItemViewInfo::SetStyle(DWORD dwStyle)
	{
		m_dwStyle = dwStyle;
	}

	DWORD CItemViewInfo::GetStyle()
	{
		return m_dwStyle;
	}

	void CItemViewInfo::SetBkColor(ARGBREF argb)
	{
		m_refBkColor = argb;
	}

	ARGBREF CItemViewInfo::GetBkColor()
	{
		return m_refBkColor ;
	}

	void CItemViewInfo::SetTextFont(int FontId)
	{
		m_nFontId= FontId;
	}

	int CItemViewInfo::GetTextFont()
	{
		return m_nFontId;
	}

	CMuiString CItemViewInfo::GetText() const
	{
		return m_strText;
	}

	void CItemViewInfo::SetText(LPCTSTR pstrText)
	{
		m_strText = pstrText;
		//OnViewChange((VOID *)m_strText.GetData());
	}

	void CItemViewInfo::SetTextColor(ARGBREF argb)
	{
		m_refTextColor = argb;
	}

	ARGBREF CItemViewInfo::GetTextColor()
	{
		return m_refTextColor ;
	}

	void CItemViewInfo::SetBorderColor(ARGBREF argb)
	{
		m_refBorderColor = argb;
	}

	ARGBREF CItemViewInfo::GetBorderColor()
	{
		return m_refBkColor;
	}

	void CItemViewInfo::SetBorderSize(int nSize)
	{
		m_nBorderSize = nSize;
	}

	int CItemViewInfo::GetBorderSize()
	{
		return m_nBorderSize;
	}

	void CItemViewInfo::SetBorderPen(int nSize, ARGBREF argb)
	{
		SetBorderSize(nSize);
		SetBorderColor(argb);
	}

	void CItemViewInfo::SetBorderRound(SIZE &size)
	{
		m_szBorderRound = size;
	}

	SIZE & CItemViewInfo::GetBorderRound()
	{
		return m_szBorderRound;
	}

	void CItemViewInfo::SetBkImage(LPCTSTR strImage)
	{
		m_strBkImage = strImage;
	}

	LPCTSTR CItemViewInfo::GetBkImage()
	{
		return m_strBkImage;
	}

	void CItemViewInfo::SetDisabledImage(LPCTSTR strImage)
	{
		m_strDisabledImage = strImage;
	}

	LPCTSTR CItemViewInfo::GetDisabledImage()
	{
		return m_strDisabledImage;
	}

	/*****************************************************************************************
	//CItemPosition
	*****************************************************************************************/
	CItemPosition::CItemPosition()
	{
		m_FloatType = EnumFloatType::FloatNon;
		m_ItemPoint.x = m_ItemPoint.y = 0;
		m_ItemSize.cx = m_ItemSize.cy = 0;
		m_SizeMin.cx = m_SizeMin.cy = 0;
		m_SizeMax.cx = m_SizeMax.cy = 99999;

		EmptyRect(m_rcItem);
		EmptyRect(m_rcRawItem);
		EmptyRect(m_rcClient);
		EmptyRect(m_rcMargin);
	}

	CItemPosition::~CItemPosition()
	{
		MUITRACE(_T("CItemPosition::~CItemPosition"));
	};

	const RECT &CItemPosition::GetRawRect() const
	{
		return m_rcRawItem;
	}

	const RECT &CItemPosition::GetClientRect() const
	{
		return m_rcClient;
	}

	RECT CItemPosition::GetItemClient()
	{
		RECT rcClient = m_rcClient;
		OffsetRect(&rcClient, m_rcItem.left, m_rcItem.top);
		return rcClient;
	}

	//下面的一系列函数，设置的x,y,cx,cy，都只是提供给布局参考，
	//布局通过参考后，计算出rcItem，rcItem才是控件真正的位置
	void CItemPosition::SetPos(RECT &rect)
	{
		//注意，SetPos设置的值只是提供给布局参考的
		m_ItemPoint.x = rect.left;
		m_ItemPoint.y = rect.top;
		m_ItemSize.cx = rect.right - rect.left;
		m_ItemSize.cy = rect.bottom - rect.top;
		AddFloatType(EnumFloatType::FloatAll);
	}

	RECT CItemPosition::GetPos()
	{
		RECT rect;
		//不校验xy. cx.cy是否有效
		rect.left = m_ItemPoint.x;
		rect.top = m_ItemPoint.y;
		rect.right = m_ItemPoint.x + MAX(m_SizeMin.cx, MIN(m_ItemSize.cx, m_SizeMax.cx));
		rect.bottom = m_ItemPoint.y + MAX(m_SizeMin.cy, MIN(m_ItemSize.cy, m_SizeMax.cy));
		return rect;
	}

	const SIZE &CItemPosition::GetSize() const
	{
		return m_ItemSize;
	}

	void CItemPosition::SetSize(SIZE &size)
	{
		m_ItemSize = size;
		AddFloatType(EnumFloatType::FloatSize);
	}

	int CItemPosition::GetHeight()
	{
		return m_ItemSize.cy;
	}

	void CItemPosition::SetHeight(int nHeight)
	{
		m_ItemSize.cy = nHeight;
		AddFloatType(EnumFloatType::FloatSizeCy);
	}

	int CItemPosition::GetWidth()
	{
		return m_ItemSize.cx;
	}

	void CItemPosition::SetWidth(int nWidth)
	{
		m_ItemSize.cx = nWidth;
		AddFloatType(EnumFloatType::FloatSizeCx);
	}

	void CItemPosition::GetPoint(POINT &pt)
	{
		pt = m_ItemPoint;
	}

	void CItemPosition::SetPoint(POINT &pt)
	{
		m_ItemPoint = pt;
		AddFloatType(EnumFloatType::FloatPoint);
	}

	void CItemPosition::SetPointX(int x)
	{
		m_ItemPoint.x = x;
		AddFloatType(EnumFloatType::FloatPointX);
	}

	int CItemPosition::GetPointX()
	{
		return m_ItemPoint.x;
	}

	void CItemPosition::SetPointY(int y)
	{
		m_ItemPoint.y;
		AddFloatType(EnumFloatType::FloatPointY);
	}

	int CItemPosition::GetPointY()
	{
		return m_ItemPoint.y;
	}

	EnumFloatType CItemPosition::GetFloatType()
	{
		return m_FloatType;
	}

	void CItemPosition::SetFloatType(EnumFloatType type)
	{
		m_FloatType = type;
	}

	void CItemPosition::AddFloatType(EnumFloatType type)
	{
		SetFloatType((EnumFloatType)(m_FloatType | type));
	}

	void CItemPosition::RemoveFloatType(EnumFloatType type)
	{
		m_FloatType = (EnumFloatType)(m_FloatType & (~type));
	}

	int CItemPosition::GetMinCx()
	{
		return m_SizeMin.cx;
	}

	void CItemPosition::SetMinCx(int cx)
	{
		m_SizeMin.cx = cx;
	}

	int CItemPosition::GetMinCy()
	{
		return m_SizeMin.cy;
	}

	void CItemPosition::SetMinCy(int cy)
	{
		m_SizeMin.cy = cy;
	}

	int CItemPosition::GetMaxCx()
	{
		return m_SizeMax.cx;
	}

	void CItemPosition::SetMaxCx(int cx)
	{
		m_SizeMax.cx = cx;
	}

	int CItemPosition::GetMaxCy()
	{
		return m_SizeMax.cy;
	}

	void CItemPosition::SetMaxCy(int cy)
	{
		m_SizeMax.cy = cy;
	}

	void CItemPosition::GetMinSize(SIZE &size)
	{
		size = m_SizeMin;
	}

	void CItemPosition::SetMinSize(SIZE &size)
	{
		m_SizeMin = size;
	}

	void CItemPosition::GetMaxSize(SIZE &size)
	{
		size = m_SizeMax;
	}

	void CItemPosition::SetMaxSize(SIZE &size)
	{
		m_SizeMax = size;
	}

	void CItemPosition::SetMargin(RECT &rcMargin)
	{
		m_rcMargin = rcMargin;
	}

	const RECT &CItemPosition::GetMargin() const
	{
		return m_rcMargin;
	}

	//控件占据的尺寸
	SIZE CItemPosition::GetValidSize()
	{
		//内边距是计算在内的
		SIZE size;
		size.cx = EnumFloatType::FloatSizeCx & m_FloatType? m_ItemSize.cx : 0;
		size.cy = EnumFloatType::FloatSizeCy & m_FloatType? m_ItemSize.cy : 0;
		size.cx = MAX(m_SizeMin.cx, MIN(size.cx, m_SizeMax.cx));
		size.cy = MAX(m_SizeMin.cy, MIN(size.cy, m_SizeMax.cy));
		size.cx += m_rcMargin.left + m_rcMargin.right;
		size.cy += m_rcMargin.top + m_rcMargin.bottom;
		return size;
	}

	const RECT &CItemPosition::GetItem() const
	{
		return m_rcItem;
	}

	bool CItemPosition::SetItem(RECT rcItem, bool bMustUpdate)
	{
		//发生变更返回TRUE， 没有发生变更返回FALSE
		if(false == bMustUpdate && TRUE == IsSameRect(m_rcItem, rcItem) ) return false;
		
		//m_rcMargin外边距区，外边距区只占据控件外部空间，不会产生任何有效操作和绘制
		//传递过来的参数已经处理好外边距区
		m_rcItem = rcItem;
		//这个函数很简单，但是却要理解一个控件十分重要的概念，那就是控件的区域组合
		//一个控件就是一个RECT区域，我们将命名为RECT m_rcItem,这个区域包含了：
		//有效区(rawitem,控件最原始的区域)， rawitem是未偏移的m_rcItem
		//item = 有效区。有效区包括：滚动条区，客户区，边框
		//边框，控件最外层的线条，这个区域控件并不会额外为边框腾出，如果希望你的控件图片
		//不被边框覆盖，可以设置图片的dest(控件内部的绝对位置)属性，更好的方式在外层添加一个布局，
		//布局添加缩进属性(内边距)，而布局缩进的区域可以绘制边框
		//滚动条区 = 显示滚动条的区域
		//客户区 = 有效区减去滚动条区，剩下的区域就是客户区
		//客户区包括了展示区(show)和内容区(content)，展示区跟客户区一样大
		//假设我在100X100的客户区显示一张200X200的图片，那么100X100的区域就是展示区，
		//200X200的区域就是内容区，超出了展示区的内容将无法看见，所以就出现了滚动条概念
		//在上层的容器中，还有一个内边距区
		//子控件都是显示在父控件的客户区的，但是我不想子控件靠近客户区的边缘，
		//这个时候可以在客户区设置一个内缩进（内边距），使子控件偏离客户区的边缘
		//注意，内缩进会扩展（增大）内容区（计算跟他的意思是相反的，内容区增大了，看上去就产生了内缩进效果）
		//内边距与外边距的差别：
		//外边距在有效区域外，只占据位置，不会产生有效的操作，比如鼠标点击和绘制，它是不响应的
		//内边距在客户区内，不仅占据位置，还能响应操作，比如鼠标点击和绘制

		m_rcRawItem = rcItem;
		//将m_rcRawItem偏移到相对于m_rcItem的位置
		OffsetRect(&m_rcRawItem, - m_rcItem.left, - m_rcItem.top);

		//原始控件中，内容区等于客户区，客户区等于原始区域，
		//但是继承原始控件的子类可能会修改客户区
		//比如有滚动条的控件中，客户区 = 原始区域（有效区） - 滚动条空间
		//内容区 = 客户区 + 滚动条偏移尺度
		m_rcClient = m_rcRawItem;

		return true;

	}

}