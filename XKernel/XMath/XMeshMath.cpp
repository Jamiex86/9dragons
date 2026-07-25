//-----------------------------------------------------------------------------
// File: Math.cpp
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include "XKernel.h"

// 최단호 쿼터니언...
D3DXQUATERNION _XRotationArc( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2 )
{
	D3DXQUATERNION	q;
	D3DXVec3Normalize( vec1, vec1 );
	D3DXVec3Normalize( vec2, vec2 );

	D3DXVECTOR3 vec3;
	D3DXVec3Cross( &vec3, vec1, vec2 );
	FLOAT		d    = D3DXVec3Dot( vec1, vec2 );
	FLOAT		s    = _XFC_sqrt( (1.0f + d) * 2.0f ) ;

	q.x = vec3.x / s;
	q.y = vec3.y / s;
	q.z = vec3.z / s;

	q.w = s / 2.0f;

	return q;
}

/*
//float theta = acosf( D3DXVec3Dot( &vVec1, &vVec2 ) / ( D3DXVec3Length( &vVec1) * D3DXVec3Length( &vVec2) ) )
*/

D3DXQUATERNION _XRotationArcNormalized( D3DXVECTOR3* vec1, D3DXVECTOR3* vec2 )
{
	D3DXQUATERNION	q;
	D3DXVECTOR3 vec3;

	D3DXVec3Cross( &vec3, vec1, vec2 );
	FLOAT		d    = D3DXVec3Dot( vec1, vec2 );
	FLOAT		s    = _XFC_sqrt( (1.0 + d) * 2.0 ) ;

	q.x = vec3.x / s;
	q.y = vec3.y / s;
	q.z = vec3.z / s;

	q.w = s / 2.0f;

	return q;
}

FLOAT _XGetAngleFromVectors( D3DXVECTOR3* v1, D3DXVECTOR3* v2 )
{
	FLOAT angle  = D3DXVec3Dot( v1, v2 );
	return acosf( angle / (D3DXVec3Length(v2) * D3DXVec3Length(v1)) );
}

FLOAT  _XFastDistance(FLOAT x, FLOAT z)
{
	int ix, iz;
	_XFC_FloatToInt( &ix, x );
	_XFC_FloatToInt( &iz, z );

	ix = abs(ix);
	iz = abs(iz);
	int minvalue = MIN(ix, iz);
	return (FLOAT)( ix + iz - (minvalue>>1) - (minvalue>>2) + (minvalue>>4));
	//return((((x+y)<<4)-(mn<<3)-(mn<<2)+mn)>>4);  //오차 수정...
}

FLOAT _XDistance(FLOAT x, FLOAT z)
{	
	return _XFC_sqrt( x*x + z*z ) ;
}

void _XMeshMath_RotateCoord( FLOAT& x, FLOAT& z, FLOAT angle )
{        
   FLOAT  c = cosf( angle ), s = sinf( angle );

   FLOAT _x = x*c - z*s;
   FLOAT _z = x*s + z*c;

   x = _x;
   z = _z;
}

void _XMeshMath_XYRotateCoord( D3DXVECTOR3& vec, FLOAT sinvalue, FLOAT cosvalue )
{		
	FLOAT _x = vec.x*cosvalue - vec.y*sinvalue;
    FLOAT _y = vec.x*sinvalue + vec.y*cosvalue;
    
	vec.x = _x;
	vec.y = _y;	
}			

void _XMeshMath_XYRotateCoord( D3DXVECTOR3& vec, FLOAT angle )
{		
	FLOAT sinvalue	=	sinf( angle );
	FLOAT cosvalue	=	cosf( angle );

	FLOAT _x = vec.x*cosvalue - vec.y*sinvalue;
    FLOAT _y = vec.x*sinvalue + vec.y*cosvalue;
    
	vec.x = _x;
	vec.y = _y;	
}			

void _XMeshMath_RotateAngleXYZ( D3DXMATRIX& m, FLOAT& x, FLOAT& y, FLOAT& z)
{
     float sx = m._23;

     float cx = _XFC_sqrt(1.0f - sx * sx);

     if (cx < 0.00001f)
     {
        if(sx > 0.0f) x =  _X_PI/2;
        else	      x = -_X_PI/2;

        y = atan2f(m._31, m._11);
        z = 0.0f;
     }
     else
     {
         x = atan2f( sx, cx);		 
         y = atan2f(-m._13, m._33);
         z = atan2f(-m._21, m._22);
     }    
}


inline FLOAT _XMeshMath_SquareMagnitude (const D3DXVECTOR3& v)
{
   return v.x*v.x + v.y*v.y + v.z*v.z;
}

inline FLOAT _XMeshMath_Magnitude (const D3DXVECTOR3& v)
{
   return _XFC_sqrt(_XMeshMath_SquareMagnitude(v));
}

//-----------------------------------------------------------------------------
// Name: D3DMath_SlerpQuaternions()
// Desc: Compute a quaternion which is the spherical linear interpolation
//       between two other quaternions by dvFraction.
//-----------------------------------------------------------------------------
VOID _XMeshMath_QuaternionSlerp( D3DXQUATERNION &q, D3DXQUATERNION &a, D3DXQUATERNION &b, FLOAT fAlpha )
{
    // Compute dot product (equal to cosine of the angle between quaternions)
    FLOAT fCosTheta = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;

    // Check angle to see if quaternions are in opposite hemispheres
    if( fCosTheta < 0.0f ) 
    {
        // If so, flip one of the quaterions
        fCosTheta = -fCosTheta;

        b.x = -b.x; 
		b.y = -b.y; 
		b.z = -b.z; 
		b.w = -b.w;
    }

    // Set factors to do linear interpolation, as a special case where the
    // quaternions are close together.
    FLOAT fBeta = 1.0f - fAlpha;
    
    // If the quaternions aren't close, proceed with spherical interpolation
    if( (1.0f - fCosTheta) > 0.001f ) 
    {   
        FLOAT fTheta = acosf( fCosTheta );
        
        fBeta  = sinf( fTheta*fBeta ) / sinf( fTheta);
        fAlpha = sinf( fTheta*fAlpha ) / sinf( fTheta);
    }

    // Do the interpolation
    q.x = fBeta*a.x + fAlpha*b.x;
    q.y = fBeta*a.y + fAlpha*b.y;
    q.z = fBeta*a.z + fAlpha*b.z;
    q.w = fBeta*a.w + fAlpha*b.w;

}

//-----------------------------------------------------------------------------
// Name: Math_QuaternionFromRotation()
// Desc: 회전값을 이용해 쿼터니언을 생성..
//-----------------------------------------------------------------------------
VOID _XMeshMath_QuaternionFromRotation(D3DXQUATERNION &q, D3DXVECTOR3 &v, FLOAT &fTheta)
{
	FLOAT sinValue = sinf(fTheta/2.0f);
	q.x = sinValue * v.x;
	q.y = sinValue * v.y;
	q.z = sinValue * v.z;
	q.w = cosf(fTheta/2.0f);
}

//-----------------------------------------------------------------------------
// Name: Math_RotationFromQuaternion()
// Desc: 쿼터니언을 이용해 회전값을 생성한다..
//-----------------------------------------------------------------------------
VOID _XMeshMath_RotationFromQuaternion( D3DXVECTOR3& v, FLOAT& fTheta, D3DXQUATERNION &q)
{
    fTheta = acosf(q.w) * 2.0f;

	FLOAT sinValue = sinf(fTheta/2.0f);
    v.x    = q.x / sinValue;
    v.y    = q.y / sinValue;
    v.z    = q.z / sinValue;
}

//-----------------------------------------------------------------------------
// Name: Math_QuaternionFromAngles()
// Desc: Angles값(fYaw, fPitch, fRoll) 값을 이용해.. 쿼터니언 생성..
//-----------------------------------------------------------------------------
VOID _XMeshMath_QuaternionFromAngles( D3DXQUATERNION &q, FLOAT fYaw, FLOAT fPitch, FLOAT fRoll )
{
    FLOAT fSinYaw   = sinf( fYaw/2.0f );
    FLOAT fSinPitch = sinf( fPitch/2.0f );
    FLOAT fSinRoll  = sinf( fRoll/2.0f );
    FLOAT fCosYaw   = cosf( fYaw/2.0f );
    FLOAT fCosPitch = cosf( fPitch/2.0f );
    FLOAT fCosRoll  = cosf( fRoll/2.0f );

    q.x = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
    q.y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
    q.z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
    q.w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;
}

VOID _XMeshMath_QuaternionToEulerAngle(const D3DXQUATERNION& q, float& fYaw, float& fPitch, float& fRoll)
{
	   float sqw = q.w*q.w;
	   float sqx = q.x*q.x;
	   float sqy = q.y*q.y;
	   float sqz = q.z*q.z;
	   fPitch = asinf (2.0f * (q.w*q.x - q.y*q.z)); // rotation about x-axis
	   fYaw   = atan2f(2.0f * (q.x*q.z + q.w*q.y),(-sqx - sqy + sqz + sqw)); // rotation about y-axis
	   fRoll  = atan2f(2.0f * (q.x*q.y + q.w*q.z),(-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

//-----------------------------------------------------------------------------
// Name: Math_UnitMatrixFromMatrix()
// Desc: 메트릭스를 유닛메트릭스로 변환.. 스칼라 값이 빠지게 된다.. 4행렬은 무시
//-----------------------------------------------------------------------------
VOID _XMeshMath_UnitMatrixFromMatrix(D3DXMATRIX& m)
{
	float a,b,c;
	a = _XFC_sqrt(m._11 * m._11 + m._12 * m._12 + m._13 * m._13 );
	b = _XFC_sqrt(m._21 * m._21 + m._22 * m._22 + m._23 * m._23 );
	c = _XFC_sqrt(m._31 * m._31 + m._32 * m._32 + m._33 * m._33 );
	m._11 /= a;	m._12 /= a;	m._13 /= a;
	m._21 /= b;	m._22 /= b;	m._23 /= b;
	m._31 /= c;	m._32 /= c;	m._33 /= c;
}

//-----------------------------------------------------------------------------
// Name: Math_MatrixFromQuaternion()
// Desc: 쿼터니언을 이용해 매트릭스 생성..
//-----------------------------------------------------------------------------
VOID _XMeshMath_MatrixFromQuaternion(D3DXMATRIX& mat, D3DXQUATERNION &trans)
{
    FLOAT xx = trans.x*trans.x; FLOAT yy = trans.y*trans.y; FLOAT zz = trans.z*trans.z;
    FLOAT xy = trans.x*trans.y; FLOAT xz = trans.x*trans.z; FLOAT yz = trans.y*trans.z;
    FLOAT wx = trans.w*trans.x; FLOAT wy = trans.w*trans.y; FLOAT wz = trans.w*trans.z;

    mat._11 = 1 - 2 * ( yy + zz ); 
    mat._12 =     2 * ( xy - wz );
    mat._13 =     2 * ( xz + wy );

    mat._21 =     2 * ( xy + wz );
    mat._22 = 1 - 2 * ( xx + zz );
    mat._23 =     2 * ( yz - wx );

    mat._31 =     2 * ( xz - wy );
    mat._32 =     2 * ( yz + wx );
    mat._33 = 1 - 2 * ( xx + yy );

    mat._14 = mat._24 = mat._34 = 0.0f;
    mat._41 = mat._42 = mat._43 = 0.0f;
    mat._44 = 1.0f;
}

//-----------------------------------------------------------------------------
// Name: Math_QuaternionFromMatrix()
// Desc: 매트릭스에서 쿼터니언 축출하기..
//-----------------------------------------------------------------------------
VOID _XMeshMath_QuaternionFromMatrix( D3DXQUATERNION &q, D3DXMATRIX& mat )
{
    if( mat._11 + mat._22 + mat._33 > 0.0f )
    {
        FLOAT s = _XFC_sqrt( mat._11 + mat._22 + mat._33 + mat._44 );

        q.x = (mat._23-mat._32) / (2*s);
        q.y = (mat._31-mat._13) / (2*s);
        q.z = (mat._12-mat._21) / (2*s);
        q.w = 0.5f * s;
    }

    FLOAT xx = q.x*q.x; FLOAT yy = q.y*q.y; FLOAT zz = q.z*q.z;
    FLOAT xy = q.x*q.y; FLOAT xz = q.x*q.z; FLOAT yz = q.y*q.z;
    FLOAT wx = q.w*q.x; FLOAT wy = q.w*q.y; FLOAT wz = q.w*q.z;
    
    mat._11 = 1 - 2 * ( yy + zz ); 
    mat._12 =     2 * ( xy - wz );
    mat._13 =     2 * ( xz + wy );

    mat._21 =     2 * ( xy + wz );
    mat._22 = 1 - 2 * ( xx + zz );
    mat._23 =     2 * ( yz - wx );

    mat._31 =     2 * ( xz - wy );
    mat._32 =     2 * ( yz + wx );
    mat._33 = 1 - 2 * ( xx + yy );

    mat._14 = mat._24 = mat._34 = 0.0f;
    mat._41 = mat._42 = mat._43 = 0.0f;
    mat._44 = 1.0f;
}

//-----------------------------------------------------------------------------
// Name: Math_QuaternionMultiply()
// Desc: 쿼터니언 곱하기.. q = a*b
//-----------------------------------------------------------------------------
VOID _XMeshMath_QuaternionMultiply( D3DXQUATERNION &q, D3DXQUATERNION &a, D3DXQUATERNION &b )
{
    FLOAT Dx =  a.x*b.w + a.y*b.z - a.z*b.y + a.w*b.x;
    FLOAT Dy = -a.x*b.z + a.y*b.w + a.z*b.x + a.w*b.y;
    FLOAT Dz =  a.x*b.y - a.y*b.x + a.z*b.w + a.w*b.z;
    FLOAT Dw = -a.x*b.x - a.y*b.y - a.z*b.z + a.w*b.w;

    q.x = Dx; q.y = Dy; q.z = Dz; q.w = Dw;
}

//-----------------------------------------------------------------------------
// Name: Math_MatrixMultiply()
// Desc: 이동 행렬을 채우고 난후, 이것을 이동시키기 위해 현재 월드 행렬과 곱할때 
//		 사용할 곱셈 함수. q = a * b
//-----------------------------------------------------------------------------
VOID _XMeshMath_MatrixMultiply( D3DXMATRIX& q, D3DXMATRIX& a, D3DXMATRIX& b )
{
	FLOAT* pA = (FLOAT*)&a;
	FLOAT* pB = (FLOAT*)&b;
	FLOAT  pM[16];

	ZeroMemory( pM, sizeof(D3DXMATRIX) );

	for( WORD i=0; i<4; i++ ) 
		for( WORD j=0; j<4; j++ ) 
			for( WORD k=0; k<4; k++ ) 
				pM[4*i+j] +=  pA[4*i+k] * pB[4*k+j];

	memcpy( &q, pM, sizeof(D3DXMATRIX) );
}

//-----------------------------------------------------------------------------
// Name: Math_VectorMatrixMultiply()
// Desc: 벡터와 매트릭스의 곱셈..
//-----------------------------------------------------------------------------
HRESULT _XMeshMath_VectorMatrixMultiply( D3DXVECTOR3& vDest, D3DXVECTOR3& vSrc, D3DXMATRIX& mat)
{
    FLOAT x = vSrc.x*mat._11 + vSrc.y*mat._21 + vSrc.z* mat._31 + mat._41;
    FLOAT y = vSrc.x*mat._12 + vSrc.y*mat._22 + vSrc.z* mat._32 + mat._42;
    FLOAT z = vSrc.x*mat._13 + vSrc.y*mat._23 + vSrc.z* mat._33 + mat._43;
    FLOAT w = vSrc.x*mat._14 + vSrc.y*mat._24 + vSrc.z* mat._34 + mat._44;
    
    if( fabs( w ) < 1.0e-5f )
        return E_INVALIDARG;

    vDest.x = x/w;
    vDest.y = y/w;
    vDest.z = z/w;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Math_VertexMatrixMultiply()
// Desc: 버택스와 매트릭스의 곱셈..
//-----------------------------------------------------------------------------
HRESULT _XMeshMath_VertexMatrixMultiply( _XMESH_VERTEX& vDest, _XMESH_VERTEX& vSrc, D3DXMATRIX& mat )
{
    HRESULT    hr;
    D3DXVECTOR3* pSrcVec  = (D3DXVECTOR3*)&vSrc.x;
    D3DXVECTOR3* pDestVec = (D3DXVECTOR3*)&vDest.x;

    if( SUCCEEDED( hr = _XMeshMath_VectorMatrixMultiply( *pDestVec, *pSrcVec, mat ) ) )
    {
        //pSrcVec  = (D3DXVECTOR3*)&vSrc.nx;
        //pDestVec = (D3DXVECTOR3*)&vDest.nx;
        //hr = _XMeshMath_VectorMatrixMultiply( *pDestVec, *pSrcVec, mat );
    }
    return hr;
}

// MATRIX 값을 단위행렬로 만드는 함수..
VOID _XMeshMath_SetIdentityMatrix( D3DXMATRIX& m )
{
	m._12 = m._13 = m._14 = m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = m._34 = m._41 = m._42 = m._43 = 0.0f;
	m._11 = m._22 = m._33 = m._44 = 1.0f;
}

// z,y,z 축을 따라 이동할 (tx,ty,tz)값을 받아들이고 이동행렬을 생성하는 함수..
VOID _XMeshMath_SetTranslateMatrix( D3DXMATRIX& m, FLOAT tx, FLOAT ty, FLOAT tz )
{ 
	_XMeshMath_SetIdentityMatrix( m ); 
	m._41 = tx; m._42 = ty; m._43 = tz; 
}

VOID _XMeshMath_SetTranslateMatrix( D3DXMATRIX& m, D3DXVECTOR3& v )
{ 
	_XMeshMath_SetTranslateMatrix( m, v.x, v.y, v.z ); 
}

// 벡터를 가지고 회전 하기..
VOID _XMeshMath_RotateVector(D3DXMATRIX &m, FLOAT vAngle, D3DXVECTOR3 &vDir)
{
	FLOAT tempSin = sinf((FLOAT)_X_RAD(vAngle));
	FLOAT tempCos = cosf((FLOAT)_X_RAD(vAngle));
	
	D3DXVECTOR3 v;	
	D3DXVec3Normalize(&v, &vDir);	

	m._11 = (v.x * v.x) * (1.0f - tempCos) + tempCos;
	m._21 = (v.x * v.y) * (1.0f - tempCos) - (v.z * tempSin);
	m._31 = (v.x * v.z) * (1.0f - tempCos) + (v.y * tempSin);

	m._12 = (v.y * v.x) * (1.0f - tempCos) + (v.z * tempSin);
	m._22 = (v.y * v.y) * (1.0f - tempCos) + tempCos;
	m._32 = (v.y * v.z) * (1.0f - tempCos) - (v.x * tempSin);

	m._13 = (v.z * v.x) * (1.0f - tempCos) - (v.y * tempSin);
	m._23 = (v.z * v.y) * (1.0f - tempCos) + (v.x * tempSin);
	m._33 = (v.z * v.z) * (1.0f - tempCos) + tempCos;

	m._14 = m._24 = m._34 = 0.0f;
	m._41 = m._42 = m._43 = 0.0f;
	m._44 = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Name DistanceBetweenTwoPoint()
// Desc 점 a와 점 b 와의 거리를 구한다.. 
//////////////////////////////////////////////////////////////////////////////
FLOAT _XMeshMath_DistanceBetweenTwoPoint(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float	distance;

	distance = _XFC_sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1) );
	return distance;
}

FLOAT _XMeshMath_DistanceBetweenTwoPoint(D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	float	distance;

	distance = _XFC_sqrt( (v2.x-v1.x)*(v2.x-v1.x) + (v2.y-v1.y)*(v2.y-v1.y) + (v2.z-v1.z)*(v2.z-v1.z) );
	return distance;
}

///////////////////////////////////////////////////////////////////////////////
// Name Situation_Eye()
// Desc D3DUtil_SetViewMatrix()와 비슷한 함수이나 eye_z 가 보는 점이아니라.
// 바라보는 방향이라는 것만 다르다.. 이것은 로컬 이동을 하기 위해서 
// 작성한 함수..
//////////////////////////////////////////////////////////////////////////////
VOID _XMeshMath_SituationEye(D3DXMATRIX& m,D3DXVECTOR3& eye_sit,D3DXVECTOR3& eye_Z,D3DXVECTOR3& eye_Y)
{
	// 눈 좌표축 X를 생성
	D3DXVECTOR3 eye_X;
	D3DXVec3Cross(&eye_X, &eye_Y, &eye_Z);

	// 눈 좌표축 변환 행렬 설정
	_XMeshMath_SetIdentityMatrix(m);
	m._11 = eye_X.x;	m._21 = eye_X.y;	m._31 = eye_X.z; 
	m._12 = eye_Y.x;	m._22 = eye_Y.y;	m._32 = eye_Y.z;
	m._13 = eye_Z.x;	m._23 = eye_Z.y;	m._33 = eye_Z.z;

	// 눈 위치 값 지정 (눈 좌표축에 기준)
	m._41 = -D3DXVec3Dot(&eye_sit,&eye_X);
	m._42 = -D3DXVec3Dot(&eye_sit,&eye_Y);
	m._43 = -D3DXVec3Dot(&eye_sit,&eye_Z);
}

//-----------------------------------------------------------------------------
// Name: Math_SetViewMatrix()
// Desc: Given an eye point, a lookat point, and an up vector, this
//       function builds a 4x4 view matrix.
//-----------------------------------------------------------------------------
HRESULT _XMeshMath_SetViewMatrix( D3DXMATRIX& mat, D3DXVECTOR3& vFrom,
                            D3DXVECTOR3& vAt, D3DXVECTOR3& vWorldUp )
{
    // Get the z basis vector, which points straight ahead. This is the
    // difference from the eyepoint to the lookat point.
    D3DXVECTOR3 vView = vAt - vFrom;

    FLOAT fLength = _XMeshMath_Magnitude( vView );
    if( fLength < 1e-6f )
        return E_INVALIDARG;

    // Normalize the z basis vector
    vView /= fLength;

    // Get the dot product, and calculate the projection of the z basis
    // vector onto the up vector. The projection is the y basis vector.
    FLOAT fDotProduct = D3DXVec3Dot( &vWorldUp, &vView );

    D3DXVECTOR3 vUp = vWorldUp - fDotProduct * vView;

    // If this vector has near-zero length because the input specified a
    // bogus up vector, let's try a default up vector
    if( 1e-6f > ( fLength = _XMeshMath_Magnitude( vUp ) ) )
    {
        vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) - vView.y * vView;

        // If we still have near-zero length, resort to a different axis.
        if( 1e-6f > ( fLength = _XMeshMath_Magnitude( vUp ) ) )
        {
            vUp = D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) - vView.z * vView;

            if( 1e-6f > ( fLength = _XMeshMath_Magnitude( vUp ) ) )
                return E_INVALIDARG;
        }
    }

    // Normalize the y basis vector
    vUp /= fLength;

    // The x basis vector is found simply with the cross product of the y
    // and z basis vectors
    D3DXVECTOR3 vRight;
	D3DXVec3Cross( &vRight, &vUp, &vView );

    // Start building the matrix. The first three rows contains the basis
    // vectors used to rotate the view to point at the lookat point
    _XMeshMath_SetIdentityMatrix( mat );
    mat._11 = vRight.x;    mat._12 = vUp.x;    mat._13 = vView.x;
    mat._21 = vRight.y;    mat._22 = vUp.y;    mat._23 = vView.y;
    mat._31 = vRight.z;    mat._32 = vUp.z;    mat._33 = vView.z;

    // Do the translation values (rotations are still about the eyepoint)
    mat._41 = - D3DXVec3Dot( &vFrom, &vRight );
    mat._42 = - D3DXVec3Dot( &vFrom, &vUp );
    mat._43 = - D3DXVec3Dot( &vFrom, &vView );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Math_MatrixInvert()
// Desc: 역행렬을 구한다.: [Q] = inv[A]. Note: this function only
//       works for matrices with [0 0 0 1] for the 4th column.
//-----------------------------------------------------------------------------
HRESULT _XMeshMath_MatrixInvert( D3DXMATRIX& q, D3DXMATRIX& a )
{
    if( fabs(a._44 - 1.0f) > .001f)
        return E_INVALIDARG;
    if( fabs(a._14) > .001f || fabs(a._24) > .001f || fabs(a._34) > .001f )
        return E_INVALIDARG;

    FLOAT fDetInv = 1.0f / ( a._11 * ( a._22 * a._33 - a._23 * a._32 ) -
                             a._12 * ( a._21 * a._33 - a._23 * a._31 ) +
                             a._13 * ( a._21 * a._32 - a._22 * a._31 ) );

    q._11 =  fDetInv * ( a._22 * a._33 - a._23 * a._32 );
    q._12 = -fDetInv * ( a._12 * a._33 - a._13 * a._32 );
    q._13 =  fDetInv * ( a._12 * a._23 - a._13 * a._22 );
    q._14 = 0.0f;

    q._21 = -fDetInv * ( a._21 * a._33 - a._23 * a._31 );
    q._22 =  fDetInv * ( a._11 * a._33 - a._13 * a._31 );
    q._23 = -fDetInv * ( a._11 * a._23 - a._13 * a._21 );
    q._24 = 0.0f;

    q._31 =  fDetInv * ( a._21 * a._32 - a._22 * a._31 );
    q._32 = -fDetInv * ( a._11 * a._32 - a._12 * a._31 );
    q._33 =  fDetInv * ( a._11 * a._22 - a._12 * a._21 );
    q._34 = 0.0f;

    q._41 = -( a._41 * q._11 + a._42 * q._21 + a._43 * q._31 );
    q._42 = -( a._41 * q._12 + a._42 * q._22 + a._43 * q._32 );
    q._43 = -( a._41 * q._13 + a._42 * q._23 + a._43 * q._33 );
    q._44 = 1.0f;

    return S_OK;
}


int D3DXLineEquation(D3DXVECTOR3& vP0, D3DXVECTOR3& vP1, D3DXVECTOR3& vP2 )
{
	float fm= 0.0f, fb = 0.0f , fLineY = 0.0f;

	fm = (vP1.z - vP0.z) / (vP1.x - vP0.x); //직선의 기울기
	fb = vP1.z - ( fm * vP1.x ); //b = y-mx
	fLineY = (fm * vP2.x) + fb;

	if( fLineY < vP2.z ) return 1;		 // 비교점이 직선 보다 위에 있다.
	else if( fLineY > vP2.z )
	{
		return 2; // 비교점이 직선 보다. 아래에 있다.
	}
	else if( fLineY == vP2.z ) 
	{
		return 3; // 비교점이 직선상에 있다.
	}
	
	return 0;
}

VOID D3DXVec3Identity( D3DXVECTOR3& Vector )
{		
	FLOAT fLength = D3DXVec3Length( &Vector );	
	Vector.x /= fLength; 
	Vector.y /= fLength;
	Vector.z /= fLength;
}

int v_minus_v3 (float r[3], float u[3], float v[3])
{
  float w[3];
  int i;
  for(i=0;i<3;i++)
    w[i] = u[i] - v[i];
  for(i=0;i<3;i++)
    r[i] = w[i];

  return(1);
}

int c_mult_v3 (float r[3], float c, float u[3])
{
  int i;
  for(i=0;i<3;i++)
    r[i] = c * u[i];
  return(1);
}

int ScalarMultD3DVector(D3DXVECTOR3* r, FLOAT c, D3DXVECTOR3* u)
{
	return c_mult_v3((float*)r, c, (float*)u);
}

int D3DVectorMinusD3DVector(D3DXVECTOR3* r, D3DXVECTOR3* u, D3DXVECTOR3* v)
{
	return v_minus_v3((float*)r, (float*)u, (float*)v);
}

FLOAT Magnitude(D3DXVECTOR3* a)
{
	return _XFC_sqrt(a->x*a->x + a->y*a->y + a->z*a->z);
}

int IsZero(float r)
{
	if(fabs(r)<2e-6)
		return 1;
	return 0;
}

BOOL NormalizeD3DVectorWithLength(D3DXVECTOR3* a, FLOAT r)
{
	if(IsZero(r))
		return FALSE;
	a->x = a->x/r;
	a->y = a->y/r;
	a->z = a->z/r;
	return TRUE;
}

HRESULT SetViewMatrix( LPD3DXMATRIX mat, D3DXVECTOR3* vFrom,
					   D3DXVECTOR3* vAt, D3DXVECTOR3* vWorldUp )
{
    // Get the z basis vector, which points straight ahead. This is the
    // difference from the eyepoint to the lookat point.
    D3DXVECTOR3 vView, tempv, vUp, vRight;
    FLOAT fLength, fDotProduct;

	D3DVectorMinusD3DVector(&vView, vAt, vFrom);
	fLength = Magnitude(&vView);

    if(IsZero(fLength))
        return E_INVALIDARG;

    // Normalize the z basis vector
    NormalizeD3DVectorWithLength(&vView, fLength);

    // Get the dot product, and calculate the projection of the z basis
    // vector onto the up vector. The projection is the y basis vector.
    fDotProduct = D3DXVec3Dot( vWorldUp, &vView );
	
	ScalarMultD3DVector(&tempv, fDotProduct, &vView);
	D3DVectorMinusD3DVector(&vUp, vWorldUp, &tempv);
    // If this vector has near-zero length because the input specified a
    // bogus up vector, let's try a default up vector
	fLength = Magnitude(&vUp);
    if(IsZero(fLength))
    {
		D3DXVECTOR3 yaxis;
		yaxis.x = 0.0f; yaxis.y = 1.0f; yaxis.z = 0.0f;

		ScalarMultD3DVector(&tempv, vView.y, &vView);
		D3DVectorMinusD3DVector(&vUp, &yaxis, &tempv);

        // If we still have near-zero length, resort to a different axis.
		fLength = Magnitude(&vUp);
        if(IsZero(fLength))
        {
			D3DXVECTOR3 zaxis;
			zaxis.x = 0.0f; zaxis.y = 0.0f; zaxis.z = 1.0f;

            ScalarMultD3DVector(&tempv, vView.z, &vView);
			D3DVectorMinusD3DVector(&vUp, &zaxis, &tempv);

			fLength = Magnitude(&vUp);
            if(IsZero(fLength))
                return E_INVALIDARG;
        }
    }

    // Normalize the y basis vector
	NormalizeD3DVectorWithLength(&vUp, fLength);

    // The x basis vector is found simply with the cross product of the y
    // and z basis vectors
	D3DXVec3Cross( &vRight, &vUp, &vView );
    
    // Start building the matrix. The first three rows contains the basis
    // vectors used to rotate the view to point at the lookat point
    D3DXMatrixIdentity( mat );
    mat->_11 = vRight.x;    mat->_12 = vUp.x;    mat->_13 = vView.x;
    mat->_21 = vRight.y;    mat->_22 = vUp.y;    mat->_23 = vView.y;
    mat->_31 = vRight.z;    mat->_32 = vUp.z;    mat->_33 = vView.z;

    // Do the translation values (rotations are still about the eyepoint)
    mat->_41 = - D3DXVec3Dot( vFrom, &vRight );
    mat->_42 = - D3DXVec3Dot( vFrom, &vUp );
    mat->_43 = - D3DXVec3Dot( vFrom, &vView );

    return S_OK;
}

VOID FindPositionOnCircle(D3DXVECTOR3& vrOrig, D3DXVECTOR3& vCurrentPo, float fRadian, float fR, int nID)
{
	//XZ 평면에서의 원의 방정식
	//(x-xc)²+ (z-zc)²= r²
	D3DXVECTOR3	v;
		
	FLOAT rad = _X_RAD( fRadian );//_X_PI / 2.0f - _X_RAD( fRadian );

	switch(nID)
	{
		case 1: // vrOrigd의 Y축을 중심으로 회전 Yaw
			v.x = fR * cosf( rad );
			v.z = fR * sinf( rad );
				
			vCurrentPo.x = v.x + vrOrig.x;
			vCurrentPo.z = v.z + vrOrig.z;
			break;
			

		case 2: // vrOrigd의 X축을 중심으로 회전 pitch			

			v.y = fR * cosf( rad );
			v.z = fR * sinf( rad );
						
			vCurrentPo.y = v.y + vrOrig.y;
			vCurrentPo.z = v.z + vrOrig.z;
			break;
	}
}

FLOAT _XMeshMath_GetRotateRadian( D3DXVECTOR3& a, D3DXVECTOR3& b )
{
	return atan2( (a.x - b.x), (a.z - b.z) );		
}

extern D3DXVECTOR3 g_vPickRayDir;
extern D3DXVECTOR3 g_vPickRayOrig;

BOOL _XIntersectTriangle( D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2 )
{
    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;
	
    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &g_vPickRayDir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
    {
        tvec = g_vPickRayOrig - v0;
    }
    else
    {
        tvec = v0 - g_vPickRayOrig;
        det = -det;
    }

    //if( det < 0.0001f ) return FALSE;

	FLOAT u, v;

    // Calculate U parameter and test bounds
    u = D3DXVec3Dot( &tvec, &pvec );
    if( u < 0.0f || u > det ) return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    v = D3DXVec3Dot( &g_vPickRayDir, &qvec );
    if( v < 0.0f || u + v > det ) return FALSE;
	
    return TRUE;
}


unsigned __int64 snapshot_rdtsc()
{
	static unsigned __int64 tick;
	static unsigned __int32 lo, hi;

	_asm _emit 0x0f
	_asm _emit 0x31
	_asm mov lo, eax
	_asm mov hi, edx

	tick = hi;
	tick <<= 32;
	tick |= lo;

	return tick;
}

