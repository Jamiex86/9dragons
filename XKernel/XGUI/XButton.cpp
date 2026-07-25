#include "stdafx.h"
#include "XKernel.h"

_XButton::_XButton()
{		
	m_MessageMode			= _BTNMESSAGEMODE_BTNUP;
	m_TooltipCallbackFunc	= NULL;
	m_ButtonText			= NULL;
	m_ButtonTextCategory	= -1;
	m_ButtonTextID			= -1;
	m_TextPosition.x		= 0;			
	m_TextPosition.y		= 0;
	m_TextColor				= D3DCOLOR_ARGB( 128, 160,160,160 );
	m_TextHighlightColor	= D3DCOLOR_ARGB( 255, 255,255,255 );
	//m_pXFont				= g_XBaseFont;
	m_FontID				= _XFONTID_BASE;
	m_TextOffset.x			= 0;
	m_TextOffset.y			= 0;
	m_TextAlign				= _XFONT_ALIGNTYPE_CENTER;
	
	m_RenderFontEdge		= FALSE;
	m_FontEdgeColor			= 0xFF000000;
	m_ButtonTextClipping	= FALSE;
}


_XButton::~_XButton()
{
	DestroyWindow();
}

BOOL _XButton::Create( _XBTN_STRUCTURE& btnstruct )
{		
	this->m_Enable = btnstruct.enable;	
	m_CommandID	   = btnstruct.commandid;
	
	m_WindowPosition = btnstruct.position;
	m_WindowSize     = btnstruct.windowsize;

	if( btnstruct.imageindex_normal != -1 )
	{
		m_btnimage_normal.Create( m_WindowPosition.x, m_WindowPosition.y, 
								  m_WindowPosition.x+m_WindowSize.cx-1, 
								  m_WindowPosition.y+m_WindowSize.cy-1, 
								  btnstruct.texturearchive, btnstruct.imageindex_normal );		
	}
	
	if( btnstruct.imageindex_click != -1 )
	{
		m_btnimage_click.Create(  m_WindowPosition.x, m_WindowPosition.y, 
								  m_WindowPosition.x+m_WindowSize.cx-1, 
								  m_WindowPosition.y+m_WindowSize.cy-1, 
								  btnstruct.texturearchive, btnstruct.imageindex_click );		
	}
	
	if( btnstruct.imageindex_ready != -1 )
	{
		m_btnimage_ready.Create(  m_WindowPosition.x, m_WindowPosition.y, 
								  m_WindowPosition.x+m_WindowSize.cx-1, 
							      m_WindowPosition.y+m_WindowSize.cy-1,
								  btnstruct.texturearchive, btnstruct.imageindex_ready );		
	}
	
	m_FontID	=	_XFONTID_BASE;
	
	return TRUE;
}

void _XButton::DestroyWindow( void )
{	
	m_Enable		= FALSE;
	m_ShowWindow	= TRUE;

	m_WindowPosition.x = 0;
	m_WindowPosition.y = 0;

	m_WindowSize.cx = 0;
	m_WindowSize.cy = 0;

	m_CommandID = NULL;

	m_TooltipCallbackFunc = NULL;
	SAFE_DELETE_ARRAY( m_ButtonText );

	m_ButtonTextID = -1;
	m_ButtonTextCategory = -1;
}

void _XButton::SetButtonImage( int normalindex, int clickindex, int readyindex, _XTextureManager* texturearchive )
{		
	m_btnimage_normal.m_TextureIndex = normalindex;
	m_btnimage_click.m_TextureIndex	 = clickindex;	
	m_btnimage_ready.m_TextureIndex	 = readyindex;

	if( texturearchive )
	{
		m_btnimage_normal.m_pTextureArchive = texturearchive;
		m_btnimage_click.m_pTextureArchive  = texturearchive;
		m_btnimage_ready.m_pTextureArchive  = texturearchive;
	}
}

void _XButton::SetButtonTextID( int buttontextcategory, int buttontextid, _XFONT_ALIGNTYPE align, int xoffset, int yoffset )
{
	SAFE_DELETE_ARRAY( m_ButtonText );

	m_TextPosition.x		= 0;			
	m_TextPosition.y		= 0;
	
	m_ButtonTextCategory = buttontextcategory;
	m_ButtonTextID = buttontextid;	

	LPTSTR pText = g_StringDataBase.GetTextString( (_XSTRINGDB_SECTIONDEFINE)m_ButtonTextCategory, m_ButtonTextID );

	if( m_ButtonTextID > -1 )
	{
		int len = strlen( pText );
	
		if( !pText || len <= 0 ) return;

		_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );
		
		if( align == _XFONT_ALIGNTYPE_LEFT )
		{
			m_TextPosition.x		= m_WindowPosition.x + xoffset;
		}
		else if( align == _XFONT_ALIGNTYPE_CENTER )
		{
			int width  = pXFont->GetWidth( pText );
			m_TextPosition.x		= m_WindowPosition.x + ( (m_WindowSize.cx >> 1) - (width >> 1) ) + xoffset;			
		}
		else if( align == _XFONT_ALIGNTYPE_RIGHT )
		{
			int width  = pXFont->GetWidth( pText );
			m_TextPosition.x = m_WindowPosition.x + m_WindowSize.cx - width + xoffset;
		}
		
		m_TextPosition.y		= m_WindowPosition.y + (m_WindowSize.cy >> 1) - (pXFont->GetHeight() >> 1) + 1 + yoffset;
	}
		
	m_TextAlign	   = align;
	m_TextOffset.x = xoffset;
	m_TextOffset.y = yoffset;
}

void _XButton::SetButtonText( LPTSTR buttontext, _XFONT_ALIGNTYPE align, int xoffset, int yoffset )
{
	m_ButtonTextID = -1;
	m_ButtonTextCategory = -1;

	SAFE_DELETE_ARRAY( m_ButtonText );
	m_TextPosition.x		= 0;			
	m_TextPosition.y		= 0;
	
	int len = strlen( buttontext );

	if( !buttontext || len <= 0 ) return;
		
	m_ButtonText = new TCHAR[len+1];

	if( !m_ButtonText )
	{
		_XFatalError( "Could't allocate memory : button text" );
	}
	else
	{
		_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );

		memset( m_ButtonText, 0, len+1 );
		strncpy( m_ButtonText, buttontext, len );

		if( align == _XFONT_ALIGNTYPE_LEFT )
		{
			m_TextPosition.x		= m_WindowPosition.x + xoffset;			
		}
		else if( align == _XFONT_ALIGNTYPE_CENTER )
		{
			int width  = pXFont->GetWidth( m_ButtonText );
			m_TextPosition.x		= m_WindowPosition.x + ( (m_WindowSize.cx >> 1) - (width >> 1) ) + xoffset;			
		}
		else if( align == _XFONT_ALIGNTYPE_RIGHT )
		{
			int width  = pXFont->GetWidth( m_ButtonText );
			m_TextPosition.x = m_WindowPosition.x + m_WindowSize.cx - width + xoffset;
		}

		m_TextPosition.y		= m_WindowPosition.y + (m_WindowSize.cy >> 1) - (pXFont->GetHeight() >> 1) + 1 + yoffset;
	}		

	m_TextAlign	   = align;
	m_TextOffset.x = xoffset;
	m_TextOffset.y = yoffset;
}

BOOL _XButton::CheckMousePosition(void)
{	
	if( !this->m_Enable ) return FALSE;

	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();

	if( scrnpos )
	{	
		if( m_Enable )
		{
			if(m_btnimage_normal.m_TextureIndex != -1 )
				return m_btnimage_normal.TestAlphaChannel( scrnpos->x, scrnpos->z );			
			else if(m_btnimage_click.m_TextureIndex != -1 )
				return m_btnimage_click.TestAlphaChannel( scrnpos->x, scrnpos->z );
			else if(m_btnimage_ready.m_TextureIndex != -1 )
				return m_btnimage_ready.TestAlphaChannel( scrnpos->x, scrnpos->z );
			else
			{
				if( (scrnpos->x>m_WindowPosition.x) && (scrnpos->x<m_WindowPosition.x + m_WindowSize.cx ) && 
					(scrnpos->z>m_WindowPosition.y) && (scrnpos->z<m_WindowPosition.y + m_WindowSize.cy ) )
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void _XButton::MoveWindow( int X, int Y )
{
	m_WindowPosition.x = X;
	m_WindowPosition.y = Y;

	if( m_btnimage_normal.m_TextureIndex != -1 )
		m_btnimage_normal.MoveWindow( X, Y );
	
	if( m_btnimage_click.m_TextureIndex != -1 )
		m_btnimage_click.MoveWindow( X, Y );
	
	if( m_btnimage_ready.m_TextureIndex != -1 )
		m_btnimage_ready.MoveWindow( X, Y );

	LPTSTR pText = NULL;

	if( m_ButtonTextID > -1 )
	{
		pText = g_StringDataBase.GetTextString( (_XSTRINGDB_SECTIONDEFINE)m_ButtonTextCategory, m_ButtonTextID );
	}
	else
	{
		pText = m_ButtonText;
	}

	if( pText )	
	{	
		_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );

		if( m_TextAlign == _XFONT_ALIGNTYPE_LEFT )
		{
			m_TextPosition.x		= m_WindowPosition.x + m_TextOffset.x;			
		}
		else if( m_TextAlign == _XFONT_ALIGNTYPE_CENTER )
		{
			int width  = pXFont->GetWidth( pText );
			m_TextPosition.x		= m_WindowPosition.x + ( (m_WindowSize.cx >> 1) - (width >> 1) ) + m_TextOffset.x;			
		}
		else if( m_TextAlign == _XFONT_ALIGNTYPE_RIGHT )
		{
			int width  = pXFont->GetWidth( pText );
			m_TextPosition.x = m_WindowPosition.x + m_WindowSize.cx - width + m_TextOffset.x;
		}
		
		m_TextPosition.y		= m_WindowPosition.y + (m_WindowSize.cy >> 1) - (pXFont->GetHeight() >> 1) + 1 + m_TextOffset.y;
	}
}

void _XButton::SetWindowSize( int width, int height )
{ 
	m_WindowSize.cx = width; 
	m_WindowSize.cy = height; 

	if( m_btnimage_normal.m_TextureIndex != -1 )
		m_btnimage_normal.SetWindowSize( width, height );
	
	if( m_btnimage_click.m_TextureIndex != -1 )
		m_btnimage_click.SetWindowSize( width, height );
	
	if( m_btnimage_ready.m_TextureIndex != -1 )
		m_btnimage_ready.SetWindowSize( width, height );
}

void _XButton::SetButtonImageClipRect( _XButtonImageClass buttontype, int left, int top, int right, int bottom )
{
	RECT rect = {  left,  top,  right,  bottom };

	switch( buttontype ) 
	{
	default:
	case _XBUTTONIMAGE_NORMAL :
		m_btnimage_normal.m_ClipRect = rect;
		break;
	case _XBUTTONIMAGE_CLICK :
		m_btnimage_click.m_ClipRect = rect;
		break;
	case _XBUTTONIMAGE_READY:
		m_btnimage_ready.m_ClipRect = rect;
		break;	
	}
}
 
void _XButton::DrawButtonText( BOOL disabled, BOOL focused )
{
	LPTSTR pText = NULL;
	
	if( m_ButtonTextID > -1 )
	{
		pText = g_StringDataBase.GetTextString( (_XSTRINGDB_SECTIONDEFINE)m_ButtonTextCategory, m_ButtonTextID );
	}
	else
	{
		pText = m_ButtonText;
	}

	
	if( !pText ) return;

	_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );

	if( m_RenderFontEdge )
	{
		pXFont->SetGrowMode( _XFONT_EDGEMODE_EDGE, m_FontEdgeColor );
		
		if( focused )
		{
			pXFont->SetColor( m_TextHighlightColor );
		}
		else
		{
			pXFont->SetColor( m_TextColor );
		}

		if( m_ButtonTextClipping )
		{
			pXFont->SetClippingArea( m_WindowPosition.x + 2, m_WindowPosition.y + 1, m_WindowPosition.x + m_WindowSize.cx - 2, m_WindowPosition.y + m_WindowSize.cy - 1 );
			pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
			pXFont->Flush();
			pXFont->DisableClippingArea();
		}
		else
		{
			pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
			pXFont->Flush();
		}	

		pXFont->DisableGrowMode();
	}
	else
	{		
		/*
		pXFont->SetColor( D3DCOLOR_ARGB( 160, 0,0,0 ) );
		pXFont->Puts( m_TextPosition.x+1, m_TextPosition.y+1, pText );
		pXFont->Flush();
		*/
		
		if( focused )
		{
			pXFont->SetColor( m_TextHighlightColor );
		}
		else
		{
			pXFont->SetColor( m_TextColor );
		}

		if( m_ButtonTextClipping )
		{
			pXFont->SetClippingArea( m_WindowPosition.x + 2, m_WindowPosition.y + 1, m_WindowPosition.x + m_WindowSize.cx - 2, m_WindowPosition.y + m_WindowSize.cy - 1 );
			pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
			pXFont->Flush();
			pXFont->DisableClippingArea();
		}
		else
		{
			pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
			pXFont->Flush();
		}	
	}
}
	
void _XButton::Draw( _XGUIObject*& pfocusobject )
{
	if( !this->m_ShowWindow ) return;
	if( !this->m_Enable )
	{
		if( m_btnimage_normal.m_TextureIndex != -1 )
		{	
			m_btnimage_normal.DrawWithRegion();				
		} // Disible button half alpha blitting
		
		DrawButtonText( TRUE );

		return;
	}

	if( pfocusobject )
	{
		if( pfocusobject != (_XGUIObject*)this )
		{
			if( m_btnimage_normal.m_TextureIndex != -1 )
			{	
				m_btnimage_normal.DrawWithRegion();					
			} 
			DrawButtonText();
			return;
		}
	}
	
	if( CheckMousePosition() )
	{
		gApp->ReadyCursor( g_SelectCursorIndex );

		MouseState* mousestate = gpInput->GetMouseState();
		if( mousestate )
		{
			if( mousestate->bButton[0] )
			{
				if( pfocusobject == (_XGUIObject*)this )
				{
					 if( m_btnimage_click.m_TextureIndex != -1 ) 
					 {
						 m_btnimage_click.DrawWithRegion();
					 }
				}
				else
				{
					if( m_btnimage_normal.m_TextureIndex != -1 )
					{
						m_btnimage_normal.DrawWithRegion();
					}
				}
			}
			else
			{
				 if( m_btnimage_ready.m_TextureIndex != -1 )
				 {
					 m_btnimage_ready.DrawWithRegion();
				 }
				 else if( m_btnimage_normal.m_TextureIndex != -1 ) 
				 {
					 m_btnimage_normal.DrawWithRegion();
				 }
			}
		}
		
		DrawButtonText( FALSE, TRUE );
		
		if( m_TooltipCallbackFunc && ( ( pfocusobject == (_XGUIObject*)this ) || !pfocusobject ) )
		{
			m_TooltipCallbackFunc( m_WindowPosition, m_WindowSize );
		}
	}
	else
	{
		if( m_btnimage_normal.m_TextureIndex != -1 )
		{
			m_btnimage_normal.DrawWithRegion();			
		}
		DrawButtonText();
	}	
}

BOOL _XButton::Process( _XGUIObject*& pfocusobject )
{
	if( !this->m_ShowWindow ) return FALSE;
	if( !this->m_Enable     ) return FALSE;
		
	MouseState* mousestate = gpInput->GetMouseState();

	if( mousestate )
	{
		if( !pfocusobject )
		{
			if( mousestate->bButton[0] && CheckMousePosition() )
			{
				pfocusobject = (_XGUIObject*)this;
				_XPlayInterfaceSound( g_ClickSoundIndex );
				gApp->ReadyCursor( g_SelectClickCursorIndex );
			}	
		}
	
		if( m_MessageMode == _BTNMESSAGEMODE_BTNUP )
		{			
			if( !mousestate->bButton[0] )
			{
				if( CheckMousePosition() )
				{	
					if( pfocusobject == (_XGUIObject*)this )
					{						
						if( m_CommandID )
						{				
							if( m_CommandID > 0xFFFF )
								PostMessage(gHWnd, WM_COMMAND, (WPARAM)m_CommandID, (LPARAM)m_lParam);
							else
								PostMessage(gHWnd, WM_COMMAND, MAKEWPARAM(m_CommandID, 0), (LPARAM)m_lParam);
							mousestate->bButton[0] = 0;							
						}	
						
						pfocusobject = NULL;
						return TRUE;
					}		
				}
				if( pfocusobject == (_XGUIObject*)this )
				{
					pfocusobject = NULL;
					return FALSE;
				}
			}		
		}
		else if( m_MessageMode == _BTNMESSAGEMODE_BTNDOWN )
		{
			if( mousestate->bButton[0] )
			{
				if( CheckMousePosition() )
				{	
					if( pfocusobject == (_XGUIObject*)this )
					{
						if( m_CommandID )
						{						
							gApp->ReadyCursor( g_SelectCursorIndex );

							if( g_XGUIControlCenter.m_GUITimer.FrameSkip( _XDEF_XTIMER_BUTTONCLICK, _XDEF_XTIMER_GUIPROCESSTICK ) )
							{
								PostMessage(gHWnd, WM_COMMAND, MAKEWPARAM(m_CommandID, 0), (LPARAM)m_lParam);
							}
							//mousestate->bButton[0] = 0;
						}						
						
						return TRUE;
					}		
				}			
			}	
			else if( pfocusobject == (_XGUIObject*)this )
			{
				pfocusobject = NULL;
				return FALSE;
			}			
		}
	}

	return FALSE;
}


LPTSTR _XButton::GetButtonText( void )
{
	if( m_ButtonTextID > -1 )
	{
		return g_StringDataBase.GetTextString( (_XSTRINGDB_SECTIONDEFINE)m_ButtonTextCategory, m_ButtonTextID );
	}
	else
	{
		return m_ButtonText;
	}

	return NULL;
}

void _XButton::NotifyUIChange( void )
{
	LPTSTR pText = NULL;
	
	if( m_ButtonTextID > -1 )
	{
		pText = g_StringDataBase.GetTextString( (_XSTRINGDB_SECTIONDEFINE)m_ButtonTextCategory, m_ButtonTextID );
	}
	else
	{
		pText = m_ButtonText;
	}
	
	if( pText )	
	{	
		_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );
		
		if( m_TextAlign == _XFONT_ALIGNTYPE_LEFT )
		{
			m_TextPosition.x		= m_WindowPosition.x + m_TextOffset.x;			
		}
		else if( m_TextAlign == _XFONT_ALIGNTYPE_CENTER )
		{
			int width  = pXFont->GetWidth( pText );
			m_TextPosition.x		= m_WindowPosition.x + ( (m_WindowSize.cx >> 1) - (width >> 1) ) + m_TextOffset.x;			
		}
		else if( m_TextAlign == _XFONT_ALIGNTYPE_RIGHT )
		{
			int width  = pXFont->GetWidth( pText );
			m_TextPosition.x = m_WindowPosition.x + m_WindowSize.cx - width + m_TextOffset.x;
		}
		
		m_TextPosition.y		= m_WindowPosition.y + (m_WindowSize.cy >> 1) - (pXFont->GetHeight() >> 1) + 1 + m_TextOffset.y;
	}	
}