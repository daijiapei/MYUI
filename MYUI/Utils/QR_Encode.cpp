#include <Windows.h>
#include "QR_Encode.h"

/*
typedef struct tagRS_BLOCKINFO
{
int icRSBlock;		// RS块数
int icAllCodeWord;	// 块内代码字数，字节
int icDataCodeWord;	// 数据代码字数 = 总数据容量 - RS编码字数
} RS_BLOCKINFO, *LPRS_BLOCKINFO;

typedef struct tagQR_VERSIONINFO
{
int iVersionNo;	   //版本(型号)号码(1 - 40)
int icAllCodeWord; //总数据容量，字节
int icDataCodeWord[4];	//数据编码部分字数(总数据容量- RS编码字数)，字节([0] = L, 1 = M, 2 = Q, 3 = H)
int icAlignPoint;	// 对齐图案坐标数，左上角到右下角
int iAlignPoint[6];	// 对齐图案中心坐标
RS_BLOCKINFO RS_BlockInfo1[4]; // RS块信息(1)
RS_BLOCKINFO RS_BlockInfo2[4]; // RS块信息(2)
} QR_VERSIONINFO, *LPQR_VERSIONINFO;
*/

// QR 条形码代码版本(型号)信息
static QR_VERSIONINFO QR_VersonInfo[] = {
    // Ver.0
        { 0 },
        // Ver.1
        { 1, 26,
        19, 16, 13, 9,
        0, 0, 0, 0, 0, 0, 0,
        1, 26, 19, 1, 26, 16, 1, 26, 13, 1, 26, 9,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        // Ver.2
        { 2, 44,
        34, 28, 22, 16,
        1, 18, 0, 0, 0, 0, 0,
        1, 44, 34, 1, 44, 28, 1, 44, 22, 1, 44, 16,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        // Ver.3
        { 3, 70,
        55, 44, 34, 26,
        1, 22, 0, 0, 0, 0, 0,
        1, 70, 55, 1, 70, 44, 2, 35, 17, 2, 35, 13,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        // Ver.4
        { 4, 100,
        80, 64, 48, 36,
        1, 26, 0, 0, 0, 0, 0,
        1, 100, 80, 2, 50, 32, 2, 50, 24, 4, 25, 9,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        // Ver.5
        { 5, 134,
        108, 86, 62, 46,
        1, 30, 0, 0, 0, 0, 0,
        1, 134, 108, 2, 67, 43, 2, 33, 15, 2, 33, 11,
        0, 0, 0, 0, 0, 0, 2, 34, 16, 2, 34, 12 },
        // Ver.6
        { 6, 172,
        136, 108, 76, 60,
        1, 34, 0, 0, 0, 0, 0,
        2, 86, 68, 4, 43, 27, 4, 43, 19, 4, 43, 15,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        // Ver.7
        { 7, 196,
        156, 124, 88, 66,
        2, 22, 38, 0, 0, 0, 0,
        2, 98, 78, 4, 49, 31, 2, 32, 14, 4, 39, 13,
        0, 0, 0, 0, 0, 0, 4, 33, 15, 1, 40, 14 },
        // Ver.8
        { 8, 242,
        194, 154, 110, 86,
        2, 24, 42, 0, 0, 0, 0,
        2, 121, 97, 2, 60, 38, 4, 40, 18, 4, 40, 14,
        0, 0, 0, 2, 61, 39, 2, 41, 19, 2, 41, 15 },
        // Ver.9
        { 9, 292,
        232, 182, 132, 100,
        2, 26, 46, 0, 0, 0, 0,
        2, 146, 116, 3, 58, 36, 4, 36, 16, 4, 36, 12,
        0, 0, 0, 2, 59, 37, 4, 37, 17, 4, 37, 13 },
        // Ver.10
        { 10, 346,
        274, 216, 154, 122,
        2, 28, 50, 0, 0, 0, 0,
        2, 86, 68, 4, 69, 43, 6, 43, 19, 6, 43, 15,
        2, 87, 69, 1, 70, 44, 2, 44, 20, 2, 44, 16 },
        // Ver.11
        { 11, 404,
        324, 254, 180, 140,
        2, 30, 54, 0, 0, 0, 0,
        4, 101, 81, 1, 80, 50, 4, 50, 22, 3, 36, 12,
        0, 0, 0, 4, 81, 51, 4, 51, 23, 8, 37, 13 },
        // Ver.12
        { 12, 466,
        370, 290, 206, 158,
        2, 32, 58, 0, 0, 0, 0,
        2, 116, 92, 6, 58, 36, 4, 46, 20, 7, 42, 14,
        2, 117, 93, 2, 59, 37, 6, 47, 21, 4, 43, 15 },
        // Ver.13
        { 13, 532,
        428, 334, 244, 180,
        2, 34, 62, 0, 0, 0, 0,
        4, 133, 107, 8, 59, 37, 8, 44, 20, 12, 33, 11,
        0, 0, 0, 1, 60, 38, 4, 45, 21, 4, 34, 12 },
        // Ver.14
        { 14, 581,
        461, 365, 261, 197,
        3, 26, 46, 66, 0, 0, 0,
        3, 145, 115, 4, 64, 40, 11, 36, 16, 11, 36, 12,
        1, 146, 116, 5, 65, 41, 5, 37, 17, 5, 37, 13 },
        // Ver.15
        { 15, 655,
        523, 415, 295, 223,
        3, 26, 48, 70, 0, 0, 0,
        5, 109, 87, 5, 65, 41, 5, 54, 24, 11, 36, 12,
        1, 110, 88, 5, 66, 42, 7, 55, 25, 7, 37, 13 },
        // Ver.16
        { 16, 733,
        589, 453, 325, 253,
        3, 26, 50, 74, 0, 0, 0,
        5, 122, 98, 7, 73, 45, 15, 43, 19, 3, 45, 15,
        1, 123, 99, 3, 74, 46, 2, 44, 20, 13, 46, 16 },
        // Ver.17
        { 17, 815,
        647, 507, 367, 283,
        3, 30, 54, 78, 0, 0, 0,
        1, 135, 107, 10, 74, 46, 1, 50, 22, 2, 42, 14,
        5, 136, 108, 1, 75, 47, 15, 51, 23, 17, 43, 15 },
        // Ver.18
        { 18, 901,
        721, 563, 397, 313,
        3, 30, 56, 82, 0, 0, 0,
        5, 150, 120, 9, 69, 43, 17, 50, 22, 2, 42, 14,
        1, 151, 121, 4, 70, 44, 1, 51, 23, 19, 43, 15 },
        // Ver.19
        { 19, 991,
        795, 627, 445, 341,
        3, 30, 58, 86, 0, 0, 0,
        3, 141, 113, 3, 70, 44, 17, 47, 21, 9, 39, 13,
        4, 142, 114, 11, 71, 45, 4, 48, 22, 16, 40, 14 },
        // Ver.20
        { 20, 1085,
        861, 669, 485, 385,
        3, 34, 62, 90, 0, 0, 0,
        3, 135, 107, 3, 67, 41, 15, 54, 24, 15, 43, 15,
        5, 136, 108, 13, 68, 42, 5, 55, 25, 10, 44, 16 },
        // Ver.21
        { 21, 1156,
        932, 714, 512, 406,
        4, 28, 50, 72, 94, 0, 0,
        4, 144, 116, 17, 68, 42, 17, 50, 22, 19, 46, 16,
        4, 145, 117, 0, 0, 0, 6, 51, 23, 6, 47, 17 },
        // Ver.22
        { 22, 1258,
        1006, 782, 568, 442,
        4, 26, 50, 74, 98, 0, 0,
        2, 139, 111, 17, 74, 46, 7, 54, 24, 34, 37, 13,
        7, 140, 112, 0, 0, 0, 16, 55, 25, 0, 0, 0 },
        // Ver.23
        { 23, 1364,
        1094, 860, 614, 464,
        4, 30, 54, 78, 102, 0, 0,
        4, 151, 121, 4, 75, 47, 11, 54, 24, 16, 45, 15,
        5, 152, 122, 14, 76, 48, 14, 55, 25, 14, 46, 16 },
        // Ver.24
        { 24, 1474,
        1174, 914, 664, 514,
        4, 28, 54, 80, 106, 0, 0,
        6, 147, 117, 6, 73, 45, 11, 54, 24, 30, 46, 16,
        4, 148, 118, 14, 74, 46, 16, 55, 25, 2, 47, 17 },
        // Ver.25
        { 25, 1588,
        1276, 1000, 718, 538,
        4, 32, 58, 84, 110, 0, 0,
        8, 132, 106, 8, 75, 47, 7, 54, 24, 22, 45, 15,
        4, 133, 107, 13, 76, 48, 22, 55, 25, 13, 46, 16 },
        // Ver.26
        { 26, 1706,
        1370, 1062, 754, 596,
        4, 30, 58, 86, 114, 0, 0,
        10, 142, 114, 19, 74, 46, 28, 50, 22, 33, 46, 16,
        2, 143, 115, 4, 75, 47, 6, 51, 23, 4, 47, 17 },
        // Ver.27
        { 27, 1828,
        1468, 1128, 808, 628,
        4, 34, 62, 90, 118, 0, 0,
        8, 152, 122, 22, 73, 45, 8, 53, 23, 12, 45, 15,
        4, 153, 123, 3, 74, 46, 26, 54, 24, 28, 46, 16 },
        // Ver.28
        { 28, 1921,
        1531, 1193, 871, 661,
        5, 26, 50, 74, 98, 122, 0,
        3, 147, 117, 3, 73, 45, 4, 54, 24, 11, 45, 15,
        10, 148, 118, 23, 74, 46, 31, 55, 25, 31, 46, 16 },
        // Ver.29
        { 29, 2051,
        1631, 1267, 911, 701,
        5, 30, 54, 78, 102, 126, 0,
        7, 146, 116, 21, 73, 45, 1, 53, 23, 19, 45, 15,
        7, 147, 117, 7, 74, 46, 37, 54, 24, 26, 46, 16 },
        // Ver.30
        { 30, 2185,
        1735, 1373, 985, 745,
        5, 26, 52, 78, 104, 130, 0,
        5, 145, 115, 19, 75, 47, 15, 54, 24, 23, 45, 15,
        10, 146, 116, 10, 76, 48, 25, 55, 25, 25, 46, 16 },
        // Ver.31
        { 31, 2323,
        1843, 1455, 1033, 793,
        5, 30, 56, 82, 108, 134, 0,
        13, 145, 115, 2, 74, 46, 42, 54, 24, 23, 45, 15,
        3, 146, 116, 29, 75, 47, 1, 55, 25, 28, 46, 16 },
        // Ver.32
        { 32, 2465,
        1955, 1541, 1115, 845,
        5, 34, 60, 86, 112, 138, 0,
        17, 145, 115, 10, 74, 46, 10, 54, 24, 19, 45, 15,
        0, 0, 0, 23, 75, 47, 35, 55, 25, 35, 46, 16 },
        // Ver.33
        { 33, 2611,
        2071, 1631, 1171, 901,
        5, 30, 58, 86, 114, 142, 0,
        17, 145, 115, 14, 74, 46, 29, 54, 24, 11, 45, 15,
        1, 146, 116, 21, 75, 47, 19, 55, 25, 46, 46, 16 },
        // Ver.34
        { 34, 2761,
        2191, 1725, 1231, 961,
        5, 34, 62, 90, 118, 146, 0,
        13, 145, 115, 14, 74, 46, 44, 54, 24, 59, 46, 16,
        6, 146, 116, 23, 75, 47, 7, 55, 25, 1, 47, 17 },
        // Ver.35
        { 35, 2876,
        2306, 1812, 1286, 986,
        6, 30, 54, 78, 102, 126, 150,
        12, 151, 121, 12, 75, 47, 39, 54, 24, 22, 45, 15,
        7, 152, 122, 26, 76, 48, 14, 55, 25, 41, 46, 16 },
        // Ver.36
        { 36, 3034,
        2434, 1914, 1354, 1054,
        6, 24, 50, 76, 102, 128, 154,
        6, 151, 121, 6, 75, 47, 46, 54, 24, 2, 45, 15,
        14, 152, 122, 34, 76, 48, 10, 55, 25, 64, 46, 16 },
        // Ver.37
        { 37, 3196,
        2566, 1992, 1426, 1096,
        6, 28, 54, 80, 106, 132, 158,
        17, 152, 122, 29, 74, 46, 49, 54, 24, 24, 45, 15,
        4, 153, 123, 14, 75, 47, 10, 55, 25, 46, 46, 16 },
        // Ver.38
        { 38, 3362,
        2702, 2102, 1502, 1142,
        6, 32, 58, 84, 110, 136, 162,
        4, 152, 122, 13, 74, 46, 48, 54, 24, 42, 45, 15,
        18, 153, 123, 32, 75, 47, 14, 55, 25, 32, 46, 16 },
        // Ver.39
        { 39, 3532,
        2812, 2216, 1582, 1222,
        6, 26, 54, 82, 110, 138, 166,
        20, 147, 117, 40, 75, 47, 43, 54, 24, 10, 45, 15,
        4, 148, 118, 7, 76, 48, 22, 55, 25, 67, 46, 16 },
        // Ver.40
        { 40, 3706,
        2956, 2334, 1666, 1276,
        6, 30, 58, 86, 114, 142, 170,
        19, 148, 118, 18, 75, 47, 34, 54, 24, 20, 45, 15,
        6, 149, 119, 31, 76, 48, 34, 55, 25, 61, 46, 16 }
};

//GF(2^8) α指数→整数转换表
static BYTE byExpToInt[] = { 1, 2, 4, 8, 16, 32, 64, 128, 29, 58, 116, 232, 205, 135, 19, 38,
76, 152, 45, 90, 180, 117, 234, 201, 143, 3, 6, 12, 24, 48, 96, 192,
157, 39, 78, 156, 37, 74, 148, 53, 106, 212, 181, 119, 238, 193, 159, 35,
70, 140, 5, 10, 20, 40, 80, 160, 93, 186, 105, 210, 185, 111, 222, 161,
95, 190, 97, 194, 153, 47, 94, 188, 101, 202, 137, 15, 30, 60, 120, 240,
253, 231, 211, 187, 107, 214, 177, 127, 254, 225, 223, 163, 91, 182, 113, 226,
217, 175, 67, 134, 17, 34, 68, 136, 13, 26, 52, 104, 208, 189, 103, 206,
129, 31, 62, 124, 248, 237, 199, 147, 59, 118, 236, 197, 151, 51, 102, 204,
133, 23, 46, 92, 184, 109, 218, 169, 79, 158, 33, 66, 132, 21, 42, 84,
168, 77, 154, 41, 82, 164, 85, 170, 73, 146, 57, 114, 228, 213, 183, 115,
230, 209, 191, 99, 198, 145, 63, 126, 252, 229, 215, 179, 123, 246, 241, 255,
227, 219, 171, 75, 150, 49, 98, 196, 149, 55, 110, 220, 165, 87, 174, 65,
130, 25, 50, 100, 200, 141, 7, 14, 28, 56, 112, 224, 221, 167, 83, 166,
81, 162, 89, 178, 121, 242, 249, 239, 195, 155, 43, 86, 172, 69, 138, 9,
18, 36, 72, 144, 61, 122, 244, 245, 247, 243, 251, 235, 203, 139, 11, 22,
44, 88, 176, 125, 250, 233, 207, 131, 27, 54, 108, 216, 173, 71, 142, 1 };

//GF(2^8) α整数→指数转换表的索引
static BYTE byIntToExp[] = { 0, 0, 1, 25, 2, 50, 26, 198, 3, 223, 51, 238, 27, 104, 199, 75,
4, 100, 224, 14, 52, 141, 239, 129, 28, 193, 105, 248, 200, 8, 76, 113,
5, 138, 101, 47, 225, 36, 15, 33, 53, 147, 142, 218, 240, 18, 130, 69,
29, 181, 194, 125, 106, 39, 249, 185, 201, 154, 9, 120, 77, 228, 114, 166,
6, 191, 139, 98, 102, 221, 48, 253, 226, 152, 37, 179, 16, 145, 34, 136,
54, 208, 148, 206, 143, 150, 219, 189, 241, 210, 19, 92, 131, 56, 70, 64,
30, 66, 182, 163, 195, 72, 126, 110, 107, 58, 40, 84, 250, 133, 186, 61,
202, 94, 155, 159, 10, 21, 121, 43, 78, 212, 229, 172, 115, 243, 167, 87,
7, 112, 192, 247, 140, 128, 99, 13, 103, 74, 222, 237, 49, 197, 254, 24,
227, 165, 153, 119, 38, 184, 180, 124, 17, 68, 146, 217, 35, 32, 137, 46,
55, 63, 209, 91, 149, 188, 207, 205, 144, 135, 151, 178, 220, 252, 190, 97,
242, 86, 211, 171, 20, 42, 93, 158, 132, 60, 57, 83, 71, 109, 65, 162,
31, 45, 67, 216, 183, 123, 164, 118, 196, 23, 73, 236, 127, 12, 111, 246,
108, 161, 59, 82, 41, 157, 85, 170, 251, 96, 134, 177, 187, 204, 62, 90,
203, 89, 95, 176, 156, 169, 160, 81, 11, 245, 22, 235, 122, 117, 44, 215,
79, 174, 213, 233, 230, 231, 173, 232, 116, 214, 244, 234, 168, 80, 88, 175 };

//生成多项式的误差修正系数α 错误更正生成多項式α係数
static BYTE byRSExp7[] = { 87, 229, 146, 149, 238, 102, 21 };
static BYTE byRSExp10[] = { 251, 67, 46, 61, 118, 70, 64, 94, 32, 45 };
static BYTE byRSExp13[] = { 74, 152, 176, 100, 86, 100, 106, 104, 130, 218, 206, 140, 78 };
static BYTE byRSExp15[] = { 8, 183, 61, 91, 202, 37, 51, 58, 58, 237, 140, 124, 5, 99, 105 };
static BYTE byRSExp16[] = { 120, 104, 107, 109, 102, 161, 76, 3, 91, 191, 147, 169, 182, 194, 225, 120 };
static BYTE byRSExp17[] = { 43, 139, 206, 78, 43, 239, 123, 206, 214, 147, 24, 99, 150, 39, 243, 163,
136 };
static BYTE byRSExp18[] = { 215, 234, 158, 94, 184, 97, 118, 170, 79, 187, 152, 148, 252, 179, 5, 98,
96, 153 };
static BYTE byRSExp20[] = { 17, 60, 79, 50, 61, 163, 26, 187, 202, 180, 221, 225, 83, 239, 156, 164,
212, 212, 188, 190 };
static BYTE byRSExp22[] = { 210, 171, 247, 242, 93, 230, 14, 109, 221, 53, 200, 74, 8, 172, 98, 80,
219, 134, 160, 105, 165, 231 };
static BYTE byRSExp24[] = { 229, 121, 135, 48, 211, 117, 251, 126, 159, 180, 169, 152, 192, 226, 228, 218,
111, 0, 117, 232, 87, 96, 227, 21 };
static BYTE byRSExp26[] = { 173, 125, 158, 2, 103, 182, 118, 17, 145, 201, 111, 28, 165, 53, 161, 21,
245, 142, 13, 102, 48, 227, 153, 145, 218, 70 };
static BYTE byRSExp28[] = { 168, 223, 200, 104, 224, 234, 108, 180, 110, 190, 195, 147, 205, 27, 232, 201,
21, 43, 245, 87, 42, 195, 212, 119, 242, 37, 9, 123 };
static BYTE byRSExp30[] = { 41, 173, 145, 152, 216, 31, 179, 182, 50, 48, 110, 86, 239, 96, 222, 125,
42, 173, 226, 193, 224, 130, 156, 37, 251, 216, 238, 40, 192, 180 };
static BYTE byRSExp32[] = { 10, 6, 106, 190, 249, 167, 4, 67, 209, 138, 138, 32, 242, 123, 89, 27,
120, 185, 80, 156, 38, 69, 171, 60, 28, 222, 80, 52, 254, 185, 220, 241 };
static BYTE byRSExp34[] = { 111, 77, 146, 94, 26, 21, 108, 19, 105, 94, 113, 193, 86, 140, 163, 125,
58, 158, 229, 239, 218, 103, 56, 70, 114, 61, 183, 129, 167, 13, 98, 62,
129, 51 };
static BYTE byRSExp36[] = { 200, 183, 98, 16, 172, 31, 246, 234, 60, 152, 115, 0, 167, 152, 113, 248,
238, 107, 18, 63, 218, 37, 87, 210, 105, 177, 120, 74, 121, 196, 117, 251,
113, 233, 30, 120 };
static BYTE byRSExp38[] = { 159, 34, 38, 228, 230, 59, 243, 95, 49, 218, 176, 164, 20, 65, 45, 111,
39, 81, 49, 118, 113, 222, 193, 250, 242, 168, 217, 41, 164, 247, 177, 30,
238, 18, 120, 153, 60, 193 };
static BYTE byRSExp40[] = { 59, 116, 79, 161, 252, 98, 128, 205, 128, 161, 247, 57, 163, 56, 235, 106,
53, 26, 187, 174, 226, 104, 170, 7, 175, 35, 181, 114, 88, 41, 47, 163,
125, 134, 72, 20, 232, 53, 35, 15 };
static BYTE byRSExp42[] = { 250, 103, 221, 230, 25, 18, 137, 231, 0, 3, 58, 242, 221, 191, 110, 84,
230, 8, 188, 106, 96, 147, 15, 131, 139, 34, 101, 223, 39, 101, 213, 199,
237, 254, 201, 123, 171, 162, 194, 117, 50, 96 };
static BYTE byRSExp44[] = { 190, 7, 61, 121, 71, 246, 69, 55, 168, 188, 89, 243, 191, 25, 72, 123,
9, 145, 14, 247, 1, 238, 44, 78, 143, 62, 224, 126, 118, 114, 68, 163,
52, 194, 217, 147, 204, 169, 37, 130, 113, 102, 73, 181 };
static BYTE byRSExp46[] = { 112, 94, 88, 112, 253, 224, 202, 115, 187, 99, 89, 5, 54, 113, 129, 44,
58, 16, 135, 216, 169, 211, 36, 1, 4, 96, 60, 241, 73, 104, 234, 8,
249, 245, 119, 174, 52, 25, 157, 224, 43, 202, 223, 19, 82, 15 };
static BYTE byRSExp48[] = { 228, 25, 196, 130, 211, 146, 60, 24, 251, 90, 39, 102, 240, 61, 178, 63,
46, 123, 115, 18, 221, 111, 135, 160, 182, 205, 107, 206, 95, 150, 120, 184,
91, 21, 247, 156, 140, 238, 191, 11, 94, 227, 84, 50, 163, 39, 34, 108 };
static BYTE byRSExp50[] = { 232, 125, 157, 161, 164, 9, 118, 46, 209, 99, 203, 193, 35, 3, 209, 111,
195, 242, 203, 225, 46, 13, 32, 160, 126, 209, 130, 160, 242, 215, 242, 75,
77, 42, 189, 32, 113, 65, 124, 69, 228, 114, 235, 175, 124, 170, 215, 232,
133, 205 };
static BYTE byRSExp52[] = { 116, 50, 86, 186, 50, 220, 251, 89, 192, 46, 86, 127, 124, 19, 184, 233,
151, 215, 22, 14, 59, 145, 37, 242, 203, 134, 254, 89, 190, 94, 59, 65,
124, 113, 100, 233, 235, 121, 22, 76, 86, 97, 39, 242, 200, 220, 101, 33,
239, 254, 116, 51 };
static BYTE byRSExp54[] = { 183, 26, 201, 87, 210, 221, 113, 21, 46, 65, 45, 50, 238, 184, 249, 225,
102, 58, 209, 218, 109, 165, 26, 95, 184, 192, 52, 245, 35, 254, 238, 175,
172, 79, 123, 25, 122, 43, 120, 108, 215, 80, 128, 201, 235, 8, 153, 59,
101, 31, 198, 76, 31, 156 };
static BYTE byRSExp56[] = { 106, 120, 107, 157, 164, 216, 112, 116, 2, 91, 248, 163, 36, 201, 202, 229,
6, 144, 254, 155, 135, 208, 170, 209, 12, 139, 127, 142, 182, 249, 177, 174,
190, 28, 10, 85, 239, 184, 101, 124, 152, 206, 96, 23, 163, 61, 27, 196,
247, 151, 154, 202, 207, 20, 61, 10 };
static BYTE byRSExp58[] = { 82, 116, 26, 247, 66, 27, 62, 107, 252, 182, 200, 185, 235, 55, 251, 242,
210, 144, 154, 237, 176, 141, 192, 248, 152, 249, 206, 85, 253, 142, 65, 165,
125, 23, 24, 30, 122, 240, 214, 6, 129, 218, 29, 145, 127, 134, 206, 245,
117, 29, 41, 63, 159, 142, 233, 125, 148, 123 };
static BYTE byRSExp60[] = { 107, 140, 26, 12, 9, 141, 243, 197, 226, 197, 219, 45, 211, 101, 219, 120,
28, 181, 127, 6, 100, 247, 2, 205, 198, 57, 115, 219, 101, 109, 160, 82,
37, 38, 238, 49, 160, 209, 121, 86, 11, 124, 30, 181, 84, 25, 194, 87,
65, 102, 190, 220, 70, 27, 209, 16, 89, 7, 33, 240 };
static BYTE byRSExp62[] = { 65, 202, 113, 98, 71, 223, 248, 118, 214, 94, 0, 122, 37, 23, 2, 228,
58, 121, 7, 105, 135, 78, 243, 118, 70, 76, 223, 89, 72, 50, 70, 111,
194, 17, 212, 126, 181, 35, 221, 117, 235, 11, 229, 149, 147, 123, 213, 40,
115, 6, 200, 100, 26, 246, 182, 218, 127, 215, 36, 186, 110, 106 };
static BYTE byRSExp64[] = { 45, 51, 175, 9, 7, 158, 159, 49, 68, 119, 92, 123, 177, 204, 187, 254,
200, 78, 141, 149, 119, 26, 127, 53, 160, 93, 199, 212, 29, 24, 145, 156,
208, 150, 218, 209, 4, 216, 91, 47, 184, 146, 47, 140, 195, 195, 125, 242,
238, 63, 99, 108, 140, 230, 242, 31, 204, 11, 178, 243, 217, 156, 213, 231 };
static BYTE byRSExp66[] = { 5, 118, 222, 180, 136, 136, 162, 51, 46, 117, 13, 215, 81, 17, 139, 247,
197, 171, 95, 173, 65, 137, 178, 68, 111, 95, 101, 41, 72, 214, 169, 197,
95, 7, 44, 154, 77, 111, 236, 40, 121, 143, 63, 87, 80, 253, 240, 126,
217, 77, 34, 232, 106, 50, 168, 82, 76, 146, 67, 106, 171, 25, 132, 93,
45, 105 };
static BYTE byRSExp68[] = { 247, 159, 223, 33, 224, 93, 77, 70, 90, 160, 32, 254, 43, 150, 84, 101,
190, 205, 133, 52, 60, 202, 165, 220, 203, 151, 93, 84, 15, 84, 253, 173,
160, 89, 227, 52, 199, 97, 95, 231, 52, 177, 41, 125, 137, 241, 166, 225,
118, 2, 54, 32, 82, 215, 175, 198, 43, 238, 235, 27, 101, 184, 127, 3,
5, 8, 163, 238 };

static LPBYTE  byRSExp[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, byRSExp7, NULL, NULL,
byRSExp10, NULL, NULL, byRSExp13, NULL, byRSExp15, byRSExp16, byRSExp17, byRSExp18, NULL,
byRSExp20, NULL, byRSExp22, NULL, byRSExp24, NULL, byRSExp26, NULL, byRSExp28, NULL,
byRSExp30, NULL, byRSExp32, NULL, byRSExp34, NULL, byRSExp36, NULL, byRSExp38, NULL,
byRSExp40, NULL, byRSExp42, NULL, byRSExp44, NULL, byRSExp46, NULL, byRSExp48, NULL,
byRSExp50, NULL, byRSExp52, NULL, byRSExp54, NULL, byRSExp56, NULL, byRSExp58, NULL,
byRSExp60, NULL, byRSExp62, NULL, byRSExp64, NULL, byRSExp66, NULL, byRSExp68 };

CQR_Encode::CQR_Encode()
{

}

CQR_Encode::~CQR_Encode()
{

}

/************************************************************************/
/*
函数名：	ANSIToUTF8
函数说明：	将字符串从 ANSI格式 转换成 UTF8格式
函数参数：	[in]pIn(源字符串), [out](存储转换后的字符串)
返回值：	无
*/
void ANSIToUTF8(const CHAR *pIn, CHAR *v_pOut)
{
    int iUnicodeLen = MultiByteToWideChar(CP_ACP, 0, pIn, -1, NULL, 0);
    WCHAR *pUnicode = new wchar_t[iUnicodeLen + 1];
    memset(pUnicode, 0, sizeof(pUnicode) / sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, pIn, -1, (LPWSTR)pUnicode, iUnicodeLen);
    pUnicode[iUnicodeLen] = '\0';

    int iAnsiLen = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)pUnicode, -1, NULL, 0, NULL, NULL);
    if (iAnsiLen <= 0
        || iAnsiLen > MAX_DATACODEWORD
        )
    {
        return;
    }
    WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)pUnicode, -1, (CHAR*)v_pOut, iAnsiLen, NULL, NULL);
    v_pOut[iAnsiLen] = '\0';

    delete pUnicode;
    pUnicode = NULL;

}//ANSIToUTF8

//将字符串从 Unicode格式 转换成 UTF8格式
/************************************************************************/
/*
函数名：	UnicodeToUTF8
函数说明：	将字符串从 Unicode格式 转换成 UTF8格式
函数参数：	[in]pIn(源字符串), [out](存储转换后的字符串)
返回值：	无
*/
void UnicodeToUTF8(const WCHAR *pIn, CHAR *v_pOut)
{
    int iLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pIn, -1, NULL, 0, NULL, NULL);
    if (iLen <= 0
        || iLen > MAX_DATACODEWORD
        )
    {
        return;
    }

    WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pIn, -1, (LPSTR)v_pOut, iLen, NULL, NULL);
    v_pOut[iLen] = '\0';

}//UnicodeToUTF8

/************************************************************************/
/*
函数名：	GetEncodeVersion
函数说明：	获取合适的版本号，将数据进行编码（头部，内容）
函数参数：	[in]v_iVersion(指定的版本号), [in]v_lpsSource(源字符串), [in](源字符串长度)
返回值：	int:合适的版本号
备注：		如果指定的版本号溢出，即>40，返回0
*/
int CQR_Encode::GetEncodeVersion(int v_iVersion, CHAR *v_lpsSource, int v_icLength)
{
    int iVerGroup = v_iVersion >= 27 ? QR_VRESION_L : (v_iVersion >= 10 ? QR_VRESION_M : QR_VRESION_S); //版本号组别

    for (int i = iVerGroup; i <= QR_VRESION_L; ++i)
    {
        //将数据进行编码（头部，内容）
        if (EncodeSourceData(v_lpsSource, v_icLength, i))
        {
            //匹配选择最小可用的版本（数据容量问题）
            if (i == QR_VRESION_S)
            {
                for (int j = 1; j <= 9; ++j)
                {
                    //m_icDataCodeWordBit(数据码字的位长度)，QR_VersonInfo[j].icDataCodeWord[m_iLevel]版本在指定容错等级下允许存储的数据容量(字节)
                    //( + 7 ) / 8，之所以/8是因为左边的是BIT，右边的是BYTE，要先转换成相同的单位再进行比较
                    //+7是与结束符有关， ( + 7) / 8得到的是一个int类型的数据，是数据编码可能的字节长度，假设数据长度是8，正好是8的倍数，这时候就不用填充结束符了
                    //是1字节，(8+7)/8还是1字节；如果是6，需要补充2个0，补充完是1字节，(6+7)/8还是1字节；如果是10，需要补充6个0，补充完后是2字节，(10+7)/8还是2字节
                    if ((m_icDataCodeWordBit + 7) / 8 <= QR_VersonInfo[j].icDataCodeWord[m_iLevel])
                    {
                        return j;
                    }
                }
            }
            else if (i == QR_VRESION_M)
            {
                for (int j = 10; j <= 26; ++j)
                {
                    if ((m_icDataCodeWordBit + 7) / 8 <= QR_VersonInfo[j].icDataCodeWord[m_iLevel])
                    {
                        return j;
                    }
                }
            }
            else if (i == QR_VRESION_L)
            {
                for (int j = 27; j <= 40; ++j)
                {
                    if ((m_icDataCodeWordBit + 7) / 8 <= QR_VersonInfo[j].icDataCodeWord[m_iLevel])
                    {
                        return j;
                    }
                }
            }
        }// if ( EncodeSourceData( v_lpsSource, v_icLength, i ) )
    }//for ( i = iVerGroup; i <= QR_VRESION_L; ++i )

    return 0;
}//GetEncodeVersion

/************************************************************************/
/*
函数名：	IsNumeralData
函数说明：	检查字节内容是否是数字
函数参数：	[in]v_c(源字符串中的一个字节)
返回值：	BOOL TRUE:是数字 FALSE:不是数字
*/
BOOL CQR_Encode::IsNumeralData(unsigned char v_c)
{
    if (v_c >= '0' && v_c <= '9')
    {
        return TRUE;
    }

    return FALSE;
}//IsNumeralData

/************************************************************************/
/*
函数名：	IsAlphabetData
函数说明：	检查字节内容是否是单字节字符
函数参数：	[in]v_c(源字符串中的一个字节)
返回值：	BOOL TRUE:是单字节字符 FALSE:不是
*/
BOOL CQR_Encode::IsAlphabetData(unsigned char v_c)
{
    //混合字符模式编码对照表
    if (v_c >= '0' && v_c <= '9')
    {
        return TRUE;
    }

    if (v_c >= 'A' && v_c <= 'Z')
    {
        return TRUE;
    }

    if (v_c == ' ' || v_c == '$' || v_c == '%'
        || v_c == '*' || v_c == '+' || v_c == '-'
        || v_c == '.' || v_c == '/' || v_c == ':'
        )
    {
        return TRUE;
    }

    return FALSE;
}//IsAlphabetData

/************************************************************************/
/*
函数名：	SplitBlocks
函数说明：	检查字符串每一字节是什么内容，按照不同的种类划分成多块
函数参数：	[in]v_lpsSource(源字符串), [in]v_icLength(源字符串长度)
返回值：	无
*/
void CQR_Encode::SplitBlocks(CHAR *v_lpsSource, int v_icLength)
{
    for (int i = 0; i < v_icLength; ++i)
    {
        //m_byBlockMode: 数字|英文|数字|8BIT|数字...
        //m_iBlockLength: 块1字节数|块2字节数|块3字节数...
        BYTE byMode;

        if (IsNumeralData(v_lpsSource[i]))
        {
            byMode = QR_MODE_NUMERAL; //数字
        }
        else if (IsAlphabetData(v_lpsSource[i]))
        {
            byMode = QR_MODE_ALPHABET; //单字节字符
        }
        else
        {
            byMode = QR_MODE_8BIT; //8位二进制
        }

        if (i == 0)
        {
            m_byBlockMode[0] = byMode;
        }

        if (m_byBlockMode[m_icDataBlock] != byMode)
        {
            m_byBlockMode[++m_icDataBlock] = byMode;
        }

        ++m_iBlockLength[m_icDataBlock];
    }
    ++m_icDataBlock;
}//SplitBlocks

/************************************************************************/
/*
函数名：	GetBitLength
函数说明：	计算块数据块位长长度
函数参数：	[in]v_bMode(数据类型), [in]v_icDataCount(块数据个数), [in]v_iVerGroup(版本号组别)
返回值：	int:数据块位长
*/
int CQR_Encode::GetBitLength(BYTE v_bMode, int v_icDataCount, int v_iVerGroup)
{
    int icBits = 0;

    //模式标识符用来帮助解码程序进行匹配,模式标识符是4bit的二进制数，数字编码0001 混合字符编码0010...
    //文本串计数标识符用来存储源内容字符串的长度，文本串长度标识符自身的长度被定义为： 数字: 10bit  混合字符: 9bit    8bit字节码: 8bit   多字节码: 8bit
    //数据长度 = 文本串计数标识符大小 + 数据长度个数计数器大小 + 数据大小
    switch (v_bMode)
    {
    case QR_MODE_NUMERAL:
        //在数字模式下，数据被限制为3个数字一段，压缩为10bit，分成若干段。当数据的长度不足3个数字时，如果只有1个数字则用4bit，如果有2个数字就用7个bit来表示。
        icBits = 4 + g_iIndicatorLen_Numeral[v_iVerGroup] + (10 * (v_icDataCount / 3));
        switch (v_icDataCount % 3)
        {
        case 1:
            icBits += 4;
            break;
        case 2:
            icBits += 7;
            break;
        default: // case 0:
            break;
        }
        break;

    case QR_MODE_ALPHABET:
        //混合字符模式编码，源码被分成两个字符一段，每段的第一个字符乘上45，再用第二个数字相加。因此每段变成了11bit的2进制码，如果字符个数只有1个，则用6bit表示。
        icBits = 4 + g_iIndicatorLen_Alphabet[v_iVerGroup] + (11 * (v_icDataCount / 2)) + (6 * (v_icDataCount % 2));
        break;

    case QR_MODE_8BIT:
        //8bit字节数据不经编码转换直接保存。
        icBits = 4 + g_iIndicatorLen_8Bit[v_iVerGroup] + (8 * v_icDataCount);
        break;
    }

    return icBits;
}//GetBitLength

/************************************************************************/
/*
函数名：	TryCombineBlocks
函数说明：	尝试将数据块合并压缩
函数参数：	[in]v_iVerGroup(版本号组别)
返回值：	无
*/
void CQR_Encode::TryCombineBlocks(int v_iVerGroup)
{
    int icRealBitsLength;
    int icBitsLength;
    int iBlock = 0; //第几个块 []当前块
    while (iBlock < m_icDataBlock - 1)
    {
        int icCombineFrontBLen; //与前面的8BIT块结合的长度
        int icCombineBehindBLen; //与后面的8BIT块结合的长度
        int iCombineMode = 0; //结合方式: -1:与前面的8BIT块结合 0:不处理 1:与后面的8BIT块结合

        //这里做的是：将相邻的 NUM ALP(-NUM-ALP- -ALP-NUM-) 合并为 ALP  
        if ((m_byBlockMode[iBlock] == QR_MODE_NUMERAL  && m_byBlockMode[iBlock + 1] == QR_MODE_ALPHABET)
            || (m_byBlockMode[iBlock] == QR_MODE_ALPHABET && m_byBlockMode[iBlock + 1] == QR_MODE_NUMERAL)
            )
        {
            icRealBitsLength = GetBitLength(m_byBlockMode[iBlock], m_iBlockLength[iBlock], v_iVerGroup) +
                GetBitLength(m_byBlockMode[iBlock + 1], m_iBlockLength[iBlock + 1], v_iVerGroup);

            icBitsLength = GetBitLength(QR_MODE_ALPHABET, m_iBlockLength[iBlock] + m_iBlockLength[iBlock + 1], v_iVerGroup); //之所以这边用 QR_MODE_ALPHABET，是因为最终的相邻的 NUM ALP 都要合并为 ALP，所以考虑都是 ALP 的情况

            if (icRealBitsLength > icBitsLength)
            {
                // -8BIT-[ALP]-NUM- 或 -8BIT-[NUM]-ALP- 情况
                if (iBlock >= 1
                    && m_byBlockMode[iBlock - 1] == QR_MODE_8BIT
                    )
                {
                    icCombineFrontBLen = GetBitLength(QR_MODE_8BIT, m_iBlockLength[iBlock - 1] + m_iBlockLength[iBlock], v_iVerGroup) +
                        GetBitLength(m_byBlockMode[iBlock + 1], m_iBlockLength[iBlock + 1], v_iVerGroup);

                    if (icCombineFrontBLen > icBitsLength + GetBitLength(QR_MODE_8BIT, m_iBlockLength[iBlock - 1], v_iVerGroup))
                    {
                        icCombineFrontBLen = 0;
                    }
                }
                else
                {
                    icCombineFrontBLen = 0;
                }

                // -NUM-[ALP]-8BIT- 或 -ALP-[NUM]-8BIT- 情况
                if (iBlock < m_icDataBlock - 2
                    && m_byBlockMode[iBlock + 2] == QR_MODE_8BIT
                    )
                {
                    icCombineBehindBLen = GetBitLength(m_byBlockMode[iBlock], m_iBlockLength[iBlock], v_iVerGroup) +
                        GetBitLength(QR_MODE_8BIT, m_iBlockLength[iBlock + 1] + m_iBlockLength[iBlock + 2], v_iVerGroup);

                    if (icCombineBehindBLen > icBitsLength + GetBitLength(QR_MODE_8BIT, m_iBlockLength[iBlock + 2], v_iVerGroup))
                    {
                        icCombineBehindBLen = 0;
                    }
                }
                else
                {
                    icCombineBehindBLen = 0;
                }

                if (icCombineFrontBLen != 0 && icCombineBehindBLen != 0)
                {
                    iCombineMode = (icCombineFrontBLen < icCombineBehindBLen) ? -1 : 1;
                }
                else
                {
                    //选择最优的，即最短的
                    iCombineMode = (icCombineFrontBLen != 0) ? -1 : ((icCombineBehindBLen != 0) ? 1 : 0);
                }

                if (iCombineMode != 0)
                {
                    if (iCombineMode == -1)
                    {
                        //与前面的8BIT块结合，合并成一个8BIT块
                        m_iBlockLength[iBlock - 1] += m_iBlockLength[iBlock];

                        //将后面的往前移
                        for (int i = iBlock; i < m_icDataBlock - 1; ++i)
                        {
                            m_byBlockMode[i] = m_byBlockMode[i + 1];
                            m_iBlockLength[i] = m_iBlockLength[i + 1];
                        }
                    }
                    else // if iCombineMode == 1
                    {
                        //与后面的8BIT块结合，合并成一个8BIT块
                        m_byBlockMode[iBlock + 1] = QR_MODE_8BIT;
                        m_iBlockLength[iBlock + 1] += m_iBlockLength[iBlock + 2];

                        //将后面的往前移
                        for (int i = iBlock + 2; i < m_icDataBlock - 1; ++i)
                        {
                            m_byBlockMode[i] = m_byBlockMode[i + 1];
                            m_iBlockLength[i] = m_iBlockLength[i + 1];
                        }
                    }

                    --m_icDataBlock; //块总量-1
                }
                else // if iCombineMode == 0 不结合
                {
                    if (iBlock < m_icDataBlock - 2
                        && m_byBlockMode[iBlock + 2] == QR_MODE_ALPHABET
                        )
                    {
                        //将 数字编码块 与 字符编码块合并
                        m_iBlockLength[iBlock + 1] += m_iBlockLength[iBlock + 2];

                        //将后面的往前移
                        for (int i = iBlock + 2; i < m_icDataBlock - 1; ++i)
                        {
                            m_byBlockMode[i] = m_byBlockMode[i + 1];
                            m_iBlockLength[i] = m_iBlockLength[i + 1];
                        }

                        --m_icDataBlock; //块总量-1
                    }

                    m_byBlockMode[iBlock] = QR_MODE_ALPHABET;
                    m_iBlockLength[iBlock] += m_iBlockLength[iBlock + 1];
                    //将后面的往前移
                    for (int i = iBlock + 1; i < m_icDataBlock - 1; ++i)
                    {
                        m_byBlockMode[i] = m_byBlockMode[i + 1];
                        m_iBlockLength[i] = m_iBlockLength[i + 1];
                    }

                    --m_icDataBlock;

                    if (iBlock >= 1
                        && m_byBlockMode[iBlock - 1] == QR_MODE_ALPHABET
                        )
                    {
                        m_iBlockLength[iBlock - 1] += m_iBlockLength[iBlock];

                        //将后面的往前移
                        for (int i = iBlock; i < m_icDataBlock - 1; ++i)
                        {
                            m_byBlockMode[i] = m_byBlockMode[i + 1];
                            m_iBlockLength[i] = m_iBlockLength[i + 1];
                        }

                        --m_icDataBlock;
                    }
                }// if iCombineMode == 0 不结合

                continue; //目前位置的块重新检查合并
            }// if (icRealBitsLength > icBitsLength)
        }// -ALP-NUM- -NUM-ALP-

        ++iBlock; // 下一块
    }// while ( iBlock < m_icDataBlock - 1 )

    //执行到这里，如果能进行合并的话，就不存在相邻的 -NUM-ALP- -ALP-NUM- 块了，都是用8BIT块隔开的(如果原本存在8BIT块)，不存在相邻的8BIT块
    //这里做的是：把最后一个8BIT块(如果存在8BIT块)之前所有的内容全部合并到最后一个8BIT块中
    iBlock = 0;
    while (iBlock < m_icDataBlock - 1)
    {
        icRealBitsLength = GetBitLength(m_byBlockMode[iBlock], m_iBlockLength[iBlock], v_iVerGroup) +
            GetBitLength(m_byBlockMode[iBlock + 1], m_iBlockLength[iBlock + 1], v_iVerGroup);

        icBitsLength = GetBitLength(QR_MODE_8BIT, m_iBlockLength[iBlock] + m_iBlockLength[iBlock + 1], v_iVerGroup);

        //每个数据长度 = 文本串计数标识符大小 + 数据长度个数计数器大小 + 数据大小,如果数据可以合并的话，文本串计数标识符大小 + 数据长度个数计数器大小就是冗余部分

        // -8BIT-[ALP]-8BIT-ALP- -8BIT-[ALP]-8BIT-NUM- -8BIT-[NUM]-8BIT-NUM- -8BIT-[NUM]-8BIT-ALP-
        if (iBlock >= 1
            && m_byBlockMode[iBlock - 1] == QR_MODE_8BIT
            )
        {
            icBitsLength -= (4 + g_iIndicatorLen_8Bit[v_iVerGroup]);
        }

        // -ALP-[8BIT]-NUM-8BIT- -ALP-[8BIT]-ALP-8BIT- -NUM-[8BIT]-NUM-8BIT- -NUM-[8BIT]-ALP-8BIT-
        if (iBlock < m_icDataBlock - 2
            && m_byBlockMode[iBlock + 2] == QR_MODE_8BIT
            )
        {
            icBitsLength -= (4 + g_iIndicatorLen_8Bit[v_iVerGroup]);
        }

        if (icRealBitsLength > icBitsLength)
        {
            // -8BIT-[ALP]-8BIT-ALP- -8BIT-[ALP]-8BIT-NUM- -8BIT-[NUM]-8BIT-NUM- -8BIT-[NUM]-8BIT-ALP-
            if (iBlock >= 1
                && m_byBlockMode[iBlock - 1] == QR_MODE_8BIT
                )
            {
                m_iBlockLength[iBlock - 1] += m_iBlockLength[iBlock];

                //将后面的往前移
                for (int i = iBlock; i < m_icDataBlock - 1; ++i)
                {
                    m_byBlockMode[i] = m_byBlockMode[i + 1];
                    m_iBlockLength[i] = m_iBlockLength[i + 1];
                }

                --m_icDataBlock;
                --iBlock;
            }

            // -ALP-[8BIT]-NUM-8BIT- -ALP-[8BIT]-ALP-8BIT- -NUM-[8BIT]-NUM-8BIT- -NUM-[8BIT]-ALP-8BIT-
            if (iBlock < m_icDataBlock - 2
                && m_byBlockMode[iBlock + 2] == QR_MODE_8BIT
                )
            {
                m_iBlockLength[iBlock + 1] += m_iBlockLength[iBlock + 2];

                //将后面的往前移
                for (int i = iBlock + 2; i < m_icDataBlock - 1; ++i)
                {
                    m_byBlockMode[i] = m_byBlockMode[i + 1];
                    m_iBlockLength[i] = m_iBlockLength[i + 1];
                }

                --m_icDataBlock;
            }

            m_byBlockMode[iBlock] = QR_MODE_8BIT;
            m_iBlockLength[iBlock] += m_iBlockLength[iBlock + 1];

            //将后面的往前移
            for (int i = iBlock + 1; i < m_icDataBlock - 1; ++i)
            {
                m_byBlockMode[i] = m_byBlockMode[i + 1];
                m_iBlockLength[i] = m_iBlockLength[i + 1];
            }

            --m_icDataBlock;

            //从合并的块的前一块开始重新检查合并
            if (iBlock >= 1)
            {
                --iBlock;
            }

            continue;
        }

        ++iBlock; //下一块
    }
}//TryCombineBlocks

/************************************************************************/
/*
函数名：	SetBitStream
函数说明：	将数据存入数据码数组中
函数参数：	[in]v_iIndex(已写入的偏移量), [in]v_wData(数据), [in]v_icBitLen(数据的位长)
返回值：	int:已写入的偏移量
*/
int CQR_Encode::SetBitStream(int v_iIndex, WORD v_wData, int v_icBitLen)
{
    if (v_iIndex == -1
        || v_iIndex + v_icBitLen > MAX_DATACODEWORD * 8
        )
        return -1;

    for (int i = 0; i < v_icBitLen; ++i)
    {
        if (v_wData & (1 << (v_icBitLen - i - 1)))
        {
            m_byDataCodeWord[(v_iIndex + i) / 8] |= 1 << (7 - ((v_iIndex + i) % 8));
        }
    }

    return v_iIndex + v_icBitLen;
}//SetBitStream

//将混合字符模式编码数据的二进制化
/************************************************************************/
/*
函数名：	AlphabetToBinaly
函数说明：	将字符编码数据二进制化
函数参数：	[in]v_c(字符编码数据)
返回值：	BYTE:字符编码数据的二进制形式
*/
BYTE CQR_Encode::AlphabetToBinaly(unsigned char v_c)
{
    if (v_c >= '0' && v_c <= '9')
    {
        return (unsigned char)(v_c - '0');
    }

    if (v_c >= 'A' && v_c <= 'Z')
    {
        return (unsigned char)(v_c - 'A' + 10);
    }

    if (v_c == ' ')
    {
        return 36;
    }

    if (v_c == '$')
    {
        return 37;
    }

    if (v_c == '%')
    {
        return 38;
    }

    if (v_c == '*')
    {
        return 39;
    }

    if (v_c == '+')
    {
        return 40;
    }

    if (v_c == '-')
    {
        return 41;
    }

    if (v_c == '.')
    {
        return 42;
    }

    if (v_c == '/')
    {
        return 43;
    }

    if (v_c == ':')
    {
        return 44;
    }
}//AlphabetToBinaly

/************************************************************************/
/*
函数名：	CreateDataCodeWord
函数说明：	生成数据码
函数参数：	[in]v_lpsSource(源字符串), [in]v_iVerGroup(版本号组别)
返回值：	无
*/
void CQR_Encode::CreateDataCodeWord(CHAR *v_lpsSource, int v_iVerGroup)
{
    //数据码：数据编码(4bit) + 数据长度标识(Nbit) + 数据内容(Nbit) + 结束符(Nbit,N个0) + [补齐码(Nbit,N>=0)]
    //执行到这里，如果前面的都能合并的话，最后会只剩下一个8BIT块
    int icComplete = 0; //已完成的数量，偏移量
    WORD wBinCode; //分组压缩后的二进制数据

    m_icDataCodeWordBit = 0; //数据码的位长度

    ZeroMemory(m_byDataCodeWord, MAX_DATACODEWORD);

    for (int i = 0; i < m_icDataBlock && m_icDataCodeWordBit != -1; ++i)
    {
        if (m_byBlockMode[i] == QR_MODE_NUMERAL) //数字模式
        {
            //模式编码标识(0001=1)
            m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, 1, 4);

            //数据长度标识
            m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, (WORD)m_iBlockLength[i], g_iIndicatorLen_Numeral[v_iVerGroup]);

            //数据内容
            for (int j = 0; j < m_iBlockLength[i]; j += 3)
            {
                //在数字模式下，数据被限制为3个数字一段，压缩为10bit，分成若干段。当数据的长度不足3个数字时，如果只有1个数字则用4bit，如果有2个数字就用7个bit来表示。
                if (j < m_iBlockLength[i] - 2)
                {
                    wBinCode = (WORD)(((v_lpsSource[icComplete + j] - '0') * 100) +
                        ((v_lpsSource[icComplete + j + 1] - '0') * 10) +
                        (v_lpsSource[icComplete + j + 2] - '0')
                        );
                    m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, wBinCode, 10);
                }
                else if (j == m_iBlockLength[i] - 2)
                {
                    wBinCode = (WORD)(((v_lpsSource[icComplete + j] - '0') * 10) +
                        (v_lpsSource[icComplete + j + 1] - '0')
                        );
                    m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, wBinCode, 7);
                }
                else if (j == m_iBlockLength[i] - 1)
                {
                    wBinCode = (WORD)(v_lpsSource[icComplete + j] - '0');
                    m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, wBinCode, 4);
                }
            }

            icComplete += m_iBlockLength[i];
        }

        else if (m_byBlockMode[i] == QR_MODE_ALPHABET) //字符模式编码
        {
            //模式编码标识(0010=2)
            m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, 2, 4);

            //数据长度标识
            m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, (WORD)m_iBlockLength[i], g_iIndicatorLen_Alphabet[v_iVerGroup]);

            //数据内容
            for (int j = 0; j < m_iBlockLength[i]; j += 2)
            {
                //混合字符模式编码，源码被分成两个字符一段，每段的第一个字符乘上45，再用第二个数字相加。因此每段变成了11bit的2进制码，如果字符个数只有1个，则用6bit表示。
                if (j < m_iBlockLength[i] - 1)
                {
                    wBinCode = (WORD)((AlphabetToBinaly(v_lpsSource[icComplete + j]) * 45) +
                        AlphabetToBinaly(v_lpsSource[icComplete + j + 1])
                        );
                    m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, wBinCode, 11);
                }
                else
                {
                    wBinCode = (WORD)AlphabetToBinaly(v_lpsSource[icComplete + j]);
                    m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, wBinCode, 6);
                }
            }

            icComplete += m_iBlockLength[i];
        }

        else if (m_byBlockMode[i] == QR_MODE_8BIT) //8BIT编码
        {
            //模式编码标识(0100=4)
            m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, 4, 4);

            //数据长度标识
            m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, (WORD)m_iBlockLength[i], g_iIndicatorLen_8Bit[v_iVerGroup]);

            //数据内容
            for (int j = 0; j < m_iBlockLength[i]; ++j)
            {
                //8bit字节数据不经编码转换直接保存，每组8位
                m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, (WORD)v_lpsSource[icComplete + j], 8);
            }

            icComplete += m_iBlockLength[i];
        }
    }
}//CreateDataCodeWord

/************************************************************************/
/*
函数名：	EncodeSourceData
函数说明：	将源字符串内容按照不同种类进行划分成多个块，并尝试合并压缩，然后生成数据码
函数参数：	[in]v_lpsSource(源字符串), [in]v_icLength(源字符串长度), [in]v_iVerGroup(版本号组别)
返回值：	BOOL TRUE:成功 FALSE:失败
*/
BOOL CQR_Encode::EncodeSourceData(CHAR *v_lpsSource, int v_icLength, int v_iVerGroup)
{
    ZeroMemory(m_iBlockLength, sizeof(m_iBlockLength));
    m_icDataBlock = 0;

    //检查输入信息是什么内容，按照不同的种类划分成多块
    SplitBlocks(v_lpsSource, v_icLength);

    //尝试将数据块合并压缩
    TryCombineBlocks(v_iVerGroup);

    //生成数据码
    CreateDataCodeWord(v_lpsSource, v_iVerGroup);

    return (m_icDataCodeWordBit != -1);
}//EncodeSourceData

/************************************************************************/
/*
函数名：	GetRSCodeWord
函数说明：	获取RS纠错码
函数参数：	[in]v_lpbyRSWork(存储纠错码), [in]v_icDataCodeWordNum(数据码个数), [in]v_icRSCodeWordNum(纠错码个数)
返回值：	无
*/
void CQR_Encode::GetRSCodeWord(LPBYTE v_lpbyRSWork, int v_icDataCodeWordNum, int v_icRSCodeWordNum)
{
    for (int i = 0; i < v_icDataCodeWordNum; ++i)
    {
        if (v_lpbyRSWork[0] != 0)
        {
            BYTE nExpFirst = byIntToExp[v_lpbyRSWork[0]];

            for (int j = 0; j < v_icRSCodeWordNum; ++j)
            {
                BYTE nExpElement = (BYTE)(((int)(byRSExp[v_icRSCodeWordNum][j] + nExpFirst)) % 255);
                v_lpbyRSWork[j] = (BYTE)(v_lpbyRSWork[j + 1] ^ byExpToInt[nExpElement]);
            }

            for (int j = v_icRSCodeWordNum; j < v_icDataCodeWordNum + v_icRSCodeWordNum - 1; ++j)
            {
                v_lpbyRSWork[j] = v_lpbyRSWork[j + 1];
            }
        }
        else
        {
            for (int j = 0; j < v_icDataCodeWordNum + v_icRSCodeWordNum - 1; ++j)
            {
                v_lpbyRSWork[j] = v_lpbyRSWork[j + 1];
            }
        }
    }
}//GetRSCodeWord

/************************************************************************/
/*
函数名：	CreateRSCodeWord
函数说明：	生成纠错码
函数参数：	[in]v_iLevel(容错等级)
返回值：	无
*/
void CQR_Encode::CreateRSCodeWord(int v_iLevel)
{
    int iDataCwIndex = 0; //数据码字处理位置
    int iBlockNo = 0; //处理中的块的号码
    int icDataCodeWordMAX = QR_VersonInfo[m_iVersion].icDataCodeWord[v_iLevel];


    //RS码块数
    int icRS1BlockNum = QR_VersonInfo[m_iVersion].RS_BlockInfo1[v_iLevel].icRSBlock;
    int icRS2BlockNum = QR_VersonInfo[m_iVersion].RS_BlockInfo2[v_iLevel].icRSBlock;
    int icRSBlockSumNum = icRS1BlockNum + icRS2BlockNum;

    //数据码个数
    int icRS1DataCWNum = QR_VersonInfo[m_iVersion].RS_BlockInfo1[v_iLevel].icDataCodeWord;
    int icRS2DataCWNum = QR_VersonInfo[m_iVersion].RS_BlockInfo2[v_iLevel].icDataCodeWord;

    for (int i = 0; i < icRS1BlockNum; ++i)
    {
        for (int j = 0; j < icRS1DataCWNum; ++j)
        {
            m_byAllCodeWord[(icRSBlockSumNum * j) + iBlockNo] = m_byDataCodeWord[iDataCwIndex++];
        }

        ++iBlockNo;
    }

    for (int i = 0; i < icRS2BlockNum; ++i)
    {
        for (int j = 0; j < icRS2DataCWNum; ++j)
        {
            if (j < icRS1DataCWNum)
            {
                m_byAllCodeWord[(icRSBlockSumNum * j) + iBlockNo] = m_byDataCodeWord[iDataCwIndex++];
            }
            else
            {
                m_byAllCodeWord[(icRSBlockSumNum * icRS1DataCWNum) + i] = m_byDataCodeWord[iDataCwIndex++];
            }
        }

        ++iBlockNo;
    }

    //纠错码个数
    int icRS1RSCWNum = QR_VersonInfo[m_iVersion].RS_BlockInfo1[v_iLevel].icAllCodeWord - icRS1DataCWNum;
    int icRS2RSCWNum = QR_VersonInfo[m_iVersion].RS_BlockInfo2[v_iLevel].icAllCodeWord - icRS2DataCWNum;

    //RS码字计算
    iDataCwIndex = 0;
    iBlockNo = 0;
    for (int i = 0; i < icRS1BlockNum; ++i)
    {
        ZeroMemory(m_byRSData, sizeof(m_byRSData));
        memmove(m_byRSData, m_byDataCodeWord + iDataCwIndex, icRS1DataCWNum);

        GetRSCodeWord(m_byRSData, icRS1DataCWNum, icRS1RSCWNum);

        for (int j = 0; j < icRS1RSCWNum; ++j)
        {
            m_byAllCodeWord[icDataCodeWordMAX + (icRSBlockSumNum * j) + iBlockNo] = m_byRSData[j];
        }

        iDataCwIndex += icRS1DataCWNum;
        ++iBlockNo;
    }

    for (int i = 0; i < icRS2BlockNum; ++i)
    {
        ZeroMemory(m_byRSData, sizeof(m_byRSData));
        memmove(m_byRSData, m_byDataCodeWord + iDataCwIndex, icRS2DataCWNum);

        GetRSCodeWord(m_byRSData, icRS2DataCWNum, icRS2RSCWNum);

        for (int j = 0; j < icRS2RSCWNum; ++j)
        {
            m_byAllCodeWord[icDataCodeWordMAX + (icRSBlockSumNum * j) + iBlockNo] = m_byRSData[j];
        }

        iDataCwIndex += icRS2DataCWNum;
        ++iBlockNo;
    }
}//CreateRSCodeWord

/************************************************************************/
/*
函数名：	SetFinderPattern
函数说明：	配置位置探测图形
函数参数：	[in]v_ix(图形左上角左边的x值), [in]v_iy(图形左上角左边的y值)
返回值：	无
*/
void CQR_Encode::SetFinderPattern(int v_ix, int v_iy)
{
    static BYTE byPattern[] = {
        0x7f,  //1111111b 
        0x41,  //1000001b 
        0x5d,  //1011101b 
        0x5d,  //1011101b 
        0x5d,  //1011101b 
        0x41,  //1000001b 
        0x7f   //1111111b 
    };

    for (int i = 0; i < 7; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            m_byModuleData[v_ix + j][v_iy + i] = (byPattern[i] & (1 << (6 - j))) ? '\x30' : '\x20'; //0x30 0x20
        }
    }
}//SetFinderPattern

/************************************************************************/
/*
函数名：	SetVersionPattern
函数说明：	配置版本（型号）信息
函数参数：
返回值：	无
备注：		扩展BCH(18, 6)作为纠错码
*/
void CQR_Encode::SetVersionPattern()
{
    //版本7-40都包含了版本信息，没有版本信息的全为0。二维码上两个位置包含了版本信息，它们是冗余的。
    //版本信息固定18位，6X3的矩阵，前6位是版本号，后12位是纠错位。

    if (m_iVersion <= 6)
    {
        return;
    }

    int iVerData = m_iVersion << 12;

    for (int i = 0; i < 6; ++i)
    {
        if (iVerData & (1 << (17 - i)))
        {
            iVerData ^= (0x1f25 << (5 - i));
        }
    }

    iVerData += m_iVersion << 12;

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            m_byModuleData[m_iSymbleSize - 11 + j][i] = m_byModuleData[i][m_iSymbleSize - 11 + j] =
                (iVerData & (1 << (i * 3 + j))) ? '\x30' : '\x20'; //0x30 0x20
        }
    }
}//SetVersionPattern

/************************************************************************/
/*
函数名：	SetAlignmentPattern
函数说明：	配置定位图形
函数参数：	[in]v_ix(图形左上角左边的x值), [in]v_iy(图形左上角左边的y值)
返回值：	无
*/
void CQR_Encode::SetAlignmentPattern(int v_ix, int v_iy)
{
    static BYTE byPattern[] = {
        0x1f,  // 11111b
        0x11,  // 10001b
        0x15,  // 10101b
        0x11,  // 10001b
        0x1f   // 11111b
    };
    int i, j;

    if (m_byModuleData[v_ix][v_iy] & 0x20)
    {
        return; //排除由于重叠的功能模块
    }

    v_ix -= 2; v_iy -= 2; //向左上角的坐标转换

    for (i = 0; i < 5; ++i)
    {
        for (j = 0; j < 5; ++j)
        {
            m_byModuleData[v_ix + j][v_iy + i] = (byPattern[i] & (1 << (4 - j))) ? '\x30' : '\x20'; //0x30 0x20
        }
    }
}//SetAlignmentPattern

/************************************************************************/
/*
函数名：	SetFunctionModule
函数说明：	配置功能图形（位置探测图形，校正图案，定位图案），格式信息，版本信息
函数参数：
返回值：	无
*/
void CQR_Encode::SetFunctionModule()
{
    m_iSymbleSize = m_iVersion * 4 + 17; //尺寸大小（码图矩阵每边字节数）Size = (Version - 1) * 4 + 21

    //位置探测图形，生成一个回字形的图案(定位图案，定位图案与尺寸大小无关，一定是一个 7×7 的矩阵)
    SetFinderPattern(0, 0);
    SetFinderPattern(m_iSymbleSize - 7, 0);
    SetFinderPattern(0, m_iSymbleSize - 7);

    //位置探测图形分隔符，在回字外面加一个空白直角
    for (int i = 0; i < 8; ++i)
    {
        m_byModuleData[i][7] = m_byModuleData[7][i] = '\x20'; //'\x20'，0x20
        m_byModuleData[m_iSymbleSize - 8][i] = m_byModuleData[m_iSymbleSize - 8 + i][7] = '\x20';
        m_byModuleData[i][m_iSymbleSize - 8] = m_byModuleData[7][m_iSymbleSize - 8 + i] = '\x20';
    }

    //格式信息，先占位，没有实际信息
    for (int i = 0; i < 9; ++i)
    {
        m_byModuleData[i][8] = m_byModuleData[8][i] = '\x20';
    }
    for (int i = 0; i < 8; ++i)
    {
        m_byModuleData[m_iSymbleSize - 8 + i][8] = m_byModuleData[8][m_iSymbleSize - 8 + i] = '\x20';
    }

    //版本信息
    SetVersionPattern();

    //校正图形(对齐图案)
    for (int i = 0; i < QR_VersonInfo[m_iVersion].icAlignPoint; ++i)
    {
        SetAlignmentPattern(QR_VersonInfo[m_iVersion].iAlignPoint[i], 6);
        SetAlignmentPattern(6, QR_VersonInfo[m_iVersion].iAlignPoint[i]);

        for (int j = 0; j < QR_VersonInfo[m_iVersion].icAlignPoint; ++j)
        {
            SetAlignmentPattern(QR_VersonInfo[m_iVersion].iAlignPoint[i], QR_VersonInfo[m_iVersion].iAlignPoint[j]);
        }
    }

    //定位图案(QRCode 结构图上面类似斑马线，黑白间隔的那个)
    for (int i = 8; i <= m_iSymbleSize - 9; ++i)
    {
        m_byModuleData[i][6] = (i % 2) == 0 ? '\x30' : '\x20';
        m_byModuleData[6][i] = (i % 2) == 0 ? '\x30' : '\x20';
    }
}//SetFunctionModule

/************************************************************************/
/*
函数名：	SetCodeWordPattern
函数说明：	配置数据和纠错码字
函数参数：
返回值：	无
*/
void CQR_Encode::SetCodeWordPattern()
{
    //结合二维码数据填充图看
    int ix = m_iSymbleSize;
    int iy = m_iSymbleSize - 1;

    int iCoef_x = 1; //x轴位置方向
    int iCoef_y = 1; //y轴位置方向

    for (int i = 0; i < m_icAllCodeWord; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            do
            {
                ix += iCoef_x;
                iCoef_x *= -1;

                if (iCoef_x < 0)
                {
                    iy += iCoef_y;

                    if (iy < 0 || iy == m_iSymbleSize)
                    {
                        iy = (iy < 0) ? 0 : m_iSymbleSize - 1;
                        iCoef_y *= -1;

                        ix -= 2;

                        if (ix == 6) //时序模式
                        {
                            --ix;
                        }
                    }
                }
            } while (m_byModuleData[ix][iy] & 0x20); //排除功能模块

            m_byModuleData[ix][iy] = (m_byAllCodeWord[i] & (1 << (7 - j))) ? '\x02' : '\x00'; //0x02 0x00
        }
    }
}

/************************************************************************/
/*
函数名：	SetMaskingPattern
函数说明：	配置掩模图案
函数参数：	[in]v_iPatternNo(掩模号码)
返回值：	无
*/
void CQR_Encode::SetMaskingPattern(int v_iPatternNo)
{
    for (int i = 0; i < m_iSymbleSize; ++i)
    {
        for (int j = 0; j < m_iSymbleSize; ++j)
        {
            if (!(m_byModuleData[j][i] & 0x20)) //排除功能模块
            {
                BOOL bMask;

                switch (v_iPatternNo)
                {
                case 0:
                    bMask = ((i + j) % 2 == 0);
                    break;

                case 1:
                    bMask = (i % 2 == 0);
                    break;

                case 2:
                    bMask = (j % 3 == 0);
                    break;

                case 3:
                    bMask = ((i + j) % 3 == 0);
                    break;

                case 4:
                    bMask = (((i / 2) + (j / 3)) % 2 == 0);
                    break;

                case 5:
                    bMask = (((i * j) % 2) + ((i * j) % 3) == 0);
                    break;

                case 6:
                    bMask = ((((i * j) % 2) + ((i * j) % 3)) % 2 == 0);
                    break;

                default: //case 7:
                    bMask = ((((i * j) % 3) + ((i + j) % 2)) % 2 == 0);
                    break;
                }

                m_byModuleData[j][i] = (BYTE)((m_byModuleData[j][i] & 0xfe) | (((m_byModuleData[j][i] & 0x02) > 1) ^ bMask));
            }
        }
    }
}//SetMaskingPattern

/////////////////////////////////////////////////////////////////////////////
// CQR_Encode::SetFormatInfoPattern
// 用  途: 版权信息部署
// 参  数: 花纹图案号码
/************************************************************************/
/*
函数名：	SetFormatInfoPattern
函数说明：	配置格式信息
函数参数：	[in]v_iPatternNo(掩码号码)
返回值：	无
*/
void CQR_Encode::SetFormatInfoPattern(int v_iPatternNo)
{
    //格式信息是一个固定 15bits 的信息。5bits 的数据位 + 10bits 纠错位。
    //数据位：2bits 用于表示使用的纠错等级，3bits 用于表示使用的掩模类别
    //纠错位：通过 BCH Code 计算
    //配合图 格式位置信息 看

    int iFormatData;

    switch (m_iLevel)
    {
    case QR_LEVEL_M:
        iFormatData = 0x00; // 00b
        break;

    case QR_LEVEL_L:
        iFormatData = 0x08; // 01b
        break;

    case QR_LEVEL_Q:
        iFormatData = 0x18; // 11b
        break;

    default: // case QR_LEVEL_H:
        iFormatData = 0x10; // 10b
        break;
    }
    iFormatData += v_iPatternNo;

    //获取纠错位
    int iFormatInfo = iFormatData << 10;
    for (int i = 0; i < 5; ++i)
    {
        if (iFormatInfo & (1 << (14 - i)))
        {
            iFormatInfo ^= (0x0537 << (4 - i)); //异或，10100110111b
        }
    }

    iFormatInfo += iFormatData << 10;
    iFormatInfo ^= 0x5412; //异或101010000010010b，减少0值，减少白色块，减少扫描后图像识别的困难(如果出现了大面积的白/黑色块，扫描会困难)

    //配置左上角的格式信息
    for (int i = 0; i <= 5; ++i)
    {
        m_byModuleData[8][i] = (iFormatInfo & (1 << i)) ? '\x30' : '\x20'; //0x30 0x20
    }

    m_byModuleData[8][7] = (iFormatInfo & (1 << 6)) ? '\x30' : '\x20';
    m_byModuleData[8][8] = (iFormatInfo & (1 << 7)) ? '\x30' : '\x20';
    m_byModuleData[7][8] = (iFormatInfo & (1 << 8)) ? '\x30' : '\x20';

    for (int i = 9; i <= 14; ++i)
    {
        m_byModuleData[14 - i][8] = (iFormatInfo & (1 << i)) ? '\x30' : '\x20';
    }

    //配置右下角的格式信息
    for (int i = 0; i <= 7; ++i)
    {
        m_byModuleData[m_iSymbleSize - 1 - i][8] = (iFormatInfo & (1 << i)) ? '\x30' : '\x20';
    }

    m_byModuleData[8][m_iSymbleSize - 8] = '\x30'; //这个位置固定是黑色块

    //配置右上角的格式信息
    for (int i = 8; i <= 14; ++i)
    {
        m_byModuleData[8][m_iSymbleSize - 15 + i] = (iFormatInfo & (1 << i)) ? '\x30' : '\x20';
    }
}//SetFormatInfoPattern

/************************************************************************/
/*
函数名：	CountPenalty
函数说明：	计算罚点记分分值
函数参数：
返回值：	int:罚点记分分值
*/
int CQR_Encode::CountPenalty()
{
    int iPenalty = 0;
    int k;

    //列中相邻的模块的颜色相同
    for (int i = 0; i < m_iSymbleSize; ++i)
    {
        for (int j = 0; j < m_iSymbleSize - 4; ++j)
        {
            int iCount = 1;

            for (k = j + 1; k < m_iSymbleSize; k++)
            {
                if (((m_byModuleData[i][j] & 0x11) == 0) == ((m_byModuleData[i][k] & 0x11) == 0))
                {
                    ++iCount;
                }
                else
                {
                    break;
                }
            }

            if (iCount >= 5)
            {
                iPenalty += 3 + (iCount - 5);
            }

            j = k - 1;
        }
    }

    //行中相邻的模块颜色相同
    for (int i = 0; i < m_iSymbleSize; ++i)
    {
        for (int j = 0; j < m_iSymbleSize - 4; ++j)
        {
            int nCount = 1;

            for (k = j + 1; k < m_iSymbleSize; k++)
            {
                if (((m_byModuleData[j][i] & 0x11) == 0) == ((m_byModuleData[k][i] & 0x11) == 0))
                    ++nCount;
                else
                    break;
            }

            if (nCount >= 5)
            {
                iPenalty += 3 + (nCount - 5);
            }

            j = k - 1;
        }
    }

    //颜色相同的模块块(2×2)
    for (int i = 0; i < m_iSymbleSize - 1; ++i)
    {
        for (int j = 0; j < m_iSymbleSize - 1; ++j)
        {
            if ((((m_byModuleData[i][j] & 0x11) == 0) == ((m_byModuleData[i + 1][j] & 0x11) == 0))
                && (((m_byModuleData[i][j] & 0x11) == 0) == ((m_byModuleData[i][j + 1] & 0x11) == 0))
                && (((m_byModuleData[i][j] & 0x11) == 0) == ((m_byModuleData[i + 1][j + 1] & 0x11) == 0))
                )
            {
                iPenalty += 3;
            }
        }
    }

    //同一列中出现1:1:3:1:1 比率（暗:明:暗:明:暗）的图形
    for (int i = 0; i < m_iSymbleSize; ++i)
    {
        for (int j = 0; j < m_iSymbleSize - 6; ++j)
        {
            if (((j == 0) || (!(m_byModuleData[i][j - 1] & 0x11)))
                && (m_byModuleData[i][j] & 0x11)  // 暗 - 1
                && (!(m_byModuleData[i][j + 1] & 0x11)) // 明 - 1
                && (m_byModuleData[i][j + 2] & 0x11)  // 暗 ┐
                && (m_byModuleData[i][j + 3] & 0x11)  // 暗 │3
                && (m_byModuleData[i][j + 4] & 0x11)  // 暗 ┘
                && (!(m_byModuleData[i][j + 5] & 0x11)) // 明 - 1
                && (m_byModuleData[i][j + 6] & 0x11)  // 暗 - 1
                && ((j == m_iSymbleSize - 7) || (!(m_byModuleData[i][j + 7] & 0x11)))
                )
            {
                //之前或之后出现4个白块
                if (((j < 2 || !(m_byModuleData[i][j - 2] & 0x11))
                    && (j < 3 || !(m_byModuleData[i][j - 3] & 0x11))
                    && (j < 4 || !(m_byModuleData[i][j - 4] & 0x11))
                    )
                    || ((j >= m_iSymbleSize - 8 || !(m_byModuleData[i][j + 8] & 0x11))
                    && (j >= m_iSymbleSize - 9 || !(m_byModuleData[i][j + 9] & 0x11))
                    && (j >= m_iSymbleSize - 10 || !(m_byModuleData[i][j + 10] & 0x11))
                    )
                    )
                {
                    iPenalty += 40;
                }
            }
        }
    }

    //同一行中出现1:1:3:1:1 比率（暗:明:暗:明:暗）的图形
    for (int i = 0; i < m_iSymbleSize; ++i)
    {
        for (int j = 0; j < m_iSymbleSize - 6; ++j)
        {
            if (((j == 0) || (!(m_byModuleData[j - 1][i] & 0x11)))
                && (m_byModuleData[j][i] & 0x11)  // 暗 - 1
                && (!(m_byModuleData[j + 1][i] & 0x11)) // 明 - 1
                && (m_byModuleData[j + 2][i] & 0x11)  // 暗 ┐
                && (m_byModuleData[j + 3][i] & 0x11)  // 暗 │3
                && (m_byModuleData[j + 4][i] & 0x11)  // 暗 ┘
                && (!(m_byModuleData[j + 5][i] & 0x11)) // 明 - 1
                && (m_byModuleData[j + 6][i] & 0x11)  // 暗 - 1
                && ((j == m_iSymbleSize - 7) || (!(m_byModuleData[j + 7][i] & 0x11)))
                )
            {
                //前或后出现4个白块
                if (((j < 2 || !(m_byModuleData[j - 2][i] & 0x11))
                    && (j < 3 || !(m_byModuleData[j - 3][i] & 0x11))
                    && (j < 4 || !(m_byModuleData[j - 4][i] & 0x11))
                    )
                    ||
                    ((j >= m_iSymbleSize - 8 || !(m_byModuleData[j + 8][i] & 0x11))
                    && (j >= m_iSymbleSize - 9 || !(m_byModuleData[j + 9][i] & 0x11))
                    && (j >= m_iSymbleSize - 10 || !(m_byModuleData[j + 10][i] & 0x11))
                    )
                    )
                {
                    iPenalty += 40;
                }
            }
        }
    }

    //整个符号中深色模块的比率
    int iCount = 0;

    for (int i = 0; i < m_iSymbleSize; ++i)
    {
        for (int j = 0; j < m_iSymbleSize; ++j)
        {
            if (!(m_byModuleData[i][j] & 0x11))
            {
                ++iCount;
            }
        }
    }

    iPenalty += (abs(50 - ((iCount * 100) / (m_iSymbleSize * m_iSymbleSize))) / 5) * 10;

    return iPenalty;
}//CountPenalty

/************************************************************************/
/*
函数名：	FormatModule
函数说明：	生成二维码矩阵
函数参数：
返回值：	无
*/
void CQR_Encode::FormatModule()
{
    //初始化符号矩阵存储区
    ZeroMemory(m_byModuleData, sizeof(m_byModuleData));

    //功能图形配置
    SetFunctionModule();

    // 数据模式部署
    SetCodeWordPattern();

    if (m_iMaskingNo == -1)
    {
        //最佳掩模选择
        m_iMaskingNo = 0;

        SetMaskingPattern(m_iMaskingNo);    //掩模
        SetFormatInfoPattern(m_iMaskingNo); //格式信息图案配置

        int iMinPenalty = CountPenalty();

        for (int i = 1; i <= 7; ++i)
        {
            SetMaskingPattern(i);
            SetFormatInfoPattern(i);

            int iPenalty = CountPenalty();

            if (iPenalty < iMinPenalty)
            {
                iMinPenalty = iPenalty;
                m_iMaskingNo = i;
            }
        }
    }

    SetMaskingPattern(m_iMaskingNo);
    SetFormatInfoPattern(m_iMaskingNo);

    //符号矩阵输出：模块模式转换成二进制值
    for (int i = 0; i < m_iSymbleSize; ++i)
    {
        for (int j = 0; j < m_iSymbleSize; ++j)
        {
            m_byModuleData[i][j] = (BYTE)((m_byModuleData[i][j] & 0x11) != 0);
        }
    }
}//FormatModule

/************************************************************************/
/*
函数名：	EncodeData
函数说明：	将字符串转换成QR Code矩阵
函数参数：	[in]v_lpsSource(源字符串), [in]v_iVersion(版本号，默认值:0), [in]v_iLevel(容错等级), [in]v_bAutoExtent(是否自动拓展), [in]v_iMaskingNo(掩模编号), [in]v_icSourceLen(源字符串长度)
返回值：	BOOL TRUE:成功 FALSE:失败
备注：		默认值：v_iVersion = 0, v_iLevel = 1, v_bAutoExtent = TRUE(指定版本号时，如果版本号的容量小于源字符串长度，是否允许换一个高版本号), v_iMaskingNo = -1, v_icSourceLen = 0
*/
BOOL CQR_Encode::EncodeData(LPCTSTR v_lpsSource, int v_iVersion, int v_iLevel, BOOL v_bAutoExtent, int v_iMaskingNo, int v_icSourceLen)
{
    CHAR szUTF8Source[MAX_DATACODEWORD] = { 0 };
#ifdef _UNICODE
    UnicodeToUTF8(v_lpsSource, szUTF8Source);
#else
    ANSIToUTF8(v_lpsSource, szUTF8Source);
#endif

    int icLength = v_icSourceLen > 0 ? v_icSourceLen : strlen(szUTF8Source);
    if (icLength == 0)
    {
        return FALSE; //没有数据 
    }

    m_iLevel = v_iLevel;
    m_iMaskingNo = v_iMaskingNo;

    //获取合适的版本号，将数据进行编码（头部，内容）
    int iEncodeVersion = GetEncodeVersion(v_iVersion, szUTF8Source, icLength);
    if (iEncodeVersion == 0)
    {
        return FALSE; //容量错误
    }

    if (v_iVersion == 0) //没有指定版本号
    {
        m_iVersion = iEncodeVersion; //使用计算值
    }
    else //指定了版本号
    {
        //指定的版本号可以大，不可以小，否则数据存不了
        if (iEncodeVersion <= v_iVersion) //计算值小于指定值
        {
            m_iVersion = v_iVersion; //以指定值为准
        }
        else //计算值大于指定值
        {
            if (v_bAutoExtent) //如允许自动扩展
            {
                m_iVersion = iEncodeVersion; //以计算值为准
            }
            else
            {
                return FALSE; //容量错误
            }
        }
    }

    //附加数据编码尾部
    int icDataCodeWordMAX = QR_VersonInfo[m_iVersion].icDataCodeWord[v_iLevel];
    m_icAllCodeWord = QR_VersonInfo[m_iVersion].icAllCodeWord;
    ZeroMemory(m_byAllCodeWord, m_icAllCodeWord);

    //附加终止码 "0000"，要看数据编码长度的情况是否是8的倍数，不是8的倍数，差多少补多少
    int icTerminater = min(4, (icDataCodeWordMAX * 8) - m_icDataCodeWordBit);
    if (icTerminater > 0)
    {
        m_icDataCodeWordBit = SetBitStream(m_icDataCodeWordBit, 0, icTerminater);
    }

    //附加填充码 "11101100, 00010001"
    BYTE byPaddingCode = 0xec; //11101100
    //( + 7 ) / 8，之所以/8是因为左边的是BIT，右边的是BYTE，要先转换成相同的单位再进行比较
    //+7是与结束符有关， ( + 7) / 8得到的是一个int类型的数据，是数据编码可能的字节长度，假设数据长度是8，正好是8的倍数，这时候就不用填充结束符了
    //是1字节，(8+7)/8还是1字节；如果是6，需要补充2个0，补充完是1字节，(6+7)/8还是1字节；如果是10，需要补充6个0，补充完后是2字节，(10+7)/8还是2字节
    for (int i = (m_icDataCodeWordBit + 7) / 8; i < icDataCodeWordMAX; ++i)
    {
        m_byDataCodeWord[i] = byPaddingCode;
        byPaddingCode = (BYTE)(byPaddingCode == 0xec ? 0x11 : 0xec); //0xec:11101100 0x11:00010001
    }

    //获取纠错码
    CreateRSCodeWord(v_iLevel);

    //生成二维码矩阵
    FormatModule();

    return TRUE;
}//EncodeData


//为了方便二维码生成， 这里提供一个自定义的快捷函数

HBITMAP CreateQrCodeImage(HDC hdc, LPCTSTR strQrCodeText, SIZE * pszBitmap)
{
    CQR_Encode * pEncode = new CQR_Encode();
    SIZE szBmp = { 0 };
    RECT rect = { 0 };
    int index = 0;
    HBITMAP hBitmap = NULL;
    BITMAPINFO bmi = { 0 };
    COLORREF * pDest = NULL;
    COLORREF refColor = NULL;

    if (FALSE == pEncode->EncodeData(strQrCodeText))
    {
        goto FINISH;
    }
    szBmp.cx = szBmp.cy = pEncode->m_iSymbleSize + (QR_MARGIN * 2);
    rect.right = rect.bottom = pEncode->m_iSymbleSize;
    OffsetRect(&rect, QR_MARGIN, QR_MARGIN);

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = szBmp.cx;
    bmi.bmiHeader.biHeight = szBmp.cy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = szBmp.cx * szBmp.cy * sizeof(COLORREF);

    if (NULL == hdc)
    {
        hdc = GetDC(NULL);
    }

    hBitmap = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (LPVOID*)&pDest, NULL, 0);

    if (NULL == hBitmap)
    {
        goto FINISH;
    }

    memset(pDest, 0, bmi.bmiHeader.biSizeImage);

    for (int x = 0; pEncode->m_iSymbleSize > x; x++)
    {
        for (int y = 0; pEncode->m_iSymbleSize > y; y++)
        {
            if (pEncode->m_byModuleData[x][y])
            {
                index = (pEncode->m_iSymbleSize - x + QR_MARGIN / 2) * szBmp.cx + y + QR_MARGIN;

                pDest[index] = 0xFF000000;
            }
        }
    }

    if (pszBitmap) *pszBitmap = szBmp;
FINISH:
    delete pEncode;
    return hBitmap;
}