#pragma once

//容错等级
#define QR_LEVEL_L  0
#define QR_LEVEL_M  1
#define QR_LEVEL_Q  2
#define QR_LEVEL_H  3

//数据类型
#define QR_MODE_NUMERAL   0 //数字编码
#define QR_MODE_ALPHABET  1 //字符编码
#define QR_MODE_8BIT      2 //字节编码

//版本(型号)组
#define QR_VRESION_S  0 //  1 ～ 9
#define QR_VRESION_M  1 // 10 ～ 26
#define QR_VRESION_L  2 // 27 ～ 40

//字符计数指示器中的位数 { S, M, L } ，用多少bit来存储字符个数，S:版本1~9 M:版本10~26 L:版本27~40
static int g_iIndicatorLen_Numeral[]	= { 10, 12, 14 }; //数字编码
static int g_iIndicatorLen_Alphabet[]	= {  9, 11, 13 }; //字符编码
static int g_iIndicatorLen_8Bit[]		= {  8, 16, 16 }; //字节编码

#define MAX_ALLCODEWORD	 3706 // 最大数据容量
#define MAX_DATACODEWORD 2956 // 数据代码字允许最大值(对应版本40 - L)
#define MAX_CODEBLOCK	  153 // 代码块数最大值(RS编码字在内)
#define MAX_MODULESIZE	  177 // 一个码图矩阵符号数量最大值（对应版本40）

//位图绘图时四周的白边宽度
#define QR_MARGIN	4

typedef struct tagRS_BLOCKINFO
{
	int icRSBlock;		//RS块数
	int icAllCodeWord;	//码字总个数
	int icDataCodeWord;	//数据码个数
} RS_BLOCKINFO, *LPRS_BLOCKINFO;

// QR 代码版本(型号)相关信息
typedef struct tagQR_VERSIONINFO
{
	int iVersionNo;	   //版本(型号)号码(1 - 40)
	int icAllCodeWord; //总数据容量，字节

	int icDataCodeWord[4];	//数据编码部分字数(总数据容量- RS编码字数)，字节([0] = L, 1 = M, 2 = Q, 3 = H) 

	int icAlignPoint;	//对齐图案坐标数，左上角到右下角
	int iAlignPoint[6];	//对齐图案中心坐标

	RS_BLOCKINFO RS_BlockInfo1[4]; //RS块信息(1)
	RS_BLOCKINFO RS_BlockInfo2[4]; //RS块信息(2)

} QR_VERSIONINFO, *LPQR_VERSIONINFO;

// CQR_Encode 类
class CQR_Encode
{
public:
	CQR_Encode();
	~CQR_Encode();

public:
	int m_iLevel;		//纠错等级
	int m_iVersion;		//版本号
	int m_iMaskingNo;	//掩模号码(蒙版图案编号)

public:
	int m_iSymbleSize;  // 符号矩阵输出边长
	BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; //[x][y] 符号矩阵存储区，最大版本号40对应边长177

private:
	int m_icDataCodeWordBit;                 //数据码的位长度
	BYTE m_byDataCodeWord[MAX_DATACODEWORD]; //数据码

	int m_icDataBlock;
	BYTE m_byBlockMode[MAX_DATACODEWORD]; //源数据每个划分块的数据类型
	int m_iBlockLength[MAX_DATACODEWORD]; //源数据每个划分块的数据的长度

	int m_icAllCodeWord; // 总代码字数(RS错误更正数据在内的)
	BYTE m_byAllCodeWord[MAX_ALLCODEWORD]; // 总代码字计算区域
	BYTE m_byRSData[MAX_CODEBLOCK]; //RS块纠错码

	//数据编码相关
public:
	BOOL EncodeData(CHAR *v_lpsSource, int v_iVersion = 0, int v_iLevel = 1, BOOL v_bAutoExtent = TRUE, int v_iMaskingNo = -1, int v_icSourceLen = 0); //将字符串转换成QR Code矩阵

private:
	int GetEncodeVersion(int v_iVersion, CHAR *v_lpsSource, int v_icLength); //获取最优的版本号，将数据进行编码（头部，内容）

	BOOL IsNumeralData(unsigned char c); //检查字节内容是否是数字
	BOOL IsAlphabetData(unsigned char c); //检查字节内容是否是单字节字符

	BOOL EncodeSourceData(CHAR *v_lpsSource, int v_icLength, int v_iVerGroup); //将源字符串内容按照不同种类进行划分成多个块，并尝试合并压缩，然后生成数据码

	void SplitBlocks(CHAR *v_lpsSource, int v_icLength); //检查字符串每一字节是什么内容，按照不同的种类划分成多块
	void TryCombineBlocks(int v_iVerGroup); //尝试将数据块合并压缩
	void CreateDataCodeWord(CHAR *v_lpsSource, int v_iVerGroup); //生成数据码

	BYTE AlphabetToBinaly(unsigned char v_c); //将字符编码数据二进制化
	int GetBitLength(BYTE v_bMode, int v_icDataCount, int v_iVerGroup); //计算块数据块位长长度
	int SetBitStream(int v_iIndex, WORD v_wData, int v_icBitLen); //将数据存入数据码数组中

	void CreateRSCodeWord(int v_iLevel); //生成纠错码
	void GetRSCodeWord(LPBYTE v_lpbyRSWork, int v_icDataCodeWordNum, int v_icRSCodeWordNum); //获取纠错码

	//生成二维码矩阵相关
private:
	void FormatModule(); //生成二维码矩阵

	void SetFunctionModule(); //配置功能图形（位置探测图形，校正图案，定位图案），格式信息，版本信息
	void SetFinderPattern(int v_ix, int v_iy); //配置位置探测图形
	void SetAlignmentPattern(int v_ix, int v_y); //配置定位图形
	void SetVersionPattern(); //配置版本（型号）信息
	void SetCodeWordPattern(); //配置数据和纠错码字
	void SetMaskingPattern(int v_iPatternNo); //配置掩模图案
	void SetFormatInfoPattern(int v_iPatternNo); //配置格式信息
	int  CountPenalty(); //计算罚点记分分值
};