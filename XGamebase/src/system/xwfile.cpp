
#include "xgamebase.h"
#include "xintern.h"

int sys_XFileIsOK(TCHAR *filename)
{
	struct _finddata_t info;
	long handle;
	
	errno = 0;
	
	if((handle = _findfirst(filename, &info)) < 0) 
		return FALSE;
	
	_findclose(handle);
	
	return TRUE;
}

int sys_XFileExists(TCHAR *filename, int attrib, int *aret)
{
	struct _finddata_t info;
	long handle;
	
	errno = 0;
	
	if((handle = _findfirst(filename, &info)) < 0) 
		return FALSE;
	
	_findclose(handle);
	
	if(aret)
		*aret = info.attrib;
	
	info.attrib &= (FA_HIDDEN | FA_SYSTEM | FA_LABEL | FA_DIREC);
	
	if((info.attrib & attrib) != info.attrib)
		return FALSE;
	
	return TRUE;
}

long sys_XFileSize(TCHAR *filename)
{
	struct _finddata_t info;
	long handle;
	
	errno = 0;
	
	if((handle = _findfirst(filename, &info)) < 0) 
		return 0;
	
	_findclose(handle);
	
	if(info.attrib & (FA_SYSTEM | FA_LABEL | FA_DIREC))
		return 0;
	
	return info.size;
}

long sys_XFileTime(TCHAR *filename)
{
	struct _finddata_t info;
	long handle;
	
	errno = 0;
	
	if((handle = _findfirst(filename, &info)) < 0) 
		return 0;
	
	_findclose(handle);
	
	if(info.attrib & (FA_SYSTEM | FA_LABEL | FA_DIREC))
		return 0;
	
	return info.time_write;
}

typedef struct FFIND_INFO 
{
	struct _finddata_t info;
	long handle;
	int attrib;
} FFIND_INFO;

void *sys_XFindFirst(TCHAR *name, int attrib, char *nameret, int *aret)
{
	FFIND_INFO *info;
	int a;
	
	info = (FFIND_INFO*)malloc(sizeof(FFIND_INFO));
	
	if(!info) 
	{
		errno = ENOMEM;
		return NULL;
	}
	
	info->attrib = attrib;
	
	errno = 0;
	
	if((info->handle = _findfirst(name, &info->info)) < 0) 
	{
		free(info);
		return NULL;
	}
	
	a = info->info.attrib & (FA_HIDDEN | FA_SYSTEM | FA_LABEL | FA_DIREC);
	
	if((a & attrib) != a) 
	{
		if(!sys_XFindNext(info, nameret, aret))
		{
			_findclose(info->handle);
			free(info);
			return NULL;
		}
		else
			return info;
	}
	
	if(nameret)
		strcpy(nameret, info->info.name);
	
	if(aret)
		*aret = info->info.attrib;
	
	return info;
}

int sys_XFindNext(void *dta, TCHAR *nameret, int *aret)
{
	FFIND_INFO *info = (FFIND_INFO *)dta;
	int a;
	
	do 
	{
		if(_findnext(info->handle, &info->info) != 0) 
			return 0;
		
		a = info->info.attrib & (FA_HIDDEN | FA_SYSTEM | FA_LABEL | FA_DIREC);
		
	} while((a & info->attrib) != a);
	
	if(nameret)
		strcpy(nameret, info->info.name);
	
	if(aret)
		*aret = info->info.attrib;
	
	return 1;
}

void sys_XFindClose(void *dta)
{
	FFIND_INFO *info = (FFIND_INFO *)dta;
	
	_findclose(info->handle);
	free(info);
}

int sys_XGetDrive()
{
	return _getdrive() - 1;
}

void sys_XGetDcwd(int drive, TCHAR *buf, int size)
{
	if(!_getdcwd(drive + 1, buf, size))
		*buf = 0;
}

int sys_XDeleteFile(TCHAR *filename)
{
	if(unlink(filename) == 0)
		return 1;
	
	return 0;
}

int sys_XCreateDir(TCHAR *dir)
{
	if(mkdir(dir) == 0)
		return 1;
	
	return 0;
}

int sys_XDeleteDir(TCHAR *dir)
{
	if(rmdir(dir) == 0)
		return 1;
	
	return 0;
}
