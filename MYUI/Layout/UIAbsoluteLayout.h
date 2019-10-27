
#ifndef _MYUI_ABSOLUTE_LAYOUT_H_
#define _MYUI_ABSOLUTE_LAYOUT_H_

#include "UIBaseLayout.h"

namespace MYUI
{
	//���Բ��֣����xy��û�����ã�������Ϊ���ֵ�xyֵ��������������
	//���cx,cy��û�����ã�������Ϊ���ֵ�cx.cy��ȥ�ӿؼ���x.y
	//ֵ��ע���һ�������ǣ�����ӿؼ������ص�
	//OnPaint��FindControlByPoint����û������ģ�����GetDisplayRectȴ����ȡ����ȷ����ʾ����
	//��ΪGetDisplayRectֻ���Ǹ��ؼ��Ƿ��ڵ�������û�п����ֵܿؼ��Ƿ��ڵ�����
	class MYUI_API CAbsoluteLayoutUI : public CBaseLayoutUI
	{
	public:
		CAbsoluteLayoutUI();
		virtual ~CAbsoluteLayoutUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;

		virtual bool SetItem(RECT rcItem, bool bMustUpdate);
		virtual SIZE GetContentSize();

	protected:
		
	};
}

#endif