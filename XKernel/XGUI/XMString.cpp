// XMString.cpp: implementation of the _XMString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"
#include "XMString.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XMString::_XMString()
{
	int i = 0 ; for(  i = 0; i < _XDEF_MULTISTRINGARRAYCOUNT; i++ )
	{
		m_stringarray[ i ]	= NULL;
		m_attribute[ i ]	= 0;
	}	
	m_CommandID = 0;
	m_pIconImageArchive = NULL;
	m_bBoldString		= FALSE;
	m_bSepaateString	= FALSE;
	m_StringWidth		= 0;
}

_XMString::_XMString( LPTSTR str, DWORD attrib, int commandid )
{
	int i = 0 ; for(  i = 0; i < _XDEF_MULTISTRINGARRAYCOUNT; i++ )
	{
		m_stringarray[ i ]	= NULL;
		m_attribute[ i ]	= 0;
	}
	
	m_CommandID = commandid;
	m_pIconImageArchive = NULL;
	m_bBoldString		= FALSE;
	m_bSepaateString	= FALSE;
	m_StringWidth		= 0;
	SetString( str );
	SetAttrib( attrib );
	
}

_XMString::_XMString( wchar_t* str, DWORD attrib, int commandid )
{
	int i = 0 ; for(  i = 0; i < _XDEF_MULTISTRINGARRAYCOUNT; i++ )
	{
		m_stringarray[ i ]	= NULL;
		m_attribute[ i ]	= 0;
	}
	
	m_CommandID = commandid;
	m_pIconImageArchive = NULL;
	m_bBoldString		= FALSE;
	m_bSepaateString	= FALSE;
	m_StringWidth		= 0;
	SetString( str );
	SetAttrib( attrib );
	
}

_XMString::~_XMString()
{
	int i = 0 ; for(  i = 0; i < _XDEF_MULTISTRINGARRAYCOUNT; i++ )
	{
		SAFE_DELETE_ARRAY( m_stringarray[ i ] );		
	}
}

void _XMString::Reset( void )
{
	int i = 0 ; for(  i = 0; i < _XDEF_MULTISTRINGARRAYCOUNT; i++ )
	{
		SAFE_DELETE_ARRAY( m_stringarray[ i ] );
		m_attribute[ i ] = 0;
	}
	m_bBoldString		= FALSE;
}

void _XMString::ResetString( int index )
{
	if( index >= _XDEF_MULTISTRINGARRAYCOUNT ) return;

	SAFE_DELETE_ARRAY( m_stringarray[ index ] );
	m_attribute[ index ] = 0;
	m_bBoldString = FALSE;
}

BOOL _XMString::SetString( LPTSTR str, int index )
{
	if( !str ) return FALSE;

	if( index >= _XDEF_MULTISTRINGARRAYCOUNT ) return FALSE;

	SAFE_DELETE_ARRAY( m_stringarray[ index ] );

	int strlength = strlen( str );
	if( strlength+1 > 0 )
		m_stringarray[ index ] = new TCHAR[strlength+1];  

	if( !m_stringarray[ index ] )
	{
		return FALSE;
	}

	memset( m_stringarray[index], 0, strlength+1 );
	strncpy( m_stringarray[index], str, strlength );
	m_bBoldString = FALSE;

	return TRUE;	
}

BOOL _XMString::SetString( wchar_t* str, int index )
{
	if( index >= _XDEF_MULTISTRINGARRAYCOUNT ) return FALSE;

	SAFE_DELETE_ARRAY( m_stringarray[ index ] );

	int strlength = wcslen( str );
	
	if( strlength+1 > 0 )
		m_stringarray[ index ] = new TCHAR[ sizeof(wchar_t) * (strlength+1) ];

	if( !m_stringarray[ index ] )
	{
		return FALSE;
	}

	memset( m_stringarray[index], 0, sizeof(wchar_t) * (strlength+1) );
	memcpy( m_stringarray[index], str, sizeof(wchar_t) * strlength );
	m_bBoldString = FALSE;

	return TRUE;
}

LPTSTR _XMString::GetString( int index )
{
	if( index >= _XDEF_MULTISTRINGARRAYCOUNT ) return NULL;
	return m_stringarray[index];
}

BOOL _XMString::SetAttrib( DWORD attrib, int index )
{
	if( index >= _XDEF_MULTISTRINGARRAYCOUNT ) return FALSE;
	m_attribute[index] = attrib;
	return TRUE;
}

DWORD _XMString::GetAttrib( int index )
{
	if( index >= _XDEF_MULTISTRINGARRAYCOUNT ) return 0;
	return m_attribute[index];
}

BOOL _XMString::SetAttrib(DWORD attrib, int index, RECT rect)
{
	if(index >= _XDEF_MULTISTRINGARRAYCOUNT)
		return FALSE;

	m_attribute[index] = attrib;
	m_drawrect[index] = rect;
	return TRUE;
}

BOOL _XMString::SetAttribHighlight(DWORD attrib, int index)
{
	if(index >= _XDEF_MULTISTRINGARRAYCOUNT)
		return FALSE;

	m_attributeHighlight[index] = attrib;
	return TRUE;
}

DWORD _XMString::GetAttribHighlight(int index)
{
	if(index >= _XDEF_MULTISTRINGARRAYCOUNT)
		return FALSE;

	return m_attributeHighlight[index];
}
