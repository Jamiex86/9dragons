// XStickGauge.cpp: implementation of the _XStickGauge class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"
#include "XImageStatic.h"
#include "XFastCode.h"
#include "XButton.h"
#include "XStickGauge.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern _XFontManager_Baseclass* g_XSmallFont;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XStickGauge::_XStickGauge()
{
	m_TooltipCallbackFunc		= NULL;
	m_FactorDrawCallbackFunc	= NULL;
	m_VerticalMode				= FALSE;
	m_bBackGroundDraw			= FALSE;
	m_GaugeProgressType			= _XGAUGEPROGRESSTYPE_FORWARD;
	m_ImageDrawType				= _XGAUGEIMAGEDRAWTYPE_SCALE;
	m_DelayUpdate				= FALSE;
	m_PrevDelayTimer			= 0;
	m_DelaySpeed				= 30; // msec...

	m_GaugeFactor				= -1.0f;
	m_GaugeImageScaleFactor		= 1.0f;
	m_DelayGaugeFactor			= 0.0f;

	m_Blinking					= FALSE;
	m_BlinkingPosition			= 10;
	
	m_bFadeOutMode				= FALSE;
	m_FadeOutStartTime			= 0;
	
	memset( m_WindowText, 0, 128 );
	strcpy( m_WindowText, _T("0") );
	memset( m_WindowTextFormat, 0, 64 );
	strcpy( m_WindowTextFormat, _T("%d") );
}

_XStickGauge::~_XStickGauge()
{
	DestroyWindow();
}

BOOL _XStickGauge::Create( _XSTICKGAUGE_STRUCTURE& gaugestruct )
{		
	this->m_Enable = gaugestruct.enable;	
	
	m_WindowPosition = gaugestruct.position;
	m_WindowSize     = gaugestruct.windowsize;
	
	if( gaugestruct.imageindex_border != -1 )
	{
		m_BorderStatic.Create( m_WindowPosition.x, m_WindowPosition.y, 
							   m_WindowPosition.x+m_WindowSize.cx-1, 
							   m_WindowPosition.y+m_WindowSize.cy-1, 
							   gaugestruct.texturearchive, gaugestruct.imageindex_border );

		m_DelayBorderStatic.Create( m_WindowPosition.x, m_WindowPosition.y, 
									m_WindowPosition.x+m_WindowSize.cx-1, 
									m_WindowPosition.y+m_WindowSize.cy-1, 
									gaugestruct.texturearchive, gaugestruct.imageindex_border );
	}

	if( gaugestruct.imageindex_overlay != -1 )
	{
		m_OverlayStatic.Create( m_WindowPosition.x, m_WindowPosition.y, 
							    m_WindowPosition.x+m_WindowSize.cx-1, 
			                    m_WindowPosition.y+m_WindowSize.cy-1, 
			                    gaugestruct.texturearchive, gaugestruct.imageindex_overlay );
	}	

	SIZE Size;
	Size.cx = g_XSmallFont->GetWidth( m_WindowText );
	Size.cy = g_XSmallFont->GetHeight();
	m_TextPosition.x = (FLOAT) ( m_WindowPosition.x + (m_WindowSize.cx>>1) - (Size.cx >> 1) );
	m_TextPosition.y = (FLOAT) ( m_WindowPosition.y + (m_WindowSize.cy>>1) - (Size.cy >> 1) );
	
	return TRUE;
}

void _XStickGauge::DestroyWindow( void )
{	
	m_Enable		= FALSE;
	m_ShowWindow	= TRUE;
	
	m_WindowPosition.x = 0;
	m_WindowPosition.y = 0;
	
	m_WindowSize.cx = 0;
	m_WindowSize.cy = 0;
			
	m_TooltipCallbackFunc = NULL;	
}

BOOL _XStickGauge::CheckMousePosition( void )
{
	if( !this->m_Enable ) return FALSE;
	
	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
	
	if( scrnpos )
	{	
		if( m_Enable )
		{
			if( (scrnpos->x > m_WindowPosition.x) && ( scrnpos->x < m_WindowPosition.x + m_WindowSize.cx ) && 
				(scrnpos->y > m_WindowPosition.y) && ( scrnpos->y < m_WindowPosition.y + m_WindowSize.cy ) )
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

void _XStickGauge::SetImage( int borderindex, int overlayindex, _XTextureManager* texturearchive )
{		
	m_BorderStatic.m_TextureIndex			= borderindex;
	m_DelayBorderStatic.m_TextureIndex		= borderindex;
	m_OverlayStatic.m_TextureIndex			= overlayindex;
	
	if( texturearchive )
	{
		m_BorderStatic.m_pTextureArchive		= texturearchive;
		m_DelayBorderStatic.m_pTextureArchive	= texturearchive;
		m_OverlayStatic.m_pTextureArchive		= texturearchive;
	}
}

void _XStickGauge::SetImageClipRect( _XGaugeImageClass imagetype, int left, int top, int right, int bottom )
{
	RECT rect = {  left,  top,  right,  bottom };

	m_ImageClipRect = rect;
	
	switch( imagetype ) 
	{
	default:
	case _XGAUGEIMAGE_BORDER :
		m_BorderStatic.m_ClipRect		= rect;
		m_DelayBorderStatic.m_ClipRect	= rect;
		break;
	case _XGAUGEIMAGE_OVERLAY :
		m_OverlayStatic.m_ClipRect		= rect;
		break;	
	}
}

void _XStickGauge::MoveWindow( int X, int Y )
{
	m_WindowPosition.x = X;
	m_WindowPosition.y = Y;

	if( m_BorderStatic.m_TextureIndex != -1 )
	{
		m_BorderStatic.MoveWindow( X, Y );
		m_DelayBorderStatic.MoveWindow( X, Y );
	}

	if( m_OverlayStatic.m_TextureIndex != -1 )
	{
		if( m_bBackGroundDraw )
			m_OverlayStatic.MoveWindow( X-1, Y );// 2004.06.18->oneway48 insert
		else
			m_OverlayStatic.MoveWindow( X, Y );
	}	

	if( !m_FactorDrawCallbackFunc )
	{
		SIZE Size;
		Size.cx = g_XSmallFont->GetWidth( m_WindowText );		
		Size.cy = g_XSmallFont->GetHeight();
		m_TextPosition.x = (FLOAT) ( m_WindowPosition.x + (m_WindowSize.cx>>1) - (Size.cx >> 1) );
		m_TextPosition.y = (FLOAT) ( m_WindowPosition.y + (m_WindowSize.cy>>1) - (Size.cy >> 1) );
	}
}


void _XStickGauge::Draw( _XGUIObject*& pfocusobject )
{
	if( !this->m_ShowWindow ) return;
	if( !this->m_Enable )
	{	
		return;
	}

	BOOL blinkdraw = TRUE;

	if( m_Blinking )
	{
		if( m_BlinkingPosition > _XFC_FloatToIntRet( m_GaugeFactor ) )
		{
			if( (gnFrameCounter % 3) == 0 )
			{
				blinkdraw = FALSE;
			}
		}
	}

	if( m_bBackGroundDraw )
	{
		if( m_OverlayStatic.m_TextureIndex != -1 ) m_OverlayStatic.DrawWithRegion();

		if( m_BorderStatic.m_TextureIndex != -1 )
		{
			if( blinkdraw ) m_BorderStatic.DrawWithRegion();

			if( m_DelayUpdate )
			{
				if( m_DelayGaugeFactor != m_GaugeFactor )
					m_DelayBorderStatic.DrawWithRegion();
			}
		}
	}
	else
	{		
		if( m_BorderStatic.m_TextureIndex != -1 )  			
		{
			if( blinkdraw ) m_BorderStatic.DrawWithRegion();

			if( m_DelayUpdate )
			{
				if( m_DelayGaugeFactor != m_GaugeFactor )
					m_DelayBorderStatic.DrawWithRegion();
			}
		}

		if( m_OverlayStatic.m_TextureIndex != -1 ) m_OverlayStatic.DrawWithRegion();
	}
	
	if( m_FactorDrawCallbackFunc )
	{
		m_FactorDrawCallbackFunc( m_WindowPosition, m_WindowSize, m_GaugeFactor );
	}
	else if(m_IsFactorDraw)
	{
		g_XSmallFont->SetColor( _XSC_GAUGENUMBER );
		g_XSmallFont->Puts( m_TextPosition.x, m_TextPosition.y, m_WindowText );
		g_XSmallFont->Flush();		
	}
}

BOOL _XStickGauge::Process( _XGUIObject*& pfocusobject )
{
	if( !this->m_ShowWindow ) return FALSE;
	if( !this->m_Enable     ) return FALSE;

	if( m_DelayUpdate )
	{
		if( fabs( m_DelayGaugeFactor - m_GaugeFactor ) > EPSILON3 )
		{
			if( m_PrevDelayTimer == 0 )
			{
				m_PrevDelayTimer = g_LocalSystemTime;
			}

			if( g_LocalSystemTime - m_PrevDelayTimer > m_DelaySpeed )
			{
				m_PrevDelayTimer = g_LocalSystemTime;
				
				if( m_DelayGaugeFactor > m_GaugeFactor )
				{
					m_DelayGaugeFactor -= 1.0f;

					if( m_DelayGaugeFactor < m_GaugeFactor )
					{
						m_DelayGaugeFactor = m_GaugeFactor;
					}

					int gaugewidth = _XFC_FloatToIntRet(m_WindowSize.cx * (m_DelayGaugeFactor / 100.0f));
					
					if( m_GaugeProgressType == _XGAUGEPROGRESSTYPE_FORWARD )
					{
						m_DelayBorderStatic.MoveWindow( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x + gaugewidth, m_WindowPosition.y + m_WindowSize.cy - 1 );

						if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
						{
							m_DelayBorderStatic.SetScale( (m_DelayGaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
						}
						else
						{
							m_DelayBorderStatic.SetClipRect( m_ImageClipRect.left, m_ImageClipRect.top, m_ImageClipRect.left + gaugewidth, m_ImageClipRect.bottom );
						}
					}
					else
					{
						m_DelayBorderStatic.MoveWindow( m_WindowPosition.x + m_WindowSize.cx - gaugewidth - 1, m_WindowPosition.y, m_WindowPosition.x + m_WindowSize.cx-1, m_WindowPosition.y + m_WindowSize.cy - 1 );

						if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
						{
							m_DelayBorderStatic.SetScale( (m_DelayGaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
						}
						else
						{
							m_DelayBorderStatic.SetClipRect( m_ImageClipRect.left + m_WindowSize.cx - gaugewidth - 1, m_ImageClipRect.top, m_ImageClipRect.right, m_ImageClipRect.bottom );
						}
					}
				}
				else
				{
					m_DelayGaugeFactor += 1.0f;
					
					if( m_DelayGaugeFactor > m_GaugeFactor )
					{
						m_DelayGaugeFactor = m_GaugeFactor;
					}

					int gaugewidth = _XFC_FloatToIntRet(m_WindowSize.cx * (m_DelayGaugeFactor / 100.0f));
					
					if( m_GaugeProgressType == _XGAUGEPROGRESSTYPE_FORWARD )
					{
						m_BorderStatic.MoveWindow( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x + gaugewidth, m_WindowPosition.y + m_WindowSize.cy - 1 );

						if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
						{
							m_BorderStatic.SetScale( (m_DelayGaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
						}
						else
						{
							m_BorderStatic.SetClipRect( m_ImageClipRect.left, m_ImageClipRect.top, m_ImageClipRect.left + gaugewidth, m_ImageClipRect.bottom );
						}
					}
					else
					{
						m_BorderStatic.MoveWindow( m_WindowPosition.x + m_WindowSize.cx - gaugewidth - 1, m_WindowPosition.y, m_WindowPosition.x + m_WindowSize.cx-1, m_WindowPosition.y + m_WindowSize.cy - 1 );

						if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
						{
							m_BorderStatic.SetScale( (m_DelayGaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
						}
						else
						{
							m_BorderStatic.SetClipRect( m_ImageClipRect.left + m_WindowSize.cx - gaugewidth - 1, m_ImageClipRect.top, m_ImageClipRect.right, m_ImageClipRect.bottom );
						}
					}
				}
			}
		}
		else
		{
			m_DelayGaugeFactor = m_GaugeFactor;
		}		
	}

	if( m_bFadeOutMode )
	{
		if( g_LocalSystemTime - m_FadeOutStartTime > 500 )		// 1√ 
		{
			m_bFadeOutMode = FALSE;			
		}
		else
		{
			DWORD alpha = 255 - static_cast<DWORD> ( (g_LocalSystemTime - m_FadeOutStartTime)/500.0f * 255.0f );
			if( alpha < 0 )
				alpha = 0;
			m_BorderStatic.m_FColor = D3DCOLOR_ARGB( alpha, 255, 255, 255 );
			m_OverlayStatic.m_FColor = D3DCOLOR_ARGB( alpha, 255, 255, 255 );
		}		
	}

	return FALSE;
}

void _XStickGauge::SetScyncDelayGaugeFactor( void )
{
	m_DelayGaugeFactor = m_GaugeFactor; 

	int gaugewidth = _XFC_FloatToIntRet(m_WindowSize.cx * (m_GaugeFactor / 100.0f));
	
	if( m_GaugeProgressType == _XGAUGEPROGRESSTYPE_FORWARD )
	{
		m_BorderStatic.MoveWindow( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x + gaugewidth, m_WindowPosition.y + m_WindowSize.cy - 1 );
		
		if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
		{
			m_BorderStatic.SetScale( (m_GaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
		}
		else
		{
			m_BorderStatic.SetClipRect( m_ImageClipRect.left, m_ImageClipRect.top, m_ImageClipRect.left + gaugewidth, m_ImageClipRect.bottom );
		}
	}
	else
	{
		m_BorderStatic.MoveWindow( m_WindowPosition.x + m_WindowSize.cx - gaugewidth - 1, m_WindowPosition.y, m_WindowPosition.x + m_WindowSize.cx-1, m_WindowPosition.y + m_WindowSize.cy - 1 );
		
		if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
		{
			m_BorderStatic.SetScale( (m_GaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
		}
		else
		{
			m_BorderStatic.SetClipRect( m_ImageClipRect.left + m_WindowSize.cx - gaugewidth - 1, m_ImageClipRect.top, m_ImageClipRect.right, m_ImageClipRect.bottom );
		}
	}

}

void _XStickGauge::SetGaugeFactor( FLOAT factor )
{
	if( factor == m_GaugeFactor ) return;

	if( factor < 0.0f )
	{
		m_GaugeFactor = 0.0f;
	}
	else if( factor > 100.0f )
	{
		m_GaugeFactor = 100.0f;
	}
	else
	{
		m_GaugeFactor = factor;
	}
	
	if( m_DelayUpdate )
	{
		if( m_DelayGaugeFactor > m_GaugeFactor ) // decrease
		{
			int gaugewidth = _XFC_FloatToIntRet(m_WindowSize.cx * (m_GaugeFactor / 100.0f));
			int delaygaugewidth = _XFC_FloatToIntRet(m_WindowSize.cx * (m_DelayGaugeFactor / 100.0f));

			if( m_GaugeProgressType == _XGAUGEPROGRESSTYPE_FORWARD )
			{
				m_BorderStatic.MoveWindow( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x + gaugewidth, m_WindowPosition.y + m_WindowSize.cy - 1 );
				
				if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
				{
					m_BorderStatic.SetScale( (m_GaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
				}
				else
				{
					m_BorderStatic.SetClipRect( m_ImageClipRect.left, m_ImageClipRect.top, m_ImageClipRect.left + gaugewidth, m_ImageClipRect.bottom );
				}
			}
			else
			{
				m_BorderStatic.MoveWindow( m_WindowPosition.x + m_WindowSize.cx - gaugewidth - 1, m_WindowPosition.y, m_WindowPosition.x + m_WindowSize.cx-1, m_WindowPosition.y + m_WindowSize.cy - 1 );

				if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
				{
					m_BorderStatic.SetScale( (m_GaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
				}
				else
				{
					m_BorderStatic.SetClipRect( m_ImageClipRect.left + m_WindowSize.cx - gaugewidth - 1, m_ImageClipRect.top, m_ImageClipRect.right, m_ImageClipRect.bottom );
				}
			}
		}
		else // increase
		{
			int gaugewidth = _XFC_FloatToIntRet(m_WindowSize.cx * (m_GaugeFactor / 100.0f));
			int delaygaugewidth = _XFC_FloatToIntRet(m_WindowSize.cx * (m_DelayGaugeFactor / 100.0f));

			if( m_GaugeProgressType == _XGAUGEPROGRESSTYPE_FORWARD )
			{
				m_BorderStatic.MoveWindow( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x + delaygaugewidth, m_WindowPosition.y + m_WindowSize.cy - 1 );
				m_DelayBorderStatic.MoveWindow( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x + gaugewidth, m_WindowPosition.y + m_WindowSize.cy - 1 );

				if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
				{
					m_BorderStatic.SetScale( (m_DelayGaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
					m_DelayBorderStatic.SetScale( (m_GaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
				}
				else
				{
					m_BorderStatic.SetClipRect( m_ImageClipRect.left, m_ImageClipRect.top, m_ImageClipRect.left + delaygaugewidth, m_ImageClipRect.bottom );
					m_DelayBorderStatic.SetClipRect( m_ImageClipRect.left, m_ImageClipRect.top, m_ImageClipRect.left + gaugewidth, m_ImageClipRect.bottom );
				}				
			}
			else
			{
				m_BorderStatic.MoveWindow( m_WindowPosition.x + m_WindowSize.cx - delaygaugewidth - 1, m_WindowPosition.y, m_WindowPosition.x + m_WindowSize.cx-1, m_WindowPosition.y + m_WindowSize.cy - 1 );
				m_DelayBorderStatic.MoveWindow( m_WindowPosition.x + m_WindowSize.cx - gaugewidth - 1, m_WindowPosition.y, m_WindowPosition.x + m_WindowSize.cx-1, m_WindowPosition.y + m_WindowSize.cy - 1 );
				
				if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
				{
					m_BorderStatic.SetScale( (m_DelayGaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
					m_DelayBorderStatic.SetScale( (m_GaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
				}
				else
				{
					m_BorderStatic.SetClipRect( m_ImageClipRect.left + m_WindowSize.cx - delaygaugewidth - 1, m_ImageClipRect.top, m_ImageClipRect.right, m_ImageClipRect.bottom );
					m_DelayBorderStatic.SetClipRect( m_ImageClipRect.left + m_WindowSize.cx - gaugewidth - 1, m_ImageClipRect.top, m_ImageClipRect.right, m_ImageClipRect.bottom );
				}
			}
		}
	}
	else
	{
		int gaugewidth = _XFC_FloatToIntRet(m_WindowSize.cx * (m_GaugeFactor / 100.0f));

		if( m_GaugeProgressType == _XGAUGEPROGRESSTYPE_FORWARD )
		{
			m_BorderStatic.MoveWindow( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x + gaugewidth, m_WindowPosition.y + m_WindowSize.cy - 1 );

			if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
			{
				m_BorderStatic.SetScale( (m_GaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
			}
			else
			{
				m_BorderStatic.SetClipRect( m_ImageClipRect.left, m_ImageClipRect.top, m_ImageClipRect.left + gaugewidth, m_ImageClipRect.bottom );
			}
		}
		else
		{
			m_BorderStatic.MoveWindow( m_WindowPosition.x + m_WindowSize.cx - gaugewidth - 1, m_WindowPosition.y, m_WindowPosition.x + m_WindowSize.cx-1, m_WindowPosition.y + m_WindowSize.cy - 1 );

			if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_SCALE )
			{
				m_BorderStatic.SetScale( (m_GaugeFactor / 100.0f) * m_GaugeImageScaleFactor, 1.0f );
			}
			else
			{
				m_BorderStatic.SetClipRect( m_ImageClipRect.left + m_WindowSize.cx - gaugewidth - 1, m_ImageClipRect.top, m_ImageClipRect.right, m_ImageClipRect.bottom );
			}
		}
	}
	
	if( !m_FactorDrawCallbackFunc )
	{
		sprintf( m_WindowText, m_WindowTextFormat, _XFC_FloatToIntRet( m_GaugeFactor ) );	

		SIZE Size;
		Size.cx = g_XSmallFont->GetWidth( m_WindowText );		
		Size.cy = g_XSmallFont->GetHeight();	

		m_TextPosition.x = (FLOAT) ( m_WindowPosition.x + (m_WindowSize.cx>>1) - (Size.cx >> 1) );
		m_TextPosition.y = (FLOAT) ( m_WindowPosition.y + (m_WindowSize.cy>>1) - (Size.cy >> 1) );
	}
}

void _XStickGauge::SetGaugeColorFactor( D3DCOLOR colorfactor, D3DCOLOR delaycolorfactor )
{
	m_BorderStatic.m_FColor = colorfactor;
	m_DelayBorderStatic.m_FColor = delaycolorfactor;
}

void _XStickGauge::SetGaugeProgressType( _XGaugeProgressType progresstype )
{
	m_GaugeProgressType =  progresstype;
}

void _XStickGauge::SetViewDelayGauge( BOOL show, DWORD speed )
{
	m_DelayUpdate		= show;
	m_DelaySpeed		= speed;
	m_PrevDelayTimer	= 0;
}

void _XStickGauge::SetGaugeImageDrawType( _XGaugeImageDrawType drawtype )
{
	m_ImageDrawType = drawtype;

	if( m_ImageDrawType == _XGAUGEIMAGEDRAWTYPE_RESIZE )
	{
		m_BorderStatic.SetScale( 1.0f, 1.0f );
	}
}
