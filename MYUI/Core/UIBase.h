
#ifndef __MYUI_UI_BASE_ATTRIBUTE_H__
#define __MYUI_UI_BASE_ATTRIBUTE_H__

#include "..//Utils//Utils.h"

namespace MYUI
{
	//虚拟窗口的界面
	class MYUI_API CItemViewInfo
	{
	public:
		CItemViewInfo();
		virtual ~CItemViewInfo();

	public:
		virtual void SetStyle(DWORD dwStyle);
		virtual DWORD GetStyle();
		//设置图形
		virtual void SetBkColor(ARGBREF argb);
		virtual ARGBREF GetBkColor();

		virtual void SetTextFont(int FontId);
		virtual int GetTextFont();

		virtual void SetTextColor(ARGBREF argb);
		virtual ARGBREF GetTextColor();

		virtual void SetText(LPCTSTR pstrText);
		virtual CMuiString GetText() const;

		virtual void SetBorderSize(int nSize);
		virtual int GetBorderSize();

		virtual void SetBorderColor(ARGBREF argb);
		virtual ARGBREF GetBorderColor();

		virtual void SetBorderPen(int nSize, ARGBREF argb);

		virtual void SetBorderRound(SIZE &size);
		virtual SIZE &GetBorderRound();

		virtual void SetBkImage(LPCTSTR strImage);
		virtual LPCTSTR GetBkImage();
		virtual void SetDisabledImage(LPCTSTR strImage);
		virtual LPCTSTR GetDisabledImage();

	protected:
		//更改通知
		virtual void OnViewChange(LPVOID pElement) = 0;
		//virtual void OnFontChange(LPVOID pElement) = 0;
	protected:

//MCS_CONTROLUI主要用于布局中
//#define MCS_CONTROLUI 0x80000000//这是一个控件
		DWORD m_dwStyle;
		DWORD m_dwStyleEx;
		ARGBREF m_refTextColor;
		int m_nFontId;
		HFONT m_hFont;
		CMuiString m_strText;

		ARGBREF m_refBkColor;

		ARGBREF m_refBorderColor;
		SIZE   m_szBorderRound;
		int m_nBorderSize;

		//图片的路径+图片参数
		CMuiString m_strBkImage;//背景
		CMuiString m_strDisabledImage;//禁用状态
	};

	enum EnumFloatType//判断控件坐标中，哪个值是有效的
	{
		FloatNon = 0x00, //无效
		FloatPointX = 0x01,
		FloatPointY = 0x02,
		//注意所有的type判断中，都是认为xy坐标一起使用的，xy不要单独使用
		FloatPoint = 0x03 , //x,y有效,
		FloatSizeCx = 0x04,
		FloatSizeCy = 0x08,
		FloatSize = 0x0C, //cx,cy有效
		FloatAll = 0x0F //都有效
	};

	class MYUI_API CItemPosition//虚拟窗口的位置
	{
	public:
		//修改CItemPosition的值，不会导致页面更新
		//所以记得调用父类的update函数
		CItemPosition();
		virtual ~CItemPosition();
		
	public:
		
		//未偏移区域
		virtual const RECT &GetRawRect() const;
		virtual const RECT &GetClientRect() const;

		//已偏移区域
		virtual RECT GetItemClient();
		//virtual const RECT &GetItemRaw() const;

		//return true = 区域发生了变更， false = 区域没有发生变更
		//bMustUpdate true = 不管区域是否相同，都一定要发生区域变更设置
		//bMustUpdate false = 如果区域相同，则不会发生变更设置
		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual const RECT &GetItem() const;
		virtual SIZE GetValidSize();//取得真正有效的尺寸
		//内部设置
		virtual EnumFloatType GetFloatType();
		virtual void SetFloatType(EnumFloatType type);
		virtual void AddFloatType(EnumFloatType type);
		virtual void RemoveFloatType(EnumFloatType type);
		
		//rect

		//SetPos跟SetItem的区别比较容易混淆，这里说明一下
		//SetPos的值，是控件设置位置时的参考值，SetItem才是设置控件真正的位置
		//在SetItem时，布局会提取GetPos的值作参考计算
		//比如我SetPos的值 x=10,y=10,width=100,height=100, 但是在垂直布局中，
		//x,y的值是无效的，所以垂直布局在SetItem时，只会提取width,height这两个值进行计算，忽略了x,y值
		//另外SetWight,SetPoint等函数性质跟SetPos是一致的：
		void SetPos(RECT &rect);
		RECT GetPos();

		//size
		const SIZE & GetSize() const;
		void SetSize(SIZE &size);
		int GetHeight();
		void SetHeight(int nHeight);
		int GetWidth();
		void SetWidth(int nWidth);

		//point
		void GetPoint(POINT &pt);
		void SetPoint(POINT &pt);
		int GetPointX();
		void SetPointX(int x);
		int GetPointY();
		void SetPointY(int y);

		//窗口大小
		int GetMinCx();
		void SetMinCx(int cx);
		int GetMinCy();
		void SetMinCy(int cy);
		int GetMaxCx();
		void SetMaxCx(int cx);
		int GetMaxCy();
		void SetMaxCy(int cy);
		void GetMinSize(SIZE &size);
		void SetMinSize(SIZE &size);
		void GetMaxSize(SIZE &size);
		void SetMaxSize(SIZE &size);
		

		void SetMargin(RECT &rcMargin);
		const RECT &GetMargin() const;

	protected:
		virtual void OnPosChange(LPVOID pElement) = 0;

	protected:

		RECT m_rcItem;//控件相对位置，相对于父控件
		//外边距可以认为是在m_rcItem缩进，形成m_rcRawItem，
		//也可以认为是在m_rcRawItem扩展，形成m_rcItem
		RECT m_rcMargin;//外边距
		//下面的区域，都是相对于m_rcItem 的
		RECT m_rcRawItem;//原始Item，m_rcItem减去外边距
		RECT m_rcClient;//客户区，m_rcRawItem减去滚动条
		
		POINT m_ItemPoint;//xy
		SIZE m_ItemSize;//宽与高
		SIZE m_SizeMin;
		SIZE m_SizeMax;
		//m_ItemPoint 与 m_ItemSize中哪个值有效
		EnumFloatType m_FloatType;
	};

}

#endif