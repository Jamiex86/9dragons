// XIME.cpp: implementation of the _XIME class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"
#include <TCHAR.h>

//////////////////////////////////////////////////////////////////////
// _XIMEContainer
//////////////////////////////////////////////////////////////////////


#ifdef _DEBUG
#define DXUTTRACE _XOutputDebugString
VOID _XOutputDebugString( LPCTSTR strMsg, ... )
{
    char strBuffer[512];
    
    va_list args;
    va_start(args, strMsg);
    vsprintf( strBuffer, strMsg, args );
    strBuffer[511] = '\0';
    va_end(args);

    OutputDebugStringA( strBuffer );
}
#else
	#define DXUTTRACE /##/
	//#define DXUTTRACE _XLog
#endif

wchar_t	g_aszIndicator[5][3] = 
{
		L"En",
		L"\x7B80",
		L"\x7E41",
		L"\xAC00",
		L"\x3042"
};

extern _XIMM g_imm;
static HIMC	 gs_hImcDef = NULL;

TCHAR g_MBCSConvertBuffer[MAX_IMEBUFFERLENGTH] = _T("");

int	_X_WtoM_ConvertString(UINT codePage, const wchar_t* wText, int wLen, char* text, int len)
{
	memset( text, 0, sizeof(char) * len );
	if(text == 0) {
		return WideCharToMultiByte(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);
	} else {
		int tLen = WideCharToMultiByte(codePage, 0, wText, wLen, NULL, 0, NULL, NULL);
		
		if(tLen > len) {
			return 0;
		} else {
			return WideCharToMultiByte(codePage, 0, wText, wLen, text, tLen, NULL, NULL);
		}
	}
}

int _X_GetPrevDBCSPos(LPCTSTR strBuf, int nPos)
{
	int checkpoint = -1;
    int i = 0  ; for( i = 0; i <= nPos; i++)
    {
        if(isascii(strBuf[i])==0)
        {
			checkpoint = i;
		}

		if(nPos == i)
		{
			break;
		}
    }

    return checkpoint;
}

BOOL IsDBCS(LPCTSTR strBuf, int nPos)
{
    BOOL bRet = FALSE;
    int i = 0  ; for( i = 0; i <= nPos; i++)
    {
        if(isascii(strBuf[i])==0)
        {
			/*if(isascii(strBuf[i+1]) ==0)
			{
			   bRet = FALSE;
			}
			else*/
			{
				if(nPos == i+1)
					bRet = TRUE;
			}
			i++;
        }
    }
    return bRet;
}

int _XMakeCurrencySeparator(char *pDest, char *pSrc, int nSepInterval, char cSepChar)
{
    int        nSrcLen=    strlen(pSrc);
    int        nDestLen;
    char    *pFindDot=    strchr(pSrc, '.');
    int        nDotPos;
	int		   nDotCount = 0;	
    // Error
    if(!nSepInterval) return nDotCount;
	
    // Initial
    if(pFindDot == NULL)
    {
        nDotPos= nSrcLen;
        nDestLen= nSrcLen + nSrcLen/nSepInterval;
    }
    else
    {
        nDotPos= pFindDot - pSrc;
        nDestLen= nSrcLen + nDotPos/nSepInterval;
    }
	
    memset(pDest, NULL, nDestLen+1);
	
    // Copy to destination buffer
    int        nCurDestPos= nDestLen - 1;
    int        nCount= 0;
	
    int i = 0  ; for( i=nSrcLen-1 ; i>=0 ; i--)
    {
        if(!(nCount%nSepInterval) && nCount)
        {
            pDest[nCurDestPos]= cSepChar;
            nCurDestPos--;
			nDotCount++;
        }
		
        pDest[nCurDestPos]= pSrc[i];
		
        if(i < nDotPos)    nCount++;
        nCurDestPos--;
    }
	
    // Adjusting
    if(*pDest == NULL)
    {
        memcpy(pDest, pDest+1, nDestLen+1);
    }

	return nDotCount;
}

int _XCalcCurrencySeparatorCount( char *pSrc, int nSepInterval )
{
	int        nSrcLen=    strlen(pSrc);
    int        nDestLen;
    char    *pFindDot=    strchr(pSrc, '.');
    int        nDotPos;
	int		   nDotCount = 0;	
    // Error
    if(!nSepInterval) return nDotCount;
	
    // Initial
    if(pFindDot == NULL)
    {
        nDotPos= nSrcLen;
        nDestLen= nSrcLen + nSrcLen/nSepInterval;
    }
    else
    {
        nDotPos= pFindDot - pSrc;
        nDestLen= nSrcLen + nDotPos/nSepInterval;
    }
	
    // Copy to destination buffer
    int        nCurDestPos= nDestLen - 1;
    int        nCount= 0;
	
    int i = 0  ; for( i=nSrcLen-1 ; i>=0 ; i--)
    {
        if(!(nCount%nSepInterval) && nCount)
        {
            nCurDestPos--;
			nDotCount++;
        }
		
        if(i < nDotPos)    nCount++;
        nCurDestPos--;
    }
	    	
	return nDotCount;
}

int _XCalcCurrencySeparatorCount( wchar_t *pSrc, int nSepInterval )
{
	int        nSrcLen   = wcslen(pSrc);
    int        nDestLen  = 0;
    wchar_t   *pFindDot  = wcschr(pSrc, '.');
    int        nDotPos   = 0;
	int		   nDotCount = 0;	
    // Error
    if(!nSepInterval) return nDotCount;
	
    // Initial
    if(pFindDot == NULL)
    {
        nDotPos= nSrcLen;
        nDestLen= nSrcLen + nSrcLen/nSepInterval;
    }
    else
    {
        nDotPos= pFindDot - pSrc;
        nDestLen= nSrcLen + nDotPos/nSepInterval;
    }
	
    // Copy to destination buffer
    int        nCurDestPos= nDestLen - 1;
    int        nCount= 0;
	
    int i = 0  ; for( i=nSrcLen-1 ; i>=0 ; i--)
    {
        if(!(nCount%nSepInterval) && nCount)
        {
            nCurDestPos--;
			nDotCount++;
        }
		
        if(i < nDotPos)    nCount++;
        nCurDestPos--;
    }
	
	return nDotCount;
}

_XIMEContainer::_XIMEContainer()
{
	m_WindowPosition.x				= m_WindowPosition.y = 0;
	m_WindowSize.cx					= 256;
	m_WindowSize.cy					= 12;
	ResetContainer();
}

_XIMEContainer::~_XIMEContainer()
{
}

void _XIMEContainer::ResetContainer( void )
{	
	m_length						= 0;
	m_MaxLength						= MAX_IMEBUFFERLENGTH;
	m_cursorX						= 0;
	m_active						= false; 
	m_comLength						= 0;
	m_candleNum						= -1;
	m_AlignType						= _XFONT_ALIGNTYPE_LEFT;
	m_ShowWindow					= FALSE;	
	m_NoFocusedDraw					= FALSE;
	m_ShowWindowRectangle			= FALSE;
	m_bWhiteLineRectagle			= FALSE;
	//m_WindowPosition.x				= m_WindowPosition.y = 0;
	//m_WindowSize.cx					= 256;
	//m_WindowSize.cy					= 12;	
	m_TextColor						= _XSC_DEFAULT;
	m_CaretColor					= D3DCOLOR_ARGB( 255, 215, 235, 188 );
	m_pReturnKeyCallbackFunction	= NULL;
	m_pEscapeKeyCallbackFunction	= NULL;
	m_pTabKeyCallbackFunction		= NULL;
	m_pUpArrowKeyCallbackFunction	= NULL;
	m_pDownArrowKeyCallbackFunction = NULL;
	m_pLeftArrowKeyCallbackFunction = NULL;
	m_pRightArrowKeyCallbackFunction = NULL;
	m_pBackSpaceKeyCallbackFunction = NULL;
	m_pCapsLockKeyCallbackFunction	= NULL;
	m_pDataChangeCallbackFunction   = NULL;
	m_pNextIMEContainer				= NULL;
	m_PasswordEdit					= FALSE;
	m_bNewLine						= FALSE;
	m_HorizScrolling				= FALSE;	
	m_ScrollPosition				= 0;
	m_NumericEditMode				= FALSE;
	m_NumericSeparationMode			= FALSE;
	m_VKeyboardTypedCount			= 0;

	memset( m_text, 0, sizeof(wchar_t) * (MAX_IMEBUFFERLENGTH + 1) );
	memset( m_candleText, 0, sizeof(TCHAR) * MAX_CANDLE_LENGTH * MAX_CANDLE_COUNT );
	memset( m_comText, 0, sizeof(wchar_t) * (MAX_IMEBUFFERLENGTH + 1) );
}

void _XIMEContainer::ClearBuffer( void )
{
	memset( m_text, 0, sizeof(wchar_t) * (MAX_IMEBUFFERLENGTH + 1) );
	memset( m_candleText, 0, sizeof(TCHAR) * MAX_CANDLE_LENGTH * MAX_CANDLE_COUNT );
	memset( m_comText, 0, sizeof(wchar_t) * (MAX_IMEBUFFERLENGTH + 1) );

	m_length						= 0;
	m_cursorX						= 0;	
	m_comLength						= 0;
	m_candleNum						= -1;
	m_ScrollPosition				= 0;
}

void _XIMEContainer::SetFocus( void )
{
	_XIMEKERNEL.SetFocus( this );
}

BOOL _XIMEContainer::IsFocused( void )
{
	return (_XIMEKERNEL.GetIMEContainer() == this);
}

int	_XIMEContainer::GetWidth( wchar_t* pText )
{
	if( !pText || wcslen(pText) <= 0 )
		return 0;

	int size = _X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, pText, wcslen( pText ), g_MBCSConvertBuffer, MAX_IMEBUFFERLENGTH);	
	if( g_XBaseFont )
	{
		return g_XBaseFont->GetWidth( g_MBCSConvertBuffer );
	}
	else
	{
		return 0;
	}

	return 0;
}

// insert word current position
void _XIMEContainer::AddUnicode(wchar_t w, BOOL callchangedcallback)
{
	if( m_NumericEditMode )
	{
		if( g_LanguageType == _XLANGUAGE_TYPE_JAPANESE ) //convert full width numeric character
		{
			if( w >= 0xFF10 && w <= 0xFF19 ) 
			{
				w = L'0' + (w - 0xFF10);
			}
		}

		if( !( w >= L'0'&& w <= L'9' ) ) // half width num 
		{
			return;
		}
	}

	if( !m_HorizScrolling )
	{
		if( GetWidth( m_text ) > m_WindowSize.cx ) return;
	}

	memset( g_MBCSConvertBuffer, 0, sizeof(TCHAR) * MAX_IMEBUFFERLENGTH );
	_X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, m_text, wcslen(m_text), g_MBCSConvertBuffer, MAX_IMEBUFFERLENGTH);
	int size1 = strlen( g_MBCSConvertBuffer );

	wchar_t text[2];
	text[0] = w; text[1] = 0;	
	_X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, text, 1, g_MBCSConvertBuffer, MAX_IMEBUFFERLENGTH);
	int size2 = strlen( g_MBCSConvertBuffer );
	
	if( (size1 + size2) > m_MaxLength) return;

	wchar_t rightStr[MAX_IMEBUFFERLENGTH];
	memset( rightStr, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
	int rightLen = GetRightString(rightStr);

	m_text[m_cursorX] = w;
	m_cursorX++;	
	m_length++;

	if(rightLen)
	{
		memset( m_text + m_cursorX, 0, sizeof(wchar_t) * (MAX_IMEBUFFERLENGTH - m_cursorX ) );
		wcsncpy(m_text + m_cursorX, rightStr ,rightLen);
	}

	m_text[m_length] = NULL;

	if( m_pDataChangeCallbackFunction && callchangedcallback )
	{
		m_pDataChangeCallbackFunction( this, 0 );
	}

	if( m_HorizScrolling )
	{
		int leftLen = 0;
		wchar_t leftStr[MAX_IMEBUFFERLENGTH];		
		memset( leftStr, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
		leftLen = GetLeftString(leftStr);
		
		if( leftLen > 0 )
		{
			if( m_PasswordEdit )
			{
				wchar_t pwString[_XDEF_MAXPWSTRINGSIZE];
				int i = 0 ; for(  i = 0; i < _XDEF_MAXPWSTRINGSIZE; i++ ) pwString[i] = L'*';
				pwString[_XDEF_MAXPWSTRINGSIZE-1] = NULL;
				pwString[ wcslen( leftStr ) ] = NULL;

				int leftWidth = g_XBaseFont->GetWidth( pwString );
				if( leftWidth > m_WindowSize.cx )
					m_ScrollPosition = leftWidth - m_WindowSize.cx;
			}
			else
			{
				int leftWidth = g_XBaseFont->GetWidth( leftStr );
				if( leftWidth > m_WindowSize.cx )
					m_ScrollPosition = leftWidth - m_WindowSize.cx;
			}
		}
	}
}

void _XIMEContainer::AddString(char* string, int _length, BOOL callchangedcallback)
{
	wchar_t tempbuffer[MAX_IMEBUFFERLENGTH];
	memset( tempbuffer, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );

	MultiByteToWideChar(_XIMEKERNEL.m_codePage, 0, string, _length, tempbuffer, MAX_IMEBUFFERLENGTH);
	AddUnicodeString(tempbuffer, wcslen(tempbuffer),callchangedcallback);
}

void _XIMEContainer::AddUnicodeString(wchar_t *string,int _length, BOOL callchangedcallback)		
{
	int i = 0  ; for( i = 0;i<_length;i++)
	{
		AddUnicode(string[i],callchangedcallback);
	}
}

void _XIMEContainer::ResetCandleList( void )
{	
	memset( m_candleText, 0, sizeof(TCHAR) * MAX_CANDLE_LENGTH * MAX_CANDLE_COUNT );
	m_candleNum = 0;
	m_candleLength = 0;
}

void _XIMEContainer::SetCandleText(int candleindex, const TCHAR *candles)
{
	if( candleindex >= MAX_CANDLE_COUNT ) return;
	memset( m_candleText[candleindex], 0, sizeof(TCHAR) * MAX_CANDLE_LENGTH );
	strncpy( m_candleText[candleindex],candles,strlen(candles) );
}

void _XIMEContainer::SetCandleNum(int num)
{
	m_candleNum = num;
}

void _XIMEContainer::SetCandleLength(int len)
{
	m_candleLength = len;
}

void _XIMEContainer::ProcessDelKey( void )				
{
	if(m_cursorX == m_length)
		return;

	wchar_t rightStr[MAX_IMEBUFFERLENGTH];
	memset( rightStr, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
	int rightLen = GetRightString(rightStr);

	m_length--;

	if( m_length < 0 )
	{
		m_length = 0;
	}
	
	if( m_cursorX < 0 )
	{ 
		m_cursorX = 0;
	}

	m_text[m_cursorX] = 0;
	
	if(rightLen > 0 )
	   wcsncpy(m_text + m_cursorX, rightStr+1,rightLen-1);

	m_text[m_length] = 0;

	if( m_pDataChangeCallbackFunction )
	{
		m_pDataChangeCallbackFunction( this, VK_DELETE );
	}
}

void _XIMEContainer::ProcessHomeKey( void )
{
	m_cursorX = 0;

	if( m_HorizScrolling )
	{
		int leftLen = 0;
		wchar_t leftStr[MAX_IMEBUFFERLENGTH];		
		memset( leftStr, 0, sizeof(wchar_t )*MAX_IMEBUFFERLENGTH );
		leftLen = GetLeftString(leftStr);
		
		if( leftLen > 0 )
		{
			if( m_PasswordEdit )
			{
				wchar_t pwString[_XDEF_MAXPWSTRINGSIZE];
				int i = 0 ; for(  i = 0; i < _XDEF_MAXPWSTRINGSIZE; i++ ) pwString[i] = L'*';
				pwString[_XDEF_MAXPWSTRINGSIZE-1] = NULL;
				pwString[ wcslen( leftStr ) ] = NULL;
				
				int leftWidth = g_XBaseFont->GetWidth( pwString );
				if( leftWidth < m_ScrollPosition )
					m_ScrollPosition = leftWidth;
			}
			else
			{
				int leftWidth = g_XBaseFont->GetWidth( leftStr );
				if( leftWidth < m_ScrollPosition )
					m_ScrollPosition = leftWidth;
			}
		}
		else
		{
			m_ScrollPosition = 0;
		}
	}
}

void _XIMEContainer::ProcessEndKey( void )
{
	m_cursorX = m_length;

	if( m_HorizScrolling )
	{
		int leftLen = 0;
		wchar_t leftStr[MAX_IMEBUFFERLENGTH];		
		memset( leftStr, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
		leftLen = GetLeftString(leftStr);
		
		if( leftLen > 0 )
		{
			if( m_PasswordEdit )
			{
				wchar_t pwString[_XDEF_MAXPWSTRINGSIZE];
				int i = 0 ; for(  i = 0; i < _XDEF_MAXPWSTRINGSIZE; i++ ) pwString[i] = L'*';
				pwString[_XDEF_MAXPWSTRINGSIZE-1] = NULL;
				pwString[ wcslen( leftStr ) ] = NULL;
				
				int leftWidth = g_XBaseFont->GetWidth( pwString );
				if( leftWidth > m_WindowSize.cx )
					m_ScrollPosition = leftWidth - m_WindowSize.cx;
			}
			else
			{
				int leftWidth = g_XBaseFont->GetWidth( leftStr );
				if( leftWidth > m_WindowSize.cx )
					m_ScrollPosition = leftWidth - m_WindowSize.cx;
			}
		}
	}
}

void _XIMEContainer::ProcessBackSpaceKey( void )
{
	if(m_cursorX <= 0)
	{
		m_cursorX = 0;
		m_bNewLine = TRUE;
		return ;
	}

	wchar_t rightStr[MAX_IMEBUFFERLENGTH];
	memset( rightStr, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
	int rightLen = GetRightString(rightStr);

	--m_cursorX;
	--m_length;

	if( m_length < 0 )
	{
		m_length = 0;
		m_cursorX = 0;
		m_ScrollPosition = 0;
		memset( m_text, 0, sizeof(wchar_t) * MAX_IMEBUFFERLENGTH );
		if( m_pDataChangeCallbackFunction )
		{
			m_pDataChangeCallbackFunction( this, VK_BACK );
		}

		return;
	}

	if( m_cursorX < 0 )
	{ 
		m_cursorX = 0;
	}

	m_text[m_cursorX] = 0;

	if(rightLen > 0)
	{
		memset( m_text + m_cursorX, 0, sizeof(wchar_t) * (MAX_IMEBUFFERLENGTH - m_cursorX ) );
		wcsncpy(m_text + m_cursorX, rightStr ,rightLen);
	}

	//m_text[m_length] = 0;
	memset( m_text + m_length, 0, sizeof(wchar_t) * (MAX_IMEBUFFERLENGTH - m_length ) );

	if( m_pDataChangeCallbackFunction )
	{
		m_pDataChangeCallbackFunction( this, VK_BACK );
	}

	if( m_HorizScrolling )
	{
		int leftLen = 0;
		wchar_t leftStr[MAX_IMEBUFFERLENGTH];		
		memset( leftStr, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
		leftLen = GetLeftString(leftStr);
		
		if( leftLen > 0 )
		{
			if( m_PasswordEdit )
			{
				wchar_t pwString[_XDEF_MAXPWSTRINGSIZE];
				int i = 0 ; for(  i = 0; i < _XDEF_MAXPWSTRINGSIZE; i++ ) pwString[i] = L'*';
				pwString[_XDEF_MAXPWSTRINGSIZE-1] = NULL;
				pwString[ wcslen( leftStr ) ] = NULL;
				
				int leftWidth = g_XBaseFont->GetWidth( pwString );
				if( leftWidth < m_ScrollPosition )
					m_ScrollPosition = leftWidth;
			}
			else
			{
				int leftWidth = g_XBaseFont->GetWidth( leftStr );
				if( leftWidth < m_ScrollPosition )
					m_ScrollPosition = leftWidth;
			}
		}
		else
		{
			m_ScrollPosition = 0;
		}
	}
}

void _XIMEContainer::ProcessEscapeKey( void )
{
	
}

void _XIMEContainer::ProcessTabKey( void )
{
	if( m_pNextIMEContainer )
	{		
		_XIMEKERNEL.SetFocus( m_pNextIMEContainer );
	}
	else if( m_pTabKeyCallbackFunction )
	{
		m_pTabKeyCallbackFunction( this );
	}
}

void _XIMEContainer::ProcessLeftKey( void )
{
	if( m_cursorX < 0 )
		m_cursorX = 0;

	if(m_cursorX == 0)
	{
		m_bNewLine = TRUE;
		return;
	}

	--m_cursorX;

	if(m_cursorX < 0 )
	{
		m_cursorX = 0;
	}

	if( m_HorizScrolling )
	{
		int leftLen = 0;
		wchar_t leftStr[MAX_IMEBUFFERLENGTH];		
		memset( leftStr, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
		leftLen = GetLeftString(leftStr);
		
		if( leftLen > 0 )
		{
			if( m_PasswordEdit )
			{
				wchar_t pwString[_XDEF_MAXPWSTRINGSIZE];
				int i = 0 ; for(  i = 0; i < _XDEF_MAXPWSTRINGSIZE; i++ ) pwString[i] = L'*';
				pwString[_XDEF_MAXPWSTRINGSIZE-1] = NULL;
				pwString[ wcslen( leftStr ) ] = NULL;
				
				int leftWidth = g_XBaseFont->GetWidth( pwString );
				if( leftWidth < m_ScrollPosition )
					m_ScrollPosition = leftWidth;
			}
			else
			{
				int leftWidth = g_XBaseFont->GetWidth( leftStr );
				if( leftWidth < m_ScrollPosition )
					m_ScrollPosition = leftWidth;
			}
		}
		else
		{
			m_ScrollPosition = 0;
		}
	}
}

void _XIMEContainer::ProcessRightKey( void )
{
	if(m_cursorX == m_length)
	{
		m_bNewLine = TRUE;
		return;
	}

	m_cursorX++;
	
	if(m_cursorX > m_length )
	{
		m_cursorX = m_length;
	}
	
	if( m_HorizScrolling )
	{
		int leftLen = 0;
		wchar_t leftStr[MAX_IMEBUFFERLENGTH];		
		memset( leftStr, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
		leftLen = GetLeftString(leftStr);
		
		if( leftLen > 0 )
		{
			if( m_PasswordEdit )
			{
				wchar_t pwString[_XDEF_MAXPWSTRINGSIZE];
				int i = 0 ; for(  i = 0; i < _XDEF_MAXPWSTRINGSIZE; i++ ) pwString[i] = L'*';
				pwString[_XDEF_MAXPWSTRINGSIZE-1] = NULL;
				pwString[ wcslen( leftStr ) ] = NULL;
				
				int leftWidth = g_XBaseFont->GetWidth( pwString );
				if( leftWidth > m_WindowSize.cx )
					m_ScrollPosition = leftWidth - m_WindowSize.cx;
			}
			else
			{
				int leftWidth = g_XBaseFont->GetWidth( leftStr );
				if( leftWidth > m_WindowSize.cx )
					m_ScrollPosition = leftWidth - m_WindowSize.cx;
			}
		}
	}
}

int _XIMEContainer::GetLeftString(wchar_t* target)			
{
	if(m_cursorX == 0)
		return 0;

	wcsncpy(target, m_text, m_cursorX);
	return m_cursorX;
}

int _XIMEContainer::GetRightString(wchar_t* target)
{
	if(m_cursorX == m_length)
		return 0;

	if( m_length - m_cursorX > 0 )
	{
		wcsncpy(target,m_text + m_cursorX, m_length - m_cursorX);
		return m_length - m_cursorX;
	}
		
	return 0;
}

void _XIMEContainer::SetCursorX( int cursorx )
{
	if( cursorx < m_length )
		m_cursorX = cursorx;
	else
		m_cursorX = m_length;
}

void _XIMEContainer::SetCursorX_ByMBCSSize( int cursorx )
{
	if( cursorx <= 0 )
	{
		m_cursorX = 0;
		return;
	}

	int mbcsstringsize = _X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, m_text, m_length, g_MBCSConvertBuffer, MAX_IMEBUFFERLENGTH);

	char tempbuffer[MAX_IMEBUFFERLENGTH];
	memset( tempbuffer, 0, sizeof(char) * MAX_IMEBUFFERLENGTH );
	strncpy( tempbuffer, g_MBCSConvertBuffer, cursorx );
	int mbcslen = strlen( g_MBCSConvertBuffer );
	
	wchar_t unicodetempbuffer[MAX_IMEBUFFERLENGTH];
	memset( unicodetempbuffer, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
	
	MultiByteToWideChar(_XIMEKERNEL.m_codePage, 0, tempbuffer, mbcslen, unicodetempbuffer, MAX_IMEBUFFERLENGTH);

	int unicodecursorx = wcslen( unicodetempbuffer );
	if( unicodecursorx < 0 )
		unicodecursorx = 0;

	if( unicodecursorx < m_length )
		m_cursorX = unicodecursorx;
	else
		m_cursorX = m_length;
}

void _XIMEContainer::SetComText(const wchar_t* text,int length)
{
	wcsncpy(m_comText,text, length);
	m_comText[length] = 0;
	m_comLength = length;
}

void _XIMEContainer::SetComTextBlank( void )
{
	m_comText[0] = 0; 
	m_comLength = 0;
}

// get..
int	_XIMEContainer::GetMaxLength( void )
{
	return m_MaxLength;
}

TCHAR* _XIMEContainer::GetText( void )
{ 
	int size = _X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, m_text, m_length, g_MBCSConvertBuffer, MAX_IMEBUFFERLENGTH);	
	return g_MBCSConvertBuffer;
}

wchar_t* _XIMEContainer::GetUnicodeText( void )
{
	return m_text; 
}

wchar_t* _XIMEContainer::GetComText( void )
{
	return m_comText;
}

int	_XIMEContainer::GetComTextlength( void )	
{ 
	return m_comLength;
}

TCHAR* _XIMEContainer::GetCandle( int candleindex )
{
	if( candleindex >= m_candleNum ) return NULL;
	return m_candleText[candleindex];
}

int	_XIMEContainer::GetCandleTextlength( void )
{
	return m_candleLength;
}

int	_XIMEContainer::GetLength( void )	
{
	return m_length;
}
int	_XIMEContainer::GetLength_ByMBCSSize( void )
{
	return _X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, m_text, m_cursorX, NULL, 0);
}

int	 _XIMEContainer::GetCursorX( void )
{
	return m_cursorX;
}

int	_XIMEContainer::GetCursorX_ByMBCSSize( void )
{
	memset( g_MBCSConvertBuffer, 0, sizeof(TCHAR) * MAX_IMEBUFFERLENGTH );
	_X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, m_text, m_cursorX, g_MBCSConvertBuffer, MAX_IMEBUFFERLENGTH);
	return strlen( g_MBCSConvertBuffer );
}

bool _XIMEContainer::GetAvtive( void )	
{
	return m_active;
}

bool _XIMEContainer::GetNewLine( void )	
{
	return m_bNewLine;
}

bool _XIMEContainer::GetFocus( void )
{
	return ( _XIMEKERNEL.GetIMEContainer() == this );
}

bool _XIMEContainer::IsCandle( void )
{
	if(m_candleNum == -1)
		return false;

	return true;
}


void _XIMEContainer::MoveWindow( int X, int Y )
{
	m_WindowPosition.x = X;
	m_WindowPosition.y = Y;
}

void _XIMEContainer::ResizeWindow( int width, int height )
{ 
	m_WindowSize.cx = width; 
	m_WindowSize.cy = height; 
}




void _XIMEContainer::Draw( _XGUIObject*& pfocusobject )
{	
	if( !m_ShowWindow && !m_NoFocusedDraw )	return;
		
	wchar_t leftstring[MAX_IMEBUFFERLENGTH];
	wchar_t rightstring[MAX_IMEBUFFERLENGTH];
	wchar_t textbuffer[MAX_IMEBUFFERLENGTH];
	char    mbcstextbuffer[MAX_IMEBUFFERLENGTH];
	
	memset( leftstring,  0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
	memset( rightstring, 0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
	memset( textbuffer,  0, sizeof(wchar_t)*MAX_IMEBUFFERLENGTH );
	
	GetLeftString(leftstring);
	GetRightString(rightstring);

	if( m_NumericEditMode )
	{
		swprintf( textbuffer, L"%s%s", leftstring, rightstring );
	}
	else
	{
		swprintf( textbuffer, L"%s%s%s", leftstring, m_comText, rightstring );
	}

	_X_WtoM_ConvertString( _XIMEKERNEL.m_codePage, textbuffer, wcslen(textbuffer), mbcstextbuffer, MAX_IMEBUFFERLENGTH);

	if( g_LanguageType == _XLANGUAGE_TYPE_TAIWANESE || g_LanguageType == _XLANGUAGE_TYPE_JAPANESE || g_LanguageType == _XLANGUAGE_TYPE_RUSSIAN ) // Unicode section
	{
		int strwidth = g_XBaseFont->GetWidth( textbuffer );
		int leftsize = g_XBaseFont->GetWidth( leftstring );

		if( m_ShowWindowRectangle )
		{		
			if( !m_bWhiteLineRectagle )
			{
				_XDrawRectAngle( m_WindowPosition.x - 2, m_WindowPosition.y-2, m_WindowPosition.x+m_WindowSize.cx+1, 
					m_WindowPosition.y+m_WindowSize.cy+1, 0.0f, D3DCOLOR_ARGB(255,20,20,20) );
			}
			else
			{
				_XDrawRectAngle( m_WindowPosition.x - 2, m_WindowPosition.y-2, m_WindowPosition.x+m_WindowSize.cx+1, 
					m_WindowPosition.y+m_WindowSize.cy+1, 0.0f, D3DCOLOR_ARGB(255,255,255,255) );
			}
		}

		if( m_PasswordEdit )
		{
			TCHAR pwString[_XDEF_MAXPWSTRINGSIZE];
			memset( pwString, '*', _XDEF_MAXPWSTRINGSIZE-1 );
			pwString[_XDEF_MAXPWSTRINGSIZE-1] = NULL;
			pwString[ wcslen( textbuffer ) ] = NULL;

			g_XBaseFont->SetColor(m_TextColor);

			if(m_HorizScrolling)
			{
				int alignpt = 0;
				int scrollpt = 0;

				g_XBaseFont->SetClippingArea( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, m_WindowPosition.y+m_WindowSize.cy+1 );

				strwidth = g_XBaseFont->GetWidth( pwString );
				
				if( strwidth > m_WindowSize.cx )
				{
					scrollpt = -m_ScrollPosition;
				}

				switch( m_AlignType ) 
				{
				default:
				case _XFONT_ALIGNTYPE_LEFT	:
					alignpt = 0;
					g_XBaseFont->Puts( m_WindowPosition.x+scrollpt, m_WindowPosition.y, pwString );
					break;
				case _XFONT_ALIGNTYPE_RIGHT :			
					alignpt = m_WindowSize.cx - strwidth + scrollpt;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pwString );
					break;
				case _XFONT_ALIGNTYPE_CENTER :
					alignpt = (m_WindowSize.cx>>1) - (strwidth>>1) + scrollpt;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pwString );
					break;		
				}		
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						int length = wcslen( leftstring );
						int asterisksize = g_XBaseFont->GetWidth( "*" );
						
						g_XBaseFont->SetColor(m_CaretColor);
						g_XBaseFont->Puts( m_WindowPosition.x+length*asterisksize+alignpt+scrollpt, m_WindowPosition.y, _XDEF_CARETTYPE );			
					}		
				}

				g_XBaseFont->Flush();
				g_XBaseFont->DisableClippingArea();
			}
			else
			{		
				g_XBaseFont->SetClippingArea( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, m_WindowPosition.y+m_WindowSize.cy+1 );

				int alignpt;
				switch( m_AlignType ) 
				{
				default:
				case _XFONT_ALIGNTYPE_LEFT	:
					alignpt = 0;
					g_XBaseFont->Puts( m_WindowPosition.x, m_WindowPosition.y, pwString );
					break;
				case _XFONT_ALIGNTYPE_RIGHT :			
					alignpt = m_WindowSize.cx - strwidth;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pwString );
					break;
				case _XFONT_ALIGNTYPE_CENTER :
					alignpt = (m_WindowSize.cx>>1) - (strwidth>>1);
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pwString );
					break;		
				}		
				g_XBaseFont->Flush();
				g_XBaseFont->DisableClippingArea();
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						int length = wcslen( leftstring );
						int asterisksize = g_XBaseFont->GetWidth( "*" );

						g_XBaseFont->SetColor(m_CaretColor);
						g_XBaseFont->Puts( m_WindowPosition.x+length*asterisksize+alignpt, m_WindowPosition.y, _XDEF_CARETTYPE );			
					}		
				}
				g_XBaseFont->Flush();
			}
		}
		else if( m_NumericEditMode && m_NumericSeparationMode )
		{
			g_XBaseFont->SetColor(m_TextColor);		

			LPTSTR pOutputTextString = mbcstextbuffer;
			TCHAR numericseparationbuffer[256];
			int nDotcount = 0;
			
			int strlength = strlen( mbcstextbuffer );

			if( strlength > 3 )
			{
				nDotcount = _XMakeCurrencySeparator( numericseparationbuffer, mbcstextbuffer, 3, ',' );
				pOutputTextString = numericseparationbuffer;
			}
			
			int strwidth = g_XBaseFont->GetWidth( pOutputTextString );
			int commawidth = g_XBaseFont->GetWidth( "," );

			if(m_HorizScrolling)
			{
				int alignpt;
							
				int rightlen = wcslen( rightstring );
				int commatotalwidth = 0;
				int rightdotcnt = _XCalcCurrencySeparatorCount( rightstring, 3 );
				
				int leftdotcnt = nDotcount - rightdotcnt;
				if( leftdotcnt > 0 )
				{
					commatotalwidth = leftdotcnt * commawidth;
				}
				else
				{
					commatotalwidth = 0;
				}

				g_XBaseFont->SetClippingArea( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, m_WindowPosition.y+m_WindowSize.cy-1 );

				if( strwidth > m_WindowSize.cx )
				{
					alignpt = -m_ScrollPosition;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pOutputTextString );
				}
				else
				{
					alignpt = 0;
					g_XBaseFont->Puts( m_WindowPosition.x, m_WindowPosition.y, pOutputTextString );
				}
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						g_XBaseFont->SetColor(m_CaretColor);
						//if( strwidth > leftsize+(-alignpt) )
						{
							g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt+commatotalwidth, m_WindowPosition.y, _XDEF_CARETTYPE );
						}
						/*else
						{
							g_XBaseFont->Puts( m_WindowPosition.x+strwidth-8, m_WindowPosition.y, _XDEF_CARETTYPE );
						}*/
					}
				}
		
				g_XBaseFont->Flush();
				g_XBaseFont->DisableClippingArea();
			}
			else
			{
				int rightlen = wcslen( rightstring );
				int commatotalwidth = 0;
				int rightdotcnt = _XCalcCurrencySeparatorCount( rightstring, 3 );
 
				int leftdotcnt = nDotcount - rightdotcnt;
				if( leftdotcnt > 0 )
				{
					commatotalwidth = leftdotcnt * commawidth;
				}
				else
				{
					commatotalwidth = 0;
				}
				
				g_XBaseFont->SetClippingArea( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, m_WindowPosition.y+m_WindowSize.cy+1 );

				int alignpt;
				switch( m_AlignType ) 
				{
				default:
				case _XFONT_ALIGNTYPE_LEFT	:
					alignpt = 0;
					g_XBaseFont->Puts( m_WindowPosition.x, m_WindowPosition.y, pOutputTextString );
					break;
				case _XFONT_ALIGNTYPE_RIGHT :			
					alignpt = m_WindowSize.cx - strwidth;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pOutputTextString );			
					break;
				case _XFONT_ALIGNTYPE_CENTER :
					alignpt = (m_WindowSize.cx>>1) - (strwidth>>1);			
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pOutputTextString );
					break;
				}
				g_XBaseFont->Flush();
				g_XBaseFont->DisableClippingArea();
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						g_XBaseFont->SetColor(m_CaretColor);
						g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt+commatotalwidth, m_WindowPosition.y, _XDEF_CARETTYPE );
					}
				}

				g_XBaseFont->Flush();
			}		
		}
		else
		{	
			g_XBaseFont->SetColor(m_TextColor);		

			if(m_HorizScrolling)
			{
				int alignpt = 0;
				
				g_XBaseFont->SetClippingArea( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, m_WindowPosition.y+m_WindowSize.cy+1 );

				if( strwidth > m_WindowSize.cx )
				{
					alignpt = -m_ScrollPosition;

					int compsize = g_XBaseFont->GetWidth( m_comText );
					if( compsize )
					{
						int startpt = m_WindowPosition.x+alignpt+leftsize;
						int endpt	= m_WindowPosition.x+alignpt+leftsize+compsize;
						if( startpt < m_WindowPosition.x ) startpt = m_WindowPosition.x;
						if( endpt > m_WindowPosition.x+m_WindowSize.cx-1 ) endpt = m_WindowPosition.x+m_WindowSize.cx-1;

						_XDrawSolidBar( startpt, m_WindowPosition.y, endpt, m_WindowPosition.y+g_XBaseFont->GetHeight(), 0xFF7F7FFF );
					}

					g_XBaseFont->SetUnicodeDataMode( TRUE );
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, (LPCTSTR)textbuffer );
					g_XBaseFont->SetUnicodeDataMode( FALSE );
				}
				else
				{
					alignpt = 0;

					int compsize = g_XBaseFont->GetWidth( m_comText );
					if( compsize )
					{
						int startpt = m_WindowPosition.x+alignpt+leftsize;
						int endpt	= m_WindowPosition.x+alignpt+leftsize+compsize;
						if( startpt < m_WindowPosition.x ) startpt = m_WindowPosition.x;
						if( endpt > m_WindowPosition.x+m_WindowSize.cx-1 ) endpt = m_WindowPosition.x+m_WindowSize.cx-1;
						
						_XDrawSolidBar( startpt, m_WindowPosition.y, endpt, m_WindowPosition.y+g_XBaseFont->GetHeight(), 0xFF7F7FFF );					
					}

					g_XBaseFont->SetUnicodeDataMode( TRUE );
					g_XBaseFont->Puts( m_WindowPosition.x, m_WindowPosition.y, (LPCTSTR)textbuffer);
					g_XBaseFont->SetUnicodeDataMode( FALSE );
				}
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						g_XBaseFont->SetColor(m_CaretColor);
						//if( strwidth > leftsize+(-alignpt) )
						{
							g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt, m_WindowPosition.y, _XDEF_CARETTYPE );
						}
						/*else
						{
							g_XBaseFont->Puts( m_WindowPosition.x+strwidth-8, m_WindowPosition.y, _XDEF_CARETTYPE );
						}*/
					}
				}
		
				g_XBaseFont->Flush();
				g_XBaseFont->DisableClippingArea();
			}
			else
			{			
				int alignpt;
				switch( m_AlignType ) 
				{
				default:
				case _XFONT_ALIGNTYPE_LEFT	:
					alignpt = 0;
					break;
				case _XFONT_ALIGNTYPE_RIGHT :			
					alignpt = m_WindowSize.cx - strwidth;
					break;
				case _XFONT_ALIGNTYPE_CENTER :
					alignpt = (m_WindowSize.cx>>1) - (strwidth>>1);
					break;
				}
				
				int compsize = g_XBaseFont->GetWidth( m_comText );
				if( compsize )
				{
					int startpt = m_WindowPosition.x+alignpt+leftsize;
					int endpt	= m_WindowPosition.x+alignpt+leftsize+compsize;
					if( startpt < m_WindowPosition.x ) startpt = m_WindowPosition.x;
					if( endpt > m_WindowPosition.x+m_WindowSize.cx-1 ) endpt = m_WindowPosition.x+m_WindowSize.cx-1;
					
					_XDrawSolidBar( startpt, m_WindowPosition.y, endpt, m_WindowPosition.y+g_XBaseFont->GetHeight(), 0xFF7F7FFF );
				}

				g_XBaseFont->SetClippingArea( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, m_WindowPosition.y+m_WindowSize.cy+1 );
				g_XBaseFont->SetUnicodeDataMode( TRUE );
				g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, (LPCTSTR)textbuffer );
				g_XBaseFont->SetUnicodeDataMode( FALSE );
				g_XBaseFont->Flush();
				g_XBaseFont->DisableClippingArea();
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						g_XBaseFont->SetColor(m_CaretColor);
						g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt, m_WindowPosition.y, _XDEF_CARETTYPE );
					}
				}

				g_XBaseFont->Flush();
			}
		}	

		// draw reading string...
		if( _XIMEKERNEL.GetIMEContainer() == this )
		{
			int scrollposition = 0;

			if(m_HorizScrolling)
			{
				if( strwidth > m_WindowSize.cx )
				{
					scrollposition = -m_ScrollPosition;
				}
			}
					
			int alignpt;
			switch( m_AlignType ) 
			{
			default:
			case _XFONT_ALIGNTYPE_LEFT	:
				alignpt = 0;
				break;
			case _XFONT_ALIGNTYPE_RIGHT :			
				alignpt = m_WindowSize.cx - strwidth;
				break;
			case _XFONT_ALIGNTYPE_CENTER :
				alignpt = (m_WindowSize.cx>>1) - (strwidth>>1);
				break;
			}

			int compositestart = _XIMEKERNEL.m_ulStart;
			int compositeend = _XIMEKERNEL.m_ulEnd;
			
			int compositestartpt = 0;
			int compositeendpt = 0;

			if( compositestart != compositeend )
			{
				wchar_t starttext[256];
				wchar_t endtext[256];
				memset( starttext,0, sizeof(wchar_t) * 256 );
				memset( endtext,0, sizeof(wchar_t) * 256 );

				wcsncpy( starttext, _XIMEKERNEL.m_comp.c_str(), compositestart);
				wcsncpy( endtext, _XIMEKERNEL.m_comp.c_str(), compositeend);

				compositestartpt	= g_XBaseFont->GetWidth( starttext );
				compositeendpt	= g_XBaseFont->GetWidth( endtext );

				if( compositestartpt != compositeendpt )
				{
					_XDrawLine2D( m_WindowPosition.x+leftsize+alignpt+scrollposition + compositestartpt, m_WindowPosition.y + g_XBaseFont->GetHeight(), 
								  m_WindowPosition.x+leftsize+alignpt+scrollposition + compositeendpt, m_WindowPosition.y + g_XBaseFont->GetHeight(), 0xFFFF1010, 0xFFFF1010 );
				}
			}

			wchar_t text[256];
			int textLen = _XIMEKERNEL.GetReading(text, sizeof(text));

			if( textLen > 0 )
			{
				if(!_XIMEKERNEL.IsVerticalReading()) 
				{
					int _readingbasepos = 0;
					
					if( m_WindowPosition.y + (g_XBaseFont->GetHeight()<<1) + 4 < gnHeight )
					{
						_readingbasepos = m_WindowPosition.y + g_XBaseFont->GetHeight() + 2;
					}
					else
					{
						_readingbasepos = m_WindowPosition.y - ( 2 + g_XBaseFont->GetHeight() );
					}

					g_XBaseFont->SetColor(D3DCOLOR_ARGB( 255, 215, 235, 188 ));
					g_XBaseFont->SetUnicodeDataMode( TRUE );
					g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt+scrollposition, _readingbasepos, (LPCTSTR)text );
					g_XBaseFont->SetUnicodeDataMode( FALSE );
					g_XBaseFont->Flush();

					int _readingwidth = g_XBaseFont->GetWidth( text );

					_XDrawSolidBar( m_WindowPosition.x+leftsize+alignpt - 2+scrollposition+compositestartpt, 
									_readingbasepos-1,
									m_WindowPosition.x+leftsize+alignpt + 1 + _readingwidth+scrollposition+compositestartpt, 
									_readingbasepos + g_XBaseFont->GetHeight() + 1,
									0x40303030 );
					_XDrawRectAngle( m_WindowPosition.x+leftsize+alignpt - 2+scrollposition+compositestartpt, 
									 _readingbasepos-1,
									 m_WindowPosition.x+leftsize+alignpt + 1 + _readingwidth+scrollposition+compositestartpt, 
									 _readingbasepos + g_XBaseFont->GetHeight() + 1,
									 0.0f, D3DCOLOR_ARGB(192, 215, 235, 188) );
				} 
				else 
				{
					int _readingbasepos = 0;
					
					if( m_WindowPosition.y + g_XBaseFont->GetHeight() + 6 + (g_XBaseFont->GetHeight() * 4) < gnHeight )
					{
						_readingbasepos = m_WindowPosition.y + g_XBaseFont->GetHeight() + 3;
					}
					else
					{
						_readingbasepos = m_WindowPosition.y - ( 3 + (g_XBaseFont->GetHeight() * 4) );
					}

					g_XBaseFont->SetColor(D3DCOLOR_ARGB( 255, 215, 235, 188));
					
					wchar_t outputbuffer[2];
					outputbuffer[1] = 0x0000;

					int maxwidth = 0;
					int textpos = _readingbasepos;
					int i = 0  ; for( i=0; i < textLen; i++ ) 
					{
						outputbuffer[0] = text[i];
						if( outputbuffer[0] != 0 )
						{					
							g_XBaseFont->SetUnicodeDataMode( TRUE );
							g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt+scrollposition+compositestartpt, textpos, (LPCTSTR)outputbuffer );
							g_XBaseFont->SetUnicodeDataMode( FALSE );
							int width = g_XBaseFont->GetWidth( outputbuffer );
							if( width > maxwidth )
							{
								maxwidth = width;
							}
							
							textpos += g_XBaseFont->GetHeight();
						}
					}

					g_XBaseFont->Flush();

					_XDrawSolidBar( m_WindowPosition.x+leftsize+alignpt - 2+scrollposition+compositestartpt, 
									_readingbasepos - 1, 
									m_WindowPosition.x+leftsize+alignpt + 1 + maxwidth+scrollposition+compositestartpt, 
									_readingbasepos + (g_XBaseFont->GetHeight() << 2)+1, 
									0x30303030 );

					_XDrawRectAngle( m_WindowPosition.x+leftsize+alignpt - 2+scrollposition+compositestartpt, 
									 _readingbasepos - 1, 
									 m_WindowPosition.x+leftsize+alignpt + 1 + maxwidth+scrollposition+compositestartpt, 
									 _readingbasepos + (g_XBaseFont->GetHeight() << 2)+1, 
									 0.0f, D3DCOLOR_ARGB(192, 215, 235, 188) );
				}
			}			
		}
	}
	else // MBCS Section
	{
		int strwidth = g_XBaseFont->GetWidth( mbcstextbuffer );
		int leftsize = g_XBaseFont->GetWidth( leftstring );

		if( m_ShowWindowRectangle )
		{		
			if( !m_bWhiteLineRectagle )
			{
				_XDrawRectAngle( m_WindowPosition.x - 2, m_WindowPosition.y-2, m_WindowPosition.x+m_WindowSize.cx+1, 
					m_WindowPosition.y+m_WindowSize.cy+1, 0.0f, D3DCOLOR_ARGB(255,20,20,20) );
			}
			else
			{
				_XDrawRectAngle( m_WindowPosition.x - 2, m_WindowPosition.y-2, m_WindowPosition.x+m_WindowSize.cx+1, 
					m_WindowPosition.y+m_WindowSize.cy+1, 0.0f, D3DCOLOR_ARGB(255,255,255,255) );
			}
		}

		if( m_PasswordEdit )
		{
			TCHAR pwString[_XDEF_MAXPWSTRINGSIZE];
			memset( pwString, '*', _XDEF_MAXPWSTRINGSIZE-1 );
			pwString[_XDEF_MAXPWSTRINGSIZE-1] = NULL;
			//pwString[ strlen( mbcstextbuffer ) ] = NULL;
			pwString[ wcslen( textbuffer ) ] = NULL;

			g_XBaseFont->SetColor(m_TextColor);

			if(m_HorizScrolling)
			{
				int alignpt = 0;
				int scrollpt = 0;

				g_XBaseFont->SetClippingArea( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, m_WindowPosition.y+m_WindowSize.cy+1 );

				strwidth = g_XBaseFont->GetWidth( pwString );
				
				if( strwidth > m_WindowSize.cx )
				{
					scrollpt = -m_ScrollPosition;
				}

				switch( m_AlignType ) 
				{
				default:
				case _XFONT_ALIGNTYPE_LEFT	:
					alignpt = 0;
					g_XBaseFont->Puts( m_WindowPosition.x+scrollpt, m_WindowPosition.y, pwString );
					break;
				case _XFONT_ALIGNTYPE_RIGHT :			
					alignpt = m_WindowSize.cx - strwidth + scrollpt;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pwString );
					break;
				case _XFONT_ALIGNTYPE_CENTER :
					alignpt = (m_WindowSize.cx>>1) - (strwidth>>1) + scrollpt;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pwString );
					break;		
				}		
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						int length = wcslen( leftstring );
						int asterisksize = g_XBaseFont->GetWidth( "*" );
						
						g_XBaseFont->SetColor(m_CaretColor);
						g_XBaseFont->Puts( m_WindowPosition.x+(length*asterisksize)+alignpt+scrollpt, m_WindowPosition.y, _XDEF_CARETTYPE );			
					}		
				}

				g_XBaseFont->Flush();
				g_XBaseFont->DisableClippingArea();
			}
			else
			{		
				int alignpt;
				switch( m_AlignType ) 
				{
				default:
				case _XFONT_ALIGNTYPE_LEFT	:
					alignpt = 0;
					g_XBaseFont->Puts( m_WindowPosition.x, m_WindowPosition.y, pwString );
					break;
				case _XFONT_ALIGNTYPE_RIGHT :			
					alignpt = m_WindowSize.cx - strwidth;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pwString );
					break;
				case _XFONT_ALIGNTYPE_CENTER :
					alignpt = (m_WindowSize.cx>>1) - (strwidth>>1);
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pwString );
					break;		
				}		
								
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						int length = wcslen( leftstring );
						int asterisksize = g_XBaseFont->GetWidth( "*" );

						g_XBaseFont->SetColor(m_CaretColor);
						g_XBaseFont->Puts( m_WindowPosition.x+length*asterisksize+alignpt, m_WindowPosition.y, _XDEF_CARETTYPE );			
					}		
				}
			}

			g_XBaseFont->Flush();
		}
		else if( m_NumericEditMode && m_NumericSeparationMode )
		{
			g_XBaseFont->SetColor(m_TextColor);		

			LPTSTR pOutputTextString = mbcstextbuffer;
			TCHAR numericseparationbuffer[256];
			int nDotcount = 0;
			
			int strlength = strlen( mbcstextbuffer );

			if( strlength > 3 )
			{
				nDotcount = _XMakeCurrencySeparator( numericseparationbuffer, mbcstextbuffer, 3, ',' );
				pOutputTextString = numericseparationbuffer;
			}
			
			int strwidth = g_XBaseFont->GetWidth( pOutputTextString );
			int commawidth = g_XBaseFont->GetWidth( "," );

			if(m_HorizScrolling)
			{
				int alignpt;
							
				int rightlen = wcslen( rightstring );
				int commatotalwidth = 0;
				int rightdotcnt = _XCalcCurrencySeparatorCount( rightstring, 3 );
				
				int leftdotcnt = nDotcount - rightdotcnt;
				if( leftdotcnt > 0 )
				{
					commatotalwidth = leftdotcnt * commawidth;
				}
				else
				{
					commatotalwidth = 0;
				}

				g_XBaseFont->SetClippingArea( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, m_WindowPosition.y+m_WindowSize.cy-1 );

				if( strwidth > m_WindowSize.cx )
				{
					alignpt = -m_ScrollPosition;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pOutputTextString );
				}
				else
				{
					alignpt = 0;
					g_XBaseFont->Puts( m_WindowPosition.x, m_WindowPosition.y, pOutputTextString );
				}
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						g_XBaseFont->SetColor(m_CaretColor);
						//if( strwidth > leftsize+(-alignpt) )
						{
							g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt+commatotalwidth, m_WindowPosition.y, _XDEF_CARETTYPE );
						}
						/*else
						{
							g_XBaseFont->Puts( m_WindowPosition.x+strwidth-8, m_WindowPosition.y, _XDEF_CARETTYPE );
						}*/
					}
				}
		
				g_XBaseFont->Flush();
				g_XBaseFont->DisableClippingArea();
			}
			else
			{
				int rightlen = wcslen( rightstring );
				int commatotalwidth = 0;
				int rightdotcnt = _XCalcCurrencySeparatorCount( rightstring, 3 );
 
				int leftdotcnt = nDotcount - rightdotcnt;
				if( leftdotcnt > 0 )
				{
					commatotalwidth = leftdotcnt * commawidth;
				}
				else
				{
					commatotalwidth = 0;
				}
				
				int alignpt;
				switch( m_AlignType ) 
				{
				default:
				case _XFONT_ALIGNTYPE_LEFT	:
					alignpt = 0;
					g_XBaseFont->Puts( m_WindowPosition.x, m_WindowPosition.y, pOutputTextString );
					break;
				case _XFONT_ALIGNTYPE_RIGHT :			
					alignpt = m_WindowSize.cx - strwidth;
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pOutputTextString );			
					break;
				case _XFONT_ALIGNTYPE_CENTER :
					alignpt = (m_WindowSize.cx>>1) - (strwidth>>1);			
					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, pOutputTextString );
					break;
				}
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						g_XBaseFont->SetColor(m_CaretColor);
						g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt+commatotalwidth, m_WindowPosition.y, _XDEF_CARETTYPE );
					}
				}

				g_XBaseFont->Flush();
			}		
		}
		else
		{	
			g_XBaseFont->SetColor(m_TextColor);		

			if(m_HorizScrolling)
			{
				int alignpt = 0;
				
				g_XBaseFont->SetClippingArea( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x+m_WindowSize.cx-1, m_WindowPosition.y+m_WindowSize.cy+1 );

				if( strwidth > m_WindowSize.cx )
				{
					alignpt = -m_ScrollPosition;

					int compsize = g_XBaseFont->GetWidth( m_comText );
					if( compsize )
					{
						int startpt = m_WindowPosition.x+alignpt+leftsize;
						int endpt	= m_WindowPosition.x+alignpt+leftsize+compsize;
						if( startpt < m_WindowPosition.x ) startpt = m_WindowPosition.x;
						if( endpt > m_WindowPosition.x+m_WindowSize.cx-1 ) endpt = m_WindowPosition.x+m_WindowSize.cx-1;

						_XDrawSolidBar( startpt, m_WindowPosition.y, endpt, m_WindowPosition.y+g_XBaseFont->GetHeight(), 0xFF7F7FFF );
					}

					g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, mbcstextbuffer );
				}
				else
				{
					alignpt = 0;

					int compsize = g_XBaseFont->GetWidth( m_comText );
					if( compsize )
					{
						int startpt = m_WindowPosition.x+alignpt+leftsize;
						int endpt	= m_WindowPosition.x+alignpt+leftsize+compsize;
						if( startpt < m_WindowPosition.x ) startpt = m_WindowPosition.x;
						if( endpt > m_WindowPosition.x+m_WindowSize.cx-1 ) endpt = m_WindowPosition.x+m_WindowSize.cx-1;
						
						_XDrawSolidBar( startpt, m_WindowPosition.y, endpt, m_WindowPosition.y+g_XBaseFont->GetHeight(), 0xFF7F7FFF );					
					}

					g_XBaseFont->Puts( m_WindowPosition.x, m_WindowPosition.y, mbcstextbuffer );
				}
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						g_XBaseFont->SetColor(m_CaretColor);
						//if( strwidth > leftsize+(-alignpt) )
						{
							g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt, m_WindowPosition.y, _XDEF_CARETTYPE );
						}
						/*else
						{
							g_XBaseFont->Puts( m_WindowPosition.x+strwidth-8, m_WindowPosition.y, _XDEF_CARETTYPE );
						}*/
					}
				}
		
				g_XBaseFont->Flush();
				g_XBaseFont->DisableClippingArea();
			}
			else
			{			
				int alignpt;
				switch( m_AlignType ) 
				{
				default:
				case _XFONT_ALIGNTYPE_LEFT	:
					alignpt = 0;
					break;
				case _XFONT_ALIGNTYPE_RIGHT :			
					alignpt = m_WindowSize.cx - strwidth;
					break;
				case _XFONT_ALIGNTYPE_CENTER :
					alignpt = (m_WindowSize.cx>>1) - (strwidth>>1);
					break;
				}
				
				int compsize = g_XBaseFont->GetWidth( m_comText );
				if( compsize )
				{
					int startpt = m_WindowPosition.x+alignpt+leftsize;
					int endpt	= m_WindowPosition.x+alignpt+leftsize+compsize;
					if( startpt < m_WindowPosition.x ) startpt = m_WindowPosition.x;
					if( endpt > m_WindowPosition.x+m_WindowSize.cx-1 ) endpt = m_WindowPosition.x+m_WindowSize.cx-1;
					
					_XDrawSolidBar( startpt, m_WindowPosition.y, endpt, m_WindowPosition.y+g_XBaseFont->GetHeight(), 0xFF7F7FFF );
				}

				g_XBaseFont->Puts( m_WindowPosition.x+alignpt, m_WindowPosition.y, mbcstextbuffer );
				
				if( m_ShowWindow || !m_NoFocusedDraw )
				{
					if( (gnFPS>>1) < gnFrameCounter && _XIMEKERNEL.GetIMEContainer() == this )
					{
						g_XBaseFont->SetColor(m_CaretColor);
						g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt, m_WindowPosition.y, _XDEF_CARETTYPE );
					}
				}

				g_XBaseFont->Flush();
			}
		}	

		// draw reading string...
		if( _XIMEKERNEL.GetIMEContainer() == this )
		{
			int scrollposition = 0;

			if(m_HorizScrolling)
			{
				if( strwidth > m_WindowSize.cx )
				{
					scrollposition = -m_ScrollPosition;
				}
			}
					
			int alignpt;
			switch( m_AlignType ) 
			{
			default:
			case _XFONT_ALIGNTYPE_LEFT	:
				alignpt = 0;
				break;
			case _XFONT_ALIGNTYPE_RIGHT :			
				alignpt = m_WindowSize.cx - strwidth;
				break;
			case _XFONT_ALIGNTYPE_CENTER :
				alignpt = (m_WindowSize.cx>>1) - (strwidth>>1);
				break;
			}

			TCHAR text[256];
			int textLen = _XIMEKERNEL.GetReading(text, sizeof(text));

			if( textLen > 0 )
			{
				int compositestart = _XIMEKERNEL.m_ulStart;
				int compositeend = _XIMEKERNEL.m_ulEnd;

				int compositestartpt = 0;
				int compositeendpt = 0;
				
				if( compositestart != compositeend )
				{
					wchar_t starttext[256];
					wchar_t endtext[256];
					memset( starttext,0, sizeof(wchar_t) * 256 );
					memset( endtext,0, sizeof(wchar_t) * 256 );

					wcsncpy( starttext, _XIMEKERNEL.m_comp.c_str(), compositestart);
					wcsncpy( endtext, _XIMEKERNEL.m_comp.c_str(), compositeend);

					compositestartpt	= g_XBaseFont->GetWidth( starttext );
					compositeendpt	= g_XBaseFont->GetWidth( endtext );

					if( compositestartpt != compositeendpt )
					{
						_XDrawLine2D( m_WindowPosition.x+leftsize+alignpt+scrollposition + compositestartpt, m_WindowPosition.y + g_XBaseFont->GetHeight(), 
									  m_WindowPosition.x+leftsize+alignpt+scrollposition + compositeendpt, m_WindowPosition.y + g_XBaseFont->GetHeight(), 0xFFFF1010, 0xFFFF1010 );
					}
				}

				if(!_XIMEKERNEL.IsVerticalReading()) 
				{
					int _readingbasepos = 0;
					
					if( m_WindowPosition.y + (g_XBaseFont->GetHeight()<<1) + 4 < gnHeight )
					{
						_readingbasepos = m_WindowPosition.y + g_XBaseFont->GetHeight() + 2;
					}
					else
					{
						_readingbasepos = m_WindowPosition.y - ( 2 + g_XBaseFont->GetHeight() );
					}

					g_XBaseFont->SetColor(D3DCOLOR_ARGB( 255, 215, 235, 188 ));
					g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt+scrollposition+compositestartpt, _readingbasepos, text );
					g_XBaseFont->Flush();

					int _readingwidth = g_XBaseFont->GetWidth( text );

					_XDrawSolidBar( m_WindowPosition.x+leftsize+alignpt - 2+scrollposition+compositestartpt, 
									_readingbasepos-1,
									m_WindowPosition.x+leftsize+alignpt + 1 + _readingwidth+scrollposition+compositestartpt, 
									_readingbasepos + g_XBaseFont->GetHeight() + 1,
									0x40303030 );
					_XDrawRectAngle( m_WindowPosition.x+leftsize+alignpt - 2+scrollposition+compositestartpt, 
									 _readingbasepos-1,
									 m_WindowPosition.x+leftsize+alignpt + 1 + _readingwidth+scrollposition+compositestartpt, 
									 _readingbasepos + g_XBaseFont->GetHeight() + 1,
									 0.0f, D3DCOLOR_ARGB(192, 215, 235, 188) );
				} 
				else 
				{
					int _readingbasepos = 0;
					
					if( m_WindowPosition.y + g_XBaseFont->GetHeight() + 6 + (g_XBaseFont->GetHeight() * 4) < gnHeight )
					{
						_readingbasepos = m_WindowPosition.y + g_XBaseFont->GetHeight() + 3;
					}
					else
					{
						_readingbasepos = m_WindowPosition.y - ( 3 + (g_XBaseFont->GetHeight() * 4) );
					}

					TCHAR* _begin = text;
					TCHAR* _end = text + textLen;
					TCHAR  _outputtext[5];				
					
					g_XBaseFont->SetColor(D3DCOLOR_ARGB( 255, 215, 235, 188));
					
					int maxwidth = 0;
					int textpos = _readingbasepos;
					int i = 0  ; for( i=0; i<4 && _begin<_end; ++i) 
					{
						TCHAR* next = CharNextExA(_XIMEKERNEL.GetCodePage(), _begin, 0);
						
						memset( _outputtext, 0, sizeof(TCHAR) * 5 );
						strncpy( _outputtext, _begin, next-_begin );
					
						g_XBaseFont->Puts( m_WindowPosition.x+leftsize+alignpt+scrollposition+compositestartpt, textpos, _outputtext );
						int width = g_XBaseFont->GetWidth( _outputtext );
						if( width > maxwidth )
						{
							maxwidth = width;
						}
						
						textpos += g_XBaseFont->GetHeight();
						
						_begin = next;
					}

					g_XBaseFont->Flush();

					_XDrawSolidBar( m_WindowPosition.x+leftsize+alignpt - 2+scrollposition+compositestartpt, 
									_readingbasepos - 1, 
									m_WindowPosition.x+leftsize+alignpt + 1 + maxwidth+scrollposition+compositestartpt, 
									_readingbasepos + (g_XBaseFont->GetHeight() << 2)+1, 
									0x30303030 );

					_XDrawRectAngle( m_WindowPosition.x+leftsize+alignpt - 2+scrollposition+compositestartpt, 
									 _readingbasepos - 1, 
									 m_WindowPosition.x+leftsize+alignpt + 1 + maxwidth+scrollposition, 
									 _readingbasepos + (g_XBaseFont->GetHeight() << 2)+1, 
									 0.0f, D3DCOLOR_ARGB(192, 215, 235, 188) );
				}
			}
		}
	} // MBCS Section
}

BOOL _XIMEContainer::CheckMousePosition( void )
{
	if( !m_ShowWindow ) return FALSE;
	
	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
	
	if( scrnpos )
	{	
		return gpInput->CheckMousePos( m_WindowPosition.x, m_WindowPosition.y, m_WindowPosition.x + m_WindowSize.cx, m_WindowPosition.y + m_WindowSize.cy );
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// _XIMM
//////////////////////////////////////////////////////////////////////

INPUTCONTEXT* (WINAPI * _XIMM::_ImmLockIMC)( HIMC ) = _XIMM::Dummy_ImmLockIMC;
BOOL (WINAPI * _XIMM::_ImmUnlockIMC)( HIMC ) = _XIMM::Dummy_ImmUnlockIMC;
LPVOID (WINAPI * _XIMM::_ImmLockIMCC)( HIMCC ) = _XIMM::Dummy_ImmLockIMCC;
BOOL (WINAPI * _XIMM::_ImmUnlockIMCC)( HIMCC ) = _XIMM::Dummy_ImmUnlockIMCC;
BOOL (WINAPI * _XIMM::_ImmDisableTextFrameService)( DWORD ) = _XIMM::Dummy_ImmDisableTextFrameService;
LONG (WINAPI * _XIMM::_ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD ) = _XIMM::Dummy_ImmGetCompositionStringW;
DWORD (WINAPI * _XIMM::_ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD ) = _XIMM::Dummy_ImmGetCandidateListW;
HIMC (WINAPI * _XIMM::_ImmGetContext)( HWND ) = _XIMM::Dummy_ImmGetContext;
BOOL (WINAPI * _XIMM::_ImmReleaseContext)( HWND, HIMC ) = _XIMM::Dummy_ImmReleaseContext;
HIMC (WINAPI * _XIMM::_ImmAssociateContext)( HWND, HIMC ) = _XIMM::Dummy_ImmAssociateContext;
BOOL (WINAPI * _XIMM::_ImmGetOpenStatus)( HIMC ) = _XIMM::Dummy_ImmGetOpenStatus;
BOOL (WINAPI * _XIMM::_ImmSetOpenStatus)( HIMC, BOOL ) = _XIMM::Dummy_ImmSetOpenStatus;
BOOL (WINAPI * _XIMM::_ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD ) = _XIMM::Dummy_ImmGetConversionStatus;
HWND (WINAPI * _XIMM::_ImmGetDefaultIMEWnd)( HWND ) = _XIMM::Dummy_ImmGetDefaultIMEWnd;
UINT (WINAPI * _XIMM::_ImmGetIMEFileNameA)( HKL, LPSTR, UINT ) = _XIMM::Dummy_ImmGetIMEFileNameA;
UINT (WINAPI * _XIMM::_ImmGetVirtualKey)( HWND ) = _XIMM::Dummy_ImmGetVirtualKey;
BOOL (WINAPI * _XIMM::_ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD ) = _XIMM::Dummy_ImmNotifyIME;
BOOL (WINAPI * _XIMM::_ImmSetConversionStatus)( HIMC, DWORD, DWORD ) = _XIMM::Dummy_ImmSetConversionStatus;
BOOL (WINAPI * _XIMM::_ImmSimulateHotKey)( HWND, DWORD ) = _XIMM::Dummy_ImmSimulateHotKey;
BOOL (WINAPI * _XIMM::_ImmIsIME)( HKL ) = _XIMM::Dummy_ImmIsIME;
UINT (WINAPI * _XIMM::_ImmGetDescriptionW)( HKL, LPWSTR, UINT ) = _XIMM::Dummy_ImmGetDescriptionW;

UINT (WINAPI * _XIMM::_GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT ) = _XIMM::Dummy_GetReadingString; // Traditional Chinese IME
BOOL (WINAPI * _XIMM::_ShowReadingWindow)( HIMC, BOOL ) = _XIMM::Dummy_ShowReadingWindow; // Traditional Chinese IME

_XIMM::_XIMM() 
{
	m_hDllImm32 =	NULL;
	m_hDllIme	=	NULL;
}

_XIMM::~_XIMM()
{ 
	if(m_hDllImm32) 
	{	
        PLACEHOLDERPROC( ImmLockIMC );
        PLACEHOLDERPROC( ImmUnlockIMC );
        PLACEHOLDERPROC( ImmLockIMCC );
        PLACEHOLDERPROC( ImmUnlockIMCC );
        PLACEHOLDERPROC( ImmDisableTextFrameService );
        PLACEHOLDERPROC( ImmGetCompositionStringW );
        PLACEHOLDERPROC( ImmGetCandidateListW );
        PLACEHOLDERPROC( ImmGetContext );
        PLACEHOLDERPROC( ImmReleaseContext );
        PLACEHOLDERPROC( ImmAssociateContext );
        PLACEHOLDERPROC( ImmGetOpenStatus );
        PLACEHOLDERPROC( ImmSetOpenStatus );
        PLACEHOLDERPROC( ImmGetConversionStatus );
        PLACEHOLDERPROC( ImmGetDefaultIMEWnd );
        PLACEHOLDERPROC( ImmGetIMEFileNameA );
        PLACEHOLDERPROC( ImmGetVirtualKey );
        PLACEHOLDERPROC( ImmNotifyIME );
        PLACEHOLDERPROC( ImmSetConversionStatus );
        PLACEHOLDERPROC( ImmSimulateHotKey );
        PLACEHOLDERPROC( ImmIsIME );
		PLACEHOLDERPROC( ImmGetDescriptionW );

        FreeLibrary( m_hDllImm32 );
        m_hDllImm32 = NULL;
	}

	if( m_hDllIme )
    {
        PLACEHOLDERPROC( GetReadingString );
        PLACEHOLDERPROC( ShowReadingWindow );

        FreeLibrary( m_hDllIme );
        m_hDllIme = NULL;
    }
}

void _XIMM::Initialize()
{
	if( m_hDllImm32 ) // Only need to do once
        return;

    char	szPath[MAX_PATH+1];
    memset( szPath, 0, MAX_PATH+1 );
	FARPROC Temp;
	
	if( GetSystemDirectoryA( szPath, MAX_PATH+1 ) ) 
	{
		strcat( szPath, "\\imm32.dll" );
		m_hDllImm32 = LoadLibraryA( szPath );
		if( m_hDllImm32 )
		{
			GETPROCADDRESS( m_hDllImm32, ImmLockIMC, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmUnlockIMC, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmLockIMCC, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmUnlockIMCC, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmDisableTextFrameService, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmGetCompositionStringW, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmGetCandidateListW, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmGetContext, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmReleaseContext, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmAssociateContext, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmGetOpenStatus, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmSetOpenStatus, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmGetConversionStatus, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmGetDefaultIMEWnd, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmGetIMEFileNameA, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmGetVirtualKey, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmNotifyIME, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmSetConversionStatus, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmSimulateHotKey, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmIsIME, Temp );
			GETPROCADDRESS( m_hDllImm32, ImmGetDescriptionW, Temp );

			_ImmDisableTextFrameService( (DWORD)-1 );
		}
	}

	gs_hImcDef = _ImmGetContext( gHWnd );
    _ImmReleaseContext( gHWnd, gs_hImcDef );	
}

INPUTCONTEXT* _XIMM::LockIMC(HIMC hImc)
{
	if(_ImmLockIMC == NULL) 
	{
		return NULL;
	} 
	else 
	{
		return _ImmLockIMC(hImc);
	}
}

BOOL _XIMM::UnlockIMC(HIMC hImc)
{
	if(_ImmUnlockIMC == NULL) 
	{
		return FALSE;
	} 
	else 
	{
		return _ImmUnlockIMC(hImc);
	}
}

LPVOID _XIMM::LockIMCC( HIMCC himcc )
{
	if(_ImmLockIMCC == NULL) 
	{
		return NULL;
	} 
	else 
	{
		return _ImmLockIMCC(himcc);
	}
}

BOOL _XIMM::UnlockIMCC( HIMCC himcc )
{
	if(_ImmUnlockIMCC == NULL) 
	{
		return NULL;
	} else 
	{
		return _ImmUnlockIMCC(himcc);
	}
}

//////////////////////////////////////////////////////////////////////
// _XIMEKernel
//////////////////////////////////////////////////////////////////////

_XIMEKernel * _XIMEKernel::ms_pThis;

_XIMEKernel& _XIMEKernel::GetInstance( void )
{
	assert(ms_pThis!=NULL && "_XIMEKernel :: not initialized ime kernel");
	return *ms_pThis;
}

_XIMEKernel::_XIMEKernel()
{
	ms_pThis			=	this;
	m_pIMEContainer		=	NULL;	
	m_ulStart			=	0;
	m_ulEnd				=	0;	
	m_ImeState			=	IMEUI_STATE_OFF;
	m_wszCurrIndicator	=	g_aszIndicator[INDICATOR_NON_IME];
	m_dwId[0]			=	0;
	m_dwId[1]			=	0;
	m_hkl				=	0;
	
	memset( m_IMEDescription, 0, sizeof(wchar_t) * 128 );		
	ResetCombinationBuffer();
}

_XIMEKernel::~_XIMEKernel()
{	
}

_XIMEContainer* _XIMEKernel::GetIMEContainer( void )
{
	return m_pIMEContainer;
}

bool _XIMEKernel::GetKey(WPARAM wparam, LPARAM lparam)
{
	if( !m_pIMEContainer ) return false;
	
	switch (wparam){
	case VK_LEFT:
		m_pIMEContainer->ProcessLeftKey();
		if( m_pIMEContainer->GetNewLine() )
		{
			if( m_pIMEContainer->GetLeftArrowKeyCallBack() )
			{
				m_pIMEContainer->GetLeftArrowKeyCallBack()(m_pIMEContainer);
			}
			m_pIMEContainer->SetNewLine( FALSE );
		}
		
		return true;
		
	case VK_RIGHT:
		m_pIMEContainer->ProcessRightKey();
		if( m_pIMEContainer->GetNewLine() )
		{
			if( m_pIMEContainer->GetRightArrowKeyCallBack() )
			{
				m_pIMEContainer->GetRightArrowKeyCallBack()(m_pIMEContainer);
			}
			m_pIMEContainer->SetNewLine( FALSE );
		}
		return true;
		
	case VK_DELETE:
		m_pIMEContainer->ProcessDelKey();
		return true;
		
	case VK_HOME:
		m_pIMEContainer->ProcessHomeKey();
		return true;
		
	case VK_END:
		m_pIMEContainer->ProcessEndKey();
		return true;
		
	case VK_RETURN:  
		if( m_pIMEContainer->GetReturnKeyCallBack() )
		{
			m_pIMEContainer->GetReturnKeyCallBack()(m_pIMEContainer);
		}
		return true;
		
	case VK_UP:  
		if( m_pIMEContainer->GetUpArrowKeyCallBack() )
		{
			m_pIMEContainer->GetUpArrowKeyCallBack()(m_pIMEContainer);
		}
		return true;
	case VK_DOWN:  
		if( m_pIMEContainer->GetDownArrowKeyCallBack() )
		{
			m_pIMEContainer->GetDownArrowKeyCallBack()(m_pIMEContainer);
		}
		return true;
		
	case VK_ESCAPE:
		if( m_pIMEContainer->GetEscapeKeyCallBack() )
		{
			m_pIMEContainer->GetEscapeKeyCallBack()(m_pIMEContainer);
		}
		return true;

	case VK_CAPITAL :
		if( m_pIMEContainer->GetCapsLockKeyCallBack() )
		{
			m_pIMEContainer->GetCapsLockKeyCallBack()(m_pIMEContainer);
		}
		return true;

	case VK_PROCESSKEY:
		return false;
		//return true;
	} 
	
	return false;
}

void _XIMEKernel::SetFocus(_XIMEContainer* imecontainer)
{
	BOOL samefocus = (m_pIMEContainer == imecontainer) ? TRUE : FALSE;
	
	_XIMEContainer* previmecontainer = m_pIMEContainer;

	m_pIMEContainer = imecontainer;
	if(m_pIMEContainer)
	{
		if( !samefocus )
		{
			//* win98 error
			/*
			HIMC m_hIMC = g_imm._ImmGetContext(gHWnd);
			if(m_hIMC) 
			{
				//if( g_imm._ImmNotifyIME )
				//	g_imm._ImmNotifyIME(m_hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);				
				g_imm._ImmReleaseContext(gHWnd, m_hIMC);
			}
			*/

			if(g_imm._ImmAssociateContext)
				g_imm._ImmAssociateContext( gHWnd, gs_hImcDef );
			
			/*CheckToggleState(gHWnd);			
			HIMC hImc;
			if( NULL != ( hImc = g_imm._ImmGetContext( gHWnd ) ) ) 
			{
				m_ImeState = IMEUI_STATE_OFF;

				g_imm._ImmReleaseContext( gHWnd, hImc );
				CheckToggleState(gHWnd);
			}
			*/
						
			if( m_pIMEContainer )
				m_pIMEContainer->SetComText(L"",0);

			if( previmecontainer )
				previmecontainer->SetComText(L"",0);
			
			m_comp.resize(0);
		}

		m_pIMEContainer->ShowWindow(TRUE);
	}
	else
	{
		g_IMEWindow.ShowWindow( FALSE );

		//* win98 error
		/*
		HIMC m_hIMC = g_imm._ImmGetContext(gHWnd);
		if(m_hIMC) 
		{
			//if( g_imm._ImmNotifyIME )
			//	g_imm._ImmNotifyIME(m_hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);

			g_imm._ImmReleaseContext(gHWnd, m_hIMC);
		}
		*/

		if(g_imm._ImmAssociateContext)
			g_imm._ImmAssociateContext( gHWnd, NULL );
		
		if( previmecontainer )
			previmecontainer->SetComText(L"",0);

		m_comp.resize(0);
	}
}

void _XIMEKernel::ResetCombinationBuffer( void )
{
	//* win98 error
	HIMC m_hIMC = g_imm._ImmGetContext(gHWnd);
	if(m_hIMC) 
	{
		//if( g_imm._ImmNotifyIME )
		//	g_imm._ImmNotifyIME(m_hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
		
		g_imm._ImmReleaseContext(gHWnd, m_hIMC);		
	}	
		
	if( m_pIMEContainer )
		m_pIMEContainer->SetComText(L"",0);

	m_comp.resize(0);
}



//////////////////////////////////////////////////////////////////////////
// CJK Method

void _XIMEKernel::OnInputLanguageChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	DXUTTRACE( "WM_INPUTLANGCHANGE\n" );

	m_hkl = (HKL)lParam;
	m_langId = LOWORD(m_hkl);
	m_codePage = GetCodePageFromLang(m_langId);
	//m_input.resize(0);
	//_XLog("IME CP : %d",m_codePage);
	
	/* Check Property */ 
	DWORD property = ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);

	m_bUnicodeIME = (property & IME_PROP_UNICODE) ? true : false;

	/* Update Indicator */ 
	CheckToggleState(hWnd);

	/* Bind Proc */ 
	SetupImeApi(hWnd);

	/* Update m_dwId[] */ 
	GetImeId();
	
	if( m_pIMEContainer )
		m_pIMEContainer->SetComText(L"",0);

	m_comp.resize(0);
}

bool _XIMEKernel::OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc;
	if(lParam&GCS_COMPSTR) 
	{
		DXUTTRACE( "  GCS_COMPSTR\n" );

		hImc = g_imm._ImmGetContext(hWnd);

		if(hImc) 
		{ 
			if( m_pIMEContainer )
			{
				int tempSize = g_imm._ImmGetCompositionStringW(hImc, GCS_COMPSTR, NULL, 0);
				
				wchar_t* temp = (wchar_t*)alloca(tempSize);
				
				g_imm._ImmGetCompositionStringW(hImc, GCS_COMPSTR, temp, tempSize);
				
				m_comp.assign(temp, temp+tempSize/sizeof(wchar_t));					
				
				
				m_pIMEContainer->SetComText(m_comp.c_str(), m_comp.size());
			}

			if( m_pIMEContainer && m_pIMEContainer->GetNumericEditMode() )
			{
				if( g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
				{
					if( g_imm._ImmNotifyIME )
						g_imm._ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
				}			
			}

			g_imm._ImmReleaseContext(hWnd, hImc);
		}
	}
	if(lParam&GCS_RESULTSTR) 
	{
		DXUTTRACE( "  GCS_RESULTSTR\n" );

		hImc = g_imm._ImmGetContext(hWnd);
		if(hImc) 
		{
			int tempSize = g_imm._ImmGetCompositionStringW(hImc, GCS_RESULTSTR, NULL, 0);			
			wchar_t compresultbuffer[128];
			memset( compresultbuffer, 0, sizeof(wchar_t) * 128 );			
			g_imm._ImmGetCompositionStringW(hImc, GCS_RESULTSTR, compresultbuffer, tempSize);
			
			if( m_pIMEContainer )
			{
				m_pIMEContainer->AddUnicodeString(compresultbuffer, wcslen(compresultbuffer) );

				//m_pIMEContainer->SetComText("",0);											// clear composition buffer...
				//m_comp.resize(0);

				g_IMEWindow.ShowWindow( FALSE );
			}

			g_imm._ImmReleaseContext(hWnd, hImc);
		}
	}
	if(lParam&GCS_COMPATTR) 
	{
		DXUTTRACE( "	GCS_COMPATTR\n" );
		hImc = g_imm._ImmGetContext(hWnd);
		if(hImc) 
		{ 

			int tempSize = g_imm._ImmGetCompositionStringW(hImc, GCS_COMPATTR, NULL, 0);

			BYTE* temp = (BYTE*)alloca(tempSize);

			g_imm._ImmGetCompositionStringW(hImc, GCS_COMPATTR, temp, tempSize);

			int start=0;for(; start<tempSize; ++start) if(temp[start]==ATTR_TARGET_CONVERTED || temp[start]==ATTR_TARGET_NOTCONVERTED) break;
			int end=start;for(; end<tempSize; ++end) if(temp[end]!=temp[start]) break;

			m_ulStart	= start;
			m_ulEnd		= end;

			g_imm._ImmReleaseContext(hWnd, hImc);
		}
	}

	return true;
}

bool _XIMEKernel::OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( m_pIMEContainer )
	{
		m_pIMEContainer->SetComText(L"",0);											// clear composition buffer...
	}

	m_comp.resize(0);
	m_ulStart = m_ulEnd = 0;
	m_reading.resize(0);

	g_IMEWindow.ShowWindow( FALSE );

	return true;
}

bool _XIMEKernel::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc = NULL;

	switch (wParam) 
	{
	case IMN_OPENCANDIDATE:
	case IMN_CHANGECANDIDATE:
	{
		DXUTTRACE( wParam == IMN_CHANGECANDIDATE ? "  IMN_CHANGECANDIDATE\n" : "  IMN_OPENCANDIDATE\n" );

		hImc = g_imm._ImmGetContext(hWnd);
		if(hImc) 
		{
			m_reading.resize(0);

			int candidateLen = g_imm._ImmGetCandidateListW(hImc, 0, NULL, 0);

			if(candidateLen > 0) 
			{
				m_candidate.resize(candidateLen*2);

				LPCANDIDATELIST lpCandList = (CANDIDATELIST*)&m_candidate[0];

				g_imm._ImmGetCandidateListW(hImc, 0, (CANDIDATELIST*)lpCandList, candidateLen);

				if(g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
				{
					DWORD dwRealPage = ( lpCandList->dwSelection / lpCandList->dwPageSize ) * lpCandList->dwPageSize; 

					//if( lpCandList->dwSelection >= lpCandList->dwPageSize )
					{
						if( lpCandList->dwPageStart != dwRealPage ) 
						{
							g_imm._ImmNotifyIME( hImc, NI_SETCANDIDATE_PAGESTART, 0, dwRealPage ); 
						}
					}
				}
				
				if(m_pIMEContainer)
				{
					//m_pIMEContainer->ResetCandleList();
					//m_pIMEContainer->SetCandleNum(0);
					//m_pIMEContainer->SetCandleLength(candidateLen);
					
					TCHAR text[1024];
					TCHAR textbuffer[1024];

					int count = min(GetCandidateCount(), GetCandidatePageStart()+GetCandidatePageSize());

					if( g_IMEWindow.m_pListBox )
					{
						g_IMEWindow.m_pListBox->DeleteAllItem();

						int candstart = GetCandidatePageStart();
						int index = 1;
						
						/*
						DWORD dwRealPage = ( lpCandList->dwSelection / lpCandList->dwPageSize ) * lpCandList->dwPageSize; 

						if( lpCandList->dwSelection >= lpCandList->dwPageSize )
						{
							if( lpCandList->dwPageStart != dwRealPage )
							{
								g_imm._ImmNotifyIME( hImc, NI_SETCANDIDATE_PAGESTART, 0, dwRealPage );
							}
						}
						*/

						char szImeFile[MAX_PATH + 1];
						int bTaiwaneseIMEType = 0;
						memset( szImeFile, 0, sizeof(char) * (MAX_PATH + 1) );
						g_imm._ImmGetIMEFileNameA( m_hkl, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 );
						if( strcmpi( szImeFile, "dayi.ime" ) == 0 )
						{
							bTaiwaneseIMEType = 1;
						}
						else if( strcmpi( szImeFile, "winar30.ime" ) == 0 )
						{
							bTaiwaneseIMEType = 2;
						}

						int i = 0 ; for(  i = candstart; i < count; ++i )
						{	
							if( g_LanguageType == _XLANGUAGE_TYPE_TAIWANESE || g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
							{
								memset( text, 0, sizeof(TCHAR) * 1024 );
								memset( textbuffer, 0, sizeof(TCHAR) * 1024 );
								int textLen = GetCandidate(i, text, sizeof(text));
								if( textLen > 0 )
								{
									wchar_t tempbuffer[256];

									if( bTaiwaneseIMEType == 1 ) // DaYi
									{
										swprintf( tempbuffer, L"%d  %s", index-1, (wchar_t*)text );
									}
									else if( bTaiwaneseIMEType == 2 ) // Array
									{
										if( index == 10 )
										{
											swprintf( tempbuffer, L"0  %s", (wchar_t*)text );
										}
										else
										{
											swprintf( tempbuffer, L"%d  %s", index, (wchar_t*)text );
										}
									}
									else
									{
										if( index == 10 )
										{
											swprintf( tempbuffer, L"0  %s", (wchar_t*)text );
										}
										else
										{
											swprintf( tempbuffer, L"%d  %s", index, (wchar_t*)text );
										}
									}

									memcpy( textbuffer, tempbuffer, sizeof(wchar_t) * wcslen(tempbuffer) );

									g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_DEFAULT );
									g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_DEFAULT_HIGHLIGHT );

									DWORD imecmdid = (GetCandidatePageStart() /* GetCandidatePageSize()*/)<<16 | index;

									g_IMEWindow.m_pListBox->SetItemCommandID( index-1, imecmdid );
								}
								else
								{
									wchar_t tempbuffer[256];

									if( GetCandidatePageSize() == 10 ) // DaYi
									{
										swprintf( tempbuffer, L"%d  %s", index-1, (wchar_t*)text );
									}
									else
									{
										if( index == 10 )
										{
											swprintf( tempbuffer, L"0", index );
										}
										else
										{
											swprintf( tempbuffer, L"%d", index );
										}
									}

									memcpy( textbuffer, tempbuffer, sizeof(wchar_t) * wcslen(tempbuffer) );
									g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_DEFAULT );
									g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_DEFAULT_HIGHLIGHT );
									g_IMEWindow.m_pListBox->SetItemCommandID( index-1, index );
								}
							}
							else
							{
								memset( text, 0, sizeof(TCHAR) * 1024 );
								int textLen = GetCandidate(i, text, sizeof(text));
								if( textLen > 0 )
								{						
									text[textLen] = NULL;

									sprintf( textbuffer, _T("%d  %s"), index, text );
									g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_DEFAULT );
									g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_DEFAULT_HIGHLIGHT );
									g_IMEWindow.m_pListBox->SetItemCommandID( index-1, index );
								}
								else
								{
									sprintf( textbuffer, _T("%d"), index );
									g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_DEFAULT );
									g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_DEFAULT_HIGHLIGHT );
									g_IMEWindow.m_pListBox->SetItemCommandID( index-1, index );
								}
							}

							index++;
						}

						/*
						int i = 0 ; for(  i = candstart; i < count; ++i )
						{							
							memset( text, 0, sizeof(TCHAR) * 1024 );
							int textLen = GetCandidate(i, text, sizeof(text));
							if( textLen > 0 )
							{						
								text[textLen] = NULL;
								sprintf( textbuffer, _T("%d  %s"), index, text );

								_XFontManager_Baseclass* pXFont = _XGetFont( g_IMEWindow.m_pListBox->GetFontID() );
								if( pXFont )
								{
									if( textLen >= 2 )
									{
										unsigned short _code = 0;
										_code = (text[0]<<8) | (unsigned char)text[1];
										
										if( pXFont->IsValidCharacter( _code ) )
										{
											g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_DEFAULT );
											g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_DEFAULT_HIGHLIGHT );
										}
										else
										{
											sprintf( textbuffer, _T("%d  %s"), index, _XGETINTERFACETEXT( 3163 ) );
											g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_WARNING );
											g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_WARNING_HIGHLIGHT );
										}
									}
									else
									{
										unsigned short _code = (unsigned char)text[0];
										
										if( pXFont->IsValidCharacter( _code ) )
										{
											g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_DEFAULT );
											g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_DEFAULT_HIGHLIGHT );
										}
										else
										{
											sprintf( textbuffer, _T("%d  %s"), index, _XGETINTERFACETEXT( 3163 ) );
											g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_WARNING );
											g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_WARNING_HIGHLIGHT );
										}
									}
								}
								else
								{
									sprintf( textbuffer, _T("%d  %s"), index, _XGETINTERFACETEXT( 3163 ) );
									g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_WARNING );
									g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_WARNING_HIGHLIGHT );
								}
							}
							else
							{
								sprintf( textbuffer, _T("%d"), index );
								g_IMEWindow.m_pListBox->InsertListItem( textbuffer, _XSC_DEFAULT );
								g_IMEWindow.m_pListBox->SetItemAttribHighlight( index - 1, 0, _XSC_DEFAULT_HIGHLIGHT );								
							}

							g_IMEWindow.m_pListBox->SetItemCommandID( index-1, index );

							index++;
						}
						*/

						if( GetCandidateSelection()-GetCandidatePageStart() >= 0 )
						{
							g_IMEWindow.m_pListBox->SelectItem( GetCandidateSelection()-GetCandidatePageStart() );
						}
					}
					
					POINT pt = m_pIMEContainer->GetWindowPosition();
					SIZE  sz = m_pIMEContainer->GetWindowSize();
					g_IMEWindow.MoveWindow( pt.x + sz.cx, pt.y );
					g_IMEWindow.ShowWindow( TRUE );
					
					
					pt.x = 3200;//gnWidth;
					pt.y = 3200;//gnHeight;
					//g_imm._ImmSetStatusWindowPos(hImc, &pt);
					
					COMPOSITIONFORM CompForm;					
					CompForm.dwStyle = CFS_POINT;
					CompForm.ptCurrentPos.x = 3200;
					CompForm.ptCurrentPos.y = 3200;
					//g_imm._ImmSetCompositionWindow(hImc, &CompForm);
					
					CANDIDATEFORM	CandForm;
					CandForm.dwIndex = 0;
					CandForm.dwStyle = CFS_CANDIDATEPOS;
					CandForm.ptCurrentPos.x = 3200;
					CandForm.ptCurrentPos.y = 2000;
					//g_imm._ImmSetCandidateWindow(hImc, &CandForm);
					
				}
			}
			
			g_imm._ImmReleaseContext(hWnd, hImc);			
		}
		return true;
	}

	case IMN_CLOSECANDIDATE:
		
		DXUTTRACE( "  IMN_CLOSECANDIDATE\n" );

		if(m_pIMEContainer)
		{				
			m_pIMEContainer->SetCandleLength(0);
			m_pIMEContainer->SetCandleNum(-1);
		}
		
		g_IMEWindow.ShowWindow( FALSE );

		m_candidate.resize(0);
		return true;

	case IMN_SETCONVERSIONMODE:
		DXUTTRACE( "  IMN_SETCONVERSIONMODE\n" );
	case IMN_SETOPENSTATUS:
		DXUTTRACE( "  IMN_SETOPENSTATUS\n" );
        CheckToggleState(hWnd);
		return false;

    case IMN_PRIVATE:
		{
			DXUTTRACE( "IMN_PRIVATE\n" );
			GetPrivateReadingString(hWnd);		

			// Trap some messages to hide reading window
			DWORD dwId = GetImeId();
			switch( dwId )
			{
				case IMEID_CHT_VER42:
				case IMEID_CHT_VER43:
				case IMEID_CHT_VER44:
				case IMEID_CHS_VER41:
				case IMEID_CHS_VER42:
					if((lParam==1)||(lParam==2)) 
						return true;
					break;

				case IMEID_CHT_VER50:
				case IMEID_CHT_VER51:
				case IMEID_CHT_VER52:
				case IMEID_CHT_VER60:
				case IMEID_CHS_VER53:
					if((lParam==16)||(lParam==17)||(lParam==26)||(lParam==27)||(lParam==28)) 
						return true;
					break;
			}
        }
	default :
		return false;
	}

	return false;
}

bool _XIMEKernel::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( gpInput->CheckKeyPress(DIK_LCONTROL) || gpInput->CheckKeyPress(DIK_LMENU) ) return false;
	if( !m_pIMEContainer ) return false;

	HIMC hImc;

	switch(wParam) 
	{
	case VK_ESCAPE:		
		hImc = g_imm._ImmGetContext(hWnd);
		if(hImc) 
		{ 	
			//if( g_imm._ImmNotifyIME )
			//	g_imm._ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);

			g_imm._ImmReleaseContext(hWnd, hImc);
		}	
		
		m_pIMEContainer->SetComText(L"",0);
		m_comp.resize(0);

		if( m_pIMEContainer )
			m_pIMEContainer->ProcessEscapeKey();						
		return true;
		
	case VK_TAB:		
		/* win98 error
		hImc = g_imm._ImmGetContext(hWnd);
		if(hImc) 
		{ 	
			if( g_imm._ImmNotifyIME )
			{
				//wchar_t compstr[2];
				//compstr[0] = 0x0000;
				//compstr[1] = 0x0000;
				//g_imm._ImmSetCompositionStringW( hImc, SCS_SETSTR, compstr, 0, compstr, 0 );
				
				g_imm._ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
				//g_imm._ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 );				
			}			
			g_imm._ImmReleaseContext(hWnd, hImc);
		}		
		*/
		
		m_pIMEContainer->SetComText(L"",0);
		m_comp.resize(0);

		if( m_pIMEContainer )
			m_pIMEContainer->ProcessTabKey();			
		return true;
		
	case VK_BACK:
		/* win98 error
		hImc = g_imm._ImmGetContext(hWnd);
		if(hImc) 
		{ 	
			if( g_imm._ImmNotifyIME )
				g_imm._ImmNotifyIME(hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);			
	
			//wchar_t compstr[2];
			//compstr[0] = 0x0000;
			//compstr[1] = 0x0000;
			//g_imm._ImmSetCompositionStringW( hImc, SCS_SETSTR, compstr, 0, compstr, 0 );
			g_imm._ImmReleaseContext(hWnd, hImc);
		}
		*/
				
		//m_pIMEContainer->SetComText(L"",0);
		//m_comp.resize(0);

		if( m_pIMEContainer )
		{
			m_pIMEContainer->ProcessBackSpaceKey();
			
			if( m_pIMEContainer->GetNewLine() )
			{
				if( m_pIMEContainer->GetBackSpaceKeyCallBack() )
				{
					m_pIMEContainer->GetBackSpaceKeyCallBack()(m_pIMEContainer);
				}
				m_pIMEContainer->SetNewLine( FALSE );
			}
		}
		return true;
	case VK_RETURN: 

		return true;
			
	default:
		{
			if(wParam > 31) 
			{
				wchar_t temp;
				MultiByteToWideChar(m_codePage, 0, (char*)&wParam, 1, &temp, 1);
				m_pIMEContainer->AddUnicode(temp);
			}
		}

		return true;
	}

	return true;
}

int	_XIMEKernel::GetComp(char* text, int len)
{
	return _X_WtoM_ConvertString(m_codePage, m_comp.c_str(), m_comp.size(), text, len);
}

int	_XIMEKernel::GetReading(char* text, int len)
{
	return _X_WtoM_ConvertString(m_codePage, m_reading.c_str(), m_reading.size(), text, len);
}

int	_XIMEKernel::GetReading(wchar_t* text, int len)
{
	int strlength = wcslen( m_reading.c_str() );
	memset( text, 0, len );
	wcscpy( text, m_reading.c_str() );
	return strlength;
}

int _XIMEKernel::GetIndicatorMBCS(char* text, int len)
{
	if( m_wszCurrIndicator )	
		return _X_WtoM_ConvertString(m_codePage, m_wszCurrIndicator, wcslen(m_wszCurrIndicator), text, len);
	
	text[0] = 0;
	return 0;	
}

int	_XIMEKernel::GetCandidate(DWORD index, char* text, int len) 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} 
	else 
	{
		CANDIDATELIST* candidateList = (CANDIDATELIST*)&m_candidate[0];

		if(index >= candidateList->dwCount) 
		{
			return 0;
		} 
		else 
		{
			if(m_bUnicodeIME) 
			{				
				if( g_LanguageType == _XLANGUAGE_TYPE_TAIWANESE || g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
				{
					wchar_t* wText = (wchar_t*)(&m_candidate[0] + candidateList->dwOffset[index]);
					int textlength = wcslen(wText);
					//memset( text, 0, sizeof(char) * len );
					memcpy( text, wText, sizeof(wchar_t) * textlength );
					return textlength;
				}
				else
				{
					wchar_t* wText = (wchar_t*)(&m_candidate[0] + candidateList->dwOffset[index]);
					return _X_WtoM_ConvertString(m_codePage, wText, wcslen(wText), text, len);
				}
			} 
			else 
			{
				char* temp = (char*)(&m_candidate[0] + candidateList->dwOffset[index]);

				if(text == 0) 
				{
					return strlen(temp);
				} 
				else 
				{
					int tempLen = strlen(temp);

					if(len < tempLen) 
					{
						return 0;
					} 
					else 
					{
						memcpy(text, temp, tempLen);
						return tempLen;
					}
				}
			}
		}
	}
}

int	_XIMEKernel::GetCandidateCount() 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} 
	else 
	{
		return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
	}
}

int	_XIMEKernel::GetCandidateSelection() 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} 
	else 
	{
		if(PRIMARYLANGID(m_langId) == LANG_KOREAN)
			return ((CANDIDATELIST*)&m_candidate[0])->dwCount;
		else
			return ((CANDIDATELIST*)&m_candidate[0])->dwSelection;
	}
}

int	_XIMEKernel::GetCandidatePageSize() 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} 
	else 
	{
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageSize;
	}
}

int	_XIMEKernel::GetCandidatePageStart() 
{
	if(m_candidate.empty()) 
	{
		return 0;
	} 
	else 
	{
		return ((CANDIDATELIST*)&m_candidate[0])->dwPageStart;
	}
}

void _XIMEKernel::GetUnderLine(int* start, int* end) 
{
	*start = WideCharToMultiByte(m_codePage, 0, m_comp.c_str(), m_ulStart, NULL, 0, NULL, NULL); 
	*end = WideCharToMultiByte(m_codePage, 0, m_comp.c_str(), m_ulEnd, NULL, 0, NULL, NULL);
}

DWORD _XIMEKernel::GetImeId( UINT uIndex )
{
	char		szTmp[1024];
	static HKL	hklPrev = (HKL)-1;
	
	if( uIndex >= sizeof( m_dwId ) / sizeof( m_dwId[0] ) )
        return 0;

	if( hklPrev == m_hkl )
        return m_dwId[uIndex];

	hklPrev = m_hkl;  // Save for the next invocation

    if(!((m_hkl==_CHT_HKL) || (m_hkl==_CHT_HKL2) || (m_hkl==_CHS_HKL)))
	{
		m_dwId[0] = m_dwId[1] = 0;
        return m_dwId[uIndex];
	}
        
	if ( !g_imm._ImmGetIMEFileNameA( m_hkl, szTmp, ( sizeof(szTmp) / sizeof(szTmp[0]) ) - 1 ) )
	{
        m_dwId[0] = m_dwId[1] = 0;
        return m_dwId[uIndex];
	}

    if ( !g_imm._GetReadingString ) 
	{
        if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
            ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) ) 
		{
	        m_dwId[0] = m_dwId[1] = 0;
			return m_dwId[uIndex];
        }
    }

    DWORD   dwVerHandle;
    DWORD   dwVerSize = GetFileVersionInfoSize( szTmp, &dwVerHandle );

    if( dwVerSize ) 
	{
		LPVOID  lpVerBuffer = HeapAlloc( GetProcessHeap(), 0, dwVerSize );
		if( lpVerBuffer )
		{
			if( GetFileVersionInfo( szTmp, dwVerHandle, dwVerSize, lpVerBuffer ) ) {

				LPVOID  lpVerData;
				UINT    cbVerData;

				if( VerQueryValue( lpVerBuffer, "\\", &lpVerData, &cbVerData ) ) {

					DWORD dwVer = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionMS;
					dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
					if( g_imm._GetReadingString
						||
						( m_langId == LANG_CHT &&
						  ( dwVer == MAKEIMEVERSION(4, 2) || 
							dwVer == MAKEIMEVERSION(4, 3) || 
							dwVer == MAKEIMEVERSION(4, 4) || 
							dwVer == MAKEIMEVERSION(5, 0) ||
							dwVer == MAKEIMEVERSION(5, 1) ||
							dwVer == MAKEIMEVERSION(5, 2) ||
							dwVer == MAKEIMEVERSION(6, 0) ) )
						||
						( m_langId == LANG_CHS &&
							( dwVer == MAKEIMEVERSION(4, 1) ||
							dwVer == MAKEIMEVERSION(4, 2) ||
							dwVer == MAKEIMEVERSION(5, 3) ) ) ) {

						m_dwId[0] = dwVer | m_langId;
						m_dwId[1] = ( (VS_FIXEDFILEINFO*)lpVerData )->dwFileVersionLS;
					}
				}
			}

			HeapFree( GetProcessHeap(), 0, lpVerBuffer );
		}
    }

	return m_dwId[uIndex];
}

void _XIMEKernel::SetupImeApi(HWND hWnd)
{
    char szImeFile[MAX_PATH + 1];

	g_imm._GetReadingString = NULL;
    g_imm._ShowReadingWindow = NULL;

	if(!m_hkl)
	{
		m_hkl = GetKeyboardLayout( 0 );
	}

	memset( m_IMEDescription, 0, sizeof(wchar_t) * 128 );
	g_imm._ImmGetDescriptionW( m_hkl, m_IMEDescription, 128);


	if( g_imm._ImmGetIMEFileNameA( m_hkl, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) != 0 ) {

		if( g_imm.m_hDllIme ) FreeLibrary( g_imm.m_hDllIme );
	    
		g_imm.m_hDllIme = LoadLibraryA( szImeFile );

		if ( g_imm.m_hDllIme ) 
		{
			g_imm._GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
				( GetProcAddress( g_imm.m_hDllIme, "GetReadingString" ) );
			g_imm._ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
				( GetProcAddress( g_imm.m_hDllIme, "ShowReadingWindow" ) );

			if( g_imm._ShowReadingWindow ) {
				HIMC hImc = g_imm._ImmGetContext(hWnd);
				if(hImc) 
				{
					g_imm._ShowReadingWindow( hImc, false );
					g_imm._ImmReleaseContext(hWnd, hImc);
				}
			}
		}
	}
}

bool _XIMEKernel::GetReadingWindowOrientation( DWORD dwId )
{
    bool bHorizontalReading = ( m_hkl == _CHS_HKL ) || ( m_hkl == _CHT_HKL2 ) || ( dwId == 0 );
    if( !bHorizontalReading && ( dwId & 0x0000FFFF ) == LANG_CHT )
    {
        char szRegPath[MAX_PATH];
        HKEY hKey;
        DWORD dwVer = dwId & 0xFFFF0000;
        strcpy( szRegPath, "software\\microsoft\\windows\\currentversion\\" );
        strcat( szRegPath, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? "MSTCIPH" : "TINTLGNT" );
        LONG lRc = RegOpenKeyExA( HKEY_CURRENT_USER, szRegPath, 0, KEY_READ, &hKey );
        if (lRc == ERROR_SUCCESS)
        {
            DWORD dwSize = sizeof(DWORD), dwMapping, dwType;
            lRc = RegQueryValueExA( hKey, "Keyboard Mapping", NULL, &dwType, (PBYTE)&dwMapping, &dwSize );
            if (lRc == ERROR_SUCCESS)
            {
                if ( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
                       ( (BYTE)dwMapping == 0x22 || (BYTE)dwMapping == 0x23 ) )
                     ||
                     ( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
                       (BYTE)dwMapping >= 0x22 && (BYTE)dwMapping <= 0x24 )
                   )
                {
                    bHorizontalReading = true;
                }
            }
            RegCloseKey( hKey );
        }
    }

	return bHorizontalReading;
}

void _XIMEKernel::GetPrivateReadingString(HWND hWnd)
{
	DWORD dwId = GetImeId();
    if( !dwId ) 
	{
        m_reading.resize(0);
        return;
    }

	HIMC hImc = g_imm._ImmGetContext(hWnd);
    if( !hImc ) 
	{
        m_reading.resize(0);
        return;
    }

    
    DWORD dwErr = 0;

    if( g_imm._GetReadingString ) 
	{

        UINT uMaxUiLen;
        BOOL bVertical;
        // Obtain the reading string size
        int wstrLen = g_imm._GetReadingString( hImc, 0, NULL, (PINT)&dwErr, &bVertical, &uMaxUiLen );

		if( wstrLen == 0 ) 
		{
			m_reading.resize(0);
		} 
		else 
		{
			wchar_t *wstr = (wchar_t*)alloca(sizeof(wchar_t) * wstrLen);
            g_imm._GetReadingString( hImc, wstrLen, wstr, (PINT)&dwErr, &bVertical, &uMaxUiLen );
			m_reading.assign(wstr, wstr+wstrLen);
		}

		m_bVerticalReading = bVertical ? true : false;
    } 
	else 
	{
        // IMEs that doesn't implement Reading String API
		wchar_t* temp;
		DWORD tempLen;
	    bool bUnicodeIme = false;
		INPUTCONTEXT *lpIC = g_imm.LockIMC(hImc);

		if(lpIC == NULL) 
		{
			temp = NULL;
			tempLen = 0;
		} 
		else 
		{
			LPBYTE p = 0;
			switch( dwId )
			{
				case IMEID_CHT_VER42: // New(Phonetic/ChanJie)IME98  : 4.2.x.x // Win98
				case IMEID_CHT_VER43: // New(Phonetic/ChanJie)IME98a : 4.3.x.x // WinMe, Win2k
				case IMEID_CHT_VER44: // New ChanJie IME98b          : 4.4.x.x // WinXP
					p = *(LPBYTE *)((LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + 24 );
					if( !p ) break;
					tempLen = *(DWORD *)( p + 7 * 4 + 32 * 4 );
					dwErr = *(DWORD *)( p + 8 * 4 + 32 * 4 );
					temp = (wchar_t *)( p + 56 );
					bUnicodeIme = true;
					break;

				case IMEID_CHT_VER50: // 5.0.x.x // WinME
					p = *(LPBYTE *)( (LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + 3 * 4 );
					if( !p ) break;
					p = *(LPBYTE *)( (LPBYTE)p + 1*4 + 5*4 + 4*2 );
					if( !p ) break;
					tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16);
					dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 + 1*4);
					temp = (wchar_t *)(p + 1*4 + (16*2+2*4) + 5*4);
					bUnicodeIme = false;
					break;

				case IMEID_CHT_VER51: // 5.1.x.x // IME2002(w/OfficeXP)
				case IMEID_CHT_VER52: // 5.2.x.x // (w/whistler)
				case IMEID_CHS_VER53: // 5.3.x.x // SCIME2k or MSPY3 (w/OfficeXP and Whistler)
					p = *(LPBYTE *)((LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + 4);
					if( !p ) break;
					p = *(LPBYTE *)((LPBYTE)p + 1*4 + 5*4);
					if( !p ) break;
					tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2);
					dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * 2 + 1*4);
					temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
					bUnicodeIme = true;
					break;

				// the code tested only with Win 98 SE (MSPY 1.5/ ver 4.1.0.21)
				case IMEID_CHS_VER41:
					{
						int nOffset;
						nOffset = ( GetImeId( 1 ) >= 0x00000002 ) ? 8 : 7;

						p = *(LPBYTE *)((LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + nOffset * 4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 7*4 + 16*2*4);
						dwErr = *(DWORD *)(p + 8*4 + 16*2*4);
						dwErr = min( dwErr, tempLen );
						temp = (wchar_t *)(p + 6*4 + 16*2*1);
						bUnicodeIme = true;
					}
					break;

				case IMEID_CHS_VER42: // 4.2.x.x // SCIME98 or MSPY2 (w/Office2k, Win2k, WinME, etc)
					{
						OSVERSIONINFOA osi;
						osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
						GetVersionExA( &osi );

						int nTcharSize = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? sizeof(wchar_t) : sizeof(char);
						p = *(LPBYTE *)((LPBYTE)g_imm.LockIMCC( lpIC->hPrivate ) + 1*4 + 1*4 + 6*4);
						if( !p ) break;
						tempLen = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize);
						dwErr = *(DWORD *)(p + 1*4 + (16*2+2*4) + 5*4 + 16 * nTcharSize + 1*4);
						temp  = (wchar_t *) (p + 1*4 + (16*2+2*4) + 5*4);
						bUnicodeIme = ( osi.dwPlatformId == VER_PLATFORM_WIN32_NT ) ? true : false;
					}
					break;

				default:
					temp = NULL;
					tempLen = 0;
					break;
			}
		}

		if(tempLen == 0) 
		{
			m_reading.resize(0);
		} 
		else 
		{
			if( bUnicodeIme ) 
			{
				m_reading.assign(temp, tempLen);
			} 
			else 
			{
				int wstrLen = MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, NULL, 0); 
				wchar_t* wstr = (wchar_t*)alloca(sizeof(wchar_t)*wstrLen);
				MultiByteToWideChar(m_codePage, 0, (char*)temp, tempLen, wstr, wstrLen); 
				m_reading.assign(wstr, wstrLen);
			}
		}

		g_imm.UnlockIMCC(lpIC->hPrivate);
		g_imm.UnlockIMC(hImc);

		m_bVerticalReading = !GetReadingWindowOrientation(dwId);
    }

	g_imm._ImmReleaseContext(hWnd, hImc);
}

void _XIMEKernel::CheckInputLocale( void )
{
    static HKL hklPrev = 0;	
    m_hkl = GetKeyboardLayout( 0 );
    if ( hklPrev == m_hkl )
        return;

	hklPrev = m_hkl;

	m_langId = LOWORD(m_hkl);

	/* Update Indicator */ 
    switch (PRIMARYLANGID(m_langId)) 
	{
    case LANG_KOREAN:
        m_bVerticalCandidate = false;
        m_wszCurrIndicator = g_aszIndicator[INDICATOR_KOREAN];
        break;

    case LANG_JAPANESE:
        m_bVerticalCandidate = true;
        m_wszCurrIndicator = g_aszIndicator[INDICATOR_JAPANESE];
        break;

    case LANG_CHINESE:

	    m_bVerticalCandidate = true;
		switch(SUBLANGID(m_langId)) 
		{
        case SUBLANG_CHINESE_SIMPLIFIED:
            m_bVerticalCandidate = GetImeId() == 0;
            m_wszCurrIndicator = g_aszIndicator[INDICATOR_CHS];
            break;

        case SUBLANG_CHINESE_TRADITIONAL:
            m_wszCurrIndicator = g_aszIndicator[INDICATOR_CHT];
            break;

        default:
            m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
            break;
        }
        break;

    default:
        m_wszCurrIndicator = g_aszIndicator[INDICATOR_NON_IME];
		break;
    }

    if( m_wszCurrIndicator == g_aszIndicator[INDICATOR_NON_IME] )
    {
        WCHAR szLang[5];
        GetLocaleInfoW( MAKELCID( m_langId, SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, szLang, 5 );
        m_wszCurrIndicator[0] = szLang[0];
        m_wszCurrIndicator[1] = towlower( szLang[1] );
    }
}

void _XIMEKernel::SetNativeMode( BOOL native )
{
	CheckInputLocale();
	
	bool bIme = g_imm._ImmIsIME( m_hkl ) != 0;

	HIMC hImc = g_imm._ImmGetContext(gHWnd);
    if( hImc ) 
	{
        if( ( PRIMARYLANGID(m_langId) == LANG_CHINESE ) && bIme ) 
		{
            DWORD dwConvMode, dwSentMode;
			g_imm._ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
            m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;

			if( native )
			{
				if( m_ImeState == IMEUI_STATE_ENGLISH )
				{
					DWORD dwTemp = dwConvMode & ~IME_CMODE_LANGUAGE;
					dwTemp |= IME_CMODE_NATIVE;
					g_imm._ImmSetConversionStatus(hImc,dwTemp,dwSentMode); 
					m_ImeState = IMEUI_STATE_ON;
				}
			}
			else
			{
				if( m_ImeState == IMEUI_STATE_ON )
				{
					DWORD dwTemp = dwConvMode & ~IME_CMODE_LANGUAGE;
					dwTemp |= IME_CMODE_ALPHANUMERIC;
					g_imm._ImmSetConversionStatus(hImc,dwTemp,dwSentMode); 
					m_ImeState = IMEUI_STATE_ENGLISH;
				}
			}
        } 
		else 
		{
			DWORD dwConvMode, dwSentMode;
			g_imm._ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
            m_ImeState = ( bIme && g_imm._ImmGetOpenStatus(hImc) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;

			if( native )
			{
				if( m_ImeState == IMEUI_STATE_OFF )
				{
					DWORD dwTemp = dwConvMode & ~IME_CMODE_LANGUAGE;
					dwTemp |= IME_CMODE_NATIVE;
					g_imm._ImmSetConversionStatus(hImc,dwTemp,dwSentMode); 
					m_ImeState = IMEUI_STATE_ON;
				}
			}
			else
			{
				if( m_ImeState == IMEUI_STATE_ON )
				{
					DWORD dwTemp = dwConvMode & ~IME_CMODE_LANGUAGE;
					dwTemp |= IME_CMODE_ALPHANUMERIC;
					g_imm._ImmSetConversionStatus(hImc,dwTemp,dwSentMode); 
					m_ImeState = IMEUI_STATE_OFF;
				}
			}
        }

		g_imm._ImmReleaseContext(gHWnd, hImc);        
	} 
	else 
	{
        m_ImeState = IMEUI_STATE_OFF;
	}
}

void _XIMEKernel::CheckToggleState(HWND hWnd)
{
	CheckInputLocale();
	
	bool bIme = g_imm._ImmIsIME( m_hkl ) != 0;

	HIMC hImc = g_imm._ImmGetContext(hWnd);
    if( hImc ) 
	{
        if( ( PRIMARYLANGID(m_langId) == LANG_CHINESE ) && bIme ) 
		{
            DWORD dwConvMode, dwSentMode;
			g_imm._ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
            m_ImeState = ( dwConvMode & IME_CMODE_NATIVE ) ? IMEUI_STATE_ON : IMEUI_STATE_ENGLISH;
        } 
		else 
		{
            m_ImeState = ( bIme && g_imm._ImmGetOpenStatus(hImc) != 0 ) ? IMEUI_STATE_ON : IMEUI_STATE_OFF;
        }

		g_imm._ImmReleaseContext(hWnd, hImc);        
	} 
	else 
	{
        m_ImeState = IMEUI_STATE_OFF;
	}
}

int	GetCharsetFromLang( LANGID langid )
{
	switch( PRIMARYLANGID(langid) )
	{
	case LANG_JAPANESE:
		return SHIFTJIS_CHARSET;
	case LANG_KOREAN:
		return HANGEUL_CHARSET;
	case LANG_CHINESE:
		switch( SUBLANGID(langid) )
		{
		case SUBLANG_CHINESE_SIMPLIFIED:
			return GB2312_CHARSET;
		case SUBLANG_CHINESE_TRADITIONAL:
			return CHINESEBIG5_CHARSET;
		default:
			return ANSI_CHARSET;
		}
	case LANG_GREEK:
		return GREEK_CHARSET;
	case LANG_TURKISH:
		return TURKISH_CHARSET;
	case LANG_HEBREW:
		return HEBREW_CHARSET;
	case LANG_ARABIC:
		return ARABIC_CHARSET;
	case LANG_ESTONIAN:
	case LANG_LATVIAN:
	case LANG_LITHUANIAN:
		return BALTIC_CHARSET;
	case LANG_THAI:
		return THAI_CHARSET;
	case LANG_CZECH:
	case LANG_HUNGARIAN:
	case LANG_POLISH:
	case LANG_CROATIAN:
	case LANG_MACEDONIAN:
	case LANG_ROMANIAN:
	case LANG_SLOVAK:
	case LANG_SLOVENIAN:
		return EASTEUROPE_CHARSET;
	case LANG_RUSSIAN:
	case LANG_BELARUSIAN:
	case LANG_BULGARIAN:
	case LANG_UKRAINIAN:
		return RUSSIAN_CHARSET;
	case LANG_VIETNAMESE:
		return VIETNAMESE_CHARSET;
	default:
		return ANSI_CHARSET;
	}
}

int	GetCodePageFromCharset( int charset )
{
	switch( charset )
	{
	case SHIFTJIS_CHARSET:
		return 932;
	case HANGUL_CHARSET:
		return 949;
	case GB2312_CHARSET:
		return 936;
	case CHINESEBIG5_CHARSET:
		return 950;
	case GREEK_CHARSET:
		return 1253;
	case TURKISH_CHARSET:
		return 1254;
	case HEBREW_CHARSET:
		return 1255;
	case ARABIC_CHARSET:
		return 1256;
	case BALTIC_CHARSET:
		return 1257;
	case THAI_CHARSET:
		return 874;
	case EASTEUROPE_CHARSET:
		return 1250;
	case VIETNAMESE_CHARSET:
		return 1258;
	case RUSSIAN_CHARSET:
		return 1251;
	default:
		return 1252;
	}
}

int	GetCodePageFromLang( LANGID langid )
{
	return GetCodePageFromCharset(GetCharsetFromLang(langid));
}
