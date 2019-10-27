
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
		//计算出布局显示需要的尺寸，以此可以判断出是否需要滚动条
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
		
		//计算子控件的打印区域，要注意子控件的高，是否显示
		RECT rcThisRegoin;//根据客户区计算出的子控件显示区域
		RECT rcChildItem;
		RECT rcChildMargin;
		POINT ptNextPos={0};//下一个控件的位置
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
		if(nControlCount == 0 ) return true;//布局中没有其他控件就直接返回

		//通过上面计算得出布局在窗口中的绝对位置
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
				//设置了相对位置的宽
				++nFloatControlCount;
				nFloatWidth += pControl->GetValidSize().cx;
			}
			else
			{
				//如果是相对位置
				//最后一个相对控件的index
				//最后一个相对布局，有可能像素分配不均，所以要做特殊处理
				//其实将多余的像素分配给第一个或任意一个相对布局控件也可以，
				//但是分配给最后一个，更不容易让客户发现像素分配不均，有更好的体验效果
				nLastFloatControl = i;
			}
			++nActiveControlCount;
		}

		//先设置内缩进
		siNeedSize = GetContentSize();
		rcThisRegoin = m_rcClient;
		rcThisRegoin.bottom = rcThisRegoin.top + MAX(siNeedSize.cy, rcThisRegoin.bottom - rcThisRegoin.top);
		IndentRect(&rcThisRegoin, &m_rcInset);
		ptNextPos.y = m_rcInset.top;
		//先求出绝对宽度的控件或布局的宽度总和，
		//然后将剩余的宽度分配给相对布局的控件
		if(nActiveControlCount == nFloatControlCount)
		{
			if(true == m_bCentered)
			{
				//nActiveControlCount 等于 nFixedControlCount
				//说明所有的自适应控件都指定了高度，我们尽量居中这些自适应控件
				ptNextPos.x = ((rcThisRegoin.right - rcThisRegoin.left) - nFloatWidth) / 2;
				//如果自适应的控件比垂直布局要高，那么从0开始显示
				ptNextPos.x = MAX(ptNextPos.x ,rcThisRegoin.left);
			}
			else/*不采取居中策略，从上往下排*/
			{
				ptNextPos.x = rcThisRegoin.left;
			}
		}
		else
		{
			//调整,计算出每个自适应控件可分配的高度
			ptNextPos.x = rcThisRegoin.left;
			nDispatchWidth = ((rcThisRegoin.right - rcThisRegoin.left) - nFloatWidth)\
				/ (nActiveControlCount - nFloatControlCount);
			nDispatchWidth = MAX(nDispatchWidth, 0);
		}

		for(int i=0; nControlCount > i; i++)
		{
			pControl =  (CControlUI*)m_Items[i];
			type =  pControl->GetFloatType();

			//置空
			EmptyRect(rcChildItem);
			
			if(false == pControl->IsVisible())
			{
				//不显示打印区域就是空了
				goto loop;
			}

			rcChildItem.left += ptNextPos.x;
			rcChildItem.top += ptNextPos.y;
			siValidSize = pControl->GetValidSize();

			if(EnumFloatType::FloatSizeCy & type)
			{
				rcChildItem.bottom = rcChildItem.top + siValidSize.cy;

				//进行居中计算
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
			
			if(EnumFloatType::FloatSizeCx & type)//指定了宽度
			{
				rcChildItem.right = rcChildItem.left + siValidSize.cx;
			}
			else
			{
				//如果是相对位置
				if(i == nLastFloatControl)
				{
					//最后一个相对位置控件，有可能像素分配不均，所以要做特殊处理
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
			//减去外边距
			rcChildMargin = pControl->GetMargin();
			IndentRect(&rcChildItem, &rcChildMargin);
loop:
			pControl->SetItem(rcChildItem, false);
		}
		return true;
	}
}