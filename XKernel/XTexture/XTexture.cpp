#include "stdafx.h"
#include "XKernel.h"
#include <crtdbg.h>

extern _XPackageArchive	 g_TextureAlphaInfoArchive;

LPBYTE g_TextureLoadBuffer = NULL;
DWORD  g_TextureLoadBufferSize = 0L;

LPBYTE g_TextureLoadBufferForThread = NULL;
DWORD  g_TextureLoadBufferSizeForThread = 0L;

LPBYTE g_MeshLoadBufferForThread = NULL;
DWORD  g_MeshLoadBufferSizeForThread = 0L;

DWORD  g_TextureFilter = D3DX_FILTER_BOX; //D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR;

//#define _XDEF_PRELOADINTERFACEIMAGE

BOOL _XInitializeTextureLoadBuffer( DWORD blockloadingsize, DWORD backgroundloadingsize, DWORD meshloadbuffersize )
{
	SAFE_DELETE_ARRAY( g_TextureLoadBuffer );
	SAFE_DELETE_ARRAY( g_TextureLoadBufferForThread );

	g_TextureLoadBufferSizeForThread = 0L;
	g_TextureLoadBufferSize = 0L;

#ifdef _XDEF_MODELTHREADLOADING	
	SAFE_DELETE_ARRAY( g_MeshLoadBufferForThread );
	g_MeshLoadBufferSizeForThread = 0L;	
#endif
	
	g_TextureLoadBuffer = new BYTE[ blockloadingsize ];
	
	if( !g_TextureLoadBuffer )
	{
		_XFatalError( "Can't allocate memory : texture buffer [%dBytes]", blockloadingsize );
		return FALSE;
	}
	g_TextureLoadBufferSize = blockloadingsize;

	g_TextureLoadBufferForThread = new BYTE[ backgroundloadingsize ];
	if( !g_TextureLoadBufferForThread )
	{
		_XFatalError( "Can't allocate memory : texture buffer for background thread [%dBytes]", backgroundloadingsize );
		return FALSE;
	}

	g_TextureLoadBufferSizeForThread = backgroundloadingsize;

#ifdef _XDEF_MODELTHREADLOADING	
	g_MeshLoadBufferForThread = new BYTE[ meshloadbuffersize ];
	if( !g_MeshLoadBufferForThread )
	{
		_XFatalError( "Can't allocate memory : mesh buffer for background thread [%dBytes]", meshloadbuffersize );
		return FALSE;
	}
	g_MeshLoadBufferSizeForThread = meshloadbuffersize;
#endif

	TCHAR packagefilename[_MAX_PATH];
	packagefilename[0] = NULL;
	strcpy( packagefilename, gModulePath );
	strcat( packagefilename, _T("\\Data\\Texture\\") );
	
#ifdef _XTESTSERVER
	strcat( packagefilename, _T("tsMAININTERFACEACDATA.XP") );
#else
	strcat( packagefilename, _T("MAININTERFACEACDATA.XP") );
#endif

	g_TextureAlphaInfoArchive.OpenPackage( packagefilename );

	g_TextureAlphaInfoArchive.ClosePackage();
	
	return TRUE;
}

void _XReleaseTextureLoadBuffer( void )
{
	SAFE_DELETE_ARRAY( g_TextureLoadBuffer );
	g_TextureLoadBufferSize = 0L;

	SAFE_DELETE_ARRAY( g_TextureLoadBufferForThread );
	g_TextureLoadBufferSizeForThread = 0L;

#ifdef _XDEF_MODELTHREADLOADING
	SAFE_DELETE_ARRAY( g_MeshLoadBufferForThread );
	g_MeshLoadBufferSizeForThread = 0L;
#endif

	g_TextureAlphaInfoArchive.FinalizePackage();
}


_XTexture::_XTexture()
{
	ZeroMembers();
}

_XTexture::~_XTexture()
{
	ReleaseDeviceObjects();
}

void _XTexture::ZeroMembers()
{	
	m_miplevel		= 0;
	m_width			= 0;
	m_height		= 0;
	m_tex			= NULL;	
	m_alphachannel	= NULL;
	m_initialized	= false;
	m_noresize		= false;
	m_LastUsedTime  = 0;
	m_Format		= D3DFMT_A8R8G8B8;

	m_ThreadLoad	= false;
}

bool _XTexture::ReLoad( LPBYTE pVMPtr, int size )
{
	if( m_initialized ) return true;
	
	if( !pVMPtr ) return false;
	SAFE_RELEASE(m_tex);
	
	HRESULT result;
	D3DXIMAGE_INFO	imageinfo;

	if( FAILED( result = D3DXCreateTextureFromFileInMemoryEx( gpDev, pVMPtr, size, 
		m_width, m_height, m_miplevel, 0, m_Format, g_TextureMemoryPool,
		g_TextureFilter,
		g_TextureFilter,
		0, &imageinfo, NULL, &m_tex ) ) )
	{
		_XFatalError( "Can't create texture. result:%d",result );
		return false;
	}
	
	m_width = imageinfo.Width;
	m_height = imageinfo.Height;

	m_initialized = true;		
	return true;
}

bool _XTexture::ReLoad( FILE* pVFPtr, int size )
{
	if( m_initialized ) return true;

	if( !pVFPtr ) return false;
	SAFE_RELEASE(m_tex);

	if( g_TextureLoadBuffer )
	{
		if( g_TextureLoadBufferSize < size  )
		{
			SAFE_DELETE_ARRAY( g_TextureLoadBuffer );
			g_TextureLoadBuffer = new BYTE[ size ];			
			if( !g_TextureLoadBuffer )
			{
				_XFatalError( "Can't allocate memory for virtual file [%dBytes]", size );
				g_TextureLoadBufferSize = 0L;
				return false;
			}

			g_TextureLoadBufferSize = size;
		}
	}
	else
	{
		g_TextureLoadBuffer = new BYTE[ size ];
		if( !g_TextureLoadBuffer )
		{
			_XFatalError( "Can't allocate memory for virtual file [%dBytes]", size );
			g_TextureLoadBufferSize = 0L;
			return false;
		}

		g_TextureLoadBufferSize = size;
	}

	if( fread( g_TextureLoadBuffer, size, 1, pVFPtr ) < 1 )
	{
		_XFatalError( "Can't read virtual file data" );
		return false;
	}
	
	HRESULT result;
	D3DXIMAGE_INFO	imageinfo;

	if( m_width == 0 || m_height == 0 )
	{
		D3DXIMAGE_INFO pSrcInfo;
		if( FAILED( result = D3DXGetImageInfoFromFileInMemory( g_TextureLoadBuffer, size, &pSrcInfo ) ) )
		{
			_XFatalError( "Can't read texture informations. result:%d", result );
			return false;
		}
		m_width  = pSrcInfo.Width;
		m_height = pSrcInfo.Height;
		m_Format = pSrcInfo.Format;
	}

	if( FAILED( result = D3DXCreateTextureFromFileInMemoryEx( gpDev, g_TextureLoadBuffer, size, 
							m_width, m_height, m_miplevel, 0, m_Format, g_TextureMemoryPool,
							g_TextureFilter,
							g_TextureFilter,
							0, &imageinfo, NULL, &m_tex ) ) )
	{
		_XFatalError( "Can't create texture. result:%d",result );
		return false;
	}

	m_width = imageinfo.Width;
	m_height = imageinfo.Height;

	m_initialized = true;
		
	return true;
}

bool _XTexture::ThreadReLoad( LPBYTE pVMPtr, int size )
{
	_ASSERT(0); // 현재는 Virtual memory system을 사용하지 않는다.
	return Load(pVMPtr, size);
}

bool _XTexture::ThreadReLoad( FILE* pVFPtr, int size )
{
	if( m_initialized ) return true;

	if( !pVFPtr ) return false;
	SAFE_RELEASE(m_tex);

	if( g_TextureLoadBufferSizeForThread < size )
	{
		SAFE_DELETE_ARRAY( g_TextureLoadBufferForThread );
		g_TextureLoadBufferForThread = new BYTE[size];
		if( !g_TextureLoadBufferForThread )
		{
			_XFatalError( "Can't allocate memory for virtual file [%dBytes]", size );
			g_TextureLoadBufferSizeForThread = 0;
			return false;
		}

		g_TextureLoadBufferSizeForThread = size;
	}

	if( fread( g_TextureLoadBufferForThread, size, 1, pVFPtr ) < 1 )
	{
		_XFatalError( "Can't read virtual file data" );
		return false;
	}

	HRESULT result;
	D3DXIMAGE_INFO pSrcInfo;
	if( FAILED( result = D3DXGetImageInfoFromFileInMemory( g_TextureLoadBufferForThread, size, &pSrcInfo ) ) )
	{
		_XFatalError( "Can't read texture informations. result:%d",result );
		return false;
	}
	m_width  = pSrcInfo.Width;
	m_height = pSrcInfo.Height;
	m_Format = pSrcInfo.Format;
	
	m_texturequality = g_TextureQuality;
	
	if( !m_noresize )
	{
		if( g_TextureQuality > 1 )
		{
			m_width  = pSrcInfo.Width  / g_TextureQuality;
			m_height = pSrcInfo.Height / g_TextureQuality;
		}
	}
	
	if( g_MaxTextureWidth  < m_width  ) m_width  = g_MaxTextureWidth;
	if( g_MaxTextureHeight < m_height ) m_height = g_MaxTextureHeight;
	
	if( FAILED( result = D3DXCreateTextureFromFileInMemoryEx( gpDev, g_TextureLoadBufferForThread, size, 
							m_width, m_height, m_miplevel, 0, m_Format, g_TextureMemoryPool,
							g_TextureFilter,
							g_TextureFilter,
							0, NULL, NULL, &m_tex ) ) )
	{
		_XFatalError( "Can't create texture. result:%d",result );
		return false;
	}
	
	m_initialized = true;
		
	return true;
}

bool _XTexture::Load(FILE* pVFPtr, int size, DWORD mips, bool noresize, bool createalphachannel )
{
	ReleaseDeviceObjects();
	if( !pVFPtr ) return false;

	if( mips <= 0 ) mips = 1;
	m_miplevel = mips;
	m_noresize = noresize;

	HRESULT result;

	if( createalphachannel )
	{
#ifdef _XDEF_PRELOADINTERFACEIMAGE
		if( g_TextureLoadBuffer )
		{
			if( g_TextureLoadBufferSize < size  )
			{
				SAFE_DELETE_ARRAY( g_TextureLoadBuffer );
				g_TextureLoadBuffer = new BYTE[ size ];
				if( !g_TextureLoadBuffer )
				{
					_XFatalError( "Can't allocate memory for virtual file [%dBytes]", size );
					g_TextureLoadBufferSize = 0L;
					return false;
				}
				
				g_TextureLoadBufferSize = size;
			}
		}
		else
		{
			g_TextureLoadBuffer = new BYTE[ size ];
			if( !g_TextureLoadBuffer )
			{
				_XFatalError( "Can't allocate memory for virtual file [%dBytes]", size );
				g_TextureLoadBufferSize = 0L;
				return false;
			}
			
			g_TextureLoadBufferSize = size;
		}
		
		if( fread( g_TextureLoadBuffer, size, 1, pVFPtr ) < 1 )
		{
			_XFatalError( "Can't read virtual file data" );
			return false;
		}

		D3DXIMAGE_INFO	imageinfo;

		if( FAILED( result = D3DXCreateTextureFromFileInMemoryEx( gpDev, g_TextureLoadBuffer, size, 
												D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, g_TextureMemoryPool,
												D3DX_FILTER_NONE, //D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
												D3DX_FILTER_NONE, //D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
												0, &imageinfo, NULL, &m_tex ) ) )
		{
			_XFatalError( "Can't create texture result:%d",result );
			return false;
		}
		
		D3DSURFACE_DESC surfdesc;		
		m_tex->GetLevelDesc( 0, &surfdesc );

		m_width  = imageinfo.Width;
		m_height = imageinfo.Height;
		m_Format = surfdesc.Format;
		m_initialized = true;
#else
		m_initialized = false;
#endif
	}
	else
	{
		if( g_TexturePreCacheing )
		{	
			if( g_TextureLoadBuffer )
			{
				if( g_TextureLoadBufferSize < size  )
				{
					SAFE_DELETE_ARRAY( g_TextureLoadBuffer );
					g_TextureLoadBuffer = new BYTE[ size ];
					if( !g_TextureLoadBuffer )
					{
						_XFatalError( "Can't allocate memory for virtual file [%dBytes]", size );
						g_TextureLoadBufferSize = 0L;
						return false;
					}
					
					g_TextureLoadBufferSize = size;
				}
			}
			else
			{
				SAFE_DELETE_ARRAY( g_TextureLoadBuffer );
				g_TextureLoadBuffer = new BYTE[ size ];
				if( !g_TextureLoadBuffer )
				{
					_XFatalError( "Can't allocate memory for virtual file [%dBytes]", size );
					g_TextureLoadBufferSize = 0L;
					return false;
				}
				
				g_TextureLoadBufferSize = size;
			}
			
			if( fread( g_TextureLoadBuffer, size, 1, pVFPtr ) < 1 )
			{
				_XFatalError( "Can't read virtual file data" );
				return false;
			}

			D3DXIMAGE_INFO pSrcInfo;
			if( FAILED( result = D3DXGetImageInfoFromFileInMemory( g_TextureLoadBuffer, size, &pSrcInfo ) ) )
			{
				_XFatalError( "Can't read texture informations. result:%d",result );			
				return false;
			}
			m_width  = pSrcInfo.Width;
			m_height = pSrcInfo.Height;
			m_Format = pSrcInfo.Format;
			
			m_texturequality = g_TextureQuality;
			
			if( !m_noresize )
			{
				if( g_TextureQuality > 1 )
				{
					m_width  = pSrcInfo.Width  / g_TextureQuality;
					m_height = pSrcInfo.Height / g_TextureQuality;
				}
			}
			
			if( g_MaxTextureWidth  < m_width  ) m_width  = g_MaxTextureWidth;
			if( g_MaxTextureHeight < m_height ) m_height = g_MaxTextureHeight;

			if( FAILED( result = D3DXCreateTextureFromFileInMemoryEx( gpDev, g_TextureLoadBuffer, size, 
													m_width, m_height, m_miplevel, 0, pSrcInfo.Format, g_TextureMemoryPool,
													g_TextureFilter, 
													g_TextureFilter, 
													0, NULL, NULL, &m_tex ) ) )
			{
				_XFatalError( "Can't create texture. result:%d",result );
				return false;
			}
			m_initialized = true;
		}
		else
		{
			m_initialized = false;
		}
	}
	
	return true;
}


bool _XTexture::Load(LPBYTE pVMPtr, int size, DWORD mips, bool noresize, bool createalphachannel )
{
	ReleaseDeviceObjects();
	if( !pVMPtr ) return false;
	
	if( mips <= 0 ) mips = 1;

	m_miplevel = mips;
	m_noresize = noresize;

	HRESULT result;

	if( createalphachannel )
	{
#ifdef _XDEF_PRELOADINTERFACEIMAGE

		if( FAILED( result = D3DXCreateTextureFromFileInMemoryEx( gpDev, pVMPtr, size, 
												D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, g_TextureMemoryPool,
												D3DX_FILTER_NONE, //D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
												D3DX_FILTER_NONE, //D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
												0, NULL, NULL, &m_tex ) ) )
		{
			_XFatalError( "Can't create texture. result:%d", result );
			return false;
		}		
		D3DSURFACE_DESC surfdesc;		
		m_tex->GetLevelDesc( 0, &surfdesc );
		m_width  = surfdesc.Width;
		m_height = surfdesc.Height;
		m_Format = D3DFMT_A8R8G8B8;
		
		m_initialized = true;
#else
		m_initialized = false;
#endif
	}
	else	
	{
		if( g_TexturePreCacheing )
		{			
			D3DXIMAGE_INFO pSrcInfo;
			if( FAILED( result = D3DXGetImageInfoFromFileInMemory( pVMPtr, size, &pSrcInfo ) ) )
			{
				_XFatalError( "Can't read texture informations. result:%d", result );
				return false;
			}
			m_width  = pSrcInfo.Width;
			m_height = pSrcInfo.Height;
			m_Format = pSrcInfo.Format;
			
			m_texturequality = g_TextureQuality;
			
			if( g_TextureQuality > 1 )
			{
				m_width  = pSrcInfo.Width  / g_TextureQuality;
				m_height = pSrcInfo.Height / g_TextureQuality;
			}
			
			if( g_MaxTextureWidth  < m_width  ) m_width  = g_MaxTextureWidth;
			if( g_MaxTextureHeight < m_height ) m_height = g_MaxTextureHeight;

			if( FAILED( result = D3DXCreateTextureFromFileInMemoryEx( gpDev, pVMPtr, size, 
													m_width, m_height, m_miplevel, 0, pSrcInfo.Format, g_TextureMemoryPool,
													g_TextureFilter, 
													g_TextureFilter, 
													0, NULL, NULL, &m_tex ) ) )
			{
				_XFatalError( "Can't create texture. result:%d",result );
				return false;
			}
			m_initialized = true;
		}
		else
		{
			m_initialized = false;
		}
	}
	return true;
}

bool _XTexture::InitDeviceObjects(D3DFORMAT p_fmt)
{	
	//ReleaseDeviceObjects();



	return true;
}


void _XTexture::ReleaseTexture( void )
{
	SAFE_RELEASE(m_tex);
	m_initialized = false;
}

void _XTexture::ReleaseDeviceObjects()
{
	SAFE_RELEASE(m_tex);
	//SAFE_DELETE(m_bmp);
	SAFE_DELETE_ARRAY(m_alphachannel);
	m_initialized = false;	
}

void _XTexture::ReportTextureInfo( void )
{
	D3DSURFACE_DESC surfacedesc;
			
	if( !m_tex )
	{
		_XLog( "= Texture informations [%s], Mip level : %d, Texture size (%d/%d), Not ready", m_strFileName.c_str(), m_miplevel, m_width, m_height );
	}
	else
	{
		int levelcnt = m_tex->GetLevelCount();
		m_tex->GetLevelDesc( 0, &surfacedesc );
		_XLog( "= Texture informations [%s], Mip level : %d, Texture size (%d/%d)", m_strFileName.c_str(), levelcnt, surfacedesc.Width, surfacedesc.Height );
	}
}
