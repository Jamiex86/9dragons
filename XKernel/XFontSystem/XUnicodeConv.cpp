#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#include "XKernel.h"

#ifdef _DEBUG
#include <crtdbg.h> 
#endif

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

#include "XUnicodeConv.h"

const DWORD			VnStdCharOffset		= 0x10000;
const DWORD			INVALID_STD_CHAR	= 0xFFFFFFFF;
const unsigned char PadChar				= '#';
const unsigned char PadStartQuote		= '\"';
const unsigned char PadEndQuote			= '\"';
const unsigned char PadEllipsis			= '.';

_XWinCP1258Charset		_wincp1258charset;
_XUnicodeCharset		_winunicodecharset;
_XUnicodeCompCharset	_winunicodecompcharset;

WORD _WinCP1258Table[TOTAL_VNCHARS]=
{ 
	0x0041, 0x0061, 0xec41, 0xec61, 0xcc41, 0xcc61, 0xd241, 0xd261, 0xde41, 0xde61, 0xf241, 0xf261, //a
	0x00c2, 0x00e2, 0xecc2, 0xece2, 0xccc2, 0xcce2, 0xd2c2, 0xd2e2, 0xdec2, 0xdee2, 0xf2c2, 0xf2e2, //a^
	0x00c3, 0x00e3, 0xecc3, 0xece3, 0xccc3, 0xcce3, 0xd2c3, 0xd2e3, 0xdec3, 0xdee3, 0xf2c3, 0xf2e3, //a(
	0x0042, 0x0062, 0x0043, 0x0063, 0x0044, 0x0064, //B b C c D d
	0x00d0, 0x00f0, //DD, dd
	0x0045, 0x0065, 0xec45, 0xec65, 0xcc45, 0xcc65, 0xd245, 0xd265, 0xde45, 0xde65, 0xf245, 0xf265, //e
	0x00ca, 0x00ea, 0xecca, 0xecea, 0xccca, 0xccea, 0xd2ca, 0xd2ea, 0xdeca, 0xdeea, 0xf2ca, 0xf2ea, //e^
	0x0046, 0x0066, 0x0047, 0x0067, 0x0048, 0x0068, // F f G g H h
	0x0049, 0x0069, 0xec49, 0xec69, 0xcc49, 0xcc69, 0xd249, 0xd269, 0xde49, 0xde69, 0xf249, 0xf269, //i
	0x004a, 0x006a, 0x004b, 0x006b, 0x004c, 0x006c, 0x004d, 0x006d, 0x004e, 0x006e,  // J j K k L l M m N n
	0x004f, 0x006f, 0xec4f, 0xec6f, 0xcc4f, 0xcc6f, 0xd24f, 0xd26f, 0xde4f, 0xde6f, 0xf24f, 0xf26f, //o
	0x00d4, 0x00f4, 0xecd4, 0xecf4, 0xccd4, 0xccf4, 0xd2d4, 0xd2f4, 0xded4, 0xdef4, 0xf2d4, 0xf2f4, //o^
	0x00d5, 0x00f5, 0xecd5, 0xecf5, 0xccd5, 0xccf5, 0xd2d5, 0xd2f5, 0xded5, 0xdef5, 0xf2d5, 0xf2f5, //o+
	0x0050, 0x0070, 0x0051, 0x0071, 0x0052, 0x0072, 0x0053, 0x0073, 0x0054, 0x0074,                 //P p Q q R r S s T t
	0x0055, 0x0075, 0xec55, 0xec75, 0xcc55, 0xcc75, 0xd255, 0xd275, 0xde55, 0xde75, 0xf255, 0xf275, //u
	0x00dd, 0x00fd, 0xecdd, 0xecfd, 0xccdd, 0xccfd, 0xd2dd, 0xd2fd, 0xdedd, 0xdefd, 0xf2dd, 0xf2fd, //u+
	0x0056, 0x0076, 0x0057, 0x0077, 0x0058, 0x0078, // V v W w X x
	0x0059, 0x0079, 0xec59, 0xec79, 0xcc59, 0xcc79, 0xd259, 0xd279, 0xde59, 0xde79, 0xf259, 0xf279, //y
	0x005a, 0x007a, // Z z
	0x0080, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088,
	0x0089, 0x008A, 0x008B, 0x008C, 0x008E, 0x0091, 0x0092, 0x0093,
	0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B,
	0x009C, 0x009E, 0x009F
};

WORD _WinCP1258PreTable[TOTAL_VNCHARS] =
{ 
	0x0041, 0x0061, 0x00c1, 0x00e1, 0x00c0, 0x00e0, 0xd241, 0xd261, 0xde41, 0xde61, 0xf241, 0xf261, //a
	0x00c2, 0x00e2, 0xecc2, 0xece2, 0xccc2, 0xcce2, 0xd2c2, 0xd2e2, 0xdec2, 0xdee2, 0xf2c2, 0xf2e2, //a^
	0x00c3, 0x00e3, 0xecc3, 0xece3, 0xccc3, 0xcce3, 0xd2c3, 0xd2e3, 0xdec3, 0xdee3, 0xf2c3, 0xf2e3, //a(
	0x0042, 0x0062, 0x0043, 0x0063, 0x0044, 0x0064, //B b C c D d
	0x00d0, 0x00f0, //DD, dd
	0x0045, 0x0065, 0x00c9, 0x00e9, 0x00c8, 0x00e8, 0xd245, 0xd265, 0xde45, 0xde65, 0xf245, 0xf265, //e
	0x00ca, 0x00ea, 0xecca, 0xecea, 0xccca, 0xccea, 0xd2ca, 0xd2ea, 0xdeca, 0xdeea, 0xf2ca, 0xf2ea, //e^
	0x0046, 0x0066, 0x0047, 0x0067, 0x0048, 0x0068, // F f G g H h
	0x0049, 0x0069, 0x00cd, 0x00ed, 0xcc49, 0xcc69, 0xd249, 0xd269, 0xde49, 0xde69, 0xf249, 0xf269, //i
	0x004a, 0x006a, 0x004b, 0x006b, 0x004c, 0x006c, 0x004d, 0x006d, 0x004e, 0x006e,  // J j K k L l M m N n
	0x004f, 0x006f, 0x00d3, 0x00f3, 0xcc4f, 0xcc6f, 0xd24f, 0xd26f, 0xde4f, 0xde6f, 0xf24f, 0xf26f, //o
	0x00d4, 0x00f4, 0xecd4, 0xecf4, 0xccd4, 0xccf4, 0xd2d4, 0xd2f4, 0xded4, 0xdef4, 0xf2d4, 0xf2f4, //o^
	0x00d5, 0x00f5, 0xecd5, 0xecf5, 0xccd5, 0xccf5, 0xd2d5, 0xd2f5, 0xded5, 0xdef5, 0xf2d5, 0xf2f5, //o+
	0x0050, 0x0070, 0x0051, 0x0071, 0x0052, 0x0072, 0x0053, 0x0073, 0x0054, 0x0074,                 //P p Q q R r S s T t
	0x0055, 0x0075, 0x00da, 0x00fa, 0x00d9, 0x00f9, 0xd255, 0xd275, 0xde55, 0xde75, 0xf255, 0xf275, //u
	0x00dd, 0x00fd, 0xecdd, 0xecfd, 0xccdd, 0xccfd, 0xd2dd, 0xd2fd, 0xdedd, 0xdefd, 0xf2dd, 0xf2fd, //u+
	0x0056, 0x0076, 0x0057, 0x0077, 0x0058, 0x0078, // V v W w X x
	0x0059, 0x0079, 0xec59, 0xec79, 0xcc59, 0xcc79, 0xd259, 0xd279, 0xde59, 0xde79, 0xf259, 0xf279, //y
	0x005a, 0x007a, // Z z
	0x0080, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088,
	0x0089, 0x008A, 0x008B, 0x008C, 0x008E, 0x0091, 0x0092, 0x0093,
	0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009A, 0x009B,
	0x009C, 0x009E, 0x009F
};

WORD UnicodeTable[TOTAL_VNCHARS] =
{
	0x0041, 0x0061, 0x00c1, 0x00e1, 0x00c0, 0x00e0, 0x1ea2, 0x1ea3, 0x00c3, 0x00e3, 0x1ea0, 0x1ea1, //a
	0x00c2, 0x00e2, 0x1ea4, 0x1ea5, 0x1ea6, 0x1ea7, 0x1ea8, 0x1ea9, 0x1eaa, 0x1eab, 0x1eac, 0x1ead, //a^
	0x0102, 0x0103, 0x1eae, 0x1eaf, 0x1eb0, 0x1eb1, 0x1eb2, 0x1eb3, 0x1eb4, 0x1eb5, 0x1eb6, 0x1eb7, //a(
	0x0042, 0x0062, 0x0043, 0x0063, 0x0044, 0x0064,                                                 //B b C c D d
	0x0110, 0x0111,                                                                                 // DD, dd
	0x0045, 0x0065, 0x00c9, 0x00e9, 0x00c8, 0x00e8, 0x1eba, 0x1ebb, 0x1ebc, 0x1ebd, 0x1eb8, 0x1eb9, //e
	0x00ca, 0x00ea, 0x1ebe, 0x1ebf, 0x1ec0, 0x1ec1, 0x1ec2, 0x1ec3, 0x1ec4, 0x1ec5, 0x1ec6, 0x1ec7, //e^
	0x0046, 0x0066, 0x0047, 0x0067, 0x0048, 0x0068,                                                 // F f G g H h
	0x0049, 0x0069, 0x00cd, 0x00ed, 0x00cc, 0x00ec, 0x1ec8, 0x1ec9, 0x0128, 0x0129, 0x1eca, 0x1ecb, //i
	0x004a, 0x006a, 0x004b, 0x006b, 0x004c, 0x006c, 0x004d, 0x006d, 0x004e, 0x006e,                 // J j K k L l M m N n
	0x004f, 0x006f, 0x00d3, 0x00f3, 0x00d2, 0x00f2, 0x1ece, 0x1ecf, 0x00d5, 0x00f5, 0x1ecc, 0x1ecd, //o
	0x00d4, 0x00f4, 0x1ed0, 0x1ed1, 0x1ed2, 0x1ed3, 0x1ed4, 0x1ed5, 0x1ed6, 0x1ed7, 0x1ed8, 0x1ed9, //o^
	0x01a0, 0x01a1, 0x1eda, 0x1edb, 0x1edc, 0x1edd, 0x1ede, 0x1edf, 0x1ee0, 0x1ee1, 0x1ee2, 0x1ee3, //o+
	0x0050, 0x0070, 0x0051, 0x0071, 0x0052, 0x0072, 0x0053, 0x0073, 0x0054, 0x0074,                 //P p Q q R r S s T t
	0x0055, 0x0075, 0x00da, 0x00fa, 0x00d9, 0x00f9, 0x1ee6, 0x1ee7, 0x0168, 0x0169, 0x1ee4, 0x1ee5, //u
	0x01af, 0x01b0, 0x1ee8, 0x1ee9, 0x1eea, 0x1eeb, 0x1eec, 0x1eed, 0x1eee, 0x1eef, 0x1ef0, 0x1ef1, //u+
	0x0056, 0x0076, 0x0057, 0x0077, 0x0058, 0x0078,                                                 // V v W w X x
	0x0059, 0x0079, 0x00dd, 0x00fd, 0x1ef2, 0x1ef3, 0x1ef6, 0x1ef7, 0x1ef8, 0x1ef9, 0x1ef4, 0x1ef5, //y
	0x005a, 0x007a,                                                                                // Z z
	// Symbols that have different code points in Unicode and Western charsets
	0x20AC, 0x20A1, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6,
	0x2030, 0x0160, 0x2039, 0x0152, 0x017D, 0x2018, 0x2019, 0x201C,
	0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 
	0x0153, 0x017E, 0x0178
};

DWORD UnicodeComposite[TOTAL_VNCHARS] = 
{ 
	0x00000041, 0x00000061, 0x03010041, 0x03010061, 0x03000041, 0x03000061, //a
	0x03090041, 0x03090061, 0x03030041, 0x03030061, 0x03230041, 0x03230061, //a

	0x000000c2, 0x000000e2, 0x030100c2, 0x030100e2, 0x030000c2, 0x030000e2, 
	0x030900c2, 0x030900e2, 0x030300c2, 0x030300e2, 0x032300c2, 0x032300e2, //a^

	0x00000102, 0x00000103, 0x03010102, 0x03010103, 0x03000102, 0x03000103,
	0x03090102, 0x03090103, 0x03030102, 0x03030103, 0x03230102, 0x03230103, //a(

	0x0042, 0x0062, 0x0043, 0x0063, 0x0044, 0x0064, //B b C c D d
	0x0110, 0x0111, //0x00d1, 0x00f1, //DD, dd

	0x00000045, 0x00000065, 0x03010045, 0x03010065, 0x03000045, 0x03000065, 
	0x03090045, 0x03090065, 0x03030045, 0x03030065, 0x03230045, 0x03230065, //e

	0x000000ca, 0x000000ea, 0x030100ca, 0x030100ea, 0x030000ca, 0x030000ea,
	0x030900ca, 0x030900ea, 0x030300ca, 0x030300ea, 0x032300ca, 0x032300ea, //e^

	0x0046, 0x0066, 0x0047, 0x0067, 0x0048, 0x0068, // F f G g H h

	0x00000049, 0x00000069, 0x03010049, 0x03010069, 0x03000049, 0x03000069,
	0x03090049, 0x03090069, 0x03030049, 0x03030069, 0x03230049, 0x03230069, //i

	0x004a, 0x006a, 0x004b, 0x006b, 0x004c, 0x006c, 0x004d, 0x006d, 0x004e, 0x006e,  // J j K k L l M m N n

	0x0000004f, 0x0000006f, 0x0301004f, 0x0301006f, 0x0300004f, 0x0300006f, 
	0x0309004f, 0x0309006f, 0x0303004f, 0x0303006f, 0x0323004f, 0x0323006f, //o

	0x000000d4, 0x000000f4, 0x030100d4, 0x030100f4, 0x030000d4, 0x030000f4, 
	0x030900d4, 0x030900f4, 0x030300d4, 0x030300f4, 0x032300d4, 0x032300f4, //o^

	0x000001a0, 0x000001a1, 0x030101a0, 0x030101a1, 0x030001a0, 0x030001a1,
	0x030901a0, 0x030901a1, 0x030301a0, 0x030301a1, 0x032301a0, 0x032301a1, //o+

	0x0050, 0x0070, 0x0051, 0x0071, 0x0052, 0x0072, 0x0053, 0x0073, 0x0054, 0x0074,  //P p Q q R r S s T t

	0x00000055, 0x00000075, 0x03010055, 0x03010075, 0x03000055, 0x03000075,
	0x03090055, 0x03090075, 0x03030055, 0x03030075, 0x03230055, 0x03230075, //u

	0x000001af, 0x000001b0, 0x030101af, 0x030101b0, 0x030001af, 0x030001b0,
	0x030901af, 0x030901b0, 0x030301af, 0x030301b0, 0x032301af, 0x032301b0, //u+

	0x0056, 0x0076, 0x0057, 0x0077, 0x0058, 0x0078, // V v W w X x

	0x00000059, 0x00000079, 0x03010059, 0x03010079, 0x03000059, 0x03000079,
	0x03090059, 0x03090079, 0x03030059, 0x03030079, 0x03230059, 0x03230079, //y
	0x005a, 0x007a, // Z z
	// Symbols that have different code points in Unicode and Western charsets
	0x20AC, 0x20A1, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6,
	0x2030, 0x0160, 0x2039, 0x0152, 0x017D, 0x2018, 0x2019, 0x201C,
	0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 
	0x0153, 0x017E, 0x0178
};


int StdVnNoTone[TOTAL_VNCHARS] = 
{
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, //a [A=0]
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, //a^
	0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, //a(
	36, 37, 38, 39, 40, 41, // bcd [D=40, d=41]
	40, 41,                 // DD dd [mapped to D, d]
	44, 45, 44, 45, 44, 45, 44, 45, 44, 45, 44, 45, // 3: e [E = 44]
	44, 45, 44, 45, 44, 45, 44, 45, 44, 45, 44, 45, // 4: e^
	68, 69, 70, 71, 72, 73, // fgh
	74, 75, 74, 75, 74, 75, 74, 75, 74, 75, 74, 75, // 5: i
	86, 87, 88, 89, 90, 91, 92, 93, 94, 95,  //jklmn
	96, 97, 96, 97, 96, 97, 96, 97, 96, 97, 96, 97, // 6: o [o=96]
	96, 97, 96, 97, 96, 97, 96, 97, 96, 97, 96, 97, // 7: o^
	96, 97, 96, 97, 96, 97, 96, 97, 96, 97, 96, 97, // 8: o+
	132, 133, 134, 135, 136, 137, 138, 139, 140, 141, // pqrst
	142, 143, 142, 143, 142, 143, 142, 143, 142, 143, 142, 143, // 9: u [U=142]
	142, 143, 142, 143, 142, 143, 142, 143, 142, 143, 142, 143, //10: u+ 
	166, 167, 168, 169, 170, 171, //vwx
	172, 173, 172, 173, 172, 173, 172, 173, 172, 173, 172, 173, //11: y [Y=172]
	184, 185, // z
	186, 187, 188, 189, 190, 191, 192, 193,
	194, 195, 196, 197, 198, 199, 200, 201,
	202, 203, 204, 205, 206, 207, 208, 209,
	210, 211, 212
};


int wideCharCompare(const void *ele1, const void *ele2)
{
	WORD ch1 = LOWORD(*((DWORD *)ele1));
	WORD ch2 = LOWORD(*((DWORD *)ele2));
	return (ch1 == ch2)? 0 : ((ch1 > ch2)? 1 : -1);
}

int uniCompInfoCompare(const void *ele1, const void *ele2)
{
	DWORD ch1 = ((_XUniCompCharInfo*)ele1)->compChar;
	DWORD ch2 = ((_XUniCompCharInfo*)ele2)->compChar;
	return (ch1 == ch2)? 0 : ((ch1 > ch2)? 1 : -1);
}

#define IS_ODD(x) (x & 1)
#define IS_EVEN(x) (!(x & 1))

unsigned int StdVnToUpper(unsigned int  ch)
{
	if (ch >= VnStdCharOffset && 
		ch<(VnStdCharOffset + TOTAL_ALPHA_VNCHARS) && 
		IS_ODD(ch))
		ch -= 1;
	return ch;
}

//----------------------------------------
unsigned int  StdVnToLower(unsigned int  ch)
{
	if (ch >= VnStdCharOffset && 
		ch<(VnStdCharOffset + TOTAL_ALPHA_VNCHARS) && 
		IS_EVEN(ch))
		ch += 1;
	return ch;
}

//----------------------------------------
unsigned int  StdVnRemoveTone(unsigned int  ch)
{
	if (ch >= VnStdCharOffset && ch<VnStdCharOffset+TOTAL_VNCHARS)
		ch = VnStdCharOffset + StdVnNoTone[ch-VnStdCharOffset];
	return ch;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// _XStringBIStream

_XStringBIStream::_XStringBIStream(BYTE *data, int len)
{
	m_data = m_current = data;
	m_len = m_left = len;
	if (len == -1)
		m_eos = (*data == 0);
	else
		m_eos = (len <= 0);	
}

int _XStringBIStream::eos()
{
	return m_eos;
}

int _XStringBIStream::getNext(BYTE & b)
{
	if (m_eos)
		return 0;
	b = *m_current++;
	if (m_len == -1) 
	{
		m_eos = (b == 0);
	}
	else 
	{
		m_left--;
		m_eos = (m_left <= 0);
	}

	return 1;
}

int _XStringBIStream::unget(BYTE b)
{
	if (m_current != m_data) 
	{
		*--m_current = b;
		m_eos = 0;
		if (m_len != -1)
			m_left++;
	}

	return 1;
}

int _XStringBIStream::getNextW(WORD & w)
{
	if (m_eos) return 0;
	w = *((WORD *)m_current);
	m_current += 2;
	if (m_len == -1)
		m_eos = (w == 0);
	else 
	{
		m_left -= 2;
		m_eos = (m_left <= 0);
	}
	return 1;
}

int _XStringBIStream::peekNext(BYTE & b)
{
	if (m_eos)
		return 0;
	b = *m_current;
	return 1;
}

int _XStringBIStream::peekNextW(WORD & w)
{
	if (m_eos)
		return 0;
	w = *((WORD *)m_current);
	return 1;
}

void _XStringBIStream::reopen()
{
	m_current = m_data;
	m_left = m_len;
	if (m_len == -1)
		m_eos = (m_data == 0);
	else
		m_eos = (m_len <= 0);	
}

int _XStringBIStream::close()
{
	return 1;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// _XStringBOStream

_XStringBOStream::_XStringBOStream(BYTE *buf, int len)
{
	m_current = m_buf = buf;
	m_len = len;
	m_out = 0;
	m_bad = 0;
}

int _XStringBOStream::putB(BYTE b)
{
	m_out++;
	
	if (m_bad)
		return 0;
	
	if (m_out <= m_len) 
	{
		*m_current++ = b;
		return 1;
	}
	m_bad = 1;
	return 0;
}

int _XStringBOStream::putW(WORD w)
{
	m_out += 2;
	
	if (m_bad)
		return 0;
	
	if (m_out <= m_len) 
	{
		*((WORD *)m_current) = w;
		m_current += 2;
		return 1;
	}
	
	m_bad = 1;

	return 0;
}

int _XStringBOStream::puts(const char *s, int size)
{
	if (size == -1) 
	{
		while (*s) 
		{
			m_out++;
			if (m_out <= m_len) 
				*m_current++ = *s;
			s++;
		}

		if (!m_bad && m_out > m_len)
			m_bad = 1;

		return (!m_bad);
	}

	int n;
	if (!m_bad && m_out <= m_len) 
	{
		n = m_len - m_out;

		if (n>size)
			n = size;

		memcpy(m_current, s, n);

		m_current += n;
	}

	m_out += size;

	if (!m_bad && m_out > m_len)
		m_bad = 1;

	return (!m_bad);
}

void _XStringBOStream::reopen()
{
	m_current = m_buf;
	m_out = 0;
	m_bad = 0;
}

int _XStringBOStream::isOK()
{
	return !m_bad;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// _XWinCP1258Charset

_XWinCP1258Charset::_XWinCP1258Charset()
{
	m_toDoubleChar = _WinCP1258Table;
	memset(m_stdMap, 0, 256*sizeof(WORD));

	// encode composite chars
	int i=0;for (; i<TOTAL_VNCHARS; i++) 
	{
		if (_WinCP1258Table[i] >> 8) // a 2-byte character
			m_stdMap[_WinCP1258Table[i] >> 8] = 0xFFFF; //INVALID_STD_CHAR;
		else if (m_stdMap[_WinCP1258Table[i]] == 0)
			m_stdMap[_WinCP1258Table[i]] = i+1;

		m_vnChars[i] = (i << 16) + _WinCP1258Table[i]; // high word is used for StdChar index
	}

	m_totalChars = TOTAL_VNCHARS;

	//add precomposed chars to the table
	int k;
	for (k=0, i=TOTAL_VNCHARS; k<TOTAL_VNCHARS; k++)
		if (_WinCP1258PreTable[k] != _WinCP1258Table[k]) 
		{
			if (_WinCP1258PreTable[k] >> 8) // a 2-byte character
				m_stdMap[_WinCP1258PreTable[k] >> 8] = 0xFFFF; //INVALID_STD_CHAR;
			else if (m_stdMap[_WinCP1258PreTable[k]] == 0)
				m_stdMap[_WinCP1258PreTable[k]] = k+1;

			m_vnChars[i] = (k << 16) + _WinCP1258PreTable[k];
			m_totalChars++;
			i++;
		}

		qsort(m_vnChars, m_totalChars, sizeof(DWORD), wideCharCompare);
}


//////////////////////////////////////////////////////////////////
// This fuction is basically the same as that of DoubleByteCharset
// with m_totalChars is used instead of constant TOTAL_VNCHARS
int _XWinCP1258Charset::nextInput(_XStringBIStream & is, DWORD & stdChar, int & bytesRead)
{
	unsigned char ch;

	// read first byte
	bytesRead = 0;
	if (!is.getNext(ch))
		return 0;
	bytesRead = 1;
	stdChar = m_stdMap[ch];
	if (stdChar == 0)
		stdChar = ch;
	else if (stdChar == 0xFFFF)
		stdChar = INVALID_STD_CHAR;
	else {
		stdChar += VnStdCharOffset - 1;
		BYTE hi;
		if (is.peekNext(hi) && hi > 0) 
		{
			//test if a double-byte character is encountered
			DWORD key = MAKEWORD(ch,hi);
			DWORD *pChar = (DWORD *)bsearch(&key, m_vnChars, m_totalChars, sizeof(DWORD), wideCharCompare);
			
			if (pChar) 
			{
				stdChar = VnStdCharOffset + HIWORD(*pChar);
				bytesRead = 2;
				is.getNext(hi);
			}
		}
	}
	return 1;
}

//////////////////////////////////////////////////////////////////
// This fuction is exactly the same as that of DoubleByteCharset
int _XWinCP1258Charset::putChar(_XStringBOStream & os, DWORD stdChar, int & outLen)
{
	int ret;
	if (stdChar	>= VnStdCharOffset) 
	{
		WORD wCh = m_toDoubleChar[stdChar-VnStdCharOffset];

		if (wCh & 0xFF00) {
			outLen = 2;
			os.putB((BYTE)(wCh & 0x00FF));
			os.putB((BYTE)(wCh >> 8));
		}
		else 
		{
			unsigned char b = (unsigned char)wCh;
			if (m_stdMap[b] == 0xFFFF)
				b = PadChar;
			outLen = 1;
			os.putB(b);
		}
	}
	else 
	{
		if (stdChar > 255 || m_stdMap[stdChar]) 
		{
			outLen = 1;
			ret = os.putB((BYTE)PadChar);
		}
		else 
		{
			outLen = 1;
			ret = os.putB((BYTE)stdChar);
		}
	}
	return ret;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// _XUnicodeCharset

_XUnicodeCharset::_XUnicodeCharset()
{
	DWORD i;
	m_toUnicode = UnicodeTable;
	
	for (i=0; i<TOTAL_VNCHARS; i++)
		m_vnChars[i] = (i << 16) + UnicodeTable[i]; // high word is used for index

	qsort(m_vnChars, TOTAL_VNCHARS, sizeof(DWORD), wideCharCompare);
}

int _XUnicodeCharset::nextInput(_XStringBIStream & is, DWORD & stdChar, int & bytesRead)
{
	WORD uniCh;

	if (!is.getNextW(uniCh)) 
	{
		bytesRead = 0;
		return 0;
	}

	bytesRead = sizeof(WORD);
	DWORD key = uniCh;
	DWORD *pChar = (DWORD *)bsearch(&key, m_vnChars, TOTAL_VNCHARS, sizeof(DWORD), wideCharCompare);
	
	if (pChar)
		stdChar = VnStdCharOffset + HIWORD(*pChar);
	else
		stdChar = uniCh;
	
	return 1;
}

int _XUnicodeCharset::putChar(_XStringBOStream & os, DWORD stdChar, int & outLen)
{
	outLen = sizeof(WORD);
	
	return os.putW((stdChar >= VnStdCharOffset)? m_toUnicode[stdChar-VnStdCharOffset] : (WORD)stdChar);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// _XUnicodeCompCharset

_XUnicodeCompCharset::_XUnicodeCompCharset()
{
	m_uniCompChars = UnicodeComposite;
	m_totalChars = 0;
	int i=0;for (; i<TOTAL_VNCHARS; i++) 
	{
		m_info[i].compChar = UnicodeComposite[i];
		m_info[i].stdIndex = i;
		m_totalChars++;
	}
	
	int k;
	for (k=0, i=TOTAL_VNCHARS; k<TOTAL_VNCHARS; k++)
	{
		if (UnicodeTable[k] != UnicodeComposite[k]) 
		{
			m_info[i].compChar = UnicodeTable[k];
			m_info[i].stdIndex = k;
			m_totalChars++;
			i++;
		}
	}
	
	qsort(m_info, m_totalChars, sizeof(_XUniCompCharInfo), uniCompInfoCompare);
}

int _XUnicodeCompCharset::nextInput(_XStringBIStream & is, DWORD & stdChar, int & bytesRead)
{
	// read first char
	
	_XUniCompCharInfo key;
	WORD w;
	if (!is.getNextW(w)) 
	{
		bytesRead = 0;
		return 0;
	}
	key.compChar = w;
	bytesRead = 2;
	
	_XUniCompCharInfo *pInfo = (_XUniCompCharInfo *)bsearch(&key, m_info, m_totalChars, sizeof(_XUniCompCharInfo), uniCompInfoCompare);
	if (!pInfo)
	{
		stdChar = key.compChar;
	}
	else 
	{
		stdChar = pInfo->stdIndex + VnStdCharOffset;
		if (is.peekNextW(w)) 
		{
			DWORD hi = w;
			
			if (hi > 0) 
			{
				key.compChar += hi << 16;
				pInfo = (_XUniCompCharInfo *)bsearch(&key, m_info, m_totalChars, sizeof(_XUniCompCharInfo), uniCompInfoCompare);
				if (pInfo) 
				{
					stdChar = pInfo->stdIndex + VnStdCharOffset;
					bytesRead += 2;
					is.getNextW(w);
				}
			}
		}
	}

	return 1;
}

int _XUnicodeCompCharset::putChar(_XStringBOStream & os, DWORD stdChar, int & outLen)
{
	int ret;
	if (stdChar	>= VnStdCharOffset) 
	{
		DWORD uniCompCh = m_uniCompChars[stdChar-VnStdCharOffset];
		WORD lo = LOWORD(uniCompCh);
		WORD hi = HIWORD(uniCompCh);
		outLen = 2;
		ret = os.putW(lo);
		
		if (hi > 0) 
		{
			outLen += 2;
			ret = os.putW(hi);
		}
	}
	else 
	{
		outLen = 2;
		ret = os.putW((WORD)stdChar);
	}
	return ret;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Convert function

int _XGenConvert(_XStringBIStream & input, _XStringBOStream & output, int ConvertType)
{
	DWORD stdChar;
	int bytesRead, bytesWritten;
	
	int ret = 1;
	while (!input.eos()) 
	{
		if (_wincp1258charset.nextInput(input, stdChar, bytesRead)) 
		{
			if (stdChar != INVALID_STD_CHAR) 
			{
				if (ConvertType&CONV_LOWERCASE)
					stdChar = StdVnToLower(stdChar);
				else if (ConvertType&CONV_UPPERCASE)
					stdChar = StdVnToUpper(stdChar);
				
				if (ConvertType&CONV_STRIPACCENT)
					stdChar = StdVnRemoveTone(stdChar);

				ret = _winunicodecharset.putChar(output, stdChar, bytesWritten);
			}
		}
		else break;
	}
	
	return (ret? 0 : _XVNCONV_OUT_OF_MEMORY);
}


int _XCP1258ToUniCode(BYTE *input, BYTE *output, int & inLen, int & maxOutLen, int option)
{
	int ret = -1;

	if (inLen != -1 && inLen < 0) // invalid inLen
		return ret;
	
	_XStringBIStream is(input, inLen);
	_XStringBOStream os(output, maxOutLen);
	
	ret = _XGenConvert(is, os, option);
	maxOutLen = os.getOutBytes();
	inLen = is.left();
	
	return ret;
}


int _XGenConvert_UnicodeToCP1258(_XStringBIStream & input, _XStringBOStream & output, int ConvertType)
{
	DWORD stdChar;
	int bytesRead, bytesWritten;
	
	int ret = 1;
	while (!input.eos()) 
	{
		if (_winunicodecharset.nextInput(input, stdChar, bytesRead)) 
		{
			if (stdChar != INVALID_STD_CHAR) 
			{
				if (ConvertType&CONV_LOWERCASE)
					stdChar = StdVnToLower(stdChar);
				else if (ConvertType&CONV_UPPERCASE)
					stdChar = StdVnToUpper(stdChar);
				
				if (ConvertType&CONV_STRIPACCENT)
					stdChar = StdVnRemoveTone(stdChar);
				
				ret = _wincp1258charset.putChar(output, stdChar, bytesWritten);
			}
		}
		else break;
	}
	
	return (ret? 0 : _XVNCONV_OUT_OF_MEMORY);
}

int _XUniCodeToCP1258(BYTE *input, BYTE *output, int & inLen, int & maxOutLen, int option)
{
	int ret = -1;
	
	if (inLen != -1 && inLen < 0) // invalid inLen
		return ret;
	
	_XStringBIStream is(input, inLen);
	_XStringBOStream os(output, maxOutLen);
	
	ret = _XGenConvert_UnicodeToCP1258(is, os, option);
	maxOutLen = os.getOutBytes();
	inLen = is.left();
	
	return ret;
}


int _XGenConvert_UnicodeCompToCP1258(_XStringBIStream & input, _XStringBOStream & output, int ConvertType)
{
	DWORD stdChar;
	int bytesRead, bytesWritten;
	
	int ret = 1;
	while (!input.eos()) 
	{
		if (_winunicodecompcharset.nextInput(input, stdChar, bytesRead)) 
		{
			if (stdChar != INVALID_STD_CHAR) 
			{
				if (ConvertType&CONV_LOWERCASE)
					stdChar = StdVnToLower(stdChar);
				else if (ConvertType&CONV_UPPERCASE)
					stdChar = StdVnToUpper(stdChar);
				
				if (ConvertType&CONV_STRIPACCENT)
					stdChar = StdVnRemoveTone(stdChar);

				ret = _wincp1258charset.putChar(output, stdChar, bytesWritten);
			}
		}
		else break;
	}
	
	return (ret? 0 : _XVNCONV_OUT_OF_MEMORY);
}

int _XUniCodeCompToCP1258(BYTE *input, BYTE *output, int & inLen, int & maxOutLen, int option)
{
	int ret = -1;
	
	if (inLen != -1 && inLen < 0) // invalid inLen
		return ret;
	
	_XStringBIStream is(input, inLen);
	_XStringBOStream os(output, maxOutLen);
	
	ret = _XGenConvert_UnicodeCompToCP1258(is, os, option);
	maxOutLen = os.getOutBytes();
	inLen = is.left();
	
	return ret;
}



int _XGenConvert_CP1258ToCP1258(_XStringBIStream & input, _XStringBOStream & output, int ConvertType)
{
	DWORD stdChar;
	int bytesRead, bytesWritten;
	
	int ret = 1;
	while (!input.eos()) 
	{
		if (_wincp1258charset.nextInput(input, stdChar, bytesRead)) 
		{
			if (stdChar != INVALID_STD_CHAR) 
			{
				if (ConvertType&CONV_LOWERCASE)
					stdChar = StdVnToLower(stdChar);
				else if (ConvertType&CONV_UPPERCASE)
					stdChar = StdVnToUpper(stdChar);
				
				if (ConvertType&CONV_STRIPACCENT)
					stdChar = StdVnRemoveTone(stdChar);
				
				ret = _wincp1258charset.putChar(output, stdChar, bytesWritten);
			}
		}
		else break;
	}
	
	return (ret? 0 : _XVNCONV_OUT_OF_MEMORY);
}

int _XCP1258Conv(BYTE *input, BYTE *output, int & inLen, int & maxOutLen, int option)
{
	int ret = -1;
	
	if (inLen != -1 && inLen < 0) // invalid inLen
		return ret;
	
	_XStringBIStream is(input, inLen);
	_XStringBOStream os(output, maxOutLen);
	
	ret = _XGenConvert_CP1258ToCP1258(is, os, option);
	maxOutLen = os.getOutBytes();
	inLen = is.left();
	
	return ret;
}