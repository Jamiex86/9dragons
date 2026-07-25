// XFont.cpp: implementation of the _XFont class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#include "XKernel.h"
#include "XUnicodeConv.h"

#ifdef _DEBUG
#include <crtdbg.h> 
#endif

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

extern _XTextureManager g_MainInterfaceTextureArchive;

_pixfmt g_pixfmt[] =  
{
	{ D3DFMT_R5G6B5, 2, 3, 11, 2, 5, 3, 0, 8, 0 },
	{ D3DFMT_R8G8B8, 3, 0, 16, 0, 8, 0, 0, 8, 0 },
	{ D3DFMT_A1R5G5B5, 2, 3, 10, 3, 5, 3, 0, 7, 15 },
	{ D3DFMT_X1R5G5B5, 2, 3, 10, 3, 5, 3, 0, 8, 16 },
	{ D3DFMT_R3G3B2, 1, 5, 5, 5, 2, 6, 0, 8, 0 },
	{ D3DFMT_A4R4G4B4, 2, 4, 8, 4, 4, 4, 0, 4, 12 },
	{ D3DFMT_A8R8G8B8, 4, 0, 16, 0, 8, 0, 0, 0, 24 },
	{ D3DFMT_A8R3G3B2, 2, 5, 5, 5, 2, 6, 0, 0, 8 },
	{ D3DFMT_X8R8G8B8, 4, 0, 16, 0, 8, 0, 0, 8, 24 }
} ;

TCHAR g_LastErrorCheckedCharacterName[3];

_XCREATECHARACTERRESULT	_XCheckUsableCharacterName_Unicode( wchar_t* pszName, wchar_t& invalidcharacter )
{
	if(g_LanguageType != _XLANGUAGE_TYPE_TAIWANESE && g_LanguageType != _XLANGUAGE_TYPE_ENGLISH && 
	   g_LanguageType != _XLANGUAGE_TYPE_JAPANESE  && g_LanguageType != _XLANGUAGE_TYPE_RUSSIAN ) 
		return _XCREATECHARACTERRESULT_UNKNOWNERROR;
	
	memset( g_LastErrorCheckedCharacterName, 0, sizeof(TCHAR) * 3 );
	
	if( !pszName ) return _XCREATECHARACTERRESULT_STRINGISNULL;
	
	int length = wcslen( pszName );
	if( length <=0 ) return _XCREATECHARACTERRESULT_LENGTHISZERO;
	
	// check space
	int i = 0 ; for(  i = 0; i < length; i++ )
	{
		if( pszName[i] == 0x0020 || pszName[i] == 0x3000 )
		{
			return _XCREATECHARACTERRESULT_INCLUDESPACE;
		}
	}
	
	wchar_t* curpoint = pszName;
	
	while( 1 )
	{
		if( *curpoint == L'\0'|| *curpoint == L'\r' || *curpoint == L'\n' )
		{
			break;
		}
		
		if( *curpoint == 0x002D ) // hyphen
		{				
		}
		else if( *curpoint == 0x005F ) // underline
		{			
		}
		else if( *curpoint == 0x005B || *curpoint == 0x005D ) // brakets
		{
		}
		else if( (*curpoint >= 0x0041 && *curpoint <= 0x005A) || ( *curpoint >= 0x0061 && *curpoint <= 0x007A ) ) // Alphabet 
		{
		}
		else if( *curpoint >= 0x0030 && *curpoint <= 0x0039 ) // Numeric
		{
		}
		else
		{
			if( g_LanguageType == _XLANGUAGE_TYPE_ENGLISH  )
			{
				invalidcharacter = *curpoint;
				return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
			}
			else if( g_LanguageType == _XLANGUAGE_TYPE_RUSSIAN )
			{
				if( *curpoint >= 0x0410 && *curpoint <= 0x044F ) // 키릴자모
				{
					if( g_XBaseFont )
					{
						if( !g_XBaseFont->IsValidCharacter( *curpoint )  )
						{
							invalidcharacter = *curpoint;
							return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
						}
					}
					else
					{
						return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
					}
				}
				else
				{
					invalidcharacter = *curpoint;
					return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
				}				
			}
			else
			{
				BOOL invalidcharacterchecked = FALSE;
				if( *curpoint >= 0x4E00 && *curpoint <= 0x9FA5 ) // Unified CJK Ideographs
				{
					//pass
				}				
#ifdef _XJAPANESE
				else if( (*curpoint >= 0x3041 && *curpoint <= 0x3094) || // hiragana 
						 (*curpoint >= 0x30A1 && *curpoint <= 0x30FE) )  // katakana
				{
					if( g_XBaseFont )
					{
						if( !g_XBaseFont->IsValidCharacter( *curpoint )  )
						{
							invalidcharacter = *curpoint;
							return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
						}
					}
					else
					{
						return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
					}
				}

#endif
				else if( *curpoint >= 0x3400 && *curpoint <= 0x4DBF ) // CJK Ideographs Ext. A
				{
					if( g_XBaseFont )
					{
						if( !g_XBaseFont->IsValidCharacter( *curpoint )  )
						{
							invalidcharacter = *curpoint;
							return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
						}
					}
					else
					{
						return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
					}
				}
				else if( *curpoint >= 0xF900 && *curpoint <= 0xFAFF ) // CJK Compatibility ideographs
				{
					if( !g_XBaseFont->IsValidCharacter( *curpoint )  )
					{
						invalidcharacter = *curpoint;
						return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
					}
				}			
				else if( *curpoint >= 0xFF00 && *curpoint <= 0xFFEE ) // Half width and Full width
				{
					if( !g_XBaseFont->IsValidCharacter( *curpoint )  )
					{
						invalidcharacter = *curpoint;
						return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
					}
				}
				else
				{
					invalidcharacter = *curpoint;
					return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
				}
			}
		}
		
		curpoint++;
	}	

	return _XCREATECHARACTERRESULT_NOERROR;
}
	

_XCREATECHARACTERRESULT	_XCheckUsableCharacterName( LPCSTR pszName )
{	
	if(g_LanguageType != _XLANGUAGE_TYPE_KOREAN) 
	{
		memset( g_LastErrorCheckedCharacterName, 0, sizeof(TCHAR) * 3 );
		
		if( !pszName ) return _XCREATECHARACTERRESULT_STRINGISNULL;
		
		int length = strlen( pszName );
		if( length <=0 ) return _XCREATECHARACTERRESULT_LENGTHISZERO;
		
		// check space
		int i = 0 ; for(  i = 0; i < length; i++ )
		{
			if( pszName[i] == 0x20 )
			{
				return _XCREATECHARACTERRESULT_INCLUDESPACE;
			}
		}
		
		LPBYTE	curpoint = (LPBYTE)pszName;
		
		while( 1 )
		{
			if( *curpoint == '\0'|| *curpoint == '\r' || *curpoint == '\n' )
			{
				break;
			}
			
			if( *curpoint == 0x2D ) // hyphen
			{				
			}
			else if( *curpoint == 0x5F ) // underline
			{			
			}
			else if( *curpoint == 0x5B || *curpoint == 0x5D ) // brakets
			{
			}
			else if( (*curpoint >= 0x41 && *curpoint <= 0x5A) || ( *curpoint >= 0x61 && *curpoint <= 0x7A ) ) // Alphabet 
			{
			}
			else if( *curpoint >= 0x30 && *curpoint <= 0x39 ) // Numeric
			{
			}
			else if( *curpoint >= 0x80 ) // vietnamese & taiwanese
			{
			}
			else
			{
				g_LastErrorCheckedCharacterName[0] = *curpoint;
				g_LastErrorCheckedCharacterName[1] = 0;
				g_LastErrorCheckedCharacterName[2] = 0;				
				return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
			}
			
			curpoint++;
		}	
		
		if(g_LanguageType == _XLANGUAGE_TYPE_VIETNAMESE )
		{
			WCHAR _ucodeconvertbuffer[2048];
			int outputmaxlength = 2048;
			memset( _ucodeconvertbuffer, 0, sizeof(WCHAR) * 2048 );
			_XCP1258ToUniCode( (LPBYTE)pszName, (LPBYTE)_ucodeconvertbuffer, length, outputmaxlength );

			int i = 0 ; for(  i = 0; i < outputmaxlength; i++ )
			{
				if( _ucodeconvertbuffer[i] == 0x0000 )
				{
					break;
				}

				if( _ucodeconvertbuffer[i] == 0xA0 )
				{
					g_LastErrorCheckedCharacterName[0] = (TCHAR)0xA0;
					g_LastErrorCheckedCharacterName[1] = 0;
					g_LastErrorCheckedCharacterName[2] = 0;				
					return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
				}
				else if( _ucodeconvertbuffer[i] == 0xFF )
				{
					g_LastErrorCheckedCharacterName[0] = (TCHAR)0xFF;
					g_LastErrorCheckedCharacterName[1] = 0;
					g_LastErrorCheckedCharacterName[2] = 0;				
					return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
				}
			}			
		}
		
		return _XCREATECHARACTERRESULT_NOERROR;
	}
	else
	{
		memset( g_LastErrorCheckedCharacterName, 0, sizeof(TCHAR) * 3 );

		if( !pszName ) return _XCREATECHARACTERRESULT_STRINGISNULL;

		int length = strlen( pszName );
		if( length <=0 ) return _XCREATECHARACTERRESULT_LENGTHISZERO;

		// check space
		int i = 0 ; for(  i = 0; i < length; i++ )
		{
			if( pszName[i] == 0x20 )
			{
				return _XCREATECHARACTERRESULT_INCLUDESPACE;
			}
		}

		LPBYTE	curpoint = (LPBYTE)pszName;
		
		while( 1 )
		{
			if( *curpoint == '\0'|| *curpoint == '\n' )
			{
				break;
			}
			
			if (*curpoint&0x80) // check mbcs...
			{
				if( strncmp( (LPCTSTR)curpoint, _T("뷁"), 2 ) == 0 ) // ? ^^;
				{

				}
				else if( *curpoint >= 0xA4 && *curpoint <= 0xFE ) // check high byte
				{
					if( ( *curpoint >= 0xB0 && *curpoint <= 0xC8 ) /*한글*/ )
					{
						if( (*(curpoint+1)) >= 0xA1 && (*(curpoint+1)) <= 0xFE ) // check low byte
						{
							
						}
						else
						{
							g_LastErrorCheckedCharacterName[0] = *curpoint;
							g_LastErrorCheckedCharacterName[1] = *(curpoint+1);
							return _XCREATECHARACTERRESULT_INVALIDCHARACTER;
						}
					}
					else if( *curpoint == 0xA4 ) //낱자
					{
						g_LastErrorCheckedCharacterName[0] = *curpoint;
						g_LastErrorCheckedCharacterName[1] = *(curpoint+1);
						return _XCREATECHARACTERRESULT_SINGLEKOREANCHARACTER;
					}
					else
					{
						g_LastErrorCheckedCharacterName[0] = *curpoint;
						g_LastErrorCheckedCharacterName[1] = *(curpoint+1);
						return _XCREATECHARACTERRESULT_INVALIDCHARACTER;
					}
				}
				else
				{
					g_LastErrorCheckedCharacterName[0] = *curpoint;
					g_LastErrorCheckedCharacterName[1] = *(curpoint+1);
					return _XCREATECHARACTERRESULT_INVALIDCHARACTER;
				}

				curpoint+=2;
			}
			else
			{
				if( *curpoint == 0x2D ) // hyphen
				{				
				}
				else if( *curpoint == 0x5F ) // underline
				{			
				}
				else if( *curpoint == 0x5B || *curpoint == 0x5D ) // brakets
				{
				}
				else if( (*curpoint >= 0x41 && *curpoint <= 0x5A) || ( *curpoint >= 0x61 && *curpoint <= 0x7A ) ) // Alphabet 
				{
				}
				else if( *curpoint >= 0x30 && *curpoint <= 0x39 ) // Numeric
				{
				}
				else
				{
					g_LastErrorCheckedCharacterName[0] = *curpoint;
					g_LastErrorCheckedCharacterName[1] = 0;
					g_LastErrorCheckedCharacterName[2] = 0;				
					return _XCREATECHARACTERRESULT_NOVALIDASCIICHARACTER;
				}

				curpoint++;
			}		
		}	

		return _XCREATECHARACTERRESULT_NOERROR;
	}

	return _XCREATECHARACTERRESULT_NOERROR;
}

//////////////////////////////////////////////////////////////////////
// _XFont
//////////////////////////////////////////////////////////////////////

_XFont::_XFont()
{
	m_maxheight		=	0;	
	m_info			=	NULL;
	m_image			=	NULL;		
	m_LookupTable   = new unsigned short[65536];
	memset( m_LookupTable, 0xffff, sizeof( unsigned short ) * 65536 );
}

_XFont::~_XFont()
{	
	SAFE_DELETE_ARRAY( m_info  );
	SAFE_DELETE_ARRAY( m_image );	
	SAFE_DELETE_ARRAY( m_LookupTable );	
}

bool _XFont::Load( LPCTSTR szFilename )
{	
	int i, size, len, offset;

	TCHAR packagefilename[_MAX_PATH];
	packagefilename[0] = NULL;
	strcpy( packagefilename, gModulePath );
	strcat( packagefilename, _T("\\Data\\Texture\\") );
	
	_XPackageArchive fontarchive;
#ifdef _XTESTSERVER		
	strcat( packagefilename, _T("tsTR_XFONT.XP") );
#else
	strcat( packagefilename, _T("TR_XFONT.XP") );	
#endif

	if( !fontarchive.OpenPackage( packagefilename ) )
	{
		return false;
	}
	
	FILE * fp = fontarchive.GetPackedFile( (LPSTR)szFilename );

	if (fp) 
	{
		fread(&m_cnt, 1, 2, fp);
		fread(&size, 1, 4, fp);
		
		SAFE_DELETE_ARRAY( m_info  );
		SAFE_DELETE_ARRAY( m_image );
		
		m_info = new Chinfo [m_cnt];
		m_image = new unsigned char [size];
		
		unsigned short code;

		for(i=0, offset=0; i<m_cnt; i++)
		{
			fread(&code, 1, 2, fp);
			fread(&m_info[i].w, 1, 1, fp);
			fread(&m_info[i].h, 1, 1, fp);
			m_info[i].offset = offset;

			if (m_info[i].h > m_maxheight)
				m_maxheight = m_info[i].h;

			len = ((m_info[i].w * m_info[i].h + 3) >> 2);

			if( len > 0 )
			{
				fread(&m_image[offset], len, 1, fp);
				offset += len;
			}

			m_LookupTable[ code ] = i;
		}
		
		fontarchive.FinalizePackage();

		int i = 0 ; for(  i = 0; i < 65536; i++ )
		{
			if( m_LookupTable[i] == 0xFFFF )
			{
				m_LookupTable[i] = m_LookupTable[_T('?')]; //0xA1E0
			}
		}	

		m_LookupTable['\r'] = m_LookupTable[' '];
		m_LookupTable['\t'] = m_LookupTable[' '];
		m_LookupTable['\n'] = m_LookupTable[' '];
		
		return true;
	}

	fontarchive.FinalizePackage();

	return false;
}

int _XFont::GetWidth(unsigned short code)
{
	return m_info[m_LookupTable[code]].w + _XCHAR_GAP;
}

int _XFont::CalcWidth(const char * text)
{
	if( !text ) return 0;
	int length = strlen( text );
	if( length <= 0 || length > 2048 ) return 0;

	int i, code, w = 0;

	for(i=0; text[i] != '\0';) 
	{
		code = (unsigned char)text[i++];

		if( code == '\r' ) i++;
		else if( code == '\t' ) i++;
		else if( code == '\n' ) break;//i++;	
		else
		{
			if (code&0x80)
			{
				//if( text[i+1] != '\0' )
					code = (code<<8) | (unsigned char)text[i++];
			}
			
			w += GetWidth(code);

			if( text[i] == '\0' ) return w;
		}

		if( i > 2048 ) break;
	}
	return w;
}

extern int	_X_WtoM_ConvertString(UINT codePage, const wchar_t* wText, int wLen, char* text, int len);

int	_XFont::CalcWidth(wchar_t* text)
{
	if( !text ) return 0;

	int strlength = wcslen(text); 
	
	TCHAR mbcsconvertbuffer[2048];
	_X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, text, wcslen( text ), mbcsconvertbuffer, MAX_IMEBUFFERLENGTH);	

	int i, code, w = 0;

	for(i=0; mbcsconvertbuffer[i] != '\0';) 
	{
		code = (unsigned char)mbcsconvertbuffer[i++];

		if( code == '\r' ) i++;
		else if( code == '\t' ) i++;
		else if( code == '\n' ) break;//i++;	
		else
		{
			if (code&0x80)
			{
				//if( mbcsconvertbuffer[i+1] != '\0' )
					code = (code<<8) | (unsigned char)mbcsconvertbuffer[i++];
			}
			
			w += GetWidth(code);

			if( mbcsconvertbuffer[i] == '\0' ) return w;
		}

		if( i > 2048 ) break;
	}
	
	return w;
}

int _XFont::DrawClip(unsigned char * ptr, int pitch, unsigned short code, int skip, int rest)
{
	Chinfo * info = &m_info[m_LookupTable[code]];
	unsigned char * img, byte;
	int x, y, i, w1;

	img = &m_image[info->offset];

	if (skip == 0 && info->w + _XCHAR_GAP < rest)
	{
		for(y=0, i=8; y<info->h; y++, ptr+=pitch) 
		{
			for(x=0; x<info->w; x++, i+=2, byte>>=2) 
			{
				if (i >= 8) 
				{
					byte = *img++;
					i = 0;
				}
				ptr[x] = byte & 0x03;
			}
#if _XCHAR_GAP > 0
			for(; x<info->w+_XCHAR_GAP; x++)
				ptr[x] = 0;
#endif
		}
		for(; y<m_maxheight; y++, ptr+=pitch)
			memset(ptr, 0, info->w+_XCHAR_GAP);
	}	
	else
	{
		w1 = info->w > rest ? rest : info->w;

		for(y=0, i=8; y<info->h; y++, ptr+=pitch) 
		{
			for(x=0; x<skip; x++, i+=2, byte>>=2) 
			{
				if (i >= 8) 
				{
					byte = *img++;
					i = 0;
				}
			}
			for(; x<w1; x++, i+=2, byte>>=2) 
			{
				if (i >= 8) 
				{
					byte = *img++;
					i = 0;
				}
				ptr[x] = byte & 0x03;
			}
			for(; x<info->w; x++, i+=2, byte>>=2) 
			{
				if (i >= 8) 
				{
					byte = *img++;
					i = 0;
				}
			}
#if _XCHAR_GAP > 0
			for(; x<info->w+_XCHAR_GAP; x++)
			{
				if (x < rest) ptr[x] = 0;
			}
#endif
		}

		w1 = info->w+_XCHAR_GAP > rest ? rest : info->w+_XCHAR_GAP;

		for(; y<m_maxheight; y++, ptr+=pitch)
		{
			memset(ptr+skip, 0, w1-skip);
		}
	}

	return info->w + _XCHAR_GAP;
}

int _XFont::Draw(unsigned char * ptr, int pitch, unsigned short code)
{
	Chinfo* info = NULL;
	
	/*
	if( m_LookupTable[code] == 0xFFFF )
	{
		info = &m_info[m_LookupTable[_T(' ')]];
	}
	else*/
	{
		info = &m_info[m_LookupTable[code]];
	}

	unsigned char * img, byte;
	int x, y, i;

	img = &m_image[info->offset];

	for(y=0, i=8; y<info->h; y++, ptr+=pitch) 
	{
		for(x=0; x<info->w; x++, i+=2, byte>>=2) 
		{
			if (i >= 8) 
			{
				byte = *img++;
				i = 0;
			}
			ptr[x] = byte & 0x03;
		}
#if _XCHAR_GAP > 0
		for(; x<info->w+_XCHAR_GAP; x++)
		{
			ptr[x] = 0;
		}
#endif
	}

	return info->w + _XCHAR_GAP;
}

//////////////////////////////////////////////////////////////////////
// _XFontWorkspace
//////////////////////////////////////////////////////////////////////
_XFontWorkspace::_XFontWorkspace(int maxtex)
{
	m_maxtex		= maxtex;
	m_sparetex		= 0;
	
	m_curtex		= 0;
	m_curtexb		= 0;
	m_piece			= NULL;
	m_reserve		= NULL;
	m_u				= 0; 
	m_v				= 0;
	m_vtop			= 0;
	m_vbottom		= 0;
	m_updatecnt		= 1;
	m_oldtype		= 0;
	m_oldtexhandle  = -1;

	m_fontedgemode	= _XFONT_EDGEMODE_NONE;
	m_fontedgecolor = 0xFF000000;
	
	m_renderfontbold = FALSE;

	memset(&m_fonttexturelist, 0, sizeof(_XFONTTEXTURE)*128);

	m_texhandle = new int [maxtex];
	memset(m_texhandle, 0, sizeof(int) * maxtex);
	
	m_image = new unsigned char [TEX_W * TEX_H];
	memset(m_image, 0, TEX_W * TEX_H);

	m_EnableClippingArea = FALSE;
	m_ClippingViewPort.X = 0;
	m_ClippingViewPort.Y = 0;
	m_ClippingViewPort.MinZ = 0.0f;
	m_ClippingViewPort.MaxZ = 1.0f;
	m_ClippingViewPort.Width = gnWidth;
	m_ClippingViewPort.Width = gnHeight;
}

_XFontWorkspace::~_XFontWorkspace()
{
	ClearAll();

	SAFE_DELETE_ARRAY( m_image );
	SAFE_DELETE_ARRAY( m_texhandle );
}

void _XFontWorkspace::Draw(float x, float y, float scale, unsigned long type, unsigned long color, LPVOID ptr)
{
	int u1, u2, v, tex, remain;
	_XPEICETEX * p;

#ifndef _XSPLIT_CHAR_
	int code, i=0, w;
#endif
	p = (_XPEICETEX *) ptr;

	tex = p->texidx;
	u1 = u2 = p->u;
	v = p->v;

	remain = p->w;

	do 
	{
		if (remain + u1 < TEX_W) 
		{
			u2 = u1 + remain;
		}	
		else 
		{
#ifndef _XSPLIT_CHAR_
			for(; p->text[i] != '\0'; ) 
			{
				code = (unsigned char)p->text[i++];

				if (code&0x80)
					code = (code<<8) | (unsigned char)p->text[i++];

				w = p->font->GetWidth(code);

				if (u2 + w > TEX_W) break;

				u2 += w;
			}
#else
			u2 = TEX_W;
#endif
		}

		Render(m_texhandle[tex], x, y, (u2 - u1) * scale, p->h * scale, (float)u1 / TEX_W, (float)v / TEX_H, (float)(u2 - u1) / TEX_W, (float)p->h / TEX_H, type, color);
		remain -= u2 - u1;

		if (remain > 0) 
		{
			x += (u2 - u1) * scale;
			v += p->h + 1;

			if (v + p->h + 1 > TEX_H) 
			{
				tex = (tex + 1) % (m_maxtex+m_sparetex);
				v = 0;
			}

			u1 = 0;
#ifndef _XSPLIT_CHAR_
			u2 = w;
#endif
		}
	}	while(remain > 0);
}

int	 _XFontWorkspace::CreateTexture(int w, int h)
{
	D3DFORMAT fmt = D3DFMT_A1R5G5B5;
	//D3DFORMAT fmt = D3DFMT_A8R8G8B8;
	int		  i;
	int		  fmtidx;
	
	for(i=1; i<sizeof(m_fonttexturelist)/sizeof(*m_fonttexturelist); i++)
	{
		if (m_fonttexturelist[i].tex == NULL)
		{
			if (!FAILED(gpDev->CreateTexture(w, h, 1, 0, fmt, D3DPOOL_MANAGED, &m_fonttexturelist[i].tex, NULL)))
			{
				m_fonttexturelist[i].fmt = fmt;
				m_fonttexturelist[i].w = w;
				m_fonttexturelist[i].h = h;
				
				for(fmtidx=0; g_pixfmt[fmtidx].fmt != fmt; fmtidx++) ;

				m_fonttexturelist[i].fmtidx = fmtidx;
				return i;
			}
		}
	}
	
	return 0;	
}

void _XFontWorkspace::ReleaseTexture(DWORD texidx)
{
	SAFE_RELEASE( m_fonttexturelist[texidx].tex );	
}

void _XFontWorkspace::UpdateTexture(DWORD texidx, unsigned char *ptr, int x, int y, int w, int h, int bpl, unsigned char (* palette)[3])
{
	unsigned long code;
	unsigned char * tag, * src, * p, * pbits;
	int i, j, fmtidx, pitch;
	
	pbits = (unsigned char*) Lock(texidx, &pitch);
	
	if (pbits) {
		fmtidx = m_fonttexturelist[texidx].fmtidx;
		
		pbits += x + y * pitch;
		
		for(i=0; i<h; i++, pbits+=pitch, src+=bpl) 
		{
			src = ptr + i * bpl;
			tag = (unsigned char*) pbits;
			
			for(j=0; j<w; j++, tag+=g_pixfmt[fmtidx].cpp) 
			{
				p = !palette ? &src[j*3] : palette[src[j]];
				
				//code = g_pixfmt[fmtidx].Pack(p[2], p[1], p[0], (p[0] ? 255 : 0) );
				code = g_pixfmt[fmtidx].Pack(255, 255, 255, (p[0] ? 255 : 0) );
				memcpy(tag, &code, g_pixfmt[fmtidx].cpp);
			}
		}
		
		Unlock(texidx);
	}
}

LPVOID _XFontWorkspace::Lock(DWORD texidx, int * pitch)
{
	D3DLOCKED_RECT rect;
	
	if (m_fonttexturelist[texidx].tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
	{
		if (pitch)
			*pitch = rect.Pitch;
		return rect.pBits;
	}
	return NULL;
}

void _XFontWorkspace::Unlock(DWORD texidx)
{
	m_fonttexturelist[texidx].tex->UnlockRect(0);	
}

void _XFontWorkspace::ClearAll()
{
	_XPEICETEX * next, * p;
	int i;

	if( m_texhandle )
	{
		for(i=0; i<m_maxtex+m_sparetex; i++) 
		{
			if (m_texhandle[i]) 
			{
				ReleaseTexture(m_texhandle[i]);
				m_texhandle[i] = 0;
			}
		}

		delete[] m_texhandle;
		m_texhandle = NULL;
	}

	m_sparetex = 0;

	if( m_piece )
	{
		for(p=m_piece; p; p=next) 
		{
			next = p->next;
			delete p;
		}
		m_piece = NULL;	
	}
}

void _XFontWorkspace::Flush()
{
	if (m_vtop != m_vbottom) 
	{
		m_curtex = m_curtex % (m_maxtex + m_sparetex);
		UpdateTexture(m_curtex, m_vtop, m_vbottom-1);
	}

	m_vtop = m_vbottom = m_v;

	m_updatecnt++;
	m_curtexb = m_curtex;
	m_oldtexhandle = -1;
	m_oldtype = -1;
}

int _XFontWorkspace::GetWidth(void *p)
{
	return ((_XPEICETEX *)p)->w;
}

LPVOID _XFontWorkspace::GetPieceTex(LPCTSTR text, _XFont* font)
{
	_XPEICETEX * p;

	for(p=m_piece; p; p=p->next)
	{
		if (!strcmp(p->text, text) && p->font == font)
			break;
	}

	if (p == NULL) 
	{
		p = (_XPEICETEX*) (new char [sizeof(_XPEICETEX) + strlen(text)]);
		strcpy(p->text, text);
		p->font = font;
		p->updatecnt = 0;

		m_piece = AddLink(m_piece, p);
		WriteTexture(p);
	}

	p->updatecnt = m_updatecnt;

	return (void*) p;
}

void _XFontWorkspace::ClearPage(int idx)
{
	_XPEICETEX * p, * next;

	for(p=m_piece; p; ) 
	{
		next = p->next;

		if (p->texidx == idx) 
		{
			m_piece = RemoveLink(m_piece, p);

			if (p->updatecnt == m_updatecnt)
			{
				m_reserve = AddLink(m_reserve, p);
			}
			else
			{
				delete p;
			}
		}

		p = next;
	}
}

void _XFontWorkspace::NextLine(int height)
{
	m_u = 0;
	m_v += height + 1;

	if (m_v + height + 1 > TEX_H)
	{
		UpdateTexture(m_curtex % (m_maxtex+m_sparetex), m_vtop, TEX_H-1);

		memset(m_image, 0, TEX_W*TEX_H);

		m_curtex++;
		m_v = 0;
		m_vbottom = 0;
		m_vtop = 0;

		ClearPage(m_curtex % (m_maxtex+m_sparetex));
	}
}

void _XFontWorkspace::WriteTexture(_XPEICETEX * p)
{
	int i, code, w;
	_XFont * font = p->font;

	p->u = m_u;
	p->v = m_v;
	p->w = 0;
	p->h = font->GetHeight();
	p->texidx = m_curtex % (m_maxtex+m_sparetex);
	p->updatecnt = m_updatecnt;

	for(i=0; p->text[i] != '\0';) 
	{
		code = (unsigned char)p->text[i++];

		if (code&0x80)
			code = (code<<8) | (unsigned char)p->text[i++];

#ifndef _XSPLIT_CHAR_
		w = font->GetWidth(code);

		if (m_u + w > TEX_W)
			NextLine(font->GetHeight());

		w = font->Draw(m_image + m_u + m_v * TEX_W, TEX_W, code);
		m_u += w;
		p->w += w;
#else
		if (m_u >= TEX_W)
			NextLine(font->GetHeight());

		int t = TEX_W - m_u;
		w = font->DrawClip(m_image + m_u + m_v * TEX_W, TEX_W, code, 0, t);
		p->w += w;

		assert(m_u + w < TEX_W*2) ; // never !!

		if (t < w) 
		{
			NextLine(font->GetHeight());

			font->DrawClip(m_image + m_v * TEX_W - t, TEX_W, code, t, TEX_W);
			m_u = w - t;
		}	
		else
		{
			m_u += w;
		}
#endif
	}

	if (m_vbottom < m_v + font->GetHeight() + 1)
		m_vbottom = m_v + font->GetHeight() + 1;

	m_u += 1;

	if (m_u >= TEX_W)
		NextLine(font->GetHeight());

	if (m_curtex - m_curtexb >= m_maxtex + m_sparetex) 
	{
		IncTexCnt();
		Rearrange();
	}

	if (m_reserve) 
	{
		_XPEICETEX * next, * p = m_reserve;
		m_reserve = NULL;
		for(; p; p=next) 
		{
			next = p->next;
			m_piece = AddLink(m_piece, p);
			WriteTexture(p);
		}
	}
}

void _XFontWorkspace::IncTexCnt()
{
	int * texhandle = new int [m_maxtex+m_sparetex+1];

	memcpy(texhandle, m_texhandle, (m_maxtex + m_sparetex) * sizeof(int*));
	texhandle[m_maxtex + m_sparetex] = 0;

	SAFE_DELETE_ARRAY( m_texhandle );

	m_sparetex += 1;
	m_texhandle = texhandle;
}

void _XFontWorkspace::Rearrange()
{
	_XPEICETEX * tail, * p, * next;

	if (!m_piece) 
	{
		p = m_reserve;
	}	
	else 
	{
		for(tail=p=m_piece; tail->next; tail=tail->next) ;
		
		tail->next = m_reserve;
		
		if (m_reserve)
			m_reserve->prev = tail;
	}

	m_piece = NULL;
	m_reserve = NULL;

	memset(m_image, 0, TEX_W*TEX_H);
	m_curtex = m_curtexb = 0;
	m_u = m_v = 0;
	m_vtop = m_vbottom = 0;

	for(; p; p=next) 
	{
		next = p->next;
		m_piece = AddLink(m_piece, p);
		WriteTexture(p);
	}
}

void _XFontWorkspace::UpdateTexture(int idx, int top, int bottom)
{
	if( !m_texhandle ) return;

	static unsigned char palette[4][3] = 
	{
		0, 0, 0, 
		128, 128, 128,
		196, 196, 196,
		255, 255, 255,
	} ;

	if (m_texhandle[idx] == 0)
		m_texhandle[idx] = CreateTexture(TEX_W, TEX_H);

	UpdateTexture(m_texhandle[idx], m_image + top * TEX_W, 0, top, TEX_W, bottom - top + 1, TEX_W, palette);
}

void _XFontWorkspace::DisableClippingArea( void )
{
	m_EnableClippingArea = FALSE;
}

void _XFontWorkspace::SetClippingArea( _XFRECT& cliprect )
{
	if( cliprect.left < 0 ) m_ClippingViewPort.X = 0;
	else m_ClippingViewPort.X = cliprect.left;

	if( m_ClippingViewPort.X >= gnWidth ) m_ClippingViewPort.X = gnWidth-1;
	
	if( cliprect.top < 0 ) m_ClippingViewPort.Y = 0;
	else m_ClippingViewPort.Y = cliprect.top;

	if( m_ClippingViewPort.Y >= gnHeight ) m_ClippingViewPort.Y = gnHeight-1;

	m_ClippingViewPort.Width = cliprect.right - cliprect.left;

	if( m_ClippingViewPort.X + m_ClippingViewPort.Width >= gnWidth )
	{
		m_ClippingViewPort.Width -=  (m_ClippingViewPort.X + m_ClippingViewPort.Width) - gnWidth;
	}

	if( m_ClippingViewPort.Width < 1 ) m_ClippingViewPort.Width = 1;

	m_ClippingViewPort.Height = cliprect.bottom - cliprect.top;

	if( m_ClippingViewPort.Y + m_ClippingViewPort.Height >= gnHeight )
	{
		m_ClippingViewPort.Height -=  (m_ClippingViewPort.Y + m_ClippingViewPort.Height ) - gnHeight;
	}

	if( m_ClippingViewPort.Height < 1 ) m_ClippingViewPort.Height = 1;

	m_ClippingViewPort.MinZ = 0.0f;
	m_ClippingViewPort.MaxZ = 1.0f;
	m_EnableClippingArea = TRUE;
}

void _XFontWorkspace::SetClippingArea( FLOAT left, FLOAT top, FLOAT right, FLOAT bottom )
{
	if( left < 0 ) m_ClippingViewPort.X = 0;
	else m_ClippingViewPort.X = left;

	if( m_ClippingViewPort.X >= gnWidth ) m_ClippingViewPort.X = gnWidth-1;
	
	if( top < 0 ) m_ClippingViewPort.Y = 0;
	else m_ClippingViewPort.Y = top;

	if( m_ClippingViewPort.Y >= gnHeight ) m_ClippingViewPort.Y = gnHeight-1;
	

	m_ClippingViewPort.Width = right - left;
	
	if( m_ClippingViewPort.X + m_ClippingViewPort.Width >= gnWidth )
	{
		m_ClippingViewPort.Width -=  (m_ClippingViewPort.X + m_ClippingViewPort.Width) - gnWidth;
	}

	if( m_ClippingViewPort.Width < 1 ) m_ClippingViewPort.Width = 1;
	
	m_ClippingViewPort.Height = bottom - top;

	if( m_ClippingViewPort.Y + m_ClippingViewPort.Height >= gnHeight )
	{
		m_ClippingViewPort.Height -=  (m_ClippingViewPort.Y + m_ClippingViewPort.Height ) - gnHeight;
	}

	if( m_ClippingViewPort.Height < 1 ) m_ClippingViewPort.Height = 1;

	m_ClippingViewPort.MinZ = 0.0f;
	m_ClippingViewPort.MaxZ = 1.0f;
	m_EnableClippingArea = TRUE;
}

void _XFontWorkspace::Render( int texhandle, float x, float y, float w, float h, 
							  float tu, float tv, float tw, float th, 
							  unsigned long type, unsigned long color )
{
	gpDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	gpDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);	
	gpDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	_XFONTVERTEX v[8];

	SetTexture(texhandle);
	gpDev->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1);

	static FLOAT offsetvalue[12][2] = 
	{
		{ -2.0f,  0.0f },	// 0
		{  2.0f,  0.0f },	// 1
		{  0.0f, -2.0f },	// 2
		{  0.0f,  2.0f },   // 3

		{ -1.0f, -1.0f },	// 4
		{  0.0f, -1.0f },	// 5
		{  1.0f, -1.0f },	// 6

		{ -1.0f,  0.0f },	// 7
		{  1.0f,  0.0f },	// 8

		{ -1.0f,  1.0f },	// 9
		{  0.0f,  1.0f },	// 10
		{  1.0f,  1.0f }	// 11		
	};
	static int halfedgeindextable[4] = { 5, 7, 8, 10 };

	v[0].x = x + _XMAGICVALUE;
	v[0].y = y + _XMAGICVALUE;
	v[0].z = 0.9f;
	v[0].rhw = _XDEF_DEFAULTDEPTH_FORINTERFACE;
	v[0].tu = tu;
	v[0].tv = tv;
	
	v[1].x = x + w + _XMAGICVALUE;
	v[1].y = y + _XMAGICVALUE;
	v[1].z = 0.9f;
	v[1].rhw = _XDEF_DEFAULTDEPTH_FORINTERFACE;
	v[1].tu = tu + tw;
	v[1].tv = tv;
	
	v[2].x = x + w + _XMAGICVALUE;
	v[2].y = y + h + _XMAGICVALUE;
	v[2].z = 0.9f;
	v[2].rhw = _XDEF_DEFAULTDEPTH_FORINTERFACE;
	v[2].tu = tu + tw;
	v[2].tv = tv + th;
	
	v[3].x = x + _XMAGICVALUE;
	v[3].y = y + h + _XMAGICVALUE;
	v[3].z = 0.9f;
	v[3].rhw = _XDEF_DEFAULTDEPTH_FORINTERFACE;
	v[3].tu = tu;
	v[3].tv = tv + th;
	
	if( m_fontedgemode == _XFONT_EDGEMODE_GLOW )
	{
		v[0].color = v[1].color = v[2].color = v[3].color = (m_fontedgecolor & 0xFFFFFF) | (((m_fontedgecolor & 0xFF000000)>>25)<<24);

		int i = 0 ; for(  i = 0; i < 12; i++ )
		{
			if( i == 4 )
			{
				v[0].color = v[1].color = v[2].color = v[3].color = m_fontedgecolor;
			}

			memcpy( v+4, v, sizeof(_XFONTVERTEX) << 2 );

			v[4].x += offsetvalue[i][0];
			v[4].y += offsetvalue[i][1];
			v[5].x += offsetvalue[i][0];
			v[5].y += offsetvalue[i][1];
			v[6].x += offsetvalue[i][0];
			v[6].y += offsetvalue[i][1];
			v[7].x += offsetvalue[i][0];
			v[7].y += offsetvalue[i][1];

			gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v+4, sizeof(_XFONTVERTEX));
		}
		
		v[0].color = v[1].color = v[2].color = v[3].color = color;
		gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(_XFONTVERTEX));
	}
	else if( m_fontedgemode == _XFONT_EDGEMODE_FULLEDGE )
	{
		v[0].color = v[1].color = v[2].color = v[3].color = m_fontedgecolor;
		
		int i = 0 ; for(  i = 4; i < 12; i++ )
		{
			memcpy( v+4, v, sizeof(_XFONTVERTEX) << 2 );
			
			v[4].x += offsetvalue[i][0];
			v[4].y += offsetvalue[i][1];
			v[5].x += offsetvalue[i][0];
			v[5].y += offsetvalue[i][1];
			v[6].x += offsetvalue[i][0];
			v[6].y += offsetvalue[i][1];
			v[7].x += offsetvalue[i][0];
			v[7].y += offsetvalue[i][1];
			
			gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v+4, sizeof(_XFONTVERTEX));
		}
		
		v[0].color = v[1].color = v[2].color = v[3].color = color;
		gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(_XFONTVERTEX));
	}
	else if( m_fontedgemode == _XFONT_EDGEMODE_EDGE )
	{
		v[0].color = v[1].color = v[2].color = v[3].color = m_fontedgecolor;
		
		int i = 0 ; for(  i = 0; i < 4; i++ )
		{
			memcpy( v+4, v, sizeof(_XFONTVERTEX) << 2 );

			FLOAT _x = offsetvalue[halfedgeindextable[i]][0];
			FLOAT _y = offsetvalue[halfedgeindextable[i]][1];
			
			v[4].x += _x;
			v[4].y += _y;
			v[5].x += _x;
			v[5].y += _y;
			v[6].x += _x;
			v[6].y += _y;
			v[7].x += _x;
			v[7].y += _y;
			
			gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v+4, sizeof(_XFONTVERTEX));
		}
		
		v[0].color = v[1].color = v[2].color = v[3].color = color;
		gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(_XFONTVERTEX));
	}
	else 
	{
		v[0].color = v[1].color = v[2].color = v[3].color = color;

		if( m_renderfontbold )
		{
			memcpy( v+4, v, sizeof(_XFONTVERTEX) << 2 );
			v[4].x += 1.0f; v[5].x += 1.0f; v[6].x += 1.0f; v[7].x += 1.0f;
			gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(_XFONTVERTEX));
			gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v+4, sizeof(_XFONTVERTEX));
		}
		else // normal mode
		{	
			v[0].color = v[1].color = v[2].color = v[3].color = color;
			gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(_XFONTVERTEX));
		}
	}	

	gpDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gpDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	
	gpDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

void _XFontWorkspace::SetTexture(DWORD idx)
{	
	//if (m_oldtexhandle != idx) 
	{
		m_oldtexhandle = idx;
		gpDev->SetTexture(0, idx > 0 ? m_fonttexturelist[idx].tex : NULL);
	}
}

void _XFontWorkspace::SetPolyType(unsigned long type)
{		
	switch(type) 
	{
	case _TEXBLENDNORMAL :
		gpDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		gpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	case _TEXBLENDADD :
		gpDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		gpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);				
		break;
	case _TEXBLENDMODULATE :
		gpDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		gpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		break;
	case _TEXBLENDINVMODULATE :
		gpDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		gpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		break;
	}	
}

void _XFontWorkspace::DrawPolygon(_XFONTVERTEX *v, int n)
{
	gpDev->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1);
	gpDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, n-2, v, sizeof(_XFONTVERTEX));
}

//////////////////////////////////////////////////////////////////////
// _XFontManager
//////////////////////////////////////////////////////////////////////

_XFontManager::_XFontManager()
{
	m_polylistn = 0;
	m_color		= 0xffffffff;
	m_polytype	= _TEXBLENDNORMAL;

	m_workspace = NULL;
	m_font = NULL;
	
	m_workspace = new _XFontWorkspace(3);
	m_font = new _XFont;
	m_alphablending = FALSE;

	m_UnicodeDataMode = FALSE;
}

_XFontManager::~_XFontManager()
{
	SAFE_DELETE( m_workspace );
	SAFE_DELETE( m_font );
}

void _XFontManager::SetColor(unsigned long color) 
{ 
	m_color = color; 
}

void _XFontManager::DeleteDeviceObject()
{
	if( m_workspace )	
		m_workspace->ClearAll();
}

void _XFontManager::DisableClippingArea( void )
{
	if( m_workspace )
		m_workspace->DisableClippingArea();	
}

void _XFontManager::SetClippingArea( _XFRECT& cliprect )
{
	if( m_workspace )
		m_workspace->SetClippingArea( cliprect );
}

void _XFontManager::SetClippingArea( FLOAT left, FLOAT top, FLOAT right, FLOAT bottom )
{
	if( m_workspace )
		m_workspace->SetClippingArea( left, top, right, bottom );
}

void _XFontManager::SetAlphaBlendingEnable( BOOL alphablending )
{
	m_alphablending = alphablending;
}

bool _XFontManager::Load(const char *fname)
{
	m_workspace->m_ClippingViewPort.X = 0;
	m_workspace->m_ClippingViewPort.Y = 0;
	m_workspace->m_ClippingViewPort.MinZ = 0.0f;
	m_workspace->m_ClippingViewPort.MaxZ = 1.0f;
	m_workspace->m_ClippingViewPort.Width = gnWidth;
	m_workspace->m_ClippingViewPort.Width = gnHeight;
	
	if( m_font )
		return m_font->Load(fname);
	else 
		return false;
}

BOOL _XFontManager::IsValidCharacter( unsigned short _code )
{
	//if( _code == _T('?') ) return TRUE;

	if( m_font->m_LookupTable[ _code ] != m_font->m_LookupTable[_T('?')] )
	{
		return TRUE;
	}

	return FALSE;
}

void _XFontManager::Flush()
{
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, m_alphablending ? 9 : 2 );

	if( m_workspace->m_EnableClippingArea )
	{
		gpDev->SetViewport(&m_workspace->m_ClippingViewPort);
	}
	
	m_workspace->Flush();

	int i = 0  ; for( i=0; i<m_polylistn; i++)
	{
		m_workspace->Draw(m_polylist[i].x, m_polylist[i].y, m_polylist[i].scale, m_polylist[i].type, m_polylist[i].color, m_polylist[i].p);
	}

	m_polylistn = 0;

	if( m_workspace->m_EnableClippingArea )
		gpDev->SetViewport(&g_MainViewPort);
}

void _XFontManager::Puts(int x, int y, LPCTSTR text, float scale)
{		
	if( !text ) return;
	if( strlen( text ) < 1 ) return;

	LPVOID p;

	if (m_polylistn > sizeof(m_polylist)/sizeof(*m_polylist))
	{
		Flush();
	}
	
	m_polylist[m_polylistn].p = p = m_workspace->GetPieceTex(text, m_font);
	m_polylist[m_polylistn].color = m_color;
	m_polylist[m_polylistn].x = x;
	m_polylist[m_polylistn].y = y;
	m_polylist[m_polylistn].scale = scale;
	m_polylist[m_polylistn].type = m_polytype;
	m_polylistn++;	
}

void _XFontManager::PutsVertical(int x, int y, LPCTSTR str, float scale)
{
	if( !str ) return;
	if( strlen( str ) < 1 ) return;

	LPCTSTR	curpoint = str;	
	TCHAR	buffer[3];
	int     height = y;
	memset( buffer, 0, 3 );

	while( 1 )
	{		
		if( *curpoint == '\0'|| *curpoint == '\n' )
		{				
			break;
		}
		
		if (*curpoint&0x80) // check mbcs...
		{			
			buffer[0] = *curpoint;
			buffer[1] = *(curpoint+1);
			Puts( x, height, buffer, scale );
			
			buffer[0] = 0;
			buffer[1] = 0;
			
			height += GetHeight();
			curpoint+=2;
		}
		else
		{
			if( *curpoint == ' ' ) 
			{				
				height += GetHeight();
			}
			else
			{
				buffer[0] = *curpoint;
				Puts( x, height, buffer, scale );
				buffer[0] = 0;				
				height += GetHeight();
			}

			curpoint++;
		}		
	}	

}


void _XFontManager::Print(int x, int y, float scale, TCHAR* args, ...)
{
	if( !args ) return;

	TCHAR strVBuffer[512];   	
	va_list ap;   
	va_start(ap, args);   
	vsprintf(strVBuffer, args, ap);   
	va_end(ap);

	Puts( x, y, strVBuffer, scale );
}

void _XFontManager::PutsAlign(int x, int y, _XFONT_ALIGNTYPE aligntype, TCHAR* str)
{
	if( !str ) return;

	if( aligntype == _XFONT_ALIGNTYPE_LEFT  )
	{
		Puts( x, y, str, 1.0f );
	}
	else if( aligntype == _XFONT_ALIGNTYPE_RIGHT  )
	{	
		x -= GetWidth( str );
		Puts( x, y, str, 1.0f );
	}
	else if( aligntype == _XFONT_ALIGNTYPE_CENTER  )
	{
		x -= (GetWidth( str ) >> 1);
		Puts( x, y, str, 1.0f );		
	}
}

void _XFontManager::PrintAlign(int x, int y, float scale, _XFONT_ALIGNTYPE aligntype, TCHAR* args, ...)
{	
	if( !args ) return;
	
	TCHAR strVBuffer[512];   	
	va_list ap;   
	va_start(ap, args);   
	vsprintf(strVBuffer, args, ap);   
	va_end(ap);
		
	if( aligntype == _XFONT_ALIGNTYPE_LEFT  )
	{
		Puts( x, y, strVBuffer, scale );
	}
	else if( aligntype == _XFONT_ALIGNTYPE_RIGHT  )
	{	
		x -= GetWidth( strVBuffer );
		Puts( x, y, strVBuffer, scale );
	}
	else if( aligntype == _XFONT_ALIGNTYPE_CENTER  )
	{
		x -= (GetWidth( strVBuffer ) >> 1);
		Puts( x, y, strVBuffer, scale );		
	}
}

int	_XFontManager::Puts_SeparateAlign(int x, int y, LPCTSTR str, int width, _XFONT_ALIGNTYPE aligntype, float scale, int addlinepitch )
{
	if( !str ) return 0;

	LPCTSTR	curpoint = str;	
	TCHAR	buffer[2048];
	int		separatedlinecount = 0;
	int		insertpoint = 0;
	int     height = y;
	memset( buffer, 0, 2048 );
	
	while( 1 )
	{		
		if( *curpoint == '\0'|| *curpoint == '\n' )
		{
			separatedlinecount++;
			PrintAlign( x, height, scale, aligntype, buffer );
			break;
		}
		
		if (*curpoint&0x80) // check mbcs...
		{			
			buffer[insertpoint] = *curpoint;
			buffer[insertpoint+1] = *(curpoint+1);
			
			if( m_font->CalcWidth( buffer ) > width )
			{
				buffer[insertpoint] = 0;
				buffer[insertpoint+1] = 0;				
				PrintAlign( x, height, scale, aligntype, buffer );
				memset( buffer, 0, 2048 );
				insertpoint = 0;
				height += GetHeight() + addlinepitch;
				separatedlinecount++;				
			}
			else
			{
				insertpoint+=2;
				curpoint+=2;
			}		
		}
		else
		{
			if( *curpoint == '\t' )
			{
				curpoint++;
			}
			else
			{
				buffer[insertpoint] = *curpoint;
				
				if( m_font->CalcWidth( buffer ) > width )
				{
					buffer[insertpoint] = 0;
					PrintAlign( x, height, scale, aligntype, buffer );
					memset( buffer, 0, 2048 );
					insertpoint = 0;
					height += GetHeight() + addlinepitch;
					separatedlinecount++;				
				}
				else
				{
					insertpoint++;
					curpoint++;			
				}		
			}
		}		
	}	
	
	return separatedlinecount;
}

void _XFontManager::PrintClipping(int x, int y, int width, TCHAR* args, ...)
{
	if( !args ) return;

	TCHAR strVBuffer[2048];
	va_list ap;   
	va_start(ap, args);   
	vsprintf(strVBuffer, args, ap);   
	va_end(ap);
	
	LPCTSTR	curpoint = strVBuffer;	
	TCHAR	buffer[2048];
	int		separatedlinecount = 0;
	int		insertpoint = 0;
	int     height = y;
	memset( buffer, 0, 2048 );
	
	while( 1 )
	{		
		if( *curpoint == '\0'|| *curpoint == '\n' )
		{
			separatedlinecount++;
			Puts( x, height, buffer, 1.0f );
			break;
		}
		
		if (*curpoint&0x80) // check mbcs...
		{			
			buffer[insertpoint] = *curpoint;
			buffer[insertpoint+1] = *(curpoint+1);
			
			if( m_font->CalcWidth( buffer ) > width )
			{
				buffer[insertpoint] = 0;
				buffer[insertpoint+1] = 0;
				Puts( x, height, buffer, 1.0f );
				memset( buffer, 0, 2048 );				
				return;
			}
			else
			{
				insertpoint+=2;
				curpoint+=2;
			}		
		}
		else
		{
			if( *curpoint == '\t' )
			{
				curpoint++;
			}
			else
			{			
				buffer[insertpoint] = *curpoint;
				
				if( m_font->CalcWidth( buffer ) > width )
				{
					buffer[insertpoint] = 0;
					Puts( x, height, buffer, 1.0f );
					return;
				}
				else
				{
					insertpoint++;
					curpoint++;			
				}		
			}
		}		
	}	
}

int	_XFontManager::Puts_Separate(int x, int y, LPCTSTR str, int width, float scale, int addlinepitch )
{
	if( !str ) return 0;

	LPCTSTR	curpoint = str;	
	TCHAR	buffer[2048];
	int		separatedlinecount = 0;
	int		insertpoint = 0;
	int     height = y;
	memset( buffer, 0, 2048 );

	while( 1 )
	{		
		if( *curpoint == '\0'|| *curpoint == '\n' )
		{
			separatedlinecount++;
			Puts( x, height, buffer, scale );
			break;
		}

		if (*curpoint&0x80) // check mbcs...
		{			
			buffer[insertpoint] = *curpoint;
			buffer[insertpoint+1] = *(curpoint+1);

			if( m_font->CalcWidth( buffer ) > width )
			{
				buffer[insertpoint] = 0;
				buffer[insertpoint+1] = 0;
				Puts( x, height, buffer, scale );
				memset( buffer, 0, 2048 );
				insertpoint = 0;
				height += GetHeight() + addlinepitch;
				separatedlinecount++;				
			}
			else
			{
				insertpoint+=2;
				curpoint+=2;
			}		
		}
		else
		{
			if( *curpoint == '\t' )
			{
				curpoint++;
			}
			else
			{			
				buffer[insertpoint] = *curpoint;

				if( m_font->CalcWidth( buffer ) > width )
				{
					buffer[insertpoint] = 0;
					Puts( x, height, buffer, scale );
					memset( buffer, 0, 2048 );
					insertpoint = 0;
					height += GetHeight() + addlinepitch;
					separatedlinecount++;				
				}
				else
				{
					insertpoint++;
					curpoint++;			
				}		
			}
		}		
	}	

	return separatedlinecount;
}

int	_XFontManager::Puts_SeparateNewLineVersion(int x, int y, LPCTSTR str, int width, float scale, int addlinepitch )
{
	if( !str ) return 0;
	
	LPCTSTR	curpoint = str;	
	TCHAR	buffer[2048];
	int		separatedlinecount = 0;
	int		insertpoint = 0;
	int     height = y;
	memset( buffer, 0, 2048 );
	
	while( 1 )
	{		
		if( *curpoint == '\0' )
		{
			separatedlinecount++;
			Puts( x, height, buffer, scale );
			break;
		}
		else if( *curpoint == '\n' )
		{
			if( insertpoint == 0 )
			{
				curpoint+=1;
				buffer[insertpoint] = 0;
				buffer[insertpoint+1] = 0;
				Puts( x, height, buffer, scale );
				memset( buffer, 0, 2048 );
				insertpoint = 0;
				height += GetHeight() + addlinepitch;
			}
			else
			{
				curpoint+=1;
				buffer[insertpoint] = 0;
				buffer[insertpoint+1] = 0;
				Puts( x, height, buffer, scale );
				memset( buffer, 0, 2048 );
				insertpoint = 0;
				height += GetHeight() + addlinepitch;
			}
			separatedlinecount++;
		}
		else if (*curpoint&0x80) // check mbcs...
		{			
			buffer[insertpoint] = *curpoint;
			buffer[insertpoint+1] = *(curpoint+1);
			
			if( m_font->CalcWidth( buffer ) > width )
			{
				buffer[insertpoint] = 0;
				buffer[insertpoint+1] = 0;
				Puts( x, height, buffer, scale );
				memset( buffer, 0, 2048 );
				insertpoint = 0;
				height += GetHeight() + addlinepitch;
				separatedlinecount++;				
			}
			else
			{
				insertpoint+=2;
				curpoint+=2;
			}		
		}
		else
		{
			if( *curpoint == '\t' )
			{
				curpoint++;
			}
			else
			{			
				buffer[insertpoint] = *curpoint;
				
				if( m_font->CalcWidth( buffer ) > width )
				{
					buffer[insertpoint] = 0;
					Puts( x, height, buffer, scale );
					memset( buffer, 0, 2048 );
					insertpoint = 0;
					height += GetHeight() + addlinepitch;
					separatedlinecount++;				
				}
				else
				{
					insertpoint++;
					curpoint++;			
				}		
			}
		}		
	}	
	
	return separatedlinecount;
}

int	_XFontManager::GetRowCount( const TCHAR* str, int interval )
{	
	if( !str ) return 0;
	
	LPCTSTR	curpoint = str;	
	TCHAR	buffer[2048];
	int		separatedlinecount = 0;
	int		insertpoint = 0;
	memset( buffer, 0, 2048 );
	
	while( 1 )
	{		
		if( *curpoint == '\0'|| *curpoint == '\n' )
		{
			separatedlinecount++;
			break;
		}
		
		if (*curpoint&0x80) // check mbcs...
		{			
			buffer[insertpoint] = *curpoint;
			buffer[insertpoint+1] = *(curpoint+1);
			
			if( m_font->CalcWidth( buffer ) > interval )
			{
				buffer[insertpoint] = 0;
				buffer[insertpoint+1] = 0;				
				memset( buffer, 0, 2048 );
				insertpoint = 0;
				separatedlinecount++;				
			}
			else
			{
				insertpoint+=2;
				curpoint+=2;
			}		
		}
		else
		{
			if( *curpoint == '\t' )
			{
				curpoint++;
			}
			else
			{			
				buffer[insertpoint] = *curpoint;
				
				if( m_font->CalcWidth( buffer ) > interval )
				{
					buffer[insertpoint] = 0;
					memset( buffer, 0, 2048 );
					insertpoint = 0;
					separatedlinecount++;				
				}
				else
				{
					insertpoint++;
					curpoint++;			
				}		
			}
		}		
	}	
	
	return separatedlinecount;
}

int	_XFontManager::GetRowCountNewLineVersion( const TCHAR* str, int interval ) 
{	
	if( !str ) return 0;
	
	LPCTSTR	curpoint = str;	
	TCHAR	buffer[2048];
	int		separatedlinecount = 0;
	int		insertpoint = 0;
	memset( buffer, 0, 2048 );
	
	while( 1 )
	{		
		if( *curpoint == '\0')
		{
			separatedlinecount++;
			break;
		}
		else if( *curpoint == '\n' )
		{
			if( insertpoint == 0 )
			{
				curpoint+=1;
				insertpoint = 0;
			}
			else
			{
				curpoint+=1;
				insertpoint = 0;
			}
			separatedlinecount++;
		}
		else if (*curpoint&0x80) // check mbcs...
		{			
			buffer[insertpoint] = *curpoint;
			buffer[insertpoint+1] = *(curpoint+1);
			
			if( m_font->CalcWidth( buffer ) > interval )
			{
				buffer[insertpoint] = 0;
				buffer[insertpoint+1] = 0;				
				memset( buffer, 0, 2048 );
				insertpoint = 0;
				separatedlinecount++;				
			}
			else
			{
				insertpoint+=2;
				curpoint+=2;
			}		
		}
		else
		{
			if( *curpoint == '\t' )
			{
				curpoint++;
			}
			else
			{			
				buffer[insertpoint] = *curpoint;
				
				if( m_font->CalcWidth( buffer ) > interval )
				{
					buffer[insertpoint] = 0;
					memset( buffer, 0, 2048 );
					insertpoint = 0;
					separatedlinecount++;				
				}
				else
				{
					insertpoint++;
					curpoint++;			
				}		
			}
		}		
	}	
	
	return separatedlinecount;
}

void _XFontManager::SplitString(LPCTSTR str, int width, _XStringSplitInfo& splitinfo, int tabwidth, _XStringSplitFunction splitfunction )
{
	memset( &splitinfo, 0, sizeof(_XStringSplitInfo) );
	if( !str ) return;

	LPCTSTR	curpoint = str;
	int		prevspacecharpoint = -1;
	TCHAR	buffer[2048];
	
	int		insertpoint = 0;	
	memset( buffer, 0, 2048 );

	splitinfo.splitpoint[0] = (LPTSTR)curpoint;
	splitinfo.splitcount = 0;
	
	while( 1 )
	{
		if( *curpoint == '\0'|| *curpoint == '\n' )
		{	
			splitinfo.splitlength[splitinfo.splitcount] = strlen(buffer);
			splitinfo.splitcount++;			
			break;
		}
		
		if (*curpoint&0x80) // check mbcs...
		{
			if( *curpoint == ' ' ) prevspacecharpoint = (curpoint - str);

			buffer[insertpoint] = *curpoint;
			buffer[insertpoint+1] = *(curpoint+1);
			
			if( m_font->CalcWidth( buffer ) > width )
			{
				if( splitinfo.splitcount == 0 )
				{
					width -= tabwidth;
					if( width < 0 ) width  = 0;
				}

				if( splitfunction == _XSTRINGSPLITFUNCTION_WIDTH  )
				{
					buffer[insertpoint] = 0;
					buffer[insertpoint+1] = 0;					
				}
				else //if( splitfunction == _XSTRINGSPLITFUNCTION_SPACECHAR )				
				{
					if( prevspacecharpoint == -1 )
					{
						buffer[insertpoint] = 0;
						buffer[insertpoint+1] = 0;

						prevspacecharpoint = (curpoint - str);
					}
					else
					{
						if( (curpoint - str) - prevspacecharpoint > 5 )
						{
							buffer[insertpoint] = 0;
							prevspacecharpoint = (curpoint - str);
						}
						else
						{
							int i = 0 ; for(  i = insertpoint; i >= prevspacecharpoint;  i-- ) buffer[i] = 0;					
							curpoint -= (insertpoint - prevspacecharpoint) - 1;
						}
					}				
				}

				splitinfo.splitlength[splitinfo.splitcount] = strlen(buffer);
				
				if( splitinfo.splitcount+1 >= _XDEF_STRINGSPRITINFOMAXSIZE )
				{
					splitinfo.splitcount++;
					break;
				}
				
				splitinfo.splitcount++;
				splitinfo.splitpoint[splitinfo.splitcount] = (LPTSTR)curpoint;
				
				memset( buffer, 0, 2048 );
				insertpoint = 0;
				
			}
			else
			{
				insertpoint+=2;
				curpoint+=2;
			}		
		}
		else
		{
			if( *curpoint == ' ' ) prevspacecharpoint = (curpoint - str);

			buffer[insertpoint] = *curpoint;
						
			if( m_font->CalcWidth( buffer ) > width )
			{
				if( splitinfo.splitcount == 0 )
				{
					width -= tabwidth;
					if( width < 0 ) width  = 0;
				}

				if( splitfunction == _XSTRINGSPLITFUNCTION_WIDTH )
				{
					buffer[insertpoint] = 0;					
				}
				else //if( splitfunction == _XSTRINGSPLITFUNCTION_SPACECHAR )				
				{
					if( prevspacecharpoint == -1 )
					{
						buffer[insertpoint] = 0;						
						prevspacecharpoint = (curpoint - str);
					}
					else
					{						
						if( (curpoint - str) - prevspacecharpoint > 5 )
						{
							buffer[insertpoint] = 0;
							prevspacecharpoint = (curpoint - str);
						}
						else
						{
							int i = 0 ; for(  i = insertpoint; i >= prevspacecharpoint;  i-- ) buffer[i] = 0;
							curpoint -= (insertpoint - prevspacecharpoint) - 1;						
						}
					}					
					
				}

				splitinfo.splitlength[splitinfo.splitcount] = strlen(buffer);
				
				if( splitinfo.splitcount+1 >= _XDEF_STRINGSPRITINFOMAXSIZE )
				{
					splitinfo.splitcount++;
					break;
				}
				
				splitinfo.splitcount++;
				splitinfo.splitpoint[splitinfo.splitcount] = (LPTSTR)curpoint;				
				
				memset( buffer, 0, 2048 );
				insertpoint = 0;

			}
			else
			{
				insertpoint++;
				curpoint++;			
			}		
		}		
	}
}

void _XFontManager::SplitStringByCR(LPCTSTR str, _XStringSplitInfo& splitinfo)
{
	memset( &splitinfo, 0, sizeof(_XStringSplitInfo) );
	if( !str ) return;

	LPCTSTR	curpoint = str;	
	TCHAR	buffer[2048];
	
	int		insertpoint = 0;	
	memset( buffer, 0, 2048 );
	

	splitinfo.splitpoint[0] = (LPTSTR)curpoint;
	splitinfo.splitcount = 0;
	
	while( 1 )
	{
		if( *curpoint == '\0' )
		{	
			splitinfo.splitlength[splitinfo.splitcount] = strlen(buffer);
			splitinfo.splitcount++;			
			break;
		}
		
		buffer[insertpoint] = *curpoint;
					
		if( *curpoint == '\n' )
		{				
			buffer[insertpoint] = 0;
			
			splitinfo.splitlength[splitinfo.splitcount] = strlen(buffer);
			
			if( splitinfo.splitcount+1 >= _XDEF_STRINGSPRITINFOMAXSIZE )
			{
				splitinfo.splitcount++;
				break;
			}
			
			curpoint++;
			splitinfo.splitcount++;
			splitinfo.splitpoint[splitinfo.splitcount] = (LPTSTR)(curpoint);				
			
			memset( buffer, 0, 2048 );
			insertpoint = 0;
		}
		else
		{
			insertpoint++;
			curpoint++;			
		}
	}	
}

