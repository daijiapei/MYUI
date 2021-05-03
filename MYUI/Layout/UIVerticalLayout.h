
#ifndef __MYUI_VERTICAL_LAYOUT_H__
#define __MYUI_VERTICAL_LAYOUT_H__

#include "UIBaseLayout.h"

namespace MYUI
{
	//��ֱ���֣��ؼ������������У�����xyֵ��
	//����ӿؼ�û������cx�����ӿؼ��Ŀ��CVerticalLayoutUIһ��
	//����ӿؼ�û������cy�����ӿؼ��ĸ߰�CVerticalLayoutUI�߶ȵĶ���ռ�ƽ������

	class MYUI_API CVerticalLayoutUI : public CBaseLayoutUI
	{
	public:
		CVerticalLayoutUI();
		virtual ~CVerticalLayoutUI();
		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual bool SetItem(RECT rcItem, bool bMustUpdate) override;
		virtual SIZE GetContentSize();
	protected:
		bool m_bCentered;//�������ؼ�����
	};
}

#endif