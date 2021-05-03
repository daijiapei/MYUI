
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

	//�����һϵ�к��������õ�x,y,cx,cy����ֻ���ṩ�����ֲο���
	//����ͨ���ο��󣬼����rcItem��rcItem���ǿؼ�������λ��
	void CItemPosition::SetPos(RECT &rect)
	{
		//ע�⣬SetPos���õ�ֵֻ���ṩ�����ֲο���
		m_ItemPoint.x = rect.left;
		m_ItemPoint.y = rect.top;
		m_ItemSize.cx = rect.right - rect.left;
		m_ItemSize.cy = rect.bottom - rect.top;
		AddFloatType(EnumFloatType::FloatAll);
	}

	RECT CItemPosition::GetPos()
	{
		RECT rect;
		//��У��xy. cx.cy�Ƿ���Ч
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

	//�ؼ�ռ�ݵĳߴ�
	SIZE CItemPosition::GetValidSize()
	{
		//�ڱ߾��Ǽ������ڵ�
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
		//�����������TRUE�� û�з����������FALSE
		if(false == bMustUpdate && TRUE == IsSameRect(m_rcItem, rcItem) ) return false;
		
		//m_rcMargin��߾�������߾���ֻռ�ݿؼ��ⲿ�ռ䣬��������κ���Ч�����ͻ���
		//���ݹ����Ĳ����Ѿ��������߾���
		m_rcItem = rcItem;
		//��������ܼ򵥣�����ȴҪ���һ���ؼ�ʮ����Ҫ�ĸ���Ǿ��ǿؼ����������
		//һ���ؼ�����һ��RECT�������ǽ�����ΪRECT m_rcItem,�����������ˣ�
		//��Ч��(rawitem,�ؼ���ԭʼ������)�� rawitem��δƫ�Ƶ�m_rcItem
		//item = ��Ч������Ч�������������������ͻ������߿�
		//�߿򣬿ؼ��������������������ؼ����������Ϊ�߿��ڳ������ϣ����Ŀؼ�ͼƬ
		//�����߿򸲸ǣ���������ͼƬ��dest(�ؼ��ڲ��ľ���λ��)���ԣ����õķ�ʽ��������һ�����֣�
		//���������������(�ڱ߾�)��������������������Ի��Ʊ߿�
		//�������� = ��ʾ������������
		//�ͻ��� = ��Ч����ȥ����������ʣ�µ�������ǿͻ���
		//�ͻ���������չʾ��(show)��������(content)��չʾ�����ͻ���һ����
		//��������100X100�Ŀͻ�����ʾһ��200X200��ͼƬ����ô100X100���������չʾ����
		//200X200�����������������������չʾ�������ݽ��޷����������Ծͳ����˹���������
		//���ϲ�������У�����һ���ڱ߾���
		//�ӿؼ�������ʾ�ڸ��ؼ��Ŀͻ����ģ������Ҳ����ӿؼ������ͻ����ı�Ե��
		//���ʱ������ڿͻ�������һ�����������ڱ߾ࣩ��ʹ�ӿؼ�ƫ��ͻ����ı�Ե
		//ע�⣬����������չ�������������������������˼���෴�ģ������������ˣ�����ȥ�Ͳ�����������Ч����
		//�ڱ߾�����߾�Ĳ��
		//��߾�����Ч�����⣬ֻռ��λ�ã����������Ч�Ĳ���������������ͻ��ƣ����ǲ���Ӧ��
		//�ڱ߾��ڿͻ����ڣ�����ռ��λ�ã�������Ӧ����������������ͻ���

		m_rcRawItem = rcItem;
		//��m_rcRawItemƫ�Ƶ������m_rcItem��λ��
		OffsetRect(&m_rcRawItem, - m_rcItem.left, - m_rcItem.top);

		//ԭʼ�ؼ��У����������ڿͻ������ͻ�������ԭʼ����
		//���Ǽ̳�ԭʼ�ؼ���������ܻ��޸Ŀͻ���
		//�����й������Ŀؼ��У��ͻ��� = ԭʼ������Ч���� - �������ռ�
		//������ = �ͻ��� + ������ƫ�Ƴ߶�
		m_rcClient = m_rcRawItem;

		return true;

	}

}