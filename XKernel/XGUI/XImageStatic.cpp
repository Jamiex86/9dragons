// XImageStatic.cpp: implementation of the _XImageStatic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XKernel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_XImageStatic::_XImageStatic()
{
	m_WindowRect.left		=	0;
	m_WindowRect.top		=	0;
	m_WindowRect.right		=	0;
	m_WindowRect.bottom		=	0;
	m_WindowSize.cx			=	0;
	m_WindowSize.cy			=	0;
	m_TextureIndex			=	-1;
	m_pTextureArchive		=   NULL;
	m_TooltipCallbackFunc	=	NULL;

	m_Scale.x				=	1.0f;
	m_Scale.y				=	1.0f;

	m_Position.x			=	0.0f;
	m_Position.y			=	0.0f;
	m_WindowPosition.x		=	0;
	m_WindowPosition.y		=	0;

	m_Center.x				=	0.0f;
	m_Center.y				=	0.0f;
	
	m_RotateAngle			=   0.0f;
	
	m_ClipRect.left			=	0;
	m_ClipRect.top			=	0;
	m_ClipRect.right		=	0;
	m_ClipRect.bottom		=	0;

	m_FColor				=   D3DCOLOR_ARGB( 255, 255,255,255 );

	m_Enable				=	TRUE;	
}

_XImageStatic::~_XImageStatic()
{
	DestroyWindow();
}

void _XImageStatic::DestroyWindow( void )
{

}

HRESULT _XImageStatic::Create( int left, int top, int right, int bottom, _XTextureManager* ptexturemanager, int textureindex )
{	
	m_WindowRect.left		=	left;
	m_WindowRect.top		=	top;
	m_WindowRect.right		=	right;
	m_WindowRect.bottom		=	bottom;
	m_TextureIndex			=	textureindex;
	m_pTextureArchive		=   ptexturemanager;

	m_Position.x			=	left;
	m_Position.y			=	top;
	m_WindowPosition.x		=	left;
	m_WindowPosition.y		=	top;

	m_WindowSize.cx			=	right-left;
	m_WindowSize.cy			=	bottom-top;

	m_ClipRect.left			=	0;
	m_ClipRect.top			=	0;
	m_ClipRect.right		=	m_WindowSize.cx;
	m_ClipRect.bottom		=	m_WindowSize.cy;

	if( ptexturemanager )
	{
		_XTexture* texture = ptexturemanager->GetTextureObject( textureindex );
		if( texture )
		{
			if( texture->m_width )
				m_ClipRect.right		=	texture->m_width;

			if( texture->m_height )
				m_ClipRect.bottom		=	texture->m_height;
		}
	}

	return S_OK;
}

void _XImageStatic::Draw( _XGUIObject*& pfocusobject )
{	
	if( !this->m_ShowWindow || !g_pd3dxSprite ) return;
	if( m_ClipRect.right <= m_ClipRect.left ) return;
	if( m_ClipRect.bottom<= m_ClipRect.top  ) return;
	if( m_pTextureArchive )
	{	
		if( m_TextureIndex >= 0 )
		{
			//if( !this->m_Enable ){		
			//	g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ), &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &m_Position, 0x7FFFFFFF);			
			//}
			//else{
			//	g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ), &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &m_Position, m_FColor);
			//}
			D3DXMATRIX mat;
			D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &m_Scale, &m_Center, m_RotateAngle, &m_Position);
			g_pd3dxSprite->SetTransform(&mat);
			g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ),&m_ClipRect,NULL,NULL, this->m_Enable ? m_FColor : 0x7FFFFFFF);
		}
	}		
} 

void _XImageStatic::Draw( int X, int Y )
{	
	if( !this->m_ShowWindow || !g_pd3dxSprite ) return;
	if( m_ClipRect.right <= m_ClipRect.left ) return;
	if( m_ClipRect.bottom<= m_ClipRect.top  ) return;

	D3DXVECTOR2 position( X,Y );
	if( m_pTextureArchive  )
	{		
		if( m_TextureIndex >= 0 )
		{
			//if( !this->m_Enable )				
			//	g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ), &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &position, 0x7FFFFFFF);			
			//else 
			//	g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ), &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &position, m_FColor);
			D3DXMATRIX mat;
			D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &m_Scale, &m_Center, m_RotateAngle, &m_Position);
			g_pd3dxSprite->SetTransform(&mat);
			g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ),&m_ClipRect,NULL,NULL, this->m_Enable ? m_FColor : 0x7FFFFFFF);
		}
	}
}

void _XImageStatic::Draw( int X, int Y, LPDIRECT3DTEXTURE9	pTexture )
{
	if( !this->m_ShowWindow || !g_pd3dxSprite ) return;
	if( m_ClipRect.right <= m_ClipRect.left ) return;
	if( m_ClipRect.bottom<= m_ClipRect.top  ) return;
	
	D3DXVECTOR2 position( X,Y );
	if( pTexture  )
	{		
		if( pTexture )
		{
			//if( !this->m_Enable )				
			//	g_pd3dxSprite->Draw(pTexture, &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &position, 0x7FFFFFFF);			
			//else 
			//	g_pd3dxSprite->Draw(pTexture, &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &position, m_FColor);\D3DXMATRIX mat;
			D3DXMATRIX mat;
			D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &m_Scale, &m_Center, m_RotateAngle, &m_Position);
			g_pd3dxSprite->SetTransform(&mat);
			g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ),&m_ClipRect,NULL,NULL, this->m_Enable ? m_FColor : 0x7FFFFFFF);
		}
	}
}

void _XImageStatic::DrawSolid( int X, int Y )
{
	if( !this->m_ShowWindow || !g_pd3dxSprite ) return;
		
	D3DXVECTOR2 position( X,Y );
	
	//g_pd3dxSprite->Draw(NULL, NULL, &m_Scale, &m_Center, m_RotateAngle, &position, m_FColor);	
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &m_Scale, &m_Center, m_RotateAngle, &m_Position);
	g_pd3dxSprite->SetTransform(&mat);
	g_pd3dxSprite->Draw( NULL ,NULL,NULL,NULL, m_FColor);
}


void _XImageStatic::DrawWithRegion( void )
{
	if( !this->m_ShowWindow || !g_pd3dxSprite ) return;
	if( m_ClipRect.right <= m_ClipRect.left ) return;
	if( m_ClipRect.bottom<= m_ClipRect.top  ) return;

	if( m_pTextureArchive  )
	{			
		if( m_TextureIndex >= 0 )
		{
			//if( !this->m_Enable )
			//{
			//	g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ), &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &m_Position, 0x7FFFFFFF);
			//}
			//else 
			//{
			//	g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ), &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &m_Position, m_FColor);
			//}
			D3DXMATRIX mat;
			D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &m_Scale, &m_Center, m_RotateAngle, &m_Position);
			g_pd3dxSprite->SetTransform(&mat);
			g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ),&m_ClipRect,NULL,NULL, this->m_Enable ? m_FColor : 0x7FFFFFFF);
		}
	}		
}

void _XImageStatic::DrawWithRegion(int X, int Y, RECT rect)
{
	if( !this->m_ShowWindow || !g_pd3dxSprite ) return;		
	if( rect.right <= rect.left ) return;
	if( rect.bottom<= rect.top  ) return;
	
	D3DXVECTOR2 position(X, Y);
	if( m_pTextureArchive )
	{	
		if( m_TextureIndex >= 0 )
		{
			//if(!this->m_Enable)				
			//	g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture(m_TextureIndex), &rect, &m_Scale, &m_Center, m_RotateAngle, &position, 0x7FFFFFFF);
			//else 
			//	g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture(m_TextureIndex), &rect, &m_Scale, &m_Center, m_RotateAngle, &position, m_FColor);
			D3DXMATRIX mat;
			D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &m_Scale, &m_Center, m_RotateAngle, &m_Position);
			g_pd3dxSprite->SetTransform(&mat);
			g_pd3dxSprite->Draw(m_pTextureArchive->GetTexture( m_TextureIndex ),&m_ClipRect,NULL,NULL, this->m_Enable ? m_FColor : 0x7FFFFFFF);
		}
	}		
}

void _XImageStatic::DrawWithRegion( LPDIRECT3DTEXTURE9	pTexture )
{
	if( !this->m_ShowWindow || !g_pd3dxSprite ) return;
	if( m_ClipRect.right <= m_ClipRect.left ) return;
	if( m_ClipRect.bottom<= m_ClipRect.top  ) return;
	
	if( pTexture  )
	{			
		//if( !this->m_Enable )
		//{
		//	g_pd3dxSprite->Draw(pTexture, &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &m_Position, 0x7FFFFFFF);
		//}
		//else 
		//{
		//	g_pd3dxSprite->Draw(pTexture, &m_ClipRect, &m_Scale, &m_Center, m_RotateAngle, &m_Position, m_FColor);
		//}
		D3DXMATRIX mat;
		D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &m_Scale, &m_Center, m_RotateAngle, &m_Position);
		g_pd3dxSprite->SetTransform(&mat);
		g_pd3dxSprite->Draw(pTexture ,&m_ClipRect,NULL,NULL, this->m_Enable ? m_FColor : 0x7FFFFFFF);
	}
}

BOOL _XImageStatic::Process( _XGUIObject*& pfocusobject )
{
	if( !this->m_ShowWindow ) return FALSE;
	if( !this->m_Enable     ) return FALSE;

	return TRUE;
}

BOOL _XImageStatic::CheckMousePosition( void )
{
	if( !this->m_Enable ) return FALSE;
	ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
	
	if( scrnpos )
	{	
		return TestAlphaChannel( scrnpos->x, scrnpos->z );
	}
	return FALSE;
}

void _XImageStatic::SetWindowSize( SIZE size )
{ 
	m_WindowSize = size; 

	m_WindowRect.right  = m_WindowRect.left + size.cx - 1;
	m_WindowRect.bottom = m_WindowRect.top  + size.cy - 1;
}

void _XImageStatic::SetWindowSize( int width, int height )
{ 
	m_WindowSize.cx = width; 
	m_WindowSize.cy = height; 

	m_WindowRect.right  = m_WindowRect.left + width - 1;
	m_WindowRect.bottom = m_WindowRect.top  + height - 1;
}

BOOL _XImageStatic::TestAlphaChannel( int x, int y )
{
	if( m_TextureIndex == -1 || !m_pTextureArchive ) return FALSE;
	
	if( (x>m_WindowRect.left) && (x<m_WindowRect.right ) && (y>m_WindowRect.top ) && (y<m_WindowRect.bottom) )
	{
		return m_pTextureArchive->TestAlphaChannel( m_TextureIndex, m_ClipRect.left + (x -m_WindowRect.left) , m_ClipRect.top + (y - m_WindowRect.top) );
	}
	return FALSE;	
}

void _XImageStatic::DrawToolTip(_XGUIObject*& pfocusobject)
{
	if( m_TooltipCallbackFunc && ( !pfocusobject || (pfocusobject != _XDEF_DUMMYID_MESSAGEBOX) ) )
	{
		ScrnPos* scrnpos = gpInput->GetMouseVirScreenPos();
		
		if( scrnpos )
		{				
			if( TestAlphaChannel( scrnpos->x, scrnpos->z ) )
			{
				m_TooltipCallbackFunc( m_WindowPosition, m_WindowSize );
			}
		}
	}
}

void _XImageStatic::MoveWindow( int X, int Y )
{
	FLOAT width  = m_WindowRect.right  - m_WindowRect.left;
	FLOAT height = m_WindowRect.bottom - m_WindowRect.top;
	m_WindowRect.left  = X;
	m_WindowRect.top   = Y;
	m_WindowRect.right = X + width;
	m_WindowRect.bottom= Y + height;

	m_Position.x	   = X;
	m_Position.y	   = Y;
	m_WindowPosition.x = X;
	m_WindowPosition.y = Y;

	m_WindowSize.cx	   =	width;
	m_WindowSize.cy	   =	height;
}

void _XImageStatic::MoveWindow( int left, int top, int right, int bottom )
{
	m_WindowRect.left		=	left;
	m_WindowRect.top		=	top;
	m_WindowRect.right		=	right;
	m_WindowRect.bottom		=	bottom;	
	m_Position.x			=	left;
	m_Position.y			=	top;
	m_WindowSize.cx			=	right-left;
	m_WindowSize.cy			=	bottom-top;
	m_WindowPosition.x		=	left;
	m_WindowPosition.y		=	top;
}

void _XImageStatic::SetScale( FLOAT xscale, FLOAT yscale )
{
	m_Scale.x = xscale;
	m_Scale.y = yscale;
}