
#ifndef __MYUI_TAB_LAYOUT_H__
#define __MYUI_TAB_LAYOUT_H__

#include "UIBaseLayout.h"

namespace MYUI
{
	//��ǩ����ֻ��ʾm_pSelect�ؼ������������������
	//����xy,cx,cyֵ��ÿ���ӿؼ��Ĵ�СĬ�Ϻ�CTabLayoutUIһ��
	//��ǩ������Щ�������ظ�����ģ���������������Ч��
	//��������ԭ����Ϊ�˽����������ɵ�һ�������У������ܸ�������޸Ľӿ�
	class MYUI_API CTabLayoutUI : public CBaseLayoutUI
	{
	public:
		CTabLayoutUI();
		virtual ~CTabLayoutUI();
		const static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);


		virtual bool SetItem(RECT rcItem, bool bMustUpdate) override;
		virtual SIZE GetContentSize();

		virtual CControlUI * GetSelect();
		virtual int GetIndex();
		virtual bool SelectItem(int nIndex);
		virtual bool SelectItem(CControlUI * pControl);
		

		virtual bool AddAt(CControlUI* pControl, int iIndex);
		virtual bool RemoveAt(int iIndex);

		virtual bool OnPaint(const RECT& rcUpdate) override;
	protected:

	private:
		CControlUI * m_pSelect;
	};
}

#endif