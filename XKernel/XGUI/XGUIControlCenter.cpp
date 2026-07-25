// XGUIControlCenter.cpp: implementation of the _XGUIControlCenter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern	_XTextureManager	g_MainInterfaceTextureArchive;

_XGUIControlCenter::_XGUIControlCenter()
{
	m_GUITimer.Init_All();
}

_XGUIControlCenter::~_XGUIControlCenter()
{	
	
}

BOOL _XGUIControlCenter::Initialize( void )
{
	g_SystemColorTable[_XSYSCOLOR_DEFAULT			]	=	D3DCOLOR_ARGB( 255, 189, 188, 175 );
	g_SystemColorTable[_XSYSCOLOR_DEFAULT_HIGHLIGHT	]	=	D3DCOLOR_ARGB( 255, 255, 255, 255 );
	g_SystemColorTable[_XSYSCOLOR_DEFAULT_CLICK		]	=	D3DCOLOR_ARGB( 255, 110, 109, 102 );
	g_SystemColorTable[_XSYSCOLOR_WARNING			]	=	D3DCOLOR_ARGB( 255, 214,  15,  15 );
	g_SystemColorTable[_XSYSCOLOR_WARNING_HIGHLIGHT	]	=	D3DCOLOR_ARGB( 255, 255, 123, 113 );
	g_SystemColorTable[_XSYSCOLOR_WARNING_CLICK		]	=	D3DCOLOR_ARGB( 255,  80,  13,  13 );	
	g_SystemColorTable[_XSYSCOLOR_AREAINFO			]	=	D3DCOLOR_ARGB( 255,   0,   0,   0 );
	g_SystemColorTable[_XSYSCOLOR_GAUGENUMBER		]	=	D3DCOLOR_ARGB( 255, 184, 184, 184 );
	g_SystemColorTable[_XSYSCOLOR_INFORMATION		]	=	D3DCOLOR_ARGB( 255, 219, 196, 136 );
	g_SystemColorTable[_XSYSCOLOR_TABMENU_1			]	=	D3DCOLOR_ARGB( 255, 181, 136,  96 );
	g_SystemColorTable[_XSYSCOLOR_TABMENU_2			]	=	D3DCOLOR_ARGB( 255, 132, 105,  81 );
	g_SystemColorTable[_XSYSCOLOR_ITEM_SELECT		]	=	D3DCOLOR_ARGB( 255, 150, 157, 253 );
	g_SystemColorTable[_XSYSCOLOR_ITEM_CLICK		]	=	D3DCOLOR_ARGB( 255, 207, 210, 253 );

	return TRUE;
}

void _XGUIControlCenter::Release( void )
{

}

void _XDrawLine( D3DXVECTOR3& startvec, D3DXVECTOR3& endvec, D3DCOLOR color )
{	
	BOXVERTEX vertlist[2];
	vertlist[0].color = vertlist[1].color = color;	
	vertlist[0].p = startvec;
	vertlist[1].p = endvec;	
	
	gpDev->SetTexture(0, NULL); 	
	gpDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	
	gpDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	gpDev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	gpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );
	gpDev->SetRenderState( D3DRS_ZENABLE,   D3DZB_FALSE );
	
	gpDev->SetFVF( D3DFVF_BOXVERTEX ); 
	gpDev->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertlist, sizeof(BOXVERTEX));	
	
	gpDev->SetRenderState( D3DRS_ZENABLE, g_RS_UseZBuffer );
	gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	gpDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

void _XDrawLine2( D3DXVECTOR3& startvec, D3DXVECTOR3& endvec, D3DCOLOR color )
{	
	BOXVERTEX vertlist[2];
	vertlist[0].color = vertlist[1].color = color;	
	vertlist[0].p = startvec;
	vertlist[1].p = endvec;	
	
	gpDev->SetFVF( D3DFVF_BOXVERTEX ); 
	gpDev->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertlist, sizeof(BOXVERTEX));	
}

void _XDrawRectAngle( FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT baseheight, D3DCOLOR color )
{
	if( right < left ) _SWAP( right , left );
	if( bottom < top ) _SWAP( bottom , top );
	
	_XWINDOWEFFECTVERTEXTYPE vertlist[8];
	int i = 0 ; for(  i = 0; i < 8; i++ )
	{
		vertlist[i].diffuse = color;
		vertlist[i].z = 0.5f;
		vertlist[i].rhw = _XDEF_DEFAULTDEPTH_FORINTERFACE;
	}
	
	vertlist[0].x = left+1;
	vertlist[0].y = top;
	vertlist[1].x = right;
	vertlist[1].y = top;	
	
	vertlist[2].x = left;
	vertlist[2].y = top;
	vertlist[3].x = left;
	vertlist[3].y = bottom;
	
	vertlist[4].x = right;
	vertlist[4].y = top;
	vertlist[5].x = right;
	vertlist[5].y = bottom;
	
	vertlist[6].x = left;
	vertlist[6].y = bottom;
	vertlist[7].x = right+1;
	vertlist[7].y = bottom;
		
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 1 );
		
	gpDev->SetFVF( D3DFVF_XWINDOWEFFECTVERTEX ); 
	gpDev->DrawPrimitiveUP(D3DPT_LINELIST, 4, vertlist, sizeof(_XWINDOWEFFECTVERTEXTYPE));
	
	//gpDev->SetRenderState( D3DRS_ZENABLE, g_RS_UseZBuffer );
	
	
	//gpDev->SetRenderState(D3DRS_LIGHTING, TRUE);	
	//gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//gpDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

void _XDrawShadowedRectAngle( FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT baseheight, D3DCOLOR highlightcolor, D3DCOLOR shadowcolor  )
{
	if( right < left ) _SWAP( right , left );
	if( bottom < top ) _SWAP( bottom , top );
	
	_XWINDOWEFFECTVERTEXTYPE vertlist[8];
	int i = 0;for( ; i < 4; i++ )
	{
		vertlist[i].diffuse = highlightcolor;
		vertlist[i].z = 0.5f;
		vertlist[i].rhw = _XDEF_DEFAULTDEPTH_FORINTERFACE;
	}
	for( i = 4; i < 8; i++ )
	{
		vertlist[i].diffuse = shadowcolor;
		vertlist[i].z = 0.5f;
		vertlist[i].rhw = _XDEF_DEFAULTDEPTH_FORINTERFACE;
	}
	
	vertlist[0].x = left+1;
	vertlist[0].y = top;
	vertlist[1].x = right;
	vertlist[1].y = top;	
	
	vertlist[2].x = left;
	vertlist[2].y = top;
	vertlist[3].x = left;
	vertlist[3].y = bottom;
	
	vertlist[4].x = right;
	vertlist[4].y = top;
	vertlist[5].x = right;
	vertlist[5].y = bottom;
	
	vertlist[6].x = left;
	vertlist[6].y = bottom;
	vertlist[7].x = right+1;
	vertlist[7].y = bottom;
		
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 1 );
		
	gpDev->SetFVF( D3DFVF_XWINDOWEFFECTVERTEX ); 
	gpDev->DrawPrimitiveUP(D3DPT_LINELIST, 4, vertlist, sizeof(_XWINDOWEFFECTVERTEXTYPE));
	
	//gpDev->SetRenderState( D3DRS_ZENABLE, g_RS_UseZBuffer );
	
	
	//gpDev->SetRenderState(D3DRS_LIGHTING, TRUE);	
	//gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//gpDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

void _XDrawRoundRectAngle( FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, FLOAT baseheight, D3DCOLOR color )
{
	if( right < left ) _SWAP( right , left );
	if( bottom < top ) _SWAP( bottom , top );
	
	_XWINDOWEFFECTVERTEXTYPE vertlist[8];
	int i = 0 ; for(  i = 0; i < 8; i++ )
	{
		vertlist[i].diffuse = color;
		vertlist[i].z = 0.5f;
		vertlist[i].rhw = _XDEF_DEFAULTDEPTH_FORINTERFACE;
	}
	
	vertlist[0].x = left+1;
	vertlist[0].y = top;
	vertlist[1].x = right;
	vertlist[1].y = top;	
	
	vertlist[2].x = left;
	vertlist[2].y = top+1;
	vertlist[3].x = left;
	vertlist[3].y = bottom;
	
	vertlist[4].x = right;
	vertlist[4].y = top+1;
	vertlist[5].x = right;
	vertlist[5].y = bottom;
	
	vertlist[6].x = left+1;
	vertlist[6].y = bottom;
	vertlist[7].x = right;
	vertlist[7].y = bottom;
	
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 1 );
	
	gpDev->SetFVF( D3DFVF_XWINDOWEFFECTVERTEX ); 
	gpDev->DrawPrimitiveUP(D3DPT_LINELIST, 4, vertlist, sizeof(_XWINDOWEFFECTVERTEXTYPE));
	
	//gpDev->SetRenderState( D3DRS_ZENABLE, g_RS_UseZBuffer );
	
	
	//gpDev->SetRenderState(D3DRS_LIGHTING, TRUE);	
	//gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//gpDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

void _XDrawLine2D( FLOAT px1, FLOAT py1, FLOAT px2, FLOAT py2, D3DCOLOR color1, D3DCOLOR color2 )
{				
	_XWINDOWEFFECTVERTEXTYPE vertlist[2];	
	vertlist[1].diffuse = color1;
	vertlist[0].diffuse = color2;
	vertlist[1].z = vertlist[0].z = 0.5f;
	vertlist[1].rhw = vertlist[0].rhw = _XDEF_DEFAULTDEPTH_FORINTERFACE;
		
	vertlist[0].x = px1;
	vertlist[0].y = py1;
	vertlist[1].x = px2;
	vertlist[1].y = py2;
	
	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 1 );		
	gpDev->SetFVF( D3DFVF_XWINDOWEFFECTVERTEX ); 
	gpDev->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertlist, sizeof(_XWINDOWEFFECTVERTEXTYPE));
}



WORD	g_SolidBarIndices[6] = {0,1,2,2,3,0};

void _XDrawSolidBar(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, D3DCOLOR color)
{
	if( right < left ) _SWAP( right , left );
	if( bottom < top ) _SWAP( bottom , top );
	
	INTERFACETVERTEX	QVertices[4] =
    {   
		{ left-0.5f,  top-0.5f,    0.5f,		_XDEF_DEFAULTDEPTH_FORINTERFACE, color, 0.0f, 0.0f },
        { right-0.5f, top-0.5f,   0.5f,		_XDEF_DEFAULTDEPTH_FORINTERFACE, color, 1.0f, 0.0f },
		{ right-0.5f, bottom-0.5f,0.5f,		_XDEF_DEFAULTDEPTH_FORINTERFACE, color, 1.0f, 1.0f },
		{ left-0.5f,  bottom-0.5f, 0.5f,		_XDEF_DEFAULTDEPTH_FORINTERFACE, color, 0.0f, 1.0f }
    };
	

	g_MainInterfaceTextureArchive.m_RenderStyleManager.SetRenderStyle( NULL, 1 );
	gpDev->SetFVF( D3DFVF_INTERFACETVERTEX );
	gpDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 2, g_SolidBarIndices, D3DFMT_INDEX16, QVertices, sizeof( INTERFACETVERTEX )  );
}

void _XDrawCircle( D3DXVECTOR3 *pCenter, FLOAT fRadius, UINT nLineSegs,DWORD color )
{
	D3DXVECTOR3 vPoints[64];
	DWORD i;
	D3DXMATRIX mRot;
	
	if( nLineSegs > 64 ) nLineSegs = 64;
	else if( nLineSegs < 8 ) nLineSegs = 8;
	
	vPoints[0].x = 0.0f;	
	vPoints[0].y = 0.0f;	
	vPoints[0].z = fRadius;	
	
	D3DXMatrixRotationY( &mRot, 6.283185307178f / nLineSegs );
	
	for( i=1; i < nLineSegs; i++ )
	{
		vPoints[i] = vPoints[i-1];
		D3DXVec3TransformCoord( &vPoints[i], &vPoints[i], &mRot );		
	}
	
	gpDev->SetTexture(0, NULL); 	
	gpDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	
	gpDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	gpDev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	gpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );
	gpDev->SetRenderState( D3DRS_ZENABLE, g_RS_UseZBuffer );
	//gpDev->SetRenderState( D3DRS_ZENABLE,   D3DZB_FALSE );

	gpDev->SetFVF( D3DFVF_BOXVERTEX ); 		
	
	vPoints[0] += *pCenter;
	for( i = 1; i < nLineSegs; i++ )
	{
		vPoints[i] += *pCenter;
		_XDrawLine2( vPoints[i-1], vPoints[i], color);
	}	
	
	_XDrawLine2( vPoints[nLineSegs-1], vPoints[0], color);

	//gpDev->SetRenderState( D3DRS_ZENABLE, g_RS_UseZBuffer );
	gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	gpDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

_XFontManager_Baseclass* _XGetFont( _XFONTID fontid )
{
	switch(fontid)
	{
	case _XFONTID_SMALL : return g_XSmallFont; break;
	default:
	case _XFONTID_BASE  : return g_XBaseFont;  break;
	case _XFONTID_LARGE : return g_XLargeFont; break;
	}

	return g_XBaseFont;
}