
#ifndef __MYUI_TILE_LAYOUT_H__
#define __MYUI_TILE_LAYOUT_H__

#include "UIBaseLayout.h"

namespace MYUI
{
	//��ש���֣����ָ�ÿ��Itemָ�����ԵĿ�͸ߣ���Ӧ�õ��ӿؼ���
	//Ȼ�����שһ�������к��е���ʽ�������г���
	enum TileAdaptType
	{
		//���䷽ʽ���ܻ��ʹ�ã�ֻ��ѡ��һ
		WidthAdapt = 1,//ÿ����ʾ���ٸ�����CTileLayoutUI�Ŀ������������ٻ���ʾһ��
		HeightAdap, //ÿ����ʾ���ٸ�����CTileLayoutUI�ĸ������������ٻ���ʾһ��
		
		ColumnAdapt,//ָ��CTileLayoutUI�ж����У�������Ӧ��������ʾһ����
		RowAdapt, //ָ��CTileLayoutUI�ж����У�������Ӧ��������ʾһ����
		AutoAdapt = WidthAdapt,//Ĭ�Ϸ��
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

		//HeightAdap �� HeightAdap ����nCount����
		bool SetAdaptType(TileAdaptType type);
		TileAdaptType GetAdaptType();
		bool SetAdaptColumn(int nCount);
		int GetAdaptColumn() const;

		//����������ӿؼ����оֻ࣬��ҪΪÿ���ӿؼ����һ�����༴��
		//����Ϊÿ���ӿؼ������ʵ��̫�鷳�ˣ������������SetAdaptPadding����
		//Ϊÿ��Adapt�ṩ��ͬ���ڼ�ࣨ����ӿؼ������ࣩ
		void SetAdaptPadding(RECT &rect);
		const RECT &GetAdaptPadding() const;
		void SetAdaptItemSize(SIZE &size);
		const SIZE &GetAdaptItemSize() const;
	protected:
		
	private:
		TileAdaptType m_dwAdaptType;
		int m_nAdaptCount;
		SIZE m_siAdaptItem;//ÿ���ӿؼ��ĳߴ�
		RECT m_rcAdaptPadding;//item������
	};
}

#endif