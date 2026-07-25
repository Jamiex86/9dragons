// XHSlider.cpp: implementation of the _XHSlider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XHSlider::_XHSlider()
{
	m_SlidingPosition = 0;
	m_PrevGaugeValue  = m_GaugeValue = 0.0f;
	m_GabSize		  = 0;
	m_SpinTrackingSize= 1;
	m_UseSpinButton	  = FALSE;
}

_XHSlider::~_XHSlider()
{

}


BOOL _XHSlider::Create( _XSLIDERCONTROL_STRUCTURE& sliderwindowstruct )
{	
	m_WindowPosition = sliderwindowstruct.position;
	m_WindowSize	 = sliderwindowstruct.windowsize;
	m_CommandID		 = sliderwindowstruct.commandid;		
	_XImageStatic::Create( sliderwindowstruct.position.x, sliderwindowstruct.position.y,
						   sliderwindowstruct.position.x + sliderwindowstruct.windowsize.cx, 
						   sliderwindowstruct.position.y + sliderwindowstruct.windowsize.cy,
						   sliderwindowstruct.texturearchive, sliderwindowstruct.topbordertextureindex );

	m_IndicatorImage.Create( sliderwindowstruct.position.x, sliderwindowstruct.position.y,
							 sliderwindowstruct.position.x + sliderwindowstruct.windowsize.cx, 
							 sliderwindowstruct.position.y + sliderwindowstruct.windowsize.cy,
							 sliderwindowstruct.texturearchive, sliderwindowstruct.indicatortextureindex );

	_XBTN_STRUCTURE trackbarbtnstruct = { TRUE, 
		{ sliderwindowstruct.trackbarbaseoffset.x, sliderwindowstruct.trackbarbaseoffset.y }, 
		{ sliderwindowstruct.trackbarsize.cx, sliderwindowstruct.trackbarsize.cy }, -1,
		sliderwindowstruct.trackbarnormaltextureindex,
		sliderwindowstruct.trackbarpushtextureindex,
		sliderwindowstruct.trackbarhighlighttextureindex, 
		sliderwindowstruct.texturearchive };

	m_TrackBarButtonOffset = sliderwindowstruct.trackbarbaseoffset;
	m_TrackBarButtonSize   = sliderwindowstruct.trackbarsize;
	m_TrackBarButton.Create( trackbarbtnstruct );
	m_TrackBarButton.setMessageMode( _BTNMESSAGEMODE_BTNDOWN );

	if( m_pTextureArchive )
	{
		_XTexture* pTexture = m_pTextureArchive->GetTextureObject( m_TextureIndex );
		if( pTexture )
		{
			_XImageStatic::SetClipRect( 0, 0, m_WindowSize.cx, m_WindowSize.cy );
			//_XImageStatic::SetScale( m_WindowSize.cx / pTexture->m_width, m_WindowSize.cy / pTexture->m_height );
		}
	}

	m_IndicatorImage.SetDrawSize( m_IndicatorImage.m_ClipRect.left, m_IndicatorImage.m_ClipRect.top, 
								  m_IndicatorImage.m_ClipRect.left + m_SlidingPosition + m_TrackBarButtonOffset.x,m_IndicatorImage.m_ClipRect.bottom );
		
	return TRUE;
}

void _XHSlider::DestroyWindow( void )
{

}

void _XHSlider::MoveWindow( int X, int Y )
{	
	_XImageStatic::MoveWindow( X, Y );

	/*
	if( m_pTextureArchive )
	{
		_XTexture* pTexture = m_pTextureArchive->GetTextureObject( m_TextureIndex );
		if( pTexture )
		{
			_XImageStatic::SetClipRect( 0, 0, m_WindowSize.cx, m_WindowSize.cy );
			//_XImageStatic::SetScale( m_WindowSize.cx / pTexture->m_width, m_WindowSize.cy / pTexture->m_height );
		}
	}
	*/

	m_IndicatorImage.MoveWindow( X, Y );	
	m_TrackBarButton.MoveWindow( X+m_TrackBarButtonOffset.x+m_SlidingPosition, Y+m_TrackBarButtonOffset.y );

	m_SpinLeftButton.MoveWindow( m_WindowPosition.x - m_SpinLeftButton.GetWindowSize().cx, m_WindowPosition.y );
	m_SpinRightButton.MoveWindow( m_WindowPosition.x + m_WindowSize.cx, m_WindowPosition.y );
}

void _XHSlider::Draw( _XGUIObject*& pfocusobject )
{
	//if( !this->m_Enable     ) return;	
	
	if( this->m_ShowWindow )
	{
		m_IndicatorImage.DrawWithRegion();
		
		_XImageStatic::DrawWithRegion();

		if( m_UseSpinButton )
		{
			m_SpinLeftButton.Draw( pfocusobject );
			m_SpinRightButton.Draw( pfocusobject );
		}
		
		m_TrackBarButton.Draw( pfocusobject );
	}		
	
	//g_XBaseFont->Print( this->m_WindowPosition.x, m_WindowPosition.y, 1.0f, "%d %f",  m_SlidingPosition,m_GaugeValue  );
	//g_XBaseFont->Flush();	
}

BOOL _XHSlider::Process( _XGUIObject*& pfocusobject )
{
	if( !this->m_Enable     ) return FALSE;
	if( !this->m_ShowWindow ) return FALSE;

	//if( !_XImageStatic::Process(pfocusobject) ) return FALSE;
	
	m_TrackBarButton.Process( pfocusobject );
	
	if( m_UseSpinButton )
	{
		if( m_SpinLeftButton.Process( pfocusobject ) )
		{
			m_SlidingPosition -= m_SpinTrackingSize;
			if( m_SlidingPosition < 0 )
				m_SlidingPosition = 0;
			
			if( m_GabSize > 0 )
			{
				m_SlidingPosition = m_SlidingPosition - (m_SlidingPosition % m_GabSize);
			}

			m_TrackBarButton.MoveWindow( m_WindowPosition.x + m_TrackBarButtonOffset.x + m_SlidingPosition, m_TrackBarButton.GetWindowPos().y );		
			FLOAT factor = ((FLOAT)m_SlidingPosition / (FLOAT)(m_WindowSize.cx));
			m_GaugeValue = factor * 100.0f;		
			
			m_IndicatorImage.SetDrawSize( m_IndicatorImage.m_ClipRect.left, m_IndicatorImage.m_ClipRect.top, 
				m_IndicatorImage.m_ClipRect.left + m_SlidingPosition,m_IndicatorImage.m_ClipRect.bottom );
		}
			
		if( m_SpinRightButton.Process( pfocusobject ) )
		{
			m_SlidingPosition += m_SpinTrackingSize;
			if( m_SlidingPosition > m_WindowSize.cx )
				m_SlidingPosition = m_WindowSize.cx;
			
			if( m_GabSize > 0 )
			{
				m_SlidingPosition = m_SlidingPosition - (m_SlidingPosition % m_GabSize);
			}
			
			m_TrackBarButton.MoveWindow( m_WindowPosition.x + m_TrackBarButtonOffset.x + m_SlidingPosition, m_TrackBarButton.GetWindowPos().y );
			FLOAT factor = ((FLOAT)m_SlidingPosition / (FLOAT)(m_WindowSize.cx));
			m_GaugeValue = factor * 100.0f;
			
			m_IndicatorImage.SetDrawSize( m_IndicatorImage.m_ClipRect.left, m_IndicatorImage.m_ClipRect.top, 
				m_IndicatorImage.m_ClipRect.left + m_SlidingPosition,m_IndicatorImage.m_ClipRect.bottom );
		}
	}

	if( (pfocusobject == this) || (pfocusobject == &m_TrackBarButton) )
	{
		ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
		int pos = scrnpos->x - (m_WindowPosition.x);
		if( pos < 0 ) pos = 0;
		
		m_SlidingPosition = pos;
		if( m_SlidingPosition > m_WindowSize.cx )
			m_SlidingPosition = m_WindowSize.cx;

		if( m_GabSize > 0 )
		{
			m_SlidingPosition = m_SlidingPosition - (m_SlidingPosition % m_GabSize);
		}

		m_TrackBarButton.MoveWindow( m_WindowPosition.x + m_TrackBarButtonOffset.x + m_SlidingPosition, m_TrackBarButton.GetWindowPos().y );		
		FLOAT factor = ((FLOAT)m_SlidingPosition / (FLOAT)(m_WindowSize.cx));
		m_GaugeValue = factor * 100.0f;		
		
		m_IndicatorImage.SetDrawSize( m_IndicatorImage.m_ClipRect.left, m_IndicatorImage.m_ClipRect.top, 
			                          m_IndicatorImage.m_ClipRect.left + m_SlidingPosition,m_IndicatorImage.m_ClipRect.bottom );
	}
	
	BOOL commandsended = FALSE;
	if( m_PrevGaugeValue != m_GaugeValue )
	{
		if( m_CommandID )
		{						
			PostMessage(gHWnd, WM_COMMAND, MAKEWPARAM(m_CommandID, 0), *((LPARAM*)&m_GaugeValue) );
		}
		
		m_PrevGaugeValue = m_GaugeValue;
		commandsended = TRUE;
	}

	MouseState* mousestate = gpInput->GetMouseState();	
	if( mousestate )
	{		
		if( !pfocusobject || (pfocusobject == &m_TrackBarButton) )
		{
			if( CheckMousePosition() && mousestate->bButton[0] )
			{
				pfocusobject = (_XGUIObject*)this;
			}
		}
		
		if( !mousestate->bButton[0] )
		{
			//if( CheckMousePosition() )
			{	
				if( pfocusobject == (_XGUIObject*)this || (pfocusobject == &m_TrackBarButton) )
				{
					if( m_CommandID )
					{
						if( !commandsended )
							PostMessage(gHWnd, WM_COMMAND, MAKEWPARAM(m_CommandID, 0), *((LPARAM*)&m_GaugeValue) );

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

BOOL _XHSlider::CheckMousePosition( void )
{
	if( !this->m_Enable ) return FALSE;
	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
	
	if( scrnpos )
	{	
		if( m_WindowPosition.x <= scrnpos->x && m_WindowPosition.y <= scrnpos->z &&
			m_WindowPosition.x+m_WindowSize.cx > scrnpos->x && m_WindowPosition.y+m_WindowSize.cy >= scrnpos->z )
			return TRUE;
	}

	return FALSE;
}

void _XHSlider::SetScale(void)
{
	_XImageStatic::SetScale((FLOAT)(m_WindowSize.cx/32.0f), 1.0f);
}


void _XHSlider::CreateSpinButton( SIZE btnsize, _XTextureManager* pTextureArchive, int leftbtntextureindex, int rightbtntextureindex, RECT* leftbuttonrect, RECT* rightbuttonrect )
{
	_XBTN_STRUCTURE leftbtnstruct = { TRUE, { m_WindowPosition.x - btnsize.cx, m_WindowPosition.y }, { btnsize.cx, btnsize.cy }, NULL,
								      leftbtntextureindex,leftbtntextureindex,leftbtntextureindex, pTextureArchive };
	
	m_SpinLeftButton.Create( leftbtnstruct );
	m_SpinLeftButton.SetButtonImageClipRect( _XBUTTONIMAGE_NORMAL, leftbuttonrect[0].left, leftbuttonrect[0].top, leftbuttonrect[0].right, leftbuttonrect[0].bottom );
	m_SpinLeftButton.SetButtonImageClipRect( _XBUTTONIMAGE_READY,  leftbuttonrect[1].left, leftbuttonrect[1].top, leftbuttonrect[1].right, leftbuttonrect[1].bottom );
	m_SpinLeftButton.SetButtonImageClipRect( _XBUTTONIMAGE_CLICK,  leftbuttonrect[2].left, leftbuttonrect[2].top, leftbuttonrect[2].right, leftbuttonrect[2].bottom );


	_XBTN_STRUCTURE rightbtnstruct = { TRUE, { m_WindowPosition.x + m_WindowSize.cx, m_WindowPosition.y }, { btnsize.cx, btnsize.cy }, NULL,
									   rightbtntextureindex,rightbtntextureindex,rightbtntextureindex, pTextureArchive };
	
	m_SpinRightButton.Create( rightbtnstruct );
	m_SpinRightButton.SetButtonImageClipRect( _XBUTTONIMAGE_NORMAL, rightbuttonrect[0].left, rightbuttonrect[0].top, rightbuttonrect[0].right, rightbuttonrect[0].bottom );
	m_SpinRightButton.SetButtonImageClipRect( _XBUTTONIMAGE_READY,  rightbuttonrect[1].left, rightbuttonrect[1].top, rightbuttonrect[1].right, rightbuttonrect[1].bottom );
	m_SpinRightButton.SetButtonImageClipRect( _XBUTTONIMAGE_CLICK,  rightbuttonrect[2].left, rightbuttonrect[2].top, rightbuttonrect[2].right, rightbuttonrect[2].bottom );
}

void _XHSlider::ShowWindow( BOOL show )
{ 
	m_ShowWindow = show;
	
	m_SpinLeftButton.ShowWindow( show );
	m_SpinRightButton.ShowWindow( show );
	
	if( !show )
	{
		if( g_CurrentFocusedObject == this )
		{
			g_CurrentFocusedObject = NULL;
		}
	}
}