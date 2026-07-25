// XGUIObject.cpp: implementation of the _XGUIObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XGUIObject.h"
#include "XKernel.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XGUIObject::_XGUIObject()
{
	m_Enable			= FALSE;
	m_ShowWindow		= TRUE;
	m_WindowPosition.x	= 0;
	m_WindowPosition.y	= 0;

	m_WindowSize.cx		= 0;
	m_WindowSize.cy		= 0;

	m_CommandID			= NULL;
	m_lParam			= 0;
	
	m_pParentWindow		= NULL;
}

_XGUIObject::~_XGUIObject()
{

}

void _XGUIObject::CenterMouse( void )
{
	POINT centerposition;
	centerposition.x = m_WindowPosition.x + (m_WindowSize.cx>>1);
	centerposition.y = m_WindowPosition.y + (m_WindowSize.cy>>1);

	RECT rect;
	::GetClientRect( gHWnd, &rect );	
	ClientToScreen( gHWnd, &centerposition );
	::SetCursorPos( centerposition.x, centerposition.y );
	gApp->ReleaseClickCursor();
}
