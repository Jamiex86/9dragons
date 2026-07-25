#include "stdafx.h"
#include "XCollide.h"
#include "XCollideMath.h"
#include "XFastCode.h"

//-----------------------------------------------------------------------------
// Name: ExtractFrustumPlanes(LPDIRECT3DDEVICE9	pd3dDevice, PLANE2 *Planes )
// Desc: This function Extracts the Frustum Planes from a Combined View/Projection matrix.
//       뷰 볼륨 6개의 평면의 방정식을 구한다. 과 면의 노멀 벡터와
//        평면의 방정식은 노멀벡터와 상수 D값으로 정의됨  
//----------------------------------------------------------------------------- 

void ExtractFrustumPlanes(SuVIEWVOLUME* pViewVolume, LPDIRECT3DDEVICE9	pd3dDevice)//, PLANE2 *Planes )
{

	D3DXMATRIX ViewMatrix,ProjMatrix,ViewProj;

	pd3dDevice->GetTransform(D3DTS_PROJECTION, &ProjMatrix);// get the current projection matrix
	pd3dDevice->GetTransform(D3DTS_VIEW, &ViewMatrix);// get the current projection matrix

	D3DXMatrixMultiply(&ViewProj,&ViewMatrix,&ProjMatrix);// combine View and Projection matrices

	// Left clipping plane
	pViewVolume->Plane[0].Normal.x = -(ViewProj._14 + ViewProj._11);
	pViewVolume->Plane[0].Normal.y = -(ViewProj._24 + ViewProj._21);
	pViewVolume->Plane[0].Normal.z = -(ViewProj._34 + ViewProj._31);
	pViewVolume->Plane[0].Distance = -(ViewProj._44 + ViewProj._41);

	// Right clipping plane
	pViewVolume->Plane[1].Normal.x = -(ViewProj._14 - ViewProj._11);
	pViewVolume->Plane[1].Normal.y = -(ViewProj._24 - ViewProj._21);
	pViewVolume->Plane[1].Normal.z = -(ViewProj._34 - ViewProj._31);
	pViewVolume->Plane[1].Distance = -(ViewProj._44 - ViewProj._41);

	// Top clipping plane
	pViewVolume->Plane[2].Normal.x = -(ViewProj._14 - ViewProj._12);
	pViewVolume->Plane[2].Normal.y = -(ViewProj._24 - ViewProj._22);
	pViewVolume->Plane[2].Normal.z = -(ViewProj._34 - ViewProj._32);
	pViewVolume->Plane[2].Distance = -(ViewProj._44 - ViewProj._42);

	// Bottom clipping plane
	pViewVolume->Plane[3].Normal.x = -(ViewProj._14 + ViewProj._12);
	pViewVolume->Plane[3].Normal.y = -(ViewProj._24 + ViewProj._22);
	pViewVolume->Plane[3].Normal.z = -(ViewProj._34 + ViewProj._32);
	pViewVolume->Plane[3].Distance = -(ViewProj._44 + ViewProj._42);

	// Near clipping plane
	pViewVolume->Plane[4].Normal.x = -(ViewProj._14 + ViewProj._13);
	pViewVolume->Plane[4].Normal.y = -(ViewProj._24 + ViewProj._23);
	pViewVolume->Plane[4].Normal.z = -(ViewProj._34 + ViewProj._33);
	pViewVolume->Plane[4].Distance = -(ViewProj._44 + ViewProj._43);

	// Far clipping plane
	pViewVolume->Plane[5].Normal.x = -(ViewProj._14 - ViewProj._13);
	pViewVolume->Plane[5].Normal.y = -(ViewProj._24 - ViewProj._23);
	pViewVolume->Plane[5].Normal.z = -(ViewProj._34 - ViewProj._33);
	pViewVolume->Plane[5].Distance = -(ViewProj._44 - ViewProj._43);
	
} //End Funtion Extract Clip Planes


//-----------------------------------------------------------------------------
// Name: ViewCulling( SuVIEWVOLUME* pViewVolume, D3DXVECTOR3* pvPoint)
// Desc: 뷰볼륨의 포인터 와 체크할 점의 포인터
//       
//----------------------------------------------------------------------------- 

bool IsPointInViewVolume( SuVIEWVOLUME* pViewVolume, D3DXVECTOR3* pvPoint)
{
	D3DXVECTOR3 v1[4], vtemp[4] ;
	bool bisIn[6];
			
	vtemp[0].x = pvPoint->x - MOVEOFFSET_PATCHCENTERPOINT;	vtemp[0].y = MOVEOFFSET_PATCHCENTERPOINT;	vtemp[0].z =  pvPoint->z + MOVEOFFSET_PATCHCENTERPOINT;
	vtemp[1].x = pvPoint->x - MOVEOFFSET_PATCHCENTERPOINT;	vtemp[1].y = MOVEOFFSET_PATCHCENTERPOINT;	vtemp[1].z =  pvPoint->z - MOVEOFFSET_PATCHCENTERPOINT;
	vtemp[2].x = pvPoint->x + MOVEOFFSET_PATCHCENTERPOINT;	vtemp[2].y = MOVEOFFSET_PATCHCENTERPOINT;	vtemp[2].z =  pvPoint->z + MOVEOFFSET_PATCHCENTERPOINT;
	vtemp[3].x = pvPoint->x + MOVEOFFSET_PATCHCENTERPOINT;  vtemp[3].y = MOVEOFFSET_PATCHCENTERPOINT;	vtemp[3].z =  pvPoint->z - MOVEOFFSET_PATCHCENTERPOINT;
		
	float fconst = 0.0f;
	float f = 0.0f;
	
	int j = 0 ; for( j = 0; j<4; j++)
	{
		int i = 0  ; for( i = 0; i<6 ; i++)
		{
			if ( fconst = (D3DXVec3Dot(&pViewVolume->Plane[i].Normal, &vtemp[j] ) + pViewVolume->Plane[i].Distance ) < 0.0f ) 
			{
				bisIn[i] = true;

			}
			else 
				bisIn[i] = false;
		}
		if( bisIn[0] && bisIn[1] && bisIn[2] && bisIn[3] && bisIn[4] && bisIn[5] )
		{
			return true; // 점이 뷰 볼륨 안에 있으므로
		}
	}
	
	return false ;

}

//----------------------------------------------------------------------------------------------------
// Name: CMap::IntersectTriangle()
// Desc: 한 직선과 삼각형과의 충돌을 체크한다. in DX9 Pick Poly
//       Cramer의 determinant 공식과 barycentric coordinate를 이용해서 UV좌표를 알아 내는 것이다.
//		this function returns TRUE and the interpolated texture coordinates if the ray intersects the triangle
//----------------------------------------------------------------------------------------------------
BOOL IntersectRayTriangle( const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
							  D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
							  SUINTERSECTION* pintersectionInfo )                                       
{
	float u, v, det, t,f = 0.0f;
    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );
	
    // If determinant is near zero, ray lies in plane of triangle
	// |M| = | m,1 , m,2 m,3| = | mx, my, mz | = (mx × my) ● mz
    det = D3DXVec3Dot( &edge1, &pvec );
    D3DXVECTOR3 Svec;  //O - v0;
    if( det > 0 )
    {
        Svec = orig - v0;
    }
    else
    {
        Svec = v0 - orig;
        det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

	//barycentric coordinate u , v
	/* 거리 = t			1          det( s, e1, e2)
	          u  = ----------------(  det(-d,  s, e2)	  )
	          v    det(-d, e1, e2)	   det(-d, e1, s)

		그리구 determinant 값은 det(a, b, c) = |a b c| = -(a × c) ● b = -( c × b) ● a
		그래서 위의 식을
		t           1         ( s× e1 )●e2          1       q ● e2
		u  = ---------------( ( d× e2 )●s  ) = -----------( p ● s )
		v    (d × e2)●e1    ( s× e1 )●d       pvec●e2    q ● d
	*/
	f = 1/det;
    // Calculate U parameter and test bounds
    u = f*D3DXVec3Dot( &Svec, &pvec );
    if( u < 0.0f || u > 1.0f ) //barycentric coordinate좌표계가 성립 하느냐
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &Svec, &edge1 );

    // Calculate V parameter and test bounds
    v = f * D3DXVec3Dot( &dir, &qvec );

    if( v < 0.0f || u + v > 1.0f ) //barycentric coordinate좌표를 만족 하느냐
        return FALSE;
	t = f* D3DXVec3Dot( &edge2, &qvec );

	pintersectionInfo->tu		= u;
	pintersectionInfo->tv		= v;
	pintersectionInfo->fDist	= t;
    return TRUE;
}


//----------------------------------------------------------------------------------------------
// BOOL ( POINT ptCursor, BOUNDINGCROSS* pBoundingCross, LPDIRECT3DDEVICE9 pd3dDevice )
// 마우스 picking 점으로 캐릭터의 pBoundingCross에서의 Picking되었는지만 체크 한다.
//----------------------------------------------------------------------------------------------
BOOL CharicPick( POINT ptCursor, BOUNDINGCROSS* pBoundingCross, LPDIRECT3DDEVICE9 pd3dDevice ,
				 D3DXMATRIX* p_Viewmat, D3DXMATRIX*	pProjMat, D3DVIEWPORT9*	pvp, SUINTERSECTION* pIntersectionInfo )
{
	D3DXVECTOR3		vPickRayDir, vPickRayOrig, v;   
	D3DVIEWPORT9	vp;
	D3DXMATRIX		mat, m, matproj;
	D3DXVECTOR3		vFrontFace0, vFrontFace1, vFrontFace2;
	D3DXVECTOR3		vRface0, vRface1, vRface2;
	D3DXVECTOR3		vLface0, vLface1, vLface2;

	vLface0.x = vLface0.y = vLface0.z = 0.0f;


	int				i = 0; //m_IntersectionArray[MAX_INTERSECTIONS] 배열에 사용될 인자.
	SUINTERSECTION	interinfo;
	byte			length		= sizeof(SUINTERSECTION);
	bool			bintesets	= false;
	BOUNDINGCROSS   BoundingCross;
	ZeroMemory( &BoundingCross,  sizeof(BOUNDINGCROSS) );
	BoundingCross = *pBoundingCross;

	int nIndexedVertex[6] = {0,1,2,0,2,3};
	int	FrontPolyIndex = 0;  //nIndexedVertex의 버텍스 인덱스가 될거다
	int	SidedPolyIndex = 0;  //nIndexedVertex의 버텍스 인덱스가 될거다

	D3DXMatrixIdentity( &mat );
	D3DXMatrixIdentity( &matproj );
	D3DXMatrixIdentity( &m );
	ZeroMemory( &interinfo,  sizeof(interinfo) );
	    	
	if(!pd3dDevice)
		return NULL;
    // Get the pick ray from the mouse position	  
    //m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matproj);
	matproj		= *pProjMat;
			  
	// Compute the vector of the pick ray in screen space
	// 마우스의 좌표를 -1 ~ 1사이로 수정 y는 윈도우좌표가 +- 가 바뀌어있기때문에 바꾼다.
	// 원점에서 깊이가 1인 곳까지의 방향 벡터..
	//m_pd3dDevice->GetViewport(&vp);
	vp = *pvp;
	v.x =  ( ( ( 2.0f * ptCursor.x ) / vp.Width  ) - 1 ) / matproj._11;
	v.y = -( ( ( 2.0f * ptCursor.y ) / vp.Height ) - 1 ) / matproj._22;
	v.z =  1.0f;
			
	// Get the inverse view matrix	
	mat = *p_Viewmat;
	D3DXMatrixInverse( &m, NULL, &mat );
	float fZPoOfView = m._43; //뷰의 월드상의 Z 위치

	// Transform the screen space pick ray into 3D space
	// 뷰좌표계에 존재한는 벡터에다 뷰의의 역행렬을 곱해서 월드 좌표계로 변환 한다.
	//D3DXVec3TransformCoord(&vPickRayDir, &vPickRayDir, &matWorld);
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;

	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;	

	int ii = 0 ; for(  ii=0 ; ii<2; ii++)
	{
		if(bintesets)	break;
		FrontPolyIndex = 0;
		SidedPolyIndex = 0;
		int jj = 0 ; for( jj=0; jj<2; jj++ )
		{
			if(bintesets)	continue;
			if(jj == 2)
			{
				int iioio = 9;
			}

			vRface0 = BoundingCross.FrontPoly[ii].p[ nIndexedVertex[ FrontPolyIndex ] ]; FrontPolyIndex++;
			vRface1 = BoundingCross.FrontPoly[ii].p[ nIndexedVertex[ FrontPolyIndex ] ]; FrontPolyIndex++;
			vRface2 = BoundingCross.FrontPoly[ii].p[ nIndexedVertex[ FrontPolyIndex ] ]; FrontPolyIndex++;

			//=====================================================================================================
			// 카레라나 뷰의 원점에서 뒤에있는 객체는 체클를 하지 안고 시킵 하다.
			// 카메라나 뷰가 회전을 하지 안았을때 가능 하다.
			////=====================================================================================================
			if( ( fZPoOfView > vRface0.z )&& ( fZPoOfView > vRface1.z ) && ( fZPoOfView > vRface2.z )   ) continue;

			if( IntersectRayTriangle( vPickRayOrig, vPickRayDir, vRface0, vRface1, vRface2, &interinfo) )
			{
				   pBoundingCross->fIntersectedLength = interinfo.fDist;
				   pIntersectionInfo->poly.p[0] = vRface0;
				   pIntersectionInfo->poly.p[1] = vRface1;
				   pIntersectionInfo->poly.p[2] = vRface2;
				   pBoundingCross->IsPicked = bintesets =  true;
				   //MessageBox(NULL,"페이스와 교차", "페이스와 교차", MB_OK);
				   return true;
			}

			//=====================================================================================================
			// 카레라나 뷰의 원점에서 뒤에있는 객체는 체클를 하지 안고 시킵 하다.
			// 카메라나 뷰가 회전을 하지 안았을때 가능 하다.
			////=====================================================================================================
	
			if( ( fZPoOfView > vLface0.z )&& ( fZPoOfView > vLface1.z ) && ( fZPoOfView > vLface2.z )   ) continue;
			vLface0 = BoundingCross.SidePoly[ii].p[ nIndexedVertex[ SidedPolyIndex ] ]; SidedPolyIndex++;
			vLface1 = BoundingCross.SidePoly[ii].p[ nIndexedVertex[ SidedPolyIndex ] ]; SidedPolyIndex++;
			vLface2 = BoundingCross.SidePoly[ii].p[ nIndexedVertex[ SidedPolyIndex ] ]; SidedPolyIndex++;

			if( IntersectRayTriangle( vPickRayOrig, vPickRayDir, vLface0, vLface1, vLface2, &interinfo) )
			{
				   pBoundingCross->fIntersectedLength = interinfo.fDist;
				   pIntersectionInfo->poly.p[0] = vLface0;
				   pIntersectionInfo->poly.p[1] = vLface1;
				   pIntersectionInfo->poly.p[2] = vLface2;
				   pBoundingCross->IsPicked = bintesets =  true;
				   //MessageBox(NULL,"페이스와 교차 했다", "교차 했다", MB_OK);
				   return true;
			}

		}
		
	}

	if(!bintesets)
	{
		pBoundingCross->IsPicked = false;
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------
// int SercheClosestNo(SUINTERSECTION* interInfo)
// 방향벡타와 교차된 패치들에서.. 가장 가까운 패치의 인덱스를 챃는다.
//----------------------------------------------------------------------------------------------
int SercheClosestNo(SUINTERSECTION* interInfo, int n)
{
	int i ;
	float	fMin	= 0.0f;
	int		index	= interInfo[0].dwFace;
	int		No		= 0; //SUINTERSECTION 배열중 가장 fDist가 작은값의 배열의 인덱스
    fMin = interInfo[0].fDist ;

	for( i = 1; i<n; i++)
	{
		if( fMin > interInfo[i].fDist)
		{
			index = interInfo[i].dwFace;
			fMin  = interInfo[i].fDist ;
			No    = i;
		}
	}
	return No;
}

//----------------------------------------------------------------------------------------------
// D3DXVECTOR3 FindYValueOfm_pVertex( D3DXVECTOR* pv0)
// 월드상의 위치벡터를 가지구.. 현재 맵상의 y값을 추출 해낸다.
//----------------------------------------------------------------------------------------------
D3DXVECTOR3* FindYValueOfm_pVertex( D3DXVECTOR3* pv0 ,SUPOLIGON* pPlan)
{
	PLANEQUATION  pPlaneEq;
	int		indexy = 0, indexx = 0;

	D3DXVECTOR3 v0, v1, v2;	

	v0	= pPlan->p[0];
	v1	= pPlan->p[1];
	v2	= pPlan->p[2];
	PlaneEquation( &pPlaneEq, &v0, &v1, &v2);

	// Ax+By+Cz+D = 0 따라서 y = -( Ax+Cz+D ) / B
	pv0->y	= -( (pPlaneEq.vNomal.x * pv0->x ) + (pPlaneEq.vNomal.z * pv0->z ) + pPlaneEq.fD  ) / pPlaneEq.vNomal.y ;

	return pv0;
}

//----------------------------------------------------------------------------------------------
// int SercheClosestIntersectionInfo(SUINTERSECTION* interInfo, int n)
// 교차된 인터섹션 정보에서 가장 가까운 인터섹션 정보의 인텍스를 가져온다.
//----------------------------------------------------------------------------------------------
int SercheClosestIntersectionInfo(SUINTERSECTION* interInfo, int n)
{
	int i ;
	float	fMin	= 0.0f;
	int		index	= interInfo[0].dwFace;
	int		No		= 0; //SUINTERSECTION 배열중 가장 fDist가 작은값의 배열의 인덱스
    fMin = interInfo[0].fDist ;

	for( i = 0; i<n; i++)
	{
		if( fMin > interInfo[i].fDist)
		{
			index = interInfo[i].dwFace;
			fMin  = interInfo[i].fDist ;
			No    = i;
		}
	}
	return No;
}

//=================================================================================================================
// CreateBoundCross()
// 오브젝트 내부를 교차하는 판대기 2장을 만든다... 에효....
// 마우스 픽킹을 위해서 사용된다.
// fcenterXPo : 판대기의 중점들 
// ㅇ좌표를 업데이트 하는거랑 같다.. 닝기릴 띠발
//=======================================================================================================================

BOUNDINGCROSS* CreateAndUpdateBoundCross( BOUNDINGCROSS* pBoundingCrss, float fcenterXPo, float fcenterYPo, float fcenterZPo, 
					  int nXOffset, int nYOffset , int nZOffset )
{
	
	float	m_fPoX			= fcenterXPo ;
	float	m_fPoY			= fcenterYPo ;
	float	m_fPoZ			= fcenterZPo ;
	int		m_nWidthOffset	= nXOffset ;
	int		m_nHeightOffset	= nYOffset ;
	int		m_nZOffset		= nZOffset ;

	BOUNDINGCROSS	BoundingCross;
	ZeroMemory( &BoundingCross, sizeof(BOUNDINGCROSS) );

	//===============================================================================================
	//구조체 수정 필요 Y값에 100을 더하는 이유는 대충 본중심으로 이동시키기 위해서 어림값

	BoundingCross.FrontPoly[0].p[0].x = m_fPoX- m_nWidthOffset ;
	BoundingCross.FrontPoly[0].p[0].y = m_fPoY + ( m_nHeightOffset + 100 ) ;
	BoundingCross.FrontPoly[0].p[0].z = m_fPoZ ;

	BoundingCross.FrontPoly[0].p[1].x = m_fPoX + m_nWidthOffset ;
	BoundingCross.FrontPoly[0].p[1].y = m_fPoY + ( m_nHeightOffset + 100 ) ;
	BoundingCross.FrontPoly[0].p[1].z = m_fPoZ ;

	BoundingCross.FrontPoly[0].p[2].x = m_fPoX + m_nWidthOffset ;
	BoundingCross.FrontPoly[0].p[2].y = m_fPoY + ( m_nHeightOffset - 100 ) ;
	BoundingCross.FrontPoly[0].p[2].z = m_fPoZ ;

	BoundingCross.FrontPoly[0].p[3].x = m_fPoX - m_nWidthOffset ;
	BoundingCross.FrontPoly[0].p[3].y = m_fPoY + ( m_nHeightOffset - 100 ) ;
	BoundingCross.FrontPoly[0].p[3].z = m_fPoZ ;
    //================================================================================================

	BoundingCross.FrontPoly[1].p[0].x = m_fPoX ;
	BoundingCross.FrontPoly[1].p[0].y = m_fPoY + ( 100 + m_nHeightOffset ) ;
	BoundingCross.FrontPoly[1].p[0].z = m_fPoZ - m_nZOffset ;

	BoundingCross.FrontPoly[1].p[1].x = m_fPoX ;
	BoundingCross.FrontPoly[1].p[1].y = m_fPoY + ( 100.0f + m_nHeightOffset ) ;
	BoundingCross.FrontPoly[1].p[1].z = m_fPoZ + m_nZOffset ;

	BoundingCross.FrontPoly[1].p[2].x = m_fPoX ;
	BoundingCross.FrontPoly[1].p[2].y = m_fPoY + ( 100.0f - m_nHeightOffset ) ;
	BoundingCross.FrontPoly[1].p[2].z = m_fPoZ + m_nZOffset ;

	BoundingCross.FrontPoly[1].p[3].x = m_fPoX ;
	BoundingCross.FrontPoly[1].p[3].y = m_fPoY + ( 100.0f - m_nHeightOffset ) ;
	BoundingCross.FrontPoly[1].p[3].z = m_fPoZ - m_nZOffset ;

	memcpy( pBoundingCrss, &BoundingCross, sizeof(BOUNDINGCROSS) );
	
	return pBoundingCrss;
}

//=====================================================================================================================
// BOOL RayAndSphereIntersect( D3DXVECTOR3& vO, D3DXVECTOR3& vD, D3DXVECTOR3& vC, float fr)
// 구와 레이와의 교차여부 ex) 빠른 Reject로 교차여부 확인.. 충돌지점을 챃기 위해서 즉 직선의 보간 계수 를 챃기 위해서 좀더 필요 코딩 필요
// vO : 레이의 원점 벡터 ,, vD : 직선의 노말 방향 벡터 ,, vC : 구의 원점 , fr : 구의 반지름 길이.
// 알고리즘은 리얼타임 렌더링책 295페이지 에^^
//=======================================================================================================================
BOOL RayAndSphereIntersect( D3DXVECTOR3& vO, D3DXVECTOR3& vD, D3DXVECTOR3& vC, float fr) //구와 레이의 교차여부
{
	D3DXVECTOR3 vL = vC - vO;
	
	float fd = D3DXVec3Dot( &vL, &vD ) ; //벡터 vL를  Ray의 방향벡터에 영사 시켜서
	float fl = D3DXVec3Dot( &vL, &vL );  //구의 중심과 레이의 원점 벡터로.. 벡터를 구하고 그 벡터의 길이를 구한다.
	float frSquare = fr * fr ; //자승은 square 
	
	//레이의 원점이 구안에 있는지 체크 하기 위해서
	//레이의 원점과. 구의 중심의 길이에서.. 루트 대신에 걍. 그 길이를 제곱하고 반지름을 제곱해서.
	//비교만 한다.
	if(fd<0 && fl > frSquare) return false;

	float fm = fl - fd*fd;
	//레이와..스피어는 교차되지 않는다. 
	if( fm * fm > frSquare) return false;	

	return true;
}

bool MgcTestIntersection (const D3DXVECTOR3& rkRay, const D3DXVECTOR3& vC, float fR)
{
    //MgcReal fSqrDist = MgcSqrDistance(rkSphere.Center(),rkRay);
	D3DXVECTOR3 vtemp = vC - rkRay;
	
	float fSqrDist = D3DXVec3Length( &vtemp );
    return fSqrDist <= fR;
}

FLOAT _XSphereIntersectRay( D3DXVECTOR3 &p_RayO, D3DXVECTOR3 &p_RayV, D3DXVECTOR3 &p_SO, FLOAT p_dSR )
{	
	D3DXVECTOR3 Q = p_SO-p_RayO;
	
	FLOAT c = D3DXVec3Length(&Q);
	FLOAT v = D3DXVec3Dot(&Q, &p_RayV);
	FLOAT d = (p_dSR*p_dSR) - ((c*c) - (v*v));
	
	// If there was no intersection, return -1
	if(d < 0.0)
		return (-1.0f);
	
	// Return the distance to the [first] intersecting point
	return (v - _XFC_sqrt(d));
}


//=====================================================================================================================
// BOOL RayAndSphereIntersect( D3DXVECTOR3& vO, D3DXVECTOR3& vD, D3DXVECTOR3& vC, float fr)
// 구와 레이와의 교차여부 ex) 빠른 Reject로 교차여부 확인.. 
// 그리고 구와 레이의 충돌지점을 챃기 위해서 즉 직선의 보간 계수 를 챃기 위해서 좀더 필요 코딩 필요
// vO : 레이의 원점 벡터 ,, vD : 직선의 노말 방향 벡터 ,, vC : 구의 원점 , fr : 구의 반지름 길이.
// 알고리즘은 리얼타임 렌더링책 295페이지 에^^
// ft:직선의 보간계수
//=======================================================================================================================
BOOL RayAndSphereIntersectFindPoint( D3DXVECTOR3& vO, D3DXVECTOR3& vD, D3DXVECTOR3& vC, float fr, float& ft) //구와 레이의 교차여부
{
	D3DXVECTOR3 vL = vC - vO;
	float fd = D3DXVec3Dot( &vL, &vD ) ; //벡터 vL를  Ray의 방향벡터에 영사 시켜서
	float fl = D3DXVec3Dot( &vL, &vL );  //구의 중심과 레이의 원점 벡터로.. 벡터를 구하고 그 벡터의 길이를 구한다.
	float frSquare = fr * fr ; //자승은 square 
	float flsquare = fl *fl;

	//레이의 원점이 구안에 있는지 체크 하기 위해서
	//레이의 원점과. 구의 중심의 길이에서.. 루트 대신에 걍. 그 길이를 제곱하고 반지름을 제곱해서.
	//비교만 한다.
	if(fd<0 && flsquare > frSquare) return false;

	float fm = flsquare - fd*fd;
	//레이와..스피어는 교차되지 않는다. 
	if( fm * fm > frSquare) return false; 

	//충돌 지점을 챃기 위해서.. 나중에 코딩 더 필요.. 근대.. 구랑. 교차 지점이 꼭 필요할까..???
	return true;
}
