/////////////////////////////////////////////////////////////////
// Log system...

#include "xgamebase.h"
#include "xintern.h"

static TCHAR logfile_name[MAX_CHAR_STRING] = "\0\0\0";
static TCHAR pushed_logfile_name[MAX_CHAR_STRING] = "\0\0\0";

static int  first_log_open = 1;

extern bool _XDebugWindow;
extern HWND  g_DebugWindowHandle;
extern HWND  g_DebugWindowListHandle;

extern TCHAR gModulePath[_MAX_PATH];
extern HWND gHWnd;

#ifdef _XDWDEBUG
extern void _XDebugOutputString( LPSTR message, int attrib , int slot = 0 );
#endif

void _XSetLogFile(TCHAR *filename)
{
    if(filename)
	{
		memset( logfile_name, 0, MAX_CHAR_STRING );
		strcpy(logfile_name, gModulePath );
		strcat(logfile_name, "\\" );
        strcat(logfile_name, filename );
	}
    else
        memset(logfile_name, 0, MAX_CHAR_STRING);

    first_log_open = 1;
}

LPTSTR _XGetLogFileName( void )
{
	return logfile_name;
}

void _XLog(TCHAR *msg, ...)
{
    _XVFile *rwops = NULL;
    va_list ap;
    TCHAR buf[2048];

	SYSTEMTIME TimeInfo;
	GetLocalTime(&TimeInfo);
	sprintf( buf, "[ %2d:%2d:%2d ]  ", TimeInfo.wHour, TimeInfo.wMinute, TimeInfo.wSecond );
	
	int startpos = strlen( buf );
		    
    if((!logfile_name[0]) || (!msg))
		return;
    
    if(first_log_open)
    {
        rwops = _XVFOpenFile(logfile_name, "w");
        first_log_open = 0;
    }
    else
    {
        rwops = _XVFOpenFile(logfile_name, "a");
    }
    
    if(!rwops)
        return;
    
    va_start(ap, msg);
    vsprintf(buf+startpos, msg, ap);
    va_end(ap);
    
	LPTSTR pStr = buf+startpos;
	while( *pStr )
	{
		if( *pStr == '\r' )
			*pStr = ' ';
		else if( *pStr == '\n' )
			*pStr = ' ';

		pStr++;
	}

	*pStr = '\n';
	*(pStr+1) = '\0';

    _XVFPutStr(rwops, buf);	
   
    _XDestroy(VFile, rwops);
}

void _XPushLogFile(void)
{
    if(logfile_name[0])
        memcpy(pushed_logfile_name, logfile_name, MAX_CHAR_STRING);
}

void _XPopLogFile(void)
{
    if(pushed_logfile_name[0])
        memcpy(logfile_name, pushed_logfile_name, MAX_CHAR_STRING);
}

void _XMessage( TCHAR* argv, ... )
{
	TCHAR szBuff[2048];
	int retValue;
	va_list argptr;
	
	va_start( argptr, argv );
	retValue = vsprintf( szBuff, argv, argptr );
	va_end( argptr );

	_XLog( szBuff );
	//_XLog( "\n" );

	MessageBox( gHWnd, szBuff, "9Dragons online", MB_OK );
}

#ifdef _XDWDEBUG

#define _XDEF_LISTBOXMAXLOGLINE		500
void _XDWINPRINTONLY( TCHAR *msg, ... )
{

	TCHAR szBuff[2048];
	int retValue;
	va_list argptr;
		
	va_start( argptr, msg );
	retValue = vsprintf( szBuff, msg, argptr );
	va_end( argptr );
			
	if( _XDebugWindow && g_DebugWindowListHandle )
	{
		int col = SendMessage( g_DebugWindowListHandle, LB_ADDSTRING, 0, (LPARAM)szBuff );
		if( col > _XDEF_LISTBOXMAXLOGLINE )
		{
			SendMessage( g_DebugWindowListHandle, LB_DELETESTRING, 0, 0 );
			SendMessage( g_DebugWindowListHandle, LB_SETCURSEL, col-1, 0 );
		}		
		else
		{
			SendMessage( g_DebugWindowListHandle, LB_SETCURSEL, col, 0 );
		}		
	}

	_XDebugOutputString( szBuff, 1, 1 );
}

void _XDWINPRINT( TCHAR *msg, ... )
{
	TCHAR szBuff[2048];
	int retValue;
	va_list argptr;
	
	va_start( argptr, msg );
	retValue = vsprintf( szBuff, msg, argptr );
	va_end( argptr );
	
	//_cputs( szBuff );
	//_putch( '\n' );
	_XLog( szBuff );

	if( _XDebugWindow && g_DebugWindowListHandle )
	{			
		int col = SendMessage( g_DebugWindowListHandle, LB_ADDSTRING, 0, (LPARAM)szBuff );		
		if( col > _XDEF_LISTBOXMAXLOGLINE )
		{
			SendMessage( g_DebugWindowListHandle, LB_DELETESTRING, 0, 0 );
			SendMessage( g_DebugWindowListHandle, LB_SETCURSEL, col-1, 0 );
		}
		else
		{
			SendMessage( g_DebugWindowListHandle, LB_SETCURSEL, col, 0 );
		}
	}

	_XDebugOutputString( szBuff, 1 );
}

#endif