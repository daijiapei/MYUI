
#if !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)
#define AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_


#include <Windows.h>

//�ݴ�ȼ�
#define QR_LEVEL_L  0
#define QR_LEVEL_M  1
#define QR_LEVEL_Q  2
#define QR_LEVEL_H  3

//��������
#define QR_MODE_NUMERAL   0 //���ֱ���
#define QR_MODE_ALPHABET  1 //�ַ�����
#define QR_MODE_8BIT      2 //�ֽڱ���

//�汾(�ͺ�)��
#define QR_VRESION_S  0 //  1 �� 9
#define QR_VRESION_M  1 // 10 �� 26
#define QR_VRESION_L  2 // 27 �� 40

//�ַ�����ָʾ���е�λ�� { S, M, L } ���ö���bit���洢�ַ�������S:�汾1~9 M:�汾10~26 L:�汾27~40
static int g_iIndicatorLen_Numeral[] = { 10, 12, 14 }; //���ֱ���
static int g_iIndicatorLen_Alphabet[] = { 9, 11, 13 }; //�ַ�����
static int g_iIndicatorLen_8Bit[] = { 8, 16, 16 }; //�ֽڱ���

#define MAX_ALLCODEWORD	 3706 // �����������
#define MAX_DATACODEWORD 2956 // ���ݴ������������ֵ(��Ӧ�汾40 - L)
#define MAX_CODEBLOCK	  153 // ����������ֵ(RS����������)
#define MAX_MODULESIZE	  177 // һ����ͼ��������������ֵ����Ӧ�汾40��

//λͼ��ͼʱ���ܵİױ߿��
#define QR_MARGIN	2

typedef struct tagRS_BLOCKINFO
{
    int icRSBlock;		//RS����
    int icAllCodeWord;	//�����ܸ���
    int icDataCodeWord;	//���������
} RS_BLOCKINFO, *LPRS_BLOCKINFO;

// QR ����汾(�ͺ�)�����Ϣ
typedef struct tagQR_VERSIONINFO
{
    int iVersionNo;	   //�汾(�ͺ�)����(1 - 40)
    int icAllCodeWord; //�������������ֽ�

    int icDataCodeWord[4];	//���ݱ��벿������(����������- RS��������)���ֽ�([0] = L, 1 = M, 2 = Q, 3 = H) 

    int icAlignPoint;	//����ͼ�������������Ͻǵ����½�
    int iAlignPoint[6];	//����ͼ����������

    RS_BLOCKINFO RS_BlockInfo1[4]; //RS����Ϣ(1)
    RS_BLOCKINFO RS_BlockInfo2[4]; //RS����Ϣ(2)

} QR_VERSIONINFO, *LPQR_VERSIONINFO;

// CQR_Encode ��
class CQR_Encode
{
public:
    CQR_Encode();
    ~CQR_Encode();

public:
    int m_iLevel;		//����ȼ�
    int m_iVersion;		//�汾��
    int m_iMaskingNo;	//��ģ����(�ɰ�ͼ�����)

public:
    int m_iSymbleSize;  // ���ž�������߳�
    BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; //[x][y] ���ž���洢�������汾��40��Ӧ�߳�177

private:
    int m_icDataCodeWordBit;                 //�������λ����
    BYTE m_byDataCodeWord[MAX_DATACODEWORD]; //������

    int m_icDataBlock;
    BYTE m_byBlockMode[MAX_DATACODEWORD]; //Դ����ÿ�����ֿ����������
    int m_iBlockLength[MAX_DATACODEWORD]; //Դ����ÿ�����ֿ�����ݵĳ���

    int m_icAllCodeWord; // �ܴ�������(RS��������������ڵ�)
    BYTE m_byAllCodeWord[MAX_ALLCODEWORD]; // �ܴ����ּ�������
    BYTE m_byRSData[MAX_CODEBLOCK]; //RS�������

    //���ݱ������
public:
    BOOL EncodeData(LPCTSTR v_lpsSource, int v_iVersion = 0, int v_iLevel = 1, BOOL v_bAutoExtent = TRUE, int v_iMaskingNo = -1, int v_icSourceLen = 0); //���ַ���ת����QR Code����

private:
    int GetEncodeVersion(int v_iVersion, CHAR *v_lpsSource, int v_icLength); //��ȡ���ŵİ汾�ţ������ݽ��б��루ͷ�������ݣ�

    BOOL IsNumeralData(unsigned char c); //����ֽ������Ƿ�������
    BOOL IsAlphabetData(unsigned char c); //����ֽ������Ƿ��ǵ��ֽ��ַ�

    BOOL EncodeSourceData(CHAR * v_lpsSource, int v_icLength, int v_iVerGroup); //��Դ�ַ������ݰ��ղ�ͬ������л��ֳɶ���飬�����Ժϲ�ѹ����Ȼ������������

    void SplitBlocks(CHAR *v_lpsSource, int v_icLength); //����ַ���ÿһ�ֽ���ʲô���ݣ����ղ�ͬ�����໮�ֳɶ��
    void TryCombineBlocks(int v_iVerGroup); //���Խ����ݿ�ϲ�ѹ��
    void CreateDataCodeWord(CHAR *v_lpsSource, int v_iVerGroup); //����������

    BYTE AlphabetToBinaly(unsigned char v_c); //���ַ��������ݶ����ƻ�
    int GetBitLength(BYTE v_bMode, int v_icDataCount, int v_iVerGroup); //��������ݿ�λ������
    int SetBitStream(int v_iIndex, WORD v_wData, int v_icBitLen); //�����ݴ���������������

    void CreateRSCodeWord(int v_iLevel); //���ɾ�����
    void GetRSCodeWord(LPBYTE v_lpbyRSWork, int v_icDataCodeWordNum, int v_icRSCodeWordNum); //��ȡ������

    //���ɶ�ά��������
private:
    void FormatModule(); //���ɶ�ά�����

    void SetFunctionModule(); //���ù���ͼ�Σ�λ��̽��ͼ�Σ�У��ͼ������λͼ��������ʽ��Ϣ���汾��Ϣ
    void SetFinderPattern(int v_ix, int v_iy); //����λ��̽��ͼ��
    void SetAlignmentPattern(int v_ix, int v_y); //���ö�λͼ��
    void SetVersionPattern(); //���ð汾���ͺţ���Ϣ
    void SetCodeWordPattern(); //�������ݺ;�������
    void SetMaskingPattern(int v_iPatternNo); //������ģͼ��
    void SetFormatInfoPattern(int v_iPatternNo); //���ø�ʽ��Ϣ
    int  CountPenalty(); //���㷣��Ƿַ�ֵ
};

#ifdef __cplusplus
extern "C" {
#endif
    HBITMAP CreateQrCodeImage(HDC hdc, LPCTSTR strQrCodeText, SIZE * pszBitmap);
#ifdef __cplusplus
};
#endif



#endif