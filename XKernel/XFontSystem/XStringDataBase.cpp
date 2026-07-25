// XStringDataBase.cpp: implementation of the _XStringDataBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"
#include "XUnicodeConv.h"
#include "XStringDataBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCHAR	g_NoneString[] = _T(" ");//_T("NULL");
TCHAR	g_CompositeString[2048];

extern int _xvsprintf(char *buf, const char *fmt, void** args);

_XStringDataBase		g_StringDataBase;

_XSTRINGSECTIONINFO::~_XSTRINGSECTIONINFO()
{
	std::for_each( m_StringVector.begin(), m_StringVector.end(), DELETEOBJECT() );
	m_StringVector.clear();

	m_StringMap.clear();
}

_XStringDataBase::_XStringDataBase()
{
	m_TotalSectionCount =	0;
	m_pSectionInfo		=	NULL;
}

_XStringDataBase::~_XStringDataBase()
{
	Release();
}

//int g_strmaxlen = 0;

BOOL _XStringDataBase::Load( LPCTSTR filename, int languageindex )
{
	Release();

	FILE* pFile = fopen( filename, "rb" );

	if( !pFile )
	{
		_XFatalError( _T("Could not open %s file"), filename );
		return FALSE;
	}

// 	fseek(pFile,0,SEEK_END);
// 	int pSize = ftell(pFile);
// 	fseek(pFile,0,SEEK_SET);
// 	unsigned char* pBuffer = new unsigned char[pSize];
// 	pSize = fread(pBuffer,sizeof(unsigned char), pSize,pFile);
// 	fclose(pFile);

	DWORD header = 0x0000;
	fread( &header, sizeof(DWORD), 1, pFile );

	if( header != _XDEF_XSTRINGDBHEADER )
	{
		_XFatalError( _T("Could not support %s file : [%x]"), filename, header );
		fclose( pFile );
		return FALSE;
	}

	DWORD version = 0x0000;
	fread( &version, sizeof(DWORD), 1, pFile );

#ifdef _XDEF_NEWSTRINGDBFORMAT
	if( version != _XDEF_XSTRINGDBSEPARATEDVERSION )
	{
		_XFatalError( _T("Could not support this format [%x:%s]: "), version, filename );
		fclose( pFile );
		return FALSE;
	}
#else
	if( version != _XDEF_XSTRINGDBVERSION )
	{
		_XFatalError( _T("Could not support this format [%x:%s]: "), version, filename );
		fclose( pFile );
		return FALSE;
	}
#endif

	m_TotalSectionCount = 0;
	fread( &m_TotalSectionCount, sizeof(int), 1, pFile );

	m_pSectionInfo = new _XSTRINGSECTIONINFO[ m_TotalSectionCount ];
	
	typedef struct _XSTRINGSECTIONHEADERINFO
	{
		int		stringcnt;
		wchar_t	sectioname[64];
	} XSTRINGSECTIONREADINFO;

	_XSTRINGSECTIONHEADERINFO	sectionstchunk;
	wchar_t						readstring[2048];
#ifdef _XJAPANESE
	wchar_t						parsesecondstring[256];
#endif
	char						mbcsreadstring[2048];
	int							readstringlength;
	//wchar_t						koreanstring[2048];
	
	int i = 0 ; for(  i = 0; i < m_TotalSectionCount; i++ )
	{
		fread( &sectionstchunk, sizeof(_XSTRINGSECTIONHEADERINFO), 1, pFile );

		m_pSectionInfo[i].m_stringcnt = sectionstchunk.stringcnt;
		memset( m_pSectionInfo[i].m_sectioname, 0, sizeof(TCHAR) * 64 );
		//strcpy( m_pSectionInfo[i].m_sectioname, sectionstchunk.sectioname );
	
		_XTEXTSTRING_INFO*	pTextstringItem;

		int j = 0 ; for( j = 0; j < sectionstchunk.stringcnt; j++ )
		{
			pTextstringItem = new _XTEXTSTRING_INFO;
			
			fread( &pTextstringItem->iResourceIndex, sizeof(int), 1, pFile );


#ifdef _XDEF_NEWSTRINGDBFORMAT
			fread( &pTextstringItem->iParameterOrder, sizeof(int), 1, pFile );
#else
			int lancnt = 0 ;for( ; lancnt < _XDEF_MAXLANGUAGECOUNT; lancnt++ )
			{			
				if( languageindex == lancnt )
				{			
					fread( &pTextstringItem->iParameterOrder, sizeof(int), 1, pFile );
				}
				else
				{
					fseek( pFile, sizeof(int), SEEK_CUR );
				}
			}		
#endif			

#ifdef _XDEF_NEWSTRINGDBFORMAT
			fread( &readstringlength, sizeof(int), 1, pFile );
					
			memset( readstring, 0, sizeof(wchar_t) * 2048 );
			if( readstringlength > 0 )
			{
				fread( readstring, sizeof(wchar_t) * readstringlength, 1, pFile );
			}

			memset( mbcsreadstring, 0, sizeof(char) * 2048 );

			if( languageindex == _XLANGUAGE_TYPE_VIETNAMESE )
			{
				int strbyteslength = (wcslen(readstring) << 1);
				int outputmaxlength = 2048;
				_XUniCodeCompToCP1258( (LPBYTE)readstring, (LPBYTE)mbcsreadstring, strbyteslength, outputmaxlength );									
				
				if( outputmaxlength > 0 )
				{
					pTextstringItem->strTextString = new TCHAR[outputmaxlength+1];
					memcpy( pTextstringItem->strTextString, mbcsreadstring, outputmaxlength );

					// ------------------------------------------------------------┓
					// "\n"을 New line으로 변경
					BOOL bNewLine = FALSE;
					int sublength = 0;
					
					int i = 1 ; for( ; i < outputmaxlength ; i++)
					{
						if(pTextstringItem->strTextString[i] == 'n')
						{
							if(pTextstringItem->strTextString[i-1] == '\\')
							{
								pTextstringItem->strTextString[i-1] = '\n';
								sublength ++;
								bNewLine = TRUE;
								
								int j = 0 ; for( j = i ; j < outputmaxlength-1 ; j++)
								{
									pTextstringItem->strTextString[j] = pTextstringItem->strTextString[j+1];
								}
							}
						}
						if(pTextstringItem->strTextString[i] == '"')
						{
							if(pTextstringItem->strTextString[i-1] == '\\')
							{
								pTextstringItem->strTextString[i-1] = '\"';
								sublength++;
								bNewLine = TRUE;
								
								int j = 0 ; for( j = i ; j < outputmaxlength-1 ; j++)
								{
									pTextstringItem->strTextString[j] = pTextstringItem->strTextString[j+1];
								}
							}
						}
					}
					
					if(bNewLine)
					{
						int newlength = outputmaxlength - sublength;
						int j = 0 ; for( j = newlength ; j < outputmaxlength ; j++)
						{
							pTextstringItem->strTextString[j] = 0;
						}
					}
					// ------------------------------------------------------------┛
					pTextstringItem->strTextString[outputmaxlength] = NULL;
					
				}
				else
				{
					pTextstringItem->strTextString = new TCHAR[5];
					memset( pTextstringItem->strTextString, 0, sizeof(TCHAR) * 5 );
					strcpy( pTextstringItem->strTextString, _T("NULL"));
				}
				//int requirelength = WideCharToMultiByte(1258, WC_SEPCHARS, readstring, -1, mbcsreadstring, 2048, NULL, NULL);
				//pTextstringItem->strTextString = new TCHAR[requirelength];
			}
			else
			{
				UINT codepage = GetACP();

				if( languageindex == _XLANGUAGE_TYPE_TAIWANESE )
				{
					codepage = 950;
				}
				else if( g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
				{
					codepage = 932;
				}
				else if( g_LanguageType == _XLANGUAGE_TYPE_RUSSIAN )
				{
					codepage = 1251;
				}

#ifdef _XJAPANESE
				if( g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
				{
					if( i == _XSTRINGDB_SN_AREATITLE || i == _XSTRINGDB_SN_SKILL_TABLE_XLS_NAME || i == _XSTRINGDB_SN_MOBMODEL_XMS )
					{
						for( int ti = 0; ti < readstringlength; ti++ )
						{
							if( readstring[ti] == 0xFF08 ) // u'('
							{
								memset( parsesecondstring, 0, sizeof(wchar_t) * 256 );
								memcpy( parsesecondstring, readstring+ti+1, sizeof(wchar_t) * (readstringlength - ti - 2) );
								readstring[ti] = NULL;

								int strlength = wcslen( parsesecondstring );

								if( strlength > 0 )
								{
									int requirelength = WideCharToMultiByte(codepage, 0, parsesecondstring, -1, mbcsreadstring, 2048, NULL, NULL);
				
									if( requirelength > 0 )
									{
										pTextstringItem->strHiraganaTextString = new TCHAR[requirelength];
										strcpy( pTextstringItem->strHiraganaTextString , mbcsreadstring );
									}
								}

								break;
							}
						}
						
					}
				}
#endif

				int requirelength = WideCharToMultiByte(codepage, 0, readstring, -1, mbcsreadstring, 2048, NULL, NULL);
				
				if( requirelength > 0 )
				{
					pTextstringItem->strTextString = new TCHAR[requirelength];
					strcpy( pTextstringItem->strTextString, mbcsreadstring );

					// ------------------------------------------------------------┓
					// "\n"을 New line으로 변경
					BOOL bNewLine = FALSE;
					int sublength = 0;
					
					int i = 1 ; for( ; i < requirelength ; i++)
					{
						if(pTextstringItem->strTextString[i] == 'n') 
						{
							if(pTextstringItem->strTextString[i-1] == '\\')
							{
								pTextstringItem->strTextString[i-1] = '\n';
								sublength ++;
								bNewLine = TRUE;
								
								int j = 0 ; for( j = i ; j < requirelength-1 ; j++)
								{
									pTextstringItem->strTextString[j] = pTextstringItem->strTextString[j+1];
								}
							}
						}
						if(pTextstringItem->strTextString[i] == '"')
						{
							if(pTextstringItem->strTextString[i-1] == '\\')
							{
								pTextstringItem->strTextString[i-1] = '\"';
								sublength++;
								bNewLine = TRUE;
								
								int j = 0 ; for( j = i ; j < requirelength-1 ; j++)
								{
									pTextstringItem->strTextString[j] = pTextstringItem->strTextString[j+1];
								}
							}
						}
					}
					
					if(bNewLine)
					{
						int newlength = requirelength - sublength;
						int j = 0 ; for( j = newlength ; j < requirelength ; j++)
						{
							pTextstringItem->strTextString[j] = 0;
						}
					}
					// ------------------------------------------------------------┛
				}
				else
				{
					pTextstringItem->strTextString = new TCHAR[5];
					memset( pTextstringItem->strTextString, 0, sizeof(TCHAR) * 5 );
					strcpy( pTextstringItem->strTextString, _T("NULL"));
				}
			}

			m_pSectionInfo[i].m_StringVector.push_back( pTextstringItem );
			m_pSectionInfo[i].m_StringMap.insert(smdef_XTEXTSTRING_INFOMAP::value_type(pTextstringItem->iResourceIndex, pTextstringItem));

#else

			for( lancnt = 0; lancnt < _XDEF_MAXLANGUAGECOUNT; lancnt++ )
			{
				if( languageindex == lancnt )
				{
					fread( &readstringlength, sizeof(int), 1, pFile );
					
					memset( readstring, 0, sizeof(wchar_t) * 2048 );
					if( readstringlength > 0 )
					{
						fread( readstring, sizeof(wchar_t) * readstringlength, 1, pFile );
					}

					memset( mbcsreadstring, 0, sizeof(char) * 2048 );

					if( languageindex == _XLANGUAGE_TYPE_VIETNAMESE )
					{
						int strbyteslength = (wcslen(readstring) << 1);
						int outputmaxlength = 2048;
						_XUniCodeCompToCP1258( (LPBYTE)readstring, (LPBYTE)mbcsreadstring, strbyteslength, outputmaxlength );					
						
						/*
						if( g_strmaxlen < outputmaxlength )
						{
							g_strmaxlen = outputmaxlength;
						}
						*/

						if( outputmaxlength > 0 )
						{
							pTextstringItem->strTextString = new TCHAR[outputmaxlength+1];
							//strcpy( pTextstringItem->strTextString, mbcsreadstring );
							//memset( pTextstringItem->strTextString, 0, sizeof(char) * (outputmaxlength+1) );
							memcpy( pTextstringItem->strTextString, mbcsreadstring, outputmaxlength );

							//strcpy( pTextstringItem->strTextString, _T("TEMP") );							
							
							// ------------------------------------------------------------┓
							// "\n"을 New line으로 변경
							BOOL bNewLine = FALSE;
							int sublength = 0;
							
							int i = 1 ; for( ; i < outputmaxlength ; i++)
							{
								if(pTextstringItem->strTextString[i] == 'n')
								{
									if(pTextstringItem->strTextString[i-1] == '\\')
									{
										pTextstringItem->strTextString[i-1] = '\n';
										sublength ++;
										bNewLine = TRUE;
										
										int j = 0 ; for( j = i ; j < outputmaxlength-1 ; j++)
										{
											pTextstringItem->strTextString[j] = pTextstringItem->strTextString[j+1];
										}
									}
								}
								if(pTextstringItem->strTextString[i] == '"')
								{
									if(pTextstringItem->strTextString[i-1] == '\\')
									{
										pTextstringItem->strTextString[i-1] = '\"';
										sublength++;
										bNewLine = TRUE;
										
										int j = 0 ; for( j = i ; j < outputmaxlength-1 ; j++)
										{
											pTextstringItem->strTextString[j] = pTextstringItem->strTextString[j+1];
										}
									}
								}
							}
							
							if(bNewLine)
							{
								int newlength = outputmaxlength - sublength;
								int j = 0 ; for( j = newlength ; j < outputmaxlength ; j++)
								{
									pTextstringItem->strTextString[j] = 0;
								}
							}
							// ------------------------------------------------------------┛
							pTextstringItem->strTextString[outputmaxlength] = NULL;
							
						}
						else
						{
							pTextstringItem->strTextString = new TCHAR[5];
							memset( pTextstringItem->strTextString, 0, sizeof(TCHAR) * 5 );
							strcpy( pTextstringItem->strTextString, _T("NULL"));
						}
						//int requirelength = WideCharToMultiByte(1258, WC_SEPCHARS, readstring, -1, mbcsreadstring, 2048, NULL, NULL);
						//pTextstringItem->strTextString = new TCHAR[requirelength];
					}
					else
					{
						/*
						if( languageindex == _XLANGUAGE_TYPE_ENGLISH && lancnt == _XLANGUAGE_TYPE_ENGLISH ) // 영문일때 문자열이 비었으면 한글로 대체
						{
							if( wcslen(readstring) == 0 )
								wcscpy( readstring, koreanstring );
						}
						*/

						UINT codepage = GetACP();

						if( languageindex == _XLANGUAGE_TYPE_TAIWANESE )
						{
							codepage = 950;
						}
						else if( g_LanguageType == _XLANGUAGE_TYPE_JAPANESE )
						{
							codepage = 932;
						}
						else if( g_LanguageType == _XLANGUAGE_TYPE_RUSSIAN )
						{
							codepage = 1251;
						}

						int requirelength = WideCharToMultiByte(codepage, 0, readstring, -1, mbcsreadstring, 2048, NULL, NULL);
						
						//pTextstringItem->strTextString = new TCHAR[requirelength];
						//memset( pTextstringItem->strTextString, 0, sizeof( TCHAR ) * requirelength );

						if( requirelength > 0 )
						{
							pTextstringItem->strTextString = new TCHAR[requirelength];
							strcpy( pTextstringItem->strTextString, mbcsreadstring );
							
							// ------------------------------------------------------------┓
							// "\n"을 New line으로 변경
							BOOL bNewLine = FALSE;
							int sublength = 0;
							
							int i = 1 ; for( ; i < requirelength ; i++)
							{
								if(pTextstringItem->strTextString[i] == 'n') 
								{
									if(pTextstringItem->strTextString[i-1] == '\\')
									{
										pTextstringItem->strTextString[i-1] = '\n';
										sublength ++;
										bNewLine = TRUE;
										
										int j = 0 ; for( j = i ; j < requirelength-1 ; j++)
										{
											pTextstringItem->strTextString[j] = pTextstringItem->strTextString[j+1];
										}
									}
								}
								if(pTextstringItem->strTextString[i] == '"')
								{
									if(pTextstringItem->strTextString[i-1] == '\\')
									{
										pTextstringItem->strTextString[i-1] = '\"';
										sublength++;
										bNewLine = TRUE;
										
										int j = 0 ; for( j = i ; j < requirelength-1 ; j++)
										{
											pTextstringItem->strTextString[j] = pTextstringItem->strTextString[j+1];
										}
									}
								}
							}
							
							if(bNewLine)
							{
								int newlength = requirelength - sublength;
								int j = 0 ; for( j = newlength ; j < requirelength ; j++)
								{
									pTextstringItem->strTextString[j] = 0;
								}
							}
							// ------------------------------------------------------------┛
						}
						else
						{
							pTextstringItem->strTextString = new TCHAR[5];
							memset( pTextstringItem->strTextString, 0, sizeof(TCHAR) * 5 );
							strcpy( pTextstringItem->strTextString, _T("NULL"));
						}
					}					

					//pTextstringItem->strTextString = new TCHAR[5];
					//memset( pTextstringItem->strTextString, 0, sizeof(TCHAR) * 5 );
					//strcpy( pTextstringItem->strTextString, _T("NULL"));
					

					m_pSectionInfo[i].m_StringVector.push_back( pTextstringItem );
					m_pSectionInfo[i].m_StringMap.insert(smdef_XTEXTSTRING_INFOMAP::value_type(pTextstringItem->iResourceIndex, pTextstringItem));
				}
				else
				{
					fread( &readstringlength, sizeof(int), 1, pFile );

					/*
					if( languageindex == _XLANGUAGE_TYPE_ENGLISH && lancnt == _XLANGUAGE_TYPE_KOREAN ) // 영문일때 한글 미리 로드.
					{
						memset( koreanstring, 0, sizeof(wchar_t) * 2048 );
						fread( koreanstring, sizeof(wchar_t) * readstringlength, 1, pFile );
					}
					else
					*/
					{
						fseek( pFile, sizeof(wchar_t) * readstringlength, SEEK_CUR );
					}
				}
			}
#endif
		}
	}

	fclose( pFile );

	//_XLog( _T("Max tex size : %d"), g_strmaxlen );

	return TRUE;
}

void _XStringDataBase::Release( void )
{
	SAFE_DELETE_ARRAY( m_pSectionInfo );
	m_pSectionInfo		=	NULL;

	m_TotalSectionCount =	0;	
}

LPTSTR _XStringDataBase::GetTextString( _XSTRINGDB_SECTIONDEFINE sectionindex, int resourceid )
{
	if( sectionindex < 0 || m_TotalSectionCount <= sectionindex )
	{
		return g_NoneString;
	}


	smdef_XTEXTSTRING_INFOMAP::iterator it  = m_pSectionInfo[sectionindex].m_StringMap.find(resourceid);	
	if(it != m_pSectionInfo[sectionindex].m_StringMap.end())
	{
		_XTEXTSTRING_INFO* pStringItem = (*it).second;
		if(pStringItem)
		{
			if( pStringItem->strTextString[0] == 0 )
				return g_NoneString;
			else
				return pStringItem->strTextString;
		}
	}
	
	//_XLog("[string] === Not Found %d ===]", resourceid);
	return g_NoneString;
}

#ifdef _XJAPANESE
LPTSTR _XStringDataBase::GetSubTextString( _XSTRINGDB_SECTIONDEFINE sectionindex, int resourceid )
{
	if( sectionindex < 0 || m_TotalSectionCount <= sectionindex )
	{
		return NULL;
	}


	smdef_XTEXTSTRING_INFOMAP::iterator it  = m_pSectionInfo[sectionindex].m_StringMap.find(resourceid);	
	if(it != m_pSectionInfo[sectionindex].m_StringMap.end())
	{
		_XTEXTSTRING_INFO* pStringItem = (*it).second;
		if(pStringItem)
		{
			if( !pStringItem->strHiraganaTextString )
				return NULL;
			else
				return pStringItem->strHiraganaTextString;
		}
	}
	
	//_XLog("[string] === Not Found %d ===]", resourceid);
	return NULL;
}
#endif

int	_XStringDataBase::GetTextOrder( _XSTRINGDB_SECTIONDEFINE sectionindex, int resourceid )
{
	if( sectionindex < 0 || m_TotalSectionCount <= sectionindex )
	{
		return 0;
	}
	
	
	smdef_XTEXTSTRING_INFOMAP::iterator it  = m_pSectionInfo[sectionindex].m_StringMap.find(resourceid);	
	if(it != m_pSectionInfo[sectionindex].m_StringMap.end())
	{
		_XTEXTSTRING_INFO* pStringItem = (*it).second;
		if(pStringItem)
		{
			if( pStringItem->strTextString[0] == 0 )
				return 0;
			else
				return pStringItem->iParameterOrder;
		}
	}
	
	return 0;	
}

_XTEXTSTRING_INFO* _XStringDataBase::GetTextInfo( _XSTRINGDB_SECTIONDEFINE sectionindex, int resourceid )
{
	if( sectionindex < 0 || m_TotalSectionCount <= sectionindex )
	{
		return NULL;
	}
	
	
	smdef_XTEXTSTRING_INFOMAP::iterator it  = m_pSectionInfo[sectionindex].m_StringMap.find(resourceid);	
	if(it != m_pSectionInfo[sectionindex].m_StringMap.end())
	{
		_XTEXTSTRING_INFO* pStringItem = (*it).second;
		if(pStringItem)
		{
			return pStringItem;
		}
	}
	
	return NULL;
}


LPTSTR _XStringDataBase::CompositeString( _XSTRINGDB_SECTIONDEFINE sectionindex, int resourceid, 
										 LPVOID	param1, LPVOID param2, LPVOID param3, LPVOID param4,
										 LPVOID	param5, LPVOID param6, LPVOID param7, LPVOID param8)
{
	_XTEXTSTRING_INFO*	pTextInfo = GetTextInfo( sectionindex, resourceid );

	if( !pTextInfo )
	{
		return g_NoneString;
	}

	// no composite
	LPVOID	argPointer[8];
	if( pTextInfo->iParameterOrder == 0 ) 
	{
		argPointer[0] = param1; argPointer[1] = param2; 
		argPointer[2] = param3; argPointer[3] = param4; 
		argPointer[4] = param5; argPointer[5] = param6; 
		argPointer[6] = param7; argPointer[7] = param8;
		
		_xvsprintf(g_CompositeString, pTextInfo->strTextString, argPointer);
		return g_CompositeString;
	}

	int		numArgs;
	TCHAR	wordorderarray[8];	

	memset( wordorderarray, 0, sizeof(TCHAR) << 3 );
	itoa( pTextInfo->iParameterOrder, wordorderarray, 10 );
	
	memset( argPointer, 0, sizeof(LPVOID) << 3 );

	numArgs = strlen(wordorderarray);

	if( numArgs > 8 )
	{
		return g_NoneString;
	}

 	int i = 0 ; for(  i = 0; i < 8; i++ )
	{
		if( wordorderarray[i] == NULL ) 
		{			
			break;
		}

		switch( wordorderarray[i] ) 
		{
		case _T('1'): argPointer[i] = param1; break;
		case _T('2'): argPointer[i] = param2; break;
		case _T('3'): argPointer[i] = param3; break;
		case _T('4'): argPointer[i] = param4; break;
		case _T('5'): argPointer[i] = param5; break;
		case _T('6'): argPointer[i] = param6; break;
		case _T('7'): argPointer[i] = param7; break;
		case _T('8'): argPointer[i] = param8; break;
		}
	}		
	
	_xvsprintf(g_CompositeString, pTextInfo->strTextString, argPointer);

	return g_CompositeString;
}


LPTSTR _XStringDataBase::CompositeString( LPTSTR formatstring, int wordorder,
										 LPVOID	param1, LPVOID param2, LPVOID param3, LPVOID param4,
										 LPVOID	param5, LPVOID param6, LPVOID param7, LPVOID param8 )
{
	if( !formatstring )
	{
		return g_NoneString;
	}
	
	// no composite
	LPVOID	argPointer[8];

	if( wordorder == 0 ) 
	{
		argPointer[0] = param1; argPointer[1] = param2; 
		argPointer[2] = param3; argPointer[3] = param4; 
		argPointer[4] = param5; argPointer[5] = param6; 
		argPointer[6] = param7; argPointer[7] = param8;

		_xvsprintf(g_CompositeString, formatstring, argPointer);
		return g_CompositeString;
	}
	
	int		numArgs;
	TCHAR	wordorderarray[8];
		
	memset( wordorderarray, 0, sizeof(TCHAR) << 3 );
	itoa( wordorder, wordorderarray, 10 );
	
	memset( argPointer, 0, sizeof(LPVOID) << 3 );
	
	numArgs = strlen(wordorderarray);
	
	if( numArgs > 8 )
	{
		return g_NoneString;
	}
	
	int i = 0 ; for(  i = 0; i < 8; i++ )
	{
		if( wordorderarray[i] == NULL ) 
		{			
			break;
		}
		
		switch( wordorderarray[i] ) 
		{
		case _T('1'): argPointer[i] = param1; break;
		case _T('2'): argPointer[i] = param2; break;
		case _T('3'): argPointer[i] = param3; break;
		case _T('4'): argPointer[i] = param4; break;
		case _T('5'): argPointer[i] = param5; break;
		case _T('6'): argPointer[i] = param6; break;
		case _T('7'): argPointer[i] = param7; break;
		case _T('8'): argPointer[i] = param8; break;
		}
	}		
	
	_xvsprintf(g_CompositeString, formatstring, argPointer);
	
	return g_CompositeString;
}











