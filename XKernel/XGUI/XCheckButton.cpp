// XCheckButton.cpp: implementation of the _XCheckButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XCheckButton::_XCheckButton()
{
	m_Checked				= FALSE;
	m_TooltipCallbackFunc	= NULL;
	
	m_ButtonText			= NULL;
	m_ButtonTextCategory	= -1;
	m_ButtonTextID			= -1;

	m_TextPosition.x		= 0;			
	m_TextPosition.y		= 0;		
	m_SwapMode				= FALSE;	

	m_TextColor				= _XSC_DEFAULT;
	m_TextHightlightColor   = _XSC_DEFAULT_HIGHLIGHT;	
	m_TextMouseOverColor	= 0xFFB2B2B2;

	m_RenderFontEdge		= FALSE;
	m_FontEdgeColor			= 0xFF000000;

	m_FontID				= _XFONTID_BASE;
	
	m_ButtonTextClipping	= FALSE;
}

_XCheckButton::~_XCheckButton()
{
	DestroyWindow();
}

BOOL  _XCheckButton::Create( _XCHECKBTN_STRUCTURE& checkbtnstruct )
{
	this->m_Enable = checkbtnstruct.enable;
	m_CommandID	   = checkbtnstruct.commandid;
	
	m_WindowPosition = checkbtnstruct.position;
	m_WindowSize     = checkbtnstruct.windowsize;
	
	if( checkbtnstruct.imageindex_normal != -1 )
	{
		m_btnimage_normal.Create( m_WindowPosition.x, m_WindowPosition.y, 
			m_WindowPosition.x+m_WindowSize.cx-1, 
			m_WindowPosition.y+m_WindowSize.cy-1, 
			checkbtnstruct.texturearchive, checkbtnstruct.imageindex_normal );		
	}
	
	if( checkbtnstruct.imageindex_checked != -1 )
	{
		m_btnimage_checked.Create(  m_WindowPosition.x, m_WindowPosition.y, 
			m_WindowPosition.x+m_WindowSize.cx-1, 
			m_WindowPosition.y+m_WindowSize.cy-1, 
			checkbtnstruct.texturearchive, checkbtnstruct.imageindex_checked );		
	}
			
	m_FontID	=	_XFONTID_BASE;

	return TRUE;
}

void  _XCheckButton::DestroyWindow( void )
{
	m_Enable				= FALSE;
	m_ShowWindow			= TRUE;
	m_Checked				= FALSE;
	
	m_WindowPosition.x		= 0;
	m_WindowPosition.y		= 0;
	
	m_WindowSize.cx			= 0;
	m_WindowSize.cy			= 0;
	
	m_CommandID				= NULL;
	
	m_TooltipCallbackFunc	= NULL;

	m_ButtonTextID			= -1;
	m_ButtonTextCategory	= -1;
	SAFE_DELETE_ARRAY( m_ButtonText );
}

void  _XCheckButton::Draw( _XGUIObject*& pfocusobject )
{
	if( !this->m_ShowWindow ) return;

	if( !this->m_Enable )
	{
		if( m_btnimage_normal.m_TextureIndex != -1 )
		{	
			if( m_SwapMode )
			{
				if( m_Checked )
				{
					m_btnimage_checked.DrawWithRegion();
				}
				else
				{
					m_btnimage_normal.DrawWithRegion();
				}
			}
			else
			{
				m_btnimage_normal.DrawWithRegion();	
				if( m_Checked )
				{
					m_btnimage_checked.DrawWithRegion();
				}
			}

			DrawButtonText();
		}
		return;
	}

	if( m_SwapMode )
	{
		if( m_Checked )
		{
			m_btnimage_checked.DrawWithRegion();
		}
		else
		{
			m_btnimage_normal.DrawWithRegion();
		}
		
		DrawButtonText();
	}
	else
	{
		m_btnimage_normal.DrawWithRegion();

		if( m_Checked )
		{
			m_btnimage_checked.DrawWithRegion();
		}	

		DrawButtonText();
	}
			
	if( CheckMousePosition() )
	{
		gApp->ReadyCursor( g_SelectCursorIndex );

		if( m_TooltipCallbackFunc  )
		{
			m_TooltipCallbackFunc( m_WindowPosition, m_WindowSize );
		}
	}
}

BOOL  _XCheckButton::Process( _XGUIObject*& pfocusobject )
{
	if( !this->m_ShowWindow ) return FALSE;
	if( !this->m_Enable     ) return FALSE;
	
	MouseState* mousestate = gpInput->GetMouseState();
	
	if( mousestate )
	{
		if( !pfocusobject )
		{
			if( mousestate->bButton[0] && CheckMousePosition()  )
			{
				gApp->ReadyCursor( g_SelectClickCursorIndex );
				pfocusobject = (_XGUIObject*)this;				
			}	
		}
		
		if( !mousestate->bButton[0] )
		{
			if( CheckMousePosition() )
			{	
				if( pfocusobject == (_XGUIObject*)this )
				{		
					_XPlayInterfaceSound( g_ClickSoundIndex );
					m_Checked = !m_Checked;

					if( m_CommandID )
					{							
						PostMessage(gHWnd, WM_COMMAND, (WPARAM)m_CommandID, (LPARAM)m_Checked );						
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
	
	return FALSE;	
}

void  _XCheckButton::MoveWindow( int X, int Y )
{
	m_WindowPosition.x = X;
	m_WindowPosition.y = Y;
	
	if( m_btnimage_normal.m_TextureIndex != -1 )
		m_btnimage_normal.MoveWindow( X, Y );
	
	if( m_btnimage_checked.m_TextureIndex != -1 )
		m_btnimage_checked.MoveWindow( X, Y );

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

		int width  = pXFont->GetWidth( pText );
		m_TextPosition.x		= m_WindowPosition.x + ( (m_WindowSize.cx >> 1) - (width >> 1) );
		m_TextPosition.y		= m_WindowPosition.y + (m_WindowSize.cy >> 1) - (pXFont->GetHeight() >> 1);
	}		
}

BOOL  _XCheckButton::CheckMousePosition( void )
{
	if( !this->m_Enable ) return FALSE;
	
	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
	
	if( scrnpos )
	{	
		if( m_SwapMode )
		{
			BOOL checked = FALSE;
			if(m_btnimage_checked.m_TextureIndex != -1 )
					checked = m_btnimage_checked.TestAlphaChannel( scrnpos->x, scrnpos->z );			

			if( !checked )
			{
				if(m_btnimage_normal.m_TextureIndex != -1 )
					return m_btnimage_normal.TestAlphaChannel( scrnpos->x, scrnpos->z );
			}

			return checked;
		}
		else
		{
			if(m_btnimage_normal.m_TextureIndex != -1 )
				return m_btnimage_normal.TestAlphaChannel( scrnpos->x, scrnpos->z );			
			else if(m_btnimage_checked.m_TextureIndex != -1 )
				return m_btnimage_checked.TestAlphaChannel( scrnpos->x, scrnpos->z );						
		}		
	}
	
	return FALSE;
}

void _XCheckButton::SetButtonImage( int normalindex, int checkedindex, _XTextureManager* texturearchive )
{
	m_btnimage_normal.m_TextureIndex	 = normalindex;
	m_btnimage_checked.m_TextureIndex	 = checkedindex;		

	if( texturearchive )
	{
		m_btnimage_normal.m_pTextureArchive   = texturearchive;
		m_btnimage_checked.m_pTextureArchive  = texturearchive;		
	}
}

void _XCheckButton::SetButtonTextID( int buttontextcategory, int buttontextid )
{
	m_TextPosition.x		= 0;			
	m_TextPosition.y		= 0;
	
	SAFE_DELETE_ARRAY( m_ButtonText );

	m_ButtonTextCategory = buttontextcategory;
	m_ButtonTextID = buttontextid;

	LPTSTR pText = g_StringDataBase.GetTextString( (_XSTRINGDB_SECTIONDEFINE)m_ButtonTextCategory, m_ButtonTextID );

	if( m_ButtonTextID > -1 )
	{
		int len = strlen( pText );
		
		if( !pText || len <= 0 ) return;
		
		_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );

		int width  = pXFont->GetWidth( pText );
		m_TextPosition.x		= m_WindowPosition.x + ( (m_WindowSize.cx >> 1) - (width >> 1) );
		m_TextPosition.y		= m_WindowPosition.y + (m_WindowSize.cy >> 1) - (pXFont->GetHeight() >> 1);
	}
}

void _XCheckButton::SetButtonText( LPTSTR buttontext )
{
	m_ButtonTextID			= -1;
	m_ButtonTextCategory	= -1;

	SAFE_DELETE_ARRAY( m_ButtonText );

	m_TextPosition.x		= 0;			
	m_TextPosition.y		= 0;
	
	int len = strlen( buttontext );
	
	if( !buttontext || len <= 0 ) return;
	
	m_ButtonText = new TCHAR[len+1];
	
	if( !m_ButtonText )
	{
		_XFatalError( "Can't allocate memory : check button text string" );
	}
	else
	{
		memset( m_ButtonText, 0, len+1 );
		strncpy( m_ButtonText, buttontext, len );
		
		_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );

		int width  = pXFont->GetWidth( m_ButtonText );
		m_TextPosition.x		= m_WindowPosition.x + ( (m_WindowSize.cx >> 1) - (width >> 1) );
		m_TextPosition.y		= m_WindowPosition.y + (m_WindowSize.cy >> 1) - (pXFont->GetHeight() >> 1);
	}		
}


void _XCheckButton::SetButtonImageClipRect( _XCheckButtonImageClass buttontype, int left, int top, int right, int bottom )
{
	RECT rect = {  left,  top,  right,  bottom };
	
	switch( buttontype ) 
	{
	default:
	case _XCHECKBUTTONIMAGE_NORMAL :
		m_btnimage_normal.m_ClipRect = rect;
		break;
	case _XCHECKBUTTONIMAGE_CLICK :
		m_btnimage_checked.m_ClipRect = rect;
		break;	
	}
}

void _XCheckButton::DrawButtonText( BOOL disabled )
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

	if( m_ButtonTextClipping )
	{
		pXFont->SetClippingArea( m_WindowPosition.x + 2, m_WindowPosition.y + 1, m_WindowPosition.x + m_WindowSize.cx - 2, m_WindowPosition.y + m_WindowSize.cy - 1 );
	}

	if( m_RenderFontEdge )
	{
		pXFont->SetGrowMode( _XFONT_EDGEMODE_EDGE, m_FontEdgeColor );

		if( m_Checked )
		{
			pXFont->SetColor( m_TextHightlightColor );
			pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
		}
		else
		{			
			if( CheckMousePosition() )
			{
				pXFont->SetColor( m_TextMouseOverColor );
				pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
			}
			else
			{
				pXFont->SetColor( m_TextColor );
				pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
			}
		}
		pXFont->Flush();

		pXFont->DisableGrowMode();
	}
	else
	{
		if( m_Checked )
		{			
			pXFont->SetColor( m_TextHightlightColor );
			pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
		}
		else
		{	
			if( CheckMousePosition() )
			{
				pXFont->SetColor( m_TextMouseOverColor );
				pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
			}
			else
			{
				pXFont->SetColor( m_TextColor );
				pXFont->Puts( m_TextPosition.x, m_TextPosition.y, pText );
			}
		}
		pXFont->Flush();
	}

	if( m_ButtonTextClipping )
	{
		pXFont->DisableClippingArea();
	}
}

LPTSTR _XCheckButton::GetButtonText( void )
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

void _XCheckButton::NotifyUIChange( void )
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
		int len = strlen( pText );
		
		if( !pText || len <= 0 ) return;
		
		_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );
		
		int width  = pXFont->GetWidth( pText );
		m_TextPosition.x		= m_WindowPosition.x + ( (m_WindowSize.cx >> 1) - (width >> 1) );
		m_TextPosition.y		= m_WindowPosition.y + (m_WindowSize.cy >> 1) - (pXFont->GetHeight() >> 1);
	}
}