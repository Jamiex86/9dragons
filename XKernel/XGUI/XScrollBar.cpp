// XDIScrollBar.cpp: implementation of the _X?ScrollBar class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "XKernel.h"
#include "XScrollBar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _XSCROLLBAR_TRACKBARGABAREA		24
#define _XSCROLLBAR_TRACKBARBORDERSIZE  37.0f

_XVScrollBar::_XVScrollBar()
{	
	m_Enable		 = FALSE;
	m_Focus			 = FALSE;	

	m_Totallinecount = 0;
	m_Viewlinecount  = 0;
	m_Scrollpos		 = 0;
	m_Height         = 0;	
	m_Width			 = 0;
	m_Barsize		 = 0;
	m_Btnsize        = 0;
	m_Barposition    = 0;
	m_ScrollRate     = 0;
	m_ScrollLevel	 = 1;
	m_PageLinecount  = 4;
	m_Border		 = TRUE;
	m_ViewNullBorder = TRUE;
	memset( &m_WindowPosition, 0, sizeof( POINT ) );
}

_XVScrollBar::~_XVScrollBar()
{
	
}

void _XVScrollBar::MoveWindow( int X, int Y )
{
	POINT	movepoint = { m_WindowPosition.x - X, m_WindowPosition.y - Y };

	m_WindowPosition.x = X;
	m_WindowPosition.y = Y;
			
	if( m_topnormalbutton.m_TextureIndex != -1 )
	{
		m_topnormalbutton.MoveWindow( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_Btnsize, m_WindowPosition.y+m_Btnsize );
	}

	if( m_topclickbutton.m_TextureIndex != -1 )
	{
		m_topclickbutton.MoveWindow( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_Btnsize, m_WindowPosition.y+m_Btnsize );
	}

	if( m_bottomnormalbutton.m_TextureIndex != -1 )
	{
		m_bottomnormalbutton.MoveWindow( this->m_WindowPosition.x, this->m_WindowPosition.y+m_Height-m_Btnsize,
										 this->m_WindowPosition.x+m_Btnsize, this->m_WindowPosition.y+m_Height-m_Btnsize+m_Btnsize );
	}

	if( m_bottomclickbutton.m_TextureIndex != -1 )
	{
		m_bottomclickbutton.MoveWindow( this->m_WindowPosition.x, this->m_WindowPosition.y+m_Height-m_Btnsize,
										this->m_WindowPosition.x+m_Btnsize, this->m_WindowPosition.y+m_Height-m_Btnsize+m_Btnsize );
	}

	if( m_TrackbarImage.m_TextureIndex != -1 )
	{		
		m_TrackbarImage.MoveWindow( this->m_WindowPosition.x, this->m_WindowPosition.y + m_Btnsize + m_Scrollpos + 1 );
	}
}

BOOL _XVScrollBar::Create( _XSCRBAR_STRUCTURE scrbarstructure )
{			
	m_Enable		 = scrbarstructure.enable;
	m_Focus          = TRUE;

	m_Height		 = scrbarstructure.height;	
	m_Width			 = scrbarstructure.width; 

	m_Totallinecount = scrbarstructure.totallinecount;
	m_Viewlinecount	 = scrbarstructure.viewlinecount;
	m_Scrollpos		 = 0;
	m_ScrollLevel	 = 1;
	m_Barposition	 = 0;
	m_Barsize		 = 0;
	m_DragStartPos   = 0;	
	m_WindowPosition = scrbarstructure.position;	

	m_Trackbar_Dragging = FALSE;

	m_Btnsize = scrbarstructure.btnysize;
	m_Barsize = scrbarstructure.trackbarysize;
		
	m_topnormalbutton.Create( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_Btnsize, m_WindowPosition.y+m_Btnsize,
							  scrbarstructure.texturearchive, scrbarstructure.imageindex_uparrow );	

	m_topclickbutton.Create( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_Btnsize, m_WindowPosition.y+m_Btnsize,
							 scrbarstructure.texturearchive, scrbarstructure.imageindex_uparrowclick );

	m_bottomnormalbutton.Create( this->m_WindowPosition.x, this->m_WindowPosition.y+m_Height-m_Btnsize,
								 this->m_WindowPosition.x+m_Btnsize, this->m_WindowPosition.y+m_Height-m_Btnsize+m_Btnsize,
								 scrbarstructure.texturearchive, scrbarstructure.imageindex_downarrow );	

	m_bottomclickbutton.Create( this->m_WindowPosition.x, this->m_WindowPosition.y+m_Height-m_Btnsize,
								this->m_WindowPosition.x+m_Btnsize, this->m_WindowPosition.y+m_Height-m_Btnsize+m_Btnsize,
								scrbarstructure.texturearchive, scrbarstructure.imageindex_downarrowclick );

	m_TrackbarImage.Create( this->m_WindowPosition.x, this->m_WindowPosition.y+m_Btnsize + 1, 
		                    this->m_WindowPosition.x+m_Btnsize, this->m_WindowPosition.y+m_Btnsize+m_Barsize + 1,
							scrbarstructure.texturearchive, scrbarstructure.imageindex_trackbar );
			
	UpdateData();

	return TRUE;
}

void _XVScrollBar::SetClipRect( _XSCRBAR_IMAGECLIPRECTSTRUCTURE cliprectstructure )
{
	m_topnormalbutton.SetClipRect( cliprectstructure.topnormalrect );
    m_topclickbutton.SetClipRect( cliprectstructure.topclickrect );
    m_bottomnormalbutton.SetClipRect( cliprectstructure.bottomnormalrect );
    m_bottomclickbutton.SetClipRect( cliprectstructure.bottomclickrect );
    m_TrackbarImage.SetClipRect( cliprectstructure.trackbarrect );    
}

void _XVScrollBar::DestroyWindow( void )
{	
	m_Enable		   = FALSE;
	m_Focus            = FALSE;

	m_Height		   = 0;
	m_Width			   = 0;
	m_Totallinecount   = 0;
	m_Viewlinecount	   = 0;
	m_Scrollpos		   = 0;
	m_ScrollLevel	   = 0;
	m_Barsize		   = 0;
	m_Btnsize		   = 0;
	m_Barposition	   = 0;
	m_WindowPosition.x = 0;	
	m_WindowPosition.y = 0;		
	m_ScrollRate       = 0.f;	
}

void _XVScrollBar::Draw( _XGUIObject*& pfocusobject )
{
	if( !this->m_Enable     ) return;
	if( !this->m_ShowWindow ) return;

	BOOL upbtnfocus = FALSE;
	BOOL downbtnfocus = FALSE;

	MouseState* mousestate = gpInput->GetMouseState();
	ScrnPos* mousepos = gpInput->GetMouseVirScreenPos();
	
	if( ( pfocusobject == (_XGUIObject*)this ) && !mousestate->bButton[0] )
	{
		pfocusobject = NULL;
	}
	
	if( m_Trackbar_Dragging )
	{
		if( mousestate->bButton[0] && ( pfocusobject == (_XGUIObject*)this ) )
		{
			if( gpInput->CheckMousePos( this->m_WindowPosition.x-_XSCROLLBAR_TRACKBARGABAREA, 
								 this->m_WindowPosition.y+m_Btnsize, 
						         this->m_WindowPosition.x+m_Width+_XSCROLLBAR_TRACKBARGABAREA,
						         this->m_WindowPosition.y+m_Height-m_Btnsize ) )
			{
				m_Scrollpos = mousepos->z - m_DragStartPos;
				if( m_Scrollpos < 0 ) m_Scrollpos = 0;
				if( m_Scrollpos > m_Height-(m_Btnsize<<1)-m_Barsize-2 )
					m_Scrollpos = m_Height-(m_Btnsize<<1)-m_Barsize-2;

				if( m_ScrollRate != 0.f )
				{
					m_LineScrollpos = (int)( (float)m_Scrollpos / (float)m_ScrollRate );
					
					if( m_Scrollpos == m_Height-(m_Btnsize<<1)-m_Barsize-2 )
					{
						m_LineScrollpos  =  (m_Totallinecount - m_Viewlinecount);
					}					
					else if( m_LineScrollpos >  (m_Totallinecount - m_Viewlinecount) )
					{
						m_LineScrollpos  =  (m_Totallinecount - m_Viewlinecount);
					}
				}
				else 
				{
					m_LineScrollpos = 0;
					m_Scrollpos = 0;
					m_Trackbar_Dragging = FALSE;
					pfocusobject = NULL;
				}

				RebuildTrackBar();
			}
		}
		else 
		{
			m_Trackbar_Dragging = FALSE;
			pfocusobject = NULL;
			m_DragStartPos = 0;			
		}
	}
	else 
	{
		upbtnfocus   = ( gpInput->CheckMousePos( this->m_WindowPosition.x, this->m_WindowPosition.y, 
											  this->m_WindowPosition.x+m_Width,this->m_WindowPosition.y+m_Btnsize  ) );
		downbtnfocus = ( gpInput->CheckMousePos( this->m_WindowPosition.x, this->m_WindowPosition.y+m_Height-m_Btnsize, 
											  this->m_WindowPosition.x+m_Width,this->m_WindowPosition.y+m_Height) );

		if( mousestate->bButton[0] && m_ScrollRate != 0.f )
		{
			if( upbtnfocus )
			{
				if( pfocusobject == NULL )
				{
					pfocusobject = (_XGUIObject*)this;
				}
				if( pfocusobject == (_XGUIObject*)this ) LineUp(m_ScrollLevel);
			}
			else if( downbtnfocus )
			{
				if( pfocusobject == NULL )
				{
					pfocusobject = (_XGUIObject*)this;
				}
				if( pfocusobject == (_XGUIObject*)this ) LineDown(m_ScrollLevel);
			}			
		}
		
		if( gpInput->CheckMousePos( this->m_WindowPosition.x, this->m_WindowPosition.y+m_Btnsize, 
						     this->m_WindowPosition.x+m_Width,
						     this->m_WindowPosition.y+m_Height-m_Btnsize ) )
		{

			if( m_ScrollRate != 0.f )
			{
				// select track bar
				if( ( mousepos->z >= this->m_WindowPosition.y+m_Btnsize+m_Scrollpos ) && 
					( mousepos->z <= this->m_WindowPosition.y+m_Btnsize+m_Barsize+m_Scrollpos ) )
				{
					if( mousestate->bButton[0] && pfocusobject == NULL )
					{
						m_Trackbar_Dragging = TRUE;

						pfocusobject = (_XGUIObject*)this;

						m_DragStartPos = mousepos->z - m_Scrollpos;

						m_Scrollpos = mousepos->z - m_DragStartPos;
						if( m_Scrollpos < 0 ) m_Scrollpos = 0;
						if( m_Scrollpos >  m_Height-(m_Btnsize<<1)-m_Barsize-2 )
							m_Scrollpos =  m_Height-(m_Btnsize<<1)-m_Barsize-2;
						
						m_LineScrollpos = (int)( (float)m_Scrollpos / (float)m_ScrollRate );
						RebuildTrackBar();
					}
				}		
				else if( mousestate->bButton[0] )
				{
					if( mousepos->z < this->m_WindowPosition.y+m_Btnsize+m_Scrollpos )
					{
						if( pfocusobject == NULL )
						{
							pfocusobject = (_XGUIObject*)this;
						}
						if( pfocusobject == (_XGUIObject*)this ) LineUp(m_PageLinecount);
					}
					else
					{
						if( pfocusobject == NULL )
						{
							pfocusobject = (_XGUIObject*)this;
						}
						if( pfocusobject == (_XGUIObject*)this ) LineDown(m_PageLinecount);
					}
				}
			}
		}
	}

	if( m_ScrollRate > 0.f )
	{					
		if( m_Border )
		{
			_XDrawSolidBar( m_WindowPosition.x, m_WindowPosition.y + m_Btnsize, 
							m_WindowPosition.x + m_Width, this->m_WindowPosition.y+m_Height-m_Btnsize,
							D3DCOLOR_ARGB( 128, 0,0,0 ) );

			// Scroll bar rect
			/*_XDrawRectAngle( m_WindowPosition.x, m_WindowPosition.y, 
				             m_WindowPosition.x+m_Btnsize, 
				             m_WindowPosition.y+m_Height, 0.0f, D3DCOLOR_ARGB(255,0,0,0) );*/

			/*
			_XDrawLine2D( m_WindowPosition.x + (m_Width>>1) - 1, m_WindowPosition.y + m_Btnsize + 1, 
						  m_WindowPosition.x + (m_Width>>1) - 1, this->m_WindowPosition.y+(m_Height>>1), 
						  D3DCOLOR_ARGB( 96, 255,255,255 ),D3DCOLOR_ARGB( 4, 32,32,32 ) );
			_XDrawLine2D( m_WindowPosition.x + (m_Width>>1), m_WindowPosition.y + m_Btnsize + 1, 
						  m_WindowPosition.x + (m_Width>>1), this->m_WindowPosition.y+((m_Height)>>1), 
						  D3DCOLOR_ARGB( 96, 255,255,255 ),D3DCOLOR_ARGB( 4, 32,32,32 ) );

			_XDrawLine2D( m_WindowPosition.x + (m_Width>>1) - 1, m_WindowPosition.y + (m_Height>>1), 
						  m_WindowPosition.x + (m_Width>>1) - 1, this->m_WindowPosition.y+m_Height-m_Btnsize-1, 
						  D3DCOLOR_ARGB( 4, 32,32,32 ), D3DCOLOR_ARGB( 96, 255,255,255 ) );
			_XDrawLine2D( m_WindowPosition.x + (m_Width>>1), m_WindowPosition.y + (m_Height>>1), 
						  m_WindowPosition.x + (m_Width>>1), this->m_WindowPosition.y+m_Height-m_Btnsize-1, 
						  D3DCOLOR_ARGB( 4, 32,32,32 ), D3DCOLOR_ARGB( 96, 255,255,255 ) );
			*/
		}
	}
	else
	{
		if( m_ViewNullBorder )
		{
			_XDrawSolidBar( m_WindowPosition.x, m_WindowPosition.y, 
							m_WindowPosition.x + m_Width, this->m_WindowPosition.y+m_Height,
							D3DCOLOR_ARGB( 128, 0,0,0 ) );
			m_topnormalbutton.DrawWithRegion();
			m_bottomnormalbutton.DrawWithRegion();
			m_TrackbarImage.DrawWithRegion();
		}
	}
	
	if( !(mousestate->bButton[0] && upbtnfocus ) )
	{
		if( m_topnormalbutton.m_TextureIndex != -1 ) 
		{
			if( m_ScrollRate != 0.f )
			{					
				m_topnormalbutton.DrawWithRegion();
			}
		}
	}
	else
	{
		if( m_topclickbutton.m_TextureIndex != -1  )
		{
			if( m_ScrollRate != 0.f )
			{
				if( pfocusobject == (_XGUIObject*)this )					
					m_topclickbutton.DrawWithRegion();
				else					
					m_topnormalbutton.DrawWithRegion();
			}
		}
	}

	if( !(mousestate->bButton[0] && downbtnfocus ) )
	{
		if( m_bottomnormalbutton.m_TextureIndex != -1 ) 
		{
			if( m_ScrollRate != 0.f )
			{					
				m_bottomnormalbutton.DrawWithRegion();
			}
		}
	}
	else
	{
		if( m_bottomclickbutton.m_TextureIndex != -1 )
		{
			if( m_ScrollRate != 0.f )
			{
				if( pfocusobject == (_XGUIObject*)this )					
					m_bottomclickbutton.DrawWithRegion();
				else					
					m_bottomnormalbutton.DrawWithRegion();
			}
		}
	}	

	if( this->m_TrackbarImage.m_TextureIndex != -1 ) 
	{
		if( m_ScrollRate != 0.f )			
			m_TrackbarImage.DrawWithRegion();
	}
	
/*	if( !mousestate->bButton[0] && XDD->m_MouseWheelDelta )
	{
		if( CheckMousePosition() )
		{
			if( XDD->m_MouseWheelDelta > 0 )
			{
				int i = 0 ; for(  i = 0; i < XDD->m_MouseWheelDelta; i++ ) LineUp();
			}
			else
			{
				int i = 0 ; for(  i = XDD->m_MouseWheelDelta; i < 0 ; i++ ) LineDown();
			}
			XDD->m_MouseWheelDelta = 0;	
		}
	}	*/
}

BOOL _XVScrollBar::Process( _XGUIObject*& pfocusobject )
{
	MouseState* mousestate = gpInput->GetMouseState();
	
	if( mousestate )
	{
		if( !pfocusobject )
		{
			if( mousestate->bButton[0] && CheckMousePosition() )
			{
				pfocusobject = (_XGUIObject*)this;
			}	
		}
		
		if( !mousestate->bButton[0] )
		{
			if( CheckMousePosition() )
			{	
				if( pfocusobject == (_XGUIObject*)this )
				{	
					/*
					if( m_CommandID )
					{				
						if( m_CommandID > 0xFFFF )
							PostMessage(gHWnd, WM_COMMAND, (WPARAM)m_CommandID, (LPARAM)m_lParam);
						else
							PostMessage(gHWnd, WM_COMMAND, MAKEWPARAM(m_CommandID, 0), (LPARAM)m_lParam);
						mousestate->bButton[0] = 0;							
					}
					*/
					
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

BOOL _XVScrollBar::ProcessWheelMessage( short zDelta )
{
	if( CheckMousePosition() )
	{
		int MoveDelta  = (FLOAT)zDelta / 40;			
		
		if( zDelta > 0 )
		{
			LineUp( abs(MoveDelta) );
		}
		else
		{
			LineDown( abs(MoveDelta) );
		}	
		return TRUE;
	}

	return FALSE;	
}

BOOL _XVScrollBar::CheckMousePosition( void )
{
	if( !this->m_Enable ) return FALSE;

	ScrnPos* mousepos = gpInput->GetMouseVirScreenPos();		
	return ( (mousepos->x>m_WindowPosition.x) && (mousepos->x<m_WindowPosition.x+m_Width) &&  
	         (mousepos->z>m_WindowPosition.y) && (mousepos->z<m_WindowPosition.y+m_Height) ); 		
}

void _XVScrollBar::ResetPosition( void )
{
	m_LineScrollpos = 0;
	m_Scrollpos		= 0;
}

void _XVScrollBar::UpdateData( void )
{		
	if( (m_Totallinecount - m_Viewlinecount ) > 0 )
	{
		 this->m_ScrollRate = (float)( m_Height-(m_Btnsize<<1)-m_Barsize-2 ) / 
			                  (float)( m_Totallinecount - m_Viewlinecount );
	}
	else this->m_ScrollRate = 0.0f;

	if( m_ScrollRate == 0.0f )
	{
		 m_LineScrollpos = 0;
	}
	else m_LineScrollpos = (int)( (float)m_Scrollpos / m_ScrollRate );

/*	if( m_LineScrollpos < 0 ) m_LineScrollpos = 0;
	if( m_LineScrollpos  >=  (m_Totallinecount - m_Viewlinecount) ) 
		m_LineScrollpos  =   (m_Totallinecount - m_Viewlinecount)-1;

	if( m_LineScrollpos == (m_Totallinecount - m_Viewlinecount)-1 )
		 m_Scrollpos =  m_Height-(m_Btnsize<<1)-m_Barsize;
	else m_Scrollpos = (int)((float)m_LineScrollpos * m_ScrollRate);*/
	
	RebuildTrackBar();
}

void _XVScrollBar::LineUp( int line )
{
	if( m_ScrollRate == 0.0f ) return;

	m_LineScrollpos-=line;
	
	if( m_LineScrollpos < 0 ) m_LineScrollpos = 0;
	if( m_LineScrollpos  >=  (m_Totallinecount - m_Viewlinecount) ) 
		m_LineScrollpos  =   (m_Totallinecount - m_Viewlinecount);

	if( m_LineScrollpos == (m_Totallinecount - m_Viewlinecount) )
		 m_Scrollpos =  m_Height-(m_Btnsize<<1)-m_Barsize - 2;
	else m_Scrollpos = (int)((float)m_LineScrollpos * m_ScrollRate);

	RebuildTrackBar();
}

void _XVScrollBar::LineDown( int line )
{
	if( m_ScrollRate == 0.0f ) return;

	m_LineScrollpos+=line;
	if( m_LineScrollpos < 0 ) m_LineScrollpos = 0;
	if( m_LineScrollpos  >= (m_Totallinecount - m_Viewlinecount) ) 	
		m_LineScrollpos  =  (m_Totallinecount - m_Viewlinecount);	

	if( m_LineScrollpos == (m_Totallinecount - m_Viewlinecount) )
		 m_Scrollpos =  m_Height-(m_Btnsize<<1)-m_Barsize - 2;
	else m_Scrollpos = (int)((float)m_LineScrollpos * m_ScrollRate);

	RebuildTrackBar();
}


void _XVScrollBar::RebuildTrackBar( void )
{
	if( m_Scrollpos < 0 ) m_Scrollpos = 0;
	if( m_Scrollpos > m_Height-(m_Btnsize<<1)-m_Barsize - 2 )
		m_Scrollpos = m_Height-(m_Btnsize<<1)-m_Barsize - 2;

	m_TrackbarImage.MoveWindow( this->m_WindowPosition.x, this->m_WindowPosition.y + m_Btnsize + m_Scrollpos + 1 );	
}