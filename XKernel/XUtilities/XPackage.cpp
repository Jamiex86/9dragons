// XPackage.cpp: implementation of the CPackage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "XKernel.h"
#include "XPackage.h"
#include "XSystem.h"
#include <crtdbg.h>

#define _FILEDATAENCODEKEY		0xA5

void _EncodeData( LPSTR string, BYTE key )
{
	if( !string ) return;

	/*
	assert( string );

	int length = strlen( (LPCSTR)string );

	LPSTR tptr = string;

	int i = 0 ; for(  i = 0; i < length; i++ )
	{
		*tptr ^= key;
		tptr++;
	}
	*/

	do 
	{
		if( *string == 0 ) 
			break;

		*string ^= key;
		string++;

	} while(1);
}

LPSTR getExtention( LPSTR String )
{	  
	int strSize = strlen( String );
	while( --strSize )
	{
		if( String[strSize]== '.' ) return String+strSize;
	}
	return String;
}

BOOL _XCheckExistFile( LPCSTR filename )
{
	HANDLE      hFind;
	WIN32_FIND_DATA rFind;	
	
	hFind = FindFirstFile ( filename, &rFind );
	
	if ( INVALID_HANDLE_VALUE == hFind )
	{
		FindClose ( hFind );
		return FALSE;
	}
	
	FindClose ( hFind );
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
_XPackageArchive::_XPackageArchive()
{
	m_PackageFileptr = NULL;
	m_EncryptPackageFileptr = NULL;

	m_PackefFileStartPosition = 0;

	m_UseVirtualMemory			= FALSE;
	m_PackageVirtualMemory		= NULL;
	m_PackageVirtualMemorySize	= 0;
	m_PackageFileSize			= 0;

	m_EncryptedPackage			= FALSE;

	memset( m_ArchiveName, 0, _MAX_PATH );
}

_XPackageArchive::~_XPackageArchive()
{
	if( m_PackageFileptr ) 
		fclose( m_PackageFileptr );

#ifdef _XDEF_NEWENCRYPTION_PACKDATA
	if( m_EncryptPackageFileptr )
		fclose(m_EncryptPackageFileptr);
#endif

	SAFE_DELETE_ARRAY( m_PackageVirtualMemory );

	m_smFileTable.clear();

	std::for_each(m_svFileInfo.begin(), m_svFileInfo.end(), DELETEOBJECT());
	m_svFileInfo.clear();
}

BOOL _XPackageArchive::ReOpenPackage_Thread( void )
{
	if( m_UseVirtualMemory )
	{
		if( m_PackageVirtualMemory ) return TRUE;
		else return FALSE;
	}
	else
	{
		if( !this->m_PackageFileptr )
		{
			if( m_ArchiveName[0] == NULL )
			{
				_XFatalError( "Package reopen_thread : Invalid archive name [NULL]" );
				return FALSE;
			}
			/*
			std::string strFileName = gModulePath;
			strFileName += "\\";
			strFileName += lpPath;
			strFileName += "\\";
			strFileName += m_ArchiveName;
			*/
			//this->m_PackageFileptr = fopen( strFileName.c_str(), "rb" );	
			if( !this->m_PackageFileptr )
			{
				this->m_PackageFileptr = fopen( m_ArchiveName, "rb" );
				if( !this->m_PackageFileptr )
				{
					_XFatalError( "Failed to open package file [%s]", m_ArchiveName );
					return FALSE;
				}
			}
		}
	}
	
	//_XDWINPRINT( "ReOpen package file [%s]\n", m_ArchiveName );
	return TRUE;
}

BOOL _XPackageArchive::ReOpenPackage( void )
{
	if( m_UseVirtualMemory )
	{
		if( m_PackageVirtualMemory ) return TRUE;
		else return FALSE;
	}
	else
	{
		if( m_ArchiveName[0] == NULL )
		{
			_XFatalError( "Package reopen : Invalid archive name [NULL]" );
			return FALSE;
		}

		if( !this->m_PackageFileptr )
		{
			this->m_PackageFileptr = fopen( m_ArchiveName, "rb" );
			if( !this->m_PackageFileptr )
			{
				_XFatalError( "Failed to open package file [%s]", m_ArchiveName );
				return FALSE;
			}
		}
	}
	
	//_XDWINPRINT( "ReOpen package file [%s]\n", m_ArchiveName );
	return TRUE;
}

BOOL _XPackageArchive::OpenPackage( LPCTSTR filename )
{
	if( !filename[0] ) 
		return FALSE;

	FinalizePackage();
	//_ASSERT(m_svFileInfo.size()==0);

	memset( m_ArchiveName, 0, _MAX_PATH );
	strcpy( m_ArchiveName, filename );

	this->m_PackageFileptr = fopen( filename, "rb" );

	if( !this->m_PackageFileptr )
	{
		#ifdef _XDEF_NEWENCRYPTION_PACKDATA
			// ..
		#else
			if( strcmpi( g_XScriptDecryptFilename, filename ) == 0 )
			{
				_XFatalError( "Access xcore script" );
			}
			else
		#endif
			{
				_XFatalError( "Failed to open package file [%s]", filename );
			}

		return FALSE;
	}

	_PACKAGEFILE_HEADER packagefileheader;

	if( fread( &packagefileheader, sizeof( _PACKAGEFILE_HEADER ), 1, m_PackageFileptr ) < 1 )
	{
		_XFatalError( "Package header loading [%s]", filename );
		FinalizePackage();
		return FALSE;
	}

	if( packagefileheader.HeaderIndicator != _HEADERINDICATOR_PACKAGEFILE )
	{
		_XFatalError( "Package header loading [%s]", filename );
		FinalizePackage();
		return FALSE;
	}

	if( packagefileheader.FileVersion != _HEADERINDICATOR_PACKAGEFILEVERSION )
	{
		_XFatalError( "Package version [%s]", filename );
		FinalizePackage();
		return FALSE;
	}

	int	nPackedFileCount = packagefileheader.PackedFileCount;
	int*		iPackageFilePositionArray;
	_XFileInfo* iPackageFilenameArray;

	iPackageFilePositionArray = new int[ nPackedFileCount ];

	if( !iPackageFilePositionArray )
	{
		_XFatalError( "Allocate position array [%s]", filename );
		FinalizePackage();
		return FALSE;
	}

	// Alloc.. & Read filename informations...
	iPackageFilenameArray = new _XFileInfo[ nPackedFileCount ];

	if( !iPackageFilenameArray )
	{
		_XFatalError( "Allocate resource descrpition array [%s]", filename );
		FinalizePackage();
		return FALSE;
	}

	WORD length;
	TCHAR tempbuffer[_MAX_PATH];
	LONG  filenameinfosize = 0;
	int i = 0;for( ; i < nPackedFileCount; i++ )
	{
		length = 0;
		memset( tempbuffer, 0, _MAX_PATH );

		if( fread( &length, sizeof(WORD), 1, m_PackageFileptr ) < 1 )
		{
			_XFatalError( "Read file descrpition array size [%s]", filename );
			FinalizePackage();
			return FALSE;
		}

		if( length > 0 )
		{
			if( fread( &tempbuffer , length, 1, m_PackageFileptr ) < 1 )
			{
				_XFatalError( "Read file descrpition array [%s]", filename );
				FinalizePackage();
				return FALSE;
			}
		}

		filenameinfosize += length;
		
		_EncodeData( tempbuffer, _ENCODEKEY );
		
		iPackageFilenameArray[i].Set( tempbuffer );
	}

	filenameinfosize += sizeof(WORD) * nPackedFileCount;

	if( fread( iPackageFilePositionArray, sizeof(int)*nPackedFileCount, 1, m_PackageFileptr ) < 1 )
	{
		_XFatalError( "Read position array [%s]", filename );
		FinalizePackage();
		return FALSE;
	}
	
	// by mahwang
	for( i = 0; i < nPackedFileCount; i++ )
	{
		XFILE_INFO* pFileInfo		= new XFILE_INFO;
		pFileInfo->strFileName		= iPackageFilenameArray[i].m_filename;
		pFileInfo->nLength			= iPackageFilenameArray[i].m_length;
		pFileInfo->nFilePosition	= iPackageFilePositionArray[i];
		m_svFileInfo.push_back(pFileInfo);
		m_smFileTable.insert(smdef_FileTable::value_type(iPackageFilenameArray[i].m_filename, i));
	}
	
	m_PackefFileStartPosition =  sizeof( _PACKAGEFILE_HEADER ) + (sizeof(int)*nPackedFileCount) + filenameinfosize;	

	if( m_UseVirtualMemory )
	{
		LONG curptr = ftell( m_PackageFileptr );

		fseek( m_PackageFileptr, 0L, SEEK_END );
		LONG endptr = ftell( m_PackageFileptr );
		
		LONG virtualmemsize = endptr - curptr;

		m_PackageVirtualMemory = new BYTE[ virtualmemsize ];

		if( !m_PackageVirtualMemory )
		{
			_XFatalError( "Can't allocate package virtual memory" );
			return FALSE;
		}

		fseek( m_PackageFileptr, curptr, SEEK_SET );
		if( fread( m_PackageVirtualMemory, sizeof(BYTE)*virtualmemsize, 1, m_PackageFileptr ) < 1) 
		{
			_XFatalError( "Can't read package to the virtual memory" );
			return FALSE;
		}

		m_PackageVirtualMemorySize = virtualmemsize;
		fclose( m_PackageFileptr );
		m_PackageFileptr = NULL;
	}
	else
	{
		fseek( m_PackageFileptr, 0L, SEEK_END );
		m_PackageFileSize = ftell( m_PackageFileptr );
		fseek( m_PackageFileptr, 0, SEEK_SET );
	}

	_XDWINPRINT( "Open package file [%s]\n", m_ArchiveName );

	delete[] iPackageFilePositionArray;
	delete[] iPackageFilenameArray;

	return TRUE;
}

void _XPackageArchive::ClosePackage( void )
{
	if( m_PackageFileptr )
	{
		fclose( m_PackageFileptr );
		m_PackageFileptr = NULL;
	}

#ifdef _XDEF_NEWENCRYPTION_PACKDATA
	if( m_EncryptedPackage )
	{
		// clear
		if( m_EncryptPackageFileptr )
		{
			fclose(m_EncryptPackageFileptr);
			m_EncryptPackageFileptr = NULL;
		}
			
		m_EncryptPackageFileptr = fopen( gPackTemporaryFilePath, _T("wb") );
		if( m_EncryptPackageFileptr )
		{
			BYTE tempdata = 0xEC;
			fwrite( &tempdata, sizeof(BYTE), 1, m_EncryptPackageFileptr );
			fclose( m_EncryptPackageFileptr );
			m_EncryptPackageFileptr = NULL;
		}			

		::DeleteFile( gPackTemporaryFilePath );
	}
#endif

//	_XDWINPRINT( "Closed package file [%s]\n", m_ArchiveName );
}

void _XPackageArchive::FinalizePackage( void )
{
	if( m_PackageFileptr )
	{
		fclose( m_PackageFileptr );
		m_PackageFileptr = NULL;
	}

	m_PackefFileStartPosition	= 0;
	m_PackageFileSize			= 0;

	SAFE_DELETE_ARRAY( m_PackageVirtualMemory );
	m_PackageVirtualMemorySize = 0;

	if( m_smFileTable.size() > 0 )
	{
		m_smFileTable.clear();
	}

	if( m_svFileInfo.size() > 0 )
	{
		std::for_each(m_svFileInfo.begin(), m_svFileInfo.end(), DELETEOBJECT());
		m_svFileInfo.clear();
	}

	//_XDWINPRINT( "Finalize package file [%s]\n", m_ArchiveName );

	//m_ArchiveName[0] = 0;//BM_Disable

#ifdef _XDEF_NEWENCRYPTION_PACKDATA
	if( m_EncryptedPackage )
	{
		if( m_EncryptPackageFileptr )
		{
			fclose(m_EncryptPackageFileptr);
			m_EncryptPackageFileptr = NULL;
		}
			
		// clear
		m_EncryptPackageFileptr = fopen( gPackTemporaryFilePath, _T("wb") );
		if( m_EncryptPackageFileptr )
		{
			BYTE tempdata = 0xEC;
			fwrite( &tempdata, sizeof(BYTE), 1, m_EncryptPackageFileptr );
			fclose( m_EncryptPackageFileptr );
			m_EncryptPackageFileptr = NULL;
		}			

		::DeleteFile( gPackTemporaryFilePath );
	}
#endif


}

LONG _XPackageArchive::GetPackedFilePosition( int file_id )
{
	if( !m_PackageFileptr )
	{
		if( !ReOpenPackage() )
			return -1;
	}
	
	if( m_svFileInfo.size() <= file_id )
	{
		_XFatalError( "Invalid Package ID  [%s : %d]", m_ArchiveName, file_id );
		return -1;
	}
	
	return m_svFileInfo[file_id]->nFilePosition;
}

LONG  _XPackageArchive::GetPackedFileSize( int file_id )
{
	if( m_UseVirtualMemory )
	{
		if( !m_PackageVirtualMemory )
		{
			_XFatalError( "Not ready package [%s : %d]", m_ArchiveName, file_id  );
			return -1;
		}

		if( m_svFileInfo.size() <= file_id )
		{
			_XFatalError( "Invalid Package ID  [%s : %d]", m_ArchiveName, file_id );
			return -1;
		}
		
		if( file_id < m_svFileInfo.size()-1 )
		{
			return m_svFileInfo[file_id+1]->nFilePosition - m_svFileInfo[file_id]->nFilePosition;
		}
		else // 팩키지의 맨끝 파일이면...
		{
					  // Total size
			return ( (m_PackefFileStartPosition + m_PackageVirtualMemorySize) - m_svFileInfo[file_id]->nFilePosition );
		}	
	}
	else
	{
		if( !m_PackageFileptr )		
		{
			if( !ReOpenPackage() )
				return -1;
		}

		if( m_svFileInfo.size() <= file_id )
		{
			_XFatalError( "Invalid Package ID  [%s : %d]", m_ArchiveName, file_id );
			return -1;
		}

		if( file_id < m_svFileInfo.size()-1 )
		{
			int size = m_svFileInfo[file_id+1]->nFilePosition - m_svFileInfo[file_id]->nFilePosition;
			_ASSERT( size > 0 );
			return size;
		}
		else // 팩키지의 맨끝 파일이면...
		{
			return ( m_PackageFileSize - m_svFileInfo[file_id]->nFilePosition );
		}	
	}

	return -1;
}

BOOL	_XPackageArchive::GetPackedFileInfo( int file_id, DWORD& position, DWORD& size )
{
	if( m_UseVirtualMemory )
	{
		if( !m_PackageVirtualMemory )
		{
			_XFatalError( "Not ready package [%s : %d]", m_ArchiveName, file_id  );
			return FALSE;
		}
		
		if( m_svFileInfo.size() <= file_id )
		{
			_XFatalError( "Invalid Package ID  [%s : %d]", m_ArchiveName, file_id );
			return FALSE;
		}
		
		if( file_id < m_svFileInfo.size()-1 )
		{
			position = m_svFileInfo[file_id]->nFilePosition;
			// Total size
			size = m_svFileInfo[file_id+1]->nFilePosition - m_svFileInfo[file_id]->nFilePosition;
		}
		else // 팩키지의 맨끝 파일이면...
		{
			// packed position
			position = m_svFileInfo[file_id]->nFilePosition;
			// Total size
			size = ( (m_PackefFileStartPosition + m_PackageVirtualMemorySize) - position );
		}	
	}
	else
	{
		if( !m_PackageFileptr )
		{
			if( !ReOpenPackage() )
				return FALSE;
		}
		
		if( m_svFileInfo.size() <= file_id )
		{
			_XFatalError( "Invalid Package ID  [%s : %d]", m_ArchiveName, file_id );
			return FALSE;
		}
		
		if( file_id < m_svFileInfo.size()-1 )
		{
			// packed position
			position = m_svFileInfo[file_id]->nFilePosition;
			// Total size
			size = m_svFileInfo[file_id+1]->nFilePosition - m_svFileInfo[file_id]->nFilePosition;
		}
		else // 팩키지의 맨끝 파일이면...
		{
			// packed position
			position = m_svFileInfo[file_id]->nFilePosition;

			// Total size
			size = ( m_PackageFileSize - m_svFileInfo[file_id]->nFilePosition );
		}	
	}
	
	return TRUE;
}

FILE* _XPackageArchive::GetPackedFile( int file_id )
{
	if( m_UseVirtualMemory )
	{
		return NULL;
	}

	if( !m_PackageFileptr )
	{
		if( !ReOpenPackage() )
			return NULL;
	}

	if( m_svFileInfo.size() <= file_id )
	{
		_XFatalError( "Invalid Package ID  [%s : %d]", m_ArchiveName, file_id );
		return NULL;
	}
	
	if( fseek( this->m_PackageFileptr, m_svFileInfo[file_id]->nFilePosition, SEEK_SET ) == 0 )
	{

#ifdef _XDEF_NEWENCRYPTION_PACKDATA

		if( m_EncryptedPackage )
		{
			long packedfilesize = 0;
			if( file_id < m_svFileInfo.size()-1 )
			{
				packedfilesize = m_svFileInfo[file_id+1]->nFilePosition - m_svFileInfo[file_id]->nFilePosition;
			}
			else // 팩키지의 맨끝 파일이면...
			{
				packedfilesize = ( m_PackageFileSize - m_svFileInfo[file_id]->nFilePosition );
			}

			if( m_EncryptPackageFileptr )
				fclose(m_EncryptPackageFileptr);

			if( ExportEncryptedFile( this->m_PackageFileptr, packedfilesize, gPackTemporaryFilePath ) )
			{
				m_EncryptPackageFileptr = fopen( gPackTemporaryFilePath, _T("rb") );

				if( !m_EncryptPackageFileptr )
					_XFatalError( "Open temporary file (En)" );

				return m_EncryptPackageFileptr;
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return this->m_PackageFileptr;
		}

#else
		return this->m_PackageFileptr;
#endif

	}
	else
	{
		DWORD ecode = GetLastError();
		_XFatalError( "Package not found [%s : %d,0x%X,%d]", m_ArchiveName, file_id, m_svFileInfo[file_id]->nFilePosition, ecode );
		return NULL;
	}

	return NULL;
}

FILE* _XPackageArchive::GetPackedFile( int file_id, int offset )
{
	if( m_UseVirtualMemory )
	{
		return NULL;
	}
	
	if( !m_PackageFileptr )
	{
		if( !ReOpenPackage() )
			return NULL;
	}
	
	if( m_svFileInfo.size() <= file_id )
	{
		_XFatalError( "Invalid Package ID  [%s : %d]", m_ArchiveName, file_id );
		return NULL;
	}
	
#ifdef _XDEF_NEWENCRYPTION_PACKDATA

	if( m_EncryptedPackage )
	{
		if( fseek( this->m_PackageFileptr, m_svFileInfo[file_id]->nFilePosition, SEEK_SET ) == 0 )
		{
			long packedfilesize = 0;
			if( file_id < m_svFileInfo.size()-1 )
			{
				packedfilesize = m_svFileInfo[file_id+1]->nFilePosition - m_svFileInfo[file_id]->nFilePosition;
			}
			else // 팩키지의 맨끝 파일이면...
			{
				packedfilesize = ( m_PackageFileSize - m_svFileInfo[file_id]->nFilePosition );
			}

			if( m_EncryptPackageFileptr )
				fclose(m_EncryptPackageFileptr);

			if( ExportEncryptedFile( this->m_PackageFileptr, packedfilesize, gPackTemporaryFilePath ) )
			{
				m_EncryptPackageFileptr = fopen( gPackTemporaryFilePath, _T("rb") );

				if( !m_EncryptPackageFileptr )
					_XFatalError( "Open temporary file (En)" );

				fseek( m_EncryptPackageFileptr, offset, SEEK_SET );
				return m_EncryptPackageFileptr;
			}
			else
			{
				return NULL;
			}
		}
	}
	else
	{
		if( fseek( this->m_PackageFileptr, m_svFileInfo[file_id]->nFilePosition + offset, SEEK_SET ) == 0 )
		{
			return this->m_PackageFileptr;
		}
		else
		{
			_XFatalError( "Package seek error [%s : %d]", m_ArchiveName, file_id );
			return NULL;
		}
	}

#else

	if( fseek( this->m_PackageFileptr, m_svFileInfo[file_id]->nFilePosition + offset, SEEK_SET ) == 0 )
	{
		return this->m_PackageFileptr;
	}
	else
	{
		_XFatalError( "Package seek error [%s : %d]", m_ArchiveName, file_id );
		return NULL;
	}

#endif
	
	return NULL;
}

FILE* _XPackageArchive::GetPackedFile( LPSTR file_name )
{	
	assert( file_name );

	if( m_UseVirtualMemory )
	{
		return NULL;
	}

	memset( g_LastAccessPackedFile, 0, _MAX_PATH );

	if( !file_name ) return NULL;

	strcpy( g_LastAccessPackedFile, file_name );

	if( !m_PackageFileptr )
	{
		if( !ReOpenPackage() )
			return NULL;
	}

	int resourceindex = FindResource( file_name );

	if( resourceindex == -1 )
	{		
		_XFatalError( "Can't find resource [%s : %s]", m_ArchiveName, file_name );
		return NULL;
	}

	return GetPackedFile( resourceindex );
}

LPBYTE _XPackageArchive::GetPackedMemory( int file_id )
{
	if( !m_UseVirtualMemory )
	{
		_XFatalError( "Not ready package [%s : %d]", m_ArchiveName, file_id  );
		return NULL;
	}
	
	if( m_svFileInfo.size() <= file_id )
	{
		_XFatalError( "Invalid Package ID  [%s : %d]", m_ArchiveName, file_id );
		return NULL;
	}
	
	return ( m_PackageVirtualMemory + (m_svFileInfo[file_id]->nFilePosition - m_svFileInfo[0]->nFilePosition ) );
}

LPBYTE _XPackageArchive::LoadPackedFileToMemory( LPSTR file_name, LONG* buffersize, BOOL reporterror )
{
	assert( file_name );
	if( buffersize ) *buffersize = 0;
	
	memset( g_LastAccessPackedFile, 0, _MAX_PATH );
	
	if( !file_name ) return NULL;
	
	strcpy( g_LastAccessPackedFile, file_name );

	if( m_UseVirtualMemory )
	{
		if( !m_PackageVirtualMemory )
		{
			_XFatalError( "Not ready package [%s : %s]", m_ArchiveName, file_name );
			return NULL;
		}
	}
	else
	{
		if( !m_PackageFileptr )
		{
			if( !ReOpenPackage() )
				return NULL;
		}
	}

	
	int resourceindex = FindResource( file_name );
	
	if( resourceindex == -1 )
	{		
		if( reporterror ) _XFatalError( "Can't find resource [%s : %s]", m_ArchiveName, file_name );
		return NULL;
	}
	
	LPBYTE pResourceData = NULL;

	if( m_UseVirtualMemory )
	{
		LONG filesize = GetPackedFileSize( resourceindex );
		
		if( filesize <= 0 )
		{
			_XFatalError( "Invalid Resource size [%s : %s]", m_ArchiveName, file_name );
			return NULL;
		}

		if( buffersize ) *buffersize = filesize;

		return GetPackedMemory( resourceindex );
	}
	else
	{
		LONG filesize = GetPackedFileSize( resourceindex );

		if( filesize <= 0 )
		{
			_XFatalError( "Invalid Resource size [%s : %s]", m_ArchiveName, file_name );
			return NULL;
		}

		FILE* fp = GetPackedFile( resourceindex );
		if( !fp )
		{
			return NULL;
		}
				
		pResourceData = new BYTE[ filesize ];
		if( !pResourceData )
		{
			_XFatalError( "Can't allocate Resource buffer [%s : %s]", m_ArchiveName, file_name );
			return NULL;	
		}
		
		if( fread( pResourceData, filesize, 1, fp  ) < 1 )
		{
			delete pResourceData;
			_XFatalError( "Can't read resource data [%s : %s]", m_ArchiveName, file_name );
			return NULL;	
		}

		if( buffersize ) *buffersize = filesize;
	}
	
	return pResourceData;
}

LPBYTE _XPackageArchive::LoadPackedFileToMemory( DWORD resourceindex, LONG* buffersize )
{	
	if( buffersize ) *buffersize = 0;
	
	memset( g_LastAccessPackedFile, 0, _MAX_PATH );
			
	if( m_UseVirtualMemory )
	{
		if( !m_PackageVirtualMemory )
		{
			_XFatalError( "Not ready package [%s : [ri:%d]]", m_ArchiveName, resourceindex );
			return NULL;
		}
	}
	else
	{
		if( !m_PackageFileptr )
		{
			if( !ReOpenPackage() )
				return NULL;
		}
	}

	LONG filesize = GetPackedFileSize( resourceindex );	
	if( filesize <= 0 )
	{
		_XFatalError( "Invalid Resource size [%s : [ri:%d]]", m_ArchiveName, resourceindex );
		return NULL;
	}

	LPBYTE pResourceData = NULL;

	if( m_UseVirtualMemory )
	{
		if( filesize <= 0 )
		{
			_XFatalError( "Invalid Resource size [%s : [ri:%d]]", m_ArchiveName, resourceindex );
			return NULL;
		}
		
		if( buffersize ) *buffersize = filesize;
		
		return GetPackedMemory( resourceindex );
	}
	else
	{		
		FILE* fp = GetPackedFile( resourceindex );
		if( !fp )
		{
			return NULL;
		}
				
		pResourceData = new BYTE[ filesize ];
		if( !pResourceData )
		{
			_XFatalError( "Can't allocate Resource buffer [%s : [ri:%d]]", m_ArchiveName, resourceindex );
			return NULL;	
		}
		
		if( fread( pResourceData, filesize, 1, fp  ) < 1 )
		{
			delete pResourceData;
			_XFatalError( "Can't read resource data [%s : [ri:%d]]", m_ArchiveName, resourceindex );
			return NULL;	
		}
		
		if( buffersize ) *buffersize = filesize;
	}

	return pResourceData;
}

int  _XPackageArchive::FindResource( LPCTSTR resourcename )
{
	assert( resourcename );
	if( !resourcename ) return -1;

	smdef_FileTable::iterator it = m_smFileTable.find(resourcename);
	if ( it != m_smFileTable.end() )
	{
		return (*it).second;
	}
	return -1;
}

LPCTSTR _XPackageArchive::GetResourceName( int file_id )
{
	if( !m_PackageFileptr )
	{
		if( !ReOpenPackage() )
			return NULL;
	}

	if( m_svFileInfo.size() <= file_id )
	{
		_XFatalError( "Invalid Package ID [%s : %d]", m_ArchiveName, file_id );
		return NULL;
	}
	
	return m_svFileInfo[ file_id ]->strFileName.c_str();
}

int	_XPackageArchive::GetResourceCount( void )
{
	if( !m_PackageFileptr ) return 0;

	return m_svFileInfo.size();
}


#include "imagehlp.h" 

BOOL _XPackageArchive::ExportFile( int file_id, LPCTSTR filename )
{	
	if( !filename || !filename[0] ) return FALSE;

	FILE* pExportFile = NULL;
	FILE* pPackageFile = this->GetPackedFile( file_id );	
	if( !pPackageFile )	return FALSE;

	TCHAR FilePath[ _MAX_PATH+1 ];				
	memset( FilePath, 0, _MAX_PATH+1 );
	strncpy( FilePath, filename, _MAX_PATH );
		
	int i = 0 ; for(  i = strlen(FilePath)-1; i > 0; i-- )
	{
		if( FilePath[i] == '\\' ) break;
		else FilePath[i] = NULL;
	}

	if(FilePath[0]) MakeSureDirectoryPathExists( FilePath );
	
	
	//if( CheckExistFile( filename, TRUE ) ) CFile::Remove( filename );

	
	pExportFile = fopen( filename, "wb" );

	if( !pExportFile )
	{
		_XFatalError( "Export file open [%s : %s]", m_ArchiveName, filename );
		return FALSE;
	}
	
	long filesize = 0L;
	long currentfilepos = 0L;
	int  databuffer = 0;
		
	if( file_id < m_svFileInfo.size()-1 )
	{		
		filesize = m_svFileInfo[file_id+1]->nFilePosition - m_svFileInfo[file_id]->nFilePosition;
	}	
	else // 팩키지의 맨끝 파일이면...
	{
		filesize = -1;		
	}


	do
	{
		if( feof( pPackageFile ) ) break;

		databuffer = fgetc( pPackageFile );

		if( databuffer == EOF ) break;

		if( EOF == fputc( databuffer , pExportFile ) )
		{
			_XFatalError( "Export data [%s : %s]", m_ArchiveName, filename );
			break;
		}
		
		currentfilepos++;

		//한개 파일의 끝이면 종료..
		if( filesize != -1 )
		{
			if( filesize == currentfilepos ) break;
		}

	}while( !feof( pPackageFile ) );

	fclose( pExportFile );

	return TRUE;
}

BOOL _XPackageArchive::ExportEncryptedFile( FILE* pPackedFilePtr, LONG filesize, LPCTSTR filename )
{
	if( !filename || !filename[0] ) 
	{
		_XFatalError( "Invalid file name [NULL]" );
		return FALSE;
	}

	FILE* pExportFile = NULL;		
	pExportFile = fopen( filename, "wb" );

	if( !pExportFile )
	{
		_XFatalError( "Export file open (En)" );
		return FALSE;
	}

	#define _TEMPREADBUFFERSIZE	65536
	
	BYTE ReadBuffer[_TEMPREADBUFFERSIZE];

	long nextreadsize = _TEMPREADBUFFERSIZE;
	long leftsize = filesize;

	while( leftsize > 0 )
	{
		if( leftsize < _TEMPREADBUFFERSIZE )
		{
			nextreadsize = leftsize;
		}
		else
		{
			nextreadsize = _TEMPREADBUFFERSIZE;
		}
		
		if( fread( ReadBuffer, nextreadsize, 1, pPackedFilePtr ) < 1 )
		{
			_XFatalError( "Can't read enc pack data file" );			
			
			fclose( pExportFile );
			::DeleteFile( filename );

			FinalizePackage();
			return FALSE;
		}

		for( register int i = 0; i < nextreadsize; i++ )
		{
			ReadBuffer[i] ^= _FILEDATAENCODEKEY;
		}

		if( fwrite( ReadBuffer, nextreadsize, 1, pExportFile ) < 1 )
		{
			_XFatalError( "Can't write decrypt data file" );
			
			fclose( pExportFile );
			::DeleteFile( filename );

			FinalizePackage();
			return FALSE;
		}

		leftsize -= nextreadsize;
	}
	
	fclose( pExportFile );

	return TRUE;
}


BOOL _XPackageArchive::Patch( LPCTSTR packagefilename, LPCTSTR patchinfofilename )
{

/*
	if( !packagefilename[0] || !patchinfofilename[0] ) return FALSE;

	if( m_PackageFileptr ) this->Close();
	
	if( !this->Open( packagefilename ) )
	{		
		return FALSE;
	}

	FILE* pPatchinfoFile = NULL;

	pPatchinfoFile = fopen( patchinfofilename, "rt" );

	if( !pPatchinfoFile )
	{
		_DXMessage( "ERROR : Upgrade information file open" );
		this->Close();
		return FALSE;
	}

	TCHAR  parsingString[256];
	int   packageindexcount = 0;
	FILE* pCurrentpackagefile = NULL;

	TCHAR  currentPathString[_MAX_PATH];
	CString tFilenamestring;
	CString tMessagestring;

	::GetCurrentDirectory( _MAX_PATH, currentPathString );

	while( !feof( pPatchinfoFile ) )
	{
		if( feof( pPatchinfoFile ) ) break;

		memset( parsingString, 0, 256 );
		fgets( parsingString, 255, pPatchinfoFile );

		if( strlen(parsingString) && parsingString[0] != '\n' && parsingString[0] != ';' )
		{
			if( this->GetPackedFile( packageindexcount ) )
			{				
				tFilenamestring = currentPathString;
				
				tFilenamestring += parsingString;

				tFilenamestring.Remove( '\n' );				

				if( !this->ExportFile( packageindexcount, tFilenamestring ) ) break;

			}
			packageindexcount++;
		}
	}
	
	fclose( pPatchinfoFile );
	this->Close();
*/

	return TRUE;
}

/*
BOOL CheckExistFile( LPCSTR filename, BOOL resetAttrib )
{
	HANDLE      hFind;
	WIN32_FIND_DATA rFind;	

	hFind = FindFirstFile ( filename, &rFind );

	if ( INVALID_HANDLE_VALUE == hFind )
	{
		FindClose ( hFind );
		return FALSE;
	}

	FindClose ( hFind );

	if( resetAttrib )
	{
		CFileStatus filestatus;
		CFile::GetStatus( filename, filestatus );
		filestatus.m_attribute = 0x00; // normal file attrib setting
		CFile::SetStatus( filename, filestatus );
	}

	return TRUE;
}


BOOL WriteDirectory(CString dd)
{


	HANDLE		fFile;					// File Handle
	WIN32_FIND_DATA fileinfo;			// File Information Structure
	CStringArray	m_arr;				// CString Array to hold Directory Structures
	BOOL tt;							// BOOL used to test if Create Directory was successful
	int x1 = 0;							// Counter
	CString tem = "";					// Temporary CString Object

	// Before we go to a lot of work.  
	// Does the file exist

	fFile = FindFirstFile(dd,&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}

	m_arr.RemoveAll();					// Not really necessary - Just habit
	for(x1=0;x1<dd.GetLength();x1++)	// Parse the supplied CString Directory String
	{									
		if(dd.GetAt(x1) != '\\')		// if the Charachter is not a \ 
			tem += dd.GetAt(x1);		// else add character to Temp String
		else
		{
			m_arr.Add(tem);				// if the Character is a \ Add the Temp String to the CString Array
			tem += "\\";				// Now add the \ to the temp string
		}
		if(x1 == dd.GetLength()-1)		// If we reached the end of the file add the remaining string
			m_arr.Add(tem);
	}


	// Close the file
	FindClose(fFile);
	
	// Now lets cycle through the String Array and create each directory in turn
	for(x1 = 1;x1<m_arr.GetSize();x1++)
	{
		tem = m_arr.GetAt(x1);
		tt = CreateDirectory(tem,NULL);

		// If the Directory exists it will return a false
		if(tt)
			SetFileAttributes(tem,FILE_ATTRIBUTE_NORMAL);
		// If we were successful we set the attributes to normal
	}
	m_arr.RemoveAll();
	//  Now lets see if the directory was successfully created
	fFile = FindFirstFile(dd,&fileinfo);

	// if the file exists and it is a directory
	if(fileinfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		//  Directory Exists close file and return
		FindClose(fFile);
		return TRUE;
	}
	else
	{
		FindClose(fFile);
		return FALSE;
	}
}

*/

BOOL _XPackageArchive::CreateDecryptFile( LPTSTR orgpackagefilename, LPTSTR newpackagefilename )
{
	if( !OpenPackage( orgpackagefilename ) )
	{
		return FALSE;
	}

	LPBYTE pLoadBuffer = new BYTE[m_PackefFileStartPosition];
	if( !pLoadBuffer )
	{
		_XFatalError( "Can't create decrypt buffer [header]" );
		FinalizePackage();
		return FALSE;
	}

	FILE* pSrcFile = fopen( orgpackagefilename, "rb" );
	if( !pSrcFile )
	{
		_XFatalError( "Can't open src encrypt file" );
		delete[] pLoadBuffer;
		FinalizePackage();
		return FALSE;
	}
	
	FILE* pDestFile = fopen( newpackagefilename, "wb" );
	if( !pDestFile )
	{
		_XFatalError( "Can't create dest decrypt file" );
		delete[] pLoadBuffer;
		fclose( pSrcFile );
		FinalizePackage();
		return FALSE;
	}

	if( fread( pLoadBuffer, m_PackefFileStartPosition, 1, pSrcFile ) < 1 )
	{
		_XFatalError( "Can't read src encrypt file" );
		delete[] pLoadBuffer;
		fclose( pSrcFile );
		fclose( pDestFile );
		_XDeleteFile( newpackagefilename );
		FinalizePackage();
		return FALSE;
	}

	if( fwrite( pLoadBuffer, m_PackefFileStartPosition, 1, pDestFile ) < 1 )
	{
		_XFatalError( "Can't write dest decrypt file" );
		delete[] pLoadBuffer;
		fclose( pSrcFile );
		fclose( pDestFile );
		_XDeleteFile( newpackagefilename );
		FinalizePackage();
		return FALSE;
	}

	delete[] pLoadBuffer;

	rewind( pSrcFile );
	fseek( pSrcFile, 0, SEEK_END );
	long filesize = ftell( pSrcFile );
	long leftsize = filesize - m_PackefFileStartPosition;

	pLoadBuffer = new BYTE[4096];

	fseek( pSrcFile, m_PackefFileStartPosition, SEEK_SET );

	long nextreadsize = 4096;

	while( leftsize > 0 )
	{
		if( leftsize < 4096  )
		{
			nextreadsize = leftsize;
		}
		else
		{
			nextreadsize = 4096;
		}
		
		if( fread( pLoadBuffer, nextreadsize, 1, pSrcFile ) < 1 )
		{
			_XFatalError( "Can't read encrypt data file" );
			delete[] pLoadBuffer;
			fclose( pSrcFile );
			fclose( pDestFile );
			_XDeleteFile( newpackagefilename );
			FinalizePackage();
			return FALSE;
		}

		for( register int i = 0; i < nextreadsize; i++ )
		{
			pLoadBuffer[i] ^= _FILEDATAENCODEKEY;
		}

		if( fwrite( pLoadBuffer, nextreadsize, 1, pDestFile ) < 1 )
		{
			_XFatalError( "Can't write decrypt data file" );
			delete[] pLoadBuffer;
			fclose( pSrcFile );
			fclose( pDestFile );
			_XDeleteFile( newpackagefilename );
			FinalizePackage();
			return FALSE;
		}

		leftsize -= nextreadsize;
	}
	
	
	delete[] pLoadBuffer;

	fclose( pSrcFile );
	fclose( pDestFile );

	FinalizePackage();

	SetFileAttributes( newpackagefilename, FILE_ATTRIBUTE_HIDDEN );

	return TRUE;
}


