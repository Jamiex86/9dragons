// XToolTip.cpp: implementation of the _XToolTip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XToolTip::_XToolTip()
{
	m_AlphaLevel = 255;
	m_TextBold = FALSE;
}

_XToolTip::~_XToolTip()
{

}

BOOL _XToolTip::Create( _XTOOLTIP_STRUCTURE tooltipstructure )
{
	m_WindowPosition = tooltipstructure.position;
	m_WindowSize     = tooltipstructure.windowsize;
	m_TextOffset	 = tooltipstructure.textoffset;
	
	if( tooltipstructure.imageindex_border != -1 )
	{
		m_Border.Create( 0, 0, 32, 32, tooltipstructure.texturearchive, tooltipstructure.imageindex_border );		
	}

	memset( m_TextString, 0, sizeof(TCHAR)*512 );
	strcpy( m_TextString, _T(" ") );

	m_TextColor = _XSC_INFORMATION;

	return TRUE;
}

void _XToolTip::DestroyWindow( void )
{
	m_Border.DestroyWindow();
}

void _XToolTip::Draw( _XGUIObject*& pfocusobject )
{
	if( !m_ShowWindow ) return;

	if( m_Border.m_TextureIndex != -1 ) m_Border.Draw();	
	else
	{
		_XDrawSolidBar(  m_WindowPosition.x-3, m_WindowPosition.y-3, 
						 m_WindowPosition.x + m_WindowSize.cx + 3, m_WindowPosition.y+m_WindowSize.cy +2, 
						 D3DCOLOR_ARGB(178,0,0,0) );
		_XDrawRectAngle( m_WindowPosition.x-3, m_WindowPosition.y-3, 
						 m_WindowPosition.x + m_WindowSize.cx + 3, m_WindowPosition.y+m_WindowSize.cy +2, 
						 1.0f, D3DCOLOR_ARGB(255,123,112,88) );
	}
	
	if( m_TextBold )
	{
		g_XBaseFont->SetColor(m_TextColor);
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x+1, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Flush();
	}
	else
	{
		g_XBaseFont->SetColor(m_TextColor);
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Flush();
	}
}


void _XToolTip::SetTooltipAlphaLevel( DWORD alphalevel )
{
	m_AlphaLevel = alphalevel;
	m_TextColor = (m_TextColor & 0xFFFFFF) | (alphalevel<<24);
	m_Border.m_FColor = 0xFFFFFF | (alphalevel<<24);
}

void _XToolTip::DrawAlpha( void )
{
	if( !m_ShowWindow ) return;

	if( m_Border.m_TextureIndex != -1 ) m_Border.Draw();
	else
	{
		_XDrawSolidBar( m_WindowPosition.x-3, m_WindowPosition.y-3, 
						 m_WindowPosition.x + m_WindowSize.cx + 3, m_WindowPosition.y+m_WindowSize.cy +2,
			D3DCOLOR_ARGB(178,0,0,0) );
		_XDrawRectAngle( m_WindowPosition.x-3, m_WindowPosition.y-3, 
						 m_WindowPosition.x + m_WindowSize.cx + 3, m_WindowPosition.y+m_WindowSize.cy +2,
			1.0f, D3DCOLOR_ARGB(255,123,112,88) );
	}

	POINT pos = m_Border.GetWindowPos();	
	g_XBaseFont->SetColor(m_TextColor);
	g_XBaseFont->Puts( pos.x+m_TextOffset.x, pos.y+m_TextOffset.y, m_TextString );
	g_XBaseFont->Flush();
}

void _XToolTip::DrawText( void )
{
	if( !m_ShowWindow ) return;
	
	POINT pos = m_Border.GetWindowPos();
	g_XBaseFont->SetColor(m_TextColor);
	g_XBaseFont->Puts( pos.x+m_TextOffset.x, pos.y+m_TextOffset.y, m_TextString );
	g_XBaseFont->Flush();
}

#ifdef _XTS_PK
void _XToolTip::DrawNoEdge(int bordercolor)
{
	if( !m_ShowWindow ) return;
	
	if( m_Border.m_TextureIndex != -1 ) m_Border.Draw();	
	else
	{
		D3DCOLOR color;
		
		if(bordercolor == 0)
		{
			// normal
			color = D3DCOLOR_ARGB(76, 0, 0, 0);
		}
		else if(bordercolor == 1)
		{
			// enemy
			color = D3DCOLOR_ARGB(76, 255, 0, 0);
		}
		else if(bordercolor == 2)
		{
			// party
			color = D3DCOLOR_ARGB(128,0,102,212);
		}
		else if(bordercolor == 3)
		{
			// no group
			color = D3DCOLOR_ARGB(76,180, 180, 20);
		}
		else if(bordercolor == 4)
		{
			// friend
			color = D3DCOLOR_ARGB(76, 20, 180, 20);
		}
		else if(bordercolor == 5)
		{
			// fame
			color = D3DCOLOR_ARGB(196, 0, 0, 0);
		}
		
		_XDrawSolidBar(  m_WindowPosition.x-3, m_WindowPosition.y-3, 
			m_WindowPosition.x + m_WindowSize.cx + 3, m_WindowPosition.y+m_WindowSize.cy +2, color );
	}
	
	if( m_TextBold )
	{
		g_XBaseFont->SetColor(m_TextColor);
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x+1, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Flush();
	}
	else
	{
		g_XBaseFont->SetColor(m_TextColor);
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Flush();
	}
}
#else
void _XToolTip::DrawNoEdge(BOOL bEnemy)
{
	if( !m_ShowWindow ) return;
	
	if( m_Border.m_TextureIndex != -1 ) m_Border.Draw();	
	else
	{
		if(bEnemy)
		{
			_XDrawSolidBar(  m_WindowPosition.x-3, m_WindowPosition.y-3, 
				m_WindowPosition.x + m_WindowSize.cx + 3, m_WindowPosition.y+m_WindowSize.cy +2, 
				D3DCOLOR_ARGB(76,255,0,0) );
		}
		else
		{
			_XDrawSolidBar(  m_WindowPosition.x-3, m_WindowPosition.y-3, 
				m_WindowPosition.x + m_WindowSize.cx + 3, m_WindowPosition.y+m_WindowSize.cy +2, 
				D3DCOLOR_ARGB(76,0,0,0) );
		}
	}
	
	if( m_TextBold )
	{
		g_XBaseFont->SetColor(m_TextColor);
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x+1, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Flush();
	}
	else
	{
		g_XBaseFont->SetColor(m_TextColor);
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Flush();
	}
}
#endif


void _XToolTip::DrawParty(void)
{
	if( !m_ShowWindow ) return;
	
	if( m_Border.m_TextureIndex != -1 ) m_Border.Draw();	

	_XDrawSolidBar(  m_WindowPosition.x-3, m_WindowPosition.y-3, 
		m_WindowPosition.x + m_WindowSize.cx + 3, m_WindowPosition.y+m_WindowSize.cy +2, 
		D3DCOLOR_ARGB(128,0,102,212 ));
	
	if( m_TextBold )
	{
		g_XBaseFont->SetColor(m_TextColor);
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x+1, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Flush();
	}
	else
	{
		g_XBaseFont->SetColor(m_TextColor);
		g_XBaseFont->Puts( m_WindowPosition.x+m_TextOffset.x, m_WindowPosition.y+m_TextOffset.y, m_TextString );
		g_XBaseFont->Flush();
	}
}


BOOL _XToolTip::Process( _XGUIObject*& pfocusobject )
{
	
	return TRUE;
}

void _XToolTip::MoveWindow( int X, int Y )
{
	if( m_Border.m_TextureIndex != -1 ) m_Border.MoveWindow( X, Y );

	m_WindowPosition.x = X;
	m_WindowPosition.y = Y;
}

BOOL _XToolTip::CheckMousePosition( void )
{

	return TRUE;
}

void _XToolTip::SetToolTipString( int x, int y, LPTSTR message, D3DCOLOR textcolor, BOOL centeralign, BOOL checkclipping )
{
	memset( m_TextString, 0, sizeof(TCHAR)*512 );
	if( strlen(message) ) strcpy( m_TextString, message );
	m_TextColor = textcolor;
	m_CenterAlign = centeralign;
	m_CheckClipping = checkclipping;
	m_ToolTipPosition.x = x;
	m_ToolTipPosition.y = y;

	SIZE size;
	size.cx = g_XBaseFont->GetWidth( message );
	size.cx = size.cx + (m_TextOffset.x<<1);
	size.cy = g_XBaseFont->GetHeight() + (m_TextOffset.y<<1) - 1;

	if( centeralign ) 
	{
		x -= (size.cx>>1);
	}

	m_WindowSize = size;

	if( checkclipping )
	{
		if( x < 0 ){ x = 0; }
		if( y < 0 ){ y = 0; }
		if( x + m_WindowSize.cx >= gnWidth  ){ x = gnWidth  - m_WindowSize.cx - 1; }
		if( y + m_WindowSize.cy >= gnHeight ){ y = gnHeight - m_WindowSize.cy - 1; }
	}

	if( m_Border.m_TextureIndex != -1 )
	{
		m_WindowPosition.x = x;
		m_WindowPosition.y = y;
		m_Border.SetScale( size.cx/32.0f, size.cy/32.0f );
		m_Border.MoveWindow( x, y );			
	}
	else
	{		
		MoveWindow( x, y );
	}

	m_ShowWindow = TRUE;
}

void _XToolTip::SetToolTipStringEx( int x, int y, LPTSTR message, D3DCOLOR textcolor, int height, BOOL centeralign, BOOL checkclipping )
{
	memset( m_TextString, 0, sizeof(TCHAR)*512 );
	if( strlen(message) ) strcpy( m_TextString, message );
	m_TextColor = textcolor;
	m_CenterAlign = centeralign;
	m_CheckClipping = checkclipping;
	m_ToolTipPosition.x = x;
	m_ToolTipPosition.y = y;

	SIZE size;
	size.cx = g_XBaseFont->GetWidth( message );
	size.cx = size.cx + (m_TextOffset.x<<1);
	size.cy = g_XBaseFont->GetHeight() + (m_TextOffset.y<<1) - 1;
	
	if( centeralign ) 
	{
		x -= (size.cx>>1);
	}
	
	m_WindowSize = size;

	if( checkclipping )
	{
		if( x < 0 ){ x = 0; }
		if( y < 0 ){ y = 0; }
		if( x + m_WindowSize.cx >= gnWidth  ){ x = gnWidth  - m_WindowSize.cx - 1; }
		if( y + m_WindowSize.cy >= gnHeight ){ y = gnHeight - m_WindowSize.cy - 1; }
	}
	
	if( m_Border.m_TextureIndex != -1 )
	{		
		m_WindowPosition.x = x;
		m_WindowPosition.y = y;
		m_Border.SetScale( size.cx/32.0f, size.cy/32.0f );
		m_Border.MoveWindow( x, y );	
	}
	else
	{	
		MoveWindow( x, y );
	}

	m_ShowWindow = TRUE;
}

void _XToolTip::SetToolTipStringEx_Noresize( int x, int y, LPTSTR message, D3DCOLOR textcolor  )
{
	memset( m_TextString, 0, sizeof(TCHAR)*512 );
	if( strlen(message) ) strcpy( m_TextString, message );
	m_TextColor = textcolor;
	
	m_TextOffset.x = (m_WindowSize.cx>>1) - (g_XBaseFont->GetWidth( message )>>1);
	m_TextOffset.y = (m_WindowSize.cy>>1) - (g_XBaseFont->GetHeight()>>1);
	
	x -= (m_WindowSize.cx>>1);
	y -= m_WindowSize.cy;
	
	m_Border.MoveWindow( x, y );
	MoveWindow( x, y );
		
	m_ShowWindow = TRUE;
}

void _XToolTip::NotifyUIChange( void )
{
	SIZE size;
	size.cx = g_XBaseFont->GetWidth( m_TextString );
	size.cx = size.cx + (m_TextOffset.x<<1);
	size.cy = g_XBaseFont->GetHeight() + (m_TextOffset.y<<1) - 1;
	
	int x = m_ToolTipPosition.x;
	int y = m_ToolTipPosition.y;

	if( m_CenterAlign ) 
	{
		x -= (size.cx>>1);
	}
	
	m_WindowSize = size;
	
	if( m_CheckClipping )
	{
		if( x < 0 ){ x = 0; }
		if( y < 0 ){ y = 0; }
		if( x + m_WindowSize.cx >= gnWidth  ){ x = gnWidth  - m_WindowSize.cx - 1; }
		if( y + m_WindowSize.cy >= gnHeight ){ y = gnHeight - m_WindowSize.cy - 1; }
	}
	
	if( m_Border.m_TextureIndex != -1 )
	{		
		m_WindowPosition.x = x;
		m_WindowPosition.y = y;
		m_Border.SetScale( size.cx/32.0f, size.cy/32.0f );
		m_Border.MoveWindow( x, y );	
	}
	else
	{	
		MoveWindow( x, y );
	}
}
