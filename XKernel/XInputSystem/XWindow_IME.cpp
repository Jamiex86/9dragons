// XWindow_IME.cpp: implementation of the _XWindow_IME class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"
#include "XListBox.h"

#define _XDEF_IMECADIDATELISTBOX				1250

extern	_XTextureManager	g_MainInterfaceTextureArchive;
extern	int	_X_WtoM_ConvertString(UINT codePage, const wchar_t* wText, int wLen, char* text, int len);
extern	TCHAR g_MBCSConvertBuffer[MAX_IMEBUFFERLENGTH];

extern _XIMM g_imm;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL __stdcall _XIMECandidateListSelectedCallBack( int param1, int param2 )
{
	HIMC hImc = NULL;
	hImc = ImmGetContext(gHWnd);

	if(hImc) 
	{
		DWORD candidatelistindex = (param2 & 0xFFFF0000) >> 16;
		DWORD candidateindex = (param2 & 0xFFFF);
		ImmNotifyIME( hImc, NI_SELECTCANDIDATESTR, 0, candidatelistindex + candidateindex-1 ); 

		ImmReleaseContext(gHWnd, hImc);
	}	
	
	return TRUE;	
}

_XWindow_IME::_XWindow_IME()
{
	m_pListBox = NULL;
}

_XWindow_IME::~_XWindow_IME()
{

}

BOOL _XWindow_IME::Initialize(void)
{
	int scrollresourceindex = g_MainInterfaceTextureArchive.FindResource("MI_chat_quick.tga");
	_XLISTBOX_STRUCTURE listboxstructure = 
	{
		TRUE,								// 윈도우 활성화
		{ 6, 24 },							// 윈도우 좌표
		{ 116, 140 },						// 윈도우 사이즈
		_XDEF_IMECADIDATELISTBOX,			// 윈도우 아이디
		-1,									// 윈도우 border 이미지 인덱스
		&g_MainInterfaceTextureArchive,		// Texture 리스트
		_T(" "),							// 윈도우 타이틀
		14,									// 리스트 아이템 라인 피치
		10,									// 리스트 박스에 보이는 최대 라인수
		_XLSTYLE_RIGHTSCROLLBAR,			// 스크롤바의 위치

		// Scroll bar property
		{12, 140},
		12,
		22,
		150,
		scrollresourceindex,
		scrollresourceindex,
		scrollresourceindex,
		scrollresourceindex,
		scrollresourceindex
	};

	m_pListBox = new _XListBox;
	m_pListBox->Create(listboxstructure);

	if( g_LanguageType == _XLANGUAGE_TYPE_TAIWANESE || g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
	{
		m_pListBox->SetUnicodeDataMode( TRUE );
	}

	POINT coloffset = { 2, 2 };
	m_pListBox->SetColumnOffset(0, coloffset);
	m_pListBox->SetColumnStyle(0, _XLCOLUMNSTYLE_STRING);
	m_pListBox->SetAlignType(0, _XFONT_ALIGNTYPE_LEFT);
	m_pListBox->SetColumnWidth(0, 110 );
	m_pListBox->SetTrackHeight(14);
	m_pListBox->SetTrackBarColor( D3DCOLOR_ARGB(64,128,128,128) );
	m_pListBox->SetSelectedTrackBarColor( D3DCOLOR_ARGB(128,128,150,150) );
	//m_pListBox->EnableSelectItem(FALSE);
	//m_pListBox->EnableMouseFocusing( FALSE );
	m_pListBox->SetAlwaysShowScrollBarMode( FALSE );
	
	if( g_LanguageType == _XLANGUAGE_TYPE_TAIWANESE || g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
	{
		m_pListBox->EnableSelectItem(TRUE);
		
		if( g_imm._ImmDisableTextFrameService )
		{
			m_pListBox->SetSelectItemCallBack( _XIMECandidateListSelectedCallBack );		
		}
	}
	else
	{
		m_pListBox->EnableSelectItem(FALSE);
		m_pListBox->SetSelectItemCallBack( FALSE );
	}

	_XVScrollBar* pScrollBar = m_pListBox->GetScrollBarObject();
	if( pScrollBar )
	{
		_XSCRBAR_IMAGECLIPRECTSTRUCTURE cliprectstructure = 
		{
			{ 0,  0, 12, 12 },
			{ 24, 0, 36, 12 },
			{ 0,  12, 12, 24 },
			{ 24, 12, 36, 24 },
			{ 40,157, 52, 179}
		};
		pScrollBar->SetClipRect( cliprectstructure );	
	}

	InsertChildObject(m_pListBox);	
	
	return TRUE;
}

void _XWindow_IME::DestroyWindow(void)
{
	_XWindow::DestroyWindow();
}

void _XWindow_IME::Draw(_XGUIObject*& pfocusobject)
{		
	_XWindow::Draw(pfocusobject);
	
	_XIMEContainer* pIMEContainer = _XIMEKERNEL.GetIMEContainer();

	if(this->m_ShowWindow)
	{
		DWORD blackcolor = D3DCOLOR_ARGB(64, 215, 235, 188);//D3DCOLOR_ARGB(255,64,64,64);
		_XDrawRectAngle( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, 
						 m_WindowPosition.y+m_WindowSize.cy-1, 0.0f, blackcolor );
		_XDrawRectAngle( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, 
			m_WindowPosition.y+m_WindowSize.cy-1, 0.0f, D3DCOLOR_ARGB(255,32,32,32) );
		_XDrawRectAngle( m_WindowPosition.x + 2, m_WindowPosition.y + 2, 
						 m_WindowPosition.x+m_WindowSize.cx-3, m_WindowPosition.y+m_WindowSize.cy-3, 0.0f, blackcolor );

		_XDrawLine2D( m_WindowPosition.x + 3, m_WindowPosition.y + 19, 
					  m_WindowPosition.x  + m_WindowSize.cx - 3, m_WindowPosition.y + 19, blackcolor,blackcolor );
	
		if(pIMEContainer)
		{
			g_XBaseFont->SetClippingArea( m_WindowPosition.x + 3, m_WindowPosition.y + 19, 
										  m_WindowPosition.x + m_WindowSize.cx - 3, m_WindowPosition.y + 19 );

			if( m_pListBox->GetUnicodeDataMode() )
			{
				g_XBaseFont->SetColor(_XSC_DEFAULT);
				g_XBaseFont->SetUnicodeDataMode(TRUE);
				g_XBaseFont->Puts( m_WindowPosition.x + 8, m_WindowPosition.y + 6, (LPCTSTR)pIMEContainer->GetComText() );
				g_XBaseFont->SetUnicodeDataMode(FALSE);
				g_XBaseFont->Flush();
			}
			else
			{
				_X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, pIMEContainer->GetComText(), wcslen(pIMEContainer->GetComText()), g_MBCSConvertBuffer, MAX_IMEBUFFERLENGTH);
				g_XBaseFont->SetColor(_XSC_DEFAULT);
				g_XBaseFont->Puts( m_WindowPosition.x + 8, m_WindowPosition.y + 6, g_MBCSConvertBuffer );
				g_XBaseFont->Flush();
			}

			g_XBaseFont->DisableClippingArea();
		}
	}

	if( g_LanguageType == _XLANGUAGE_TYPE_TAIWANESE || g_LanguageType == _XLANGUAGE_TYPE_CHINESE|| g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
	{
		if(pIMEContainer)
		{
			int _width = g_XBaseFont->GetWidth( _XIMEKERNEL.GetIndicator() );

			if(_XIMEKERNEL.GetImeState() == 1) 
			{
				_XDrawSolidBar( gnWidth-2-_width, gnHeight-2-g_XBaseFont->GetHeight(), gnWidth-1, gnHeight-1, 0xFF7F7FFF );
				_XDrawRectAngle( gnWidth-2-_width, gnHeight-2-g_XBaseFont->GetHeight(), gnWidth-1, gnHeight-1, 0.0f, D3DCOLOR_ARGB(220, 215, 235, 188) );
				g_XBaseFont->SetColor(_XSC_DEFAULT);		
				g_XBaseFont->SetUnicodeDataMode( TRUE );
				g_XBaseFont->PutsAlign( gnWidth-1, gnHeight-g_XBaseFont->GetHeight()-1, _XFONT_ALIGNTYPE_RIGHT, (char*)_XIMEKERNEL.GetIndicator() );			
				g_XBaseFont->Flush();
				g_XBaseFont->SetUnicodeDataMode( FALSE );
			}
			else
			{
				//_XDrawSolidBar( gnWidth-2-_width, gnHeight-2-g_XBaseFont->GetHeight(), gnWidth-1, gnHeight-1, 0xFF7F7FFF );
				_XDrawRectAngle( gnWidth-2-_width, gnHeight-2-g_XBaseFont->GetHeight(), gnWidth-1, gnHeight-1, 0.0f, D3DCOLOR_ARGB(220, 215, 235, 188) );
				g_XBaseFont->SetColor(_XSC_DEFAULT);
				g_XBaseFont->SetUnicodeDataMode( TRUE );
				g_XBaseFont->PutsAlign( gnWidth-1, gnHeight-g_XBaseFont->GetHeight()-1, _XFONT_ALIGNTYPE_RIGHT, (char*)_XIMEKERNEL.GetIndicator() );
				g_XBaseFont->Flush();
				g_XBaseFont->SetUnicodeDataMode( FALSE );
			}

			//if( g_LanguageType == _XLANGUAGE_TYPE_TAIWANESE || g_LanguageType == _XLANGUAGE_TYPE_CHINESE || g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
			{
				int descriptionwidth = g_XBaseFont->GetWidth( _XIMEKERNEL.m_IMEDescription );

				if( (descriptionwidth > 0) && gpInput->CheckMousePos( gnWidth-2-_width, gnHeight-2-g_XBaseFont->GetHeight(), gnWidth-1, gnHeight-1 ) )
				{
					_XDrawSolidBar( gnWidth-3-descriptionwidth - _width - 4, gnHeight-g_XBaseFont->GetHeight()-2, gnWidth-1- _width - 4, gnHeight-1, 0xFF7F7FFF );
					_XDrawRectAngle( gnWidth-3-descriptionwidth- _width - 4, gnHeight-g_XBaseFont->GetHeight()-2, gnWidth-1- _width - 4, gnHeight-1, 0.0f, D3DCOLOR_ARGB(170, 215, 235, 188) );

					g_XBaseFont->SetColor(_XSC_DEFAULT_HIGHLIGHT);
					g_XBaseFont->SetUnicodeDataMode( TRUE );
					g_XBaseFont->PutsAlign( gnWidth-1- _width - 4, gnHeight-g_XBaseFont->GetHeight()-1, _XFONT_ALIGNTYPE_RIGHT, (char*)_XIMEKERNEL.m_IMEDescription );
					g_XBaseFont->Flush();
					g_XBaseFont->SetUnicodeDataMode( FALSE );
				}
			}
		}
	}
}

BOOL _XWindow_IME::Process(_XGUIObject*& pfocusobject)
{
	if(!this->m_Enable)
		return FALSE;

	if(!this->m_ShowWindow)
		return FALSE;
		
	_XWindow::Process(pfocusobject);
	
	return FALSE;
}

BOOL _XWindow_IME::CheckArea(void)
{
	if( !m_pMyManager ) return FALSE;

	if(CheckMousePosition())
	{
		return TRUE;
	}
	
	return FALSE;	
}

void _XWindow_IME::ShowWindow(BOOL show)
{	
	if( show ) CheckOutside();
	_XWindow::ShowWindow(show);
}
