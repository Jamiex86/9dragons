// XTextureManager.cpp: implementation of the _XTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

#include "LoadThread.h"

#include "XDataPool.h"

#define _RENDEROBJECT_POOL_

_XPackageArchive	 g_TextureAlphaInfoArchive;
BOOL				 g_SetTextureFailed = FALSE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XTextureManager::_XTextureManager()
{
	m_pRenderStyleBuffer=   NULL;

	m_nNumObjectStack	= 0;
	m_pObjectStack		=   NULL;	
	m_pLightmapObjectStack = NULL;
	m_Initialized		=	FALSE;
	memset( m_ArchiveName, 0, sizeof(TCHAR)*_MAX_PATH );
	strcpy( m_ArchiveName, _T("Noname") );

	m_bUseThread		= FALSE;
	SetFormat();
}

_XTextureManager::~_XTextureManager()
{
	DisposeTexture();	
	m_TextureArchive.FinalizePackage();
}


void _XTextureManager::DisposeTexture( void )
{
// #ifdef _XDWDEBUG
// 	_XDWINPRINT( "Cleanup texture list [PN:%s][count : %d]",m_ArchiveName,m_svXTextureBuffer.size() );
// #endif
	
	std::for_each(m_svXTextureBuffer.begin(), m_svXTextureBuffer.end(), DELETEOBJECT());
	m_svXTextureBuffer.clear();

	SAFE_DELETE_ARRAY( m_pObjectStack );
	SAFE_DELETE_ARRAY( m_pLightmapObjectStack );
	m_nNumObjectStack = NULL;

	m_RenderObjectPool.DeleteData();
	m_RenderLightmapObjectPool.DeleteData();

	SetFormat();

	SAFE_DELETE_ARRAY( m_pRenderStyleBuffer );
	m_RenderStyleManager.Dispose();

	m_Initialized = FALSE;
}

BOOL _XTextureManager::ReserveObjectStack( void )
{
	SAFE_DELETE_ARRAY( m_pObjectStack );
	SAFE_DELETE_ARRAY( m_pLightmapObjectStack );
	m_nNumObjectStack = 0;

	m_RenderObjectPool.DeleteData();
	m_RenderLightmapObjectPool.DeleteData();


	if( m_svXTextureBuffer.empty() ) return TRUE;

	m_RenderObjectPool.Reserve(50);
	m_RenderLightmapObjectPool.Reserve(50);

	m_nNumObjectStack = m_svXTextureBuffer.size();
	
	if( m_nNumObjectStack > 0 )
	{
		m_pObjectStack = new svdef_XRenderObject[m_nNumObjectStack];
		if( !m_pObjectStack )
		{
			_XFatalError( "Can't allocate draw stack [%s]", m_ArchiveName );
			return FALSE;
		}	
	}

	if( m_nNumObjectStack > 0 )
	{
		m_pLightmapObjectStack = new svdef_XRenderObject[m_nNumObjectStack];
		if( !m_pLightmapObjectStack )
		{
			_XFatalError( "Can't allocate lightmap object draw stack [%s]", m_ArchiveName );
			return FALSE;
		}	
	}
	
	return TRUE;
}

void _XTextureManager::PushRenderObject( int stackindex, _XRenderObject* prenderobject )
{
	if(!prenderobject) return;
	
	if( stackindex < 0 || stackindex >= m_svXTextureBuffer.size() )
	{
		_XLog( "WARNING : Invalid stack index or overflow stack index" );
#ifndef _RENDEROBJECT_POOL_
		delete prenderobject;
#endif
		return;
	}

	m_pObjectStack[stackindex].push_back(prenderobject);
}

void _XTextureManager::PushLightmapRenderObject( int stackindex, _XRenderObject* prenderobject )
{
	if(!prenderobject) return;
	
	if( stackindex < 0 || stackindex >= m_svXTextureBuffer.size() )
	{
		_XLog( "WARNING : Invalid stack index or overflow stack index" );
#ifndef _RENDEROBJECT_POOL_		
		delete prenderobject;
#endif
		return;
	}
	
	m_pLightmapObjectStack[stackindex].push_back(prenderobject);
}

void _XTextureManager::RenderAboveWaterObjectStack( void )
{
	if( !m_pObjectStack ) return;
	
	if( FAILED( gpDev->SetFVF( D3DFVF_XMESH_VERTEX ) ) ) return;
	
	svdef_XRenderObject::iterator it;
	_XRenderObject* pRenderObject;
	int i = 0 ; for(  i = 0; i < m_nNumObjectStack; i++ )
	{				
		if( m_pObjectStack[i].empty() ) continue;
		
		g_SetTextureFailed = FALSE;

		if( m_pRenderStyleBuffer && m_pRenderStyleBuffer[i] )
		{
			m_pRenderStyleBuffer[i]->SetRenderStyle( this );
		}
		else
		{
			m_RenderStyleManager.SetRenderStyle( NULL, 0 );
			SetTexture_NoRenderStyle( i, 0 );
		}		
		
		if( !g_SetTextureFailed )
		{
			gpDev->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
			
			for (it = m_pObjectStack[i].begin(); it != m_pObjectStack[i].end(); ++ it)
			{
				pRenderObject = (*it);
				if( pRenderObject->abovewater )
				{
					if(pRenderObject->pObject->nVertexCnt > 0)
					{
						gpDev->SetTransform( D3DTS_WORLD, pRenderObject->pObjectMatrix );					
						
						if( pRenderObject->usesecondbuffer )
						{		
							gpDev->SetStreamSource( 0, pRenderObject->pObject->pSecondVertexBuffer, 0, sizeof(_XMESH_VERTEX) );
							gpDev->SetIndices( pRenderObject->pObject->pIndex );
							gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pRenderObject->pObject->nVertexCnt, 
								0, pRenderObject->pObject->nIndexCnt );
						}
						else
						{							
							gpDev->SetStreamSource( 0, pRenderObject->pObject->pVertex, 0, sizeof(_XMESH_VERTEX) );
							gpDev->SetIndices( pRenderObject->pObject->pIndex );
							
							gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pRenderObject->pObject->nVertexCnt, 
								0, pRenderObject->pObject->nIndexCnt );
						}	
					}
				}
			}
		}
	}
}

extern _XTextureManager g_TerrainTextureArchive;

void _XTextureManager::RenderLightMapObjectStack( void )
{
	if( !m_pLightmapObjectStack ) return;
	
	if( FAILED( gpDev->SetFVF( D3DFVF_XMESH_VERTEX_UV2 ) ) ) return;

	g_TerrainTextureArchive.m_RenderStyleManager.SetRenderStyle(&g_TerrainTextureArchive, 19);
	
	_XRenderObject* pRenderObject;
	svdef_XRenderObject::iterator it;
	
	int i = 0 ; for(  i = 0; i < m_nNumObjectStack; i++ )
	{
		if( m_pLightmapObjectStack[i].empty() ) continue;
		
		g_SetTextureFailed = FALSE;
		SetTexture_NoRenderStyleUseCulling( i );
		
		if( !g_SetTextureFailed )
		{
			for (it = m_pLightmapObjectStack[i].begin(); it != m_pLightmapObjectStack[i].end(); ++ it)
			{
				pRenderObject = (*it);
				if(pRenderObject->pObject->nVertexCnt > 0)
				{
					g_SetTextureFailed = FALSE;
					SetTexture_NoRenderStyle( pRenderObject->pObject->nBakemapId, 1);				

					if( !g_SetTextureFailed )
					{
						gpDev->SetTransform( D3DTS_WORLD, pRenderObject->pObjectMatrix );					
						gpDev->SetStreamSource( 0, pRenderObject->pObject->pVertex, 0, sizeof(_XMESH_VERTEX_UV2) );
						gpDev->SetIndices( pRenderObject->pObject->pIndex );				
						gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pRenderObject->pObject->nVertexCnt, 0, pRenderObject->pObject->nIndexCnt );
					}
				}
			}
		}
	}
}

void _XTextureManager::RenderAvobeWaterLightMapObjectStack( void )
{
	if( !m_pLightmapObjectStack ) return;
	
	if( FAILED( gpDev->SetFVF( D3DFVF_XMESH_VERTEX_UV2 ) ) ) return;
	
	g_TerrainTextureArchive.m_RenderStyleManager.SetRenderStyle(&g_TerrainTextureArchive, 19);
	
	_XRenderObject* pRenderObject;
	svdef_XRenderObject::iterator it;
	
	int i = 0 ; for(  i = 0; i < m_nNumObjectStack; i++ )
	{
		if( m_pLightmapObjectStack[i].empty() ) continue;
		
		g_SetTextureFailed = FALSE;
		SetTexture_NoRenderStyleUseCulling( i );

		if(!g_SetTextureFailed)
		{
			for (it = m_pLightmapObjectStack[i].begin(); it != m_pLightmapObjectStack[i].end(); ++ it)
			{
				pRenderObject = (*it);

				if( pRenderObject->abovewater && pRenderObject->pObject->nVertexCnt > 0)
				{
					gpDev->SetTransform( D3DTS_WORLD, pRenderObject->pObjectMatrix );
					
					g_SetTextureFailed = FALSE;
					SetTexture_NoRenderStyle( pRenderObject->pObject->nBakemapId, 1);				

					if( !g_SetTextureFailed )
					{
						gpDev->SetStreamSource( 0, pRenderObject->pObject->pVertex, 0, sizeof(_XMESH_VERTEX_UV2) );
						gpDev->SetIndices( pRenderObject->pObject->pIndex );				
						gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pRenderObject->pObject->nVertexCnt, 0, pRenderObject->pObject->nIndexCnt );
					}
				}
			}
		}
	}
}


void _XTextureManager::RenderObjectStack( void )
{
	if( !m_pObjectStack ) return;
	
	if( FAILED( gpDev->SetFVF( D3DFVF_XMESH_VERTEX ) ) ) return;

	_XRenderObject* pRenderObject;
	svdef_XRenderObject::iterator it;

	int i = 0 ; for(  i = 0; i < m_nNumObjectStack; i++ )
	{
		if( m_pObjectStack[i].empty() ) continue;

		g_SetTextureFailed = FALSE;

		if( m_pRenderStyleBuffer && m_pRenderStyleBuffer[i] )
		{
			m_pRenderStyleBuffer[i]->SetRenderStyle( this );
		}
		else
		{
			m_RenderStyleManager.SetRenderStyle( NULL, 0 );
			SetTexture_NoRenderStyle( i, 0 );
		}
		
		if( !g_SetTextureFailed )
		{
			for (it = m_pObjectStack[i].begin(); it != m_pObjectStack[i].end(); ++ it)
			{
				pRenderObject = (*it);
				if(pRenderObject->pObject->nVertexCnt > 0)
				{
					gpDev->SetTransform( D3DTS_WORLD, pRenderObject->pObjectMatrix );

					if( pRenderObject->usesecondbuffer )
					{		
						gpDev->SetStreamSource( 0, pRenderObject->pObject->pSecondVertexBuffer, 0, sizeof(_XMESH_VERTEX) );
						gpDev->SetIndices( pRenderObject->pObject->pIndex );
						gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pRenderObject->pObject->nVertexCnt, 
																		 0, pRenderObject->pObject->nIndexCnt );
					}
					else
					{							
						gpDev->SetStreamSource( 0, pRenderObject->pObject->pVertex, 0, sizeof(_XMESH_VERTEX) );
						gpDev->SetIndices( pRenderObject->pObject->pIndex );
						
						gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pRenderObject->pObject->nVertexCnt, 
																		 0, pRenderObject->pObject->nIndexCnt );
					}	
				}
			}
		}
	}
}

void _XTextureManager::ResetObjectStack( void )
{
	if( m_pObjectStack )
	{	
		int i = 0 ; for(  i = 0; i < m_nNumObjectStack; i++ )
		{
			m_pObjectStack[i].clear();
		}
	}

	if( m_pLightmapObjectStack )
	{	
		int i = 0 ; for(  i = 0; i < m_nNumObjectStack; i++ )
		{
			m_pLightmapObjectStack[i].clear();
		}
	}

	m_RenderObjectPool.Reset();
	m_RenderLightmapObjectPool.Reset();
}

BOOL _XTextureManager::LoadTexturePackage( LPSTR archivename, int miplevel, BOOL noresize, BOOL createalphachannel )
{
	assert( archivename );
	if( !archivename || !archivename[0] ) return FALSE;
	DisposeTexture();
		
	if( !m_TextureArchive.OpenPackage( archivename ) ) return FALSE;

	int resourcecount = m_TextureArchive.GetResourceCount();

	if( resourcecount <= 0 )
	{
		_XFatalError( "Empty texture archive [%s]", archivename );
		return FALSE;
	}

	m_svXTextureBuffer.reserve(resourcecount);
	
#ifdef _XDWDEBUG
	LARGE_INTEGER frequency, count1, count2;
	if (!QueryPerformanceFrequency(&frequency))	_XDWINPRINT( "QueryPerformanceFrequency : No performancefreq" );	
#endif

	bool b_noresize = ( noresize ? true : false );
	bool b_createalphachannel = ( createalphachannel ? true : false );
	int i = 0 ; for(  i = 0; i < resourcecount; i++ )
	{
#ifdef _XDWDEBUG
		QueryPerformanceCounter(&count1);
#endif
		int   filesize = m_TextureArchive.GetPackedFileSize(i);
		FILE* pFP = m_TextureArchive.GetPackedFile(i);

		XString  filename( m_TextureArchive.GetResourceName( i ) );

		_XTexture* pTexture = new _XTexture;
		pTexture->SetFileName(filename);
		pTexture->m_strPath = filename;

		if( !pTexture->Load( pFP, filesize, miplevel, b_noresize, b_createalphachannel ) )
		{
			m_TextureArchive.ClosePackage();
			return FALSE;
		}

#ifdef _XDWDEBUG
		QueryPerformanceCounter(&count2);	 
		double timeinmicroseconds = (double)( (double)((count2.QuadPart - count1.QuadPart) * 1000 * 1000) / (double)frequency.QuadPart );
		_XDWINPRINT( "Ready texture object [%s]/ %f second", m_TextureArchive.GetResourceName( i ), timeinmicroseconds/1000000 );
#endif 	

		if( b_createalphachannel )
			AttachAlphaChannelData( pTexture );

		m_svXTextureBuffer.push_back(pTexture);
	}
	
	m_TextureArchive.ClosePackage();	
	
	memset( m_ArchiveName, 0, _MAX_PATH );
	strcpy( m_ArchiveName, archivename );

#ifdef _XDWDEBUG
	_XDWINPRINT( "Loaded texture package [%s]", archivename );
#endif

	if( !ReserveObjectStack() )
	{
		return FALSE;	
	}
	
	return TRUE;
}

BOOL _XTextureManager::MergeTexturePackage( LPSTR archivename )
{
	assert( archivename );
	if( !archivename || !archivename[0] ) return FALSE;
			
	return TRUE;
}

IDirect3DTexture9* _XTextureManager::GetTexture( int index )
{
	if( index < 0 || index >= m_svXTextureBuffer.size() ) return NULL;	

#ifdef _XDEF_PRELOADINTERFACEIMAGE	
	
	return m_svXTextureBuffer[index]->m_tex;

#else

	_XTexture* pTexture = m_svXTextureBuffer[index];

	if( !pTexture->m_initialized )
	{
		if (m_bUseThread == TRUE && g_LoadTextureThread.m_bEnable == TRUE)
		{
			g_LoadTextureThread.LoadTexture(this, pTexture);
		}
		else
		{
			if( !m_TextureArchive.ReOpenPackage() )
			{
				return NULL;
			}
			
			int resourceindex = m_TextureArchive.FindResource( pTexture->m_strPath );
			if( resourceindex == -1 ) return NULL;
			
			int   filesize = m_TextureArchive.GetPackedFileSize( resourceindex );
			
			if( m_TextureArchive.GetUseVirtualMemory() )
			{
				LPBYTE memarchiveptr = m_TextureArchive.GetPackedMemory( resourceindex );
				
				if( !pTexture->ReLoad( memarchiveptr, filesize ) )
				{
					m_TextureArchive.ClosePackage();
					return NULL;
				}
			}
			else
			{
				FILE* filearchiveptr = m_TextureArchive.GetPackedFile( resourceindex );
				
				if( !pTexture->ReLoad( filearchiveptr, filesize ) )
				{
					m_TextureArchive.ClosePackage();
					return NULL;
				}
			}
			
			m_TextureArchive.ClosePackage();
			_XDWINPRINT( "Texture Reloaded [%20s]", pTexture->m_strPath.operator TCHAR* () );
		}
	}	

	pTexture->m_LastUsedTime = g_LocalSystemTime;

	return pTexture->m_tex;
#endif

}

BYTE g_XByteFilter[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04,0x02, 0x01 };

BOOL _XTextureManager::TestAlphaChannel( int index, int x, int y )
{
	if( index < 0 || index >= m_svXTextureBuffer.size() ) return FALSE;	

	_XTexture* pTexture = m_svXTextureBuffer[index];

	if( !pTexture->m_alphachannel ) return TRUE;	

	int alphaarraywidth = (pTexture->m_width>>3);
	if( pTexture->m_width % 8 )
	{
		alphaarraywidth++;
	}

	int		bitoffset = x - ((x>>3)<<3);
	BYTE	alphachannel = pTexture->m_alphachannel[ (x>>3) + y * alphaarraywidth ];
	return	(alphachannel & g_XByteFilter[bitoffset]);

	return FALSE;
}

_XTexture*	_XTextureManager::GetTextureObject( int index )
{
	if( index < 0 || index >= m_svXTextureBuffer.size() ) return NULL;
	return m_svXTextureBuffer[index];
}

extern LPSTR getExtention( LPSTR String );

int	_XTextureManager::FindResource( LPSTR resourcename )
{
	assert( resourcename );
	if( !resourcename || !resourcename[0] ) return -1;

	int i = 0;
	for(svdef_XTexture::iterator it = m_svXTextureBuffer.begin(); it != m_svXTextureBuffer.end(); ++ i, ++ it)
	{
		if( (*it)->m_strPath.Compare( resourcename ) == 0 )
		{
#ifdef _XDWDEBUG
//			_XDWINPRINT( "Found texture in package [%s]", resourcename );
#endif
			return i;
		}
	}

#ifdef _XDWDEBUG
			_XDWINPRINT( "texture not found [%s]", resourcename );
#endif
	
	return -1;
}

#define _XDEF_TEXTURECACHECHECKTIME		120000

void _XTextureManager::CheckTextureUseHitRate( void )
{
	_XTexture* pTexture = NULL;
	for(svdef_XTexture::iterator it = m_svXTextureBuffer.begin(); it != m_svXTextureBuffer.end(); ++ it)
	{
		pTexture = (*it);
		if( !pTexture->m_ThreadLoad && pTexture->m_initialized )
		{
			if( (g_LocalSystemTime-pTexture->m_LastUsedTime) > _XDEF_TEXTURECACHECHECKTIME )
			{
				pTexture->ReleaseTexture();
			}
		}		
	}
}

void _XTextureManager::ResetTextureCacheTime( void )
{
	for(svdef_XTexture::iterator it = m_svXTextureBuffer.begin(); it != m_svXTextureBuffer.end(); ++ it)
	{
		(*it)->m_LastUsedTime = g_LocalSystemTime;
	}
}

void _XTextureManager::ReleaseTextureData( void )
{
	_XTexture* pTexture = NULL;
	for(svdef_XTexture::iterator it = m_svXTextureBuffer.begin(); it != m_svXTextureBuffer.end(); ++ it)
	{
		pTexture = (*it);
		if( !pTexture->m_ThreadLoad && pTexture->m_initialized )
			pTexture->ReleaseTexture();
	}	
}

BOOL _XTextureManager::IsAlphaBlending( int index )
{
	if( index < 0 || index >= m_svXTextureBuffer.size() )
	{
		return FALSE;
	}	

	if( m_pRenderStyleBuffer )
	{
		if( m_pRenderStyleBuffer[index] )
		{
			if( m_pRenderStyleBuffer[index]->m_rs.blendmode == _XRS_NOBLEND ) return FALSE;
			else return TRUE;
		}
		else 
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

void _XTextureManager::SetTexture( int index, DWORD stage )
{	
	if( index < 0 || index >= m_svXTextureBuffer.size() )
	{
		gpDev->SetTexture( stage, NULL );
		return;
	}	

	_XTexture* pTexture = m_svXTextureBuffer[index];

	if( !pTexture->m_initialized )
	{
		if (m_bUseThread == TRUE && g_LoadTextureThread.m_bEnable == TRUE)
		{
			g_LoadTextureThread.LoadTexture(this, pTexture);
			gpDev->SetTexture( stage, NULL );
			g_SetTextureFailed = TRUE;
			return;
		}
		else
		{
			if( !m_TextureArchive.ReOpenPackage() )
			{
				gpDev->SetTexture( stage, NULL );
				g_SetTextureFailed = TRUE;
				return;
			}

			int resourceindex = m_TextureArchive.FindResource( pTexture->m_strPath );
			if( resourceindex == -1 ) 
			{
				g_SetTextureFailed = TRUE;
				return;
			}
			
			int   filesize = m_TextureArchive.GetPackedFileSize( resourceindex );

			if( m_TextureArchive.GetUseVirtualMemory() )
			{
				LPBYTE memarchiveptr = m_TextureArchive.GetPackedMemory( resourceindex );
				
				if( !pTexture->ReLoad( memarchiveptr, filesize ) )
				{			
					gpDev->SetTexture( stage, NULL );
					m_TextureArchive.ClosePackage();
					g_SetTextureFailed = TRUE;
					return;
				}
			}
			else
			{
				FILE* filearchiveptr = m_TextureArchive.GetPackedFile( resourceindex );

				if( !pTexture->ReLoad( filearchiveptr, filesize ) )
				{			
					gpDev->SetTexture( stage, NULL );
					m_TextureArchive.ClosePackage();
					g_SetTextureFailed = TRUE;
					return;
				}
			}

			m_TextureArchive.ClosePackage();

			_XDWINPRINT( "Texture Reloaded [%20s]", pTexture->m_strPath.operator TCHAR* () );		
		}
	}	
		
#ifdef _XDWDEBUG
	if( !g_UseTexture )
	{
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		gpDev->SetTexture( stage, NULL );		
	}
	else
	{
#endif

	if( m_pRenderStyleBuffer )
	{
		if( m_pRenderStyleBuffer[index] )
		{
			m_pRenderStyleBuffer[index]->SetRenderStyle( this );
		}
		else
		{
			m_RenderStyleManager.SetRenderStyle( NULL, 0 ); // 렌더스타일이 없는경우 0번째 렌더스타일 적용.
			gpDev->SetTexture( stage, pTexture->m_tex );
		}
	}
	else 
	{
		gpDev->SetTexture( stage, pTexture->m_tex );
	}
	
#ifdef _XDWDEBUG
	}
#endif

	pTexture->m_LastUsedTime = g_LocalSystemTime;
}

void _XTextureManager::SetTexture_NoRenderStyle( int index, DWORD stage )
{	
	if( index < 0 || index >= m_svXTextureBuffer.size() )
	{
		gpDev->SetTexture( stage, NULL );
		return;
	}	
	
	if( !m_svXTextureBuffer[index]->m_initialized )
	{		
		if (m_bUseThread == TRUE && g_LoadTextureThread.m_bEnable == TRUE)
		{
			g_LoadTextureThread.LoadTexture(this, m_svXTextureBuffer[index]);
			gpDev->SetTexture( stage, NULL );
			g_SetTextureFailed = TRUE;
			return;
		}
		else
		{
			if( !m_TextureArchive.ReOpenPackage() )
			{
				gpDev->SetTexture( stage, NULL );
				g_SetTextureFailed = TRUE;
				return;
			}
			
			int resourceindex = m_TextureArchive.FindResource( m_svXTextureBuffer[index]->m_strPath );
			if( resourceindex == -1 ) 
			{
				g_SetTextureFailed = TRUE;
				return;
			}
			
			int   filesize = m_TextureArchive.GetPackedFileSize( resourceindex );

			if( m_TextureArchive.GetUseVirtualMemory() )
			{
				LPBYTE memarchiveptr = m_TextureArchive.GetPackedMemory( resourceindex );
				
				if( !m_svXTextureBuffer[index]->ReLoad( memarchiveptr, filesize ) )
				{			
					gpDev->SetTexture( stage, NULL );
					m_TextureArchive.ClosePackage();
					g_SetTextureFailed = TRUE;
					return;
				}
			}
			else
			{
				FILE* filearchiveptr = m_TextureArchive.GetPackedFile( resourceindex );
				
				if( !m_svXTextureBuffer[index]->ReLoad( filearchiveptr, filesize ) )
				{
					gpDev->SetTexture( stage, NULL );
					m_TextureArchive.ClosePackage();
					g_SetTextureFailed = TRUE;
					return;
				}
			}
			
			_XDWINPRINT( "Texture Reloaded [%20s]", m_svXTextureBuffer[index]->m_strPath.operator TCHAR* () );
			
			m_TextureArchive.ClosePackage();
		}
	}	
	
#ifdef _XDWDEBUG
	if( !g_UseTexture )
	{
		gpDev->SetTexture( stage, NULL );
	}
	else
#endif
	{
		gpDev->SetTexture( stage, m_svXTextureBuffer[index]->m_tex );
	}
		
	m_svXTextureBuffer[index]->m_LastUsedTime = g_LocalSystemTime;
}

void _XTextureManager::SetTexture_NoRenderStyleUseCulling( int index, DWORD stage )
{
	if( index < 0 || index >= m_svXTextureBuffer.size() )
	{
		gpDev->SetTexture( stage, NULL );		
		return;
	}	
	
	if( !m_svXTextureBuffer[index]->m_initialized )
	{		
		if (m_bUseThread == TRUE && g_LoadTextureThread.m_bEnable == TRUE)
		{
			g_LoadTextureThread.LoadTexture(this, m_svXTextureBuffer[index]);
			g_SetTextureFailed = TRUE;
			return;
		}
		else
		{
			if( !m_TextureArchive.ReOpenPackage() )
			{
				gpDev->SetTexture( stage, NULL );
				g_SetTextureFailed = TRUE;
				return;
			}
			
			int resourceindex = m_TextureArchive.FindResource( m_svXTextureBuffer[index]->m_strPath );
			if( resourceindex == -1 ) 
			{
				g_SetTextureFailed = TRUE;
				return;
			}
			
			int   filesize = m_TextureArchive.GetPackedFileSize( resourceindex );
			
			if( m_TextureArchive.GetUseVirtualMemory() )
			{
				LPBYTE memarchiveptr = m_TextureArchive.GetPackedMemory( resourceindex );
				
				if( !m_svXTextureBuffer[index]->ReLoad( memarchiveptr, filesize ) )
				{			
					gpDev->SetTexture( stage, NULL );
					m_TextureArchive.ClosePackage();
					g_SetTextureFailed = TRUE;
					return;
				}
			}
			else
			{
				FILE* filearchiveptr = m_TextureArchive.GetPackedFile( resourceindex );
				
				if( !m_svXTextureBuffer[index]->ReLoad( filearchiveptr, filesize ) )
				{
					gpDev->SetTexture( stage, NULL );
					m_TextureArchive.ClosePackage();
					g_SetTextureFailed = TRUE;
					return;
				}
			}
			
			_XDWINPRINT( "Texture Reloaded [%20s]", m_svXTextureBuffer[index]->m_strPath.operator TCHAR* () );
			
			m_TextureArchive.ClosePackage();
		}
	}	

#ifdef _XDWDEBUG
	if( !g_UseTexture )
	{
		gpDev->SetTexture( stage, NULL );
	}
	else
#endif
	{
		if( m_pRenderStyleBuffer )
		{
			if( m_pRenderStyleBuffer[index] )
			{
				gpDev->SetRenderState(D3DRS_CULLMODE,m_pRenderStyleBuffer[index]->m_rs.cullingmode);
			}
			else
			{
				if( m_pRenderStyleBuffer[0] )
					gpDev->SetRenderState(D3DRS_CULLMODE,m_pRenderStyleBuffer[0]->m_rs.cullingmode);
			}

			gpDev->SetTexture( stage, m_svXTextureBuffer[index]->m_tex );
		}
		else 
		{
			gpDev->SetTexture( stage, m_svXTextureBuffer[index]->m_tex );
		}
	}
	
	m_svXTextureBuffer[index]->m_LastUsedTime = g_LocalSystemTime;
}

BOOL _XTextureManager::InitDeviceObject( void )
{	
	return TRUE;

	if( m_Initialized ) 
	{
		_XDWINPRINT("Skipped Initializing texture object=================================" );
		return TRUE;
	}

	_XDWINPRINT("Initialize texture object===============================================" );

	if(m_svXTextureBuffer.empty())
	{
		_XFatalError("_XTextureManager::InitDeviceObjects() : Empty texture buffer!");
		return FALSE;
	}
	
#ifdef _XDWDEBUG
	LARGE_INTEGER frequency, count1, count2;
	if (!QueryPerformanceFrequency(&frequency))	_XDWINPRINT( "QueryPerformanceFrequency : No performancefreq" );	
#endif

	int i = 0;
	for(svdef_XTexture::iterator it = m_svXTextureBuffer.begin(); it != m_svXTextureBuffer.end(); ++ i, ++ it)
	{

#ifdef _XDWDEBUG
		QueryPerformanceCounter(&count1);
#endif

		if(!(*it)->InitDeviceObjects(m_TextureFormat))
		{
			_XFatalError("_XTextureManager::InitDeviceObjects() : Could not create texture.[%s][%d]",
				        (*it)->m_strPath.operator const TCHAR* (), i);
			return FALSE;
		}

#ifdef _XDWDEBUG
		QueryPerformanceCounter(&count2);	 
		double timeinmicroseconds = (double)( (double)((count2.QuadPart - count1.QuadPart) * 1000 * 1000) / (double)frequency.QuadPart );
		_XDWINPRINT( "Texture initialized [%20s]/ %f second [%p]", (*it)->m_strPath.operator TCHAR* (),timeinmicroseconds/1000000, (*it)->m_tex );
#else
		_XDWINPRINT( "Texture initialized [%20s]", (*it)->m_strPath.operator TCHAR* () );
#endif 	

	}

	m_Initialized = TRUE;

	_XDWINPRINT("End of Initialize texture object========================================" );
	
	return TRUE;
}

void _XTextureManager::ReleaseDeviceObject( void )
{
	if( m_Initialized ) 
	{
		_XDWINPRINT("Skipped Release texture object=======================================" );
		return;
	}

	if( m_svXTextureBuffer.empty() ) return;

	for(svdef_XTexture::iterator it = m_svXTextureBuffer.begin(); it != m_svXTextureBuffer.end(); ++ it)
	{
		(*it)->ReleaseDeviceObjects();
	}
}


BOOL _XTextureManager::SetTexturePackage( LPSTR archivename )
{
	assert( archivename );
	
	memset( m_ArchiveName, 0, _MAX_PATH );
	std::for_each(m_svTextureName.begin(), m_svTextureName.end(), DELETEOBJECT());
	m_svTextureName.clear();

	if( !archivename || !archivename[0] ) return FALSE;
	strcpy( m_ArchiveName, archivename );

	m_TextureArchive.OpenPackage( archivename );
	
	return TRUE;
}

BOOL _XTextureManager::BuildTextureList( BOOL createalphachannel ) 
{
	_XDWINPRINT("Build texture list======================================================" );

	if( m_svTextureName.empty() ) return TRUE;

	DisposeTexture();

	m_svXTextureBuffer.reserve(m_svTextureName.size());

	_XTexture* pTexture = NULL;
	_XStringItem* curitem = NULL;	
	
	if( !m_TextureArchive.ReOpenPackage() )
	{
		return FALSE;
	}

#ifdef _XDWDEBUG
	LARGE_INTEGER frequency, count1, count2;
	if (!QueryPerformanceFrequency(&frequency))	_XDWINPRINT( "QueryPerformanceFrequency : No performancefreq" );	
#endif

	bool b_createalphachannel = ( createalphachannel ? true : false );

	for (svdef_XStringItem::iterator it = m_svTextureName.begin(); it != m_svTextureName.end(); ++ it)
	{
		curitem = (*it);

#ifdef _XDWDEBUG
		QueryPerformanceCounter(&count1);
#endif
		int resourceindex = m_TextureArchive.FindResource( curitem->Get_String() );
		if( resourceindex != -1 )
		{
			int   miplevel = LOWORD(curitem->Get_Attribute());
			bool  noresize   = ( ( HIWORD(curitem->Get_Attribute()) ) ? true : false );

			pTexture = CreateLoadTexture(resourceindex, miplevel, noresize, curitem->Get_String(), b_createalphachannel);			
			if (pTexture == NULL)
			{
				m_TextureArchive.ClosePackage();
				return FALSE;
			}

			pTexture->SetFileName(curitem->Get_String());
			XString  filename( curitem->Get_String() );
			pTexture->m_strPath = filename;

			if( b_createalphachannel )
				AttachAlphaChannelData( pTexture );

			m_svXTextureBuffer.push_back(pTexture);
		}
		else
		{
			_XFatalError( "Could not find texture object.[%s : %s]", m_ArchiveName, curitem->Get_String() );
			m_TextureArchive.ClosePackage();
			return FALSE;
		}						
		
#ifdef _XDWDEBUG
		QueryPerformanceCounter(&count2);	 
		double timeinmicroseconds = (double)( (double)((count2.QuadPart - count1.QuadPart) * 1000 * 1000) / (double)frequency.QuadPart );
		_XDWINPRINT( "Texture ready [%20s]/ %f second", pTexture->m_strPath.operator TCHAR* (),timeinmicroseconds/1000000 );
#else
		_XDWINPRINT( "Texture ready [%20s]", pTexture->m_strPath.operator TCHAR* () );
#endif 	

	}

	std::for_each(m_svTextureName.begin(), m_svTextureName.end(), DELETEOBJECT());
	m_svTextureName.clear();

	m_TextureArchive.ClosePackage();
	
	_XDWINPRINT("End of build texture list===============================================" );

	if( !ReserveObjectStack() )
	{
		return FALSE;	
	}

	return TRUE;
}

_XTexture* _XTextureManager::CreateLoadTexture(int resourceindex, int miplevel, bool noresize, TCHAR* pName, bool b_createalphachannel) 
{
	_XTexture* pTexture = new _XTexture;

	if( resourceindex == -1 )
	{
		resourceindex = m_TextureArchive.FindResource( pName );
	}

	int filesize = m_TextureArchive.GetPackedFileSize( resourceindex );

	if( m_TextureArchive.GetUseVirtualMemory() )
	{
		LPBYTE memarchiveptr = m_TextureArchive.GetPackedMemory( resourceindex );

		if( !memarchiveptr )
		{
			_XFatalError( "_XTextureManager::Could not find file pointer.[%s : %s]", m_ArchiveName, pName );			
			delete pTexture;
			return NULL;
		}
		
		if( !pTexture->Load(memarchiveptr, filesize, miplevel, noresize, b_createalphachannel ) )
		{
			_XFatalError( "_XTextureManager::Could not read texture.[%s : %s]", m_ArchiveName, pName );					
			delete pTexture;
			return NULL;					
		}
	}
	else
	{
		FILE* filearchiveptr = m_TextureArchive.GetPackedFile( resourceindex );

		if( !filearchiveptr )
		{
			_XFatalError( "_XTextureManager::Could not find file pointer.[%s : %s]", m_ArchiveName, pName );			
			delete pTexture;
			return NULL;
		}
									
		if( !pTexture->Load(filearchiveptr, filesize, miplevel, noresize, b_createalphachannel ) )
		{
			_XFatalError( "Could not read texture.[%s : %s]", m_ArchiveName, pName );					
			delete pTexture;
			return NULL;					
		}
	}
	return pTexture;
}


int	_XTextureManager::AddTextureList( LPSTR resourcename, int miplevel, BOOL noresize, bool b_createalphachannel )
{
	assert( resourcename );
	if( !resourcename || !resourcename[0] ) return -1;

	if( !m_TextureArchive.ReOpenPackage() )
	{
		return FALSE;
	}


	int index = FindTextureName(resourcename);
	if (index >= 0) return index;

	_XTexture* pTexture = CreateLoadTexture(index, miplevel, noresize ? true : false, resourcename, b_createalphachannel);	
	if (pTexture == NULL)
	{
		m_TextureArchive.ClosePackage();
		return -1;
	}

	pTexture->SetFileName(resourcename);	
	XString  filename( resourcename );
	pTexture->m_strPath = filename;

	if( b_createalphachannel )
		AttachAlphaChannelData( pTexture );		
	m_svXTextureBuffer.push_back(pTexture);

	m_TextureArchive.ClosePackage();

	if( !ReserveObjectStack() )
	{
		return -1;	
	}

	return m_svXTextureBuffer.size()-1;
}

int	_XTextureManager::GetResourceIndex( LPSTR resourcename, int miplevel, BOOL noresize )
{
	assert( resourcename );
	if( !resourcename || !resourcename[0] ) return -1;

	int index = FindTextureName(resourcename);
	if (index >= 0) return index;

	_XStringItem* pStringItem = new _XStringItem( resourcename, (noresize<<16) | miplevel );
	m_svTextureName.push_back(pStringItem);

	return m_svTextureName.size()-1;
}

int _XTextureManager::FindTextureName(LPSTR resourcename)
{
	/*
	int count = 0;
	_XTexture* pTexture;
	svdef_XTexture::iterator it;
	for (it = m_svXTextureBuffer.begin(); it != m_svXTextureBuffer.end(); ++ count, ++ it)
	{
		pTexture = (*it);
		if( strcmpi( pTexture->GetFileName(), resourcename ) == 0 )
		{
			return count;
		}
	}
	*/

	_XStringItem* curitem = NULL;
	
	int count = 0;
	svdef_XStringItem::iterator it;
	for ( it = m_svTextureName.begin(); it != m_svTextureName.end(); ++count, ++ it)
	{
		curitem = (*it);
		if( strcmpi( curitem->Get_String(), resourcename ) == 0 )
		{
			return count;
		}
	}

	return -1;
}

BOOL _XTextureManager::ReadyRenderStyle(  LPTSTR filename )
{		
	_XRenderStyleManager renderstylemanager;
	_XSetDefaultPath( _XDEF_DEFAULTPATH_TEXTURE );
	renderstylemanager.Load( filename );
	
	int i = 0 ; for(  i = 0; i < renderstylemanager.m_RenderStyleCount; i++ )
	{		
		//GetResourceIndex( m_RenderStyleManager.m_RenderStyleList[i].m_rs.texturename[0] );
		if( strlen( renderstylemanager.m_RenderStyleList[i].m_rs.texturename[1] ) > 0 )
			GetResourceIndex( renderstylemanager.m_RenderStyleList[i].m_rs.texturename[1] );
		if( strlen( renderstylemanager.m_RenderStyleList[i].m_rs.texturename[2] ) > 0 )
			GetResourceIndex( renderstylemanager.m_RenderStyleList[i].m_rs.texturename[2] );
		if( strlen( renderstylemanager.m_RenderStyleList[i].m_rs.texturename[3] ) > 0 )
			GetResourceIndex( renderstylemanager.m_RenderStyleList[i].m_rs.texturename[3] );
	}

	return TRUE;
}

BOOL _XTextureManager::AttachRenderStyle( LPTSTR filename )
{
	SAFE_DELETE_ARRAY( m_pRenderStyleBuffer );

	m_pRenderStyleBuffer = new _XRenderStyleItem*[ m_svXTextureBuffer.size() ];	
	if( !m_pRenderStyleBuffer )
	{
		_XFatalError( "Allocation render style buffer [%s]", filename );		
		return FALSE;
	}	
	memset( m_pRenderStyleBuffer, 0, sizeof(_XRenderStyleItem*) * m_svXTextureBuffer.size() );

	_XSetDefaultPath( _XDEF_DEFAULTPATH_TEXTURE );
	if( !m_RenderStyleManager.Load( filename ) )
	{
		//_XFatalError( "Can't access [%s] file.", filename );		
		return FALSE;
	}

	int i = 0 ; for(  i = 0; i < m_RenderStyleManager.m_RenderStyleCount; i++ )
	{		
		m_RenderStyleManager.m_RenderStyleList[i].m_textureindex[0] = 
			FindResource( m_RenderStyleManager.m_RenderStyleList[i].m_rs.texturename[0] );		
		m_RenderStyleManager.m_RenderStyleList[i].m_textureindex[1] = 
			FindResource( m_RenderStyleManager.m_RenderStyleList[i].m_rs.texturename[1] );
		m_RenderStyleManager.m_RenderStyleList[i].m_textureindex[2] = 
			FindResource( m_RenderStyleManager.m_RenderStyleList[i].m_rs.texturename[2] );
		m_RenderStyleManager.m_RenderStyleList[i].m_textureindex[3] = 
			FindResource( m_RenderStyleManager.m_RenderStyleList[i].m_rs.texturename[3] );
		
		if( m_RenderStyleManager.m_RenderStyleList[i].m_textureindex[0] != -1 )
		{
			m_pRenderStyleBuffer[m_RenderStyleManager.m_RenderStyleList[i].m_textureindex[0]] = 
				&m_RenderStyleManager.m_RenderStyleList[i];
		}
	}	

	return TRUE;
}

void _XTextureManager::ReportTextureInfo( void )
{
	_XLog( "= [%s] Texture archive informations========", m_ArchiveName );
	_XLog( "= Texture count : %d", m_svXTextureBuffer.size() );

	for(svdef_XTexture::iterator it = m_svXTextureBuffer.begin(); it != m_svXTextureBuffer.end(); ++ it)
	{
		(*it)->ReportTextureInfo();
	}
}

void _XTextureManager::AttachAlphaChannelData( _XTexture* pTextureObject )
{
	TCHAR acfilename[64];
	memset( acfilename, 0, sizeof(TCHAR) * 64 );
	strcpy( acfilename, pTextureObject->m_strPath );

	int i = strlen(acfilename)-1;for( ; i >= 0; i-- )
	{
		if( acfilename[i] == _T('.') )
		{
			break;
		}
	}

	if( i <= 0 ) return;
	acfilename[i+1] = _T('x');
	acfilename[i+2] = _T('a');
	acfilename[i+3] = _T('c');		
	
	SAFE_DELETE_ARRAY( pTextureObject->m_alphachannel );
	
	int textureindex = g_TextureAlphaInfoArchive.FindResource( acfilename );

	if( textureindex < 0 ) return;

	if( !g_TextureAlphaInfoArchive.ReOpenPackage() )
		return;

	DWORD packedsize = g_TextureAlphaInfoArchive.GetPackedFileSize( textureindex );
	packedsize -= (sizeof(int)<<1);

	FILE* pFile = g_TextureAlphaInfoArchive.GetPackedFile( textureindex );
	if( !pFile ) 
	{
		g_TextureAlphaInfoArchive.ClosePackage();
		return;
	}

	int width  = 0;
	int height = 0;

	if( fread( &width, sizeof(int), 1, pFile ) < 1 )
	{
		g_TextureAlphaInfoArchive.ClosePackage();
		_XFatalError( "Can't read alphachannel width [%s]", acfilename );
		return;
	}
	if( fread( &height, sizeof(int), 1, pFile ) < 1 )
	{
		g_TextureAlphaInfoArchive.ClosePackage();
		_XFatalError( "Can't read alphachannel height [%s]", acfilename );
		return;
	}

#ifdef _XDEF_PRELOADINTERFACEIMAGE
	if( width != pTextureObject->m_width || height != pTextureObject->m_height )
	{
		_XFatalError( "Invalid alphachannel size %s:[a<%d:%d> t<%d:%d>]", acfilename, width, height, pTextureObject->m_width, pTextureObject->m_height );
		return;
	}
#endif

	pTextureObject->m_alphachannel = new BYTE[packedsize];

	if( !pTextureObject->m_alphachannel )
	{
		g_TextureAlphaInfoArchive.ClosePackage();
		_XFatalError( "Can't allocate alphachannel buffer [%s]", acfilename );		
		return;
	}

	if( fread( pTextureObject->m_alphachannel, packedsize, 1, pFile ) < 1 )
	{
		g_TextureAlphaInfoArchive.ClosePackage();
		_XFatalError( "Can't read alphachannel buffer [%s]", acfilename );		
		return;
	}

	g_TextureAlphaInfoArchive.ClosePackage();
}

BOOL _XTextureManager::PreCachingTexture( int index )
{
	if( index < 0 || index >= m_svXTextureBuffer.size() )
	{
		return FALSE;
	}	
	
	_XTexture* pTexture = m_svXTextureBuffer[index];
	
	if( !pTexture->m_initialized )
	{
		if (m_bUseThread == TRUE && g_LoadTextureThread.m_bEnable == TRUE)
		{
			g_LoadTextureThread.LoadTexture(this, pTexture);
		}
		else
		{
			if( !m_TextureArchive.ReOpenPackage() )
			{
				return FALSE;
			}
			
			int resourceindex = m_TextureArchive.FindResource( pTexture->m_strPath );
			if( resourceindex == -1 ) return FALSE;
			
			int   filesize = m_TextureArchive.GetPackedFileSize( resourceindex );
			
			if( m_TextureArchive.GetUseVirtualMemory() )
			{
				LPBYTE memarchiveptr = m_TextureArchive.GetPackedMemory( resourceindex );
				
				if( !pTexture->ReLoad( memarchiveptr, filesize ) )
				{
					m_TextureArchive.ClosePackage();
					return FALSE;
				}
			}
			else
			{
				FILE* filearchiveptr = m_TextureArchive.GetPackedFile( resourceindex );
				
				if( !pTexture->ReLoad( filearchiveptr, filesize ) )
				{
					m_TextureArchive.ClosePackage();
					return FALSE;
				}
			}
			
			m_TextureArchive.ClosePackage();
			
			_XDWINPRINT( "Texture pre cached [%20s]", pTexture->m_strPath.operator TCHAR* () );
		}
	}	

	return TRUE;
}