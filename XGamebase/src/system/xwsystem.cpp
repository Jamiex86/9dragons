
#include "xgamebase.h"
#include "xintern.h"

#if 0
void disable_fpu_exceptions(void)
{
	U32 fpenv[8];
	
	_asm {
		fnstenv fpenv
			or fpenv, 0x3F
			fldenv fpenv
	}
}
#else
void disable_fpu_exceptions(void)
{
}
#endif

void _XGetSystemInformationFromRegistry(LPTSTR szDevice, LPTSTR szInfo, LPTSTR szResult );

static OSVERSIONINFO detect_os(void)
{
	unsigned int win_ver = GetVersion();
	OSVERSIONINFO ver;
	
	ver.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx(&ver);
	
	if(ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if(ver.dwMajorVersion == 6) // vista, longhorn
		{
			gnOSType = OSTYPE_WINVISTA;
		}		
		else if(ver.dwMajorVersion == 5)
		{
			if( ver.dwMinorVersion == 0 )
			{
				gnOSType = OSTYPE_WIN2K;
			}
			else if( ver.dwMinorVersion == 1 )
			{
				gnOSType = OSTYPE_WINXP;
			}
			else if( ver.dwMinorVersion == 2 )
			{
				gnOSType = OSTYPE_WIN2003;
			}
		}
		else
		{
			gnOSType = OSTYPE_WINNT;
		}
	}
	else if((win_ver & 0xFF) == 4) 
	{
		if((win_ver & 0xFF00) < 40)
		{
			gnOSType = OSTYPE_WIN95;
		}
		else
		{
			gnOSType = OSTYPE_WIN98;
		}
	}
	else
	{
		gnOSType = OSTYPE_WIN3;
	}	

	return ver;
}

#include <time.h>

int sys_XStartup(void)
{
	_XUseLFN = 1;
	
	disable_fpu_exceptions();
	OSVERSIONINFO osinfo = detect_os();
		       
	_XLog("========================================================================" );
	_XLog("OS System : %s/%d[%d:%d]/%s", _XOSTypeString(gnOSType), osinfo.dwBuildNumber, 
		                                                           osinfo.dwMajorVersion, 
																   osinfo.dwMinorVersion, 
																   osinfo.szCSDVersion );	

	TCHAR tmpbuf[1024];
    time_t ltime;    
    struct tm *today;

	time( &ltime );
	today = localtime( &ltime );
    
    strftime( tmpbuf, 256, "Process start time : %H:%M:%S / %A, day %d of %B in the year %Y.", today );	
	_XLog( tmpbuf );	
	_XLog("========================================================================" );		       
/*
	_XLog("========================================================================" );
	_XGetSystemInformationFromRegistry( "i8042prt", "Keyboard",	tmpbuf );		_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "Kbdclass", "KBD class",	tmpbuf );	_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "mouhid", "Mouse",	tmpbuf );			_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "Mouclass", "MOUSE class",	tmpbuf );	_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "Disk", "Hard disk",	tmpbuf );		_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "Cdrom", "ODD",	tmpbuf );			_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "atapi", "IDE Channel",	tmpbuf );		_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "audstub", "Audio stub",	tmpbuf );	_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "Flpydisk", "FDD",	tmpbuf );	_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "Serial", "Serial port",	tmpbuf );		_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "Parport", "Par port",	tmpbuf );		_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "PCI", "PCI bus",	tmpbuf );	_XLog( tmpbuf );
	_XGetSystemInformationFromRegistry( "Processor", "CPU",tmpbuf );		_XLog( tmpbuf );
	_XLog("========================================================================" );
*/	
	return 1;
}

void sys_XShutdown(void)
{
	_XLog("Exit application.");
}

void sys_XAtExit(void (*func)())
{
	atexit((void(*)(void))func);
}

void sys_XGetExecutableName(TCHAR *output, int size)
{
	unsigned char *cmd = (U8*)GetCommandLine();
	int pos = 0;
	int i = 0;
	int q;
	
	while((cmd[i]) && (isspace(cmd[i])))
		i++;
	
	if((cmd[i] == '\'') || (cmd[i] == '"'))
		q = cmd[i++];
	else
		q = 0;
	
	size--;
	
	while((cmd[i]) && ((q) ? (cmd[i] != q) : (!isspace(cmd[i]))))
	{
		size--;
		if(size < 0)
			break;
		
		output[pos] = cmd[i++];
		pos++;
	}
	
	output[pos] = 0;
}


static inline void do_8253(void) 
{ 
    __asm 
    { 
        mov     al, 34h 
			out     43h, al         ; _outp(0x43, 0x34); 
        mov     ax, bx 
			out     40h, al         ; _outp(0x40, low); 
        mov     al, ah 
			out     40h, al         ; _outp(0x40, high); 
        _emit   0xCF          ; iret 
    }; 
} 


static void __declspec(naked) Win98_8253(unsigned freq) 
{ 
    // prolog 
    __asm 
    { 
        push    ebp 
			mov     ebp, esp 
			push    ebx 
    }; 
	
    __asm 
    { 
        push    ecx 
			push    eax 
			sidt    [esp-2] 
			pop     eax 
			mov     [ebp-4], eax 
			mov     eax, [ebp-4] 
			mov     ecx, offset do_8253 
			mov     [ebp-4], ecx 
			mov     dl, byte ptr [ebp-1] 
			mov     [eax+28h], cl 
			mov     cl, byte ptr [ebp-2] 
			mov     [eax+29h], ch 
			mov     [eax+2Eh], cl 
			mov     [eax+2Fh], dl 
			mov     ebx, [ebp+8] 
			int     5 
    }; 
	
    // epilog 
    __asm 
    { 
        pop     ebx 
			mov     esp, ebp 
			pop     ebp 
			retn 
    }; 
} 

static void (*Outport) (unsigned short port, int data) = NULL; 
//Outport = (void (*)(unsigned short, int)) GetProcAddress(hNTPortModule, "Outport"); 


static void __declspec(naked) NT_8253(unsigned freq) 
{ 
    // prolog 
    __asm 
    { 
        push    ebp 
			mov     ebp, esp 
			push    ebx 
    }; 
	
    __asm 
    { 
        push 0x34 
			push 0x43 
			call Outport 
			
			mov ebx, [ebp+8] 
			mov al, bl 
			and eax, 0xFF 
			push eax 
			push 0x40 
			call Outport 
			
			xor ecx, ecx 
			mov cl, bh 
			push ecx 
			push 0x40 
			call Outport 
    }; 
	
    // epilog 
    __asm 
    { 
        pop     ebx 
			mov     esp, ebp 
			pop     ebp 
			retn 
    }; 
} 


void CheckSystemClock( void )
{ 
	switch( gnOSType ) 
	{
	case OSTYPE_WIN98 :
		{
			
		}
		break;

	case OSTYPE_WINNT:
	case OSTYPE_WIN2K:
	case OSTYPE_WINXP:
		{

		}
		break;	
	}	
}


extern UINT _XRegReadInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault, BOOL bDefaultSet );
extern BOOL _XRegReadString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault,LPTSTR lpRet, DWORD nSize, BOOL bDefaultSet );

void _XGetSystemInformationFromRegistry(LPTSTR szDevice, LPTSTR szInfo, LPTSTR szResult )
{
	if( !szResult ) return;
	
    TCHAR szBuff[512], szFindLoc[512];
    TCHAR szTemp[8];	
	
    sprintf(szFindLoc, "SYSTEM\\CurrentControlSet\\Services\\%s\\Enum", szDevice);	
    DWORD    dwNumber = _XRegReadInt( HKEY_LOCAL_MACHINE, szFindLoc, "Count", 0, FALSE );
    TCHAR    findpath[1024];
	
    for ( int i=0; i<(int)dwNumber; i++ )		
    {		
        _XRegReadString( HKEY_LOCAL_MACHINE, szFindLoc, _itoa(i, szTemp, 10), " ", szBuff, 511, FALSE );
        sprintf(findpath, "SYSTEM\\ControlSet001\\Enum\\%s", szBuff);
		
		TCHAR temp[512];
		memset(temp,0,512);

		_XRegReadString( HKEY_LOCAL_MACHINE, findpath, "FriendlyName", " ", temp, 511, FALSE );
		
        if ( strlen(temp) == 0 )
        {			
			_XRegReadString( HKEY_LOCAL_MACHINE, findpath, "DeviceDesc", " ", temp, 511, FALSE );
        }       
		

        if ( strlen(temp) ) strcpy(szBuff, temp);
        else strcpy(szBuff, "Failed...");

		sprintf(szResult, "%-12s [%d]\t%s", szInfo, i+1, szBuff);
    }	
}