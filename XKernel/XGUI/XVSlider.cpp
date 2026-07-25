// XVSlider.cpp: implementation of the _XVSlider class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XVSlider::_XVSlider()
{
	m_SlidingPosition = 0;
	m_GaugeValue	  = 0.0f;
	m_GabSize		  = 0;
	m_InverseIndicate = FALSE;
}

_XVSlider::~_XVSlider()
{

}


BOOL _XVSlider::Create( _XSLIDERCONTROL_STRUCTURE& sliderwindowstruct )
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

	m_IndicatorImage.SetDrawSize( m_IndicatorImage.m_ClipRect.left, m_IndicatorImage.m_ClipRect.top, 
								  m_IndicatorImage.m_ClipRect.right,
								  m_IndicatorImage.m_ClipRect.top + m_SlidingPosition + m_TrackBarButtonOffset.y );
		
	return TRUE;
}

void _XVSlider::DestroyWindow( void )
{

}

void _XVSlider::MoveWindow( int X, int Y )
{
	_XImageStatic::MoveWindow( X, Y );
	m_IndicatorImage.MoveWindow( X, Y );				 
	m_TrackBarButton.MoveWindow( X+m_TrackBarButtonOffset.x, Y+m_TrackBarButtonOffset.y+m_SlidingPosition );
}

void _XVSlider::Draw( _XGUIObject*& pfocusobject )
{
	if( !this->m_Enable     ) return;	
	
	if( this->m_ShowWindow )
	{
		m_IndicatorImage.DrawWithRegion();
	}
	
	_XImageStatic::Draw(pfocusobject);
	
	if( this->m_ShowWindow )
	{
		m_TrackBarButton.Draw( pfocusobject );
	}		

	//g_XBaseFont->Print( this->m_WindowPosition.x, m_WindowPosition.y, 1.0f, "%d %f",  m_SlidingPosition,m_GaugeValue  );
	//g_XBaseFont->Flush();
}

BOOL _XVSlider::Process( _XGUIObject*& pfocusobject )
{
	if( !this->m_Enable     ) return FALSE;
	if( !this->m_ShowWindow ) return FALSE;

	//if( !_XImageStatic::Process(pfocusobject) ) return FALSE;
	
	m_TrackBarButton.Process( pfocusobject );

	if( (pfocusobject == this) || (pfocusobject == &m_TrackBarButton) )
	{
		ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
		int pos = scrnpos->z - (m_WindowPosition.y+m_TrackBarButtonSize.cy);
		if( pos < 0 ) pos = 0;
		
		m_SlidingPosition = pos;
		if( m_SlidingPosition > m_WindowSize.cy-m_TrackBarButtonSize.cy )
			m_SlidingPosition = m_WindowSize.cy-m_TrackBarButtonSize.cy;

		if( m_GabSize > 0 )
		{
			m_SlidingPosition = m_SlidingPosition - (m_SlidingPosition % m_GabSize);
		}

		m_TrackBarButton.MoveWindow( m_TrackBarButton.GetWindowPos().x, m_WindowPosition.y + m_TrackBarButtonOffset.y + m_SlidingPosition );
				
		if(  m_InverseIndicate )
		{
			 m_GaugeValue = 100.0f - ((FLOAT)m_SlidingPosition / (FLOAT)(m_WindowSize.cy-m_TrackBarButtonSize.cy)) * 100.0f;

			 m_IndicatorImage.SetDrawSize( m_IndicatorImage.m_ClipRect.left, m_IndicatorImage.m_ClipRect.bottom-m_SlidingPosition, 
										   m_IndicatorImage.m_ClipRect.right,m_IndicatorImage.m_ClipRect.bottom );

			 m_IndicatorImage.MoveWindow( m_IndicatorImage.GetWindowPos().x, m_WindowPosition.y + m_TrackBarButtonOffset.y + m_SlidingPosition );
		}
		else
		{
			m_IndicatorImage.SetDrawSize( m_IndicatorImage.m_ClipRect.left, m_IndicatorImage.m_ClipRect.top, 
										  m_IndicatorImage.m_ClipRect.right,
										  m_IndicatorImage.m_ClipRect.top + m_SlidingPosition + m_TrackBarButtonOffset.y );
						
			m_GaugeValue = ((FLOAT)m_SlidingPosition / (FLOAT)(m_WindowSize.cy-m_TrackBarButtonSize.cy)) * 100.0f;
		}
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
				if( pfocusobject == (_XGUIObject*)this || (pfocusobject == &m_TrackBarButton)  )
				{
					if( m_CommandID )
					{						
						PostMessage(gHWnd, WM_COMMAND, MAKEWPARAM(m_CommandID, 0), *((LPARAM*)&m_GaugeValue));
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

BOOL _XVSlider::CheckMousePosition( void )
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