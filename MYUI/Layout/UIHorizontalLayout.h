
#ifndef __MYUI_HORIZONTAIL_LAYOUT_H__
#define __MYUI_HORIZONTAIL_LAYOUT_H__

#include "UIBaseLayout.h"

namespace MYUI
{
	//ˮƽ���֣��ؼ������������У�����xyֵ��
	//����ӿؼ�û������cy�����ӿؼ��ĸ߸�CHorizontalLayoutUIһ��
	//����ӿؼ�û������cx�����ӿؼ��Ŀ�CHorizontalLayoutUI��ȵĶ���ռ�ƽ������
	class MYUI_API CHorizontalLayoutUI : public CBaseLayoutUI
	{
	public:
		CHorizontalLayoutUI();
		virtual ~CHorizontalLayoutUI();
		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual SIZE GetContentSize();
		virtual bool SetItem(RECT rcItem, bool bMustUpdate) override;
	protected:
		bool m_bCentered;//�������ؼ�����
	};
}


#endif