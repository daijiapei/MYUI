
#ifndef _MYUI_OPTION_CONTROL_H_
#define _MYUI_OPTION_CONTROL_H_

#include "UIButton.h"

namespace MYUI
{
	//����һ��COptionUI�����ã�����˵�����Option������(group)����Ļ�����ô����
	//�൱��һ��CheckBox����ѡ�򣩣���������˷���Option���൱��һ��radioBox����ѡ��
	//��ѡ��Ĺ�Ч�ǣ��������ʮ����ѡ����ôʮ�����������ѡ��N��Option
	//��ѡ��Ĺ�Ч�ǣ��������ʮ����ѡ����ôʮ����ֻ��ѡ������һ����
	//��ѡ�����ʱ��һ����յ�һ��SelectItem֪ͨ,(BOOL)wParam��ʾ�Ƿ�ѡ��
	//��ѡ�����ʱ��һ����յ�����SelectItem֪ͨ��һ���Ǳ�ѡ�еĿؼ�֪ͨ
	//һ����ȡ��ѡ�еĿؼ�֪ͨ��ͬ����ͨ��ͬ��(BOOL)wParam�ж��Ƿ�ѡ��
	class MYUI_API COptionUI : public CLableUI
	{
	public:
		COptionUI();
		virtual ~COptionUI();
		static CMuiString g_strClassName;
		virtual CMuiString GetClassName() const;
		virtual void SetAttribute(LPCTSTR strItem, LPCTSTR strValue);
		
	public:
		void SetGroupName(LPCTSTR strGroup);
		LPCTSTR GetGroupName() const;

		void SetForeUnknowImage(LPCTSTR strImage);
		LPCTSTR GetForeUnknowImage() const;
		void SetForeNormalImage(LPCTSTR strImage);
		LPCTSTR GetForeNormalImage() const;
		void SetForeCheckImage(LPCTSTR strImage);
		LPCTSTR GetForeCheckImage() const;

		bool SetCheck(bool bCheck);
		bool GetCheck();
		
		virtual void SetState(DWORD dwState);
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		virtual void PaintStatusImage( const RECT& rcItem, const RECT& rcPaint);
		virtual void OnAttach(HWND hNewWnd);//���ؼ����ӵ��µĴ���ʱ�������һ�θú���
		virtual void OnDetach(HWND hOldWnd);//���ؼ��뿪һ������ʱ�������һ�θú��� 

		bool AttachToGroup(bool bAttach);
	
	protected:
		CMuiString m_strGroupName;
		//Fore��ǰ��ͼƬ���Ȼ���Lable��״̬ͼƬ���ٻ���ǰ��ͼƬ
		//ǰ��˵���ˣ�Option�൱�ڵ�ѡ����ѡ�����û��ѡ�У���ôһ�������ʾNormal�ˣ�
		//���ѡ���ˣ���ô������ʾCheck����ôUnKnow��ʲô�أ�������ù����ݿ⣬������
		//�����ף�BOOL������ʵ������״̬���ֱ���: FALSE/TRUE/NULL��NULL�����ݿ��һ��
		//����״̬�����ʱ�������Unknow��ʾ����Ȼ��Ҳ���Բ��ã�Unknowһ���ǳ�ʼ����
		//ʱ�����ã��Ժ�ÿ���޸�STATE_CHECK״̬�������STATE_UNKNOWȥ��
		CMuiString m_strForeUnknowImage;
		CMuiString m_strForeNormalImage;
		CMuiString m_strForeCheckImage;
	};

}

#endif