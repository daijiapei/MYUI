
#ifndef __MYUI_TILE_LAYOUT_H__
#define __MYUI_TILE_LAYOUT_H__

#include "UIBaseLayout.h"

namespace MYUI
{
	//地砖布局，布局给每个Item指定绝对的宽和高，并应用到子控件中
	//然后像地砖一样，按行和列的形式整齐排列出来
	enum TileAdaptType
	{
		//适配方式不能混合使用，只能选其一
		WidthAdapt = 1,//每行显示多少个列由CTileLayoutUI的宽来决定，至少会显示一列
		HeightAdap, //每列显示多少个行由CTileLayoutUI的高来决定，至少会显示一行
		
		ColumnAdapt,//指定CTileLayoutUI有多少列，行自适应，至少显示一个列
		RowAdapt, //指定CTileLayoutUI有多少行，列自适应，至少显示一个行
		AutoAdapt = WidthAdapt,//默认风格
	};

	class MYUI_API CTileLayoutUI : public CBaseLayoutUI
	{
	public:
		CTileLayoutUI();
		virtual ~CTileLayoutUI();
		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);

		virtual bool SetItem(RECT rcItem, bool bMustUpdate) override;
		virtual SIZE GetContentSize();

		//HeightAdap 和 HeightAdap 忽略nCount参数
		bool SetAdaptType(TileAdaptType type);
		TileAdaptType GetAdaptType();
		bool SetAdaptColumn(int nCount);
		int GetAdaptColumn() const;

		//如果想设置子控件的行距，只需要为每个子控件添加一个外间距即可
		//但是为每个子控件都添加实在太麻烦了，所以这个增加SetAdaptPadding函数
		//为每个Adapt提供相同的内间距（相对子控件的外间距）
		void SetAdaptPadding(RECT &rect);
		const RECT &GetAdaptPadding() const;
		void SetAdaptItemSize(SIZE &size);
		const SIZE &GetAdaptItemSize() const;
	protected:
		
	private:
		TileAdaptType m_dwAdaptType;
		int m_nAdaptCount;
		SIZE m_siAdaptItem;//每个子控件的尺寸
		RECT m_rcAdaptPadding;//item内缩进
	};
}

#endif