
#include "xgamebase.h"
#include "xintern.h"

void ConvertCommandLineToArgcArgv(TCHAR *argbuf);
static TCHAR **gpArgv;
static int gnArgc;

TCHAR gstrGameBaseVersion[MAX_CHAR_STRING];
int  _XUseLFN = 0;

bool _XGameBaseStarted = 0;

#define MAX_EXIT_FUNCS     64
typedef void (*funcptr)(void);
static funcptr exit_funcs[MAX_EXIT_FUNCS] = 
{ 
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

extern LPTSTR _XGetLogFileName( void );
extern void _XSetDefaultPath( LPSTR localpath );

int _XStartup(TCHAR *cmdline)
{
	//ConvertCommandLineToArgcArgv(cmdline);
	return _XStartup(gnArgc, gpArgv);
}

int _XStartup(int argc, TCHAR **argv)
{
	if(_XGameBaseStarted)
		return 1;
	
	gpArgv = argv;
	gnArgc = argc;
	
	_XUseLFN = _X_USE_LFN;
	
	memset(gstrErrorMsg, 0, MAX_CHAR_STRING);
	
	sprintf(gstrGameBaseVersion, "version %d.%d.%d",
		_X_VERSION, _X_SUB_VERSION, _X_PATCH_VERSION/*,_X_DATE*/);
	
	if(!sys_XStartup())
	{
		_XLogError("_XStartup() - could not start the xshell!");
		return 0;
	}
	
	if(!_XTimerStartup())
	{
		_XLogError("_XStartup() - could not start the game timer!");
		return 0;
	}
	
	if(_XCmdLineOption("xkernel_version",0))
	{
		_XPushLogFile();
		
		_XSetLogFile("xkernelversion.log");
		_XLog("__XKernel version________\n"
			  "%s\n"
              "code by %s[%s]\n"
              "homepage : %s\n"
			  "__xkernel end log________\n",
			   gstrGameBaseVersion,
			   _X_AUTHORS, _X_EMAIL, _X_WEB_SITE);

		_XPopLogFile();
		exit(0);
	}
	
	_XGameBaseStarted = 1;
	
	_XLog("%s [%s] Started!", _X_STR, gstrGameBaseVersion);
	
	sys_XAtExit(_XShutdown);
	
	return 1;
}

void _XShutdown(void)
{
	if(!_XGameBaseStarted)	return;
	
	_XCallExitFuncs();
	sys_XShutdown();
	
	if(gstrErrorMsg[0])
		_XLog("Error detected");
	else
		_XLog("No error");

	_XLog("Shutdown %s",_X_STR);

	TCHAR tmpbuf[128];
    time_t ltime;    
    struct tm *today;

	time( &ltime );
	today = localtime( &ltime );
    
    strftime( tmpbuf, 128, "Process end time : %H:%M:%S / %A, day %d of %B in the year %Y.", today );	
	_XLog( tmpbuf );	
	_XLog("========================================================================" );		     
	
	if( strstr( _XGetLogFileName(), "EOD.log" ) == NULL )
	{
		if(!gstrErrorMsg[0])
		{
			_XSetDefaultPath( NULL );
			_XDeleteFile( _XGetLogFileName() );
		}
	}
	
	_XGameBaseStarted = 0;
}

void _XAddExitFunc(void (*func)(void))
{
	int c;
	
	for(c = 0; c < MAX_EXIT_FUNCS; c++)
	{
		if(!exit_funcs[c])
		{
			exit_funcs[c] = func;
			break;
		}
	}
}

void _XRemoveExitFunc(void (*func)(void))
{
	int c;
	
	for(c = 0; c < MAX_EXIT_FUNCS; c++)
	{
		if(exit_funcs[c] == func)
		{
			while(c < MAX_EXIT_FUNCS-1)
			{
				exit_funcs[c] = exit_funcs[c+1];
				c++;
			}
			exit_funcs[MAX_EXIT_FUNCS-1] = NULL;
			break;
		}
	}
}

void _XCallExitFuncs(void)
{
	int c;
	
	for(c = MAX_EXIT_FUNCS-1; c >= 0; c--)
		if(exit_funcs[c])
			(*exit_funcs[c])();
}

void ConvertCommandLineToArgcArgv(TCHAR *argbuf)
{
	int i, q;
	int nArgc;
	static TCHAR *pArgv[64]; 
	
	nArgc = 0;
	i = 0;
	while(argbuf[i])
	{
		while((argbuf[i]) && (isspace(argbuf[i])))
			i++;
		
		if(argbuf[i])
		{
			if((argbuf[i] == '\'') || (argbuf[i] == '"'))
			{
				q = argbuf[i++];
				if(!argbuf[i])
					break;
			}
			else
				q = 0;
			
			pArgv[nArgc++] = &argbuf[i];
			
			while((argbuf[i]) && ((q) ? (argbuf[i] != q) : (!isspace(argbuf[i]))))
				i++;
			
			if(argbuf[i])
			{
				argbuf[i] = 0;
				i++;
			}
		}
	}
	
	gpArgv = pArgv;
	gnArgc = nArgc;
}

int _XArgc(void)
{
	return gnArgc;
}

TCHAR *_XArgv(int i)
{
	if(i >= gnArgc)
		return "";
	
	return gpArgv[i];
}
