// XWindowManager.cpp: implementation of the _XWindowManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


RECT g_WindowCloseRect = { 0, 0, 0, 0 };
_XWINDOWEFFECTVERTEXTYPE g_WindowEffectVertices[8];



_XWindowManager::_XWindowManager()
{
	m_pFocusedWindow = NULL;
	m_CheckSelfDestroyWindow = FALSE;

	int i = 0 ; for(  i=0; i<8; i++ )
	{
		g_WindowEffectVertices[i].z			= 0.1f;
		g_WindowEffectVertices[i].rhw       = 1.0f;
		g_WindowEffectVertices[i].diffuse   = D3DCOLOR_ARGB( 128, 185, 175, 160 );
	}

	m_bStlInit = TRUE;
}

_XWindowManager::~_XWindowManager()
{
	DestroyManager();
	m_bStlInit = FALSE;
}

void _XWindowManager::DestroyManager( void )
{	
	if( m_WindowList.listEmpty() ) return;

	// Save window property

	_XWindow* currentObject = NULL;
	int i = 0 ; for(  i = 0; i < m_WindowList.getitemcount(); i++ )
	{
		currentObject = (_XWindow*)m_WindowList.getItem( i );

		if(currentObject)
		{				
			currentObject->SaveWindowProperty();
			currentObject->DestroyWindow();
			
			//_XDWINPRINT( "Save [%s : %d] window properties", currentObject->GetWindowTitle(), currentObject->GetWindowID() );
		}
	}

	// by mahwang
	m_smWindow.clear();
	
	m_WindowList.disposeList();
	_XDWINPRINT( "Disposed _XGUI_Windows" );
}

void _XWindowManager::DeleteAllWindow( int windowid )
{
	if( m_WindowList.listEmpty() ) return;
	
	// by mahwang
	smdef_XWindow::iterator it = m_smWindow.begin();
	while(it != m_smWindow.end())
	{
		if ((*it).second->GetWindowID() == windowid)
		{
			it = m_smWindow.erase(it);
		}
		else
		{
			++ it;
		}
	}


	m_WindowList.resetList();
	_XWindow* currentObject = NULL;
	
	do
	{
		currentObject = (_XWindow*)m_WindowList.currentItem();
		
		if(currentObject)
		{
			if( currentObject->GetWindowID() == windowid )
			{
				if( m_pFocusedWindow == currentObject )
				{
					m_pFocusedWindow = NULL;
				}

				currentObject->SaveWindowProperty();

				m_WindowList.deleteItem( currentObject );				
			}
			else
			{
				m_WindowList.nextItem();
			}
		}		
		else
		{
			break;
		}

		if( m_WindowList.listEmpty() ) break;
		
	}while( !m_WindowList.atHeadOfList() );
}

void _XWindowManager::DeleteWindow( int windowid )
{
	_XWindow* window = FindWindow( windowid );
	if( window )
	{
		// by mahwang
		smdef_XWindow::iterator it = m_smWindow.find(windowid);
		if (it != m_smWindow.end())
		{
			m_smWindow.erase(it);
		}

		window->SaveWindowProperty();
		
		if( m_pFocusedWindow == window )
		{
			m_pFocusedWindow = NULL;
		}
		
		m_WindowList.deleteItem( window );
	}	
}

_XWindow* _XWindowManager::InsertWindow( _XWindow* xguiobject )
{
	assert( xguiobject );
	if( xguiobject )
	{		
		m_pFocusedWindow = xguiobject;
		
		m_pFocusedWindow->SetMyManager( this );

		m_WindowList.resetList();		
		m_WindowList.insertItem( xguiobject );
		m_WindowList.setAnchor( xguiobject );		

		// by mahwang
		m_smWindow.insert(smdef_XWindow::value_type(xguiobject->GetWindowID(), xguiobject));

		return xguiobject;
	}
	return NULL;
}

_XWindow* _XWindowManager::FindWindow( LPSTR windowtitle )
{
	if( m_WindowList.listEmpty() ) return NULL;
	
	m_WindowList.resetList();
	_XWindow* currentObject = NULL;

	do
	{
		currentObject = (_XWindow*)m_WindowList.currentItem();

		if(currentObject)
		{
			if( strcmp( currentObject->GetWindowTitle(), windowtitle ) == 0 )
			{
				return currentObject;
			}
		}

		m_WindowList.nextItem();
	}while( !m_WindowList.atHeadOfList() );

	return NULL;
}

_XWindow* _XWindowManager::FindWindow( DWORD windowid )
{
	// by mahwang
	if (m_bStlInit == FALSE) return NULL;
	smdef_XWindow::iterator it = m_smWindow.find(windowid);
	if (it != m_smWindow.end())
	{
		return (*it).second;
	}
	return NULL;
/*
	if( m_WindowList.listEmpty() ) return NULL;
	
	m_WindowList.resetList();
	_XWindow* currentObject = NULL;
	
	do
	{
		currentObject = (_XWindow*)m_WindowList.currentItem();
		
		if(currentObject)
		{
			if( currentObject->GetWindowID() == windowid )
			{
				return currentObject;
			}
		}
		
		m_WindowList.nextItem();
	}while( !m_WindowList.atHeadOfList() );
	
	return NULL;
*/
}

int _XWindowManager::FindWindowIndex( LPSTR windowtitle )
{
	if( m_WindowList.listEmpty() ) return -1;
	
	m_WindowList.resetList();
	_XWindow* currentObject = NULL;
	
	int windowindex = 0;

	do
	{
		currentObject = (_XWindow*)m_WindowList.currentItem();

		if(currentObject)
		{
			if( strcmp( currentObject->GetWindowTitle(), windowtitle ) == 0 )
			{
				return windowindex;
			}
		}

		windowindex++;
		m_WindowList.nextItem();

	}while( !m_WindowList.atHeadOfList() );

	return -1;
}

int _XWindowManager::FindWindowIndex( DWORD windowid )
{
	if( m_WindowList.listEmpty() ) return -1;
	
	m_WindowList.resetList();
	_XWindow* currentObject = NULL;
	
	int windowindex = 0;
	
	do
	{
		currentObject = (_XWindow*)m_WindowList.currentItem();
		
		if(currentObject)
		{
			if( currentObject->GetWindowID() == windowid )
			{
				return windowindex;
			}
		}
		
		windowindex++;
		m_WindowList.nextItem();
		
	}while( !m_WindowList.atHeadOfList() );
	
	return -1;
}

_XWindow* _XWindowManager::GetWindow( int index )
{
	return (_XWindow*)this->m_WindowList.getItem( index );
}

void _XWindowManager::Draw( void )
{
	if( m_WindowList.listEmpty() ) return;

	_XWindow* currentObject = NULL;
	int i = 0 ; for(  i = m_WindowList.getitemcount()-1; i >= 0 ; i-- )
	{
		currentObject = (_XWindow*)m_WindowList.getItem( i );	

		if(currentObject) 
		{				
		   currentObject->Draw();
		}		
	}
}

BOOL _XWindowManager::ProcessWheelMessage( short zDelta )
{
	if( m_WindowList.listEmpty() ) return FALSE;
	
	m_WindowList.resetList();
	_XWindow* currentObject = NULL;
	
	do
	{
		currentObject = (_XWindow*)m_WindowList.currentItem();
		
		if(currentObject)
		{
			if( currentObject->ProcessWheelMessage( zDelta ) )
			{
				return TRUE;
			}
		}	
		
		m_WindowList.nextItem();
		
	}while( !m_WindowList.atHeadOfList() );


	return FALSE;
}

void _XWindowManager::Process( void )
{
	if( m_WindowList.listEmpty() ) return;
	
	m_WindowList.resetList();
	_XWindow* currentObject = NULL;

	if( m_pFocusedWindow )
	{
		if( !m_pFocusedWindow->IsEnableWindow() )
		{			
			_XWindow* nextwindow = m_pFocusedWindow->GetNextXWindow();
			if( nextwindow )
				m_WindowList.setAnchor( nextwindow );
		}

		MouseState* mousestate = gpInput->GetMouseState();
		
		if( mousestate->bButton[0] )
		{
			if( !m_pFocusedWindow->IsDragging() &&
				!g_CurrentFocusedObject &&
				!m_pFocusedWindow->CheckMousePosition() )
			{	
				int windowcount = m_WindowList.getitemcount();
				int i = 0 ; for(  i = 0; i < windowcount; i++ )
				{
					currentObject = (_XWindow*)m_WindowList.getItem( i );

					if(currentObject)
					{
						if( currentObject->CheckMousePosition() )
						{
							m_pFocusedWindow = currentObject;

							if( currentObject->GetEnableTopWindow() )
							{
								m_WindowList.removeItem( currentObject );
								m_WindowList.resetList();
								m_WindowList.insertItem( currentObject );							
								m_WindowList.setAnchor( currentObject ); // Front window...
							}
							
							break;
						}
					}
				}				
			}	
		}
	}	
	
	//int windowcount = m_WindowList.getitemcount();
	_XWindow* pCurrentWindow = (_XWindow*)m_WindowList.getAnchor();
	_XWindow* pLastFocusedWindow = NULL;

	if( pCurrentWindow )
	{
		do 
		{
			pCurrentWindow->Process();

			if( g_CurrentFocusedObject == (_XGUIObject*)pCurrentWindow )
			{
				pLastFocusedWindow = pCurrentWindow;
			}

			pCurrentWindow = (_XWindow*)pCurrentWindow->getright();
			
		} while( (_XWindow*)m_WindowList.getAnchor() != pCurrentWindow );
	}
	
	if(pLastFocusedWindow)
	{
		SetTopWindow( pLastFocusedWindow );
	}

	if( m_CheckSelfDestroyWindow )
	{
		if( !m_WindowList.listEmpty() )
		{
			m_WindowList.resetList();

			do
			{
				currentObject = (_XWindow*)m_WindowList.currentItem();
				
				if(currentObject)
				{			
					if( currentObject->GetSelfDestroyWindow() )
					{
						DWORD windowid = currentObject->GetWindowID();

						if( m_pFocusedWindow == currentObject ) m_pFocusedWindow = NULL;												
						m_WindowList.deleteItem( currentObject );

						// by mahwang
						smdef_XWindow::iterator it = m_smWindow.find(windowid);
						if (it != m_smWindow.end())
						{
							m_smWindow.erase(it);
						}

						if( m_WindowList.listEmpty() ) break;
					}
					else
					{
						m_WindowList.nextItem();
					}
				}				
				else
				{
					break;
				}

			}while( !m_WindowList.atHeadOfList() );
		}	

		m_CheckSelfDestroyWindow = FALSE;
	}
}

void _XWindowManager::SetTopWindow( _XWindow* ptopwindow )
{
	if( !ptopwindow ) return;
	if( !ptopwindow->GetEnableTopWindow() ) return;

	if( m_WindowList.searchItem(ptopwindow) != -1 )
	{
		if( m_WindowList.removeItem( ptopwindow ) )
		{
			m_WindowList.resetList();
			m_WindowList.insertItem( ptopwindow );
			m_WindowList.setAnchor( ptopwindow ); // Front window...
			m_pFocusedWindow = ptopwindow;
		}
	}
}



void _XWindowManager::RebuildDockedWindow(void)
{
	_XWindow* currentObject = NULL;

	int i = 0 ; for(  i = 0; i < m_WindowList.getitemcount() ; i++ )
	{
		currentObject = (_XWindow*)m_WindowList.getItem( i );	
		
		if(currentObject) 
		{				
			currentObject->RebuildDockedWindow();
		}		
	}	
}

void _XWindowManager::SetDefaultPosition( void )
{
	_XWindow* currentObject = NULL;
	
	int rep =0 ; for( rep = 0; rep < 2; rep++ ){	// 도킹 순서 처리전...
		int i = 0 ; for(  i = 0; i < m_WindowList.getitemcount() ; i++ )
		{
			currentObject = (_XWindow*)m_WindowList.getItem( i );	
			
			if(currentObject) 
			{				
				currentObject->SetDefaultPosition();
			}		
		}
	}
}

BOOL _XWindowManager::IsTopWindow(_XWindow* topwindow)
{
	_XWindow* anchor = (_XWindow *)m_WindowList.getAnchor();
	
	if(anchor == topwindow)
		return TRUE;
	else
		return FALSE;
}

//2004.06.24->oneway48 insert
BOOL _XWindowManager::EscapeWindow()
{
	if( m_WindowList.listEmpty() ) return FALSE;
	
	m_WindowList.resetList();
	_XWindow* currentObject = NULL;
	
	do
	{
		currentObject = (_XWindow*)m_WindowList.currentItem();
		
		if(currentObject)
		{
			if( currentObject->GetEscapeWindow() )
			{
				if( currentObject->GetShowStatus() )
				{
					if( currentObject == m_pFocusedWindow )
					{
						currentObject->ShowWindow(FALSE);
						return TRUE;
					}
				}
			}
		}	
		
		m_WindowList.nextItem();
		
	}while( !m_WindowList.atHeadOfList() );
	
	
	do
	{
		currentObject = (_XWindow*)m_WindowList.currentItem();
		
		if(currentObject)
		{
			if( currentObject->GetEscapeWindow() )
			{
				if( currentObject->GetShowStatus() )
				{
					currentObject->ShowWindow(FALSE);
					return TRUE;
				}
			}
		}	
		
		m_WindowList.nextItem();
		
	}while( !m_WindowList.atHeadOfList() );
	return FALSE;	
}

BOOL _XWindowManager::OnKeyDown(WPARAM wparam, LPARAM lparam)
{
	if( m_pFocusedWindow && m_pFocusedWindow->OnKeyDown( wparam, lparam ) )
	{
		return TRUE;
	}
	else
	{
		if( m_WindowList.listEmpty() ) return FALSE;
		
		m_WindowList.resetList();
		_XWindow* currentObject = NULL;
		
		do
		{
			currentObject = (_XWindow*)m_WindowList.currentItem();
			
			if(currentObject)
			{
				if( currentObject != m_pFocusedWindow )
				{
					if( currentObject->OnKeyDown( wparam, lparam ) )
					{
						return TRUE;
					}
				}
			}	
			
			m_WindowList.nextItem();
			
		}while( !m_WindowList.atHeadOfList() );
	}
	
	return FALSE;
}

void _XWindowManager::NotifyUIChange( void )
{
	if( m_WindowList.listEmpty() ) return;
	
	m_WindowList.resetList();
	_XWindow* currentObject = NULL;
	
	do
	{
		currentObject = (_XWindow*)m_WindowList.currentItem();
		
		if(currentObject)
		{
			currentObject->NotifyUIChange();
		}	
		
		m_WindowList.nextItem();
		
	}while( !m_WindowList.atHeadOfList() );
}