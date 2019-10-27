// testqrcode.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "QR_Encode.h"

/*
http://www.psytec.co.jp/
https://blog.csdn.net/hwhua1986/article/details/53889039
https://blog.csdn.net/u012611878/article/details/53167009
https://blog.csdn.net/liulina603/article/details/42099121
https://blog.csdn.net/xiaoxian8023/article/details/8536257
http://www.makaidong.com/%E5%8D%9A%E5%AE%A2%E5%9B%AD%E7%89%9B/32647.shtml
https://blog.csdn.net/ajianyingxiaoqinghan/article/details/78837864
*/

void MakeQRCode()
{
	CHAR llpstr[] = "�л����񹲺͹� \n ���A���񹲺͇� \n People's Republic of China \n ~`����!@#$%^&*()_+-=��@#��%����&����������+-={}|:\"<>?[]\\;',.//����|��������������������������//";

	CQR_Encode *pEncode = new CQR_Encode;
	BOOL bPass = pEncode->EncodeData(llpstr);//�ؼ�����
	if (bPass)
	{
		int width = 0;//ԭʼ��ά��Ŀ�͸�
		width = pEncode->m_iSymbleSize + (QR_MARGIN * 2);//
		CDC memdc;//����һ���ڴ�DC��������DC�ϻ��ƶ�ά��
		CBitmap memBmp;//�ڴ�BMP
		memdc.CreateCompatibleDC(NULL);
		memBmp.CreateCompatibleBitmap(&memdc, width, width);
		CBitmap *pOld = memdc.SelectObject(&memBmp);
		memdc.PatBlt(0, 0, width, width, WHITENESS);
		for (int i = 0; i<pEncode->m_iSymbleSize;++i)
		{
			for (int j = 0; j<pEncode->m_iSymbleSize; ++j)
			{
				if (pEncode->m_byModuleData[i][j])//��ά���Ӧ�����ݴ���������
				{
					memdc.SetPixel(i + QR_MARGIN, j + QR_MARGIN, RGB(0, 0, 0));//����Ӧ����������Ϊ��ɫ
				}
			}
		}
		int nDesSize = width * 5;//��Ϊԭʼ��ά���С������Ĵ��뽫ԭʼ�Ķ�ά��Ŵ�10���󱣴浽�ļ�
		CDC tempDc;
		tempDc.CreateCompatibleDC(NULL);
		CBitmap tempBmg;
		tempBmg.CreateCompatibleBitmap(&tempDc, nDesSize, nDesSize);
		CBitmap *pTold = tempDc.SelectObject(&tempBmg);
		tempDc.StretchBlt(0, 0, nDesSize, nDesSize, &memdc, 0, 0, width, width, SRCCOPY);


		CImage img;
		img.Attach(tempBmg.operator HBITMAP());
		img.Save(_T("QR_Encode_Output.jpg"));
		img.Detach();

		memdc.SelectObject(pOld);
		memBmp.DeleteObject();
		memdc.DeleteDC();

		tempDc.SelectObject(pTold);
		tempBmg.DeleteObject();
		tempDc.DeleteDC();
	}

	delete pEncode;
	pEncode = NULL;

	ShellExecute( NULL, _T("open"), _T("QR_Encode_Output.jpg"), NULL, NULL, SW_SHOW );
}

int _tmain(int argc, _TCHAR* argv[])
{
	MakeQRCode();
	return 0;
}

