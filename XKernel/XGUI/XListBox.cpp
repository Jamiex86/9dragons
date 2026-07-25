// XListBox.cpp: implementation of the _XListBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

extern	_XFontManager_Baseclass* g_XBaseFont;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XListBox::_XListBox()
{
	m_YPitch			=	12;
	m_TrackWidth		=	64;
	m_TrackHeight		=	12;
	m_SelectedItem		=	-1;
	m_PrevSelectedItem	=	-1;
	m_HoverItem			=	0;
	m_MaxViewLineCount	=	0;
	m_CurrentScrollPos	=	0;
	m_TextXOffset		=	0;
	m_nMouseOverItem	= -1;
	m_ListBoxStyle		=	_XLSTYLE_RIGHTSCROLLBAR;	
	m_ItemSelectable	=	TRUE;
	m_pScrbarFocusedObject	= NULL;
	
	int i = 0 ; for(  i = 0; i < _XDEF_MAXCOLUMN; i++ )
	{
		m_ColumnWidth[i]	 = 100;
		m_ColumnAlignType[i] = _XFONT_ALIGNTYPE_LEFT;	// 좌상 기본 정렬
		m_ColumnOffset[i].x  = 0;
		m_ColumnOffset[i].y  = 0;
		m_ColumnStyle[i]	 = _XLCOLUMNSTYLE_NONE;

		m_ColumnClipRect[i].left	=	0.0f;
		m_ColumnClipRect[i].top		=	0.0f;
		m_ColumnClipRect[i].right	=	gnWidth-1;
		m_ColumnClipRect[i].bottom	=	gnHeight-1;
	}
	m_ColumnStyle[0]	 = _XLCOLUMNSTYLE_STRING;	// 첫번째 서브아이템만 스트링 포맷 설정 
	
	m_SelectedImageHeight = 0;
	m_SelectedImagePos.x = 0;
	m_SelectedImagePos.y = 0;

	m_IconReportMode		=   FALSE;
	m_IconModeRowCount		=	1;
	m_IconModeColumnCount	=	1;
	m_IconModeRowSize		=	1;	
	m_IconModeColumnSize	=	1;

	m_ItemSelectCallBack	=	NULL;
	m_ItemMouseHoverCallBack=	NULL;	
	m_ItemSelectDrawCallBack =  NULL;
	m_ItemClickedDrawCallBack = NULL;

	m_bAlwaysShowScrollBarMode = FALSE;
	m_bMoveScrollBarMode	=	FALSE;
	m_ScrollBarPositon.x	=	0.0f;
	m_ScrollBarPositon.y	=	0.0f;

	m_FontID				=	_XFONTID_BASE;
	
	m_EnableMouseFocusing	=	TRUE;
	
	m_UnicodeDataMode		=	FALSE;
}

_XListBox::~_XListBox()
{
	m_ItemList.disposeList();
	m_ScrollBar.DestroyWindow();
	m_IconImageStatic.DestroyWindow();
	m_SelectStatic.ReleaseObject();
}

BOOL _XListBox::Create(  _XLISTBOX_STRUCTURE listboxstructure  )
{
	m_ItemList.disposeList();
	m_ScrollBar.DestroyWindow();

	// 리스트 박스 윈도우 생성
	_XWINDOW_STRUCTURE windowstruct;

	windowstruct.enable				=	listboxstructure.enable;
	windowstruct.position			=	listboxstructure.position;
	windowstruct.windowsize			=	listboxstructure.windowsize;
	windowstruct.commandid			=	listboxstructure.commandid;
	windowstruct.windowtextureindex =	listboxstructure.windowtextureindex;
	windowstruct.texturearchive		=	listboxstructure.texturearchive;
	windowstruct.facecolor			=	0x00000000;

	/*
	if( strlen(listboxstructure.WindowTitle) > 0 )
	{
		strcpy( windowstruct.WindowTitle, listboxstructure.WindowTitle );
	}
	else
	{
		memset( windowstruct.WindowTitle, 0, _XDEF_WINDOWTITLESTRINGSIZE );
	}
	*/

	_XWindow::Create( windowstruct );
	_XWindow::SetWindowMoveMode( _XWMOVE_FIXED );

	m_YPitch						=	listboxstructure.ypitch;
	m_MaxViewLineCount				=	listboxstructure.maxviewlinecount;
	m_ListBoxStyle					=	listboxstructure.listboxstyle;


	// 스크롤바 생성
	int scrollbarxpos = 0;

	if( m_ListBoxStyle == _XLSTYLE_LEFTSCROLLBAR ) // left align
	{
		scrollbarxpos = m_WindowPosition.x;
	}
	else if( m_ListBoxStyle == _XLSTYLE_RIGHTSCROLLBAR ) // right align
	{
		scrollbarxpos = m_WindowPosition.x + m_WindowSize.cx - listboxstructure.scrollbarsize.cx;
	}
	else if( m_ListBoxStyle == _XLSTYLE_NOSCROLLBAR ) // right align
	{
		scrollbarxpos = m_WindowPosition.x + m_WindowSize.cx - listboxstructure.scrollbarsize.cx;		
	}
	
	_XSCRBAR_STRUCTURE listscrbarstructure =
	{
		TRUE, 
		{scrollbarxpos,m_WindowPosition.y}, 
		listboxstructure.scrollbarsize.cx, 
		listboxstructure.scrollbarsize.cy,	// width, height
		listboxstructure.btnysize,			// btnysize
		listboxstructure.trackbarysize,		// trackbar size
		100, 
		m_MaxViewLineCount, 
		0,
		listboxstructure.imageindex_uparrow,
		listboxstructure.imageindex_uparrowclick,
		listboxstructure.imageindex_downarrow,
		listboxstructure.imageindex_downarrowclick,
		listboxstructure.imageindex_trackbar,
		listboxstructure.texturearchive			
	};

	m_ScrollBar.Create( listscrbarstructure );
	m_ScrollBar.ShowWindow( (m_ListBoxStyle == _XLSTYLE_NOSCROLLBAR) ? FALSE : TRUE );
	m_ScrollBar.SetViewBorder( TRUE );
	m_ScrollBar.SetTotalLineCount(0);
	m_ScrollBar.SetViewLineCount( m_MaxViewLineCount );	
	m_ScrollBar.UpdateData();	
	m_ScrollBar.RebuildTrackBar();

	m_IconImageStatic.Create( 0, 0, 32, 32, NULL, -1 );
	
	m_SelectedImageStatic.Create(0, 0, 32, 32, NULL, -1);

	m_TrackWidth = m_WindowSize.cx - listboxstructure.scrollbarsize.cx;
	m_TrackHeight = m_YPitch-1;

	m_SelectStatic.Create( 0, 0, m_TrackWidth, m_TrackHeight,
						   _XDEF_DEFAULTDEPTH_FORINTERFACE, D3DCOLOR_ARGB(16,128,128,128) );
	m_SelectedStatic.Create( 0, 0, m_TrackWidth, m_TrackHeight,
						   _XDEF_DEFAULTDEPTH_FORINTERFACE, D3DCOLOR_ARGB(64,128,128,128) );
	
	m_FontID				=	_XFONTID_BASE;
	
	return TRUE;
}

void _XListBox::DestroyWindow( void )
{
	m_ItemSelectCallBack = NULL;
	m_ItemMouseHoverCallBack = NULL;
}

void _XListBox::SetFont( _XFONTID fontid )
{ 
	m_FontID = fontid;
}

void _XListBox::MoveWindow( int X, int Y )
{	
	_XWindow::MoveWindow( X, Y );

	if( !m_bMoveScrollBarMode )
	{
		int scrollbarxpos = 0;
		if( m_ListBoxStyle == _XLSTYLE_LEFTSCROLLBAR ) // left align
		{
			scrollbarxpos = m_WindowPosition.x;
		}
		else // right align
		{
			scrollbarxpos = m_WindowPosition.x + m_WindowSize.cx - m_ScrollBar.GetWindowWidth();
		}

		m_ScrollBar.MoveWindow( scrollbarxpos, m_WindowPosition.y );
	}
	else
	{
		m_ScrollBar.MoveWindow( m_WindowPosition.x + m_ScrollBarPositon.x, m_WindowPosition.y + m_ScrollBarPositon.y );

	}

	int i = 0 ; for(  i = 0; i < _XDEF_MAXCOLUMN; i++ )
	{
		m_ColumnClipRect[i].left	=	m_WindowPosition.x + m_ColumnOffset[i].x;
		m_ColumnClipRect[i].right	=	m_ColumnClipRect[i].left + m_ColumnWidth[i];

		m_ColumnClipRect[i].top		=	m_WindowPosition.y;
		m_ColumnClipRect[i].bottom	=	m_WindowPosition.y + m_WindowSize.cy;
	}
}

int	_XListBox::CheckSelectedItem( void )
{
	if( !m_EnableMouseFocusing ) return -1;

	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
		
	if( m_IconReportMode )
	{
		if( scrnpos->x <= (m_WindowPosition.x + m_TextXOffset) ||
			scrnpos->x >= (m_WindowPosition.x + m_TextXOffset + m_IconModeColumnSize * m_IconModeColumnCount ) ||
			scrnpos->z <= m_WindowPosition.y ||
			scrnpos->z >= m_WindowPosition.y + m_WindowSize.cy ) return -1;

		int selectitempos = -1;
		int curitempos = -1;
		int ypos = m_WindowPosition.y + m_ColumnOffset[0].y;
		int iconmoderowpos = 0 ; for(  iconmoderowpos = 0; iconmoderowpos < m_IconModeRowCount; iconmoderowpos++ )
		{
			int iconmodecolumnpos = 0 ; for(  iconmodecolumnpos = 0; iconmodecolumnpos < m_IconModeColumnCount; iconmodecolumnpos++ )
			{	
				int xpos = m_WindowPosition.x + m_ColumnOffset[0].x + iconmodecolumnpos * m_IconModeColumnSize;				
								
				if( scrnpos->x > xpos && scrnpos->z > ypos && scrnpos->x < xpos + m_IconModeColumnSize && scrnpos->z < ypos + m_IconModeRowSize )
				{
					curitempos = (m_CurrentScrollPos * m_IconModeColumnCount) + (iconmoderowpos * m_IconModeColumnCount) + iconmodecolumnpos;

					if( curitempos < m_ItemList.getitemcount() ) 
					{
						selectitempos = curitempos;
					}
					
					break;
				}				
			}

			if( curitempos != -1 ) break;
						
			if( iconmoderowpos >= m_MaxViewLineCount ) break;
			ypos += m_IconModeRowSize;
		}
		
		if( selectitempos > -1 )
		{
			_XMString* curstring;
			curstring = (_XMString*)m_ItemList.getItem( selectitempos );
			if( curstring )
			{		
				if( curstring->GetCommandID() > 0 ) return selectitempos;
			}
		}
	}
	else
	{
		if( scrnpos->x <= (m_WindowPosition.x + m_TextXOffset) ||
			scrnpos->x >= (m_WindowPosition.x + m_TextXOffset + m_TrackWidth) ||
			scrnpos->z <= m_WindowPosition.y ||
			scrnpos->z >= m_WindowPosition.y + m_WindowSize.cy ) return -1;

		int itempos = ((scrnpos->z - m_WindowPosition.y) / m_YPitch) + m_CurrentScrollPos;

		_XMString* curstring;
		curstring = (_XMString*)m_ItemList.getItem( itempos );
		if( curstring )
		{		
			if( curstring->GetCommandID() > 0 ) return itempos;
		}
	}

	return -1;
}

void _XListBox::Draw( _XGUIObject*& pfocusobject )
{
	if( !this->m_Enable     ) return;

	_XWindow::Draw( pfocusobject );

	if( !this->m_ShowWindow ) return;

	BOOL itemhovered = FALSE;
	int hoverindex = -1;

	_XFontManager_Baseclass* pXFont = _XGetFont( m_FontID );
	
	if( !m_ItemList.listEmpty() )
	{
		m_ItemList.resetList();
				
		BOOL		usehighlight = FALSE;
				
		_XMString* curstring;
		int ypos = 0;		
		int ycoord = m_WindowPosition.y;
		ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
										
		
		if( m_IconReportMode )
		{				
			m_ItemList.resetList();
			
			int i = 0 ; for(  i = 0; i < (m_CurrentScrollPos * m_IconModeColumnCount); i++ ) 
			{
				m_ItemList.nextItem();
			}

			BOOL hovered = FALSE;

			int iconmoderowpos = 0 ; for(  iconmoderowpos = 0; iconmoderowpos < m_IconModeRowCount; iconmoderowpos++ )
			{
				int iconmodecolumnpos = 0 ; for(  iconmodecolumnpos = 0; iconmodecolumnpos < m_IconModeColumnCount; iconmodecolumnpos++ )
				{
					curstring = (_XMString*)m_ItemList.currentItem();

					int itembaseposx = m_WindowPosition.x + iconmodecolumnpos * m_IconModeColumnSize + m_TextXOffset; 
					int itembaseposy = ycoord; 

					int rxcoord = m_WindowPosition.x + m_ColumnOffset[0].x + iconmodecolumnpos * m_IconModeColumnSize;
					int rycoord = ycoord + m_ColumnOffset[0].y;

					if( m_ItemSelectable )
					{
						if( m_SelectedItem == ((m_CurrentScrollPos+iconmoderowpos) * m_IconModeColumnCount) + iconmodecolumnpos )
						{
							if( m_ItemClickedDrawCallBack )
							{
								m_ItemClickedDrawCallBack( rxcoord, rycoord, rxcoord+m_IconModeColumnSize-1, rycoord+m_IconModeRowSize-1 );
							}
							else
							{
								_XDrawRectAngle( rxcoord, rycoord, rxcoord+m_IconModeColumnSize-1, rycoord+m_IconModeRowSize-1, 1.0f, D3DCOLOR_ARGB( 128, 255, 255, 255 ) );
								if( CheckMousePosition() ) 
									gApp->ReadyCursor( g_SelectCursorIndex );
							}
					
						}					
						
						if( !pfocusobject || pfocusobject == (_XGUIObject*)this )
						{
							if( scrnpos->x > rxcoord && scrnpos->x < rxcoord + m_IconModeColumnSize &&
								scrnpos->z > rycoord && scrnpos->z < rycoord + m_IconModeRowSize )
							{	

								hovered = TRUE;

								int hoveringitempos = (m_CurrentScrollPos * m_IconModeColumnCount) + (iconmoderowpos * m_IconModeColumnCount) + iconmodecolumnpos;
								
								itemhovered = TRUE;

								_XMString* curstring;
								curstring = (_XMString*)m_ItemList.getItem( hoveringitempos );
								if( curstring )
								{
									if( curstring->GetCommandID() )
									{
										if( m_ItemSelectDrawCallBack )
										{
											m_ItemSelectDrawCallBack( rxcoord, rycoord, m_IconModeColumnSize, m_IconModeRowSize );
										}
										else
										{
											if( m_SelectedImageStatic.m_TextureIndex == -1 )
											{
												_XDrawRectAngle( rxcoord, rycoord, rxcoord+m_IconModeColumnSize-1, rycoord+m_IconModeRowSize-1, 1.0f, D3DCOLOR_ARGB( 64, 255, 255, 255 ) );
											}
											else
											{
												m_SelectedImageStatic.Draw( rxcoord, rycoord );
											}
										}
										
										if( CheckMousePosition() ) 
											gApp->ReadyCursor( g_SelectCursorIndex );

										usehighlight = TRUE;
									}
								}
								if( m_ItemMouseHoverCallBack )
								{
									if( m_ItemList.getitemcount() > hoveringitempos )
									{
										_XMString* curstring;
										curstring = (_XMString*)m_ItemList.getItem( hoveringitempos );
										if( curstring )
										{	
											m_ItemMouseHoverCallBack( m_CommandID, curstring->GetCommandID(), rxcoord, rycoord );
										}
									}
								}
							}
						}						
					}

					for( int col = 0; col < _XDEF_MAXCOLUMN; col++ )
					{
						switch( m_ColumnStyle[col] )
						{
						default:
						case _XLCOLUMNSTYLE_NONE   :
							break;
						case _XLCOLUMNSTYLE_STRING :
							{								
								D3DCOLOR color;
								if(usehighlight)
									color = curstring->GetAttribHighlight(col);
								else
									color = curstring->GetAttrib(col);

								if( !curstring->GetSeparateString() )
								{
									if( _XFONT_ALIGNTYPE_LEFT == m_ColumnAlignType[col] )
									{
										pXFont->SetClippingArea( m_ColumnClipRect[col] );
									}
									else if( _XFONT_ALIGNTYPE_CENTER == m_ColumnAlignType[col] )
									{
										int halflength = (pXFont->GetWidth( curstring->GetString( col ) ) >> 1);
										
										_XFRECT alignrect = m_ColumnClipRect[col];
										alignrect.left  -= halflength;
										alignrect.right -= halflength;
										
										pXFont->SetClippingArea( alignrect );
									}
									else if( _XFONT_ALIGNTYPE_RIGHT == m_ColumnAlignType[col] )
									{
										int length = pXFont->GetWidth( curstring->GetString( col ) );
										
										_XFRECT alignrect = m_ColumnClipRect[col];
										alignrect.left  = itembaseposx + m_ColumnOffset[col].x + m_TextXOffset - length;
										alignrect.right = itembaseposx + m_ColumnOffset[col].x + m_TextXOffset;
										
										pXFont->SetClippingArea( alignrect );
									}
									
									pXFont->SetColor(color);

									pXFont->SetUnicodeDataMode( m_UnicodeDataMode );
									pXFont->PutsAlign( itembaseposx + m_ColumnOffset[col].x + m_TextXOffset, 
										itembaseposy + m_ColumnOffset[col].y, m_ColumnAlignType[col], 
										curstring->GetString( col ) );
									pXFont->SetUnicodeDataMode( FALSE );
									
									pXFont->Flush();								
									pXFont->DisableClippingArea();
								}
								else
								{
									pXFont->SetColor(color);
									
									pXFont->SetUnicodeDataMode( m_UnicodeDataMode );
									pXFont->Puts_SeparateAlign(itembaseposx + m_ColumnOffset[col].x + m_TextXOffset, 
										itembaseposy + m_ColumnOffset[col].y - 6,
										curstring->GetString( col ), curstring->GetStringWidth(), 
										m_ColumnAlignType[col] ,1.0f, -1 );
									pXFont->SetUnicodeDataMode( FALSE );

									pXFont->Flush();					
									
								}
							}
							break;
						case _XLCOLUMNSTYLE_ICON   :
							{
								if(curstring->GetIconArchive())
								{						
									if( curstring->GetAttrib( col ) != (DWORD)-1 )
									{
										m_IconImageStatic.SetTexture( curstring->GetIconArchive(), curstring->GetAttrib( col ) );
										m_IconImageStatic.Draw( itembaseposx + m_ColumnOffset[col].x, 
																itembaseposy + m_ColumnOffset[col].y );
									}
								}

							}
							break;
						case _XLCOLUMNSTYLE_ICONRECT :
							{
								if(curstring->GetIconArchive())
								{
									if( curstring->GetAttrib( col ) != (DWORD)-1 )
									{
										m_IconImageStatic.SetTexture(curstring->GetIconArchive(), curstring->GetAttrib(col));
										m_IconImageStatic.DrawWithRegion( itembaseposx + m_ColumnOffset[col].x,
																		  itembaseposy + m_ColumnOffset[col].y, curstring->GetRect(col));
									}
								}
							}
							break;						
						}					
					}				
					
					usehighlight = FALSE;


					m_ItemList.nextItem();				
					
					if( m_ItemList.atHeadOfList() ) break;
				}

				if( m_ItemList.atHeadOfList() ) break;
				if( iconmoderowpos >= m_MaxViewLineCount ) break;
				ycoord += m_IconModeRowSize;
			}			

			if( !hovered )
			{
				if( m_ItemMouseHoverCallBack )
					m_ItemMouseHoverCallBack( m_CommandID, -1, 0, 0 );
			}
		}
		else
		{							
			int i = 0 ; for(  i = 0; i < m_CurrentScrollPos; i++ ) m_ItemList.nextItem();

			do
			{	
				curstring = (_XMString*)m_ItemList.currentItem();

				if( curstring )
				{						
					if( m_ItemSelectable )
					{
						if( m_SelectedItem == m_CurrentScrollPos+ypos )
						{	
							if( curstring->GetCommandID() )
							{
								if( m_ItemClickedDrawCallBack )
								{
									m_ItemClickedDrawCallBack( m_WindowPosition.x + m_TextXOffset, ycoord, m_TrackWidth, m_TrackHeight );
								}
								else
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
								}
								
								if( CheckMousePosition() ) 
									gApp->ReadyCursor( g_SelectCursorIndex );
								
								usehighlight = TRUE;

								if( !pfocusobject || pfocusobject == (_XGUIObject*)this && m_EnableMouseFocusing )
								{
									if( scrnpos->x > (m_WindowPosition.x + m_TextXOffset) && 
										scrnpos->x < (m_WindowPosition.x + m_TextXOffset + m_TrackWidth) &&
										scrnpos->z > ycoord && scrnpos->z < (ycoord+m_TrackHeight-1) )
									{
										hoverindex = ypos;
									}
								}
							}
						}
						else
						{
							if( !pfocusobject || pfocusobject == (_XGUIObject*)this && m_EnableMouseFocusing )
							{
								if( scrnpos->x > (m_WindowPosition.x + m_TextXOffset) && 
									scrnpos->x < (m_WindowPosition.x + m_TextXOffset + m_TrackWidth) &&
									scrnpos->z > ycoord && scrnpos->z < (ycoord+m_TrackHeight-1) )
								{	
									
									if( curstring->GetCommandID() )
									{
										m_HoverItem = curstring->GetCommandID();
										itemhovered = TRUE;
										hoverindex = ypos;

										if( m_ItemSelectDrawCallBack )
										{
											m_ItemSelectDrawCallBack( m_WindowPosition.x+m_TextXOffset, ycoord, m_TrackWidth, m_TrackHeight );
										}
										else
										{
											if( m_SelectedImageStatic.m_TextureIndex == -1 )
											{
												curstring->SetBoldString(TRUE);
												m_SelectStatic.MoveWindow( m_WindowPosition.x + m_TextXOffset, ycoord );
												m_SelectStatic.RenderSolid();
											}
											else
											{
												curstring->SetBoldString(TRUE);
												m_SelectedImageStatic.Draw(m_WindowPosition.x+m_SelectedImagePos.x, 
													m_WindowPosition.y+m_SelectedImagePos.y+(ypos*m_SelectedImageHeight));
												usehighlight = TRUE;
											}
										}
										
										if( CheckMousePosition() ) 
											gApp->ReadyCursor( g_SelectCursorIndex );
									}
								}
								else
								{
									curstring->SetBoldString(FALSE);									
								}
							}
						}
					}		

					for( int col = 0; col < _XDEF_MAXCOLUMN; col++ )
					{
						switch( m_ColumnStyle[col] )
						{
						default:
						case _XLCOLUMNSTYLE_NONE   :
							break;
						case _XLCOLUMNSTYLE_STRING :
							{								
								D3DCOLOR color;
								if(usehighlight)
									color = curstring->GetAttribHighlight(col);
								else
									color = curstring->GetAttrib(col);

								/*
								_XFRECT cliprect;
								cliprect.left   = m_WindowPosition.x;
								cliprect.top    = m_WindowPosition.y;
								cliprect.right  = m_WindowPosition.x + m_WindowSize.cx;
								cliprect.bottom = m_WindowPosition.y + m_WindowSize.cy;
								*/
								
								if( _XFONT_ALIGNTYPE_LEFT == m_ColumnAlignType[col] )
								{
									pXFont->SetClippingArea( m_ColumnClipRect[col] );
								}
								else if( _XFONT_ALIGNTYPE_CENTER == m_ColumnAlignType[col] )
								{
									int halflength = (pXFont->GetWidth( curstring->GetString( col ) ) >> 1);
									
									_XFRECT alignrect = m_ColumnClipRect[col];
									alignrect.left  -= halflength;
									alignrect.right -= halflength;
									
									pXFont->SetClippingArea( alignrect );
								}
								else if( _XFONT_ALIGNTYPE_RIGHT == m_ColumnAlignType[col] )
								{
									int length = pXFont->GetWidth( curstring->GetString( col ) );
									
									_XFRECT alignrect = m_ColumnClipRect[col];
									alignrect.left  = m_WindowPosition.x + m_ColumnOffset[col].x + m_TextXOffset - length;
									alignrect.right = m_WindowPosition.x + m_ColumnOffset[col].x + m_TextXOffset;
									
									pXFont->SetClippingArea( alignrect );
								}

								//pXFont->SetClippingArea( m_ColumnClipRect[col] );
								pXFont->SetColor(color);
								
								pXFont->SetUnicodeDataMode( m_UnicodeDataMode );
								
								pXFont->PutsAlign( m_WindowPosition.x + m_ColumnOffset[col].x + m_TextXOffset, 
												  ycoord + m_ColumnOffset[col].y, 
												  m_ColumnAlignType[col], curstring->GetString( col ) );

								if( curstring->GetBoldString() )//한번 더 그려서 진하게 만든다.
								{
									pXFont->PutsAlign( m_WindowPosition.x + m_ColumnOffset[col].x + m_TextXOffset + 1, 
													   ycoord + m_ColumnOffset[col].y, 
													   m_ColumnAlignType[col], curstring->GetString( col ) );
								}
								
								pXFont->SetUnicodeDataMode( FALSE );

								pXFont->Flush();								
								pXFont->DisableClippingArea();
							}
							break;
						case _XLCOLUMNSTYLE_ICON   :
							{
								if(curstring->GetIconArchive())
								{						
									if( curstring->GetAttrib( col ) != (DWORD)-1 )
									{
										m_IconImageStatic.SetTexture( curstring->GetIconArchive(), curstring->GetAttrib( col ) );
										m_IconImageStatic.Draw( m_WindowPosition.x + m_ColumnOffset[col].x + m_TextXOffset, 
																ycoord + m_ColumnOffset[col].y );
									}
								}

							}
							break;
						case _XLCOLUMNSTYLE_ICONRECT :
							{
								if(curstring->GetIconArchive())
								{
									if( curstring->GetAttrib( col ) != (DWORD)-1 )
									{
										m_IconImageStatic.SetTexture(curstring->GetIconArchive(), curstring->GetAttrib(col));
										m_IconImageStatic.DrawWithRegion(m_WindowPosition.x + m_ColumnOffset[col].x + m_TextXOffset, 
																		ycoord + m_ColumnOffset[col].y, curstring->GetRect(col));
									}
								}
							}
							break;						
						}					
					}				
					
					usehighlight = FALSE;

					ypos++;
					ycoord += m_YPitch;
					if( ypos >= m_MaxViewLineCount ) break;					
				}

				m_ItemList.nextItem();

			}while( !m_ItemList.atHeadOfList() );
		}
	}	

	if( !itemhovered )
	{
		m_HoverItem = 0;
	}

	m_CurrentScrollPos = m_ScrollBar.GetLineScrollPos();
	m_ScrollBar.Draw( m_pScrbarFocusedObject );	

	if( m_ItemMouseHoverCallBack )
	{
		if( hoverindex != -1 )
		{
			m_ItemMouseHoverCallBack( m_CommandID, m_CurrentScrollPos+hoverindex, m_WindowPosition.x+m_TextXOffset, m_WindowPosition.y + (m_YPitch*hoverindex) );
		}
		else
		{
			m_ItemMouseHoverCallBack( m_CommandID, -1, 0, 0 );
		}
	}
}

BOOL _XListBox::Process( _XGUIObject*& pfocusobject )
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
				m_nMouseOverItem = selecteditemcommand; //Author : 양희왕 //breif : 미니툴팁 때문에
				
				if( selecteditemcommand != -1 )
				{	
					if( pfocusobject == (_XGUIObject*)this )
					{						
						if( m_CommandID )
						{	
							_XMString* curstring;
							curstring = (_XMString*)m_ItemList.getItem( selecteditemcommand );
							if( curstring )
							{									
								if( m_ItemSelectCallBack )
								{
									if( m_ItemSelectCallBack( m_CommandID, curstring->GetCommandID() ) )
									{
										if( curstring->GetCommandID() > 0 )
										{
											m_PrevSelectedItem = m_SelectedItem;
											m_SelectedItem = selecteditemcommand;
											
											PostMessage(gHWnd, WM_COMMAND, m_CommandID, curstring->GetCommandID() );
										}
									}
								}
								else
								{
									if( curstring->GetCommandID() > 0 )
									{
										m_PrevSelectedItem = m_SelectedItem;
										m_SelectedItem = selecteditemcommand;												
										
										PostMessage(gHWnd, WM_COMMAND, m_CommandID, curstring->GetCommandID() );
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

void _XListBox::SetMaxViewLineCount( int viewlinecount )
{ 
	m_MaxViewLineCount = viewlinecount;
	m_ScrollBar.SetViewLineCount( viewlinecount );
}

BOOL _XListBox::InsertListItem( LPTSTR itemstring, DWORD attrib, int commandid, _XTextureManager* pIconArchive )
{		
	_XMString* pString = NULL;
	
	if( !itemstring || strlen(itemstring) <= 0 )
	{
		if( m_UnicodeDataMode )
		{
			pString = new _XMString( L" ", attrib, commandid );	
		}
		else
		{
			pString = new _XMString( _T(" "), attrib, commandid );	
		}
		pString->SetBoldString(FALSE);
	}
	else 
	{
		if( m_UnicodeDataMode )
		{
			pString = new _XMString( (wchar_t*)itemstring, attrib, commandid );	
		}
		else
		{
			pString = new _XMString( itemstring, attrib, commandid );	
		}
		pString->SetBoldString(FALSE);	
	}

	if( pString )
	{
		if( pIconArchive ) pString->SetIconArchive( pIconArchive );
		m_ItemList.resetList();
		m_ItemList.insertItem( pString );
		

		if( m_IconReportMode )
		{
			m_ScrollBar.SetTotalLineCount( (m_ItemList.getitemcount() + (m_IconModeColumnCount - 1)) / m_IconModeColumnCount );
		}
		else
		{
			m_ScrollBar.SetTotalLineCount( m_ItemList.getitemcount() );
		}

		m_ScrollBar.UpdateData();

		if( !m_bAlwaysShowScrollBarMode ) // 스크롤바가 항상 보이는 모드가 아닐때는
		{
			// 리스트 박스에 보이는 최대 라인수보다 아이템이 작으면 스크롤바는 보이지 않는다.
			if( m_MaxViewLineCount >= m_ItemList.getitemcount() )
			{
				m_ScrollBar.ShowWindow(FALSE);
			}
			else
			{
				m_ScrollBar.ShowWindow(TRUE);
			}
		}
	}
	else return FALSE;

	return TRUE;	
}

void _XListBox::SetIconArchive( int index, _XTextureManager* pIconArchive )
{
	if( m_ItemList.listEmpty() ) return;
	
	_XMString* curstring = NULL;	
	curstring = (_XMString*)m_ItemList.getItem( index );
	if( curstring )
	{
		curstring->SetIconArchive( pIconArchive );
	}	
}

_XTextureManager* _XListBox::GetIconArchive( int index )
{
	if( m_ItemList.listEmpty() ) return NULL;
	
	_XMString* curstring = NULL;	
	curstring = (_XMString*)m_ItemList.getItem( index );
	if( curstring )
	{
		return curstring->GetIconArchive();
	}		
	return NULL;
}


BOOL _XListBox::SetItemText( int index, int column, LPTSTR itemstring , BOOL boldstring )
{
	if( m_ItemList.listEmpty() ) return FALSE;
	
	_XMString* curstring = NULL;	
	curstring = (_XMString*)m_ItemList.getItem( index );
	if( curstring )
	{
		curstring->SetString( itemstring, column );	
		curstring->SetBoldString( boldstring );
	}
	else return FALSE;
	
	return TRUE;
}

BOOL _XListBox::SetSeparateText( int index, int column, LPTSTR itemstring, int width, BOOL boldstring )
{
	if( m_ItemList.listEmpty() ) return FALSE;
	
	_XMString* curstring = NULL;	
	curstring = (_XMString*)m_ItemList.getItem( index );
	if( curstring )
	{
		curstring->SetString( itemstring, column );	
		curstring->SetSeparateString(TRUE);
		curstring->SetStringWidth(width);
		curstring->SetBoldString( boldstring );
	}
	else return FALSE;
	
	return TRUE;
}


LPTSTR _XListBox::GetItemText( int index, int column )
{
	if( m_ItemList.listEmpty() ) return NULL;

	_XMString* curstring = NULL;	
	curstring = (_XMString*)m_ItemList.getItem( index );
	if( !curstring ) return NULL;
	
	return curstring->GetString( column );
}

BOOL _XListBox::SetItemAttrib( int index, int column, DWORD attrib )
{
	if( m_ItemList.listEmpty() ) return FALSE;

	_XMString* curstring = NULL;	
	curstring = (_XMString*)m_ItemList.getItem( index );
	
	if( !curstring ) return FALSE;
	
	curstring->SetAttrib( attrib, column );	
	return TRUE;
}

BOOL _XListBox::SetItemAttrib(int index, int column, DWORD attrib, RECT rect)
{
	if(m_ItemList.listEmpty())
		return FALSE;

	_XMString* curstring = NULL;
	curstring = (_XMString *)m_ItemList.getItem(index);
	if(!curstring)
		return FALSE;

	curstring->SetAttrib(attrib, column, rect);
	return TRUE;
}

BOOL _XListBox::SetItemAttribHighlight(int index, int column, DWORD attrib)
{
	if(m_ItemList.listEmpty())
		return FALSE;

	_XMString* curstring = NULL;
	curstring = (_XMString *)m_ItemList.getItem(index);

	if(!curstring)
		return FALSE;

	curstring->SetAttribHighlight(attrib, column);
	return TRUE;
}

DWORD _XListBox::GetItemAttrib( int index, int column )
{
	if( m_ItemList.listEmpty() ) return 0;

	_XMString* curstring = NULL;	
	curstring = (_XMString*)m_ItemList.getItem( index );
	if( !curstring ) return 0;
	
	return curstring->GetAttrib( column );
}

BOOL _XListBox::SetItemCommandID( int index, DWORD commandid )
{
	if( m_ItemList.listEmpty() ) return FALSE;

	_XMString* curstring = NULL;	
	curstring = (_XMString*)m_ItemList.getItem( index );
	if( !curstring ) return FALSE;

	curstring->SetCommandID( commandid );
	
	return TRUE;
}

DWORD _XListBox::GetItemCommandID( int index )
{
	if( m_ItemList.listEmpty() ) return 0;

	_XMString* curstring = NULL;	
	curstring = (_XMString*)m_ItemList.getItem( index );
	if( !curstring ) return 0;
	
	return curstring->GetCommandID();
}


BOOL _XListBox::ProcessWheelMessage( short zDelta )
{	
	if( m_ScrollBar.ProcessWheelMessage( zDelta ) )
	{
		m_CurrentScrollPos = m_ScrollBar.GetLineScrollPos();
		return TRUE;
	}

	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();

	if( m_IconReportMode )
	{
		if( scrnpos->x <= (m_WindowPosition.x + m_TextXOffset) ||
			scrnpos->x >= (m_WindowPosition.x + m_TextXOffset + m_IconModeColumnSize * m_IconModeColumnCount ) ||
			scrnpos->z <= m_WindowPosition.y ||
			scrnpos->z >= m_WindowPosition.y + m_WindowSize.cy ) return FALSE;
	}
	else
	{	
		if( scrnpos->x <= (m_WindowPosition.x + m_TextXOffset) ||
			scrnpos->x >= (m_WindowPosition.x + m_TextXOffset + m_TrackWidth) ||
			scrnpos->z <= m_WindowPosition.y ||
			scrnpos->z >= m_WindowPosition.y + m_WindowSize.cy ) return FALSE;
	}
	
		
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

void _XListBox::SelectItem( int index )
{
	if( index < -1 || index >= m_ItemList.getitemcount() ) return;

	m_PrevSelectedItem = m_SelectedItem;

	m_SelectedItem = index;
}

void _XListBox::DeleteItem( int index )
{
	if( index < 0 || index >= m_ItemList.getitemcount() ) return;
	
	if( m_ItemList.deleteItem( index ) )
	{
		if( m_IconReportMode )
		{
			m_ScrollBar.SetTotalLineCount( (m_ItemList.getitemcount() + (m_IconModeColumnCount - 1)) / m_IconModeColumnCount );
		}
		else
		{
			m_ScrollBar.SetTotalLineCount( m_ItemList.getitemcount() );
		}
		
		m_ScrollBar.UpdateData();

		m_PrevSelectedItem	= -1;

		if( m_ItemList.listEmpty() )
		{
			m_SelectedItem = -1;
		}
		else
		{
			if( m_SelectedItem > 0 )
			{
				if( m_SelectedItem >= m_ItemList.getitemcount() ) m_SelectedItem = m_ItemList.getitemcount() - 1;
			}
		}			
	}
}

void _XListBox::DeleteAllItem( void )
{
	m_ItemList.disposeList();

	m_SelectedItem			=	-1;
	m_PrevSelectedItem		=	-1;
	m_nMouseOverItem		=	-1;
	m_CurrentScrollPos		=	 0;	
	m_pScrbarFocusedObject	= NULL;
	
	if( m_IconReportMode )
	{
		m_ScrollBar.SetTotalLineCount( (m_ItemList.getitemcount() + (m_IconModeColumnCount - 1)) / m_IconModeColumnCount );
	}
	else
	{
		m_ScrollBar.SetTotalLineCount( m_ItemList.getitemcount() );
	}

	m_ScrollBar.UpdateData();
}

int _XListBox::GetSelectedItemCommandID(int selected)
{	
	if(!m_ItemList.listEmpty())
	{
		if( m_IconReportMode )
		{				
			_XMString* curstring = (_XMString *)m_ItemList.getItem( (m_CurrentScrollPos * m_IconModeColumnCount) + selected );
			if(curstring)
			{
				if(curstring->GetCommandID() > 0)
				   return curstring->GetCommandID();
			}
		}
		else
		{	
			_XMString* curstring = (_XMString *)m_ItemList.getItem( selected + m_CurrentScrollPos );
				
			if(curstring)
				if(curstring->GetCommandID() > 0) return curstring->GetCommandID();
		}
	}

	return 0;
}

void _XListBox::SetSelectedImageArchive(int index, _XTextureManager* pImageArchive)
{
	m_SelectedImageStatic.SetTexture(pImageArchive, index);
}

void _XListBox::SetSelectedImageHeight(int height)
{
	m_SelectedImageHeight = height;
}

void _XListBox::SetSelectedImagePosition(int X, int Y)
{
	m_SelectedImagePos.x = X;
	m_SelectedImagePos.y = Y;
}

BOOL _XListBox::IsValidItem(int index)
{
	_XMString* curstring = NULL;
	int itempos = -1;

	if( m_IconReportMode )
	{	
		itempos = (m_CurrentScrollPos * m_IconModeColumnCount) + index;		
	}
	else
	{
		itempos = index + m_CurrentScrollPos;			
	}

	curstring = (_XMString*)m_ItemList.getItem( itempos );

	if( curstring )
		return TRUE;
	else
		return FALSE;
}

void _XListBox::MoveScrollBarPos(int X, int Y)
{
	m_ScrollBarPositon.x = X;
	m_ScrollBarPositon.y = Y;	
	m_ScrollBar.MoveWindow( m_WindowPosition.x + m_ScrollBarPositon.x, m_WindowPosition.y + m_ScrollBarPositon.y );

}

void _XListBox::SetAlwaysShowScrollBarMode( BOOL balwaysshowscrollbarmode )
{
	m_bAlwaysShowScrollBarMode = balwaysshowscrollbarmode;
	m_ScrollBar.SetViewBorder(balwaysshowscrollbarmode);
}

POINT _XListBox::GetColumnOffsetWindowPos( int count, int offsetnumber )
{
	int ypos = m_WindowPosition.y + m_ColumnOffset[0].y;
	int xpos = -1;
	POINT tempPos;

	if( m_IconReportMode )
	{
		int iconmoderowpos = 0 ; for(  iconmoderowpos = 0; iconmoderowpos < m_IconModeRowCount; iconmoderowpos++ )
		{
			int iconmodecolumnpos = 0 ; for(  iconmodecolumnpos = 0; iconmodecolumnpos < m_IconModeColumnCount; iconmodecolumnpos++ )
			{
				if( count == (m_CurrentScrollPos * m_IconModeColumnCount) + (iconmoderowpos * m_IconModeColumnCount) + iconmodecolumnpos )
				{
					xpos = m_WindowPosition.x + m_ColumnOffset[0].x + iconmodecolumnpos * m_IconModeColumnSize;				
					break;
				}
			}
			
			if( xpos != -1 ) break;
			if( iconmoderowpos >= m_MaxViewLineCount ) break;
			ypos += m_IconModeRowSize;
		}
	}

	tempPos.x = xpos + m_ColumnOffset[offsetnumber].x;
	tempPos.y = ypos + m_ColumnOffset[offsetnumber].y;
	
	return tempPos;
}