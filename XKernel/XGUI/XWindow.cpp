#include "stdafx.h"
#include "XKernel.h"

#define _XDEF_WINDOWEFFECT_FRAME		8

extern _XTextureManager g_MainInterfaceTextureArchive;

BOOL _XWindow::m_GlobalPlaySoundEffect = FALSE;

_XWindow::_XWindow()
{		
	m_WindowID				= 0;
	m_SolidBorderMode		= FALSE;
	m_WindowAnimationType	= _XW_ANIMTYPE_NONE;
	m_WindowAnimationLevel	= 0;
	m_UseWindowEffect		= FALSE;
	m_WindowMoveMode		= _XWMOVE_FREE;
	m_WindowDockingType		= _XW_DOCKINGTYPE_NONE;
	m_DockedParentWindow	= NULL;
	m_DockedChildWindow		= NULL;
	m_DockableWindow		= FALSE;
	m_EdgeDockableWindow	= TRUE;
	m_Dragging				= FALSE;
	m_DragStartPos.x		= m_DragStartPos.y		= 0;	
	m_DockingGroup			= 0;
	m_DrawBorder			= TRUE;
	m_SelfDestroy			= FALSE;

	memset( m_WindowTitle, 0, _XDEF_WINDOWTITLESTRINGSIZE );
	strcpy( m_WindowTitle, "Notitle" );

	m_pMyManager			= NULL;

	m_bEscapeWindow			= FALSE;//2004.06.24->oneway48 insert
	m_bEnableTopWindow		= TRUE;
	
	m_PlaySoundEffect		= FALSE;
}


_XWindow::~_XWindow()
{
	DestroyWindow();
}

BOOL _XWindow::Create( _XWINDOW_STRUCTURE& windowstruct )
{
	//_XLog( "Window creating : wnd id : %d", windowstruct.windowid );

	DestroyWindow();

	m_Enable			= windowstruct.enable;
	m_CommandID			= windowstruct.commandid;	
	m_WindowPosition	= windowstruct.position;
	m_WindowSize		= windowstruct.windowsize;
	m_WindowID			= windowstruct.windowid;

	/*if( strlen( windowstruct.WindowTitle ) == 0 )
		strcpy( m_WindowTitle, "Notitle" );
	else
		strcpy( m_WindowTitle, windowstruct.WindowTitle );*/
		
	if( windowstruct.windowtextureindex != -1 )
	{
		m_borderstatic.Create( m_WindowPosition.x, m_WindowPosition.y, 
							   m_WindowPosition.x+m_WindowSize.cx-1, 
							   m_WindowPosition.y+m_WindowSize.cy-1, windowstruct.texturearchive, windowstruct.windowtextureindex );
		
		m_solidborderstatic.Create( D3DXVECTOR3(m_WindowPosition.x, m_WindowPosition.y+1, 0.5 ),
								    D3DXVECTOR3(m_WindowPosition.x+m_WindowSize.cx, m_WindowPosition.y+1, 0.5 ),
									D3DXVECTOR3(m_WindowPosition.x, m_WindowPosition.y+m_WindowSize.cy, 0.5 ),
									D3DXVECTOR3(m_WindowPosition.x+m_WindowSize.cx, m_WindowPosition.y+m_WindowSize.cy, 0.5 ),
									_XDEF_DEFAULTDEPTH_FORINTERFACE,
									windowstruct.facecolor );
		
		m_SolidBorderMode = FALSE;
	}
	else
	{
		m_borderstatic.Create( m_WindowPosition.x, m_WindowPosition.y, 
			m_WindowPosition.x+m_WindowSize.cx-1, 
			m_WindowPosition.y+m_WindowSize.cy-1, windowstruct.texturearchive, windowstruct.windowtextureindex );

		m_solidborderstatic.Create( D3DXVECTOR3(m_WindowPosition.x, m_WindowPosition.y+1, 0.5 ),
								    D3DXVECTOR3(m_WindowPosition.x+m_WindowSize.cx, m_WindowPosition.y+1, 0.5 ),
							        D3DXVECTOR3(m_WindowPosition.x, m_WindowPosition.y+m_WindowSize.cy, 0.5 ),
							        D3DXVECTOR3(m_WindowPosition.x+m_WindowSize.cx, m_WindowPosition.y+m_WindowSize.cy, 0.5 ),
							        _XDEF_DEFAULTDEPTH_FORINTERFACE,
							        windowstruct.facecolor );

		m_SolidBorderMode = TRUE;
	}	
	
	_XLog( "Created window : cmd id : %d / wnd id :%d", m_CommandID, m_WindowID );
	
	return TRUE;
}

// hotblood zerolevel
void _XWindow::SetBorderColorFactor( D3DCOLOR factor )
{
	m_borderstatic.m_FColor = factor;
	m_solidborderstatic.SetFaceColor( factor );
}

void _XWindow::DestroyWindow( void )
{	
	m_Enable			= FALSE;
	m_ShowWindow		= TRUE;
	m_SolidBorderMode	= FALSE;

	m_WindowPosition.x	= 0;
	m_WindowPosition.y	= 0;

	m_WindowSize.cx		= 0;
	m_WindowSize.cy		= 0;

	m_CommandID			= NULL;

	m_pMyManager		= NULL;

	memset( m_WindowTitle, 0, _XDEF_WINDOWTITLESTRINGSIZE );
	
	m_solidborderstatic.ReleaseObject();

	m_ObjectList.disposeList();
}

void _XWindow::EnableWindow( BOOL enable )
{ 
	m_Enable = enable;
	
	if( !enable )
	{
		if( g_CurrentFocusedObject == (_XGUIObject*)this )
			g_CurrentFocusedObject = NULL;
	}

	if( !m_ObjectList.listEmpty() )
	{
		_XGUIObject* childwindow;
		
		do 
		{
			childwindow = (_XGUIObject*)m_ObjectList.currentItem();

			if( childwindow )
			{
				childwindow->EnableWindow( m_Enable );
			}
			else
			{
				break;
			}
			
			m_ObjectList.nextItem();
		} while( !m_ObjectList.atHeadOfList() );
	}
	
	m_borderstatic.EnableWindow( m_Enable );	
}

void _XWindow::ShowWindow( BOOL show )
{ 
	if( !show )
	{
		if( g_CurrentFocusedObject == (_XGUIObject*)this )
			g_CurrentFocusedObject = NULL;
	}

	if( m_GlobalPlaySoundEffect && m_PlaySoundEffect )
	{
		if( show != m_ShowWindow )
			_XPlayInterfaceSound( (show ? g_WindowOpenSoundIndex : g_WindowCloseSoundIndex), true );
	}

	m_DockedParentWindow = NULL;
	m_DockedChildWindow  = NULL;

	if( m_ShowWindow != show )
	{				
		if( m_UseWindowEffect )
		{
			if( m_WindowAnimationType == _XW_ANIMTYPE_NONE )
			{
				if( show )
				{
					m_WindowAnimationType  = _XW_ANIMTYPE_OPEN;
					m_WindowAnimationLevel = _XDEF_WINDOWEFFECT_FRAME;
				}
				else
				{
					m_WindowAnimationType  = _XW_ANIMTYPE_CLOSE;
					m_WindowAnimationLevel = 0;
				}
			}
		}
		else
		{
			m_ShowWindow = show;
		}
	}

	if( !m_ObjectList.listEmpty() )
	{	
		m_ObjectList.resetList();
		_XGUIObject* currentObject = NULL;
		
		// Check sub window focused...
		do
		{
			currentObject = (_XGUIObject*)m_ObjectList.currentItem();
			
			if(currentObject)
			{
				if( !show )
				{
					if( g_CurrentFocusedObject == (_XGUIObject*)currentObject )
						g_CurrentFocusedObject = NULL;
				}
				else
				{
					currentObject->RefreshData();
				}
			}
			
			m_ObjectList.nextItem();
		}while( !m_ObjectList.atHeadOfList() );
	}
}	
			
BOOL _XWindow::CheckMousePosition(void)
{	
	if( !this->m_Enable ) return FALSE;

	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();

	if( scrnpos )
	{	
		if( m_SolidBorderMode )
		{
			if( ((scrnpos->x > m_WindowPosition.x) && (scrnpos->x < m_WindowPosition.x+m_WindowSize.cx) &&
				 (scrnpos->z > m_WindowPosition.y) && (scrnpos->z < m_WindowPosition.y+m_WindowSize.cy) ) ) return TRUE;
		}
		else 
		{
			if(m_borderstatic.m_TextureIndex != -1 )
			{
				return m_borderstatic.TestAlphaChannel( scrnpos->x, scrnpos->z );
			}				
		}		
	}

	return FALSE;
}

_XGUIObject* _XWindow::InsertChildObject( _XGUIObject* xguiobject )
{
	assert( xguiobject );
	if( xguiobject )
	{
		POINT childwindowpos = xguiobject->GetWindowPos();
		xguiobject->MoveWindow( this->m_WindowPosition.x + childwindowpos.x, this->m_WindowPosition.y + childwindowpos.y );

		xguiobject->m_pParentWindow = this;

		//xguiobject->EnableWindow( this->m_Enable );
		m_ObjectList.resetList();
		return (_XGUIObject*)m_ObjectList.insertItem( xguiobject );		
	}
	return NULL;
}

void _XWindow::CheckOutside( void )
{	
	BOOL  reposition = FALSE;
	
	POINT winpos = m_WindowPosition;
	
	if( winpos.x < 0 ){ winpos.x = 0; reposition = TRUE; }
	if( winpos.y < 0 ){ winpos.y = 0; reposition = TRUE; }

	if( winpos.x+m_WindowSize.cx >= gnWidth)
	{ 
		winpos.x = gnWidth - m_WindowSize.cx; reposition = TRUE;  
	}
	if( winpos.y+m_WindowSize.cy >= gnHeight)
	{ 
		winpos.y = gnHeight- m_WindowSize.cy; reposition = TRUE;  
	}
	if( reposition ) 
	{
		if( m_DockedParentWindow )
		{
			m_DockedParentWindow->m_DockedChildWindow = NULL;
			m_DockedParentWindow = NULL;
		}
		
		m_WindowDockingType = _XW_DOCKINGTYPE_NONE;

		MoveWindow( winpos.x, winpos.y );
	}
}

void _XWindow::SetChildWindow ( _XWindow* pChildWindow  )
{	
	if( !m_pMyManager ) return;
	
	_XWindow* pWindow = NULL;
	int windowcount = m_pMyManager->GetWindowCount();		
	int i = 0 ; for(  i = 0; i < windowcount ; i++ )
	{
		pWindow = m_pMyManager->GetWindow(i);
		
		if( (pWindow != this) && (pWindow->m_DockedChildWindow == pChildWindow) )
		{
			pWindow->m_DockedChildWindow = NULL;
		}
	}

	if( pChildWindow )
	{
		if( pChildWindow->m_DockedChildWindow == this ) pChildWindow->m_DockedChildWindow = NULL;
		m_DockedChildWindow = pChildWindow;
	}
	else m_DockedChildWindow = NULL;
}


void _XWindow::WindowPosChanging( int& X, int& Y )
{
	//if( X == 0 && Y == 0 ) return;	
	if( m_EdgeDockableWindow )
	{
		if( X < _XDEF_XWINEDGESNAPDISTANCE ){ X = 0; }
		if( Y < _XDEF_XWINEDGESNAPDISTANCE ){ Y = 0; }	
		if( X+m_WindowSize.cx >= gnWidth -_XDEF_XWINEDGESNAPDISTANCE)
		{ 
			X = gnWidth - m_WindowSize.cx;
		}
		if( Y+m_WindowSize.cy >= gnHeight-_XDEF_XWINEDGESNAPDISTANCE)
		{ 
			Y = gnHeight- m_WindowSize.cy;
		}
	}

	if( !m_DockableWindow ) return;
		
	if( !m_pMyManager ) return;

	_XWindow* pWindow = NULL;
	int windowcount = m_pMyManager->GetWindowCount();	
	_XWINDOW_DOCKINGTYPE dockingtype = _XW_DOCKINGTYPE_NONE;
	_XWINDOW_DOCKINGTYPE lastdockingtype = _XW_DOCKINGTYPE_NONE;

	int i = 0 ; for(  i = 0; i < windowcount ; i++ )
	{
		pWindow = m_pMyManager->GetWindow(i);

		if( pWindow && (pWindow != (_XWindow*)this) && pWindow->GetShowStatus() && 
			pWindow->m_DockableWindow && m_DockingGroup == pWindow->GetDockingGroup() )
		{			
			dockingtype = _XW_DOCKINGTYPE_NONE;

			POINT parentpos  = pWindow->GetWindowPos();
			SIZE  parentsize = pWindow->GetWindowSize();
			RECT  parentrect = { parentpos.x, parentpos.y, parentpos.x+parentsize.cx, parentpos.y+parentsize.cy };
			
			RECT curwindowrect = { X, Y, X + m_WindowSize.cx, Y + m_WindowSize.cy };
			
			if( (curwindowrect.left  >= parentrect.left && curwindowrect.left  <= parentrect.right) ||
				(curwindowrect.right >= parentrect.left && curwindowrect.right <= parentrect.right) ||
				(curwindowrect.left  <= parentrect.left && curwindowrect.right >= parentrect.right))
			{
				if(curwindowrect.top >= parentrect.bottom - _XDEF_XWINSNAPDISTANCE && curwindowrect.top <= parentrect.bottom + _XDEF_XWINSNAPDISTANCE)
				{
					lastdockingtype = dockingtype = _XW_DOCKINGTYPE_BOTTOM;
					
					Y = parentrect.bottom;
					
					if(curwindowrect.left >= parentrect.left - _XDEF_XWINSNAPDISTANCE && curwindowrect.left <= parentrect.left + _XDEF_XWINSNAPDISTANCE)
					{
						X = parentrect.left;
					}
					if(curwindowrect.right >= parentrect.right - _XDEF_XWINSNAPDISTANCE && curwindowrect.right <= parentrect.right + _XDEF_XWINSNAPDISTANCE)
					{
						X = parentrect.right - m_WindowSize.cx;
					}					
				}				
			}
			
			if(( curwindowrect.top    >= parentrect.top && curwindowrect.top    <= parentrect.bottom) ||
				(curwindowrect.bottom >= parentrect.top && curwindowrect.bottom <= parentrect.bottom) ||
				(curwindowrect.top    <= parentrect.top && curwindowrect.bottom >= parentrect.bottom))
			{
				if(dockingtype != _XW_DOCKINGTYPE_BOTTOM )					
				{
					if(curwindowrect.left >= (parentrect.right - _XDEF_XWINSNAPDISTANCE-5) && curwindowrect.left <= (parentrect.right + _XDEF_XWINSNAPDISTANCE-5) )
					{
						lastdockingtype = dockingtype = _XW_DOCKINGTYPE_RIGHT;
						X = parentrect.right;
						
						if(curwindowrect.top >= parentrect.top - _XDEF_XWINSNAPDISTANCE && curwindowrect.top <= parentrect.top + _XDEF_XWINSNAPDISTANCE)
						{
							Y = parentrect.top;
						}
						if(curwindowrect.bottom >= parentrect.bottom - _XDEF_XWINSNAPDISTANCE && curwindowrect.bottom <= parentrect.bottom + _XDEF_XWINSNAPDISTANCE)
						{
							Y = parentrect.bottom - m_WindowSize.cy;
						}					
					}
					else if(curwindowrect.right >= parentrect.left - _XDEF_XWINSNAPDISTANCE && curwindowrect.right <= parentrect.left + _XDEF_XWINSNAPDISTANCE)
					{
						lastdockingtype = dockingtype = _XW_DOCKINGTYPE_LEFT;
						X = parentrect.left - m_WindowSize.cx;
						
						if(curwindowrect.top >= parentrect.top - _XDEF_XWINSNAPDISTANCE && curwindowrect.top <= parentrect.top + _XDEF_XWINSNAPDISTANCE)
						{
							Y = parentrect.top;
						}
						if(curwindowrect.bottom >= parentrect.bottom - _XDEF_XWINSNAPDISTANCE && curwindowrect.bottom <= parentrect.bottom + _XDEF_XWINSNAPDISTANCE)
						{
							Y = parentrect.bottom - m_WindowSize.cy;
						}
					}					
				}
			}	
						
			
			// 도킹시 차일드와 패런트윈도우 링크... 사용 불편함...
			/*if( dockingtype != _XW_DOCKINGTYPE_NONE )
			{									
				if( m_DockedChildWindow != pWindow )
				{						
					pWindow->SetChildWindow( this );
					m_DockedParentWindow = pWindow;

					if( pWindow->m_DockedParentWindow == this ) pWindow->m_DockedParentWindow = NULL;
				}
				else
				{
					//m_DockedParentWindow = NULL;
					pWindow->SetChildWindow( NULL );
				}				
			}
			else
			{					
				if( pWindow->m_DockedChildWindow == this )
				{
					pWindow->m_DockedChildWindow = NULL;					
					m_DockedParentWindow = NULL;
				}				
			}*/
		}
	}	
	
	m_WindowDockingType = lastdockingtype;
}


void _XWindow::RebuildDockedWindow( void )
{
	if( m_EdgeDockableWindow )
	{
		if( m_WindowPosition.x < _XDEF_XWINEDGESNAPDISTANCE ){ m_WindowPosition.x = 0; }
		if( m_WindowPosition.y < _XDEF_XWINEDGESNAPDISTANCE ){ m_WindowPosition.y = 0; }	
		if( m_WindowPosition.x+m_WindowSize.cx >= gnWidth -_XDEF_XWINEDGESNAPDISTANCE)
		{ 
			m_WindowPosition.x = gnWidth - m_WindowSize.cx;
		}
		if( m_WindowPosition.y+m_WindowSize.cy >= gnHeight-_XDEF_XWINEDGESNAPDISTANCE)
		{ 
			m_WindowPosition.y = gnHeight- m_WindowSize.cy;
		}
	}

	if( !this->m_DockableWindow ) return;
	int X = m_WindowPosition.x;
	int Y = m_WindowPosition.y;
	
	if( !m_pMyManager ) return;

	_XWindow* pWindow = NULL;
	int windowcount = m_pMyManager->GetWindowCount();		
	_XWINDOW_DOCKINGTYPE dockingtype = _XW_DOCKINGTYPE_NONE;
	_XWINDOW_DOCKINGTYPE lastdockingtype = _XW_DOCKINGTYPE_NONE;

	int i = 0 ; for(  i = 0; i < windowcount ; i++ )
	{
		pWindow = m_pMyManager->GetWindow(i);

		if( pWindow && (pWindow != (_XWindow*)this) && pWindow->GetShowStatus() && pWindow->m_DockableWindow &&			
			pWindow->m_DockableWindow && m_DockingGroup == pWindow->GetDockingGroup() )
		{			
			dockingtype = _XW_DOCKINGTYPE_NONE;

			POINT parentpos  = pWindow->GetWindowPos();
			SIZE  parentsize = pWindow->GetWindowSize();
			RECT  parentrect = { parentpos.x, parentpos.y, parentpos.x+parentsize.cx, parentpos.y+parentsize.cy };
			
			RECT curwindowrect = { X, Y, X + m_WindowSize.cx, Y + m_WindowSize.cy };
			
			if( (curwindowrect.left  >= parentrect.left && curwindowrect.left  <= parentrect.right) ||
				(curwindowrect.right >= parentrect.left && curwindowrect.right <= parentrect.right) ||
				(curwindowrect.left  <= parentrect.left && curwindowrect.right >= parentrect.right))
			{
				if(curwindowrect.top >= parentrect.bottom - _XDEF_XWINSNAPDISTANCE && curwindowrect.top <= parentrect.bottom + _XDEF_XWINSNAPDISTANCE)
				{
					lastdockingtype = dockingtype = _XW_DOCKINGTYPE_BOTTOM;
					
					Y = parentrect.bottom;
					
					if(curwindowrect.left >= parentrect.left - _XDEF_XWINSNAPDISTANCE && curwindowrect.left <= parentrect.left + _XDEF_XWINSNAPDISTANCE)
					{
						X = parentrect.left;
					}
					if(curwindowrect.right >= parentrect.right - _XDEF_XWINSNAPDISTANCE && curwindowrect.right <= parentrect.right + _XDEF_XWINSNAPDISTANCE)
					{
						X = parentrect.right - m_WindowSize.cx;
					}					
				}				
			}
			
			if(( curwindowrect.top    >= parentrect.top && curwindowrect.top    <= parentrect.bottom) ||
				(curwindowrect.bottom >= parentrect.top && curwindowrect.bottom <= parentrect.bottom) ||
				(curwindowrect.top    <= parentrect.top && curwindowrect.bottom >= parentrect.bottom))
			{
				if(dockingtype != _XW_DOCKINGTYPE_BOTTOM )					
				{
					if(curwindowrect.left >= parentrect.right - _XDEF_XWINSNAPDISTANCE && curwindowrect.left <= parentrect.right + _XDEF_XWINSNAPDISTANCE)
					{
						lastdockingtype = dockingtype = _XW_DOCKINGTYPE_RIGHT;
						X = parentrect.right;
						
						if(curwindowrect.top >= parentrect.top - _XDEF_XWINSNAPDISTANCE && curwindowrect.top <= parentrect.top + _XDEF_XWINSNAPDISTANCE)
						{
							Y = parentrect.top;
						}
						if(curwindowrect.bottom >= parentrect.bottom - _XDEF_XWINSNAPDISTANCE && curwindowrect.bottom <= parentrect.bottom + _XDEF_XWINSNAPDISTANCE)
						{
							Y = parentrect.bottom - m_WindowSize.cy;
						}					
					}
					else if(curwindowrect.right >= parentrect.left - _XDEF_XWINSNAPDISTANCE && curwindowrect.right <= parentrect.left + _XDEF_XWINSNAPDISTANCE)
					{
						lastdockingtype = dockingtype = _XW_DOCKINGTYPE_LEFT;
						X = parentrect.left - m_WindowSize.cx;
						
						if(curwindowrect.top >= parentrect.top - _XDEF_XWINSNAPDISTANCE && curwindowrect.top <= parentrect.top + _XDEF_XWINSNAPDISTANCE)
						{
							Y = parentrect.top;
						}
						if(curwindowrect.bottom >= parentrect.bottom - _XDEF_XWINSNAPDISTANCE && curwindowrect.bottom <= parentrect.bottom + _XDEF_XWINSNAPDISTANCE)
						{
							Y = parentrect.bottom - m_WindowSize.cy;
						}
					}					
				}
			}	
			
			 
			// 도킹시 차일드와 패런트윈도우 링크... 사용 불편함...
			/*if( dockingtype != _XW_DOCKINGTYPE_NONE )
			{	
				if( m_DockedChildWindow != pWindow )
				{					
					pWindow->SetChildWindow( this );
					m_DockedParentWindow = pWindow;
					
					if( pWindow->m_DockedParentWindow == this ) pWindow->m_DockedParentWindow = NULL;
				}
				else
				{
					//m_DockedParentWindow = NULL;
					pWindow->SetChildWindow( NULL );
				}
			}
			else
			{					
				if( pWindow->m_DockedChildWindow == this )
				{
					pWindow->m_DockedChildWindow = NULL;			
					m_DockedParentWindow = NULL;
				}
			}*/
		}
	}	
	
	m_WindowDockingType = lastdockingtype;

	MoveWindow( X, Y );
}


void _XWindow::MoveWindow( int X, int Y )
{
	POINT	movepoint = { m_WindowPosition.x - X, m_WindowPosition.y - Y };

	if( m_DockedChildWindow )
	{		
		m_DockedChildWindow->MoveWindow( m_DockedChildWindow->GetWindowPos().x - movepoint.x, m_DockedChildWindow->GetWindowPos().y - movepoint.y );
		m_DockedChildWindow->CheckOutside();
	}

	if( m_WindowPosition.x == X && m_WindowPosition.y == Y ) return;

	m_WindowPosition.x = X;
	m_WindowPosition.y = Y;

	/*
	if( m_SolidBorderMode )
	{
		m_solidborderstatic.MoveWindow( X, Y+1 );
	}
	else
	{
		m_borderstatic.MoveWindow( X, Y );
	}
	*/

	m_solidborderstatic.MoveWindow( X, Y+1 );
	m_borderstatic.MoveWindow( X, Y );

	if( m_ObjectList.listEmpty() ) return;
	
	m_ObjectList.resetList();
	_XGUIObject* currentObject = NULL;

	do
	{
		currentObject = (_XGUIObject*)m_ObjectList.currentItem();

		if(currentObject)
		{
			POINT wpos = currentObject->GetWindowPos();

			currentObject->MoveWindow( wpos.x - movepoint.x, wpos.y - movepoint.y );
		}

		m_ObjectList.nextItem();
	}while( !m_ObjectList.atHeadOfList() );
}
				
void _XWindow::Draw( _XGUIObject*& pfocusobject )
{		
	if( m_UseWindowEffect )
	{
		if( m_WindowAnimationType != _XW_ANIMTYPE_NONE )
		{		
			if( m_WindowAnimationType == _XW_ANIMTYPE_OPEN )
			{
				m_WindowAnimationLevel--;
				if( m_WindowAnimationLevel <= 0 )
				{
					m_WindowAnimationLevel = 0;
					m_WindowAnimationType = _XW_ANIMTYPE_NONE;
					m_ShowWindow = TRUE;					
				}
			}
			else if( m_WindowAnimationType == _XW_ANIMTYPE_CLOSE )
			{
				m_WindowAnimationLevel++;
				if( m_WindowAnimationLevel >= _XDEF_WINDOWEFFECT_FRAME )
				{
					m_WindowAnimationLevel = _XDEF_WINDOWEFFECT_FRAME;
					m_WindowAnimationType = _XW_ANIMTYPE_NONE;
					m_ShowWindow = FALSE;
					if( g_CurrentFocusedObject == this )
					{
						g_CurrentFocusedObject = NULL;
					}
				}		
			}

			DrawWindowEffect();
		}
	}
	
	if( !this->m_ShowWindow ) return;
	if( m_WindowAnimationType != _XW_ANIMTYPE_NONE ) return;

	if( m_DrawBorder )
	{
		if( !this->m_Enable )
		{
			if( !m_SolidBorderMode )
				m_borderstatic.Draw();
			else
				m_solidborderstatic.RenderSolid();
		}
		else
		{
			if( !m_SolidBorderMode )
			{		
				m_borderstatic.Draw();
			}
			else
			{
				m_solidborderstatic.RenderSolid();
			}
		}
	}

	if( m_ObjectList.listEmpty() ) return;
	
	m_ObjectList.resetList();
	_XGUIObject* currentObject = NULL;

	do
	{
		currentObject = (_XGUIObject*)m_ObjectList.currentItem();

		if(currentObject) currentObject->Draw( pfocusobject );

		m_ObjectList.nextItem();
	}while( !m_ObjectList.atHeadOfList() );

//#define _XDEF_VIEWWINDOWLINKEDINFO
#ifdef  _XDEF_VIEWWINDOWLINKEDINFO
	if( m_DockedParentWindow )
		g_XBaseFont->Puts( this->m_WindowPosition.x , m_WindowPosition.y, (LPTSTR)m_DockedParentWindow->GetWindowTitle()  );
	else
		g_XBaseFont->Puts( this->m_WindowPosition.x , m_WindowPosition.y, (LPTSTR)"Null"  );

	if( m_DockedChildWindow )
		g_XBaseFont->Puts( this->m_WindowPosition.x , m_WindowPosition.y+12, (LPTSTR)m_DockedChildWindow->GetWindowTitle()  );
	else
		g_XBaseFont->Puts( this->m_WindowPosition.x , m_WindowPosition.y+12, (LPTSTR)"Null"  );

	if( m_WindowDockingType == _XW_DOCKINGTYPE_NONE )
		g_XBaseFont->Print( this->m_WindowPosition.x , m_WindowPosition.y+24, 1.0f, "NONE" );
	if( m_WindowDockingType == _XW_DOCKINGTYPE_LEFT )
		g_XBaseFont->Print( this->m_WindowPosition.x , m_WindowPosition.y+24,  1.0f, "LEFT" );
	if( m_WindowDockingType == _XW_DOCKINGTYPE_RIGHT )
		g_XBaseFont->Print( this->m_WindowPosition.x , m_WindowPosition.y+24,  1.0f, "RIGHT" );
	if( m_WindowDockingType == _XW_DOCKINGTYPE_BOTTOM )
		g_XBaseFont->Print( this->m_WindowPosition.x , m_WindowPosition.y+24,  1.0f, "BOTTOM" );

	g_XBaseFont->Flush();
	
#endif

}


void _XWindow::DrawWindowEffect( void )
{	
	int differ_x,differ_y;
	int differ_x1, differ_y1;

	differ_x  = (g_WindowCloseRect.left   - m_WindowPosition.x)/_XDEF_WINDOWEFFECT_FRAME;
	differ_y  = (g_WindowCloseRect.top    - m_WindowPosition.y)/_XDEF_WINDOWEFFECT_FRAME;

	differ_x1 = (g_WindowCloseRect.right  - (m_WindowPosition.x+m_WindowSize.cx)) /_XDEF_WINDOWEFFECT_FRAME;
	differ_y1 = (g_WindowCloseRect.bottom - (m_WindowPosition.y+m_WindowSize.cy)) /_XDEF_WINDOWEFFECT_FRAME;

	FLOAT left		= m_WindowPosition.x + differ_x * m_WindowAnimationLevel;
	FLOAT top		= m_WindowPosition.y + differ_y * m_WindowAnimationLevel;
	FLOAT right		= m_WindowPosition.x+m_WindowSize.cx + differ_x1* m_WindowAnimationLevel;
	FLOAT bottom	= m_WindowPosition.y+m_WindowSize.cy + differ_y1* m_WindowAnimationLevel;

	g_WindowEffectVertices[0].x = left;
	g_WindowEffectVertices[0].y = top;
	g_WindowEffectVertices[1].x = right;
	g_WindowEffectVertices[1].y = top;

	g_WindowEffectVertices[2].x = right;
	g_WindowEffectVertices[2].y = top;
	g_WindowEffectVertices[3].x = right;
	g_WindowEffectVertices[3].y = bottom;

	g_WindowEffectVertices[4].x = right;
	g_WindowEffectVertices[4].y = bottom;
	g_WindowEffectVertices[5].x = left;
	g_WindowEffectVertices[5].y = bottom;

	g_WindowEffectVertices[6].x = left;
	g_WindowEffectVertices[6].y = bottom;
	g_WindowEffectVertices[7].x = left;
	g_WindowEffectVertices[7].y = top;

	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 0 );
	gpDev->SetTexture(0, NULL); 	
	/*gpDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	
	gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	gpDev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	gpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );
	gpDev->SetRenderState( D3DRS_ZENABLE,   D3DZB_FALSE );*/

	gpDev->SetFVF( D3DFVF_XWINDOWEFFECTVERTEX ); 
	gpDev->DrawPrimitiveUP(D3DPT_LINELIST, 4, g_WindowEffectVertices, sizeof(_XWINDOWEFFECTVERTEXTYPE));	
}


BOOL _XWindow::Process( _XGUIObject*& pfocusobject )
{
	if( !this->m_Enable     ) return FALSE;
	if( !this->m_ShowWindow ) return FALSE;	

	BOOL retvalue = FALSE;

	if( !m_ObjectList.listEmpty() )
	{
		m_ObjectList.resetList();
		m_ObjectList.prevItem();
		_XGUIObject* currentObject = NULL;

		do
		{
			currentObject = (_XGUIObject*)m_ObjectList.currentItem();

			if(currentObject) 
			{
				if( currentObject->Process( pfocusobject ) )
				{
					retvalue = TRUE;					
				}
			}

			m_ObjectList.prevItem();
		}while( !m_ObjectList.atEndOfList() );
	}
		
	MouseState* mousestate = gpInput->GetMouseState();

	if( mousestate )
	{
		if( !pfocusobject )
		{
			if( CheckMousePosition() && mousestate->bButton[0] )
			{
				pfocusobject = (_XGUIObject*)this;

				if( (m_WindowMoveMode != _XWMOVE_FIXED) && !m_Dragging )
				{
					m_Dragging = TRUE;
					ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
					m_DragStartPos.x = m_WindowPosition.x - scrnpos->x;
					m_DragStartPos.y = m_WindowPosition.y - scrnpos->z;
					gApp->ReadyCursor( g_SelectClickCursorIndex );
				}
			}
		}
			
		if( !mousestate->bButton[0] )
		{
			if( CheckMousePosition() )
			{	
				if( pfocusobject == (_XGUIObject*)this )
				{
					if( m_CommandID )
					{						
						PostMessage(gHWnd, WM_COMMAND, MAKEWPARAM(m_CommandID, 0), (LPARAM)gHWnd);
						mousestate->bButton[0] = 0;
					}						
					m_Dragging = FALSE;	
					m_DragStartPos.x = m_DragStartPos.y = 0;					
					pfocusobject = NULL;
					return TRUE;
				}		
			}
			if( pfocusobject == (_XGUIObject*)this )
			{
				pfocusobject = NULL;
				m_Dragging = FALSE;	
				m_DragStartPos.x = m_DragStartPos.y = 0;
				return FALSE;
			}			
		}
		else if( pfocusobject == (_XGUIObject*)this )
		{
			if( m_Dragging )
			{
				ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
				int newxpos = scrnpos->x + m_DragStartPos.x;
				int newypos = scrnpos->z + m_DragStartPos.y;

				if( newxpos < 0	) newxpos = 0;
				if( newypos < 0	) newypos = 0;				
				if( newxpos+m_WindowSize.cx >= gnWidth ) newxpos = gnWidth - m_WindowSize.cx;
				if( newypos+m_WindowSize.cy >= gnHeight) newypos = gnHeight- m_WindowSize.cy;

				switch( m_WindowMoveMode )
				{
				case _XWMOVE_FREE :
					WindowPosChanging( newxpos, newypos );
					MoveWindow( newxpos, newypos );
					break;
				case _XWMOVE_HORIZ :
					WindowPosChanging( newxpos, (int&)m_WindowPosition.y );
					MoveWindow( newxpos, this->m_WindowPosition.y );					
					break;
				case _XWMOVE_VERT :								
					WindowPosChanging( (int&)this->m_WindowPosition.x, newypos );
					MoveWindow( this->m_WindowPosition.x, newypos );					
					break;
				}				

				gApp->ReadyCursor( g_SelectClickCursorIndex );
			}
		}
	}

	return retvalue;
}


BOOL _XWindow::ProcessWheelMessage( short zDelta )
{
	if( !this->m_ShowWindow ) return FALSE;
	if( !this->m_Enable     ) return FALSE;
	
	if( !CheckMousePosition() ) return FALSE;

	if( !m_ObjectList.listEmpty() )
	{
		m_ObjectList.resetList();
		_XGUIObject* currentObject = NULL;
		
		do
		{
			currentObject = (_XGUIObject*)m_ObjectList.currentItem();
			
			if(currentObject)
			{
				if( currentObject->ProcessWheelMessage( zDelta ) )
				{
					return TRUE;
				}
			}
			
			m_ObjectList.nextItem();
		}while( !m_ObjectList.atHeadOfList() );
	}

	return FALSE;
}

void _XWindow::SetClippingArea( FLOAT left, FLOAT top, FLOAT right, FLOAT bottom )
{
	D3DVIEWPORT9 ClippingViewPort;

	if( left < 0 ) ClippingViewPort.X = 0;
	else ClippingViewPort.X = left;
	
	if( ClippingViewPort.X >= gnWidth ) ClippingViewPort.X = gnWidth-1;
	
	if( top < 0 ) ClippingViewPort.Y = 0;
	else ClippingViewPort.Y = top;
	
	if( ClippingViewPort.Y >= gnHeight ) ClippingViewPort.Y = gnHeight-1;
	
	
	ClippingViewPort.Width = right - left;
	
	if( ClippingViewPort.X + ClippingViewPort.Width >= gnWidth )
	{
		ClippingViewPort.Width -=  (ClippingViewPort.X + ClippingViewPort.Width) - gnWidth;
	}
	
	if( ClippingViewPort.Width < 1 ) ClippingViewPort.Width = 1;
	
	ClippingViewPort.Height = bottom - top;
	
	if( ClippingViewPort.Y + ClippingViewPort.Height >= gnHeight )
	{
		ClippingViewPort.Height -=  (ClippingViewPort.Y + ClippingViewPort.Height ) - gnHeight;
	}
	
	if( ClippingViewPort.Height < 1 ) ClippingViewPort.Height = 1;
	
	ClippingViewPort.MinZ = 0.0f;
	ClippingViewPort.MaxZ = 1.0f;

	gpDev->SetViewport(&ClippingViewPort);
}

void _XWindow::DisableClippingArea( void )
{
	gpDev->SetViewport(&g_MainViewPort);
}

void _XWindow::NotifyUIChange( void )
{
	if( !m_ObjectList.listEmpty() )
	{
		m_ObjectList.resetList();
		_XGUIObject* currentObject = NULL;
		
		do
		{
			currentObject = (_XGUIObject*)m_ObjectList.currentItem();
			
			if(currentObject)
			{
				currentObject->NotifyUIChange();
			}
			
			m_ObjectList.nextItem();
		}while( !m_ObjectList.atHeadOfList() );
	}	
}



