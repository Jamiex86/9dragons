#ifndef _REGISTRY_H_
#define _REGISTRY_H_

#include <windows.h>
#include "RegistryDef.h"

#define _REGSTRING_MAXLENGTH	256

/** 레지스트리 값(INT형) 읽기.
 */
UINT _XRegReadInt(HKEY hKey,LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault, BOOL bDefaultSet = FALSE );

/** 레지스트리 값(STRING형) 읽기.
 */
BOOL _XRegReadString(HKEY hKey, LPCTSTR lpKey,LPCTSTR lpValue, LPCTSTR lpDefault,LPTSTR lpRet, DWORD nSize, BOOL bDefaultSet = FALSE );

/** 레지스트리 값(INT형) 쓰기.
 */
BOOL _XRegWriteInt(HKEY hKey,LPCTSTR lpKey, LPCTSTR lpValue, UINT nData);

/** 레지스트리 값(STRING형) 쓰기.
 */
BOOL _XRegWriteString(HKEY hKey,LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData);

/** 레지스트리 값 삭제.
 */
BOOL _XRegDeleteNode(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue );

#endif