// XStatic.cpp: implementation of the _XStatic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"
#include "XStatic.h"

extern _XTextureManager g_MainInterfaceTextureArchive;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XStatic::_XStatic()
{
	m_MaxRate = m_fDepth = m_fLeft	= m_fTop = m_fRight = m_fBottom = 0.f;
	m_TextureIndex1	= m_TextureIndex2 = -1;
	m_Percentage = 0;

#ifdef _XDEF_SYSTEMVERTEX		
	m_FaceColor	=	D3DCOLOR_ARGB( 255, 255,255,255 );
	m_dwNumVertices = 4;
	m_dwNumIndices	= 6;
#endif

	m_pTextureArchive = NULL;
	m_TooltipCallbackFunc = NULL;
}

_XStatic::~_XStatic()
{
	m_pTextureArchive = NULL;	
}

HRESULT _XStatic::Create( int left, int top, int right, int bottom, FLOAT depth, D3DCOLOR facecolor )
{	
	ReleaseObject();

#ifndef _XDEF_SYSTEMVERTEX		
	HRESULT hr = this->CreateVertexBuffer( 4, D3DUSAGE_WRITEONLY, D3DFVF_INTERFACETVERTEX, D3DPOOL_MANAGED );

	if( FAILED(hr) ) return E_FAIL;

	INTERFACETVERTEX* pVertices  = NULL;

	hr = this->LockVertexBuffer( 0, pVertices );
	if( FAILED(hr) ) return E_FAIL;
#endif

	m_fLeft		= (FLOAT)left;
	m_fTop		= (FLOAT)top;
	m_fRight	= (FLOAT)right;
	m_fBottom	= (FLOAT)bottom;
	m_FaceColor	= facecolor;
	m_fDepth	= depth;

	m_dwNumVertices	= 4;
	m_dwNumIndices	= 6;
		
	INTERFACETVERTEX	QVertices[4] =
    {       	
		{ m_fLeft-0.5f,	m_fTop-0.5f,		0.5f, m_fDepth, m_FaceColor, 0.0f,		0.0f },
        { m_fRight-0.5f,	m_fTop-0.5f,		0.5f, m_fDepth, m_FaceColor, 1.0f,		0.0f },
		{ m_fRight-0.5f, m_fBottom-0.5f,	0.5f, m_fDepth, m_FaceColor, 1.0f,		1.0f },
		{ m_fLeft-0.5f,	m_fBottom-0.5f,	0.5f, m_fDepth, m_FaceColor, 0.0f,		1.0f }

		/*{ m_fLeft,	m_fTop,		0.5f, m_fDepth, m_FaceColor, 0.01f,		0.01f },
        { m_fRight,	m_fTop,		0.5f, m_fDepth, m_FaceColor, 0.99f,		0.01f },
		{ m_fRight, m_fBottom,	0.5f, m_fDepth, m_FaceColor, 0.99f,		0.99f },
		{ m_fLeft,	m_fBottom,	0.5f, m_fDepth, m_FaceColor, 0.01f,		0.99f }	*/
    };
	
	memcpy( pVertices, QVertices, sizeof(INTERFACETVERTEX)*4 );

#ifndef _XDEF_SYSTEMVERTEX		
	this->UnlockVertexBuffer();
#endif

	//Floor 인덱스버퍼를 생성한다.	
	
	WORD	Indices[6] = {0,1,2,2,3,0};
	
#ifndef _XDEF_SYSTEMVERTEX		
	LPWORD  pIndices = NULL;
	hr = this->CreateIndexBuffer( 6, g_3DObjectMemoryUsage, D3DFMT_INDEX16, g_3DObjectMemoryPool );	
	if( FAILED(hr) ) return E_FAIL;	

	hr = this->LockIndexBuffer( 6, pIndices );	
	if( FAILED(hr) ) return E_FAIL;	
#endif

	memcpy( pIndices, Indices, sizeof(WORD)*6 );

#ifndef _XDEF_SYSTEMVERTEX		    
	this->UnlockIndexBuffer();
#endif

	return S_OK;
}

HRESULT _XStatic::Create( D3DXVECTOR3 lefttop, D3DXVECTOR3 righttop, 
						  D3DXVECTOR3 leftbottom, D3DXVECTOR3 rightbottom, FLOAT depth, D3DCOLOR facecolor)
{		
#ifndef _XDEF_SYSTEMVERTEX
	ReleaseObject();
	
	HRESULT hr = this->CreateVertexBuffer( 4, D3DUSAGE_WRITEONLY, D3DFVF_INTERFACETVERTEX, D3DPOOL_MANAGED );

	if( FAILED(hr) ) return E_FAIL;

	INTERFACETVERTEX* pVertices  = NULL;

	hr = this->LockVertexBuffer( 0, pVertices );
	if( FAILED(hr) ) return E_FAIL;
#endif

	m_FaceColor	= facecolor;
	m_fDepth	= depth;

	m_dwNumVertices	= 4;
	m_dwNumIndices	= 6;

	m_MaxRate = abs(rightbottom.x - righttop.x);

	FLOAT maxleft  = 9999.0f;
	FLOAT maxright = -9999.0f;

	FLOAT maxtop    = 9999.0f;
	FLOAT maxbottom = -9999.0f;

	if( maxleft > lefttop.x		) maxleft = lefttop.x;
	if( maxleft > righttop.x	) maxleft = righttop.x;
	if( maxleft > leftbottom.x  ) maxleft = leftbottom.x;
	if( maxleft > rightbottom.x ) maxleft = rightbottom.x;

	if( maxright < lefttop.x	) maxright = lefttop.x;
	if( maxright < righttop.x	) maxright = righttop.x;
	if( maxright < leftbottom.x ) maxright = leftbottom.x;
	if( maxright < rightbottom.x) maxright = rightbottom.x;

	if( maxtop > lefttop.y		) maxtop = lefttop.y;
	if( maxtop > righttop.y		) maxtop = righttop.y;
	if( maxtop > leftbottom.y	) maxtop = leftbottom.y;
	if( maxtop > rightbottom.y	) maxtop = rightbottom.y;

	if( maxbottom < lefttop.y	 ) maxbottom = lefttop.y;
	if( maxbottom < righttop.y	 ) maxbottom = righttop.y;
	if( maxbottom < leftbottom.y ) maxbottom = leftbottom.y;
	if( maxbottom < rightbottom.y) maxbottom = rightbottom.y;

	m_fLeft		= maxleft;
	m_fTop		= maxtop;
	m_fRight	= maxright;
	m_fBottom	= maxbottom;
	
	INTERFACETVERTEX	QVertices[4] =
    {   /*    	
		{ lefttop.x,    lefttop.y,    0.5f,		m_fDepth, m_FaceColor, 0.01f, 0.01f },
        { righttop.x,   righttop.y,   0.5f,		m_fDepth, m_FaceColor, 0.99f, 0.01f },
		{ rightbottom.x,rightbottom.y,0.5f,	m_fDepth, m_FaceColor, 0.99f, 0.01f },
		{ leftbottom.x, leftbottom.y, 0.5f,		m_fDepth, m_FaceColor, 0.01f, 0.99f }
		*/
		{ lefttop.x-0.5f,    lefttop.y-0.5f,    0.5f,		m_fDepth, m_FaceColor, 0.0f, 0.0f },
        { righttop.x-0.5f,   righttop.y-0.5f,   0.5f,		m_fDepth, m_FaceColor, 1.0f, 0.0f },
		{ rightbottom.x-0.5f,rightbottom.y-0.5f,0.5f,		m_fDepth, m_FaceColor, 1.0f, 1.0f },
		{ leftbottom.x-0.5f, leftbottom.y-0.5f, 0.5f,		m_fDepth, m_FaceColor, 0.0f, 1.0f }

    };
	
	memcpy( pVertices, QVertices, sizeof(INTERFACETVERTEX)*4 );

#ifndef _XDEF_SYSTEMVERTEX		    
	this->UnlockVertexBuffer();
#endif

	//Floor 인덱스버퍼를 생성한다.
	WORD	Indices[6] = {0,1,2,2,3,0};

#ifndef _XDEF_SYSTEMVERTEX		    
	LPWORD  pIndices = NULL;
	hr = this->CreateIndexBuffer( 6, g_3DObjectMemoryUsage, D3DFMT_INDEX16, g_3DObjectMemoryPool );
	
	if( FAILED(hr) ) return E_FAIL;	

	hr = this->LockIndexBuffer( 6, pIndices );	
	if( FAILED(hr) ) return E_FAIL;	
#endif

	memcpy( pIndices, Indices, sizeof(WORD)*6 );

#ifndef _XDEF_SYSTEMVERTEX		        
	this->UnlockIndexBuffer();
#endif

	return S_OK;
}

void _XStatic::ReleaseObject( void )
{	
#ifndef _XDEF_SYSTEMVERTEX		    
	_X3DObject::ReleaseObject();
#endif
}


void _XStatic::SetTexture( _XTextureManager* ptexturearchive, int textureindex1, int textureindex2 )
{
	m_TextureIndex1 = textureindex1;
	m_TextureIndex2 = textureindex2;

	m_pTextureArchive = ptexturearchive;
}

void _XStatic::SetFaceColor( D3DCOLOR facecolor )
{
#ifndef _XDEF_SYSTEMVERTEX		    	
	INTERFACETVERTEX* pVertices  = NULL;
	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif
		pVertices[0].diffuse = facecolor;
		pVertices[1].diffuse = facecolor;
		pVertices[2].diffuse = facecolor;
		pVertices[3].diffuse = facecolor;
		
		//_XDWINPRINT( "Set face color %d", facecolor );

#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}	
#endif
}

HRESULT _XStatic::LockVertexBuffer( DWORD Length, INTERFACETVERTEX*& pVertices )
{		

#ifndef _XDEF_SYSTEMVERTEX		    
	if( m_VertexLockState ) UnlockVertexBuffer();

	HRESULT hr = m_pVB->Lock( 0, 0, (BYTE**)&pVertices, NULL );
		
	if( SUCCEEDED(hr) ) m_VertexLockState =	TRUE;
    
	return hr;
#else
	return S_OK;
#endif

}

HRESULT _XStatic::CreateVertexBuffer( UINT VertexCount, DWORD Usage, DWORD FVF, D3DPOOL Pool )
{

#ifndef _XDEF_SYSTEMVERTEX		    
	assert( gpDev );

	ReleaseVertexBuffer();

	// Create vertex buffer
    HRESULT hr = gpDev->CreateVertexBuffer( VertexCount*sizeof(INTERFACETVERTEX), Usage, D3DFVF_INTERFACETVERTEX, Pool, &m_pVB, NULL );
    if( FAILED(hr) )
        return E_FAIL;	

	m_dwNumVertices = VertexCount;
#endif

	return S_OK;
}

void _XStatic::SetXRate( FLOAT x, BOOL contollupvertex )
{
	if( fabs(x) > m_MaxRate ) x = (x < 0) ? -m_MaxRate : m_MaxRate;
	
#ifndef _XDEF_SYSTEMVERTEX		    	
	INTERFACETVERTEX* pVertices  = NULL;
	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif
		if( contollupvertex )
		{			
			pVertices[0].x	= pVertices[1].x + x;
			pVertices[3].x	= pVertices[2].x + x;
		}
		else
		{			
			pVertices[1].x	= pVertices[0].x + x;
			pVertices[2].x	= pVertices[3].x + x;
		}
#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}	
#endif
	
}

void _XStatic::SetYRate( FLOAT y, BOOL contollupvertex )
{
	if( fabs(y) > m_MaxRate ) y = (y < 0) ? -m_MaxRate : m_MaxRate;

#ifndef _XDEF_SYSTEMVERTEX		    	
	INTERFACETVERTEX* pVertices  = NULL;
	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif
		if( contollupvertex )
		{			
			pVertices[0].y	= pVertices[3].y + y;			
			pVertices[1].y	= pVertices[2].y + y;
		}
		else
		{			
			pVertices[2].y	= pVertices[1].y + y;			
			pVertices[3].y	= pVertices[0].y + y;
		}
#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}	
#endif

}

void _XStatic::SetXPercentage( DWORD percent, FLOAT x,BOOL contollupvertex )
{	
	if( percent > 100 ) percent = 100;
	m_Percentage = percent;
	
	FLOAT size = m_MaxRate * ((FLOAT)percent / 100.0f);	
	SetXRate( ((x < 0) ? -size : size), contollupvertex );	
}

void _XStatic::SetYPercentage( DWORD percent, FLOAT y,BOOL contollupvertex )
{	
	if( percent > 100 ) percent = 100;
	m_Percentage = percent;
	
	FLOAT size = m_MaxRate * ((FLOAT)percent / 100.0f);	
	SetYRate( ((y < 0) ? -size : size), contollupvertex );	
}

void _XStatic::SetPercentage( DWORD percent, FLOAT x, FLOAT y,BOOL contollupvertex )
{
	if( percent > 100 ) percent = 100;
	m_Percentage = percent;

	FLOAT size = m_MaxRate * ((FLOAT)percent / 100.0f);	
	SetRate( ((x < 0) ? -size : size), ((y < 0) ? -size : size), contollupvertex );
}

void _XStatic::SetRate( FLOAT x, FLOAT y, BOOL contollupvertex )
{
	if( fabs(x) > m_MaxRate ) x = (x < 0) ? -m_MaxRate : m_MaxRate;
	if( fabs(y) > m_MaxRate ) y = (y < 0) ? -m_MaxRate : m_MaxRate;

#ifndef _XDEF_SYSTEMVERTEX		    	
	INTERFACETVERTEX* pVertices  = NULL;
	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif
		if( contollupvertex )
		{
			pVertices[0].x	= pVertices[3].x + x;
			pVertices[0].y	= pVertices[3].y + y;
			pVertices[1].x	= pVertices[2].x + x;
			pVertices[1].y	= pVertices[2].y + y;
		}
		else
		{
			pVertices[2].x	= pVertices[1].x + x;
			pVertices[2].y	= pVertices[1].y + y;
			pVertices[3].x	= pVertices[0].x + x;
			pVertices[3].y	= pVertices[0].y + y;
		}
#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}
#endif
}

void _XStatic::MoveWindow( int X, int Y )
{
#ifndef _XDEF_SYSTEMVERTEX		    
	if( !m_pVB ) return;

	INTERFACETVERTEX* pVertices  = NULL;

	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif

		if( (pVertices[0].x	== (FLOAT)X) && (pVertices[0].y	== (FLOAT)Y) )return;

		FLOAT width  = pVertices[1].x - pVertices[0].x;
		FLOAT height = pVertices[3].y - pVertices[0].y;

		pVertices[0].x	= (FLOAT)X;
		pVertices[0].y	= (FLOAT)Y;

		pVertices[1].x	= (FLOAT)X+width;
		pVertices[1].y	= (FLOAT)Y;

		pVertices[2].x	= (FLOAT)X+width;
		pVertices[2].y	= (FLOAT)Y+height;

		pVertices[3].x	= (FLOAT)X;
		pVertices[3].y	= (FLOAT)Y+height;		

		m_fLeft   = (FLOAT)X;
		m_fRight  = (FLOAT)X + width;		
		m_fTop    = (FLOAT)Y;
		m_fBottom = (FLOAT)Y + height;
#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}
#endif

}

void _XStatic::MoveWindow( int left, int top, int right, int bottom )
{
#ifndef _XDEF_SYSTEMVERTEX		    
	if( !m_pVB ) return;

	INTERFACETVERTEX* pVertices  = NULL;

	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif

	m_fLeft		= (FLOAT)left;
	m_fTop		= (FLOAT)top;
	m_fRight	= (FLOAT)right;
	m_fBottom	= (FLOAT)bottom;	

	pVertices[0].x	= m_fLeft;
	pVertices[0].y	= m_fTop;

	pVertices[1].x	= m_fRight;
	pVertices[1].y	= m_fTop;

	pVertices[2].x	= m_fRight;
	pVertices[2].y	= m_fBottom;

	pVertices[3].x	= m_fLeft;
	pVertices[3].y	= m_fBottom;			


#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}
#endif
}

void _XStatic::ChangeUV( FLOAT u0,FLOAT u1,FLOAT u2,FLOAT u3, FLOAT v0,FLOAT v1,FLOAT v2,FLOAT v3 )
{
#ifndef _XDEF_SYSTEMVERTEX		    
	if( !m_pVB ) return;

	INTERFACETVERTEX* pVertices  = NULL;

	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif

	pVertices[0].tu = u0;
	pVertices[0].tv = v0;

	pVertices[1].tu = u1;
	pVertices[1].tv = v1;

	pVertices[2].tu = u2;
	pVertices[2].tv = v2;

	pVertices[3].tu = u3;
	pVertices[3].tv = v3;
	
#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}
#endif

}

void _XStatic::ChangeUV( FLOAT centerx,FLOAT centery, FLOAT size )
{
#ifndef _XDEF_SYSTEMVERTEX
	if( !m_pVB ) return;

	INTERFACETVERTEX* pVertices  = NULL;

	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif

	pVertices[0].tu = centerx - size;
	pVertices[0].tv = centery - size;

	pVertices[1].tu = centerx + size;
	pVertices[1].tv = centery - size;

	pVertices[2].tu = centerx + size;
	pVertices[2].tv = centery + size;

	pVertices[3].tu = centerx - size;
	pVertices[3].tv = centery + size;
	
#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}
#endif
}


void _XStatic::InverseUV( int mode )
{
#ifndef _XDEF_SYSTEMVERTEX		    
	if( !m_pVB ) return;

	INTERFACETVERTEX* pVertices  = NULL;

	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif
	
	FLOAT	_tu, _tv;

	_tu = pVertices[0].tu;
	_tv = pVertices[0].tv;

	if( mode == _XDEF_UVFLIPMODE_LR )
	{
		pVertices[0].tu	= pVertices[2].tu;
		pVertices[2].tu	= _tu;
	}
	if( mode == _XDEF_UVFLIPMODE_UD )
	{
		pVertices[0].tv	= pVertices[2].tv;	
		pVertices[2].tv	= _tv;
	}

	_tu = pVertices[1].tu;
	_tv = pVertices[1].tv;

	if( mode == _XDEF_UVFLIPMODE_LR )
	{
		pVertices[1].tu	= pVertices[3].tu;
		pVertices[3].tu	= _tu;
	}

	if( mode == _XDEF_UVFLIPMODE_UD )
	{
		pVertices[1].tv	= pVertices[3].tv;	
		pVertices[3].tv	= _tv;
	}

#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}
#endif
}

void _XStatic::MoveWindow( D3DXVECTOR3 topleft, D3DXVECTOR3 topright, D3DXVECTOR3 bottomleft, D3DXVECTOR3 bottomright )
{
#ifndef _XDEF_SYSTEMVERTEX		    
	if( !m_pVB ) return;

	INTERFACETVERTEX* pVertices  = NULL;

	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif		
		pVertices[0].x	= topleft.x;
		pVertices[0].y	= topleft.y;

		pVertices[1].x	= topright.x;
		pVertices[1].y	= topright.y;

		pVertices[2].x	= bottomright.x;
		pVertices[2].y	= bottomright.y;

		pVertices[3].x	= bottomleft.x;
		pVertices[3].y	= bottomleft.y;		
		
#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}
#endif
}

void _XStatic::Rotate( FLOAT angle )
{
	FLOAT left,top,right,bottom;
	FLOAT xhalfsize = ((m_fRight  - m_fLeft)/2.0f);
	FLOAT yhalfsize = ((m_fBottom - m_fTop )/2.0f);
	FLOAT windowcenterposX = m_fLeft + xhalfsize;
	FLOAT windowcenterposY = m_fTop  + yhalfsize;

#ifndef _XDEF_SYSTEMVERTEX		    
	if( !m_pVB ) return;

	INTERFACETVERTEX* pVertices  = NULL;

	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif		
		left	= -xhalfsize;
		top		= -yhalfsize;
		_XMeshMath_RotateCoord( left,  top,	angle );		
		pVertices[ 0 ].x = windowcenterposX + left;
		pVertices[ 0 ].y = windowcenterposY + top;

		right	=  xhalfsize;
		top		= -yhalfsize;
		_XMeshMath_RotateCoord( right, top,	angle );
		pVertices[ 1 ].x = windowcenterposX + right;
		pVertices[ 1 ].y = windowcenterposY + top;

		right	=  xhalfsize;
		bottom	=  yhalfsize;
		_XMeshMath_RotateCoord( right, bottom, angle );
		pVertices[ 2 ].x = windowcenterposX + right;
		pVertices[ 2 ].y = windowcenterposY + bottom;

		left	= -xhalfsize;
		bottom	=  yhalfsize;
		_XMeshMath_RotateCoord( left, bottom, angle );
		pVertices[ 3 ].x = windowcenterposX + left;
		pVertices[ 3 ].y = windowcenterposY + bottom;

#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}
#endif
}

BOOL _XStatic::TestAlphaChannel( int x, int y )
{
	if( m_TextureIndex1 == -1 || !m_pTextureArchive ) return FALSE;

	if( (x>(int)m_fLeft) && (x<(int)m_fRight ) && (y>(int)m_fTop ) && (y<(int)m_fBottom) )
	{
		return m_pTextureArchive->TestAlphaChannel( m_TextureIndex1, 
													(x -(int)m_fLeft) , (y - (int)m_fTop) );
	}
	return FALSE;
}

void _XStatic::Render( LPDIRECT3DTEXTURE9 pTexture )
{
	//assert( gpDev );
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 0 );

	gpDev->SetTexture( 0, pTexture );
	
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	
	gpDev->SetRenderState( D3DRS_ZENABLE,   D3DZB_FALSE );

	gpDev->SetFVF( D3DFVF_INTERFACETVERTEX );

#ifndef _XDEF_SYSTEMVERTEX
    gpDev->SetStreamSource( 0, m_pVB, 0, sizeof(INTERFACETVERTEX) );
    gpDev->SetIndices( m_pIB );
    gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, 2 );
#else
	gpDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 
		                           2, pIndices, D3DFMT_INDEX16,
	                               pVertices, sizeof( INTERFACETVERTEX )  );
#endif

//#endif
/*
	gpDev->SetRenderState( D3DRS_ZENABLE, g_RS_UseZBuffer );

	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);*/
}

void _XStatic::Render( void )
{	
	//assert( gpDev );	
	if( m_pTextureArchive )
		m_pTextureArchive->SetTexture( m_TextureIndex1 );
	
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	
	gpDev->SetRenderState( D3DRS_ZENABLE,   D3DZB_FALSE );

	gpDev->SetFVF( D3DFVF_INTERFACETVERTEX );

#ifndef _XDEF_SYSTEMVERTEX
    gpDev->SetStreamSource( 0, m_pVB, 0, sizeof(INTERFACETVERTEX) );
    gpDev->SetIndices( m_pIB );
    gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, 2 );
#else
	gpDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 
		                           2, pIndices, D3DFMT_INDEX16,
	                               pVertices, sizeof( INTERFACETVERTEX )  );
#endif

//#endif
/*
	gpDev->SetRenderState( D3DRS_ZENABLE, g_RS_UseZBuffer );

	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);*/
}

void _XStatic::RenderAlphaBlend( void )
{	
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 0 );

	if( m_pTextureArchive )
		m_pTextureArchive->SetTexture_NoRenderStyle( m_TextureIndex1 );
	gpDev->SetFVF( D3DFVF_INTERFACETVERTEX );
	
#ifndef _XDEF_SYSTEMVERTEX
    gpDev->SetStreamSource( 0, m_pVB, 0, sizeof(INTERFACETVERTEX) );
    gpDev->SetIndices( m_pIB );
    gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, 2 );
#else
	gpDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 
		                           2, pIndices, D3DFMT_INDEX16,
	                               pVertices, sizeof( INTERFACETVERTEX )  );
#endif
}

void _XStatic::RenderClampMode( void )
{
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 8 );
	
	if( m_pTextureArchive )
		m_pTextureArchive->SetTexture_NoRenderStyle( m_TextureIndex1 );
	gpDev->SetFVF( D3DFVF_INTERFACETVERTEX );
	
#ifndef _XDEF_SYSTEMVERTEX
    gpDev->SetStreamSource( 0, m_pVB, 0, sizeof(INTERFACETVERTEX) );
    gpDev->SetIndices( m_pIB );
    gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, 2 );
#else
	gpDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 
		2, pIndices, D3DFMT_INDEX16,
		pVertices, sizeof( INTERFACETVERTEX )  );
#endif
}

void _XStatic::RenderClampMode( LPDIRECT3DTEXTURE9 pTexture )
{
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 8 );
	
	if( pTexture )
		gpDev->SetTexture( 0, pTexture );		

	gpDev->SetFVF( D3DFVF_INTERFACETVERTEX );
	
#ifndef _XDEF_SYSTEMVERTEX
    gpDev->SetStreamSource( 0, m_pVB, 0, sizeof(INTERFACETVERTEX) );
    gpDev->SetIndices( m_pIB );
    gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, 2 );
#else
	gpDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 
		2, pIndices, D3DFMT_INDEX16,
		pVertices, sizeof( INTERFACETVERTEX )  );
#endif	
}

void _XStatic::RenderAlphaBlend( DWORD Alphalevel )
{		
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 0 );
			
	D3DCOLOR color = (Alphalevel << 24) + 0xFFFFFF;	
	gpDev->SetRenderState( D3DRS_TEXTUREFACTOR, color );
    
	if( m_pTextureArchive )
		m_pTextureArchive->SetTexture( m_TextureIndex1 );

	gpDev->SetFVF( D3DFVF_INTERFACETVERTEX );
	
#ifndef _XDEF_SYSTEMVERTEX
    gpDev->SetStreamSource( 0, m_pVB, 0, sizeof(INTERFACETVERTEX) );
    gpDev->SetIndices( m_pIB );
    gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, 2 );
#else
	gpDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 
		                           2, pIndices, D3DFMT_INDEX16,
	                               pVertices, sizeof( INTERFACETVERTEX )  );	
#endif	
}

void _XStatic::RenderSolid( void )
{			
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 1 );		
	gpDev->SetTexture(0,NULL);
	
	gpDev->SetFVF( D3DFVF_INTERFACETVERTEX );
	
#ifndef _XDEF_SYSTEMVERTEX
    gpDev->SetStreamSource( 0, m_pVB, 0, sizeof(INTERFACETVERTEX) );
    gpDev->SetIndices( m_pIB );
    gpDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, 2 );
#else
	gpDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 
		                           2, pIndices, D3DFMT_INDEX16,
	                               pVertices, sizeof( INTERFACETVERTEX )  );
#endif	
}

void _XStatic::DrawToolTip(_XGUIObject*& pfocusobject)
{
	if( m_TooltipCallbackFunc && ( !pfocusobject || (pfocusobject != _XDEF_DUMMYID_MESSAGEBOX) ) )
	{
		ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();

		if( scrnpos )
		{				
			if( TestAlphaChannel( scrnpos->x, scrnpos->z ) )
			{
				POINT pt = { _XFC_FloatToIntRet( m_fLeft ), _XFC_FloatToIntRet( m_fTop ) };
				SIZE size = { _XFC_FloatToIntRet( m_fRight - m_fLeft ), _XFC_FloatToIntRet( m_fBottom - m_fTop ) };
				m_TooltipCallbackFunc( pt, size );
			}
		}
	}
}

BOOL _XStatic::CheckMouseRayCollision( void )
{
	D3DXVECTOR3 a,b,c;	
	int pos = 0;

#ifndef _XDEF_SYSTEMVERTEX		    
	if( !m_pVB ) return;

	INTERFACETVERTEX* pVertices  = NULL;

	if( S_OK == LockVertexBuffer( 0, pVertices ) )
	{
#endif		
		WORD	Indices[6] = {0,1,2,2,3,0};
		
		int i = 0  ; for( i = 0; i < 2; i++ )
		{
			a.x = pVertices[Indices[pos  ] ].x;
			a.y = pVertices[Indices[pos  ] ].y;
			a.z = pVertices[Indices[pos  ] ].z;

			b.x = pVertices[Indices[pos+1] ].x;
			b.y = pVertices[Indices[pos+1] ].y;
			b.z = pVertices[Indices[pos+1] ].z;

			c.x = pVertices[Indices[pos+2] ].x;
			c.y = pVertices[Indices[pos+2] ].y;
			c.z = pVertices[Indices[pos+2] ].z;
			
			if( _XIntersectTriangle( a, b, c ) )
			{
#ifndef _XDEF_SYSTEMVERTEX		    
				UnlockVertexBuffer();
#endif
				return TRUE;
			}

			pos+=3;
		}	

#ifndef _XDEF_SYSTEMVERTEX		    
		UnlockVertexBuffer();
	}
#endif


	return FALSE;	
}


