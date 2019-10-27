
#include "UIVerticalLayout.h"

namespace MYUI
{
	//��ֱ����
	CVerticalLayoutUI::CVerticalLayoutUI()
		: m_bCentered(true)
	{
		
	}

	CVerticalLayoutUI::~CVerticalLayoutUI()
	{
	}

	CMuiString CVerticalLayoutUI::g_strClassName(_T("VerticalLayoutUI"));

	CMuiString CVerticalLayoutUI::GetClassName() const
	{
		return CVerticalLayoutUI::g_strClassName;
	}

	SIZE CVerticalLayoutUI::GetContentSize()
	{
		//�����������ʾ��Ҫ�ĳߴ磬�Դ˿����жϳ��Ƿ���Ҫ������
		//ע�⣺���xy����Ϊ������С��0�������޷���ʾ��
		//��Ȼ����ʵ����ʾ�������꣬�������߼���˵�����ǲ���ȷ�ģ�
		//���Բ�����֧�����ֲ���
		SIZE szContent = {0};
		SIZE siValidSize;
		CControlUI * pControl = nullptr;
		int nControlCount = m_Items.GetSize();

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];
			
			if(false == pControl->IsVisible())
			{
				continue;
			}
			siValidSize = pControl->GetValidSize();
			szContent.cx = MAX(szContent.cx, siValidSize.cx);
			szContent.cy += siValidSize.cy;
		}

		szContent.cx += m_rcInset.left + m_rcInset.right;
		szContent.cy += m_rcInset.top + m_rcInset.bottom;
		return szContent;
	}

	bool CVerticalLayoutUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		//�����²��Ƿ������ģ����ﶼҪ���ӿؼ������޸�
		//�����Ϊ��������itemû�����仯�Ļ����Ͳ������ӿؼ������ᷢ�����µ�BUG��
		//����A�ؼ��߶�20��B�ؼ��߶�10����ʱ�����ؼ����������޸ģ�A�ĸ߶ȱ�Ϊ10��B�ĸ߶ȱ�Ϊ20
		//A,B�����ӿؼ��޸���ɺ󣬶��������и���
		//��ô��ʱ�����������������Item��û�з����仯�ģ������ӿؼ�ȴ�����˱仯��
		//����������ӿؼ������Ļ��������Ƴ�����Ŀؼ�λ�ã�����һ��Ҫ����

		//�Ż����飺ÿ�ζ������ӿؼ�������ȷʵ�Ƚϴ��ر����б�ؼ�����������ȷ���ͻ�ÿ������
		//����󣬶�����update������
		//�������ֲ�̫������Ϊ�������ӿؼ�λ�õĵ�����ȷʵ��Ҫ������Ϻ��ٶ�����λ�ý��и��£�
		//���Խ���������һ����꣬���ӿؼ������˵��������޸���������꣬�����ΪTRUEʱ����һ������
		//��������²�ķ���ֵ���ж��Ƿ����
		
		//�����ӿؼ��Ĵ�ӡ����Ҫע���ӿؼ��ĸߣ��Ƿ���ʾ
		RECT rcThisRegoin;//���ݿͻ�����������ӿؼ���ʾ����
		RECT rcChildItem;
		RECT rcChildMargin;
		POINT ptNextPos={0};//��һ���ؼ���λ��
		SIZE siValidSize;

		int nCenterPos = 0;
		int nFloatHeight = 0;
		int nDispatchHeight = 0;
		int nFloatControlCount = 0;
		int nActiveControlCount = 0;
		int nLastFloatControl = 0;

		CControlUI * pControl = nullptr;
		//CMuiRect * pRect = nullptr;
		ScrollBarStyle dwStyle = ScrollBarStyle::NonStyle;
		EnumFloatType type = EnumFloatType::FloatNon;
		int nControlCount = m_Items.GetSize();

		if(false == __super::SetItem(rcItem, bMustUpdate)) return false;
		if(nControlCount == 0 ) return true;//������û�������ؼ���ֱ�ӷ���

		//ͨ���������ó������ڴ����еľ���λ��
		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];
			type =  pControl->GetFloatType();
			if(false == pControl->IsVisible())
			{
				continue;
			}

			if(EnumFloatType::FloatSizeCy == (EnumFloatType::FloatSizeCy & type))
			{
				//���������λ�õĸ�
				++nFloatControlCount;
				nFloatHeight += pControl->GetValidSize().cy;
			}
			else
			{
				//��������λ��
				//���һ����Կؼ���index
				//���һ����Բ��֣��п������ط��䲻��������Ҫ�����⴦��
				//��ʵ����������ط������һ��������һ����Բ��ֿؼ�Ҳ���ԣ�
				//���Ƿ�������һ�������������ÿͻ��������ط��䲻�����и��õ�����Ч��
				nLastFloatControl = i;
			}
			++nActiveControlCount;
		}

		//������������
		rcThisRegoin = m_rcClient;
		rcThisRegoin.right = rcThisRegoin.left + MAX(m_szContent.cx, rcThisRegoin.right - rcThisRegoin.left);
		IndentRect(&rcThisRegoin, &m_rcInset);
		ptNextPos.x = m_rcInset.left;
		//��������Ը߶ȵĿؼ��򲼾ֵĸ߶��ܺͣ�
		//Ȼ��ʣ��ĸ߶ȷ������Բ��ֵĿؼ�
		if(nActiveControlCount == nFloatControlCount)
		{
			if(true == m_bCentered)
			{
				//nActiveControlCount ���� nFixedControlCount
				//˵�����е�����Ӧ�ؼ���ָ���˸߶ȣ����Ǿ���������Щ����Ӧ�ؼ�
				ptNextPos.y = ((rcThisRegoin.bottom - rcThisRegoin.top) - nFloatHeight) / 2;
				//�������Ӧ�Ŀؼ��ȴ�ֱ����Ҫ�ߣ���ô��0��ʼ��ʾ
				ptNextPos.y = MAX(ptNextPos.y ,rcThisRegoin.top);
			}
			else/*����ȡ���в��ԣ�����������*/
			{
				ptNextPos.y = rcThisRegoin.top;
			}
		}
		else
		{
			//����,�����ÿ������Ӧ�ؼ��ɷ���ĸ߶�
			ptNextPos.y = rcThisRegoin.top;
			nDispatchHeight = ((rcThisRegoin.bottom - rcThisRegoin.top) - nFloatHeight)\
				/ (nActiveControlCount - nFloatControlCount);
			nDispatchHeight = MAX(nDispatchHeight, 0);
		}

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];
			type =  pControl->GetFloatType();

			//�ÿ�
			EmptyRect(rcChildItem);
			
			if(false == pControl->IsVisible())
			{
				//����ʾ��ӡ������ǿ���
				goto loop;
			}

			rcChildItem.top += ptNextPos.y;
			rcChildItem.left += ptNextPos.x;
			siValidSize = pControl->GetValidSize();

			if(EnumFloatType::FloatSizeCx & type)
			{
				rcChildItem.right = rcChildItem.left + siValidSize.cx;

				//���о��м���
				nCenterPos = ((rcThisRegoin.right - rcThisRegoin.left) -\
					(rcChildItem.right - rcChildItem.left)) / 2;
				nCenterPos = MAX(nCenterPos, 0);

				rcChildItem.left += nCenterPos;
				rcChildItem.right += nCenterPos;
			}
			else
			{
				rcChildItem.right = rcChildItem.left + \
					MAX(siValidSize.cx , rcThisRegoin.right - rcThisRegoin.left);
			}

			if(EnumFloatType::FloatSizeCy & type)//ָ���˸߶�
			{
				rcChildItem.bottom = rcChildItem.top + siValidSize.cy;
			}
			else
			{
				//��������λ��
				if(i == nLastFloatControl)
				{
					//���һ�����λ�ÿؼ����п������ط��䲻��������Ҫ�����⴦��
					rcChildItem.bottom = rcChildItem.top + 
						MAX(siValidSize.cy, \
						((rcThisRegoin.bottom - rcThisRegoin.top)\
						- nDispatchHeight * (nControlCount - nFloatControlCount - 1) \
						- nFloatHeight));
				}
				else
				{
					rcChildItem.bottom = rcChildItem.top + MAX(siValidSize.cy , nDispatchHeight);
				}
			}

			ptNextPos.y += rcChildItem.bottom - rcChildItem.top;
			//��ȥ��߾�
			rcChildMargin = pControl->GetMargin();
			IndentRect(&rcChildItem, &rcChildMargin);
loop:
			pControl->SetItem(rcChildItem, false);
		}
		return true;
	}
}