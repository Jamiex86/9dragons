// XRenderStyleManager.cpp: implementation of the _XRenderStyleManager class.
//
//////////////////////////////////////////////////////////////////////

// Check out test

#include "stdafx.h"
#include "XKernel.h"

#include "XTextureController.h"

BOOL _XRenderStyleManager::m_UseVertexShader = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
_XRenderStyleItem::_XRenderStyleItem()
{
	memset( m_rsname, 0, sizeof(TCHAR) * 32 );
	
	int i = 0 ; for(  i = 0; i < 4; i++ )
	{
		m_textureindex[i]			=   -1;
		m_upanningfactor[i]			=	0.0f;
		m_vpanningfactor[i]			=	0.0f;
		m_yawrotationfactor[i]		=	0.0f;
		m_pitchrotationfactor[i]	=	0.0f;
		m_rollrotationfactor[i]		=	0.0f;
	}
}

_XRenderStyleItem::~_XRenderStyleItem()
{

}

BOOL g_LastRenderStyleTextureState = FALSE;
extern BOOL g_SetTextureFailed;

void _XRenderStyleItem::SetRenderStyle(  _XTextureManager* pTextureManager  )
{	
	if( g_LastUsedRenderStyle == this ) 
	{
		g_SetTextureFailed = g_LastRenderStyleTextureState;
		return;
	}
	g_LastUsedRenderStyle = this;

	gpDev->SetRenderState(D3DRS_LIGHTING,m_rs.uselight);
	gpDev->SetRenderState( D3DRS_SPECULARENABLE, m_rs.usespecular );
	
	if( g_pCurrentSelectMaterial )
	{
		gpDev->SetMaterial(g_pCurrentSelectMaterial);		
	}
	else gpDev->SetMaterial(&m_rs.material);
	
	switch (m_rs.zbufferoperationmode) 
	{
	case _XRS_ZBUFFEROPMODE_READWRITE :
		gpDev->SetRenderState(D3DRS_ZENABLE,g_RS_UseZBuffer);
		gpDev->SetRenderState(D3DRS_ZWRITEENABLE,TRUE); 
		break;
	case _XRS_ZBUFFEROPMODE_READONLY :
		gpDev->SetRenderState(D3DRS_ZENABLE,g_RS_UseZBuffer);
		gpDev->SetRenderState(D3DRS_ZWRITEENABLE,FALSE); 
		break;
	case _XRS_ZBUFFEROPMODE_NOOPERATION :
		gpDev->SetRenderState(D3DRS_ZENABLE,FALSE);
		gpDev->SetRenderState(D3DRS_ZWRITEENABLE,FALSE); 
		break; 
	}
	gpDev->SetRenderState(D3DRS_ZFUNC, m_rs.zbuffercomparemode );	
			
	gpDev->SetRenderState(D3DRS_ALPHATESTENABLE,m_rs.alphatest );
	gpDev->SetRenderState(D3DRS_ALPHAFUNC, m_rs.alphatestcomparemode ); 
	gpDev->SetRenderState(D3DRS_ALPHAREF,m_rs.alphareference);

	switch( m_rs.blendmode )
	{
	case _XRS_NOBLEND					:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		break;
	case _XRS_BLEND_BLENDINGON			:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		break;
	case _XRS_BLEND_ADD					:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		break;
	case _XRS_BLEND_ADDALPHA			:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_SRCALPHA);
		break;
	case _XRS_BLEND_SATURATE			:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_INVDESTCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		break;
	case _XRS_BLEND_MOD_SRCALPHA		:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		break;
	case _XRS_BLEND_MOD_SRCCOLOR		:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);
		break;
	case _XRS_BLEND_MOD_DSTCOLOR		:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_DESTCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVDESTCOLOR);
		break;
	case _XRS_BLEND_MUL_SRCCOL_DSTCOL	:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_DESTCOLOR);
		break;
	case _XRS_BLEND_MUL_SRCCOL_ONE		:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		break;
	case _XRS_BLEND_MUL_SRCALPHA_ZERO	:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
		break;
	case _XRS_BLEND_MUL_SRCALPHA_ONE	:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		break;
	case _XRS_BLEND_MUL_DSTCOL_ZERO		:	
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_DESTCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
		break;
	}
	
	gpDev->SetRenderState(D3DRS_CULLMODE,m_rs.cullingmode);
	
	if( g_RenderAlphaBlendFlag )
	{
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		gpDev->SetRenderState(D3DRS_TEXTUREFACTOR,g_RenderAlphaBlendTextureFactor);
	}
	else
	{
		gpDev->SetRenderState(D3DRS_TEXTUREFACTOR,m_rs.texturefactor);	
	}
			
	// Color operations

	if( g_pCurrentSelectMaterial )
	{
		if( g_SelectedMaterialNoModulate2X )
			 gpDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		else gpDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
			
	}
	else
	{
		gpDev->SetTextureStageState( 0, D3DTSS_COLOROP, m_rs.ts1_coloroperation );
	}

	gpDev->SetTextureStageState( 1, D3DTSS_COLOROP, m_rs.ts2_coloroperation );
	gpDev->SetTextureStageState( 2, D3DTSS_COLOROP, m_rs.ts3_coloroperation );
	gpDev->SetTextureStageState( 3, D3DTSS_COLOROP, m_rs.ts4_coloroperation );

		
	switch( m_rs.ts1_colorarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEMP ); break;	
	}
	
	switch( m_rs.ts2_colorarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_colorarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_colorarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEMP ); break;	
	}	

	switch( m_rs.ts1_colorarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts2_colorarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_colorarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_colorarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_TEMP ); break;	
	}
			
	// Alpha operations
	gpDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,m_rs.ts1_alphaoperation);
	gpDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,m_rs.ts2_alphaoperation);
	gpDev->SetTextureStageState( 2, D3DTSS_ALPHAOP,m_rs.ts3_alphaoperation);
	gpDev->SetTextureStageState( 3, D3DTSS_ALPHAOP,m_rs.ts4_alphaoperation);

	switch( m_rs.ts1_alphaarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts2_alphaarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_alphaarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_alphaarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TEMP ); break;	
	}	
	
	switch( m_rs.ts1_alphaarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts2_alphaarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_alphaarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_alphaarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_TEMP ); break;	
	}

	if( g_RenderAlphaBlendFlag )
	{
		if( g_RenderAlphaBlendUseAlphaChannelFlag )
		{
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR   );
		}
		else
		{
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		}
	}
	
	if( pTextureManager )
	{
		switch (m_rs.ts1_sourcetype)
		{
		case 0 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[0], 0);  break;
		case 1 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[1], 0);  break;
		case 2 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[2], 0);  break;
		case 3 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[3], 0);  break;
		case 4 : gpDev->SetTexture(0,NULL); break;
		}
		switch (m_rs.ts2_sourcetype)
		{
		case 0 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[0], 1);  break;
		case 1 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[1], 1);  break;
		case 2 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[2], 1);  break;
		case 3 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[3], 1);  break;
		case 4 : gpDev->SetTexture(1,NULL); break;
		}
		switch (m_rs.ts3_sourcetype)
		{
		case 0 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[0], 2);  break;
		case 1 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[1], 2);  break;
		case 2 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[2], 2);  break;
		case 3 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[3], 2);  break;
		case 4 : gpDev->SetTexture(2,NULL); break;
		}
		switch (m_rs.ts4_sourcetype)
		{
		case 0 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[0], 3);  break;
		case 1 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[1], 3);  break;
		case 2 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[2], 3);  break;
		case 3 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[3], 3);  break;
		case 4 : gpDev->SetTexture(3,NULL); break;
		}
	}	
	else
	{
		gpDev->SetTexture(0,NULL);
		gpDev->SetTexture(1,NULL);
		gpDev->SetTexture(2,NULL);
		gpDev->SetTexture(3,NULL);
	}

	if (_XRenderStyleManager::m_UseVertexShader == FALSE)
	{
		switch (m_rs.ts1_uvcoordindex) 
		{
		case _XRS_UVFROM_MODELDATA_UVSET1		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0); break; 
		case _XRS_UVFROM_MODELDATA_UVSET2		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1); break;
		case _XRS_UVFROM_MODELDATA_UVSET3		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2); break;
		case _XRS_UVFROM_MODELDATA_UVSET4		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3); break; 
		case _XRS_UVFROM_CAMERASPACENORMAL		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 0); break;
		case _XRS_UVFROM_CAMERASPACEPOSITION	:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 0); break;
		case _XRS_UVFROM_CAMERASPACEREFLTVECT	:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 0); break; 
		case _XRS_UVFROM_WORLDSPACENORMAL		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 0);	break;
		case _XRS_UVFROM_WORLDSPACEPOSITION		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 0); break;
		case _XRS_UVFROM_WORLDSPACEREFLTVECT	:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 0); break; 
		}

		switch (m_rs.ts2_uvcoordindex) 
		{
		case _XRS_UVFROM_MODELDATA_UVSET1		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0); break; 
		case _XRS_UVFROM_MODELDATA_UVSET2		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1); break;
		case _XRS_UVFROM_MODELDATA_UVSET3		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2); break;
		case _XRS_UVFROM_MODELDATA_UVSET4		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3); break; 
		case _XRS_UVFROM_CAMERASPACENORMAL		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 1); break;
		case _XRS_UVFROM_CAMERASPACEPOSITION	:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 1); break;
		case _XRS_UVFROM_CAMERASPACEREFLTVECT	:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 1); break; 
		case _XRS_UVFROM_WORLDSPACENORMAL		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 1);	break;
		case _XRS_UVFROM_WORLDSPACEPOSITION		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 1); break;
		case _XRS_UVFROM_WORLDSPACEREFLTVECT	:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 1); break; 
		}

		switch (m_rs.ts3_uvcoordindex) 
		{
		case _XRS_UVFROM_MODELDATA_UVSET1		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0); break; 
		case _XRS_UVFROM_MODELDATA_UVSET2		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1); break;
		case _XRS_UVFROM_MODELDATA_UVSET3		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2); break;
		case _XRS_UVFROM_MODELDATA_UVSET4		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3); break; 
		case _XRS_UVFROM_CAMERASPACENORMAL		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 2); break;
		case _XRS_UVFROM_CAMERASPACEPOSITION	:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 2); break;
		case _XRS_UVFROM_CAMERASPACEREFLTVECT	:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 2); break; 
		case _XRS_UVFROM_WORLDSPACENORMAL		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 2);	break;
		case _XRS_UVFROM_WORLDSPACEPOSITION		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 2); break;
		case _XRS_UVFROM_WORLDSPACEREFLTVECT	:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 2); break; 
		}

		switch (m_rs.ts4_uvcoordindex) 
		{
		case _XRS_UVFROM_MODELDATA_UVSET1		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0); break; 
		case _XRS_UVFROM_MODELDATA_UVSET2		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1); break;
		case _XRS_UVFROM_MODELDATA_UVSET3		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2); break;
		case _XRS_UVFROM_MODELDATA_UVSET4		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3); break; 
		case _XRS_UVFROM_CAMERASPACENORMAL		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 3); break;
		case _XRS_UVFROM_CAMERASPACEPOSITION	:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 3); break;
		case _XRS_UVFROM_CAMERASPACEREFLTVECT	:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 3); break; 
		case _XRS_UVFROM_WORLDSPACENORMAL		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 3);	break;
		case _XRS_UVFROM_WORLDSPACEPOSITION		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 3); break;
		case _XRS_UVFROM_WORLDSPACEREFLTVECT	:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 3); break; 
		}
	}
	else
	{
		gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0);
		gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1);
		gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2);
		gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3);
	}

	gpDev->SetSamplerState(0,D3DSAMP_ADDRESSU,m_rs.ts1_ucoordadress);
	gpDev->SetSamplerState(1,D3DSAMP_ADDRESSU,m_rs.ts2_ucoordadress);
	gpDev->SetSamplerState(2,D3DSAMP_ADDRESSU,m_rs.ts3_ucoordadress);
	gpDev->SetSamplerState(3,D3DSAMP_ADDRESSU,m_rs.ts4_ucoordadress);

	gpDev->SetSamplerState(0,D3DSAMP_ADDRESSV,m_rs.ts1_vcoordadress);
	gpDev->SetSamplerState(1,D3DSAMP_ADDRESSV,m_rs.ts2_vcoordadress);
	gpDev->SetSamplerState(2,D3DSAMP_ADDRESSV,m_rs.ts3_vcoordadress);
	gpDev->SetSamplerState(3,D3DSAMP_ADDRESSV,m_rs.ts4_vcoordadress);
	
	if( _XRenderStyleManager::m_UseVertexShader )
	{
		gpDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
		gpDev->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
		gpDev->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
		gpDev->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
	}
	else
	{	
		if( m_rs.useuvmatrix1 )
		{
			UINT TTF = m_rs.ts1_texturecoordcount;
			if( m_rs.ts1_projected )		
				if( g_d3dCaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED ) TTF |= D3DTTFF_PROJECTED;		
			
			gpDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, TTF);

			if( m_uveffect[0].useextexduveffect > _XRS_UVEFFECTYPE_DISABLE )
			{
				D3DXMATRIX	modulatematrix;
				D3DXMATRIX	rotation;
				D3DXMATRIX	translation2;
				
				modulatematrix = m_rs.uvmatrix[0];
				
				D3DXMatrixIdentity( &translation2 );
				translation2._31 = -m_rs.uvmatrix[0]._31;
				translation2._32 = -m_rs.uvmatrix[0]._32;
				
				if( m_uveffect[0].userotationeffect )
				{
					if( m_uveffect[0].yawrotationvelocity != 0.0f )
					{
						m_yawrotationfactor[0] += (m_uveffect[0].yawrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_yawrotationfactor[0] >= 6.283185 ) m_yawrotationfactor[0] = 0.0f;
						if( m_yawrotationfactor[0] <= -6.283185 ) m_yawrotationfactor[0] = 0.0f;
					}
					
					if( m_uveffect[0].pitchrotationvelocity != 0.0f )
					{
						m_pitchrotationfactor[0] += (m_uveffect[0].pitchrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_pitchrotationfactor[0] >= 6.283185 ) m_pitchrotationfactor[0] = 0.0f;
						if( m_pitchrotationfactor[0] <= -6.283185 ) m_pitchrotationfactor[0] = 0.0f;
					}
					
					if( m_uveffect[0].rollrotationvelocity != 0.0f )
					{
						m_rollrotationfactor[0] += (m_uveffect[0].rollrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_rollrotationfactor[0] >= 6.283185 ) m_rollrotationfactor[0] = 0.0f;
						if( m_rollrotationfactor[0] <= -6.283185 ) m_rollrotationfactor[0] = 0.0f;
					}
					D3DXMatrixRotationYawPitchRoll( &rotation, m_yawrotationfactor[0],  m_pitchrotationfactor[0], m_rollrotationfactor[0] );				
					
					modulatematrix = m_rs.uvmatrix[0] * rotation;
				}
				
				modulatematrix = modulatematrix * translation2;
				
				if( m_uveffect[0].upanning != 0.0f || m_uveffect[0].vpanning != 0.0f )
				{
					m_upanningfactor[0] += (m_uveffect[0].upanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[0].upanning;
					m_vpanningfactor[0] += (m_uveffect[0].vpanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[0].vpanning;
					
					if( m_uveffect[0].useextexduveffect == _XRS_UVEFFECTYPE_PANNING )
					{
						translation2._31 += m_upanningfactor[0];
						translation2._32 += m_vpanningfactor[0];
					}
					else //if( m_uveffect[0].useextexduveffect == _XRS_UVEFFECTYPE_OSCILLATION )
					{
						translation2._31 = sin( m_upanningfactor[0] );
						translation2._32 = sin( m_vpanningfactor[0] );
					}
					
					modulatematrix = modulatematrix * translation2;
				}
				
				gpDev->SetTransform(D3DTS_TEXTURE0, &modulatematrix );
			}
			else
			{
				gpDev->SetTransform(D3DTS_TEXTURE0, &m_rs.uvmatrix[0] );
			}
		}
		else gpDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

		if( m_rs.useuvmatrix2 )
		{				
			UINT TTF = m_rs.ts2_texturecoordcount;
			if( m_rs.ts2_projected ) 
				if( g_d3dCaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED ) TTF |= D3DTTFF_PROJECTED;		

			gpDev->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, TTF);

			if( m_uveffect[1].useextexduveffect > _XRS_UVEFFECTYPE_DISABLE )
			{
				D3DXMATRIX	modulatematrix;
				D3DXMATRIX	rotation;
				D3DXMATRIX	translation2;
				
				modulatematrix = m_rs.uvmatrix[1];
				
				D3DXMatrixIdentity( &translation2 );
				translation2._31 = -m_rs.uvmatrix[1]._31;
				translation2._32 = -m_rs.uvmatrix[1]._32;
				
				if( m_uveffect[1].userotationeffect )
				{
					if( m_uveffect[1].yawrotationvelocity != 0.0f )
					{
						m_yawrotationfactor[1] += (m_uveffect[1].yawrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_yawrotationfactor[1] >= 6.283185 ) m_yawrotationfactor[1] = 0.0f;
						if( m_yawrotationfactor[1] <= -6.283185 ) m_yawrotationfactor[1] = 0.0f;
					}
					
					if( m_uveffect[1].pitchrotationvelocity != 0.0f )
					{
						m_pitchrotationfactor[1] += (m_uveffect[1].pitchrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_pitchrotationfactor[1] >= 6.283185 ) m_pitchrotationfactor[1] = 0.0f;
						if( m_pitchrotationfactor[1] <= -6.283185 ) m_pitchrotationfactor[1] = 0.0f;
					}
					
					if( m_uveffect[1].rollrotationvelocity != 0.0f )
					{
						m_rollrotationfactor[1] += (m_uveffect[1].rollrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_rollrotationfactor[1] >= 6.283185 ) m_rollrotationfactor[1] = 0.0f;
						if( m_rollrotationfactor[1] <= -6.283185 ) m_rollrotationfactor[1] = 0.0f;
					}
					D3DXMatrixRotationYawPitchRoll( &rotation, m_yawrotationfactor[1],  m_pitchrotationfactor[1], m_rollrotationfactor[1] );				
					
					modulatematrix = m_rs.uvmatrix[1] * rotation;
				}
				
				modulatematrix = modulatematrix * translation2;
				
				if( m_uveffect[1].upanning != 0.0f || m_uveffect[1].vpanning != 0.0f )
				{
					m_upanningfactor[1] += (m_uveffect[1].upanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[1].upanning;
					m_vpanningfactor[1] += (m_uveffect[1].vpanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[1].vpanning;
					
					if( m_uveffect[1].useextexduveffect == _XRS_UVEFFECTYPE_PANNING )
					{
						translation2._31 += m_upanningfactor[1];
						translation2._32 += m_vpanningfactor[1];
					}
					else //if( m_uveffect[1].useextexduveffect == _XRS_UVEFFECTYPE_OSCILLATION )
					{
						translation2._31 = sin( m_upanningfactor[1] );
						translation2._32 = sin( m_vpanningfactor[1] );
					}
					
					modulatematrix = modulatematrix * translation2;
				}
				
				gpDev->SetTransform(D3DTS_TEXTURE1, &modulatematrix );
			}
			else
			{
				gpDev->SetTransform(D3DTS_TEXTURE1, &m_rs.uvmatrix[1] );
			}
		}
		else gpDev->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
			
		if( m_rs.useuvmatrix3 )
		{				
			UINT TTF = m_rs.ts3_texturecoordcount;
			if( m_rs.ts3_projected ) 
				if( g_d3dCaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED ) TTF |= D3DTTFF_PROJECTED;		

			gpDev->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, TTF);

			if( m_uveffect[2].useextexduveffect > _XRS_UVEFFECTYPE_DISABLE  )
			{
				D3DXMATRIX	modulatematrix;
				D3DXMATRIX	rotation;
				D3DXMATRIX	translation2;
				
				modulatematrix = m_rs.uvmatrix[2];
				
				D3DXMatrixIdentity( &translation2 );
				translation2._31 = -m_rs.uvmatrix[2]._31;
				translation2._32 = -m_rs.uvmatrix[2]._32;
				
				if( m_uveffect[2].userotationeffect )
				{
					if( m_uveffect[2].yawrotationvelocity != 0.0f )
					{
						m_yawrotationfactor[2] += (m_uveffect[2].yawrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_yawrotationfactor[2] >= 6.283185 ) m_yawrotationfactor[2] = 0.0f;
						if( m_yawrotationfactor[2] <= -6.283185 ) m_yawrotationfactor[2] = 0.0f;
					}
					
					if( m_uveffect[2].pitchrotationvelocity != 0.0f )
					{
						m_pitchrotationfactor[2] += (m_uveffect[2].pitchrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_pitchrotationfactor[2] >= 6.283185 ) m_pitchrotationfactor[2] = 0.0f;
						if( m_pitchrotationfactor[2] <= -6.283185 ) m_pitchrotationfactor[2] = 0.0f;
					}
					
					if( m_uveffect[2].rollrotationvelocity != 0.0f )
					{
						m_rollrotationfactor[2] += (m_uveffect[2].rollrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_rollrotationfactor[2] >= 6.283185 ) m_rollrotationfactor[2] = 0.0f;
						if( m_rollrotationfactor[2] <= -6.283185 ) m_rollrotationfactor[2] = 0.0f;
					}
					D3DXMatrixRotationYawPitchRoll( &rotation, m_yawrotationfactor[2],  m_pitchrotationfactor[2], m_rollrotationfactor[2] );				
					
					modulatematrix = m_rs.uvmatrix[2] * rotation;
				}
				
				modulatematrix = modulatematrix * translation2;
				
				if( m_uveffect[2].upanning != 0.0f || m_uveffect[2].vpanning != 0.0f )
				{
					m_upanningfactor[2] += (m_uveffect[2].upanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[2].upanning;
					m_vpanningfactor[2] += (m_uveffect[2].vpanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[2].vpanning;
					
					if( m_uveffect[2].useextexduveffect == _XRS_UVEFFECTYPE_PANNING )
					{
						translation2._31 += m_upanningfactor[2];
						translation2._32 += m_vpanningfactor[2];
					}
					else //if( m_uveffect[2].useextexduveffect == _XRS_UVEFFECTYPE_OSCILLATION )
					{
						translation2._31 = sin( m_upanningfactor[2] );
						translation2._32 = sin( m_vpanningfactor[2] );
					}
					
					modulatematrix = modulatematrix * translation2;
				}
				
				gpDev->SetTransform(D3DTS_TEXTURE2, &modulatematrix );
			}
			else
			{
				gpDev->SetTransform(D3DTS_TEXTURE2, &m_rs.uvmatrix[2] );
			}
		}
		else gpDev->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

		if( m_rs.useuvmatrix4 )
		{		
			UINT TTF = m_rs.ts4_texturecoordcount;
			if( m_rs.ts4_projected ) 
				if( g_d3dCaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED ) TTF |= D3DTTFF_PROJECTED;		

			gpDev->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, TTF);		

			if( m_uveffect[3].useextexduveffect > _XRS_UVEFFECTYPE_DISABLE  )
			{
				D3DXMATRIX	modulatematrix;
				D3DXMATRIX	rotation;
				D3DXMATRIX	translation2;
				
				modulatematrix = m_rs.uvmatrix[3];
				
				D3DXMatrixIdentity( &translation2 );
				translation2._31 = -m_rs.uvmatrix[3]._31;
				translation2._32 = -m_rs.uvmatrix[3]._32;
				
				if( m_uveffect[3].userotationeffect )
				{
					if( m_uveffect[3].yawrotationvelocity != 0.0f )
					{
						m_yawrotationfactor[3] += (m_uveffect[3].yawrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_yawrotationfactor[3] >= 6.283185 ) m_yawrotationfactor[3] = 0.0f;
						if( m_yawrotationfactor[3] <= -6.283185 ) m_yawrotationfactor[3] = 0.0f;
					}
					
					if( m_uveffect[3].pitchrotationvelocity != 0.0f )
					{
						m_pitchrotationfactor[3] += (m_uveffect[3].pitchrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_pitchrotationfactor[3] >= 6.283185 ) m_pitchrotationfactor[3] = 0.0f;
						if( m_pitchrotationfactor[3] <= -6.283185 ) m_pitchrotationfactor[3] = 0.0f;
					}
					
					if( m_uveffect[3].rollrotationvelocity != 0.0f )
					{
						m_rollrotationfactor[3] += (m_uveffect[3].rollrotationvelocity * g_fElapsedFrameMilisecondTime);
						if( m_rollrotationfactor[3] >= 6.283185 ) m_rollrotationfactor[3] = 0.0f;
						if( m_rollrotationfactor[3] <= -6.283185 ) m_rollrotationfactor[3] = 0.0f;
					}
					D3DXMatrixRotationYawPitchRoll( &rotation, m_yawrotationfactor[3],  m_pitchrotationfactor[3], m_rollrotationfactor[3] );				
					
					modulatematrix = m_rs.uvmatrix[3] * rotation;
				}
				
				modulatematrix = modulatematrix * translation2;
				
				if( m_uveffect[3].upanning != 0.0f || m_uveffect[3].vpanning != 0.0f )
				{
					m_upanningfactor[3] += (m_uveffect[3].upanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[3].upanning;
					m_vpanningfactor[3] += (m_uveffect[3].vpanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[3].vpanning;
					
					if( m_uveffect[3].useextexduveffect == _XRS_UVEFFECTYPE_PANNING )
					{
						translation2._31 += m_upanningfactor[3];
						translation2._32 += m_vpanningfactor[3];
					}
					else //if( m_uveffect[3].useextexduveffect == _XRS_UVEFFECTYPE_OSCILLATION )
					{
						translation2._31 = sin( m_upanningfactor[3] );
						translation2._32 = sin( m_vpanningfactor[3] );
					}
					
					modulatematrix = modulatematrix * translation2;
				}
				
				gpDev->SetTransform(D3DTS_TEXTURE3, &modulatematrix );
			}
			else
			{
				gpDev->SetTransform(D3DTS_TEXTURE3, &m_rs.uvmatrix[3] );
			}
		}
		else gpDev->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);		
	}

	if( g_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP )
	{
		if( m_rs.usetransformbumpenvmapping )
		{
			SetBumpEnvMapProperties( m_rs.bumpenvmappingstage );
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVLSCALE,  FtoDW(m_rs.bumpmapscale)  );
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVLOFFSET, FtoDW(m_rs.bumpmapoffset) );
		}
		else
		{
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVMAT00, 0);
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVMAT01, 0);
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVMAT10, 0);
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVMAT11, 0); 
			
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVLSCALE,  0 );
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVLOFFSET, 0 );
		}
	}

	g_LastRenderStyleTextureState = g_SetTextureFailed;
}

void _XRenderStyleItem::SetRenderStyle( _XTextureController* pTextureManager  )
{	
	if( g_LastUsedRenderStyle == this ) return;
	g_LastUsedRenderStyle = this;

	gpDev->SetRenderState(D3DRS_LIGHTING,m_rs.uselight);
	gpDev->SetRenderState( D3DRS_SPECULARENABLE, m_rs.usespecular );
	
	if( g_pCurrentSelectMaterial )
	{
		gpDev->SetMaterial(g_pCurrentSelectMaterial);		
	}
	else gpDev->SetMaterial(&m_rs.material);
	
	switch (m_rs.zbufferoperationmode) 
	{
	case _XRS_ZBUFFEROPMODE_READWRITE :
		gpDev->SetRenderState(D3DRS_ZENABLE,g_RS_UseZBuffer);
		gpDev->SetRenderState(D3DRS_ZWRITEENABLE,TRUE); 
		break;
	case _XRS_ZBUFFEROPMODE_READONLY :
		gpDev->SetRenderState(D3DRS_ZENABLE,g_RS_UseZBuffer);
		gpDev->SetRenderState(D3DRS_ZWRITEENABLE,FALSE); 
		break;
	case _XRS_ZBUFFEROPMODE_NOOPERATION :
		gpDev->SetRenderState(D3DRS_ZENABLE,FALSE);
		gpDev->SetRenderState(D3DRS_ZWRITEENABLE,FALSE); 
		break; 
	}
	gpDev->SetRenderState(D3DRS_ZFUNC, m_rs.zbuffercomparemode );	
			
	gpDev->SetRenderState(D3DRS_ALPHATESTENABLE,m_rs.alphatest );
	gpDev->SetRenderState(D3DRS_ALPHAFUNC, m_rs.alphatestcomparemode ); 
	gpDev->SetRenderState(D3DRS_ALPHAREF,m_rs.alphareference);

	switch( m_rs.blendmode )
	{
	case _XRS_NOBLEND					:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		break;
	case _XRS_BLEND_BLENDINGON			:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		break;
	case _XRS_BLEND_ADD					:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		break;
	case _XRS_BLEND_ADDALPHA			:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_SRCALPHA);
		break;
	case _XRS_BLEND_SATURATE			:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_INVDESTCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		break;
	case _XRS_BLEND_MOD_SRCALPHA		:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		break;
	case _XRS_BLEND_MOD_SRCCOLOR		:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);
		break;
	case _XRS_BLEND_MOD_DSTCOLOR		:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_DESTCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVDESTCOLOR);
		break;
	case _XRS_BLEND_MUL_SRCCOL_DSTCOL	:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_DESTCOLOR);
		break;
	case _XRS_BLEND_MUL_SRCCOL_ONE		:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		break;
	case _XRS_BLEND_MUL_SRCALPHA_ZERO	:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
		break;
	case _XRS_BLEND_MUL_SRCALPHA_ONE	:
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		break;
	case _XRS_BLEND_MUL_DSTCOL_ZERO		:	
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_DESTCOLOR); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
		break;
	}
	
	gpDev->SetRenderState(D3DRS_CULLMODE,m_rs.cullingmode);
	
	if( g_RenderAlphaBlendFlag )
	{
		gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE); 
		gpDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
		gpDev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		gpDev->SetRenderState(D3DRS_TEXTUREFACTOR,g_RenderAlphaBlendTextureFactor);
	}
	else
	{
		gpDev->SetRenderState(D3DRS_TEXTUREFACTOR,m_rs.texturefactor);	
	}
			
	// Color operations

	if( g_pCurrentSelectMaterial )
	{
		if( g_SelectedMaterialNoModulate2X )
			 gpDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		else gpDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
			
	}
	else
	{
		gpDev->SetTextureStageState( 0, D3DTSS_COLOROP, m_rs.ts1_coloroperation );
	}

	gpDev->SetTextureStageState( 1, D3DTSS_COLOROP, m_rs.ts2_coloroperation );
	gpDev->SetTextureStageState( 2, D3DTSS_COLOROP, m_rs.ts3_coloroperation );
	gpDev->SetTextureStageState( 3, D3DTSS_COLOROP, m_rs.ts4_coloroperation );

		
	switch( m_rs.ts1_colorarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEMP ); break;	
	}
	
	switch( m_rs.ts2_colorarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_colorarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_colorarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEMP ); break;	
	}	

	switch( m_rs.ts1_colorarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts2_colorarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_colorarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_colorarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_TEMP ); break;	
	}
			
	// Alpha operations
	gpDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,m_rs.ts1_alphaoperation);
	gpDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,m_rs.ts2_alphaoperation);
	gpDev->SetTextureStageState( 2, D3DTSS_ALPHAOP,m_rs.ts3_alphaoperation);
	gpDev->SetTextureStageState( 3, D3DTSS_ALPHAOP,m_rs.ts4_alphaoperation);

	switch( m_rs.ts1_alphaarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts2_alphaarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_alphaarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_alphaarg1 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TEMP ); break;	
	}	
	
	switch( m_rs.ts1_alphaarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts2_alphaarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_alphaarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_TEMP ); break;	
	}
	switch( m_rs.ts3_alphaarg2 ) {
	default:
	case 0 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_TEXTURE ); break;
	case 1 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); break; 
	case 2 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_TFACTOR ); break;		
	case 3 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_CURRENT ); break;		
	case 4 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_SPECULAR ); break;
	case 5 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_SELECTMASK ); break;
	case 6 : gpDev->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_TEMP ); break;	
	}

	if( g_RenderAlphaBlendFlag )
	{
		if( g_RenderAlphaBlendUseAlphaChannelFlag )
		{
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR   );
		}
		else
		{
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			gpDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
		}
	}
	
	if( pTextureManager )
	{
		switch (m_rs.ts1_sourcetype)
		{
		case 0 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[0], 0);  break;
		case 1 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[1], 0);  break;
		case 2 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[2], 0);  break;
		case 3 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[3], 0);  break;
		case 4 : gpDev->SetTexture(0,NULL); break;
		}
		switch (m_rs.ts2_sourcetype)
		{
		case 0 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[0], 1);  break;
		case 1 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[1], 1);  break;
		case 2 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[2], 1);  break;
		case 3 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[3], 1);  break;
		case 4 : gpDev->SetTexture(1,NULL); break;
		}
		switch (m_rs.ts3_sourcetype)
		{
		case 0 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[0], 2);  break;
		case 1 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[1], 2);  break;
		case 2 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[2], 2);  break;
		case 3 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[3], 2);  break;
		case 4 : gpDev->SetTexture(2,NULL); break;
		}
		switch (m_rs.ts4_sourcetype)
		{
		case 0 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[0], 3);  break;
		case 1 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[1], 3);  break;
		case 2 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[2], 3);  break;
		case 3 : pTextureManager->SetTexture_NoRenderStyle( m_textureindex[3], 3);  break;
		case 4 : gpDev->SetTexture(3,NULL); break;
		}
	}	
	else
	{
		gpDev->SetTexture(0,NULL);
		gpDev->SetTexture(1,NULL);
		gpDev->SetTexture(2,NULL);
		gpDev->SetTexture(3,NULL);
	}

	if (_XRenderStyleManager::m_UseVertexShader == FALSE)
	{
		switch (m_rs.ts1_uvcoordindex) 
		{
		case _XRS_UVFROM_MODELDATA_UVSET1		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0); break; 
		case _XRS_UVFROM_MODELDATA_UVSET2		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1); break;
		case _XRS_UVFROM_MODELDATA_UVSET3		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2); break;
		case _XRS_UVFROM_MODELDATA_UVSET4		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3); break; 
		case _XRS_UVFROM_CAMERASPACENORMAL		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 0); break;
		case _XRS_UVFROM_CAMERASPACEPOSITION	:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 0); break;
		case _XRS_UVFROM_CAMERASPACEREFLTVECT	:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 0); break; 
		case _XRS_UVFROM_WORLDSPACENORMAL		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 0);	break;
		case _XRS_UVFROM_WORLDSPACEPOSITION		:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 0); break;
		case _XRS_UVFROM_WORLDSPACEREFLTVECT	:	gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 0); break; 
		}

		switch (m_rs.ts2_uvcoordindex) 
		{
		case _XRS_UVFROM_MODELDATA_UVSET1		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0); break; 
		case _XRS_UVFROM_MODELDATA_UVSET2		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1); break;
		case _XRS_UVFROM_MODELDATA_UVSET3		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2); break;
		case _XRS_UVFROM_MODELDATA_UVSET4		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3); break; 
		case _XRS_UVFROM_CAMERASPACENORMAL		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 1); break;
		case _XRS_UVFROM_CAMERASPACEPOSITION	:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 1); break;
		case _XRS_UVFROM_CAMERASPACEREFLTVECT	:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 1); break; 
		case _XRS_UVFROM_WORLDSPACENORMAL		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 1);	break;
		case _XRS_UVFROM_WORLDSPACEPOSITION		:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 1); break;
		case _XRS_UVFROM_WORLDSPACEREFLTVECT	:	gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 1); break; 
		}

		switch (m_rs.ts3_uvcoordindex) 
		{
		case _XRS_UVFROM_MODELDATA_UVSET1		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0); break; 
		case _XRS_UVFROM_MODELDATA_UVSET2		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1); break;
		case _XRS_UVFROM_MODELDATA_UVSET3		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2); break;
		case _XRS_UVFROM_MODELDATA_UVSET4		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3); break; 
		case _XRS_UVFROM_CAMERASPACENORMAL		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 2); break;
		case _XRS_UVFROM_CAMERASPACEPOSITION	:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 2); break;
		case _XRS_UVFROM_CAMERASPACEREFLTVECT	:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 2); break; 
		case _XRS_UVFROM_WORLDSPACENORMAL		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 2);	break;
		case _XRS_UVFROM_WORLDSPACEPOSITION		:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 2); break;
		case _XRS_UVFROM_WORLDSPACEREFLTVECT	:	gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 2); break; 
		}

		switch (m_rs.ts4_uvcoordindex) 
		{
		case _XRS_UVFROM_MODELDATA_UVSET1		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0); break; 
		case _XRS_UVFROM_MODELDATA_UVSET2		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1); break;
		case _XRS_UVFROM_MODELDATA_UVSET3		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2); break;
		case _XRS_UVFROM_MODELDATA_UVSET4		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3); break; 
		case _XRS_UVFROM_CAMERASPACENORMAL		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 3); break;
		case _XRS_UVFROM_CAMERASPACEPOSITION	:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 3); break;
		case _XRS_UVFROM_CAMERASPACEREFLTVECT	:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 3); break; 
		case _XRS_UVFROM_WORLDSPACENORMAL		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL | 3);	break;
		case _XRS_UVFROM_WORLDSPACEPOSITION		:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 3); break;
		case _XRS_UVFROM_WORLDSPACEREFLTVECT	:	gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 3); break; 
		}
	}
	else
	{
		gpDev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 0);
		gpDev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1);
		gpDev->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 2);
		gpDev->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 3);
	}

	gpDev->SetSamplerState(0,D3DSAMP_ADDRESSU,m_rs.ts1_ucoordadress);
	gpDev->SetSamplerState(1,D3DSAMP_ADDRESSU,m_rs.ts2_ucoordadress);
	gpDev->SetSamplerState(2,D3DSAMP_ADDRESSU,m_rs.ts3_ucoordadress);
	gpDev->SetSamplerState(3,D3DSAMP_ADDRESSU,m_rs.ts4_ucoordadress);

	gpDev->SetSamplerState(0,D3DSAMP_ADDRESSV,m_rs.ts1_vcoordadress);
	gpDev->SetSamplerState(1,D3DSAMP_ADDRESSV,m_rs.ts2_vcoordadress);
	gpDev->SetSamplerState(2,D3DSAMP_ADDRESSV,m_rs.ts3_vcoordadress);
	gpDev->SetSamplerState(3,D3DSAMP_ADDRESSV,m_rs.ts4_vcoordadress);
	
	if( m_rs.useuvmatrix1 )
	{
		UINT TTF = m_rs.ts1_texturecoordcount;
		if( m_rs.ts1_projected )		
			if( g_d3dCaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED ) TTF |= D3DTTFF_PROJECTED;		
		
		gpDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, TTF);

		if( m_uveffect[0].useextexduveffect > _XRS_UVEFFECTYPE_DISABLE )
		{
			D3DXMATRIX	modulatematrix;
			D3DXMATRIX	rotation;
			D3DXMATRIX	translation2;
			
			modulatematrix = m_rs.uvmatrix[0];
			
			D3DXMatrixIdentity( &translation2 );
			translation2._31 = -m_rs.uvmatrix[0]._31;
			translation2._32 = -m_rs.uvmatrix[0]._32;
			
			if( m_uveffect[0].userotationeffect )
			{
				if( m_uveffect[0].yawrotationvelocity != 0.0f )
				{
					m_yawrotationfactor[0] += (m_uveffect[0].yawrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_yawrotationfactor[0] >= 6.283185 ) m_yawrotationfactor[0] = 0.0f;
					if( m_yawrotationfactor[0] <= -6.283185 ) m_yawrotationfactor[0] = 0.0f;
				}
				
				if( m_uveffect[0].pitchrotationvelocity != 0.0f )
				{
					m_pitchrotationfactor[0] += (m_uveffect[0].pitchrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_pitchrotationfactor[0] >= 6.283185 ) m_pitchrotationfactor[0] = 0.0f;
					if( m_pitchrotationfactor[0] <= -6.283185 ) m_pitchrotationfactor[0] = 0.0f;
				}
				
				if( m_uveffect[0].rollrotationvelocity != 0.0f )
				{
					m_rollrotationfactor[0] += (m_uveffect[0].rollrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_rollrotationfactor[0] >= 6.283185 ) m_rollrotationfactor[0] = 0.0f;
					if( m_rollrotationfactor[0] <= -6.283185 ) m_rollrotationfactor[0] = 0.0f;
				}
				D3DXMatrixRotationYawPitchRoll( &rotation, m_yawrotationfactor[0],  m_pitchrotationfactor[0], m_rollrotationfactor[0] );				
				
				modulatematrix = m_rs.uvmatrix[0] * rotation;
			}
			
			modulatematrix = modulatematrix * translation2;
			
			if( m_uveffect[0].upanning != 0.0f || m_uveffect[0].vpanning != 0.0f )
			{
				m_upanningfactor[0] += (m_uveffect[0].upanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[0].upanning;
				m_vpanningfactor[0] += (m_uveffect[0].vpanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[0].vpanning;
				
				if( m_uveffect[0].useextexduveffect == _XRS_UVEFFECTYPE_PANNING )
				{
					translation2._31 += m_upanningfactor[0];
					translation2._32 += m_vpanningfactor[0];
				}
				else //if( m_uveffect[0].useextexduveffect == _XRS_UVEFFECTYPE_OSCILLATION )
				{
					translation2._31 = sin( m_upanningfactor[0] );
					translation2._32 = sin( m_vpanningfactor[0] );
				}
				
				modulatematrix = modulatematrix * translation2;
			}
			
			gpDev->SetTransform(D3DTS_TEXTURE0, &modulatematrix );
		}
		else
		{
			gpDev->SetTransform(D3DTS_TEXTURE0, &m_rs.uvmatrix[0] );
		}
	}
	else gpDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	if( m_rs.useuvmatrix2 )
	{				
		UINT TTF = m_rs.ts2_texturecoordcount;
		if( m_rs.ts2_projected ) 
			if( g_d3dCaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED ) TTF |= D3DTTFF_PROJECTED;		

		gpDev->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, TTF);

		if( m_uveffect[1].useextexduveffect > _XRS_UVEFFECTYPE_DISABLE )
		{
			D3DXMATRIX	modulatematrix;
			D3DXMATRIX	rotation;
			D3DXMATRIX	translation2;
			
			modulatematrix = m_rs.uvmatrix[1];
			
			D3DXMatrixIdentity( &translation2 );
			translation2._31 = -m_rs.uvmatrix[1]._31;
			translation2._32 = -m_rs.uvmatrix[1]._32;
			
			if( m_uveffect[1].userotationeffect )
			{
				if( m_uveffect[1].yawrotationvelocity != 0.0f )
				{
					m_yawrotationfactor[1] += (m_uveffect[1].yawrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_yawrotationfactor[1] >= 6.283185 ) m_yawrotationfactor[1] = 0.0f;
					if( m_yawrotationfactor[1] <= -6.283185 ) m_yawrotationfactor[1] = 0.0f;
				}
				
				if( m_uveffect[1].pitchrotationvelocity != 0.0f )
				{
					m_pitchrotationfactor[1] += (m_uveffect[1].pitchrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_pitchrotationfactor[1] >= 6.283185 ) m_pitchrotationfactor[1] = 0.0f;
					if( m_pitchrotationfactor[1] <= -6.283185 ) m_pitchrotationfactor[1] = 0.0f;
				}
				
				if( m_uveffect[1].rollrotationvelocity != 0.0f )
				{
					m_rollrotationfactor[1] += (m_uveffect[1].rollrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_rollrotationfactor[1] >= 6.283185 ) m_rollrotationfactor[1] = 0.0f;
					if( m_rollrotationfactor[1] <= -6.283185 ) m_rollrotationfactor[1] = 0.0f;
				}
				D3DXMatrixRotationYawPitchRoll( &rotation, m_yawrotationfactor[1],  m_pitchrotationfactor[1], m_rollrotationfactor[1] );				
				
				modulatematrix = m_rs.uvmatrix[1] * rotation;
			}
			
			modulatematrix = modulatematrix * translation2;
			
			if( m_uveffect[1].upanning != 0.0f || m_uveffect[1].vpanning != 0.0f )
			{
				m_upanningfactor[1] += (m_uveffect[1].upanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[1].upanning;
				m_vpanningfactor[1] += (m_uveffect[1].vpanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[1].vpanning;
				
				if( m_uveffect[1].useextexduveffect == _XRS_UVEFFECTYPE_PANNING )
				{
					translation2._31 += m_upanningfactor[1];
					translation2._32 += m_vpanningfactor[1];
				}
				else //if( m_uveffect[1].useextexduveffect == _XRS_UVEFFECTYPE_OSCILLATION )
				{
					translation2._31 = sin( m_upanningfactor[1] );
					translation2._32 = sin( m_vpanningfactor[1] );
				}
				
				modulatematrix = modulatematrix * translation2;
			}
			
			gpDev->SetTransform(D3DTS_TEXTURE1, &modulatematrix );
		}
		else
		{
			gpDev->SetTransform(D3DTS_TEXTURE1, &m_rs.uvmatrix[1] );
		}
	}
	else gpDev->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		
	if( m_rs.useuvmatrix3 )
	{				
		UINT TTF = m_rs.ts3_texturecoordcount;
		if( m_rs.ts3_projected ) 
			if( g_d3dCaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED ) TTF |= D3DTTFF_PROJECTED;		

		gpDev->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, TTF);

		if( m_uveffect[2].useextexduveffect > _XRS_UVEFFECTYPE_DISABLE  )
		{
			D3DXMATRIX	modulatematrix;
			D3DXMATRIX	rotation;
			D3DXMATRIX	translation2;
			
			modulatematrix = m_rs.uvmatrix[2];
			
			D3DXMatrixIdentity( &translation2 );
			translation2._31 = -m_rs.uvmatrix[2]._31;
			translation2._32 = -m_rs.uvmatrix[2]._32;
			
			if( m_uveffect[2].userotationeffect )
			{
				if( m_uveffect[2].yawrotationvelocity != 0.0f )
				{
					m_yawrotationfactor[2] += (m_uveffect[2].yawrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_yawrotationfactor[2] >= 6.283185 ) m_yawrotationfactor[2] = 0.0f;
					if( m_yawrotationfactor[2] <= -6.283185 ) m_yawrotationfactor[2] = 0.0f;
				}
				
				if( m_uveffect[2].pitchrotationvelocity != 0.0f )
				{
					m_pitchrotationfactor[2] += (m_uveffect[2].pitchrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_pitchrotationfactor[2] >= 6.283185 ) m_pitchrotationfactor[2] = 0.0f;
					if( m_pitchrotationfactor[2] <= -6.283185 ) m_pitchrotationfactor[2] = 0.0f;
				}
				
				if( m_uveffect[2].rollrotationvelocity != 0.0f )
				{
					m_rollrotationfactor[2] += (m_uveffect[2].rollrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_rollrotationfactor[2] >= 6.283185 ) m_rollrotationfactor[2] = 0.0f;
					if( m_rollrotationfactor[2] <= -6.283185 ) m_rollrotationfactor[2] = 0.0f;
				}
				D3DXMatrixRotationYawPitchRoll( &rotation, m_yawrotationfactor[2],  m_pitchrotationfactor[2], m_rollrotationfactor[2] );				
				
				modulatematrix = m_rs.uvmatrix[2] * rotation;
			}
			
			modulatematrix = modulatematrix * translation2;
			
			if( m_uveffect[2].upanning != 0.0f || m_uveffect[2].vpanning != 0.0f )
			{
				m_upanningfactor[2] += (m_uveffect[2].upanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[2].upanning;
				m_vpanningfactor[2] += (m_uveffect[2].vpanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[2].vpanning;
				
				if( m_uveffect[2].useextexduveffect == _XRS_UVEFFECTYPE_PANNING )
				{
					translation2._31 += m_upanningfactor[2];
					translation2._32 += m_vpanningfactor[2];
				}
				else //if( m_uveffect[2].useextexduveffect == _XRS_UVEFFECTYPE_OSCILLATION )
				{
					translation2._31 = sin( m_upanningfactor[2] );
					translation2._32 = sin( m_vpanningfactor[2] );
				}
				
				modulatematrix = modulatematrix * translation2;
			}
			
			gpDev->SetTransform(D3DTS_TEXTURE2, &modulatematrix );
		}
		else
		{
			gpDev->SetTransform(D3DTS_TEXTURE2, &m_rs.uvmatrix[2] );
		}
	}
	else gpDev->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	if( m_rs.useuvmatrix4 )
	{		
		UINT TTF = m_rs.ts4_texturecoordcount;
		if( m_rs.ts4_projected ) 
			if( g_d3dCaps.TextureCaps & D3DPTEXTURECAPS_PROJECTED ) TTF |= D3DTTFF_PROJECTED;		

		gpDev->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, TTF);		

		if( m_uveffect[3].useextexduveffect > _XRS_UVEFFECTYPE_DISABLE  )
		{
			D3DXMATRIX	modulatematrix;
			D3DXMATRIX	rotation;
			D3DXMATRIX	translation2;
			
			modulatematrix = m_rs.uvmatrix[3];
			
			D3DXMatrixIdentity( &translation2 );
			translation2._31 = -m_rs.uvmatrix[3]._31;
			translation2._32 = -m_rs.uvmatrix[3]._32;
			
			if( m_uveffect[3].userotationeffect )
			{
				if( m_uveffect[3].yawrotationvelocity != 0.0f )
				{
					m_yawrotationfactor[3] += (m_uveffect[3].yawrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_yawrotationfactor[3] >= 6.283185 ) m_yawrotationfactor[3] = 0.0f;
					if( m_yawrotationfactor[3] <= -6.283185 ) m_yawrotationfactor[3] = 0.0f;
				}
				
				if( m_uveffect[3].pitchrotationvelocity != 0.0f )
				{
					m_pitchrotationfactor[3] += (m_uveffect[3].pitchrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_pitchrotationfactor[3] >= 6.283185 ) m_pitchrotationfactor[3] = 0.0f;
					if( m_pitchrotationfactor[3] <= -6.283185 ) m_pitchrotationfactor[3] = 0.0f;
				}
				
				if( m_uveffect[3].rollrotationvelocity != 0.0f )
				{
					m_rollrotationfactor[3] += (m_uveffect[3].rollrotationvelocity * g_fElapsedFrameMilisecondTime);
					if( m_rollrotationfactor[3] >= 6.283185 ) m_rollrotationfactor[3] = 0.0f;
					if( m_rollrotationfactor[3] <= -6.283185 ) m_rollrotationfactor[3] = 0.0f;
				}
				D3DXMatrixRotationYawPitchRoll( &rotation, m_yawrotationfactor[3],  m_pitchrotationfactor[3], m_rollrotationfactor[3] );				
				
				modulatematrix = m_rs.uvmatrix[3] * rotation;
			}
			
			modulatematrix = modulatematrix * translation2;
			
			if( m_uveffect[3].upanning != 0.0f || m_uveffect[3].vpanning != 0.0f )
			{
				m_upanningfactor[3] += (m_uveffect[3].upanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[3].upanning;
				m_vpanningfactor[3] += (m_uveffect[3].vpanningvelocity * g_fElapsedFrameMilisecondTime) * m_uveffect[3].vpanning;
				
				if( m_uveffect[3].useextexduveffect == _XRS_UVEFFECTYPE_PANNING )
				{
					translation2._31 += m_upanningfactor[3];
					translation2._32 += m_vpanningfactor[3];
				}
				else //if( m_uveffect[3].useextexduveffect == _XRS_UVEFFECTYPE_OSCILLATION )
				{
					translation2._31 = sin( m_upanningfactor[3] );
					translation2._32 = sin( m_vpanningfactor[3] );
				}
				
				modulatematrix = modulatematrix * translation2;
			}
			
			gpDev->SetTransform(D3DTS_TEXTURE3, &modulatematrix );
		}
		else
		{
			gpDev->SetTransform(D3DTS_TEXTURE3, &m_rs.uvmatrix[3] );
		}
	}
	else gpDev->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);		

	if( g_d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP )
	{
		if( m_rs.usetransformbumpenvmapping )
		{
			SetBumpEnvMapProperties( m_rs.bumpenvmappingstage );
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVLSCALE,  FtoDW(m_rs.bumpmapscale)  );
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVLOFFSET, FtoDW(m_rs.bumpmapoffset) );
		}
		else
		{
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVMAT00, 0);
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVMAT01, 0);
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVMAT10, 0);
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVMAT11, 0); 
			
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVLSCALE,  0 );
			gpDev->SetTextureStageState(m_rs.bumpenvmappingstage, D3DTSS_BUMPENVLOFFSET, 0 );
		}
	}
}

void _XRenderStyleItem::SetBumpEnvMapProperties( DWORD BumpEnvMapStage )
{
	D3DVIEWPORT9 vp; 
	gpDev->GetViewport(&vp);
	
	D3DXMATRIX VS;
	ZeroMemory(&VS, sizeof(D3DMATRIX));
	
	FLOAT HALFWIDTH  = (float)(vp.Width>>1);
	FLOAT HALFHIEHGT = (float)(vp.Height>>1);
	
	VS._11	=  HALFWIDTH;
	VS._22	= -HALFHIEHGT;
	VS._33	= (float)(vp.MaxZ - vp.MinZ);
	VS._41	= (float)(vp.X) + HALFWIDTH;
	VS._42	= (float)(vp.Y) + HALFHIEHGT;
	VS._43	= (float)vp.MinZ; 
	VS._44	= 1.0f;
	
	D3DXMATRIX mat, mat2, mat3;							
	D3DXMatrixMultiply(&mat,  &g_ProjectionMatrix,  &VS);
	D3DXMatrixMultiply(&mat2, &g_ViewMatrix,  &mat);
	D3DXMatrixMultiply(&mat3, &g_UserWorldMatrix, &mat2);
	
	D3DXVECTOR3 v(0.0f,0.0f,0.0f);
	D3DXVECTOR4 res; D3DXVec3Transform(&res, &v, &mat3);
	D3DXVECTOR3 Screenv0, Screenv1, Screenu0, Screenu1;
	Screenv0.x = Screenu0.x = res.x; Screenv0.y = Screenu0.y = res.y; Screenv0.z = Screenu0.z = res.z; 
	
	v.x = 1.0f; v.y = 0.0f; v.z = 0.0f;
	D3DXVec3Transform(&res, &v, &mat3);
	Screenu1.x = res.x; Screenu1.y = res.y; Screenu1.z = res.z; 
	
	v.x = 0.0f; v.y = 0.0f; v.z = 1.0f;
	D3DXVec3Transform(&res, &v, &mat3);
	Screenv1.x = res.x; Screenv1.y = res.y; Screenv1.z = res.z; 
	
	float dy = Screenu1.y - Screenu0.y;
	float dx = Screenu1.x - Screenu0.x;
	float h = (float)_XFC_sqrt(dy*dy + dx*dx);
	float costhetau = dx/h;
	float sinthetau = dy/h;
	
	gpDev->SetTextureStageState(BumpEnvMapStage, D3DTSS_BUMPENVMAT00, FtoDW(0.25f * costhetau));
	gpDev->SetTextureStageState(BumpEnvMapStage, D3DTSS_BUMPENVMAT01, FtoDW(0.25f * (-sinthetau)));
	gpDev->SetTextureStageState(BumpEnvMapStage, D3DTSS_BUMPENVMAT10, FtoDW(0.25f * sinthetau));
	gpDev->SetTextureStageState(BumpEnvMapStage, D3DTSS_BUMPENVMAT11, FtoDW(0.25f * costhetau)); 	
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
_XRenderStyleManager::_XRenderStyleManager()
{
	m_RenderStyleCount = 0;
	m_RenderStyleList  = NULL;		
}

_XRenderStyleManager::~_XRenderStyleManager()
{
	SAFE_DELETE_ARRAY( m_RenderStyleList );	
}

void _XRenderStyleManager::Dispose( void )
{
	m_RenderStyleCount = 0;
	SAFE_DELETE_ARRAY( m_RenderStyleList );	
}

BOOL _XRenderStyleManager::Load( LPTSTR Filename )
{
	if( !Filename ) return FALSE;
	
	FILE* fp = NULL;
	fp = fopen( Filename, "rb" );
	if( !fp ) 
	{
		_XFatalError( "Can't open %s file.", Filename );
		return  FALSE;
	}
	
	if( !Load( fp ) )
	{
		fclose( fp );
		return FALSE;
	}
	
	fclose( fp );
	
	return TRUE;
}

BOOL _XRenderStyleManager::Load( FILE* Fileptr )
{
	if( !Fileptr ) return FALSE;
	
	WORD header = 0;	
	fread( &header, sizeof(WORD), 1, Fileptr );
	
	if( header != _XDEF_HEADER_RENDERSTYLE )
	{
		_XFatalError( "Can't support this render style package" );
		return FALSE;
	}
	
	m_RenderStyleCount = 0;
	SAFE_DELETE_ARRAY( m_RenderStyleList );
	
	int renderstylecount = 0;
	
	fread( &renderstylecount, sizeof(int), 1, Fileptr );
	
	if( renderstylecount <= 0 )
	{
		return TRUE;
	}
	
	m_RenderStyleCount = renderstylecount;
	m_RenderStyleList = new _XRenderStyleItem[ renderstylecount ];
	
	int i = 0 ; for(  i = 0; i < renderstylecount; i++ )
	{		
		fread( &m_RenderStyleList[i].m_rsname, sizeof(TCHAR) * 32, 1, Fileptr );
		fread( &m_RenderStyleList[i].m_uveffect, sizeof(_XRS_UVEFFECT) * 4, 1, Fileptr );
		fread( &m_RenderStyleList[i].m_rs, sizeof(_XRenderStyleStructure), 1, Fileptr );

		/*
		m_RenderStyleList[i].m_rs.material.Diffuse.r = 0.8f;
		m_RenderStyleList[i].m_rs.material.Diffuse.g = 0.8f;
		m_RenderStyleList[i].m_rs.material.Diffuse.b = 0.8f;
		m_RenderStyleList[i].m_rs.material.Diffuse.a = 1.0f;

		m_RenderStyleList[i].m_rs.material.Ambient.r = 0.752941f;
		m_RenderStyleList[i].m_rs.material.Ambient.g = 0.752941f;
		m_RenderStyleList[i].m_rs.material.Ambient.b = 0.752941f;
		m_RenderStyleList[i].m_rs.material.Ambient.a = 0.752941f;

		m_RenderStyleList[i].m_rs.material.Specular.r = 0.0f;
		m_RenderStyleList[i].m_rs.material.Specular.g = 0.0f;
		m_RenderStyleList[i].m_rs.material.Specular.b = 0.0f;
		m_RenderStyleList[i].m_rs.material.Specular.a = 1.0f;

		m_RenderStyleList[i].m_rs.material.Emissive.r = 0.0f;
		m_RenderStyleList[i].m_rs.material.Emissive.g = 0.0f;
		m_RenderStyleList[i].m_rs.material.Emissive.b = 0.0f;
		m_RenderStyleList[i].m_rs.material.Emissive.a = 1.0f;

		m_RenderStyleList[i].m_rs.material.Power = 5.0f;
		*/
	}
	
	return TRUE;
}

BOOL _XRenderStyleManager::Reload( LPTSTR Filename )
{
	if( !Filename ) return FALSE;
	
	FILE* fp = NULL;
	fp = fopen( Filename, "rb" );
	if( !fp ) return  FALSE;
	
	WORD header = 0;	
	fread( &header, sizeof(WORD), 1, fp );
	
	if( header != _XDEF_HEADER_RENDERSTYLE )
	{
		fclose( fp );
		return FALSE;
	}
			
	int renderstylecount = 0;
	
	fread( &renderstylecount, sizeof(int), 1, fp );
	
	if( renderstylecount <= 0 )
	{
		fclose( fp );
		return TRUE;
	}
	
	m_RenderStyleCount = renderstylecount;
	if( !m_RenderStyleList )
		m_RenderStyleList = new _XRenderStyleItem[ renderstylecount ];
	
	int i = 0 ; for(  i = 0; i < renderstylecount; i++ )
	{		
		fread( &m_RenderStyleList[i].m_rsname, sizeof(TCHAR) * 32, 1, fp );
		fread( &m_RenderStyleList[i].m_rs, sizeof(_XRenderStyleStructure), 1, fp );
	}
	
	fclose( fp );
	
	return TRUE;
}

void _XRenderStyleManager::SetRenderStyle( _XTextureManager* pTextureManager, int index )
{
	if( !m_RenderStyleList || index >= m_RenderStyleCount ) return;
	m_RenderStyleList[index].SetRenderStyle( pTextureManager );		
} 

void _XRenderStyleManager::SetDefaultRenderState( void )
{	
	// Basic Render State defaults...
	if(g_bCanDoTableFog)
		gpDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	else
		gpDev->SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_NONE );

	gpDev->SetRenderState(D3DRS_FOGENABLE, FALSE); 	
	gpDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	gpDev->SetRenderState(D3DRS_ZENABLE, g_RS_UseZBuffer);
	gpDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	gpDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	gpDev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	gpDev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	gpDev->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	gpDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	gpDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	gpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	gpDev->SetRenderState(D3DRS_COLORVERTEX, FALSE);
	gpDev->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
	gpDev->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
	
	gpDev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	
	// Disable Clipping..
	gpDev->SetRenderState(D3DRS_CLIPPING, TRUE);
	
	// Setup Material (for D3D lighting)...
	gpDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	gpDev->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE,D3DMCS_MATERIAL);
	gpDev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL);
	gpDev->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL);
	gpDev->SetRenderState(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL);
	
	// Texture Stage States...
	gpDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);	
	gpDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);	
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	gpDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	gpDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	gpDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	gpDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	gpDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gpDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	gpDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	gpDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	gpDev->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	
	gpDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gpDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	gpDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);	
	gpDev->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gpDev->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	gpDev->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		
	// Clear the Textures...
	gpDev->SetTexture(0,NULL);
	gpDev->SetTexture(1,NULL);
	
	// Setup the Material...This should be set on the per piece basis, but for the fall release I'm just setting it the once and moving on...
	D3DMATERIAL9 Material;
	
	D3DCOLORVALUE MatColor1; MatColor1.r = 1.0f; MatColor1.g = 1.0f; MatColor1.b = 1.0f; MatColor1.a = 1.0f;
	D3DCOLORVALUE MatColor2; MatColor2.r = 0.0f; MatColor2.g = 0.0f; MatColor2.b = 0.0f; MatColor2.a = 0.0f;
	D3DCOLORVALUE MatColor3; MatColor3.r = 0.5f; MatColor3.g = 0.05; MatColor3.b = 0.5f; MatColor3.a = 1.0f;

	Material.Diffuse  = MatColor1; Material.Ambient  = MatColor1;
	Material.Emissive = MatColor2; Material.Power = 0.0f;
	Material.Specular = MatColor3; 
	Material.Power = 1.0f;
	
	gpDev->SetMaterial(&Material);	
}

int _XRenderStyleManager::FindRenderStyle( LPCTSTR texturename )
{	
	if( !m_RenderStyleList ) return -1;

	int i = 0 ; for(  i = 0; i < m_RenderStyleCount; i++ )
	{		
		if( strcmpi( m_RenderStyleList[i].m_rsname, texturename ) == 0 )
		{
			return i;
		}
	}

	return -1;
}