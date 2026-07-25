//XRegistry.h
#ifndef _XREGISTRY_H_
#define _XREGISTRY_H_

#include <windows.h>

#define SHLM							HKEY_LOCAL_MACHINE
#define SHCU							HKEY_CURRENT_USER
#define SHCR							HKEY_CLASSES_ROOT

#define _REGSTRING_MAXLENGTH			256
							
UINT _XRegReadInt(HKEY hKey,LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault, BOOL bDefaultSet = FALSE );
BOOL _XRegReadString(HKEY hKey, LPCTSTR lpKey,LPCTSTR lpValue, LPCTSTR lpDefault,LPTSTR lpRet, DWORD nSize, BOOL bDefaultSet = FALSE );
BOOL _XRegWriteInt(HKEY hKey,LPCTSTR lpKey, LPCTSTR lpValue, UINT nData);
BOOL _XRegWriteString(HKEY hKey,LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData);
BOOL _XRegDeleteNode(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue );

#endif