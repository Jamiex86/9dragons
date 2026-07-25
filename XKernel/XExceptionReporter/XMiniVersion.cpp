#include "stdafx.h"
#include "XMiniVersion.h"

#pragma comment(lib, "version.lib")

_XMiniVersion::_XMiniVersion(LPCTSTR lpszPath)
{
	ZeroMemory(m_szPath, sizeof(m_szPath));

	if (lpszPath && lpszPath[0] != 0)
	{
		lstrcpyn(m_szPath, lpszPath, sizeof(m_szPath)-1);
	}
	else
	{
	}

	m_pData = NULL;
	m_dwHandle = 0;

	int i = 0 ; for ( i = 0; i < 4; i++)
	{
		m_wFileVersion[i] = 0;
		m_wProductVersion[i] = 0;
	}

	m_dwFileFlags = 0;
	m_dwFileOS = 0;
	m_dwFileType = 0;
	m_dwFileSubtype = 0;

	ZeroMemory(m_szCompanyName, sizeof(m_szCompanyName));
	ZeroMemory(m_szProductName, sizeof(m_szProductName));
	ZeroMemory(m_szFileDescription, sizeof(m_szFileDescription));

	Init();
}

BOOL _XMiniVersion::Init()
{
	DWORD dwHandle;
	DWORD dwSize;
	BOOL rc;

	dwSize = ::GetFileVersionInfoSize((LPSTR)(LPCTSTR)m_szPath, &dwHandle);
	if (dwSize == 0)
		return FALSE;
		
	m_pData = new BYTE [dwSize + 1];	
	ZeroMemory(m_pData, dwSize+1);

	rc = ::GetFileVersionInfo((LPSTR)(LPCTSTR)m_szPath, dwHandle, dwSize, m_pData);
	if (!rc)
		return FALSE;

	// get fixed info

	VS_FIXEDFILEINFO FixedInfo;
	
	if (GetFixedInfo(FixedInfo))
	{
		m_wFileVersion[0] = HIWORD(FixedInfo.dwFileVersionMS);
		m_wFileVersion[1] = LOWORD(FixedInfo.dwFileVersionMS);
		m_wFileVersion[2] = HIWORD(FixedInfo.dwFileVersionLS);
		m_wFileVersion[3] = LOWORD(FixedInfo.dwFileVersionLS);

		m_wProductVersion[0] = HIWORD(FixedInfo.dwProductVersionMS);
		m_wProductVersion[1] = LOWORD(FixedInfo.dwProductVersionMS);
		m_wProductVersion[2] = HIWORD(FixedInfo.dwProductVersionLS);
		m_wProductVersion[3] = LOWORD(FixedInfo.dwProductVersionLS);

		m_dwFileFlags   = FixedInfo.dwFileFlags;
		m_dwFileOS      = FixedInfo.dwFileOS;
		m_dwFileType    = FixedInfo.dwFileType;
		m_dwFileSubtype = FixedInfo.dwFileSubtype;
	}
	else
		return FALSE;

	// get string info

	GetStringInfo(_T("CompanyName"),     m_szCompanyName);
	GetStringInfo(_T("FileDescription"), m_szFileDescription);
	GetStringInfo(_T("ProductName"),     m_szProductName);

	return TRUE;		
}

void _XMiniVersion::Release()
{
	// do this manually, because we can't use objects requiring
	// a dtor within an exception handler
	if (m_pData)
		delete [] m_pData;
	m_pData = NULL;
}

BOOL _XMiniVersion::GetFileVersion(WORD * pwVersion)
{
	int i = 0 ; for ( i = 0; i < 4; i++)
		*pwVersion++ = m_wFileVersion[i];
	return TRUE;
}					 	 

BOOL _XMiniVersion::GetProductVersion(WORD * pwVersion)
{
	int i = 0 ; for ( i = 0; i < 4; i++)
		*pwVersion++ = m_wProductVersion[i];
	return TRUE;
}

BOOL _XMiniVersion::GetFileFlags(DWORD& rdwFlags)
{
	rdwFlags = m_dwFileFlags;
	return TRUE;
}

BOOL _XMiniVersion::GetFileOS(DWORD& rdwOS)
{
	rdwOS = m_dwFileOS;
	return TRUE;
}

BOOL _XMiniVersion::GetFileType(DWORD& rdwType)
{
	rdwType = m_dwFileType;
	return TRUE;
}

BOOL _XMiniVersion::GetFileSubtype(DWORD& rdwType)
{
	rdwType = m_dwFileSubtype;
	return TRUE;
}

BOOL _XMiniVersion::GetCompanyName(LPTSTR lpszCompanyName, int nSize)
{
	if (!lpszCompanyName)
		return FALSE;
	ZeroMemory(lpszCompanyName, nSize);
	lstrcpyn(lpszCompanyName, m_szCompanyName, nSize-1);
	return TRUE;
}

BOOL _XMiniVersion::GetFileDescription(LPTSTR lpszFileDescription, int nSize)
{
	if (!lpszFileDescription)
		return FALSE;
	ZeroMemory(lpszFileDescription, nSize);
	lstrcpyn(lpszFileDescription, m_szFileDescription, nSize-1);
	return TRUE;
}

BOOL _XMiniVersion::GetProductName(LPTSTR lpszProductName, int nSize)
{
	if (!lpszProductName)
		return FALSE;
	ZeroMemory(lpszProductName, nSize);
	lstrcpyn(lpszProductName, m_szProductName, nSize-1);
	return TRUE;
}

BOOL _XMiniVersion::GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo)
{
	BOOL rc;
	UINT nLength;
	VS_FIXEDFILEINFO *pFixedInfo = NULL;

	if (!m_pData)
		return FALSE;

	if (m_pData)
		rc = ::VerQueryValue(m_pData, _T("\\"), (void **) &pFixedInfo, &nLength);
	else
		rc = FALSE;
		
	if (rc)
		memcpy (&rFixedInfo, pFixedInfo, sizeof (VS_FIXEDFILEINFO));	

	return rc;
}

BOOL _XMiniVersion::GetStringInfo(LPCSTR lpszKey, LPTSTR lpszReturnValue)
{
	BOOL rc;
	DWORD *pdwTranslation;
	UINT nLength;
	LPTSTR lpszValue;
	
	if (m_pData == NULL)
		return FALSE;

	if (!lpszReturnValue)
		return FALSE;

	if (!lpszKey)
		return FALSE;

	*lpszReturnValue = 0;

	rc = ::VerQueryValue(m_pData, _T("\\VarFileInfo\\Translation"), 
								(void**) &pdwTranslation, &nLength);
	if (!rc)
		return FALSE;

	char szKey[2000];
	wsprintf(szKey, _T("\\StringFileInfo\\%04x%04x\\%s"),
				 LOWORD (*pdwTranslation), HIWORD (*pdwTranslation),
				 lpszKey);

	rc = ::VerQueryValue(m_pData, szKey, (void**) &lpszValue, &nLength);

	if (!rc)
		return FALSE;
		
	lstrcpy(lpszReturnValue, lpszValue);

	return TRUE;
}
