
#include "stdafx.h"
#include "XKernel.h"

#define _XDEF_DEFAULTBILLBOARDWIDTH		16
#define _XDEF_DEFAULTBILLBOARDHEIGHT	16

_XBillboard::_XBillboard()
{
	SetBillBoard( _XDEF_DEFAULTBILLBOARDWIDTH, _XDEF_DEFAULTBILLBOARDHEIGHT, D3DXVECTOR3(0,64,0) );	
}

_XBillboard::_XBillboard(FLOAT Width,FLOAT Height,D3DXVECTOR3 pos)
{
	SetBillBoard( Width, Height, pos );
}

_XBillboard::_XBillboard(FLOAT Width,FLOAT Height,D3DXVECTOR3 pos,WORD NumOfFrames,WORD TextureSizeX,WORD TextureSizeY,WORD cols,WORD rows,WORD animspeed)
{
	WORD i,t;
	ZeroMemory(TextureX,sizeof(TextureX));
	ZeroMemory(TextureY,sizeof(TextureY));
	ZeroMemory(&Matrix,sizeof(D3DXMATRIX));

	Position=pos;
	CurrentFrame=0;
	NumberOfFrames=NumOfFrames;
	AnimationCounter=0;
	AnimationSpeed=1.0f/(FLOAT)animspeed;

	WORD counter=0;
	FLOAT TileSizeX=TextureSizeX/(FLOAT)cols;
	FLOAT TileSizeY=TextureSizeY/(FLOAT)rows ;

	// loop through and calculate texture coordinates
	for (i=0;i<rows;i++)
	{
		 for (t=0;t<cols;t++)
		 {
			  if (counter<NumberOfFrames)
			  {
  				  TextureX[counter][0]=TextureX[counter][2]=(TileSizeX*(FLOAT)t)/(FLOAT)TextureSizeX;
				  TextureX[counter][1]=TextureX[counter][3]=((TileSizeX*(FLOAT)t)+TileSizeX)/(FLOAT)TextureSizeX;
				  TextureY[counter][0]=TextureY[counter][1]=(TileSizeY*(FLOAT)i)/(FLOAT)TextureSizeY;
				  TextureY[counter][2]=TextureY[counter][3]=((TileSizeY*(FLOAT)i)+TileSizeY)/(FLOAT)TextureSizeY;
				  counter++;
			  }
		 }
	}

	VertexList[0]=_XBILLBOARD_VERTEX(Width,  Height,0,D3DCOLOR_XRGB(255,255,255),0,TextureX[0][0],TextureY[0][0]);
	VertexList[1]=_XBILLBOARD_VERTEX(-Width, Height,0,D3DCOLOR_XRGB(255,255,255),0,TextureX[0][1],TextureY[0][1]);
	VertexList[2]=_XBILLBOARD_VERTEX(Width, -Height,0,D3DCOLOR_XRGB(255,255,255),0,TextureX[0][2],TextureY[0][2]);
	VertexList[3]=_XBILLBOARD_VERTEX(-Width,-Height,0,D3DCOLOR_XRGB(255,255,255),0,TextureX[0][3],TextureY[0][3]);
}


void _XBillboard::SetBillBoard( FLOAT Width,FLOAT Height,D3DXVECTOR3 pos )
{
	AnimationSpeed=0.0;
	NumberOfFrames=0;
	CurrentFrame=0;
	ZeroMemory(TextureX,sizeof(TextureX));
	ZeroMemory(TextureY,sizeof(TextureY));
	ZeroMemory(&Matrix,sizeof(D3DXMATRIX));
	Position=pos;

	VertexList[0]=_XBILLBOARD_VERTEX( Width,  Height,0,D3DCOLOR_XRGB(255,255,255),0,0.0f,0.0f);
	VertexList[1]=_XBILLBOARD_VERTEX( -Width, Height,0,D3DCOLOR_XRGB(255,255,255),0,1.0f,0.0f);
	VertexList[2]=_XBILLBOARD_VERTEX( Width, -Height,0,D3DCOLOR_XRGB(255,255,255),0,0.0f,1.0f);
	VertexList[3]=_XBILLBOARD_VERTEX( -Width,-Height,0,D3DCOLOR_XRGB(255,255,255),0,1.0f,1.0f);
}

void _XBillboard::SetBillBoardXZ( FLOAT Width,FLOAT Height,D3DXVECTOR3 pos )
{
	AnimationSpeed=0.0;
	NumberOfFrames=0;
	CurrentFrame=0;
	ZeroMemory(TextureX,sizeof(TextureX));
	ZeroMemory(TextureY,sizeof(TextureY));
	ZeroMemory(&Matrix,sizeof(D3DXMATRIX));
	Position=pos;

	VertexList[0]=_XBILLBOARD_VERTEX( Width, pos.y,-Height,D3DCOLOR_ARGB(128,0,0,0),0,0.0f,1.0f);	
	VertexList[1]=_XBILLBOARD_VERTEX( -Width,pos.y,-Height,D3DCOLOR_ARGB(128,0,0,0),0,1.0f,1.0f);	
	VertexList[2]=_XBILLBOARD_VERTEX( Width, pos.y, Height,D3DCOLOR_ARGB(128,0,0,0),0,0.0f,0.0f);
	VertexList[3]=_XBILLBOARD_VERTEX( -Width,pos.y, Height,D3DCOLOR_ARGB(128,0,0,0),0,1.0f,0.0f);		
	
	
}

void _XBillboard::RenderCheap( double TimeElapsed )
{
	 AnimationCounter+=(FLOAT)TimeElapsed;

	 if	(AnimationCounter>AnimationSpeed)
	 {
		 CurrentFrame++;
		 AnimationCounter=0;
		 if (CurrentFrame==NumberOfFrames) CurrentFrame=0;

		 VertexList[0].tu=TextureX[CurrentFrame][0];
		 VertexList[1].tu=TextureX[CurrentFrame][1];
		 VertexList[2].tu=TextureX[CurrentFrame][2];
		 VertexList[3].tu=TextureX[CurrentFrame][3];

		 VertexList[0].tv=TextureY[CurrentFrame][0];
		 VertexList[1].tv=TextureY[CurrentFrame][1];
		 VertexList[2].tv=TextureY[CurrentFrame][2];
		 VertexList[3].tv=TextureY[CurrentFrame][3];
	}

	RenderCheap();
}



void _XBillboard::RenderCheap( void )
{
	D3DXMATRIX view;
	gpDev->GetTransform(D3DTS_VIEW,&view);

	Matrix._11 = -view._11; Matrix._21 = view._12; Matrix._31 = -view._13;
	Matrix._12 = -view._21; Matrix._22 = view._22; Matrix._32 = -view._23;
	Matrix._13 = -view._31; Matrix._23 = view._32; Matrix._33 = -view._33;

	Matrix._41=Position.x;
	Matrix._42=Position.y;
	Matrix._43=Position.z;

	Matrix._14=0.0f;
	Matrix._24=0.0f;
	Matrix._34=0.0f;
	Matrix._44=1.0f;		
	
	gpDev->SetTransform(D3DTS_WORLD,&Matrix);
	gpDev->SetFVF( D3DFVF_XBILLBOARD_VERTEX );	
	gpDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(_XBILLBOARD_VERTEX) );
}

void _XBillboard::Render(D3DXVECTOR3  ViewPosition, FLOAT zangle)
{
	D3DXVECTOR3 View = ViewPosition-Position;
	D3DXVec3Normalize(&View, &View);
  
	D3DXVECTOR3 WorldUp=D3DXVECTOR3(0.0,1.0,0.0);	

	FLOAT angle=D3DXVec3Dot(&WorldUp,&View);
	D3DXVECTOR3 Up=WorldUp-angle*View;
	D3DXVec3Normalize(&Up, &Up);

	D3DXVECTOR3 Right;
	D3DXVec3Cross(&Right, &Up, &View);
  
	Matrix._11 = Right.x; Matrix._21 = Up.x; Matrix._31 = View.x;
	Matrix._12 = Right.y; Matrix._22 = Up.y; Matrix._32 = View.y;
	Matrix._13 = Right.z; Matrix._23 = Up.z; Matrix._33 = View.z;

	Matrix._41=Position.x;
	Matrix._42=Position.y;
	Matrix._43=Position.z;

	Matrix._14=0.0f;
	Matrix._24=0.0f;
	Matrix._34=0.0f;
	Matrix._44=1.0f;

	D3DXMATRIX rotmat;	
	D3DXMatrixRotationZ( &rotmat, _X_RAD(zangle) );
	D3DXMatrixMultiply( &Matrix, &rotmat, &Matrix );

	gpDev->SetTransform(D3DTS_WORLD,&Matrix);
	gpDev->SetFVF( D3DFVF_XBILLBOARD_VERTEX );
	gpDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(_XBILLBOARD_VERTEX) );
}

void _XBillboard::Render( D3DXVECTOR3  ViewPosition )
{
	D3DXVECTOR3 View = ViewPosition-Position;
	D3DXVec3Normalize(&View, &View);
  
	D3DXVECTOR3 WorldUp=D3DXVECTOR3(0.0,1.0,0.0);	

	FLOAT angle=D3DXVec3Dot(&WorldUp,&View);
	D3DXVECTOR3 Up=WorldUp-angle*View;
	D3DXVec3Normalize(&Up, &Up);

	D3DXVECTOR3 Right;
	D3DXVec3Cross(&Right, &Up, &View);
  
	Matrix._11 = Right.x; Matrix._21 = Up.x; Matrix._31 = View.x;
	Matrix._12 = Right.y; Matrix._22 = Up.y; Matrix._32 = View.y;
	Matrix._13 = Right.z; Matrix._23 = Up.z; Matrix._33 = View.z;

	Matrix._41=Position.x;
	Matrix._42=Position.y;
	Matrix._43=Position.z;

	Matrix._14=0.0f;
	Matrix._24=0.0f;
	Matrix._34=0.0f;
	Matrix._44=1.0f;	

	gpDev->SetTransform(D3DTS_WORLD,&Matrix);
	gpDev->SetFVF( D3DFVF_XBILLBOARD_VERTEX );
	gpDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(_XBILLBOARD_VERTEX) );
}

void _XBillboard::Render( D3DXMATRIX  tm )
{
	gpDev->SetTransform(D3DTS_WORLD,&tm);
	gpDev->SetFVF( D3DFVF_XBILLBOARD_VERTEX );
	gpDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(_XBILLBOARD_VERTEX) );
}

void _XBillboard::RenderAlpha( D3DXMATRIX  tm )
{
	
	gpDev->SetRenderState( D3DRS_DEPTHBIAS, 15 );

	gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	gpDev->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	gpDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA  );	
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gpDev->SetRenderState( D3DRS_FOGENABLE, FALSE );	
	gpDev->SetRenderState( D3DRS_ZENABLE,   D3DZB_FALSE );
	
	gpDev->SetTransform(D3DTS_WORLD,&tm);
	gpDev->SetFVF( D3DFVF_XBILLBOARD_VERTEX );
	gpDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(_XBILLBOARD_VERTEX) );

	gpDev->SetRenderState( D3DRS_ZENABLE, g_RS_UseZBuffer );
	gpDev->SetRenderState( D3DRS_FOGENABLE, TRUE );
	gpDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	gpDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	gpDev->SetRenderState( D3DRS_DEPTHBIAS, 0 );
}

void _XBillboard::Render(D3DXVECTOR3 ViewPosition,double TimeElapsed)
{
	D3DXVECTOR3 WorldUp=D3DXVECTOR3(0.0,1.0,0.0);
	D3DXVECTOR3 View=ViewPosition-Position;
	
	D3DXVec3Normalize( &View, &View) ;

	FLOAT angle=D3DXVec3Dot(&WorldUp,&View);
	D3DXVECTOR3 Up=WorldUp-angle*View;
	D3DXVec3Normalize( &Up, &Up);

	D3DXVECTOR3 Right;
	D3DXVec3Cross( &Right, &Up, &View);

	Matrix._11 = Right.x; Matrix._21 = Up.x; Matrix._31 = View.x;
	Matrix._12 = Right.y; Matrix._22 = Up.y; Matrix._32 = View.y;
	Matrix._13 = Right.z; Matrix._23 = Up.z; Matrix._33 = View.z;

	Matrix._41=Position.x;
	Matrix._42=Position.y;
	Matrix._43=Position.z;

	Matrix._14=0.0f;
	Matrix._24=0.0f;
	Matrix._34=0.0f;
	Matrix._44=1.0f;

	AnimationCounter+=(FLOAT)TimeElapsed;

	if(AnimationCounter>AnimationSpeed)
	{
		 CurrentFrame++;
		 AnimationCounter=0;
		 if (CurrentFrame==NumberOfFrames) CurrentFrame=0;

		VertexList[0].tu=TextureX[CurrentFrame][0];
		VertexList[1].tu=TextureX[CurrentFrame][1];
		VertexList[2].tu=TextureX[CurrentFrame][2];
		VertexList[3].tu=TextureX[CurrentFrame][3];

		VertexList[0].tv=TextureY[CurrentFrame][0];
		VertexList[1].tv=TextureY[CurrentFrame][1];
		VertexList[2].tv=TextureY[CurrentFrame][2];
		VertexList[3].tv=TextureY[CurrentFrame][3];
	}

	gpDev->SetTransform(D3DTS_WORLD,&Matrix);
	gpDev->SetFVF( D3DFVF_XBILLBOARD_VERTEX );
	gpDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(_XBILLBOARD_VERTEX) );
}

void _XBillboard::RenderRestrictAxis(D3DXVECTOR3 ViewPosition,D3DXVECTOR3 Axis,BYTE WhichVector,double TimeElapsed)
{
	AnimationCounter+=(FLOAT)TimeElapsed;

	if(AnimationCounter>AnimationSpeed)
	{
		CurrentFrame++;
		AnimationCounter=0;
		if (CurrentFrame==NumberOfFrames) CurrentFrame=0;

		VertexList[0].tu=TextureX[CurrentFrame][0];
		VertexList[1].tu=TextureX[CurrentFrame][1];
		VertexList[2].tu=TextureX[CurrentFrame][2];
		VertexList[3].tu=TextureX[CurrentFrame][3];

		VertexList[0].tv=TextureY[CurrentFrame][0];
		VertexList[1].tv=TextureY[CurrentFrame][1];
		VertexList[2].tv=TextureY[CurrentFrame][2];
		VertexList[3].tv=TextureY[CurrentFrame][3];
	}
	RenderRestrictAxis(ViewPosition,Axis,WhichVector);
}

void _XBillboard::RenderRestrictAxis(D3DXVECTOR3 ViewPosition,D3DXVECTOR3 Axis,BYTE WhichVector)
{
	D3DXVECTOR3 Up,View,Right;
	D3DXVECTOR3 TempLookVector=ViewPosition-Position;

	if (WhichVector!=BBUpVector && WhichVector!=BBRightVector) return;

	if (WhichVector==BBUpVector)
	{
		Up=Axis;
		D3DXVec3Cross( &Right, &Up, &TempLookVector );
		D3DXVec3Normalize( &Right, &Right );
	}
	else
	{
		Right=Axis;
		D3DXVec3Cross( &Up, &TempLookVector, &Right);
		D3DXVec3Normalize( &Up, &Up );
	}

	D3DXVec3Cross( &View, &Up, &Right );
	D3DXVec3Normalize( &View, &View );

	Matrix._11 = Right.x; Matrix._21 = Up.x; Matrix._31 = View.x;
	Matrix._12 = Right.y; Matrix._22 = Up.y; Matrix._32 = View.y;
	Matrix._13 = Right.z; Matrix._23 = Up.z; Matrix._33 = View.z;

	Matrix._41=Position.x;
	Matrix._42=Position.y;
	Matrix._43=Position.z;

	Matrix._14=0.0f;
	Matrix._24=0.0f;
	Matrix._34=0.0f;
	Matrix._44=1.0f;

	gpDev->SetTransform(D3DTS_WORLD,&Matrix);	
	gpDev->SetFVF( D3DFVF_XBILLBOARD_VERTEX );
	gpDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, VertexList, sizeof(_XBILLBOARD_VERTEX) );
}

void _XBillboard::SetTextureCoords(WORD vertex,FLOAT x,FLOAT y)
{
	if (vertex>3) return; // bogus index!! must between 0-3

	VertexList[vertex].tu=x;
	VertexList[vertex].tv=y;

	TextureX[0][vertex]=x;
	TextureY[0][vertex]=y;
}


 void _XBillboard::SetTextureCoords(WORD vertex,WORD x,WORD y,WORD TexWidth,WORD TexHeight)
{
	if (vertex>3) return; // bogus index!! must between 0-3

	TextureX[0][vertex]=VertexList[vertex].tu=((FLOAT)x/(FLOAT)TexWidth);
	TextureY[0][vertex]=VertexList[vertex].tv=((FLOAT)y/(FLOAT)TexHeight);
}


void _XBillboard::SetTextureCoords(WORD frame,WORD vertex,FLOAT x,FLOAT y)
{
	if (vertex>3) return;  
	TextureX[frame][vertex]=x;
	TextureY[frame][vertex]=y;
}


void _XBillboard::SetTextureCoords(WORD frame,WORD vertex,WORD x,WORD y,WORD TexWidth,WORD TexHeight)
{
	if (vertex>3) return; 
	TextureX[frame][vertex]=((FLOAT) x/(FLOAT)TexWidth);
	TextureY[frame][vertex]=((FLOAT) y/(FLOAT)TexHeight);
}
