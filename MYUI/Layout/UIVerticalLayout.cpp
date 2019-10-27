
#include "UIVerticalLayout.h"

namespace MYUI
{
	//垂直布局
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
		//计算出布局显示需要的尺寸，以此可以判断出是否需要滚动条
		//注意：如果xy坐标为负数，小于0的区域将无法显示，
		//虽然可以实现显示负数坐标，但按照逻辑来说，这是不正确的，
		//所以不打算支持这种操作
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
		//不管下层是否发生更改，这里都要对子控件进行修改
		//如果因为滚动条和item没发生变化的话，就不更改子控件，将会发生如下的BUG：
		//假设A控件高度20，B控件高度10，此时两个控件都发生了修改，A的高度变为10，B的高度变为20
		//A,B两个子控件修改完成后，对容器进行更新
		//那么此时滚动条跟容器本身的Item是没有发生变化的，但是子控件却发生了变化，
		//如果不进行子控件调整的话，将绘制出错误的控件位置，所以一定要更新

		//优化建议：每次都更新子控件，开销确实比较大，特别是列表控件，除非你能确保客户每次设置
		//区域后，都调用update函数。
		//但是这又不太合理，因为大量的子控件位置的调整，确实需要调整完毕后再对容器位置进行更新，
		//所以建议是设置一个旗标，当子控件发生了调整，就修改容器的旗标，当旗标为TRUE时，就一定更新
		//否则根据下层的返回值来判断是否更新
		
		//计算子控件的打印区域，要注意子控件的高，是否显示
		RECT rcThisRegoin;//根据客户区计算出的子控件显示区域
		RECT rcChildItem;
		RECT rcChildMargin;
		POINT ptNextPos={0};//下一个控件的位置
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

			if(EnumFloatType::FloatSizeCy == (EnumFloatType::FloatSizeCy & type))
			{
				//设置了相对位置的高
				++nFloatControlCount;
				nFloatHeight += pControl->GetValidSize().cy;
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
		rcThisRegoin = m_rcClient;
		rcThisRegoin.right = rcThisRegoin.left + MAX(m_szContent.cx, rcThisRegoin.right - rcThisRegoin.left);
		IndentRect(&rcThisRegoin, &m_rcInset);
		ptNextPos.x = m_rcInset.left;
		//先求出绝对高度的控件或布局的高度总和，
		//然后将剩余的高度分配给相对布局的控件
		if(nActiveControlCount == nFloatControlCount)
		{
			if(true == m_bCentered)
			{
				//nActiveControlCount 等于 nFixedControlCount
				//说明所有的自适应控件都指定了高度，我们尽量居中这些自适应控件
				ptNextPos.y = ((rcThisRegoin.bottom - rcThisRegoin.top) - nFloatHeight) / 2;
				//如果自适应的控件比垂直布局要高，那么从0开始显示
				ptNextPos.y = MAX(ptNextPos.y ,rcThisRegoin.top);
			}
			else/*不采取居中策略，从上往下排*/
			{
				ptNextPos.y = rcThisRegoin.top;
			}
		}
		else
		{
			//调整,计算出每个自适应控件可分配的高度
			ptNextPos.y = rcThisRegoin.top;
			nDispatchHeight = ((rcThisRegoin.bottom - rcThisRegoin.top) - nFloatHeight)\
				/ (nActiveControlCount - nFloatControlCount);
			nDispatchHeight = MAX(nDispatchHeight, 0);
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

			rcChildItem.top += ptNextPos.y;
			rcChildItem.left += ptNextPos.x;
			siValidSize = pControl->GetValidSize();

			if(EnumFloatType::FloatSizeCx & type)
			{
				rcChildItem.right = rcChildItem.left + siValidSize.cx;

				//进行居中计算
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

			if(EnumFloatType::FloatSizeCy & type)//指定了高度
			{
				rcChildItem.bottom = rcChildItem.top + siValidSize.cy;
			}
			else
			{
				//如果是相对位置
				if(i == nLastFloatControl)
				{
					//最后一个相对位置控件，有可能像素分配不均，所以要做特殊处理
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
			//减去外边距
			rcChildMargin = pControl->GetMargin();
			IndentRect(&rcChildItem, &rcChildMargin);
loop:
			pControl->SetItem(rcChildItem, false);
		}
		return true;
	}
}