#include "xgamebase.h"
#include "xintern.h"

TCHAR *_XOSTypeString(U32 ostype)
{
 switch(ostype)
 {
  case OSTYPE_DOS: return "MS-DOS";
  case OSTYPE_WIN3: return "Windows 3.x";
  case OSTYPE_WIN95: return "Windows 95";
  case OSTYPE_WIN98: return "Windows 98";
  case OSTYPE_WINNT: return "Windows NT";
  case OSTYPE_WIN2K: return "Windows 2000";
  case OSTYPE_WINXP: return "Windows XP";
  case OSTYPE_WINVISTA: return "Windows Vista";
  case OSTYPE_WIN2003: return "Windows 2003";
  case OSTYPE_OS2: return "OS/2";
  case OSTYPE_WARP: return "OS/2 Warp";
  case OSTYPE_DOSEMU: return "DOS Emulator";
  case OSTYPE_OPENDOS: return "Open DOS";
  case OSTYPE_LINUX: return "Linux";
  case OSTYPE_UNIX: return "Unix";
  case OSTYPE_BEOS: return "BeOS";
  case OSTYPE_MACOS: return "MacOS";
  default: return "Unknown";
 }
}

void _XExecutableName(TCHAR *name, int size)
{
 sys_XGetExecutableName(name, size);
}

int _XCmdLineOption(TCHAR *str, int req_params)
{
 int i;

 for(i = 1; i < (_XArgc() - req_params); i++)
 {
  if(((_XArgv(i)[0] == '-') || (_XArgv(i)[0] == '/'))
	 && (!stricmp(&_XArgv(i)[1], str)))
  {
   return i;
  }
 }

 return 0;
}

int _XCmdLineOption(TCHAR *str, int req_params, int comparesize )
{
	int i;
	
	for(i = 1; i < (_XArgc() - req_params); i++)
	{
		if(((_XArgv(i)[0] == '-') || (_XArgv(i)[0] == '/'))
			&& ( !strncmp(&_XArgv(i)[1], str, comparesize )))
		{
			return i;
		}
	}
	
	return 0;
}
