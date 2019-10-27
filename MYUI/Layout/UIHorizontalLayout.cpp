
#include "UIHorizontalLayout.h"


namespace MYUI
{
	CHorizontalLayoutUI::CHorizontalLayoutUI()
		: m_bCentered(true)
	{
	}

	CHorizontalLayoutUI::~CHorizontalLayoutUI()
	{
	}

	CMuiString CHorizontalLayoutUI::g_strClassName(_T("HorizontalLayoutUI"));

	CMuiString CHorizontalLayoutUI::GetClassName() const
	{
		return CHorizontalLayoutUI::g_strClassName;
	}

	SIZE CHorizontalLayoutUI::GetContentSize()
	{
		//�����������ʾ��Ҫ�ĳߴ磬�Դ˿����жϳ��Ƿ���Ҫ������
		SIZE szContent = {0};
		CControlUI * pControl = nullptr;
		int nControlCount = m_Items.GetSize();

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];
			
			if(false == pControl->IsVisible())
			{
				continue;
			}

			szContent.cx += pControl->GetValidSize().cx;
			szContent.cy = MAX(szContent.cy, pControl->GetValidSize().cy);
		}

		szContent.cx += m_rcInset.left + m_rcInset.right;
		szContent.cy += m_rcInset.top + m_rcInset.bottom;
		return szContent;
	}

	bool CHorizontalLayoutUI::SetItem(RECT rcItem, bool bMustUpdate)
	{
		
		//�����ӿؼ��Ĵ�ӡ����Ҫע���ӿؼ��ĸߣ��Ƿ���ʾ
		RECT rcThisRegoin;//���ݿͻ�����������ӿؼ���ʾ����
		RECT rcChildItem;
		RECT rcChildMargin;
		POINT ptNextPos={0};//��һ���ؼ���λ��
		SIZE siValidSize;
		SIZE siNeedSize;

		int nCenterPos = 0;
		int nFloatWidth = 0;
		int nDispatchWidth = 0;
		int nFloatControlCount = 0;
		int nActiveControlCount = 0;
		int nLastFloatControl = 0;

		CControlUI * pControl = nullptr;

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

			if(EnumFloatType::FloatSizeCx == (EnumFloatType::FloatSizeCx & type))
			{
				//���������λ�õĿ�
				++nFloatControlCount;
				nFloatWidth += pControl->GetValidSize().cx;
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
		siNeedSize = GetContentSize();
		rcThisRegoin = m_rcClient;
		rcThisRegoin.bottom = rcThisRegoin.top + MAX(siNeedSize.cy, rcThisRegoin.bottom - rcThisRegoin.top);
		IndentRect(&rcThisRegoin, &m_rcInset);
		ptNextPos.y = m_rcInset.top;
		//��������Կ�ȵĿؼ��򲼾ֵĿ���ܺͣ�
		//Ȼ��ʣ��Ŀ�ȷ������Բ��ֵĿؼ�
		if(nActiveControlCount == nFloatControlCount)
		{
			if(true == m_bCentered)
			{
				//nActiveControlCount ���� nFixedControlCount
				//˵�����е�����Ӧ�ؼ���ָ���˸߶ȣ����Ǿ���������Щ����Ӧ�ؼ�
				ptNextPos.x = ((rcThisRegoin.right - rcThisRegoin.left) - nFloatWidth) / 2;
				//�������Ӧ�Ŀؼ��ȴ�ֱ����Ҫ�ߣ���ô��0��ʼ��ʾ
				ptNextPos.x = MAX(ptNextPos.x ,rcThisRegoin.left);
			}
			else/*����ȡ���в��ԣ�����������*/
			{
				ptNextPos.x = rcThisRegoin.left;
			}
		}
		else
		{
			//����,�����ÿ������Ӧ�ؼ��ɷ���ĸ߶�
			ptNextPos.x = rcThisRegoin.left;
			nDispatchWidth = ((rcThisRegoin.right - rcThisRegoin.left) - nFloatWidth)\
				/ (nActiveControlCount - nFloatControlCount);
			nDispatchWidth = MAX(nDispatchWidth, 0);
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

			rcChildItem.left += ptNextPos.x;
			rcChildItem.top += ptNextPos.y;
			siValidSize = pControl->GetValidSize();

			if(EnumFloatType::FloatSizeCy & type)
			{
				rcChildItem.bottom = rcChildItem.top + siValidSize.cy;

				//���о��м���
				nCenterPos = ((rcThisRegoin.bottom - rcThisRegoin.top) -\
					(rcChildItem.bottom - rcChildItem.top)) / 2;
				nCenterPos = MAX(nCenterPos, 0);

				rcChildItem.bottom += nCenterPos;
				rcChildItem.top += nCenterPos;
			}
			else
			{
				rcChildItem.bottom = rcChildItem.top + \
					MAX(siValidSize.cy , rcThisRegoin.bottom - rcThisRegoin.top);
			}
			
			if(EnumFloatType::FloatSizeCx & type)//ָ���˿��
			{
				rcChildItem.right = rcChildItem.left + siValidSize.cx;
			}
			else
			{
				//��������λ��
				if(i == nLastFloatControl)
				{
					//���һ�����λ�ÿؼ����п������ط��䲻��������Ҫ�����⴦��
					rcChildItem.right = rcChildItem.left + 
						MAX(siValidSize.cx, \
						((rcThisRegoin.right - rcThisRegoin.left)\
						- nDispatchWidth * (nControlCount - nFloatControlCount - 1) \
						- nFloatWidth));
				}
				else
				{
					rcChildItem.right = rcChildItem.left + MAX(siValidSize.cx , nDispatchWidth);
				}
			}

			ptNextPos.x += rcChildItem.right - rcChildItem.left;
			//��ȥ��߾�
			rcChildMargin = pControl->GetMargin();
			IndentRect(&rcChildItem, &rcChildMargin);
loop:
			pControl->SetItem(rcChildItem, false);
		}
		return true;
	}
}