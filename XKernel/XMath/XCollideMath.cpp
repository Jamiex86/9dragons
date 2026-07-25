#include "stdafx.h"
#include "XKernel.h"
#include "XCollideMath.h"
#include "XMeshMath.h"
#include "XFastCode.h"
#include "XEigen.h"

//-----------------------------------------------------------------------------
// Name: SuMath_VectorMatrixMultiply()
// Desc: 벡터와 메트릭스를 곱한다.
//-----------------------------------------------------------------------------
HRESULT SuMath_VectorMatrixMultiply( D3DXVECTOR3* vDest, D3DXVECTOR3* vSrc,
                                      D3DMATRIX* mat)
{
	vDest->x = vSrc->x*mat->_11 + vSrc->y*mat->_21 + vSrc->z* mat->_31 + mat->_41;
	vDest->y = vSrc->x*mat->_12 + vSrc->y*mat->_22 + vSrc->z* mat->_32 + mat->_42;
	vDest->z = vSrc->x*mat->_13 + vSrc->y*mat->_23 + vSrc->z* mat->_33 + mat->_43;
    
	return S_OK;
}

//-------------------------------------------------------------------------------------------------------
// Name: VOID MakeIdentiVec( SUVECTOR* psuVector)
// 인풋값: 첫번째 D3DXVECTOR3의 포인터.
// Desc:   인풋값벡터를 단위 벡터로 만든다. 벡터의 길이로 각 벡터의 인자값을 나누어서 만든다.
//-------------------------------------------------------------------------------------------------------
VOID MakeIdentiVec( D3DXVECTOR3* psuVector )
{
	// 벡터의 길이를 구하구
	float fLength = sqrtf( psuVector->x * psuVector->x + psuVector->y * psuVector->y 
					 + psuVector->z * psuVector->z );
	// 단위 벡터로 만든다.
	psuVector->x /= fLength;
	psuVector->y /= fLength;
	psuVector->z /= fLength;
}

//-------------------------------------------------------------------------------------------------------
// Name: FindThePointOnTheLine()
// 인풋값: 첫번째 D3DXVECTOR3의 포인터.
// Desc:   직선위의 한점p을 보간 계수 t위치에 있는 점을 찿는다.
//		   pvO : 선분의 원점 벡터, pvD : 단위 방향벡터	t = 보간인수
//-------------------------------------------------------------------------------------------------------
VOID FindThePointOnTheLine(D3DXVECTOR3* pOutput, D3DXVECTOR3* pvO, D3DXVECTOR3* pvD, float t)
{
	// 직선위의 한점p을 정의 하면 
	// O = 직선의 원점(시작 벡터)
	// D = 직선의 방향 벡터(유한 직선의 끝점)
	// t = 직선의 보간인수
	// R(p) = O + tD
			
	pOutput->x = pvO->x + ( t * pvD->x );
	pOutput->y = pvO->y + ( t * pvD->y );
	pOutput->z = pvO->z + ( t * pvD->z );
}

//-------------------------------------------------------------------------------------------------------
// Name: ectorsub(D3DXVECTOR3* pOutput, D3DXVECTOR3* pv0, D3DXVECTOR3* pv1)
// 인풋값: 벡터 빼기
//-------------------------------------------------------------------------------------------------------
VOID Vectorsub(D3DXVECTOR3* pOutput, D3DXVECTOR3* pv0, D3DXVECTOR3* pv1)
{
	pOutput->x = pv1->x - pv0->x;
	pOutput->y = pv1->y - pv0->y;
	pOutput->z = pv1->z - pv0->z;
}

//----------------------------------------------------------------------------------------------
// LineEquation(D3DXVECTOR3* vP0, D3DXVECTOR3* vP1)
//      vP0를 이용해서 vP1을 지나는 직선의 방정식 
//		y= mx + b를 구한다.
//     기울기 m = (y1-y0)/(x1-x0)
//  어떤 한점 vP2 가 직선 위쪽에 있는지 아래쪽에 있는지 판별한다. 위쪽이면 
//  리턴 값이 1 아래쪽이면 리턴값 2 직선위에 있다면 리턴 0 을 각각 리턴 한다. 
//---------------------------------------------------------------------------------------------
int LineEquation(D3DXVECTOR3* vP0, D3DXVECTOR3* vP1, D3DXVECTOR3* vP2 )
{
	float fm= 0.0f, fb = 0.0f , fLineY = 0.0f;

	fm = (vP1->z - vP0->x) / (vP1->x - vP0->x); //직선의 기울기
	fb = vP1->z - ( fm * vP1->x ); //b = y-mx
	fLineY = (fm * vP2->x) + fb;

	if( fLineY < vP2->z ) return 1;		 // 비교점이 직선 보다 위에 있다.
	else if( fLineY > vP2->z )
	{
		return 2; // 비교점이 직선 보다. 아래에 있다.
	}
	else if( fLineY == vP2->z ) 
	{
		return 3; // 비교점이 직선상에 있다.
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------------
// Name: FindThePointOnTheLine()
// 인풋값: 첫번째 D3DXVECTOR3의 포인터.
// Desc:   직선위의 한점p을 보간 계수 t위치에 있는 점을 찿는다.
//		   pvO : 선분의 원점 벡터, pvD : 단위 방향벡터	t = 보간인수
//-------------------------------------------------------------------------------------------------------
VOID PlaneEquation( PLANEQUATION* pPlaneEq, D3DXVECTOR3* vP0, D3DXVECTOR3* vP1, D3DXVECTOR3* vP2)
{
	D3DXVECTOR3 vLeft;
	D3DXVECTOR3 vRight;
	D3DXVECTOR3 vNoraml;

	vLeft.x		= vP1->x - vP0->x ; 
	vLeft.y		= vP1->y - vP0->y ; 
	vLeft.z		= vP1->z - vP0->z ;

	vRight.x	= vP2->x - vP0->x ; 
	vRight.y	= vP2->y - vP0->y ; 
	vRight.z	= vP2->z - vP0->z ;
	
	D3DXVec3Cross( &vNoraml, &vLeft,  &vRight );
	MakeIdentiVec( &vNoraml );
	pPlaneEq->vNomal	= vNoraml;	
	pPlaneEq->fD		= -D3DXVec3Dot( &vNoraml, vP0 );  
}

//-------------------------------------------------------------------------------------------------------
// Name  : GetRatateRadian()
// Desc	 : 마우스 피킹점을 뷰좌표계로 변환후 뷰좌표계의 원점을 기준으로 피점의 회전각을 구한다.
//-------------------------------------------------------------------------------------------------------
float GetRotateRadian( LPDIRECT3DDEVICE9 pd3dDevice, POINT ptCursor )
{
	float			x			= 0.0f;
	float			y			= 0.0f;
	float			fRadian		= 0.0f;
	float			thetha		= 0.0f; //빗변
	D3DVIEWPORT9	vp;
	D3DXMATRIX		mat;

	D3DXMatrixIdentity( &mat );
    	
	if(!pd3dDevice)
		return 0.0f;
    // Get the pick ray from the mouse position	  
    pd3dDevice->GetTransform( D3DTS_PROJECTION, &mat);
			  
	// Compute the vector of the pick ray in screen space
	// 마우스의 좌표를 -1 ~ 1사이로 수정 y는 윈도우좌표가 +- 가 바뀌어있기때문에 바꾼다.
	// 원점에서 깊이가 1인 곳까지의 방향 벡터..
	pd3dDevice->GetViewport(&vp);
	x =  ( ( ( 2.0f * ptCursor.x ) / vp.Width  ) - 1 ) / mat._11;
	y = -( ( ( 2.0f * ptCursor.y ) / vp.Height ) - 1 ) / mat._22;
	
	//tan세타  = 높이  / 밑변 에서 
	//float theta = acosf( y / hypotenuse );
	// 1라디안은 0.01745도 
	if(x>0.0f && y>0.0f ) // 뷰좌표계의 임이의 1사 분면
	{
		fRadian = atanf( x / y );//*3.14f/180.0f);
	}
	else if( x < 0.0f && y > 0.0f) // 뷰좌표계의 임이의 2사 분면
	{
		fRadian = ( atanf( x / y ) );// * -1.0f;//*3.14f/180.0f);
	}
	else if( x < 0.0f && y < 0.0f) // 뷰좌표계의 임이의 3사 분면
	{
		thetha =  atanf( x / y ) ; // + 1.5705f ) * -1.0f;//*3.14f/180.0f);
		fRadian = thetha - 3.141f;
	}
	else if( x > 0.0f && y < 0.0f) // 뷰좌표계의 임이의 3사 분면
	{
		thetha	=  atanf( x / y ) ;
		fRadian =  3.141f + thetha ;
	}
	float teset = atanf(45.0f);
	return fRadian;
}
//-------------------------------------------------------------------------------------------------------
// Name  : GetRatateRadian( LPDIRECT3DDEVICE9 pd3dDevice, float fOrigX, float fOrigY, POINT ptCursor )
// Desc	 : 임이의 위치를 원점으로해서 마우스 피킹점을 각각 뷰좌표계로 변환후 회전각을 구한다.
//-------------------------------------------------------------------------------------------------------
float GetRotateRadian( LPDIRECT3DDEVICE9 pd3dDevice, D3DXVECTOR3* pvbasis, POINT ptCursor, D3DXMATRIX*	pViewMat, D3DXMATRIX*	pProjMat, D3DVIEWPORT9*	pvp )
{
	float			x		= 0.0f;
	float			y		= 0.0f;
	float			fRadian	= 0.0f;
	float			thetha	= 0.0f; //빗변
	float			fa		= 0.0f;
	float			fb		= 0.0f;
	float			foriginx = 0.0f;
	float			foriginy = 0.0f;

	D3DXVECTOR3		vbasis;
	vbasis.x = pvbasis->x;
	vbasis.y = pvbasis->y;
	vbasis.z = pvbasis->z;


	D3DVIEWPORT9	vp  = *pvp;
	D3DXMATRIX		matProj, matView;

	D3DXMatrixIdentity( &matProj );
	D3DXMatrixIdentity( &matView );
    	
	if(!pd3dDevice)
		return 0.0f;
    // Get the pick ray from the mouse position	  
	
	matProj		= *pProjMat;
	matView		= *pViewMat;
    //pd3dDevice->GetTransform( D3DTS_PROJECTION, &mat );
	//pd3dDevice->GetTransform( D3DTS_VIEW, &matView );

	D3DXVec3TransformCoord(&vbasis, &vbasis, &matView);
	
	foriginx = vbasis.x / vbasis.z;
	foriginy = vbasis.y / vbasis.z;
	// Compute the vector of the pick ray in screen space
	// 마우스의 좌표를 -1 ~ 1사이로 수정 y는 윈도우좌표가 +- 가 바뀌어있기때문에 바꾼다.
	// 원점에서 깊이가 1인 곳까지의 방향 벡터..
	//pd3dDevice->GetViewport(&vp);
	x =  ( ( ( 2.0f * ptCursor.x ) / vp.Width  ) - 1 ) / matProj._11;
	y = -( ( ( 2.0f * ptCursor.y ) / vp.Height ) - 1 ) / matProj._22;
	
	fa = x - foriginx;
	fb = y - foriginy;

	//tan세타  = 높이  / 밑변 에서 
	//float theta = acosf( y / hypotenuse );
	// 1라디안은 0.01745도 
	if(fa>0.0f && fb>0.0f ) // 뷰좌표계의 임이의 1사 분면
	{
		fRadian = atanf( fa / fb );//*3.14f/180.0f);
	}
	else if( fa < 0.0f && fb > 0.0f) // 뷰좌표계의 임이의 2사 분면
	{
		fRadian = ( atanf( fa / fb ) );// * -1.0f;//*3.14f/180.0f);
	}
	else if( fa < 0.0f && fb < 0.0f) // 뷰좌표계의 임이의 3사 분면
	{
		thetha =  atanf( fa / fb ) ; // + 1.5705f ) * -1.0f;//*3.14f/180.0f);
		fRadian = thetha - D3DX_PI;
	}
	else if( fa > 0.0f && fb < 0.0f) // 뷰좌표계의 임이의 3사 분면
	{
		thetha	=  atanf( fa / fb ) ;
		fRadian =  D3DX_PI + thetha ;//3.141f + thetha ;
	}
	return fRadian;
}


//-------------------------------------------------------------------------------------------------------
// Name  : GetRatateRadian( LPDIRECT3DDEVICE8 pd3dDevice, float fOrigX, float fOrigY, POINT ptCursor )
// Desc	 : 임의의 두 위치 벡터를 뷰좌표계로 변환후  회전각을 구한다.
//-------------------------------------------------------------------------------------------------------
float GetRotateRadianFromTwoPoint( LPDIRECT3DDEVICE9 pd3dDevice, D3DXVECTOR3* pvbasis, D3DXVECTOR3* pvDir,
					   D3DXMATRIX*	pViewMat )	
{
	float			fDx		= 0.0f;
	float			fDy		= 0.0f;
	float			fRadian	= 0.0f;
	float			thetha	= 0.0f; 

	float			fa		= 0.0f; // 밑변 
	float			fb		= 0.0f; // 높이

	float			foriginx = 0.0f;
	float			foriginy = 0.0f;

	D3DXVECTOR3		vbasis;
	D3DXVECTOR3		vDir;

	vbasis.x = pvbasis->x;
	vbasis.y = pvbasis->y;
	vbasis.z = pvbasis->z;

	vDir.x = pvDir->x;
	vDir.y = pvDir->y;
	vDir.z = pvDir->z;


	D3DXMATRIX		mat, matView;

	D3DXMatrixIdentity( &matView );
	matView = *pViewMat;
    	
	if(!pd3dDevice)
		return 0.0f;
    // 2개의 벡터를 뷰좌표계로 변환하고
    D3DXVec3TransformCoord(&vbasis, &vbasis, &matView);
	D3DXVec3TransformCoord(pvDir, pvDir, &matView);
	
	// 뷰좌표계에서 뷰의 원점에서 깊이가 1인 곳까지의 원점 벡터..
	foriginx = vbasis.x / vbasis.z;
	foriginy = vbasis.y / vbasis.z;
	
	// 뷰좌표계에서 뷰의 원점에서 깊이가 1인 곳까지의 방향 벡터..
	
	fDx =  pvDir->x / pvDir->z;
	fDy =  pvDir->y / pvDir->z;
	
	//  두 벡터가 한축에 모두 있을때 ㅓ리는 어떻게 하지

	//tan세타  = 높이  / 밑변 에서 
	//float theta = acosf( y / hypotenuse );
	// 1라디안은 0.01745도 
	// D3DX_PI/4 == 45도 

	if( foriginx < fDx && foriginy < fDy ) // 뷰좌표계의 임이의 1사 분면
	{
		fa = fDx - foriginx;
		fb = fDy - foriginy; 
		fRadian = atanf( fb / fa );//*3.14f/180.0f);
	}
	else if( foriginx > fDx && foriginy < fDy ) // 뷰좌표계의 임이의 2사 분면
	{
		fa = fDy - foriginy;
		fb = fDx - foriginx; 
		fRadian =  atanf( fb / fa ) + ( D3DX_PI/2 );// * -1.0f;//*3.14f/180.0f);

	}
	else if( foriginx > fDx && foriginy > fDy ) // 뷰좌표계의 임이의 3사 분면
	{
		fa = fDx - foriginx;
		fb = fDy - foriginy; 
		fRadian =  atanf( fb / fa ) + ( D3DX_PI ) ; // + 1.5705f ) * -1.0f;//*3.14f/180.0f);
	}
	else if( foriginx < fDx && foriginy > fDy ) // 뷰좌표계의 임이의 4사 분면
	{
		fa = fDy - foriginy;
		fb = fDx - foriginx; 
		fRadian =  atanf( fa / fb ) + (D3DX_PI + D3DX_PI/2 );
		
	}
	return fRadian;
}

//================================================================================================================
// VOID GetRotatedPo(D3DXVECTOR3 vOrig)
// 윈 회전을 Y축을 중심으로 원 회전을 하는 원상의 한 점을 찻는다.
// vOrig = 원의 중점 fR = 원의 반지름
//================================================================================================================
VOID FindePoOnCircle(D3DXVECTOR3& vrOrig, D3DXVECTOR3& vCurrentPo, float fRadian, float fR, int nID)
{
	//XZ 평면에서의 원의 방정식
	//(x-xc)²+ (z-zc)²= r²
	D3DXVECTOR3	v;
	
	switch(nID)
	{
		case 1: // vrOrigd의 Y축을 중심으로 회전 Yaw
			v.x = fR * cosf( D3DX_PI/2 - fRadian );
			v.z = fR * sinf( D3DX_PI/2 - fRadian );
				
			vCurrentPo.x = v.x + vrOrig.x;
			vCurrentPo.z = v.z + vrOrig.z;
			break;
			

		case 2: // vrOrigd의 X축을 중심으로 회전 pitch
			v.y = fR * cosf( D3DX_PI/2 - fRadian );
			v.z = fR * sinf( D3DX_PI/2 - fRadian );
				
			vCurrentPo.y = v.y + vrOrig.y;
			vCurrentPo.z = v.z + vrOrig.z;
			break;
	}
}

//================================================================================================================
// float GetPitchRadian( D3DXVECTOR3& vrOrig, D3DXVECTOR3& vCurrendtPo, float fR )
// 피치값을 구한다. 카메라에 상용할거다
// vOrig = 원의 중점  = 원의 반지름
// nID  1== Pitch 2== Yaw 
//================================================================================================================
float GetRadian( D3DXVECTOR3& vrOrig , D3DXVECTOR3& vCurrendtPo, int nID)
{
	float fRadian = 0.0f;
	float fa = 0.0f;
	float fb = 0.0f;
	switch(nID)
	{
		case 1:
			fa = vCurrendtPo.z - vrOrig.z;
			fb = vCurrendtPo.y - vrOrig.y;
			break;

		case 2:

			fa = vCurrendtPo.x - vrOrig.x;
			fb = vCurrendtPo.z - vrOrig.z;
			break;
		default:
			return fRadian = 0.0f;
			break;
	}
	//_XFC_Abs(fMax-fMin)
	return fRadian = atanf( (float)fabs(fb) / (float)fabs(fa) );//_XFC_Abs(fb) / _XFC_Abs(fa) );//*3.14f/180.0f);
	
}
//================================================================================================================
// float GetPitchRadian( D3DXVECTOR3& vrOrig, D3DXVECTOR3& vCurrendtPo, float fR )
// 피치값을 구한다.
// vOrig = 원의 중점 fR = 빗변의 길이 = 원의 반지름
//================================================================================================================
float GetPitchRadian( D3DXVECTOR3& vrOrig, D3DXVECTOR3& vCurrendtPo )
{
	float fRadian = 0.0f;
	float fy = vCurrendtPo.y - vrOrig.y;
	float fz = vCurrendtPo.z - vrOrig.z;

	fRadian = atanf( _XFC_Abs(fy) / _XFC_Abs(fz) );//*3.14f/180.0f);
	return fRadian;
}

//================================================================================================================
// VOID FindePoOnPitchRotatedCircle(D3DXVECTOR3& vrOrig, D3DXVECTOR3& vCurrendtPo, float fYawRaian, float fR)
// 윈 회전을 X축을 중심으로 원 회전을 하는 원상의 한 점을 찻는다. 따라서 값은 XY 평면에서만 계산한다.
// vOrig = 원의 중점 fR = 원의 반지름
//================================================================================================================
VOID FindePoOnPitchRotatedCircle(D3DXVECTOR3& vrOrig, D3DXVECTOR3& vCurrendtPo, float fYaw, float fPich, float fR)
{
	//XZ 평면에서의 원의 방정식
	//(x-xc)²+ (z-zc)²= r²
	D3DXVECTOR3	v;
	
	v.x = fR * _XFC_Cos( D3DX_PI/2 - fPich );
	v.y = fR * _XFC_Sin( D3DX_PI/2 - fPich );
		
	vCurrendtPo.x = v.x + vrOrig.x;
	vCurrendtPo.y = v.y + vrOrig.y;

}

float SuVec3Length( D3DXVECTOR3& v1, D3DXVECTOR3& v2 )
{
	float fLength;
	D3DXVECTOR3 vOut = v1 - v2;
	fLength = D3DXVec3Length( &vOut );
   
	return fLength;
}
//================================================================================================================
// GetRotationFromView( D3DXMATRIX& View  )
// 축 벡터를 구한다.
//vTarget : 축을 중심으로 회전할 벡터.
//vAxis		: 축을 지나는 벡터
// 리턴값 축 벡터
//================================================================================================================
D3DXVECTOR3 GetAxisVector( D3DXVECTOR3& v1, D3DXVECTOR3& vTarget, D3DXVECTOR3& vAxis  )
{
	D3DXVECTOR3 p1 ;
	D3DXVECTOR3 p2 ;

	float		fAngle = 0.0f;

	p1 = vTarget - vAxis;
	p2 = v1		 - vAxis;

	D3DXVec3Normalize(&p1, &p1);
	D3DXVec3Normalize(&p2, &p2);

	// Get axis of rotation, which is cross product of p1 and p2
	D3DXVECTOR3 vGetAxis;
	//CROSS((float*)&vGetAxis, (float*)&p1, (flaot*)&p2);
    D3DXVec3Cross( &vGetAxis, &p1, &p2);
	D3DXVec3Normalize(&vGetAxis, &vGetAxis);
		
	return vGetAxis;
}

//================================================================================================================
// GetAngleUseingDot( D3DXVECTOR3& vA, D3DXVECTOR3& vB)
// 두 벡터의 내적을 이용해서 두 벡터의 사잇각을 구한다.
//================================================================================================================
float GetAngleUseingDot( D3DXVECTOR3* pvA, D3DXVECTOR3* pvB)
{
	// |A||B|cos theta = X2X1 + Y2Y1 + Z2Z1 = A●B  : 내적의 성질 
	//cos theta = (A●B)/|A||B|
	// |A| = X²+ Y²+ Z² = 벡터의 크기
	float fcost = _XFC_DotProduct( (float*)pvA, (float*)pvB ); //D3DXVec3Length( 
	float fTheta = acosf( fcost / ( D3DXVec3Length(pvA) *  D3DXVec3Length(pvA) ) );
	return fTheta;
}
 
VOID InitRotateMat( float fDegree, D3DXMATRIX&	matRotate)
{
	D3DXQUATERNION quat;
	D3DXVECTOR3 vAxis = D3DXVECTOR3(0.0f,  1.0f, 0.0f);  //축을 고정
	float fangle = fDegree;
	
	D3DXQuaternionRotationAxis( &quat, &vAxis, D3DXToRadian(fangle) );
	D3DXMatrixRotationQuaternion( &matRotate, &quat ); //쿼터니언으로 부터.. 메트릭스를 만들구
	
}

VOID GetRotateMat( D3DMATRIX& matRoate, D3DXVECTOR3& Axis, float fRadian)
{
	D3DXQUATERNION  quat;
	D3DXVECTOR3		vAxis = Axis; 
	
		
	D3DXQuaternionRotationAxis( &quat, &vAxis, fRadian );
//	D3DXMatrixRotationQuaternion( (matRoate), &quat ); //쿼터니언으로 부터.. 메트릭스를 만들구

}

//================================================================================================================
// GetRightAxis( D3DXVECTOR3& vRight, D3DXVECTOR3& vDirection); 
//단위 벡터인 vDirection으로 vRight벡터를 구한다.
//================================================================================================================
VOID GetRightAxis( D3DXVECTOR3& vRight, const D3DXVECTOR3& vDirection)
{
	D3DXVECTOR3 vDir = vDirection;
	D3DXVECTOR3 vUpBase;
	if( fabs(vDir.x) <= fabs(vDir.y) )
	{
		if( fabs(vDir.z) <= fabs(vDir.x) )	{ vUpBase.x = 0.0f;		vUpBase.y = 0.0f;	vUpBase.z = 1.0f; }
		else{ vUpBase.x = 1.0f;		vUpBase.y = 0.0f;	vUpBase.z = 0.0f;}
	}
	else
	{
		if( fabs(vDir.z) <= fabs(vDir.y) )	{ vDir.x = 0.0f;	vDir.y = 0.0f;	vDir.z = 1.0f; }
		else{ vUpBase.x = 0.0f;		vUpBase.y = 1.0f;	vUpBase.z =		0.0f; }
	}
	
	D3DXVec3Cross(&vRight, &vDir, &vUpBase);
}

//================================================================================================================
// GetRightAxis( D3DXVECTOR3& vRight, D3DXVECTOR3& vDirection); 
//단위 벡터인 vDirection으로 vRight벡터, vUp벡터를 구한다.
//================================================================================================================
VOID GetRight_Up_Axis( D3DXVECTOR3& vRight, D3DXVECTOR3& vUp, const D3DXVECTOR3& vDirection)
{
	D3DXVECTOR3 vDir = vDirection;
	D3DXVECTOR3 vUpBase;
	if( fabs(vDir.x) <= fabs(vDir.y) )
	{
		if( fabs(vDir.z) <= fabs(vDir.x) )	{ vUpBase.x = 0.0f;		vUpBase.y = 0.0f;	vUpBase.z = 1.0f; }
		else{ vUpBase.x = 1.0f;		vUpBase.y = 0.0f;	vUpBase.z = 0.0f;}
	}
	else
	{
		if( fabs(vDir.z) <= fabs(vDir.y) )	{ vDir.x = 0.0f;	vDir.y = 0.0f;	vDir.z = 1.0f; }
		else{ vUpBase.x = 0.0f;		vUpBase.y = 1.0f;	vUpBase.z =		0.0f; }
	}
	
	D3DXVec3Cross(&vRight, &vDir, &vUpBase);
	D3DXVec3Cross(&vUp, &vRight, &vDir);
}
//=================================================================================================================
//내용은 모른다 버텍스의 좌표들을 집어 넣으면.. obb 박스인 센터 값과. 3개의 축을 구할수 있다..
//어떻게 돌아 가는지 몰라요.. 나두 괴로워요 3D 겜 엔진책 보고 배겼음..
// 모델의 버텍스 갯수  , 모델의 버텍스(벡터 단위)
//=================================================================================================================
void SuGaussPointsFit (int iQuantity, const D3DXVECTOR3* akPoint, 
					   D3DXVECTOR3& rkCenter, D3DXVECTOR3 akAxis[3], float afExtent[3])
{
    // compute mean of points
    rkCenter = akPoint[0];
    int i;
    for (i = 1; i < iQuantity; i++)
        rkCenter += akPoint[i];
    float fInvQuantity = 1.0f/iQuantity;
    rkCenter *= fInvQuantity;

    // compute covariances of points
    float fSumXX = 0.0, fSumXY = 0.0, fSumXZ = 0.0;
    float fSumYY = 0.0, fSumYZ = 0.0, fSumZZ = 0.0;
    for (i = 0; i < iQuantity; i++)
    {
        D3DXVECTOR3 kDiff = akPoint[i] - rkCenter;
        fSumXX += kDiff.x*kDiff.x;
        fSumXY += kDiff.x*kDiff.y;
        fSumXZ += kDiff.x*kDiff.z;
        fSumYY += kDiff.y*kDiff.y;
        fSumYZ += kDiff.y*kDiff.z;
        fSumZZ += kDiff.z*kDiff.z;
    }
    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumXZ *= fInvQuantity;
    fSumYY *= fInvQuantity;
    fSumYZ *= fInvQuantity;
    fSumZZ *= fInvQuantity;

    // compute eigenvectors for covariance matrix
    SuEigen kES(3);
    kES.Matrix(0,0) = fSumXX;
    kES.Matrix(0,1) = fSumXY;
    kES.Matrix(0,2) = fSumXZ;
    kES.Matrix(1,0) = fSumXY;
    kES.Matrix(1,1) = fSumYY;
    kES.Matrix(1,2) = fSumYZ;
    kES.Matrix(2,0) = fSumXZ;
    kES.Matrix(2,1) = fSumYZ;
    kES.Matrix(2,2) = fSumZZ;
    kES.IncrSortEigenStuff3();

    akAxis[0].x = kES.GetEigenvector(0,0);
    akAxis[0].y = kES.GetEigenvector(1,0);
    akAxis[0].z = kES.GetEigenvector(2,0);
    akAxis[1].x = kES.GetEigenvector(0,1);
    akAxis[1].y = kES.GetEigenvector(1,1);
    akAxis[1].z = kES.GetEigenvector(2,1);
    akAxis[2].x = kES.GetEigenvector(0,2);
    akAxis[2].y = kES.GetEigenvector(1,2);
    akAxis[2].z = kES.GetEigenvector(2,2);

    afExtent[0] = kES.GetEigenvalue(0);
    afExtent[1] = kES.GetEigenvalue(1);
    afExtent[2] = kES.GetEigenvalue(2);
}
