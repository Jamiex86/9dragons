// X3DUFont.cpp: implementation of the _X3DUFont class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_X3DUFont::_X3DUFont()
{
	pD3DXNormalFont = NULL;
	fontheight		= 0;
}

_X3DUFont::~_X3DUFont()
{
	Release();
}


BOOL _X3DUFont::Initialize( TCHAR* fontname, int fontsize, DWORD createflag )
{
	Release();
	
	//폰트 설정
	
	D3DXFONT_DESCA lFont;
	ZeroMemory(&lFont, sizeof(LOGFONT));
	fontheight = lFont.Height = fontsize;
	lFont.Width  = 0;
	lFont.Weight = 0;
	lFont.CharSet = DEFAULT_CHARSET,
	lFont.OutputPrecision = OUT_DEFAULT_PRECIS;
	//lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lFont.Quality = DEFAULT_QUALITY;
	lFont.PitchAndFamily = FIXED_PITCH;
	strcpy(lFont.FaceName, fontname );

	if ( FAILED ( D3DXCreateFontIndirect( gpDev, &lFont, &pD3DXNormalFont ) ) ) return FALSE; 	

	/*
	D3DXFONT_DESC fontdesc;
	fontdesc.Height = fontdesc.Width = fontsize;
	fontdesc.MipLevels = 1;
	fontdesc.Italic = FALSE;
	fontdesc.CharSet = DEFAULT_CHARSET;
	fontdesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontdesc.Quality = DEFAULT_QUALITY;
	fontdesc.PitchAndFamily = FIXED_PITCH;
	strcpy(fontdesc.FaceName, fontname );

	if ( FAILED ( D3DXCreateFontIndirect( gpDev, &fontdesc, &pD3DXNormalFont ) ) ) return FALSE; 	
	*/
	
	//if ( FAILED ( D3DXCreateFont( gpDev, (HFONT)GetStockObject(DEFAULT_GUI_FONT), &pD3DXNormalFont ) ) ) return FALSE; 

	return TRUE;
}

void _X3DUFont::Release( void )
{	
	SAFE_RELEASE( pD3DXNormalFont );
}

void _X3DUFont::Puts( int x, int y, TCHAR* string, D3DCOLOR color, DWORD flag, int width, int height)
{	
	if( !string || strlen(string) <= 0 ) return;
	if( !pD3DXNormalFont ) return;
	
	int stringwidth = width;

	if( 0 == width )
	{		
		//stringwidth = ::GetTextExtentPoint(string).cx;
		/*HDC pDC = GetDC( gHWnd );
		if(pDC)
		{
		   SIZE size;
		   GetTextExtentPoint32( pDC, string, strlen( string ), &size);
		   ReleaseDC( gHWnd, pDC);
		   stringwidth = size.cx;
		} */		
	}
	else stringwidth = width;		 

	RECT rect;
	int fheight;
	if(height != 0)
		fheight = height;
	else
		fheight = fontheight;

	SetRect(&rect, x, y, x+stringwidth, y+fheight);
	
	//HDC pDC = GetDC( gHWnd );
	//pD3DXNormalFont->DrawText( pDC, string, strlen(string), &rect, flag, color );
	//ReleaseDC( gHWnd, pDC);

}


void _X3DUFont::Print( int x, int y, D3DCOLOR color, TCHAR* args, ... )
{
	TCHAR strVBuffer[512];   	
	va_list ap;   
	va_start(ap, args);   
	vsprintf(strVBuffer, args, ap);   
	va_end(ap);
	
	Puts( x, y, strVBuffer, color );
}

/*
extern HFONT g_hXWindowFont;
extern HFONT g_hXWindowOldFont;

SIZE _X3DUFont::GetTextExtentPoint( LPSTR string )
{
	SIZE size = { 0, 0 };

	if( !g_hXWindowFont ) return size;
		
	HDC pDC = GetDC( gHWnd );

	if(pDC)
	{			
		g_hXWindowOldFont = (HFONT)SelectObject(pDC, g_hXWindowFont);		
	    GetTextExtentPoint32( pDC, string, strlen( string ), &size);
		if (g_hXWindowOldFont) SelectObject(pDC, g_hXWindowOldFont);
	    ReleaseDC( gHWnd, pDC);
	}	
	return size;
}

*/