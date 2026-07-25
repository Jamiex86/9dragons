// XTreeBox.cpp: implementation of the _XTreeBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XNodeData::_XNodeData()
{
	NodeId			= 0;
	ParentNode		= NULL;
	Index			= 0;
	Opened			= false;
}

_XNodeData::~_XNodeData()
{
	if( !ChildIdList.empty() )
	{
		list<_XNodeData*>::iterator iter = ChildIdList.begin();
		for( ; iter != ChildIdList.end(); iter++ )
		{
			_XNodeData* tempNode = *iter;
			if( tempNode )
				SAFE_DELETE(tempNode);
			iter = ChildIdList.erase(iter);
		}
		ChildIdList.clear();
	}
}

_XNodeData* _XNodeData::FindNodeByIndex(_XNodeData*findNode, int index)
{
	if( findNode ) return findNode;
	if( Index == index )
	{
		findNode = this;
		return findNode;
	}
	else
	{
		if( Opened && !ChildIdList.empty() )
		{
			list<_XNodeData*>::iterator iter = ChildIdList.begin();
			for( ; iter != ChildIdList.end(); iter++ )
			{
				_XNodeData* tempNode = *iter;
				if( tempNode )
					findNode = tempNode->FindNodeByIndex(findNode, index);
			}
		}		
	}
	return findNode;
}

_XNodeData* _XNodeData::FindNodeByCommandId(_XNodeData*findNode,int commandid)
{
	if( findNode ) return findNode;
	if( NodeId == commandid )
	{
		findNode = this;
		return findNode;
	}
	else
	{
		if( !ChildIdList.empty() )
		{
			list<_XNodeData*>::iterator iter = ChildIdList.begin();
			for( ; iter != ChildIdList.end(); iter++ )
			{
				_XNodeData* tempNode = *iter;
				if( tempNode )
					findNode = tempNode->FindNodeByCommandId(findNode, commandid);
			}
		}		
	}
	return findNode;
}

int _XNodeData::GrantIndex(int count)
{
	Index = count;
	count++;
	
	if( Opened )
	{
		if( !ChildIdList.empty() )
		{
			list<_XNodeData*>::iterator iter = ChildIdList.begin();
			for( ; iter != ChildIdList.end(); iter++ )
			{
				_XNodeData* tempNode = *iter;
				if( tempNode )
					count = tempNode->GrantIndex(count);
			}
		}
	}
	return count;
}

int _XNodeData::ParentNodeCount(_XNodeData* parentnode, int count)
{
	if( parentnode )
	{
		count++;
		if( parentnode->ParentNode )
			count = ParentNodeCount( parentnode->ParentNode, count );
	}

	return count;
}

_XTreeBox::_XTreeBox()
{
	m_YPitch			=	12;
	m_TrackWidth		=	64;
	m_TrackHeight		=	12;
	m_SelectedItem		=	-1;
	m_PrevSelectedItem	=	-1;
	m_MaxViewLineCount	=	0;
	m_CurrentScrollPos	=	0;
	m_IndexListCount	=	0;
	m_TextXOffset		=	0;	
	m_ItemSelectable	=	TRUE;
	m_pScrbarFocusedObject	= NULL;
	
	
	m_ColumnWidth	 = 100;
	m_ColumnAlignType = _XFONT_ALIGNTYPE_LEFT;	// 좌상 기본 정렬
	m_ColumnOffset.x  = 0;
	m_ColumnOffset.y  = 0;		

	m_ColumnClipRect.left	=	0.0f;
	m_ColumnClipRect.top		=	0.0f;
	m_ColumnClipRect.right	=	gnWidth-1;
	m_ColumnClipRect.bottom	=	gnHeight-1;
	
	
	m_SelectedImageHeight = 0;
	m_SelectedImagePos.x = 0;
	m_SelectedImagePos.y = 0;

	m_FontID				=	_XFONTID_BASE;

	m_ItemSelectCallBack	=	NULL;
	m_ItemMouseHoverCallBack=	NULL;	

	m_bAlwaysShowScrollBarMode = FALSE;
	m_bMoveScrollBarMode	=	FALSE;
	m_ScrollBarPositon.x	=	0.0f;
	m_ScrollBarPositon.y	=	0.0f;

	m_RootItem				= NULL;

	m_IconImage				= -1;
	m_IconImageEx			= -1;
	m_IconRect.left			= 0;
	m_IconRect.top			= 0;
	m_IconRect.right		= 0;
	m_IconRect.bottom		= 0;
	m_IconRectEx.left		= 0;
	m_IconRectEx.top		= 0;
	m_IconRectEx.right		= 0;
	m_IconRectEx.bottom		= 0;
	m_SelectedCommandId		= 0;
}

_XTreeBox::~_XTreeBox()
{
	SAFE_DELETE ( m_RootItem );
	
	m_ScrollBar.DestroyWindow();
	m_IconImageStatic.DestroyWindow();
	m_SelectStatic.ReleaseObject();
}

BOOL _XTreeBox::Create(  _XTREEBOX_STRUCTURE treeboxstructure  )
{
	SAFE_DELETE( m_RootItem );	
	m_ScrollBar.DestroyWindow();

	// 리스트 박스 윈도우 생성
	_XWINDOW_STRUCTURE windowstruct;

	windowstruct.enable				=	treeboxstructure.enable;
	windowstruct.position			=	treeboxstructure.position;
	windowstruct.windowsize			=	treeboxstructure.windowsize;
	windowstruct.commandid			=	treeboxstructure.commandid;
	windowstruct.windowtextureindex =	treeboxstructure.windowtextureindex;
	windowstruct.texturearchive		=	treeboxstructure.texturearchive;
	windowstruct.facecolor			=	0x00000000;

	_XWindow::Create( windowstruct );
	_XWindow::SetWindowMoveMode( _XWMOVE_FIXED );

	m_YPitch						=	treeboxstructure.ypitch;
	m_MaxViewLineCount				=	treeboxstructure.maxviewlinecount;	


	// 스크롤바 생성
	int scrollbarxpos = 0;
	scrollbarxpos = m_WindowPosition.x + m_WindowSize.cx - treeboxstructure.scrollbarsize.cx;
	
	_XSCRBAR_STRUCTURE listscrbarstructure =
	{
		TRUE, 
		{scrollbarxpos,m_WindowPosition.y}, 
		treeboxstructure.scrollbarsize.cx, 
		treeboxstructure.scrollbarsize.cy,	// width, height
		treeboxstructure.btnysize,			// btnysize
		treeboxstructure.trackbarysize,		// trackbar size
		100, 
		m_MaxViewLineCount, 
		0,
		treeboxstructure.imageindex_uparrow,
		treeboxstructure.imageindex_uparrowclick,
		treeboxstructure.imageindex_downarrow,
		treeboxstructure.imageindex_downarrowclick,
		treeboxstructure.imageindex_trackbar,
		treeboxstructure.texturearchive			
	};

	m_ScrollBar.Create( listscrbarstructure );
	m_ScrollBar.ShowWindow( TRUE );
	m_ScrollBar.SetViewBorder( TRUE );
	m_ScrollBar.SetTotalLineCount(0);
	m_ScrollBar.SetViewLineCount( m_MaxViewLineCount );	
	m_ScrollBar.UpdateData();	
	m_ScrollBar.RebuildTrackBar();

	m_IconImageStatic.Create( 0, 0, 12, 12, NULL, -1 );
	
	m_SelectedImageStatic.Create(0, 0, 12, 12, NULL, -1);

	m_TrackWidth = m_WindowSize.cx - treeboxstructure.scrollbarsize.cx;
	m_TrackHeight = m_YPitch-1;

	m_SelectStatic.Create( 0, 0, m_TrackWidth, m_TrackHeight,
						   _XDEF_DEFAULTDEPTH_FORINTERFACE, D3DCOLOR_ARGB(16,128,128,128) );
	m_SelectedStatic.Create( 0, 0, m_TrackWidth, m_TrackHeight,
						   _XDEF_DEFAULTDEPTH_FORINTERFACE, D3DCOLOR_ARGB(64,128,128,128) );
		
	m_FontID				=	_XFONTID_BASE;

	return TRUE;
}

void _XTreeBox::DestroyWindow( void )
{
	m_ItemSelectCallBack = NULL;
	m_ItemMouseHoverCallBack = NULL;
}

void _XTreeBox::SetFont( _XFONTID fontid )
{ 
	m_FontID = fontid;
}

void _XTreeBox::MoveWindow( int X, int Y )
{	
	_XWindow::MoveWindow( X, Y );

	if( !m_bMoveScrollBarMode )
	{
		int scrollbarxpos = 0;
		
		scrollbarxpos = m_WindowPosition.x + m_WindowSize.cx - m_ScrollBar.GetWindowWidth();
		
		m_ScrollBar.MoveWindow( scrollbarxpos, m_WindowPosition.y );
	}
	else
	{
		m_ScrollBar.MoveWindow( m_WindowPosition.x + m_ScrollBarPositon.x, m_WindowPosition.y + m_ScrollBarPositon.y );

	}
	
	m_ColumnClipRect.left	=	m_WindowPosition.x + m_ColumnOffset.x;
	m_ColumnClipRect.right	=	m_ColumnClipRect.left + m_ColumnWidth;

	m_ColumnClipRect.top		=	m_WindowPosition.y;
	m_ColumnClipRect.bottom	=	m_WindowPosition.y + m_WindowSize.cy;	
}

int	_XTreeBox::CheckSelectedItem( void )
{
	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
		
	if( scrnpos->x <= (m_WindowPosition.x + m_TextXOffset) ||
		scrnpos->x >= (m_WindowPosition.x + m_TextXOffset + m_TrackWidth) ||
		scrnpos->z <= m_WindowPosition.y ||
		scrnpos->z >= m_WindowPosition.y + m_WindowSize.cy ) return -1;

	int itempos = ((scrnpos->z - m_WindowPosition.y) / m_YPitch) + m_CurrentScrollPos;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode, itempos);

	if( findNode )
	{
		if( findNode->NodeString.GetCommandID() > 0 )
			return itempos;
	}
	return -1;
}

void _XTreeBox::Draw( _XGUIObject*& pfocusobject )
{
	if( !this->m_Enable     ) return;

	_XWindow::Draw( pfocusobject );

	if( !this->m_ShowWindow ) return;

	_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );
	
	if( m_RootItem )
	{
		BOOL usehighlight = FALSE;

		int ypos = 0;
		int ycoord = m_WindowPosition.y;
		ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();

		int i = 0 ; for(  i = m_CurrentScrollPos; i < m_IndexListCount; i++ )
		{
			_XNodeData* findNode = NULL;
			findNode = m_RootItem->FindNodeByIndex(findNode, i);
			
			if( findNode )
			{
				if( m_ItemSelectable )
				{
					if( m_SelectedItem == m_CurrentScrollPos+ypos )
					{
						if( findNode->NodeString.GetCommandID() )
						{
							if( m_SelectedImageStatic.m_TextureIndex == -1 )
							{
								m_SelectedStatic.MoveWindow( m_WindowPosition.x + m_TextXOffset, ycoord );
								m_SelectedStatic.RenderSolid();								
							}
							else
							{
								m_SelectedImageStatic.Draw(m_WindowPosition.x+m_SelectedImagePos.x, 
									m_WindowPosition.y+m_SelectedImagePos.y+(ypos*m_SelectedImageHeight));
							}
							
							if( CheckMousePosition() ) 
								gApp->ReadyCursor( g_SelectCursorIndex );
							
							usehighlight = TRUE;
						}
					}
					else
					{
						if( !pfocusobject || pfocusobject == (_XGUIObject*)this )
						{
							if( scrnpos->x > (m_WindowPosition.x + m_TextXOffset) && 
								scrnpos->x < (m_WindowPosition.x + m_TextXOffset + m_TrackWidth) &&
								scrnpos->z > ycoord && scrnpos->z < (ycoord+m_TrackHeight-1) )
							{								
								if( findNode->NodeString.GetCommandID() )
								{
									if( m_SelectedImageStatic.m_TextureIndex == -1 )
									{
										findNode->NodeString.SetBoldString(TRUE);
										m_SelectStatic.MoveWindow( m_WindowPosition.x + m_TextXOffset, ycoord );
										m_SelectStatic.RenderSolid();
									}
									else
									{
										findNode->NodeString.SetBoldString(TRUE);
										m_SelectedImageStatic.Draw(m_WindowPosition.x+m_SelectedImagePos.x, 
											m_WindowPosition.y+m_SelectedImagePos.y+(ypos*m_SelectedImageHeight));
										usehighlight = TRUE;
									}
									
									if( CheckMousePosition() ) 
										gApp->ReadyCursor( g_SelectCursorIndex );
								}
							}
							else
							{
								findNode->NodeString.SetBoldString(FALSE);
							}
						}						
					}
				}

				D3DCOLOR color;
				if(usehighlight)
					color = findNode->NodeString.GetAttribHighlight();
				else
					color = findNode->NodeString.GetAttrib();
				
				pXFont->SetClippingArea( m_ColumnClipRect );
				pXFont->SetColor(color);
				
				pXFont->PrintAlign( m_WindowPosition.x + m_ColumnOffset.x + m_TextXOffset+findNode->Pos, 
					ycoord + m_ColumnOffset.y, 1.0f, m_ColumnAlignType, 
					findNode->NodeString.GetString() );

				if( findNode->NodeString.GetBoldString() )//한번 더 그려서 진하게 만든다.
				{
					pXFont->PrintAlign( m_WindowPosition.x + m_ColumnOffset.x + m_TextXOffset + 1+findNode->Pos, 
						ycoord + m_ColumnOffset.y, 1.0f, m_ColumnAlignType, 
						findNode->NodeString.GetString() );
				}
				
				pXFont->Flush();								
				pXFont->DisableClippingArea();

				if( findNode->NodeIcon.GetIconArchive() )
				{						
					if( findNode->NodeId > 1 )
					{
						if( findNode->NodeIcon.GetAttrib() != (DWORD)-1 )
						{
							m_IconImageStatic.SetTexture( findNode->NodeIcon.GetIconArchive(), findNode->NodeIcon.GetAttrib() );
							m_IconImageStatic.SetClipRect( findNode->NodeIcon.GetRect(0) );
							m_IconImageStatic.Draw( m_WindowPosition.x + m_ColumnOffset.x + m_TextXOffset+findNode->Pos-13, ycoord + m_ColumnOffset.y );
						}
					}					
				}
			}			
			usehighlight = FALSE;
					
			ypos++;
			ycoord += m_YPitch;
			if( ypos >= m_MaxViewLineCount ) break;
		}		
	}
	
	m_CurrentScrollPos = m_ScrollBar.GetLineScrollPos();
	m_ScrollBar.Draw( m_pScrbarFocusedObject );	
}

BOOL _XTreeBox::Process( _XGUIObject*& pfocusobject )
{	
	if( !this->m_Enable     ) return FALSE;	
	if( m_pScrbarFocusedObject ) return TRUE;	
	if( !this->m_ShowWindow ) return FALSE;
	
	m_ScrollBar.Process( pfocusobject );

	if( m_ItemSelectable )
	{
		MouseState* mousestate = gpInput->GetMouseState();
		
		if( mousestate )
		{
			if( !pfocusobject )
			{
				int selecteditemcommand = CheckSelectedItem();

				if( selecteditemcommand != -1 && mousestate->bButton[0] )
				{
					pfocusobject = (_XGUIObject*)this;
					_XPlayInterfaceSound( g_ClickSoundIndex );
				}	
			}	
			
			if( !mousestate->bButton[0] )
			{
				int selecteditemcommand = CheckSelectedItem();

				if( selecteditemcommand != -1 )
				{	
					if( pfocusobject == (_XGUIObject*)this )
					{						
						if( m_CommandID )
						{
							_XNodeData* findNode = NULL;
							findNode = m_RootItem->FindNodeByIndex(findNode, selecteditemcommand);

							if( findNode )
							{
								if( m_ItemSelectCallBack )
								{
									if( findNode->NodeString.GetCommandID() > 0 )
									{
										m_SelectedCommandId = findNode->NodeId;
										if( findNode->Opened )
										{
											findNode->Opened = false;
											findNode->NodeIcon.SetAttrib( m_IconImageEx, 0, m_IconRectEx);											
										}
										else
										{
											findNode->Opened = true;
											findNode->NodeIcon.SetAttrib( m_IconImage, 0, m_IconRect);											
										}
										m_PrevSelectedItem = m_SelectedItem;
										m_SelectedItem = selecteditemcommand;
										m_RootItem->Opened = true;
											
										//PostMessage(gHWnd, WM_COMMAND, m_CommandID, findNode->NodeString.GetCommandID() );
										UpdateTreeBox();
										PostMessage(gHWnd, WM_COMMAND, m_CommandID, findNode->NodeId-1 );
									}
								}
								else
								{
									if( findNode->NodeString.GetCommandID() > 0 )
									{
										m_SelectedCommandId = findNode->NodeId;
										if( findNode->Opened )
										{
											findNode->Opened = false;
											if( findNode->NodeIcon.GetIconArchive() )
												findNode->NodeIcon.SetAttrib( m_IconImage, 0, m_IconRect);
										}
										else
										{
											findNode->Opened = true;
											if( findNode->NodeIcon.GetIconArchive() )
												findNode->NodeIcon.SetAttrib( m_IconImageEx, 0, m_IconRectEx);												
										}
										m_PrevSelectedItem = m_SelectedItem;
										m_SelectedItem = selecteditemcommand;							
										m_RootItem->Opened = true;
										
										//PostMessage(gHWnd, WM_COMMAND, m_CommandID, findNode->NodeString.GetCommandID() );
										UpdateTreeBox();
										PostMessage(gHWnd, WM_COMMAND, m_CommandID, findNode->NodeId-1 );
									}
								}
							}
							mousestate->bButton[0] = 0;
						}	
						
						pfocusobject = NULL;
						return TRUE;
					}		
				}
				else
				{
					// 트랙밖에서 클릭되었을때 -1을 보냄.. 선택취소 메세지...
					/* 현재는 사용 안함.
					if( pfocusobject == (_XGUIObject*)this )
						PostMessage(gHWnd, WM_COMMAND, m_CommandID, (LPARAM)-1 );
					*/
				}

				if( pfocusobject == (_XGUIObject*)this )
				{
					pfocusobject = NULL;
					return FALSE;
				}
			}		
		}
	}

	if( !_XWindow::Process( pfocusobject ) ) return FALSE;

	if( !this->m_ShowWindow ) return FALSE;

	return TRUE;
}

void _XTreeBox::SetMaxViewLineCount( int viewlinecount )
{ 
	m_MaxViewLineCount = viewlinecount;
	m_ScrollBar.SetViewLineCount( viewlinecount );
}

BOOL _XTreeBox::InsertListItem( LPTSTR itemstring, int commandid, int parentid, _XTextureManager* pIconArchive )
{
	if( parentid )
	{
		_XNodeData* tempParentNode = NULL;
		tempParentNode = m_RootItem->FindNodeByCommandId( tempParentNode, parentid );
		if( tempParentNode )
		{
			_XNodeData* tempNode = new _XNodeData;
			tempNode->NodeId = commandid+1;		
			tempNode->ParentNode = tempParentNode;
			tempNode->Opened = false;
			int count = 0;
			tempNode->Pos = 15*tempNode->ParentNodeCount(tempParentNode, count);
			
			if( strlen(itemstring) )
			{
				tempNode->NodeString.SetString( itemstring );
				tempNode->NodeString.SetAttrib( 0xFFD3D4CE );
				tempNode->NodeString.SetCommandID( tempNode->NodeId );
			}			
			
			tempParentNode->ChildIdList.push_back( tempNode );
			if( tempParentNode->NodeId != m_RootItem->NodeId )
			{
				if( pIconArchive )
					tempParentNode->NodeIcon.SetIconArchive( pIconArchive );
				tempParentNode->NodeIcon.SetAttrib( m_IconImage, 0, m_IconRect );
				tempParentNode->NodeIcon.SetCommandID( tempParentNode->NodeId );
			}
		}
	}
	else
	{
		m_RootItem = new _XNodeData;
		m_RootItem->NodeId = commandid+1;
		m_RootItem->ParentNode = NULL;
		m_RootItem->Opened = true;
		m_RootItem->Pos = 0;
		if( strlen(itemstring) )
		{
			m_RootItem->NodeString.SetString( itemstring );
			m_RootItem->NodeString.SetAttrib( 0xFFD3D4CE );
			m_RootItem->NodeString.SetCommandID( m_RootItem->NodeId );
		}
		if( pIconArchive )
			m_RootItem->NodeIcon.SetIconArchive( pIconArchive );
	}
	
	UpdateTreeBox();
	
	return TRUE;	
}

void _XTreeBox::SetIconArchive( int index, _XTextureManager* pIconArchive )
{
	if( index < 0 ) return;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode, index);

	if( findNode )
	{
		findNode->NodeIcon.SetIconArchive( pIconArchive );
	}
}

_XTextureManager* _XTreeBox::GetIconArchive( int index )
{
	if( index < 0 ) return NULL;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode, index);

	if( findNode )
	{
		return findNode->NodeIcon.GetIconArchive();
	}
	else
		return NULL;
}


BOOL _XTreeBox::SetItemText( int index, int column, LPTSTR itemstring , BOOL boldstring )
{
	if( index < 0 ) return NULL;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode, index);

	if( findNode )
	{
		findNode->NodeString.SetString( itemstring, column );
		findNode->NodeString.SetBoldString( boldstring );		
	}
	else
		return NULL;

	return TRUE;
}

LPTSTR _XTreeBox::GetItemText( int index, int column )
{
	if( index < 0 ) return NULL;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode, index);

	if( findNode )
	{
		return findNode->NodeString.GetString( column );
	}
	else
		return NULL;	
}

BOOL _XTreeBox::SetItemAttrib( int index, DWORD attrib )
{
	if( !m_RootItem ) return FALSE;
	if( index < 0 ) return FALSE;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByCommandId(findNode, index);

	if( findNode )
	{
		findNode->NodeString.SetAttrib( attrib );
	}
	else
		return FALSE;

	return TRUE;
}

BOOL _XTreeBox::SetItemAttrib(int index, DWORD attrib, RECT rect)
{
	if( !m_RootItem ) return FALSE;
	if( index < 0 ) return FALSE;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByCommandId(findNode, index);

	if( findNode )
	{
		findNode->NodeIcon.SetAttrib( attrib, 0, rect );
	}
	else
		return FALSE;

	return TRUE;
}

BOOL _XTreeBox::SetItemAttribHighlight(int index, DWORD attrib)
{
	if( !m_RootItem ) return FALSE;
	if( index < 0 ) return FALSE;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByCommandId(findNode, index);

	if( findNode )
	{
		findNode->NodeString.SetAttribHighlight(attrib);
	}
	else
		return FALSE;

	return TRUE;
}

DWORD _XTreeBox::GetItemAttrib( int index )
{
	if( !m_RootItem ) return 0;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode, index);
	if( findNode )
	{
		return findNode->NodeString.GetAttrib();
	}
	else
		return 0;
}

BOOL _XTreeBox::SetItemCommandID( int index, DWORD commandid )
{
	if( !m_RootItem ) return FALSE;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode,  index );

	if( findNode )
		findNode->NodeString.SetCommandID( commandid );
	else
		 return FALSE;
	
	return TRUE;
}

DWORD _XTreeBox::GetItemCommandID( int index )
{
	if( !m_RootItem ) return FALSE;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode,  index );

	if( findNode )
		findNode->NodeString.GetCommandID();
	else
		 return FALSE;
	
	return TRUE;
}

BOOL _XTreeBox::ProcessWheelMessage( short zDelta )
{	
	if( m_ScrollBar.ProcessWheelMessage( zDelta ) )
	{
		m_CurrentScrollPos = m_ScrollBar.GetLineScrollPos();
		return TRUE;
	}

	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();

	
	if( scrnpos->x <= (m_WindowPosition.x + m_TextXOffset) ||
		scrnpos->x >= (m_WindowPosition.x + m_TextXOffset + m_TrackWidth) ||
		scrnpos->z <= m_WindowPosition.y ||
		scrnpos->z >= m_WindowPosition.y + m_WindowSize.cy ) return FALSE;	
	
		
	if( _XWindow::ProcessWheelMessage( zDelta ) )
	{
		return TRUE;
	}
	
	int MoveDelta  = (FLOAT)zDelta / 40;			
	
	if( zDelta > 0 )
	{
		m_ScrollBar.LineUp( abs(MoveDelta) );
	}
	else
	{
		m_ScrollBar.LineDown( abs(MoveDelta) );
	}	

	return TRUE;
}

void _XTreeBox::SelectItem( int index )
{
	if( index < -1 ) return;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode, index);
	if( !findNode ) return;
	
	m_PrevSelectedItem = m_SelectedItem;

	m_SelectedItem = index;
}

void _XTreeBox::SetSelectItemChange( int flag )
{
	_XNodeData* findNode = NULL;		

	if( flag )	// next
	{
		findNode = m_RootItem->FindNodeByCommandId(findNode, m_SelectedCommandId+1);
		if( !findNode ) return;
		
		if( !findNode->Opened )
		{
			findNode->Opened = true;
			if( findNode->NodeIcon.GetIconArchive() )
				findNode->NodeIcon.SetAttrib( m_IconImageEx, 0, m_IconRectEx);												
		}	
	}
	else		// prev
	{
		findNode = m_RootItem->FindNodeByCommandId(findNode, m_SelectedCommandId-1);
		if( !findNode ) return;
		
		if( findNode->ParentNode )
		{
			findNode->ParentNode->Opened = true;
			if( findNode->ParentNode->NodeIcon.GetIconArchive() )
				findNode->ParentNode->NodeIcon.SetAttrib( m_IconImageEx, 0, m_IconRectEx);												
		}	
	}	

	m_PrevSelectedItem = m_SelectedItem;
	m_SelectedItem = findNode->Index;
	m_RootItem->Opened = true;
	m_SelectedCommandId = findNode->NodeId;
	
	UpdateTreeBox();
	PostMessage(gHWnd, WM_COMMAND, m_CommandID, findNode->NodeId-1 );	
	m_ScrollBar.SetLineScrollPos(m_SelectedItem);
}

void _XTreeBox::DeleteItem( int index )
{
	if( index < 0 ) return;

	_XNodeData* findNode = NULL;
	findNode = m_RootItem->FindNodeByIndex(findNode, index);

	if( findNode )
	{
		SAFE_DELETE( findNode );
	}
	
	UpdateTreeBox();
}

void _XTreeBox::UpdateTreeBox()
{
	m_IndexListCount = 0;
	m_IndexListCount = m_RootItem->GrantIndex( m_IndexListCount );
	m_ScrollBar.SetTotalLineCount( m_IndexListCount );
	m_ScrollBar.UpdateData();
}

int _XTreeBox::GetSelectedItemCommandID(int selected)
{
	if( m_RootItem )
	{
		_XNodeData* findNode = NULL;
		findNode = m_RootItem->FindNodeByIndex(findNode,  selected + m_CurrentScrollPos );

		if( findNode )
		{
			if( findNode->NodeString.GetCommandID() > 0 )
				return findNode->NodeString.GetCommandID();
		}
	}

	return 0;
}

void _XTreeBox::SetSelectedImageArchive(int index, _XTextureManager* pImageArchive)
{
	m_SelectedImageStatic.SetTexture(pImageArchive, index);
}

void _XTreeBox::SetSelectedImageHeight(int height)
{
	m_SelectedImageHeight = height;
}

void _XTreeBox::SetSelectedImagePosition(int X, int Y)
{
	m_SelectedImagePos.x = X;
	m_SelectedImagePos.y = Y;
}

void _XTreeBox::MoveScrollBarPos(int X, int Y)
{
	m_ScrollBarPositon.x = X;
	m_ScrollBarPositon.y = Y;	
	m_ScrollBar.MoveWindow( m_WindowPosition.x + m_ScrollBarPositon.x, m_WindowPosition.y + m_ScrollBarPositon.y );

}

void _XTreeBox::SetAlwaysShowScrollBarMode( BOOL balwaysshowscrollbarmode )
{
	m_bAlwaysShowScrollBarMode = balwaysshowscrollbarmode;
	m_ScrollBar.SetViewBorder(balwaysshowscrollbarmode);
}

void _XTreeBox::SettingIcon(int image1, int image2, RECT rt1, RECT rt2 )
{
	m_IconImage				= image1;
	m_IconImageEx			= image2;

	m_IconRect.left			= rt1.left;
	m_IconRect.top			= rt1.top;
	m_IconRect.right		= rt1.right;
	m_IconRect.bottom		= rt1.bottom;

	m_IconRectEx.left		= rt2.left;
	m_IconRectEx.top		= rt2.top;
	m_IconRectEx.right		= rt2.right;
	m_IconRectEx.bottom		= rt2.bottom;
}