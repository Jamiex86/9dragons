#include "stdafx.h"
#include "XKernel.h"

///////////////////////////////////////////////////////////////////////////////////////
// CsuX3D_Box

#ifdef _XDWDEBUG	
//_XMeshContainer		g_AxisModel;
#endif

WORD g_OBBModelVertIndex[36] = 
{
	7, 6, 2, 2, 3, 7, // up side
	3, 2, 1, 1, 0, 3, // front side
	2, 6, 5, 5, 1, 2, // right side
	7, 3, 0, 0, 4, 7, // left side
	0, 1, 5, 5, 4, 0, // bottom side 
	6, 7, 4, 4, 5, 6  // back side
};

/*
WORD g_OBBModelVertIndex[36] = 
{
	7, 2, 6, 2, 7, 3, // up side
	3, 1, 2, 1, 3, 0, // front side
	2, 5, 6, 5, 2, 1, // right side
	7, 0, 3, 0, 7, 4, // left side
	0, 5, 1, 5, 0, 4, // bottom side 
	6, 4, 7, 4, 6, 5  // back side
};*/

CsuX3D_Box::CsuX3D_Box()
{
	memset( this, 0, sizeof(CsuX3D_Box) );    
}


// 외부에서 나온 결과값을 단지 이 클래스에서는 한곳에 관 리하도록 해줍니다. 
void CsuX3D_Box::Create(D3DXVECTOR3 &Center,D3DXVECTOR3 Axis[3],float Extent[3])
{
	// 박스의 중심 
	m_kCenter =  Center;

	// 회전 축 
	m_akAxis[0]  =  Axis[0]; // x.x x.y x.z
	m_akAxis[1]  =  Axis[1]; // y.x y.y y.z 
	m_akAxis[2]  =  Axis[2]; // z.x z.y z.z

	// Extent 값은  Max값에서 Center를 뺀것이 됩니다. 
	m_afExtent[0] = Extent[0];	
	m_afExtent[1] = Extent[1];
	m_afExtent[2] = Extent[2];
}

D3DXVECTOR3& CsuX3D_Box::Center ()
{
    return m_kCenter;
}

void CsuX3D_Box::SetCenter (D3DXVECTOR3 &Center)
{
    m_kCenter = Center;
}


D3DXVECTOR3* CsuX3D_Box::Axis ()
{
    return m_akAxis;
}

void CsuX3D_Box::SetAxis (D3DXVECTOR3 Axis[3])
{
    m_akAxis[0] = Axis[0];
	m_akAxis[1] = Axis[1];
	m_akAxis[2] = Axis[2];
}

float * CsuX3D_Box::Extents ()
{
    return m_afExtent;
}

void CsuX3D_Box::SetExtents (D3DXVECTOR3 &Extent)
{
    m_afExtent[0] = Extent.x;	
	m_afExtent[1] = Extent.y;	
	m_afExtent[2] = Extent.z;	

}

D3DXVECTOR3  CsuX3D_Box::Min()
{
    return m_kMin;
}

D3DXVECTOR3  CsuX3D_Box::Max()
{
    return m_kMax;
}

D3DXVECTOR3* CsuX3D_Box::Vertices () 
{
	D3DXVECTOR3 tempakEAxis[3];

	tempakEAxis[0] = m_afExtent[0]*m_akAxis[0];
	tempakEAxis[1] = m_afExtent[1]*m_akAxis[1];
	tempakEAxis[2] = m_afExtent[2]*m_akAxis[2];

	
	akVertex[0] = m_kCenter - tempakEAxis[0] - tempakEAxis[1] - tempakEAxis[2];
    akVertex[1] = m_kCenter + tempakEAxis[0] - tempakEAxis[1] - tempakEAxis[2];
    akVertex[2] = m_kCenter + tempakEAxis[0] + tempakEAxis[1] - tempakEAxis[2];
    akVertex[3] = m_kCenter - tempakEAxis[0] + tempakEAxis[1] - tempakEAxis[2];
    akVertex[4] = m_kCenter - tempakEAxis[0] - tempakEAxis[1] + tempakEAxis[2];
    akVertex[5] = m_kCenter + tempakEAxis[0] - tempakEAxis[1] + tempakEAxis[2];
    akVertex[6] = m_kCenter + tempakEAxis[0] + tempakEAxis[1] + tempakEAxis[2];
    akVertex[7] = m_kCenter - tempakEAxis[0] + tempakEAxis[1] + tempakEAxis[2];

	return akVertex;
}

void CsuX3D_Box::Setapex( D3DXVECTOR3* pv)
{
	int i = 0  ; for( i=0; i<8; i++)
	{
		m_vapex[i].x = pv->x;
		m_vapex[i].y = pv->y;
		m_vapex[i].z = pv->z;
		pv++;
	}
}

void CsuX3D_Box::ComputeVertices(D3DXVECTOR3 *Vertex)
{
	
	D3DXVECTOR3 akEAxis[3] =
	{
		m_afExtent[0]*m_akAxis[0],
		m_afExtent[1]*m_akAxis[1],
		m_afExtent[2]*m_akAxis[2]
	};
	
	Vertex[0] = m_kCenter - akEAxis[0] - akEAxis[1] - akEAxis[2];
	Vertex[1] = m_kCenter + akEAxis[0] - akEAxis[1] - akEAxis[2];
	Vertex[2] = m_kCenter + akEAxis[0] + akEAxis[1] - akEAxis[2];
	Vertex[3] = m_kCenter - akEAxis[0] + akEAxis[1] - akEAxis[2];
	Vertex[4] = m_kCenter - akEAxis[0] - akEAxis[1] + akEAxis[2];
	Vertex[5] =	m_kCenter + akEAxis[0] - akEAxis[1] + akEAxis[2];
	Vertex[6] =	m_kCenter + akEAxis[0] + akEAxis[1] + akEAxis[2];
	Vertex[7] =	m_kCenter - akEAxis[0] + akEAxis[1] + akEAxis[2];
}

//=====================================================================
// BOOL InterOBBvsOBB(const CX3D_Box& rkBox)
// OBB vs OBB 충돌 체크-
//=====================================================================
BOOL CsuX3D_Box::InterOBBvsOBB(CsuX3D_Box& rkBox)
{		
    D3DXVECTOR3* akA = rkBox.Axis();
    D3DXVECTOR3* akB = m_akAxis;
    const float* afEA = rkBox.Extents();
    const float* afEB = m_afExtent;

    // compute difference of box centers, D = C1-C0
	D3DXVECTOR3 kD = rkBox.Center() - m_kCenter;

    float aafC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    float aafAbsC[3][3];  // |c_{ij}|
    float afAD[3];        // Dot(A_i,D)
    float fR0, fR1, fR;   // interval radii and distance between centers
    float fR01;           // = R0 + R1
    
    // axis C0+t*A0
	aafC[0][0] = D3DXVec3Dot( &akA[0], &akB[0] );
    aafC[0][1] = D3DXVec3Dot( &akA[0], &akB[1] );
    aafC[0][2] = D3DXVec3Dot( &akA[0], &akB[2] );
	//t Dot l = afAD[0]
	// t = 두 박스간의 벡터 , l 은 A박사의 X축 
    afAD[0] = D3DXVec3Dot( &akA[0], &kD );//akA[0].Dot(kD);  분활축 같은대..
    aafAbsC[0][0]	= (float)fabs(aafC[0][0]);
    aafAbsC[0][1]	= (float)fabs(aafC[0][1]);
    aafAbsC[0][2]	= (float)fabs(aafC[0][2]);
    fR				= (float)fabs(afAD[0]);
    fR1 = afEB[0]*aafAbsC[0][0]+afEB[1]*aafAbsC[0][1]+afEB[2]*aafAbsC[0][2];
    fR01 = afEA[0] + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1
    aafC[1][0]	= D3DXVec3Dot( &akA[1], &akB[0] );
    aafC[1][1]	= D3DXVec3Dot( &akA[1], &akB[1] );
    aafC[1][2]	= D3DXVec3Dot( &akA[1], &akB[2] );
    afAD[1]		= D3DXVec3Dot( &akA[1], &kD );	

    aafAbsC[1][0]	= ( float )fabs(aafC[1][0]);
    aafAbsC[1][1]	= ( float )fabs(aafC[1][1]);
    aafAbsC[1][2]	= ( float )fabs(aafC[1][2]);
    fR				= ( float )fabs(afAD[1]);

    fR1 = afEB[0]*aafAbsC[1][0]+afEB[1]*aafAbsC[1][1]+afEB[2]*aafAbsC[1][2];
    fR01 = afEA[1] + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2
    aafC[2][0]	= D3DXVec3Dot( &akA[2], &akB[0]); 
    aafC[2][1]	= D3DXVec3Dot( &akA[2], &akB[1]); 
    aafC[2][2]	= D3DXVec3Dot( &akA[2], &akB[2]); 
    afAD[2]		= D3DXVec3Dot( &akA[2],  &kD ); 

    aafAbsC[2][0] = (float)fabs(aafC[2][0]); 
    aafAbsC[2][1] = (float)fabs(aafC[2][1]); 
    aafAbsC[2][2] = (float)fabs(aafC[2][2]); 
    fR = (float)fabs(afAD[2]); 
	
    fR1 = afEB[0]*aafAbsC[2][0]+afEB[1]*aafAbsC[2][1]+afEB[2]*aafAbsC[2][2];
    fR01 = afEA[2] + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*B0
    fR = (float)fabs(D3DXVec3Dot(&akB[0], &kD ) ); 
    fR0 = afEA[0]*aafAbsC[0][0]+afEA[1]*aafAbsC[1][0]+afEA[2]*aafAbsC[2][0];
    fR01 = fR0 + afEB[0];
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*B1
    fR = (float)fabs(D3DXVec3Dot( &akB[1], &kD ) ); //akB[1].Dot(kD));
    fR0 = afEA[0]*aafAbsC[0][1]+afEA[1]*aafAbsC[1][1]+afEA[2]*aafAbsC[2][1];
    fR01 = fR0 + afEB[1];
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*B2
    fR = (float)fabs(D3DXVec3Dot( &akB[2], &kD ) ); //MgcMath::Abs(akB[2].Dot(kD));
    fR0 = afEA[0]*aafAbsC[0][2]+afEA[1]*aafAbsC[1][2]+afEA[2]*aafAbsC[2][2];
    fR01 = fR0 + afEB[2];
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A0xB0
    fR = (float)fabs(afAD[2]*aafC[1][0] - afAD[1]*aafC[2][0]); 
    fR0 = afEA[1]*aafAbsC[2][0] + afEA[2]*aafAbsC[1][0];
    fR1 = afEB[1]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A0xB1
    fR = (float)fabs( afAD[2]*aafC[1][1] - afAD[1]*aafC[2][1]); 
    fR0 = afEA[1]*aafAbsC[2][1] + afEA[2]*aafAbsC[1][1];
    fR1 = afEB[0]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A0xB2
    fR = (float)fabs(afAD[2]*aafC[1][2] - afAD[1]*aafC[2][2]); 
    fR0 = afEA[1]*aafAbsC[2][2] + afEA[2]*aafAbsC[1][2];
    fR1 = afEB[0]*aafAbsC[0][1] + afEB[1]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1xB0
    fR = (float)fabs(afAD[0]*aafC[2][0] - afAD[2]*aafC[0][0]); 
    fR0 = afEA[0]*aafAbsC[2][0] + afEA[2]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1xB1
    fR = (float)fabs(afAD[0]*aafC[2][1] - afAD[2]*aafC[0][1]); 
    fR0 = afEA[0]*aafAbsC[2][1] + afEA[2]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1xB2
    fR = (float)fabs(afAD[0]*aafC[2][2] - afAD[2] * aafC[0][2] ); 
    fR0 = afEA[0]*aafAbsC[2][2] + afEA[2]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[1][1] + afEB[1]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2xB0
    fR = (float)fabs(afAD[1]*aafC[0][0] - afAD[0]*aafC[1][0] ); 
    fR0 = afEA[0]*aafAbsC[1][0] + afEA[1]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2xB1
    fR = (float)fabs(afAD[1]*aafC[0][1] - afAD[0]*aafC[1][1]); 
    fR0 = afEA[0]*aafAbsC[1][1] + afEA[1]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2xB2
    fR = (float)fabs(afAD[1]*aafC[0][2] - afAD[0]*aafC[1][2]); 
    fR0 = afEA[0]*aafAbsC[1][2] + afEA[1]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[2][1] + afEB[1]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    return TRUE;

}


///////////////////////////////////////////////////////////////////////////////////////
// CSuModelBox

CSuModelBox::CSuModelBox()
{
	m_pVertex = NULL;
}
CSuModelBox::~CSuModelBox()
{
	SAFE_DELETE_ARRAY( m_pVertex );
}

void CSuModelBox::InitBox( D3DXVECTOR3* pVertexs, D3DXVECTOR3& vCenter  )
{	
	D3DXVECTOR3* pV = pVertexs;
	D3DXVECTOR3  vTemp[8];
	if(m_pVertex != NULL) SAFE_DELETE_ARRAY(m_pVertex);
	m_pVertex = new BOXVERTEX[24];
	
	int i = 0  ; for( i=0 ; i<8; i++)
	{
		vTemp[i].x = pV->x;
		vTemp[i].y = pV->y;
		vTemp[i].z = pV->z;
		pV++;
	}
	m_pVertex[0].p  = vTemp[0];		m_pVertex[1].p  = vTemp[1];
	m_pVertex[2].p  = vTemp[1];		m_pVertex[3].p  = vTemp[2];
	m_pVertex[4].p  = vTemp[2];		m_pVertex[5].p  = vTemp[3];
	m_pVertex[6].p  = vTemp[3];		m_pVertex[7].p  = vTemp[0];
	m_pVertex[8].p  = vTemp[0];		m_pVertex[9].p  = vTemp[4];
	m_pVertex[10].p = vTemp[4];		m_pVertex[11].p = vTemp[7];
	m_pVertex[12].p = vTemp[7];		m_pVertex[13].p = vTemp[3];
	m_pVertex[14].p = vTemp[7];		m_pVertex[15].p = vTemp[6];
	m_pVertex[16].p = vTemp[6];		m_pVertex[17].p = vTemp[5];
	m_pVertex[18].p = vTemp[5];		m_pVertex[19].p = vTemp[1];
	m_pVertex[20].p = vTemp[5];		m_pVertex[21].p = vTemp[4];
	m_pVertex[22].p = vTemp[2];		m_pVertex[23].p = vTemp[6];

	for(int ii=0; ii<24; ii++)
	{
		m_pVertex[ii].color = 0xffF4081E;
	}
	m_vCenter = vCenter;
}

void CSuModelBox::Render(D3DCOLOR color )
{
	if(!m_pVertex) return;

	for(int ii=0; ii<24; ii++)
	{
		m_pVertex[ii].color = color;
	}	
	
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	gpDev->SetTexture(0, NULL); 		
	gpDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	gpDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	gpDev->SetFVF( D3DFVF_BOXVERTEX ); 
	gpDev->DrawPrimitiveUP(D3DPT_LINELIST, 12, m_pVertex, sizeof(BOXVERTEX));
	//gpDev->SetRenderState(D3DRS_FOGENABLE, g_LodTerrain.m_Fog );
}

void CSuModelBox::Render( D3DMATRIX& mat, D3DXVECTOR3* pAxis, D3DCOLOR color )
{
	gpDev->SetTransform( D3DTS_WORLD, &mat );
	Render( color );
	
/*	
	D3DXMATRIX obbmat, rotmat;
	D3DXMatrixTranslation( &obbmat, m_vCenter.x, m_vCenter.y, m_vCenter.z  );
	D3DXMatrixIdentity( &rotmat );
	
	rotmat._11 = pAxis[0].x;
	rotmat._12 = pAxis[0].y;
	rotmat._13 = pAxis[0].z;
	rotmat._21 = pAxis[1].x;
	rotmat._22 = pAxis[1].y;
	rotmat._23 = pAxis[1].z;
	rotmat._31 = pAxis[2].x;
	rotmat._32 = pAxis[2].y;
	rotmat._33 = pAxis[2].z;
	
	D3DXMatrixMultiply( &obbmat, &rotmat, &obbmat );

	gpDev->SetTransform( D3DTS_WORLD, &obbmat );
	//gpDev->SetRenderState( D3DRS_ZENABLE,   FALSE );
	gpDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	gpDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	gpDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
#ifdef _XDWDEBUG	
	g_AxisModel.Render( obbmat );
#endif
	//gpDev->SetRenderState( D3DRS_ZENABLE,   g_RS_UseZBuffer );
	*/
}


_XOBBData::_XOBBData()
{
	//memset( this, 0, sizeof(_XOBBData) );
	m_Radius	= 2.0f;
	m_OBBLevel	= 0;
	m_bCollidable = TRUE;
#ifdef _XDWDEBUG
	m_Collide	= FALSE;
#endif
}


// 외부에서 나온 결과값을 단지 이 클래스에서는 한곳에 관 리하도록 해줍니다. 
void _XOBBData::Create(D3DXVECTOR3 &Center,D3DXVECTOR3 Axis[3],float Extent[3])
{
		// 박스의 중심 
		m_kCenter =  Center;

		// 회전 축 
		m_akAxis[0]  =  Axis[0]; // x.x x.y x.z
		m_akAxis[1]  =  Axis[1]; // y.x y.y y.z 
		m_akAxis[2]  =  Axis[2]; // z.x z.y z.z

		// Extent 값은  Max값에서 Center를 뺀것이 됩니다. 
		m_afExtent[0] = Extent[0];	
		m_afExtent[1] = Extent[1];
		m_afExtent[2] = Extent[2];

		m_bCollidable = TRUE;
}

D3DXVECTOR3& _XOBBData::Center ()
{
    return m_kCenter;
}

void _XOBBData::SetCenter (D3DXVECTOR3 &Center)
{
    m_kCenter = Center;
}


D3DXVECTOR3* _XOBBData::Axis ()
{
    return m_akAxis;
}

void _XOBBData::SetAxis (D3DXVECTOR3 Axis[3])
{
    m_akAxis[0] = Axis[0];
	m_akAxis[1] = Axis[1];
	m_akAxis[2] = Axis[2];
}

float * _XOBBData::Extents ()
{
    return m_afExtent;
}

void _XOBBData::SetExtents (D3DXVECTOR3 &Extent)
{
    m_afExtent[0] = Extent.x;	
	m_afExtent[1] = Extent.y;	
	m_afExtent[2] = Extent.z;	

}

void _XOBBData::RecalcRadius( void )
{
	/*
	D3DXVECTOR3 tempakEAxis[3], akVertex[8];
	tempakEAxis[0] = m_afExtent[0]*m_akAxis[0];
	tempakEAxis[1] = m_afExtent[1]*m_akAxis[1];
	tempakEAxis[2] = m_afExtent[2]*m_akAxis[2];
	
	akVertex[0] = m_kCenter - tempakEAxis[0] - tempakEAxis[1] - tempakEAxis[2];
    akVertex[1] = m_kCenter + tempakEAxis[0] - tempakEAxis[1] - tempakEAxis[2];
    akVertex[2] = m_kCenter + tempakEAxis[0] + tempakEAxis[1] - tempakEAxis[2];
    akVertex[3] = m_kCenter - tempakEAxis[0] + tempakEAxis[1] - tempakEAxis[2];
    akVertex[4] = m_kCenter - tempakEAxis[0] - tempakEAxis[1] + tempakEAxis[2];
    akVertex[5] = m_kCenter + tempakEAxis[0] - tempakEAxis[1] + tempakEAxis[2];
    akVertex[6] = m_kCenter + tempakEAxis[0] + tempakEAxis[1] + tempakEAxis[2];
    akVertex[7] = m_kCenter - tempakEAxis[0] + tempakEAxis[1] + tempakEAxis[2];
	
	m_Radius = 0.0f;
	FLOAT dist;
	
	int i = 0 ; for(  i = 0; i < 8; i++ )
	{
		FLOAT dx = fabs(akVertex[i].x-m_kCenter.x);
		FLOAT dy = fabs(akVertex[i].y-m_kCenter.y);
		FLOAT dz = fabs(akVertex[i].z-m_kCenter.z);
		dist = dx*dx + dy*dy + dz*dz;
		if( dist > m_Radius ) m_Radius = dist;
	}
	*/

	m_Radius = /*_XFC_sqrt(*/ (m_afExtent[0] * m_afExtent[0]) + (m_afExtent[1] * m_afExtent[1]) + (m_afExtent[2] * m_afExtent[2]);
}

#ifdef _XDWDEBUG

D3DXVECTOR3* _XOBBData::Vertices () 
{
	D3DXVECTOR3 tempakEAxis[3];
	tempakEAxis[0] = m_afExtent[0]*m_akAxis[0];  //Right X축
	tempakEAxis[1] = m_afExtent[1]*m_akAxis[1];		//
	tempakEAxis[2] = m_afExtent[2]*m_akAxis[2];
	
	m_akVertex[0] = m_kCenter - tempakEAxis[0] - tempakEAxis[1] - tempakEAxis[2];
    m_akVertex[1] = m_kCenter + tempakEAxis[0] - tempakEAxis[1] - tempakEAxis[2];
    m_akVertex[2] = m_kCenter + tempakEAxis[0] + tempakEAxis[1] - tempakEAxis[2];
    m_akVertex[3] = m_kCenter - tempakEAxis[0] + tempakEAxis[1] - tempakEAxis[2];
    m_akVertex[4] = m_kCenter - tempakEAxis[0] - tempakEAxis[1] + tempakEAxis[2];
    m_akVertex[5] = m_kCenter + tempakEAxis[0] - tempakEAxis[1] + tempakEAxis[2];
    m_akVertex[6] = m_kCenter + tempakEAxis[0] + tempakEAxis[1] + tempakEAxis[2];
    m_akVertex[7] = m_kCenter - tempakEAxis[0] + tempakEAxis[1] + tempakEAxis[2];
	return m_akVertex;
}

void _XOBBData::CreateOBBModel( void )
{
	m_OBBModel.InitBox( Vertices(), m_kCenter );
}

#endif

void _XOBBData::CreateModelVertex( void )
{
	/*
	const static D3DXVECTOR3 _BoxModelVertex[8] = 
	{
		D3DXVECTOR3( -1, -1, -1 ),
		D3DXVECTOR3(  1, -1, -1 ),
		D3DXVECTOR3(  1,  1, -1 ),
		D3DXVECTOR3( -1,  1, -1 ),
		D3DXVECTOR3( -1, -1,  1 ),
		D3DXVECTOR3(  1, -1,  1 ),
		D3DXVECTOR3(  1,  1,  1 ),
		D3DXVECTOR3( -1,  1,  1 )
	};	
	D3DXMATRIX obbmat, transmat, rotmat, scalmat;
	D3DXMatrixTranslation( &transmat, m_kCenter.x, m_kCenter.y, m_kCenter.z  );
	D3DXMatrixScaling( &scalmat, m_afExtent[0], m_afExtent[1], m_afExtent[2] );
	D3DXMatrixIdentity( &rotmat );	
	rotmat._11 = m_akAxis[0].x;
	rotmat._12 = m_akAxis[0].y;
	rotmat._13 = m_akAxis[0].z;
	rotmat._21 = m_akAxis[1].x;
	rotmat._22 = m_akAxis[1].y;
	rotmat._23 = m_akAxis[1].z;
	rotmat._31 = m_akAxis[2].x;
	rotmat._32 = m_akAxis[2].y;
	rotmat._33 = m_akAxis[2].z;	
	
	D3DXMatrixMultiply( &obbmat, &scalmat, &rotmat );
	obbmat._41 += m_kCenter.x;
	obbmat._42 += m_kCenter.y;
	obbmat._43 += m_kCenter.z;	

	int i = 0 ; for(  i = 0; i < 8; i++ )
	{
		D3DXVec3TransformCoord( &m_ModelVertex[i], &_BoxModelVertex[i], &obbmat );		
	}
	*/
	
	D3DXVECTOR3 akEAxis[3] =
	{
		m_afExtent[0]*m_akAxis[0],
		m_afExtent[1]*m_akAxis[1],
		m_afExtent[2]*m_akAxis[2]
	};		
	
	m_ModelVertex[0] =  m_kCenter - akEAxis[0] - akEAxis[1] - akEAxis[2];
	m_ModelVertex[1] =  m_kCenter + akEAxis[0] - akEAxis[1] - akEAxis[2];
	m_ModelVertex[2] =  m_kCenter + akEAxis[0] + akEAxis[1] - akEAxis[2];
	m_ModelVertex[3] =  m_kCenter - akEAxis[0] + akEAxis[1] - akEAxis[2];
	m_ModelVertex[4] =  m_kCenter - akEAxis[0] - akEAxis[1] + akEAxis[2];
	m_ModelVertex[5] =	m_kCenter + akEAxis[0] - akEAxis[1] + akEAxis[2];
	m_ModelVertex[6] =	m_kCenter + akEAxis[0] + akEAxis[1] + akEAxis[2];
	m_ModelVertex[7] =	m_kCenter - akEAxis[0] + akEAxis[1] + akEAxis[2];
}

BOOL _XOBBData::Load( FILE* FilePtr )
{
	if( !FilePtr )
	{
		_XFatalError( "Load OBB / Invalid file handler" );
		return FALSE;
	}

	_XOBB_SaveFormat saveobbformat;

	if( fread( &saveobbformat, sizeof(_XOBB_SaveFormat), 1, FilePtr ) < 1 )
	{
		_XFatalError( "Load obb structure" );
		return FALSE;
	}

	m_kCenter = saveobbformat.kCenter;
	memcpy( m_akAxis, saveobbformat.akAxis, sizeof(D3DXVECTOR3) * 3 );
	memcpy( m_afExtent, saveobbformat.afExtent, sizeof(FLOAT) * 3 );
	m_OBBLevel = saveobbformat.OBBLevel;

	RecalcRadius();

	return TRUE;
}

BOOL _XOBBData::Save( FILE* FilePtr )
{
	if( !FilePtr )
	{ 		
		_XFatalError( "Save OBB / Invalid file handler" );
		return FALSE;
	}

	_XOBB_SaveFormat saveobbformat;

	saveobbformat.kCenter = m_kCenter;
	memcpy( saveobbformat.akAxis, m_akAxis, sizeof(D3DXVECTOR3) * 3 );
	memcpy( saveobbformat.afExtent, m_afExtent, sizeof(FLOAT) * 3 );
	saveobbformat.xcoord = 0;
	saveobbformat.zcoord = 0;
	saveobbformat.OBBLevel = m_OBBLevel;

	if( fwrite( &saveobbformat, sizeof(_XOBB_SaveFormat), 1, FilePtr ) < 1 )
	{
		_XFatalError( "Save obb structure" );
		return FALSE;
	}

	return TRUE;
}

//=====================================================================
// BOOL InterOBBvsOBB(const CX3D_Box& rkBox)
// OBB vs OBB 충돌 체크-
//=====================================================================
BOOL _XOBBData::InterOBBvsOBB(_XOBBData& rkBox)
{
	// convenience variables	
    D3DXVECTOR3* akA = rkBox.Axis();
    D3DXVECTOR3* akB = m_akAxis;
    const float* afEA = rkBox.Extents();
    const float* afEB = m_afExtent;

    // compute difference of box centers, D = C1-C0
	D3DXVECTOR3 kD = rkBox.Center() - m_kCenter;

    float aafC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    float aafAbsC[3][3];  // |c_{ij}|
    float afAD[3];        // Dot(A_i,D)
    float fR0, fR1, fR;   // interval radii and distance between centers
    float fR01;           // = R0 + R1
    
    // axis C0+t*A0
	aafC[0][0] = D3DXVec3Dot( &akA[0], &akB[0] );
    aafC[0][1] = D3DXVec3Dot( &akA[0], &akB[1] );
    aafC[0][2] = D3DXVec3Dot( &akA[0], &akB[2] );
	//t Dot l = afAD[0]
	// t = 두 박스간의 벡터 , l 은 A박사의 X축 
    afAD[0] = D3DXVec3Dot( &akA[0], &kD );//akA[0].Dot(kD);  분활축 같은대..
    aafAbsC[0][0]	= (float)fabs(aafC[0][0]);
    aafAbsC[0][1]	= (float)fabs(aafC[0][1]);
    aafAbsC[0][2]	= (float)fabs(aafC[0][2]);
    fR				= (float)fabs(afAD[0]);
    fR1 = afEB[0]*aafAbsC[0][0]+afEB[1]*aafAbsC[0][1]+afEB[2]*aafAbsC[0][2];
    fR01 = afEA[0] + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1
    aafC[1][0]	= D3DXVec3Dot( &akA[1], &akB[0] );
    aafC[1][1]	= D3DXVec3Dot( &akA[1], &akB[1] );
    aafC[1][2]	= D3DXVec3Dot( &akA[1], &akB[2] );
    afAD[1]		= D3DXVec3Dot( &akA[1], &kD );	

    aafAbsC[1][0]	= ( float )fabs(aafC[1][0]);
    aafAbsC[1][1]	= ( float )fabs(aafC[1][1]);
    aafAbsC[1][2]	= ( float )fabs(aafC[1][2]);
    fR				= ( float )fabs(afAD[1]);

    fR1 = afEB[0]*aafAbsC[1][0]+afEB[1]*aafAbsC[1][1]+afEB[2]*aafAbsC[1][2];
    fR01 = afEA[1] + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2
    aafC[2][0]	= D3DXVec3Dot( &akA[2], &akB[0]); 
    aafC[2][1]	= D3DXVec3Dot( &akA[2], &akB[1]); 
    aafC[2][2]	= D3DXVec3Dot( &akA[2], &akB[2]); 
    afAD[2]		= D3DXVec3Dot( &akA[2],  &kD ); 

    aafAbsC[2][0] = (float)fabs(aafC[2][0]); 
    aafAbsC[2][1] = (float)fabs(aafC[2][1]); 
    aafAbsC[2][2] = (float)fabs(aafC[2][2]); 
    fR = (float)fabs(afAD[2]); 
	
    fR1 = afEB[0]*aafAbsC[2][0]+afEB[1]*aafAbsC[2][1]+afEB[2]*aafAbsC[2][2];
    fR01 = afEA[2] + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*B0
    fR = (float)fabs(D3DXVec3Dot(&akB[0], &kD ) ); 
    fR0 = afEA[0]*aafAbsC[0][0]+afEA[1]*aafAbsC[1][0]+afEA[2]*aafAbsC[2][0];
    fR01 = fR0 + afEB[0];
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*B1
    fR = (float)fabs(D3DXVec3Dot( &akB[1], &kD ) ); //akB[1].Dot(kD));
    fR0 = afEA[0]*aafAbsC[0][1]+afEA[1]*aafAbsC[1][1]+afEA[2]*aafAbsC[2][1];
    fR01 = fR0 + afEB[1];
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*B2
    fR = (float)fabs(D3DXVec3Dot( &akB[2], &kD ) ); //MgcMath::Abs(akB[2].Dot(kD));
    fR0 = afEA[0]*aafAbsC[0][2]+afEA[1]*aafAbsC[1][2]+afEA[2]*aafAbsC[2][2];
    fR01 = fR0 + afEB[2];
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A0xB0
    fR = (float)fabs(afAD[2]*aafC[1][0] - afAD[1]*aafC[2][0]); 
    fR0 = afEA[1]*aafAbsC[2][0] + afEA[2]*aafAbsC[1][0];
    fR1 = afEB[1]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A0xB1
    fR = (float)fabs( afAD[2]*aafC[1][1] - afAD[1]*aafC[2][1]); 
    fR0 = afEA[1]*aafAbsC[2][1] + afEA[2]*aafAbsC[1][1];
    fR1 = afEB[0]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A0xB2
    fR = (float)fabs(afAD[2]*aafC[1][2] - afAD[1]*aafC[2][2]); 
    fR0 = afEA[1]*aafAbsC[2][2] + afEA[2]*aafAbsC[1][2];
    fR1 = afEB[0]*aafAbsC[0][1] + afEB[1]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1xB0
    fR = (float)fabs(afAD[0]*aafC[2][0] - afAD[2]*aafC[0][0]); 
    fR0 = afEA[0]*aafAbsC[2][0] + afEA[2]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1xB1
    fR = (float)fabs(afAD[0]*aafC[2][1] - afAD[2]*aafC[0][1]); 
    fR0 = afEA[0]*aafAbsC[2][1] + afEA[2]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1xB2
    fR = (float)fabs(afAD[0]*aafC[2][2] - afAD[2] * aafC[0][2] ); 
    fR0 = afEA[0]*aafAbsC[2][2] + afEA[2]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[1][1] + afEB[1]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2xB0
    fR = (float)fabs(afAD[1]*aafC[0][0] - afAD[0]*aafC[1][0] ); 
    fR0 = afEA[0]*aafAbsC[1][0] + afEA[1]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2xB1
    fR = (float)fabs(afAD[1]*aafC[0][1] - afAD[0]*aafC[1][1]); 
    fR0 = afEA[0]*aafAbsC[1][1] + afEA[1]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2xB2
    fR = (float)fabs(afAD[1]*aafC[0][2] - afAD[0]*aafC[1][2]); 
    fR0 = afEA[0]*aafAbsC[1][2] + afEA[1]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[2][1] + afEB[1]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    return TRUE;
}

int _XOBBData::InterOBBvsOBB(_XOBBData& rkBox, D3DXVECTOR3& offset )
{
	// convenience variables	
    D3DXVECTOR3* akA = rkBox.Axis();
    D3DXVECTOR3* akB = m_akAxis;
    const float* afEA = rkBox.Extents();
    const float* afEB = m_afExtent;

	D3DXVECTOR3 newcenter;
	newcenter = offset + rkBox.Center();

    // compute difference of box centers, D = C1-C0
	D3DXVECTOR3 kD = newcenter - m_kCenter;

    float aafC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    float aafAbsC[3][3];  // |c_{ij}|
    float afAD[3];        // Dot(A_i,D)
    float fR0, fR1, fR;   // interval radii and distance between centers
    float fR01;           // = R0 + R1
    
    // axis C0+t*A0
	aafC[0][0] = D3DXVec3Dot( &akA[0], &akB[0] );
    aafC[0][1] = D3DXVec3Dot( &akA[0], &akB[1] );
    aafC[0][2] = D3DXVec3Dot( &akA[0], &akB[2] );
	//t Dot l = afAD[0]
	// t = 두 박스간의 벡터 , l 은 A박스의 X축 
    afAD[0] = D3DXVec3Dot( &akA[0], &kD );//akA[0].Dot(kD);  분활축 같은대..
    aafAbsC[0][0]	= (float)fabs(aafC[0][0]);
    aafAbsC[0][1]	= (float)fabs(aafC[0][1]);
    aafAbsC[0][2]	= (float)fabs(aafC[0][2]);
    fR				= (float)fabs(afAD[0]);
    fR1 = afEB[0]*aafAbsC[0][0]+afEB[1]*aafAbsC[0][1]+afEB[2]*aafAbsC[0][2];
    fR01 = afEA[0] + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1
    aafC[1][0]	= D3DXVec3Dot( &akA[1], &akB[0] );
    aafC[1][1]	= D3DXVec3Dot( &akA[1], &akB[1] );
    aafC[1][2]	= D3DXVec3Dot( &akA[1], &akB[2] );
    afAD[1]		= D3DXVec3Dot( &akA[1], &kD );	

    aafAbsC[1][0]	= ( float )fabs(aafC[1][0]);
    aafAbsC[1][1]	= ( float )fabs(aafC[1][1]);
    aafAbsC[1][2]	= ( float )fabs(aafC[1][2]);
    fR				= ( float )fabs(afAD[1]);

    fR1 = afEB[0]*aafAbsC[1][0]+afEB[1]*aafAbsC[1][1]+afEB[2]*aafAbsC[1][2];
    fR01 = afEA[1] + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2
    aafC[2][0]	= D3DXVec3Dot( &akA[2], &akB[0]); 
    aafC[2][1]	= D3DXVec3Dot( &akA[2], &akB[1]); 
    aafC[2][2]	= D3DXVec3Dot( &akA[2], &akB[2]); 
    afAD[2]		= D3DXVec3Dot( &akA[2],  &kD ); 

    aafAbsC[2][0] = (float)fabs(aafC[2][0]); 
    aafAbsC[2][1] = (float)fabs(aafC[2][1]); 
    aafAbsC[2][2] = (float)fabs(aafC[2][2]); 
    fR = (float)fabs(afAD[2]); 
	
    fR1 = afEB[0]*aafAbsC[2][0]+afEB[1]*aafAbsC[2][1]+afEB[2]*aafAbsC[2][2];
    fR01 = afEA[2] + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*B0
    fR = (float)fabs(D3DXVec3Dot(&akB[0], &kD ) ); 
    fR0 = afEA[0]*aafAbsC[0][0]+afEA[1]*aafAbsC[1][0]+afEA[2]*aafAbsC[2][0];
    fR01 = fR0 + afEB[0];
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*B1
    fR = (float)fabs(D3DXVec3Dot( &akB[1], &kD ) ); //akB[1].Dot(kD));
    fR0 = afEA[0]*aafAbsC[0][1]+afEA[1]*aafAbsC[1][1]+afEA[2]*aafAbsC[2][1];
    fR01 = fR0 + afEB[1];
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*B2
    fR = (float)fabs(D3DXVec3Dot( &akB[2], &kD ) ); //MgcMath::Abs(akB[2].Dot(kD));
    fR0 = afEA[0]*aafAbsC[0][2]+afEA[1]*aafAbsC[1][2]+afEA[2]*aafAbsC[2][2];
    fR01 = fR0 + afEB[2];
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A0xB0
    fR = (float)fabs(afAD[2]*aafC[1][0] - afAD[1]*aafC[2][0]); 
    fR0 = afEA[1]*aafAbsC[2][0] + afEA[2]*aafAbsC[1][0];
    fR1 = afEB[1]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A0xB1
    fR = (float)fabs( afAD[2]*aafC[1][1] - afAD[1]*aafC[2][1]); 
    fR0 = afEA[1]*aafAbsC[2][1] + afEA[2]*aafAbsC[1][1];
    fR1 = afEB[0]*aafAbsC[0][2] + afEB[2]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A0xB2
    fR = (float)fabs(afAD[2]*aafC[1][2] - afAD[1]*aafC[2][2]); 
    fR0 = afEA[1]*aafAbsC[2][2] + afEA[2]*aafAbsC[1][2];
    fR1 = afEB[0]*aafAbsC[0][1] + afEB[1]*aafAbsC[0][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1xB0
    fR = (float)fabs(afAD[0]*aafC[2][0] - afAD[2]*aafC[0][0]); 
    fR0 = afEA[0]*aafAbsC[2][0] + afEA[2]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1xB1
    fR = (float)fabs(afAD[0]*aafC[2][1] - afAD[2]*aafC[0][1]); 
    fR0 = afEA[0]*aafAbsC[2][1] + afEA[2]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[1][2] + afEB[2]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A1xB2
    fR = (float)fabs(afAD[0]*aafC[2][2] - afAD[2] * aafC[0][2] ); 
    fR0 = afEA[0]*aafAbsC[2][2] + afEA[2]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[1][1] + afEB[1]*aafAbsC[1][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2xB0
    fR = (float)fabs(afAD[1]*aafC[0][0] - afAD[0]*aafC[1][0] ); 
    fR0 = afEA[0]*aafAbsC[1][0] + afEA[1]*aafAbsC[0][0];
    fR1 = afEB[1]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][1];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2xB1
    fR = (float)fabs(afAD[1]*aafC[0][1] - afAD[0]*aafC[1][1]); 
    fR0 = afEA[0]*aafAbsC[1][1] + afEA[1]*aafAbsC[0][1];
    fR1 = afEB[0]*aafAbsC[2][2] + afEB[2]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    // axis C0+t*A2xB2
    fR = (float)fabs(afAD[1]*aafC[0][2] - afAD[0]*aafC[1][2]); 
    fR0 = afEA[0]*aafAbsC[1][2] + afEA[1]*aafAbsC[0][2];
    fR1 = afEB[0]*aafAbsC[2][1] + afEB[1]*aafAbsC[2][0];
    fR01 = fR0 + fR1;
    if ( fR > fR01 )
        return FALSE;

    return TRUE;
}


bool BoxClip(float fDenom, float fNumer, float& rfT0, float& rfT1)
{
	
    if ( fDenom > 0.0f )
    {
        if ( fNumer > fDenom*rfT1 )
            return false;
        if ( fNumer > fDenom*rfT0 )
            rfT0 = fNumer/fDenom;
        return true;
    }
    else if ( fDenom < 0.0f )
    {
        if ( fNumer > fDenom*rfT0 )
            return false;
        if ( fNumer > fDenom*rfT1 )
            rfT1 = fNumer/fDenom;
        return true;
    }
    else
    {
        return fNumer <= 0.0f;
    }
}


bool ClipBoxRay (D3DXVECTOR3& rkOrigin,
				 D3DXVECTOR3& rkDirection, float afExtent[3], float& rfT0,
				 float& rfT1)
{
    float fSaveT0 = rfT0, fSaveT1 = rfT1;
	
    bool bNotEntirelyClipped =
        BoxClip(+rkDirection.x,-rkOrigin.x-afExtent[0],rfT0,rfT1) &&
        BoxClip(-rkDirection.x,+rkOrigin.x-afExtent[0],rfT0,rfT1) &&
        BoxClip(+rkDirection.y,-rkOrigin.y-afExtent[1],rfT0,rfT1) &&
        BoxClip(-rkDirection.y,+rkOrigin.y-afExtent[1],rfT0,rfT1) &&
        BoxClip(+rkDirection.z,-rkOrigin.z-afExtent[2],rfT0,rfT1) &&
        BoxClip(-rkDirection.z,+rkOrigin.z-afExtent[2],rfT0,rfT1);
	
    return bNotEntirelyClipped && ( rfT0 != fSaveT0 || rfT1 != fSaveT1 );
}

BOOL _XOBBData::InterOBBvsRay( D3DXVECTOR3 mouserayorigin, D3DXVECTOR3 mouseraydirection, D3DXVECTOR3 &PickedPoint )
{
    // convert ray to box coordinates
    D3DXVECTOR3 diff = mouserayorigin - m_kCenter;
    D3DXVECTOR3 Origin(D3DXVec3Dot(&diff,&m_akAxis[0]),
					   D3DXVec3Dot(&diff,&m_akAxis[1]),
					   D3DXVec3Dot(&diff,&m_akAxis[2])
		
		);
    D3DXVECTOR3 Direction(
        D3DXVec3Dot(&mouseraydirection,&m_akAxis[0]),
		D3DXVec3Dot(&mouseraydirection,&m_akAxis[1]),
		D3DXVec3Dot(&mouseraydirection,&m_akAxis[2])
        
		);
	
    float fT0 = 0.0f, fT1 = 1.0f;
    BOOL bIntersects = ClipBoxRay(Origin,Direction,m_afExtent,fT0,fT1);
	
    if ( bIntersects )
    {
        if ( fT0 > 0.0f )
        {
			PickedPoint = mouserayorigin + fT0*mouseraydirection;			
        }
        else  // fT0 == 0
        {
			
            PickedPoint = mouserayorigin + fT1*mouseraydirection;			
        }
    }
	
    return bIntersects;	
}

BOOL _XOBBData::InterOBBvsRay( D3DXVECTOR3 mouserayorigin, D3DXVECTOR3 mouseraydirection, D3DXVECTOR3 &Offset, D3DXVECTOR3 &PickedPoint )
{
    // convert ray to box coordinates
    D3DXVECTOR3 diff = mouserayorigin - (m_kCenter + Offset);
    D3DXVECTOR3 Origin(D3DXVec3Dot(&diff,&m_akAxis[0]),
					   D3DXVec3Dot(&diff,&m_akAxis[1]),
					   D3DXVec3Dot(&diff,&m_akAxis[2])
					   
					   );
    D3DXVECTOR3 Direction(
        D3DXVec3Dot(&mouseraydirection,&m_akAxis[0]),
		D3DXVec3Dot(&mouseraydirection,&m_akAxis[1]),
		D3DXVec3Dot(&mouseraydirection,&m_akAxis[2])
        
		);
	
    float fT0 = 0.0f, fT1 = 1.0f;
    BOOL bIntersects = ClipBoxRay(Origin,Direction,m_afExtent,fT0,fT1);
	
    if ( bIntersects )
    {
        if ( fT0 > 0.0f )
        {
			PickedPoint = mouserayorigin + fT0*mouseraydirection;			
        }
        else  // fT0 == 0
        {
			
            PickedPoint = mouserayorigin + fT1*mouseraydirection;			
        }
    }
	
    return bIntersects;	
}

static void SegmentSegmentCollision(float &t0,float &t1,D3DXVECTOR3 r0,D3DXVECTOR3  dir0,D3DXVECTOR3 r1,D3DXVECTOR3 dir1)
{//unit length of dir0 and dir1
	
	D3DXVECTOR3 p;
	D3DXVec3Subtract(&p,&r1,&r0);  
	float pa=D3DXVec3Dot(&p,&dir0);
	float pb=D3DXVec3Dot(&p,&dir1);
	float ab=D3DXVec3Dot(&dir0,&dir1);
	float det=ab*ab-((float)1.0);
	if(det<((float)-1e-6))
	{
		t0=(ab*pb-pa)/det;
		t1=(pb-ab*pa)/det;
	}
	else
	{//parellel
		t0=((float)0.0);
		t1=((float)0.0);
	}
}

//*************************************************************/
// dxCollideBB - 박스와 박스의 충돌 검사 
// 속도를 고려하여 충돌벡터와 충돌 깊이를 준다. 
//*************************************************************/
bool _XOBBData::InterMOBB(D3DXVECTOR3 &sVel, _XOBBData& rkBox, D3DXVECTOR3& offset, D3DXVECTOR3 &tVel,_XOBBCollideInfo *Contact)
{
	D3DXVECTOR3  TargetCenter = rkBox.Center() + offset;
	D3DXVECTOR3  relativeVel = sVel - tVel;
	// 상대 거리 구한다.	
	D3DXVECTOR3 Diff = m_kCenter - TargetCenter;

	float C[3][3];    //matrix C=A^T B,c_{ij}=Dot(A_i,B_j)
	float absC[3][3]; //|c_{ij}|
	float AD[3];      //Dot(A_i,D)
	float BD[3];      //Dot(B_i,D)
	float R0,R1,R;    //interval radii and distance between centers
	float R01;        //=R0+R1
	float penetration;

	D3DXVECTOR3 rA,rB;
	float tA,tB;

	D3DXVECTOR3 deltap;
	int signA[2],signB[2];
	Contact->depth = 999.0f;
	Contact->type = -1;
	float fLenth;
	D3DXVECTOR3 distVect;
	float relativeVelocity;

	D3DXVec3Normalize(&distVect, &Diff);

	
	///////////////////////////////////////////////////////////////
	// 박스 1 체크 
	///////////////////////////////////////////////////////////////
	//A0
	C[0][0]=D3DXVec3Dot(&m_akAxis[0],&rkBox.Axis()[0]);
	C[0][1]=D3DXVec3Dot(&m_akAxis[0],&rkBox.Axis()[1]);
	C[0][2]=D3DXVec3Dot(&m_akAxis[0],&rkBox.Axis()[2]);

	AD[0]=D3DXVec3Dot(&m_akAxis[0],&Diff);

	absC[0][0]=(float)fabs(C[0][0]);
	absC[0][1]=(float)fabs(C[0][1]);
	absC[0][2]=(float)fabs(C[0][2]);
	R=(float)fabs(AD[0]);

	R1=rkBox.Extents()[0] *absC[0][0]+rkBox.Extents()[1]*absC[0][1]+rkBox.Extents()[2]*absC[0][2];
	R01=m_afExtent[0] + R1;

	penetration = R01 - R;
	if(penetration < 0)	return 0;
	else if(penetration < Contact->depth)
	{
	
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
		Contact->depth = penetration;
		Contact->type  = 0;
	}


	//A1
	C[1][0]=D3DXVec3Dot(&m_akAxis[1],&rkBox.Axis()[0]);
	C[1][1]=D3DXVec3Dot(&m_akAxis[1],&rkBox.Axis()[1]);
	C[1][2]=D3DXVec3Dot(&m_akAxis[1],&rkBox.Axis()[2]);
	AD[1]=D3DXVec3Dot(&m_akAxis[1],&Diff);

	absC[1][0]=(float)fabs(C[1][0]);
	absC[1][1]=(float)fabs(C[1][1]);
	absC[1][2]=(float)fabs(C[1][2]);
	R=(float)fabs(AD[1]);

	R1=rkBox.Extents()[0] *absC[1][0]+rkBox.Extents()[1]*absC[1][1]+rkBox.Extents()[2]*absC[1][2];

	R01=m_afExtent[1] + R1;
	penetration = R01 - R;
	if(penetration < 0)	return 0;
	else if(penetration < Contact->depth)
	{
     	relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
		Contact->depth = penetration;
		Contact->type  = 1;
	}


	 //A2
	C[2][0]=D3DXVec3Dot(&m_akAxis[2],&rkBox.Axis()[0]);
	C[2][1]=D3DXVec3Dot(&m_akAxis[2],&rkBox.Axis()[1]);
	C[2][2]=D3DXVec3Dot(&m_akAxis[2],&rkBox.Axis()[2]);

	AD[2]=D3DXVec3Dot(&m_akAxis[2],&Diff);

	absC[2][0]=(float)fabs(C[2][0]);
	absC[2][1]=(float)fabs(C[2][1]);
	absC[2][2]=(float)fabs(C[2][2]);
	R=(float)fabs(AD[2]);

	R1=rkBox.Extents()[0] *absC[2][0]+rkBox.Extents()[1]*absC[2][1]+rkBox.Extents()[2]*absC[2][2];
	R01=m_afExtent[2] + R1;
	penetration = R01 - R;
	if(penetration < 0)	return 0;
	else if(penetration < Contact->depth)
	{
    	relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
		Contact->depth = penetration;
		Contact->type  = 2;
	}


	///////////////////////////////////////////////////////////////
	// 박스 2 체크 
	//////////////////////////////////////////////////////////////
	//B0	
	BD[0]=D3DXVec3Dot(&rkBox.Axis()[0],&Diff);
 	R=(float)fabs(BD[0]);

	R0 = m_afExtent[0]*absC[0][0]+m_afExtent[1]*absC[1][0]+m_afExtent[2]*absC[2][0];
	R01 = R0+rkBox.Extents()[0];

	penetration=R01-R;
	if(penetration < 0)	return 0;
	else if(penetration < Contact->depth)
	{
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
		Contact->depth = penetration;
		Contact->type  = 3;
	}


	//B1
	BD[1]=D3DXVec3Dot(&rkBox.Axis()[1],&Diff);
 	R=(float)fabs(BD[1]);

	R0 = m_afExtent[0]*absC[0][1]+m_afExtent[1]*absC[1][1]+m_afExtent[2]*absC[2][1];
	R01 = R0+rkBox.Extents()[1];

	penetration=R01-R;

	if(penetration < 0)	return 0;
	else if(penetration < Contact->depth)
	{
    	relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
		Contact->depth = penetration;
		Contact->type  = 4;
	}
	 //B2

	BD[2]=D3DXVec3Dot(&rkBox.Axis()[2],&Diff);
 	R=(float)fabs(BD[2]);

	R0 = m_afExtent[0]*absC[0][2]+m_afExtent[1]*absC[1][2]+m_afExtent[2]*absC[2][2];
	R01 = R0+rkBox.Extents()[2];
	penetration=R01-R;

	if(penetration < 0)	return 0;
	else if(penetration < Contact->depth)
	{
	    relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
		Contact->depth = penetration;
		Contact->type  = 5;
	}

	///////////////////////////////////////////////////////////////
	// 박스 A 엣지체크 
	///////////////////////////////////////////////////////////////

	 //A0xB0
	R=(float)fabs(AD[2]*C[1][0]-AD[1]*C[2][0]); 
	R0=m_afExtent[1]*absC[2][0]+m_afExtent[2]*absC[1][0];
	R1=rkBox.Extents()[1]*absC[0][2]+rkBox.Extents()[2]*absC[0][1];
	R01=R0+R1;
	penetration=R01-R;
	if ( penetration < 0 )   return false;
	fLenth = (float)_XFC_sqrt(C[0][2]*C[0][2]+C[0][1]*C[0][1]);
	if(fLenth > 0) 
	{
		penetration = penetration / fLenth;
		if(penetration < Contact->depth) 
		{
        relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
			Contact->depth = penetration;
			Contact->type  = 6;
		}
	}


	 //A0xB1
	R=(float)fabs(AD[2]*C[1][1]-AD[1]*C[2][1]); 
	R0=m_afExtent[1]*absC[2][1]+m_afExtent[2]*absC[1][1];
	R1=rkBox.Extents()[0]*absC[0][2]+rkBox.Extents()[2]*absC[0][0];
	R01=R0+R1;
	penetration=R01-R;
	if ( penetration < 0 )   return false;
	fLenth = (float)_XFC_sqrt(C[2][1]*C[2][1]+C[1][1]*C[1][1]);
	if(fLenth > 0) 
	{
		penetration = penetration / fLenth;
		if(penetration < Contact->depth) 
		{
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
			Contact->depth = penetration;
			Contact->type  = 7;
		}
	}


	 //A0xB2
	R=(float)fabs(AD[2]*C[1][2]-AD[1]*C[2][2]); 
	R0=m_afExtent[1]*absC[2][2]+m_afExtent[2]*absC[1][2];
	R1=rkBox.Extents()[0]*absC[0][1]+rkBox.Extents()[1]*absC[0][0];
	R01=R0+R1;
	penetration=R01-R;
	if ( penetration < 0 )   return false;
	fLenth = (float)_XFC_sqrt(C[2][2]*C[2][2]+C[1][2]*C[1][2]);
	if(fLenth > 0) 
	{
		penetration = penetration / fLenth;
		if(penetration < Contact->depth) 
		{
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
			Contact->depth = penetration;
			Contact->type  = 8;
		}
	}




	///////////////////////////////////////////////////
	// Box2 엣지
	///////////////////////////////////////////////////

	 //A1xB0

	R=(float)fabs(AD[0]*C[2][0]-AD[2]*C[0][0]);
	R0=m_afExtent[0]*absC[2][0]+m_afExtent[2]*absC[0][0];
	R1=rkBox.Extents()[1]*absC[1][2]+rkBox.Extents()[2]*absC[1][1];
	R01=R0+R1;
	penetration=R01-R;
	if ( penetration < 0 )   return false;
	fLenth = (float)_XFC_sqrt(C[2][0]*C[2][0]+C[0][0]*C[0][0]);
	if(fLenth > 0) 
	{
		penetration = penetration / fLenth;
		if(penetration < Contact->depth) 
		{
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
			Contact->depth = penetration;
			Contact->type  = 9;
		}
	}


	//A1xB1
	R=(float)fabs(AD[0]*C[2][1]-AD[2]*C[0][1]);
	R0=m_afExtent[0]*absC[2][1]+m_afExtent[2]*absC[0][1];
	R1=rkBox.Extents()[0]*absC[1][2]+rkBox.Extents()[2]*absC[1][0];
	R01=R0+R1;

	penetration=R01-R;
	if ( penetration < 0 )   return false;
	fLenth = (float)_XFC_sqrt(C[2][1]*C[2][1]+C[0][1]*C[0][1]);
	if(fLenth > 0) 
	{
		penetration = penetration / fLenth;
		if(penetration < Contact->depth) 
		{
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
			Contact->depth = penetration;
			Contact->type  = 10;
		}
	}



	 //A1xB2
	R=(float)fabs(AD[0]*C[2][2]-AD[2]*C[0][2]);
	R0=m_afExtent[0]*absC[2][2]+m_afExtent[2]*absC[0][2];
	R1=rkBox.Extents()[0]*absC[1][1]+rkBox.Extents()[1]*absC[1][0];
	R01=R0+R1;
	penetration=R01-R;
	if ( penetration < 0 )   return false;
	fLenth =(float)_XFC_sqrt(C[2][2]*C[2][2]+C[0][2]*C[0][2]);
	if(fLenth > 0) 
	{
		penetration = penetration / fLenth;
		if(penetration < Contact->depth) 
		{
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
			Contact->depth = penetration;
			Contact->type  = 11;
		}
	}

	

    //A2xB0
	R=(float)fabs(AD[1]*C[0][0]-AD[0]*C[1][0]);
	R0=m_afExtent[0]*absC[1][0]+m_afExtent[1]*absC[0][0];
	R1=rkBox.Extents()[1]*absC[2][2]+rkBox.Extents()[2]*absC[2][1];
	R01=R0+R1;
	penetration=R01-R;
	if ( penetration < 0 )   return false;
	fLenth =(float)_XFC_sqrt(C[1][0]*C[1][0]+C[0][0]*C[0][0]);	
	if(fLenth > 0) 
	{
		penetration = penetration / fLenth;
		if(penetration < Contact->depth) 
		{
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
			Contact->depth = penetration;
			Contact->type  = 12;
		}
	}


	 //A2xB1
	R=(float)fabs(AD[1]*C[0][1]-AD[0]*C[1][1]);
	R0=m_afExtent[0]*absC[1][1]+m_afExtent[1]*absC[0][1];
	R1=rkBox.Extents()[0]*absC[2][2]+rkBox.Extents()[2]*absC[2][0];
	R01=R0+R1;
	penetration=R01-R;
	if ( penetration < 0 )   return false;
	fLenth =(float)_XFC_sqrt(C[1][1]*C[1][1]+C[0][1]*C[0][1]);
	if(fLenth > 0) 
	{
		penetration = penetration / fLenth;
		if(penetration < Contact->depth) 
		{
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
			Contact->depth = penetration;
			Contact->type  = 13;
		}
	}


	 //A2xB2
	R=(float)fabs(AD[1]*C[0][2]-AD[0]*C[1][2]);
	R0=m_afExtent[0]*absC[1][2]+m_afExtent[1]*absC[0][2];
	R1=rkBox.Extents()[0]*absC[2][1]+rkBox.Extents()[1]*absC[2][0];

	penetration=R01-R;
	if ( penetration < 0 )   return false;
	fLenth =(float)_XFC_sqrt(C[1][2]*C[1][2]+C[0][2]*C[0][2]);
	if(fLenth > 0) 
	{
		penetration = penetration / fLenth;
		if(penetration < Contact->depth) 
		{
		relativeVelocity = D3DXVec3Dot(&distVect,&relativeVel);
		if(relativeVelocity > 0.0f) return false;
			Contact->depth = penetration;
			Contact->type  = 14;
		}
	}


	/////////////////////////////////////////////////////////////////////
	// 법선 벡터 알아내기 
	////////////////////////////////////////////////////////////////////
    if(Contact->type < 3)
	{//Normal from box0,vertex from box1
		int sign[3]=
		{
			C[Contact->type][0]>((float)0.0)?1:-1,
			C[Contact->type][1]>((float)0.0)?1:-1,
			C[Contact->type][2]>((float)0.0)?1:-1
		};

		if(AD[Contact->type]>0)
		{
			Contact->normal.x = -m_akAxis[Contact->type].x;
			Contact->normal.y = -m_akAxis[Contact->type].y;
			Contact->normal.z = -m_akAxis[Contact->type].z;
				
			Contact->pos.x = TargetCenter.x -sign[0]  * rkBox.Axis()[0].x * rkBox.Extents()[0] - 
							 sign[1]* rkBox.Axis()[1].x * rkBox.Extents()[1] - 
							 sign[2]* rkBox.Axis()[2].x *rkBox.Extents()[2];
							
			Contact->pos.y = TargetCenter.y -sign[0]  * rkBox.Axis()[0].y * rkBox.Extents()[0] - 
							 sign[1]* rkBox.Axis()[1].y * rkBox.Extents()[1] - 
							 sign[2]* rkBox.Axis()[2].y *rkBox.Extents()[2];

      		Contact->pos.z = TargetCenter.z -sign[0]  * rkBox.Axis()[0].z * rkBox.Extents()[0] - 
  							 sign[1]* rkBox.Axis()[1].z * rkBox.Extents()[1] - 
							 sign[2]* rkBox.Axis()[2].z *rkBox.Extents()[2];

		 }

	  else
		 {
		   Contact->normal.x = m_akAxis[Contact->type].x;
		   Contact->normal.y = m_akAxis[Contact->type].y;
		   Contact->normal.z = m_akAxis[Contact->type].z;

			Contact->pos.x = TargetCenter.x +sign[0] * rkBox.Axis()[0].x * rkBox.Extents()[0] + 
							 sign[1]* rkBox.Axis()[1].x * rkBox.Extents()[1] + sign[2]* rkBox.Axis()[2].x *rkBox.Extents()[2];
							
			Contact->pos.y = TargetCenter.y +sign[0] * rkBox.Axis()[0].y * rkBox.Extents()[0] + 
							 sign[1]* rkBox.Axis()[1].y * rkBox.Extents()[1] + sign[2]* rkBox.Axis()[2].y *rkBox.Extents()[2];

      		Contact->pos.z = TargetCenter.z +sign[0] * rkBox.Axis()[0].z * rkBox.Extents()[0] + 
  							 sign[1]* rkBox.Axis()[1].z * rkBox.Extents()[1] + sign[2]* rkBox.Axis()[2].z *rkBox.Extents()[2];
		}
	}

	else if(Contact->type<6)
	{//Normal from box1,vertex from box0
  
		int sign[3]=
		{
			C[0][Contact->type-3]>((float)0.0)?1:-1,
			C[1][Contact->type-3]>((float)0.0)?1:-1,
			C[2][Contact->type-3]>((float)0.0)?1:-1
		};
		
			if(D3DXVec3Dot(&rkBox.Axis()[Contact->type-3],&Diff) >0)
			{

				Contact->normal.x = -rkBox.Axis()[Contact->type-3].x;
				Contact->normal.y = -rkBox.Axis()[Contact->type-3].y;
				Contact->normal.z = -rkBox.Axis()[Contact->type-3].z;

				Contact->pos.x = m_kCenter.x +sign[0] * m_akAxis[0].x * m_afExtent[0] + 
							 sign[1]* m_akAxis[1].x * m_afExtent[1] + sign[2]* m_akAxis[2].x *m_afExtent[2];

							
				Contact->pos.y = m_kCenter.y +sign[0] * m_akAxis[0].y * m_afExtent[0] + 
							 sign[1]* m_akAxis[1].y * m_afExtent[1] + sign[2]* m_akAxis[2].y *m_afExtent[2];

		  		Contact->pos.z = m_kCenter.z +sign[0] * m_akAxis[0].z * m_afExtent[0] + 
  							 sign[1]* m_akAxis[1].z * m_afExtent[1] + sign[2]* m_akAxis[2].z *m_afExtent[2];

			}
	  else
	  {
  			Contact->normal.x = rkBox.Axis()[Contact->type-3].x;
			Contact->normal.y = rkBox.Axis()[Contact->type-3].y;
			Contact->normal.z = rkBox.Axis()[Contact->type-3].z;
		
			Contact->pos.x = m_kCenter.x -sign[0] * m_akAxis[0].x * m_afExtent[0] - 
							 sign[1]* m_akAxis[1].x * m_afExtent[1] - sign[2]* m_akAxis[2].x *m_afExtent[2];

							
			Contact->pos.y = m_kCenter.y -sign[0] * m_akAxis[0].y * m_afExtent[0] - 
							 sign[1]* m_akAxis[1].y * m_afExtent[1] - sign[2]* m_akAxis[2].y *m_afExtent[2];

		  	Contact->pos.z = m_kCenter.z -sign[0] * m_akAxis[0].z * m_afExtent[0] - 
  							 sign[1]* m_akAxis[1].z * m_afExtent[1] - sign[2]* m_akAxis[2].z *m_afExtent[2];

	  }
 }

 else switch(Contact->type)
 {
  case 6://A0xB0
  {
   signA[0]=-C[2][0]>0?1:-1;
   signA[1]=C[1][0] >0?1:-1;
   signB[0]=-C[0][2]>0?1:-1;
   signB[1]=C[0][1] >0?1:-1;
   {//A0
    float d=        -C[2][0]*AD[1]+C[1][0]*AD[2];
    float AAD[3]={0,-C[2][0]*d,   C[1][0]*d};
    float dp=
     m_afExtent[1]*signA[0]*AAD[1]+
     m_afExtent[2]*signA[1]*AAD[2];
    if(dp>((float)0.0))
    {
     rA.x=
	  m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0] +
	   m_akAxis[1].x * m_afExtent[1]*signA[0]+
       m_akAxis[2].x * m_afExtent[2]*signA[1];
	   
     rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0] +
	   m_akAxis[1].y * m_afExtent[1]*signA[0]+
       m_akAxis[2].y * m_afExtent[2]*signA[1];

     rA.z=
	   m_kCenter.z +
      -m_akAxis[0].z * m_afExtent[0] +
	   m_akAxis[1].z * m_afExtent[1]*signA[0]+
       m_akAxis[2].z * m_afExtent[2]*signA[1];
    }
    else
    {
     rA.x=
   	  m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0] +
	  -m_akAxis[1].x * m_afExtent[1]*signA[0]+
      -m_akAxis[2].x * m_afExtent[2]*signA[1];

     rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0] +
	  -m_akAxis[1].y * m_afExtent[1]*signA[0]+
      -m_akAxis[2].y * m_afExtent[2]*signA[1];

     rA.z=
       m_kCenter.z +
      -m_akAxis[0].z * m_afExtent[0] +
	  -m_akAxis[1].z * m_afExtent[1]*signA[0]+
      -m_akAxis[2].z * m_afExtent[2]*signA[1];

    }
   }//A0
   {//B0
    float d=        -C[0][2]*BD[1]+C[0][1]*BD[2];
    float BBD[3]={0,-C[0][2]*d,   C[0][1]*d};
    float dp=
     rkBox.Extents()[1]*signB[0]*BBD[1]+
     rkBox.Extents()[2]*signB[1]*BBD[2];
    if(dp>((float)0.0))
    {
     rB.x=
	   TargetCenter.x +
      -rkBox.Axis()[0].x * rkBox.Extents()[0] +
	  -rkBox.Axis()[1].x * rkBox.Extents()[1]*signB[0]+
      -rkBox.Axis()[2].x * rkBox.Extents()[2]*signB[1];
     rB.y=
       TargetCenter.y +
      -rkBox.Axis()[0].y * rkBox.Extents()[0] +
	  -rkBox.Axis()[1].y * rkBox.Extents()[1]*signB[0]+
      -rkBox.Axis()[2].y * rkBox.Extents()[2]*signB[1];

     rB.z=
       TargetCenter.z +
      -rkBox.Axis()[0].z * rkBox.Extents()[0] +
	  -rkBox.Axis()[1].z * rkBox.Extents()[1]*signB[0]+
      -rkBox.Axis()[2].z * rkBox.Extents()[2]*signB[1];

    }
    else
    {
     rB.x=
       TargetCenter.x +
      -rkBox.Axis()[0].x * rkBox.Extents()[0] +
	   rkBox.Axis()[1].x * rkBox.Extents()[1]*signB[0]+
       rkBox.Axis()[2].x * rkBox.Extents()[2]*signB[1];

     rB.y=
       TargetCenter.y +
      -rkBox.Axis()[0].y * rkBox.Extents()[0] +
	   rkBox.Axis()[1].y * rkBox.Extents()[1]*signB[0]+
       rkBox.Axis()[2].y * rkBox.Extents()[2]*signB[1];

     rB.z=
       TargetCenter.z +
      -rkBox.Axis()[0].z * rkBox.Extents()[0] +
	   rkBox.Axis()[1].z * rkBox.Extents()[1]*signB[0]+
       rkBox.Axis()[2].z * rkBox.Extents()[2]*signB[1];
    }
   }//B

   SegmentSegmentCollision(tA,tB,rA,m_akAxis[0],rB,rkBox.Axis()[0]);
   
   Contact->pos.x = rA.x + m_akAxis[0].x*tA;
   Contact->pos.y = rA.y + m_akAxis[1].x*tA;
   Contact->pos.z = rA.z + m_akAxis[2].x*tA;

   deltap.x = rB.x + rkBox.Axis()[0].x*tB - Contact->pos.x;
   deltap.y = rB.y + rkBox.Axis()[0].y*tB - Contact->pos.y;
   deltap.z = rB.z + rkBox.Axis()[0].z*tB - Contact->pos.z;
   D3DXVec3Normalize(&Contact->normal,&deltap); 
   break;
   }
   case 7://A0xB1
{

   signA[0]=-C[2][1] >0?1:-1;
   signA[1]= C[1][1] >0?1:-1;
   signB[0]= C[0][2] >0?1:-1;
   signB[1]=-C[0][0] >0?1:-1;

   {//A0
    float d=        -C[2][1]*AD[1]+C[1][1]*AD[2];
    float AAD[3]={0,-C[2][1]*d,   C[1][1]*d};
    float dp=
     m_afExtent[1]*signA[0]*AAD[1]+
     m_afExtent[2]*signA[1]*AAD[2];

    if(dp>((float)0.0))
    {
      rA.x=
       m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0] +
       m_akAxis[1].x * m_afExtent[1]*signA[0]+
       m_akAxis[2].x * m_afExtent[2]*signA[1];

     rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0] +
       m_akAxis[1].y * m_afExtent[1]*signA[0]+
       m_akAxis[2].y * m_afExtent[2]*signA[1];

     rA.z=
		m_kCenter.z +
       -m_akAxis[0].z * m_afExtent[0] +
		m_akAxis[1].z * m_afExtent[1]*signA[0]+
        m_akAxis[2].z * m_afExtent[2]*signA[1];
    }
    else
    {
  	rA.x=
   	 m_kCenter.x +
        -m_akAxis[0].x * m_afExtent[0] +
        -m_akAxis[1].x * m_afExtent[1]*signA[0]+
        -m_akAxis[2].x * m_afExtent[2]*signA[1];

       rA.y=
         m_kCenter.y +
        -m_akAxis[0].y * m_afExtent[0] +
		-m_akAxis[1].y * m_afExtent[1]*signA[0]+
        -m_akAxis[2].y * m_afExtent[2]*signA[1];

       rA.z=
        m_kCenter.z +
       -m_akAxis[0].z * m_afExtent[0] +
       -m_akAxis[1].z * m_afExtent[1]*signA[0]+
       -m_akAxis[2].z * m_afExtent[2]*signA[1];
    }
   }//A0
   {//B1

    float d=        C[0][2]*BD[0]-C[0][0]*BD[2];
    float BBD[3]={C[0][2]*d,0,-C[0][0]*d};
    float dp=
    rkBox.Extents()[0]*signB[0]*BBD[0]+
    rkBox.Extents()[2]*signB[1]*BBD[2];


    if(dp>((float)0.0))
    {
     rB.x=
      TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];

     rB.y=
       TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2]*signB[1];

     rB.z=
        TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2]*signB[1];
    }
    else
    {
     rB.x=
      TargetCenter.x + 
      rkBox.Axis()[0].x *rkBox.Extents()[0]*signB[0]+
     -rkBox.Axis()[1].x *rkBox.Extents()[1]+
      rkBox.Axis()[2].x *rkBox.Extents()[2]*signB[1];

     rB.y=
      TargetCenter.y + 
      rkBox.Axis()[0].y *rkBox.Extents()[0]*signB[0]+
     -rkBox.Axis()[1].y *rkBox.Extents()[1]+
      rkBox.Axis()[2].y *rkBox.Extents()[2]*signB[1];
     rB.z=
      TargetCenter.z + 
      rkBox.Axis()[0].z *rkBox.Extents()[0]*signB[0]+
     -rkBox.Axis()[1].z *rkBox.Extents()[1]+
      rkBox.Axis()[2].z *rkBox.Extents()[2]*signB[1];
    }
   }//B
   SegmentSegmentCollision(tA,tB,rA,m_akAxis[0],rB,rkBox.Axis()[1]);

   
   Contact->pos.x = rA.x + m_akAxis[0].x*tA;
   Contact->pos.y = rA.y + m_akAxis[1].x*tA;
   Contact->pos.z = rA.z + m_akAxis[2].x*tA;

   deltap.x = rB.x + rkBox.Axis()[1].x*tB - Contact->pos.x;
   deltap.y = rB.y + rkBox.Axis()[1].y*tB - Contact->pos.y;
   deltap.z = rB.z + rkBox.Axis()[1].z*tB - Contact->pos.z;
   D3DXVec3Normalize(&Contact->normal,&deltap); 
   break;
  
  }

  case 8://A0xB2
  {
   signA[0]=-C[2][2]>0?1:-1;
   signA[1]=C[1][2] >0?1:-1;
   signB[0]=-C[0][1]>0?1:-1;
   signB[1]=C[0][0] >0?1:-1;
   {//A0
    float d =  -C[2][2]*AD[1]+C[1][2]*AD[2];
    float AAD[3]={0,-C[2][2]*d,   C[1][2]*d};
    float dp=   
     m_afExtent[1]*signA[0]*AAD[1]+
     m_afExtent[2]*signA[1]*AAD[2];


    if(dp>((float)0.0))
    {
      rA.x=
       m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0] +
       m_akAxis[1].x * m_afExtent[1]*signA[0]+
       m_akAxis[2].x * m_afExtent[2]*signA[1];

     rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0] +
       m_akAxis[1].y * m_afExtent[1]*signA[0]+
       m_akAxis[2].y * m_afExtent[2]*signA[1];
     rA.z=
		m_kCenter.z +
       -m_akAxis[0].z * m_afExtent[0] +
		m_akAxis[1].z * m_afExtent[1]*signA[0]+
        m_akAxis[2].z * m_afExtent[2]*signA[1];

    }
    else
    {
	rA.x=
   		 m_kCenter.x +
        -m_akAxis[0].x * m_afExtent[0] +
        -m_akAxis[1].x * m_afExtent[1]*signA[0]+
        -m_akAxis[2].x * m_afExtent[2]*signA[1];

	rA.y=
		 m_kCenter.y +
        -m_akAxis[0].y * m_afExtent[0] +
		-m_akAxis[1].y * m_afExtent[1]*signA[0]+
        -m_akAxis[2].y * m_afExtent[2]*signA[1];
	rA.z=
		m_kCenter.z +
       -m_akAxis[0].z * m_afExtent[0] +
       -m_akAxis[1].z * m_afExtent[1]*signA[0]+
       -m_akAxis[2].z * m_afExtent[2]*signA[1];

    }
   }//A0
   {//B2
    float d=      -C[0][1]*BD[0]+C[0][0]*BD[1];
    float BBD[3]={-C[0][1]*d,   C[0][0]*d,0};
    float dp=
    rkBox.Extents()[0]*signB[0]*BBD[0]+
    rkBox.Extents()[1]*signB[1]*BBD[1];

    if(dp>((float)0.0))
    {
   rB.x=
      TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2];

   rB.y=
      TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2];

     rB.z=
      TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2];
    }
    else
    {
      rB.x=
      TargetCenter.x +
      rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
      rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2];

     rB.y=
      TargetCenter.y +
      rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
      rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2];
      rB.z=
      TargetCenter.z +
      rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
      rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2];
    }
   }//B2

   SegmentSegmentCollision(tA,tB,rA,m_akAxis[0],rB,rkBox.Axis()[2]);
   Contact->pos.x = rA.x + m_akAxis[0].x*tA;
   Contact->pos.y = rA.y + m_akAxis[1].x*tA;
   Contact->pos.z = rA.z + m_akAxis[2].x*tA;

   deltap.x = rB.x + rkBox.Axis()[2].x*tB - Contact->pos.x;
   deltap.y = rB.y + rkBox.Axis()[2].y*tB - Contact->pos.y;
   deltap.z = rB.z + rkBox.Axis()[2].z*tB - Contact->pos.z;
   D3DXVec3Normalize(&Contact->normal,&deltap); 
   break;
  }
    case 9://A1xB0
  {
   signA[0]=C[2][0] >0?1:-1;
   signA[1]=-C[0][0]>0?1:-1;
   signB[0]=-C[1][2]>0?1:-1;
   signB[1]=C[1][1] >0?1:-1;
   {//A1
    float d=      C[2][0]*AD[0]-C[0][0]*AD[2];
    float AAD[3]={C[2][0]*d,0,-C[0][0]*d};
    float dp=
     m_afExtent[0]*signA[0]*AAD[0]+
     m_afExtent[2]*signA[1]*AAD[2];
    if(dp>((float)0.0))
    {
     rA.x=
       m_kCenter.x +
       m_akAxis[0].x * m_afExtent[0]*signA[0]+
      -m_akAxis[1].x * m_afExtent[1]+
       m_akAxis[2].x * m_afExtent[2]*signA[1];

     rA.y=
       m_kCenter.y +
       m_akAxis[0].y * m_afExtent[0]*signA[0]+
      -m_akAxis[1].y * m_afExtent[1]+
       m_akAxis[2].y * m_afExtent[2]*signA[1];

      rA.z=
       m_kCenter.z +
       m_akAxis[0].z * m_afExtent[0]*signA[0]+
      -m_akAxis[1].z * m_afExtent[1]+
       m_akAxis[2].z * m_afExtent[2]*signA[1];
    }
    else
    {
     rA.x=
       m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0]*signA[0]+
      -m_akAxis[1].x * m_afExtent[1]+
      -m_akAxis[2].x * m_afExtent[2]*signA[1];

     rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0]*signA[0]+
      -m_akAxis[1].y * m_afExtent[1]+
      -m_akAxis[2].y * m_afExtent[2]*signA[1];
   rA.z=
       m_kCenter.z +
      -m_akAxis[0].z * m_afExtent[0]*signA[0]+
      -m_akAxis[1].z * m_afExtent[1]+
      -m_akAxis[2].z * m_afExtent[2]*signA[1];
    }
   }//A1
   {//B0
    float d=        -C[1][2]*BD[1]+C[1][1]*BD[2];
    float BBD[3]={0,-C[1][2]*d,   C[1][1]*d};
    float dp=
    rkBox.Extents()[1]*signB[0]*BBD[1]+
    rkBox.Extents()[2]*signB[1]*BBD[2];


    if(dp>((float)0.0))
    {
  rB.x=
      TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[0]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];

  rB.y=
      TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[0]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2]*signB[1];
  rB.z=
      TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[0]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2]*signB[1];
    }
    else
    {
     rB.x=
      TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]+
       rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[0]+
       rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];

      rB.y=
      TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]+
       rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[0]+
       rkBox.Axis()[2].y*rkBox.Extents()[2]*signB[1];
    rB.z=
      TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]+
       rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[0]+
       rkBox.Axis()[2].z*rkBox.Extents()[2]*signB[1];
    }
   }//B0
  SegmentSegmentCollision(tA,tB,rA,m_akAxis[1],rB,rkBox.Axis()[0]);

  Contact->pos.x = rA.x + m_akAxis[1].x*tA;
   Contact->pos.y = rA.y + m_akAxis[1].y*tA;
   Contact->pos.z = rA.z + m_akAxis[1].z*tA;

   deltap.x = rB.x + rkBox.Axis()[0].x*tB - Contact->pos.x;
   deltap.y = rB.y + rkBox.Axis()[0].y*tB - Contact->pos.y;
   deltap.z = rB.z + rkBox.Axis()[0].z*tB - Contact->pos.z;
   D3DXVec3Normalize(&Contact->normal,&deltap); 
   break;
  }
  
case 10://A1xB1
  {
   signA[0]=C[2][1] >0?1:-1;
   signA[1]=-C[0][1]>0?1:-1;
   signB[0]=C[1][2] >0?1:-1;
   signB[1]=-C[1][0]>0?1:-1;
   {//A1
    float d=      C[2][1]*AD[0]-C[0][1]*AD[2];
    float AAD[3]={C[2][1]*d,0,-C[0][1]*d};
    float dp=
     m_afExtent[0]*signA[0]*AAD[0]+
     m_afExtent[2]*signA[1]*AAD[2];

    if(dp>((float)0.0))
    {
     rA.x=
       m_kCenter.x +
       m_akAxis[0].x * m_afExtent[0]*signA[0]+
      -m_akAxis[1].x * m_afExtent[1]+
       m_akAxis[2].x * m_afExtent[2]*signA[1];

    rA.y=
       m_kCenter.y +
       m_akAxis[0].y * m_afExtent[0]*signA[0]+
      -m_akAxis[1].y * m_afExtent[1]+
       m_akAxis[2].y * m_afExtent[2]*signA[1];

      rA.z=
       m_kCenter.z +
       m_akAxis[0].z * m_afExtent[0]*signA[0]+
      -m_akAxis[1].z * m_afExtent[1]+
       m_akAxis[2].z * m_afExtent[2]*signA[1];

    }
    else
    {
     rA.x=
       m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0]*signA[0]+
      -m_akAxis[1].x * m_afExtent[1]+
      -m_akAxis[2].x * m_afExtent[2]*signA[1];
      rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0]*signA[0]+
      -m_akAxis[1].y * m_afExtent[1]+
      -m_akAxis[2].y * m_afExtent[2]*signA[1];
      rA.z=
       m_kCenter.z +
      -m_akAxis[0].z * m_afExtent[0]*signA[0]+
      -m_akAxis[1].z * m_afExtent[1]+
      -m_akAxis[2].z * m_afExtent[2]*signA[1];
    }
   }//A1
   {//B1
    float d=      C[1][2]*BD[0]-C[1][0]*BD[2];
    float 	BBD[3]={C[1][2]*d,0,-C[1][0]*d};
    float dp=
    rkBox.Extents()[0]*signB[0]*BBD[0]+
    rkBox.Extents()[2]*signB[1]*BBD[2];

    if(dp>((float )0.0))
    {
  rB.x=
      TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];

  rB.y=
      TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2]*signB[1];

  rB.z=
      TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2]*signB[1];

    }
    else
    {

     rB.x=
      TargetCenter.x +
       rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]+
       rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];


      rB.y=
      TargetCenter.y +
       rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]+
       rkBox.Axis()[2].y*rkBox.Extents()[2]*signB[1];
    rB.z=
      TargetCenter.z +
       rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]+
       rkBox.Axis()[2].z*rkBox.Extents()[2]*signB[1];
    }
   }//B1
   SegmentSegmentCollision(tA,tB,rA,m_akAxis[1],rB,rkBox.Axis()[1]);
   Contact->pos.x = rA.x + m_akAxis[1].x*tA;
   Contact->pos.y = rA.y + m_akAxis[1].y*tA;
   Contact->pos.z = rA.z + m_akAxis[1].z*tA;

   deltap.x = rB.x + rkBox.Axis()[1].x*tB - Contact->pos.x;
   deltap.y = rB.y + rkBox.Axis()[1].y*tB - Contact->pos.y;
   deltap.z = rB.z + rkBox.Axis()[1].z*tB - Contact->pos.z;
   D3DXVec3Normalize(&Contact->normal,&deltap); 
   break;
  }


  case 11://A1xB2
  {
   signA[0]=C[2][2] >0?1:-1;
   signA[1]=-C[0][2]>0?1:-1;
   signB[0]=-C[1][1]>0?1:-1;
   signB[1]=C[1][0] >0?1:-1;
   {//A1
    float d=      C[2][2]*AD[0]-C[0][2]*AD[2];
    float AAD[3]={C[2][2]*d,0,-C[0][2]*d};
    float dp=
     m_afExtent[0]*signA[0]*AAD[0]+
     m_afExtent[2]*signA[1]*AAD[2];

    if(dp>((float)0.0))
    {
     rA.x=
       m_kCenter.x +
       m_akAxis[0].x * m_afExtent[0]*signA[0]+
      -m_akAxis[1].x * m_afExtent[1]+
       m_akAxis[2].x * m_afExtent[2]*signA[1];

    rA.y=
       m_kCenter.y +
       m_akAxis[0].y * m_afExtent[0]*signA[0]+
      -m_akAxis[1].y * m_afExtent[1]+
       m_akAxis[2].y * m_afExtent[2]*signA[1];

      rA.z=
       m_kCenter.z +
       m_akAxis[0].z * m_afExtent[0]*signA[0]+
      -m_akAxis[1].z * m_afExtent[1]+
       m_akAxis[2].z * m_afExtent[2]*signA[1];
    }
    else
    {
     rA.x=
       m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0]*signA[0]+
      -m_akAxis[1].x * m_afExtent[1]+
      -m_akAxis[2].x * m_afExtent[2]*signA[1];
      rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0]*signA[0]+
      -m_akAxis[1].y * m_afExtent[1]+
      -m_akAxis[2].y * m_afExtent[2]*signA[1];
      rA.z=
       m_kCenter.z +
      -m_akAxis[0].z * m_afExtent[0]*signA[0]+
      -m_akAxis[1].z * m_afExtent[1]+
      -m_akAxis[2].z * m_afExtent[2]*signA[1];
    }
   }//A1
   {//B2
    float d=      -C[1][1]*BD[0]+C[1][0]*BD[1];
    float BBD[3]={-C[1][1]*d,   C[1][0]*d,0};
    float dp=

    rkBox.Extents()[0]*signB[0]*BBD[0]+
    rkBox.Extents()[1]*signB[1]*BBD[1];

    if(dp>((float)0.0))
    {
      rB.x=
       TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2];

       rB.z=
       TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2];
     rB.z=
       TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2];
    }
    else
    {
    rB.x=
      TargetCenter.x +
       rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
       rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2];

    rB.y=
      TargetCenter.y +
       rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
       rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2];

      rB.z=
      TargetCenter.z +
       rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
       rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2];

    }
   }//B2
   SegmentSegmentCollision(tA,tB,rA,m_akAxis[1],rB,rkBox.Axis()[2]);

   Contact->pos.x = rA.x + m_akAxis[1].x*tA;
   Contact->pos.y = rA.y + m_akAxis[1].y*tA;
   Contact->pos.z = rA.z + m_akAxis[1].z*tA;

   deltap.x = rB.x + rkBox.Axis()[2].x*tB - Contact->pos.x;
   deltap.y = rB.y + rkBox.Axis()[2].y*tB - Contact->pos.y;
   deltap.z = rB.z + rkBox.Axis()[2].z*tB - Contact->pos.z;
   D3DXVec3Normalize(&Contact->normal,&deltap); 
   break;
  }
 case 12://A2xB0
  {
   signA[0]=-C[1][0]>0?1:-1;
   signA[1]=C[0][0] >0?1:-1;
   signB[0]=-C[2][2]>0?1:-1;
   signB[1]=C[2][1] >0?1:-1;
   {//A2
    float d=      -C[1][0]*AD[0]+C[0][0]*AD[1];
    float AAD[3]={-C[1][0]*d,   C[0][0]*d,0};
    float dp=
     m_afExtent[0]*signA[0]*AAD[0]+
     m_afExtent[1]*signA[1]*AAD[1];

    if(dp>((float)0.0))
    {
     rA.x=
       m_kCenter.x +
       m_akAxis[0].x * m_afExtent[0]*signA[0]+
       m_akAxis[1].x * m_afExtent[1]*signA[1]+
       -m_akAxis[2].x * m_afExtent[2];


     rA.y=
       m_kCenter.y +
       m_akAxis[0].y * m_afExtent[0]*signA[0]+
       m_akAxis[1].y * m_afExtent[1]*signA[1]+
       -m_akAxis[2].y * m_afExtent[2];


     rA.z=
       m_kCenter.z +
       m_akAxis[0].z * m_afExtent[0]*signA[0]+
       m_akAxis[1].z * m_afExtent[1]*signA[1]+
       -m_akAxis[2].z * m_afExtent[2];

    }
    else
    {

     rA.x=
       m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0]*signA[0]+
      -m_akAxis[1].x * m_afExtent[1]*signA[1]+
      -m_akAxis[2].x * m_afExtent[2];


 
     rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0]*signA[0]+
      -m_akAxis[1].y * m_afExtent[1]*signA[1]+
      -m_akAxis[2].y * m_afExtent[2];


     rA.z=
       m_kCenter.z +
      -m_akAxis[0].z * m_afExtent[0]*signA[0]+
      -m_akAxis[1].z * m_afExtent[1]*signA[1]+
      -m_akAxis[2].z * m_afExtent[2];
    }
   }//A2   
   {//B0
    float d=        -C[2][2]*BD[1]+C[2][1]*BD[2];
    float BBD[3]={0,-C[2][2]*d,   C[2][1]*d};
    float dp=
    rkBox.Extents()[1]*signB[0]*BBD[1]+
    rkBox.Extents()[2]*signB[1]*BBD[2];

    if(dp>((float)0.0))
    {
    rB.x=
       TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[0]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];

   rB.y=
       TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[0]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2]*signB[1];
   rB.z=
       TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[0]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2]*signB[1];
    }
    else
    {
    rB.x=
      TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]+
       rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[0]+
       rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];

   rB.y=
      TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]+
       rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[0]+
       rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];
   rB.z=
      TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]+
       rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[0]+
       rkBox.Axis()[2].z*rkBox.Extents()[2]*signB[1];
    }
   }//B0
    SegmentSegmentCollision(tA,tB,rA,m_akAxis[2],rB,rkBox.Axis()[0]);

   Contact->pos.x = rA.x + m_akAxis[2].x*tA;
   Contact->pos.y = rA.y + m_akAxis[2].y*tA;
   Contact->pos.z = rA.z + m_akAxis[2].z*tA;

   deltap.x = rB.x + rkBox.Axis()[0].x*tB - Contact->pos.x;
   deltap.y = rB.y + rkBox.Axis()[0].y*tB - Contact->pos.y;
   deltap.z = rB.z + rkBox.Axis()[0].z*tB - Contact->pos.z;
   D3DXVec3Normalize(&Contact->normal,&deltap); 
   break;
  }
  
   case 13://A2xB1
  {
   signA[0]=-C[1][1]>0?1:-1;
   signA[1]=C[0][1] >0?1:-1;
   signB[0]=C[2][2] >0?1:-1;
   signB[1]=-C[2][0]>0?1:-1;
   {//A2
    float d=      -C[1][1]*AD[0]+C[0][1]*AD[1];
    float AAD[3]={-C[1][1]*d,   C[0][1]*d,0};
    float dp=
     m_afExtent[0]*signA[0]*AAD[0]+
     m_afExtent[1]*signA[1]*AAD[1];

    if(dp>((float )0.0))
    {
      rA.x=
       m_kCenter.x +
       m_akAxis[0].x * m_afExtent[0]*signA[0]+
       m_akAxis[1].x * m_afExtent[1]*signA[1]+
       -m_akAxis[2].x * m_afExtent[2];


     rA.y=
       m_kCenter.y +
       m_akAxis[0].y * m_afExtent[0]*signA[0]+
       m_akAxis[1].y * m_afExtent[1]*signA[1]+
       -m_akAxis[2].y * m_afExtent[2];


     rA.z=
       m_kCenter.z +
       m_akAxis[0].z * m_afExtent[0]*signA[0]+
       m_akAxis[1].z * m_afExtent[1]*signA[1]+
       -m_akAxis[2].z * m_afExtent[2];
    }
    else
    {

     rA.x=
       m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0]*signA[0]+
      -m_akAxis[1].x * m_afExtent[1]*signA[1]+
      -m_akAxis[2].x * m_afExtent[2];


 
     rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0]*signA[0]+
      -m_akAxis[1].y * m_afExtent[1]*signA[1]+
      -m_akAxis[2].y * m_afExtent[2];


     rA.z=
       m_kCenter.z +
      -m_akAxis[0].z * m_afExtent[0]*signA[0]+
      -m_akAxis[1].z * m_afExtent[1]*signA[1]+
      -m_akAxis[2].z * m_afExtent[2];
    }
   }//A2
   {//B1
    float d=      C[2][2]*BD[0]-C[2][0]*BD[2];
    float BBD[3]={C[2][2]*d,0,-C[2][0]*d};
    float dp=
    rkBox.Extents()[0]*signB[0]*BBD[0]+
    rkBox.Extents()[2]*signB[1]*BBD[2];
    if(dp>((float)0.0))
    {
      rB.x=
       TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];
      rB.y=
       TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2]*signB[1];
      rB.z=
       TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2]*signB[1];
    }
    else
    {
      rB.x=
       TargetCenter.x +
       rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]+
       rkBox.Axis()[2].x*rkBox.Extents()[2]*signB[1];
    rB.y=
       TargetCenter.y +
       rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]+
       rkBox.Axis()[2].y*rkBox.Extents()[2]*signB[1];

     rB.z=
       TargetCenter.z +
       rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]+
       rkBox.Axis()[2].z*rkBox.Extents()[2]*signB[1];

    }
   }//B1
   SegmentSegmentCollision(tA,tB,rA,m_akAxis[2],rB,rkBox.Axis()[1]);
  
   Contact->pos.x = rA.x + m_akAxis[2].x*tA;
   Contact->pos.y = rA.y + m_akAxis[2].y*tA;
   Contact->pos.z = rA.z + m_akAxis[2].z*tA;

   deltap.x = rB.x + rkBox.Axis()[1].x*tB - Contact->pos.x;
   deltap.y = rB.y + rkBox.Axis()[1].y*tB - Contact->pos.y;
   deltap.z = rB.z + rkBox.Axis()[1].z*tB - Contact->pos.z;
   D3DXVec3Normalize(&Contact->normal,&deltap);
   break;
  }
  
  case 14://A2xB2
  {
   signA[0]=-C[1][2]>0?1:-1;
   signA[1]=C[0][2] >0?1:-1;
   signB[0]=-C[2][1]>0?1:-1;
   signB[1]=C[2][0] >0?1:-1;
   {//A2
    float d=      -C[1][2]*AD[0]+C[0][2]*AD[1];
    float AAD[3]={-C[1][2]*d,   C[0][2]*d,0};
    float dp=
     m_afExtent[0]*signA[0]*AAD[0]+
     m_afExtent[1]*signA[1]*AAD[1];
    if(dp>((float)0.0))
    {
      rA.x=
       m_kCenter.x +
       m_akAxis[0].x * m_afExtent[0]*signA[0]+
       m_akAxis[1].x * m_afExtent[1]*signA[1]+
       -m_akAxis[2].x * m_afExtent[2];


     rA.y=
       m_kCenter.y +
       m_akAxis[0].y * m_afExtent[0]*signA[0]+
       m_akAxis[1].y * m_afExtent[1]*signA[1]+
       -m_akAxis[2].y * m_afExtent[2];


     rA.z=
       m_kCenter.z +
       m_akAxis[0].z * m_afExtent[0]*signA[0]+
       m_akAxis[1].z * m_afExtent[1]*signA[1]+
       -m_akAxis[2].z * m_afExtent[2];
    }
    else
    {
     rA.x=
       m_kCenter.x +
      -m_akAxis[0].x * m_afExtent[0]*signA[0]+
      -m_akAxis[1].x * m_afExtent[1]*signA[1]+
      -m_akAxis[2].x * m_afExtent[2];
     rA.y=
       m_kCenter.y +
      -m_akAxis[0].y * m_afExtent[0]*signA[0]+
      -m_akAxis[1].y * m_afExtent[1]*signA[1]+
      -m_akAxis[2].y * m_afExtent[2];


     rA.z=
       m_kCenter.z +
      -m_akAxis[0].z * m_afExtent[0]*signA[0]+
      -m_akAxis[1].z * m_afExtent[1]*signA[1]+
      -m_akAxis[2].z * m_afExtent[2];
    }
   }//A2
   {//B2
    float d=      -C[2][1]*BD[0]+C[2][0]*BD[1];
    float BBD[3]={-C[2][1]*d,   C[2][0]*d,0};
    float dp=
    rkBox.Extents()[0]*signB[0]*BBD[0]+
    rkBox.Extents()[1]*signB[1]*BBD[1];

    if(dp>((float)0.0))
    {
      rB.x=
       TargetCenter.x +
      -rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2];

   rB.y=
       TargetCenter.y +
      -rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2];
  rB.z=
       TargetCenter.z +
      -rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
      -rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2];
    }
    else
    {
      rB.x=
       TargetCenter.x +
       rkBox.Axis()[0].x*rkBox.Extents()[0]*signB[0]+
       rkBox.Axis()[1].x*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].x*rkBox.Extents()[2];

      rB.y=
       TargetCenter.y +
       rkBox.Axis()[0].y*rkBox.Extents()[0]*signB[0]+
       rkBox.Axis()[1].y*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].y*rkBox.Extents()[2];
      rB.z=
       TargetCenter.z +
       rkBox.Axis()[0].z*rkBox.Extents()[0]*signB[0]+
       rkBox.Axis()[1].z*rkBox.Extents()[1]*signB[1]+
      -rkBox.Axis()[2].z*rkBox.Extents()[2];
    }
   }//B
   SegmentSegmentCollision(tA,tB,rA,m_akAxis[2],rB,rkBox.Axis()[2]);
   Contact->pos.x = rA.x + m_akAxis[2].x*tA;
   Contact->pos.y = rA.y + m_akAxis[2].y*tA;
   Contact->pos.z = rA.z + m_akAxis[2].z*tA;

   deltap.x = rB.x + rkBox.Axis()[2].x*tB - Contact->pos.x;
   deltap.y = rB.y + rkBox.Axis()[2].y*tB - Contact->pos.y;
   deltap.z = rB.z + rkBox.Axis()[2].z*tB - Contact->pos.z;
   D3DXVec3Normalize(&Contact->normal,&deltap);
   break;
  }
  }
	return true;
}



bool _XOBBData::IntersectOBBvsOBB(_XOBBData& rkBox, D3DXVECTOR3& xP1 )
{
	// test penetration	
	FLOAT dx = fabs(m_kCenter.x - rkBox.m_kCenter.x);
	FLOAT dy = fabs(m_kCenter.y - rkBox.m_kCenter.y);
	FLOAT dz = fabs(m_kCenter.z - rkBox.m_kCenter.z);
	if( (m_Radius + rkBox.m_Radius) < (dx*dx + dy*dy + dz*dz) ) return false;

	//---------------------------------------------------------------------
	// the cached separation axes
	//---------------------------------------------------------------------
	D3DXVECTOR3 Nsep[15];
	FLOAT		dsep[15];
	
	D3DXVECTOR3*  N = Nsep;
	float*		  d = dsep;
	
	const D3DXVECTOR3  *A = m_akAxis;
	const D3DXVECTOR3  *B = rkBox.m_akAxis;
	
	//---------------------------------------------------------------------
	// test the 15 potential separation axes.
	// calcualte the penetration depth (which is implicitely scaled by the axis length).
	//---------------------------------------------------------------------
	*N = A[0];
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	*N = A[1];
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	*N = A[2];
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	*N = B[0];
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	*N = B[1];
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	*N = B[2];
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	
	D3DXVec3Cross( N, &A[0], &B[0] );
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	D3DXVec3Cross( N, &A[0], &B[1] );	
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	D3DXVec3Cross( N, &A[0], &B[2] );		
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	D3DXVec3Cross( N, &A[1], &B[0] );			
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	D3DXVec3Cross( N, &A[1], &B[1] );			
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;

	D3DXVec3Cross( N, &A[1], &B[2] );			
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	D3DXVec3Cross( N, &A[2], &B[0] );			
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	D3DXVec3Cross( N, &A[2], &B[1] );			
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	D3DXVec3Cross( N, &A[2], &B[2] );
	if (!SpanIntersect(*N, rkBox, *d)) return false;
	N++; d++;
	
	//---------------------------------------------------------------------
	// all axes overlap, now we need to find the collision plane among them
	// it will be the non degenerate plane with the minum depth overlap
	//---------------------------------------------------------------------
	D3DXVECTOR3  Ncoll;
	FLOAT		 dcoll;

	if (!GetCollisionPlane(Nsep, dsep, Ncoll, dcoll)) return false;
	
	//---------------------------------------------------------------------
	// we found it, make sure it's oriented correctly
	//---------------------------------------------------------------------
	D3DXVECTOR3  D = m_kCenter - rkBox.m_kCenter;
	
	if ( D3DXVec3Dot( &D, &Ncoll ) < 0.0f) Ncoll *= -1.0f;
	
	//---------------------------------------------------------------------
	// setup the separation vector
	//---------------------------------------------------------------------
	xP1 = Ncoll * dcoll;
	
	return true;
}

//---------------------------------------------------------------------
// Calcualte the intervals of a box along a random axis
//---------------------------------------------------------------------
void _XOBBData::GetSpan(const D3DXVECTOR3& xAxis, float& min, float& max)
{
	float p = D3DXVec3Dot( &m_kCenter, &xAxis );
	float r =	(float)fabs( D3DXVec3Dot( &m_akAxis[0], &xAxis ) ) * m_afExtent[0] + 
				(float)fabs( D3DXVec3Dot( &m_akAxis[1], &xAxis ) ) * m_afExtent[1] + 
				(float)fabs( D3DXVec3Dot( &m_akAxis[2], &xAxis ) ) * m_afExtent[2];
	
	min = p - r;
	max = p + r;
}

//---------------------------------------------------------------------
// Calcualte the intervals of 2 boxes along a random axis, and see if 
// the intervals overlap. if yes, return the intersection of the intervals
//---------------------------------------------------------------------
bool _XOBBData::SpanIntersect(D3DXVECTOR3& xAxis, _XOBBData& xBox, float& depth)
{
	 //---------------------------------------------------------------------
	 // calculate intervals
	 //---------------------------------------------------------------------
	 float min0, max0;
	 float min1, max1;
	
	 GetSpan(xAxis, min0, max0);
	 xBox.GetSpan(xAxis, min1, max1);
	 
	 //---------------------------------------------------------------------
	 // do the intervals intersect? 
	 //---------------------------------------------------------------------
	 if (min0 > max1 || min1 > max0)
		 return false;
	 
	 //---------------------------------------------------------------------
	 // yes, calcualte the intersection between intervals
	 //---------------------------------------------------------------------
	 float min = (min0 > min1)? min0 : min1;
	 float max = (max0 < max1)? max0 : max1;
	 
	 depth = max - min;
	 
	 return true;
}

//---------------------------------------------------------------------
// find the collsion plane among a list of separation axis vectors
//---------------------------------------------------------------------
bool _XOBBData::GetCollisionPlane(D3DXVECTOR3* A, float* d, D3DXVECTOR3& Ncoll, float &dcoll)
{
	dcoll = 100000000.0f;
	
	bool bFoundAxis = false;
	
	//---------------------------------------------------------------------
	// scan the axis, take the non degenerate axis with minimum penetration depth
	//---------------------------------------------------------------------
	int i = 0  ; for( i = 0; i < 15; i ++)
	{
		//---------------------------------------------------------------------
		// Check if axis is degenerate
		//---------------------------------------------------------------------
		float a2 = D3DXVec3Dot( &A[i], &A[i] );
		
		if (a2 < 0.000001f)
			continue;
		
		//---------------------------------------------------------------------
		// nope, normalise axis, and penetration depth 
		//---------------------------------------------------------------------
		float a = (float)_XFC_sqrt(a2);
		
		float h = d[i] / a; // penetration depth
		
		//---------------------------------------------------------------------
		// found a smaller penetration depth, set the collision plane to it
		//---------------------------------------------------------------------
		if (h < dcoll)
		{
			dcoll = h;
			Ncoll = A[i] / a;
			bFoundAxis = true;
		}
	}
	
	return bFoundAxis;
}



//////////////////////////////////////////////////////////////////////
// AABB박스 만들기 
//////////////////////////////////////////////////////////////////////
void _XOBBMakeAlignedBox (CsuX3D_Box *Box,int Size, D3DXVECTOR3* Point)    
{
	D3DXVECTOR3 min;		 // 최소 좌표 
	D3DXVECTOR3 max;		 // 최대 좌표 

    min = Point[0];
    max = min;

    int i = 0 ; for ( i = 1; i < Size; i++)
    {
        if ( Point[i].x < min.x )
            min.x = Point[i].x;
        else if ( Point[i].x > max.x )
            max.x = Point[i].x;

        if ( Point[i].y < min.y )
            min.y = Point[i].y;
        else if ( Point[i].y > max.y )
            max.y = Point[i].y;

        if ( Point[i].z < min.z )
            min.z = Point[i].z;
        else if ( Point[i].z > max.z )
            max.z = Point[i].z;
    }

	Box->m_kMin = min;
	Box->m_kMax = max;

	Box->m_kCenter.x   = 0.5f*(min.x+max.x);
	Box->m_kCenter.y   = 0.5f*(min.x+max.y);
	Box->m_kCenter.z   = 0.5f*(min.z+max.z);
}



void _XOBBGetRotMatrix(D3DXMATRIX *m,D3DXVECTOR3 *Axis)
{
	
	D3DXMatrixIdentity(m);
	m->_11 = Axis[0].x;		
	m->_12 = Axis[0].y;
	m->_13 = Axis[0].z;

	m->_21 = Axis[1].x;		
	m->_22 = Axis[1].y;
	m->_23 = Axis[1].z;

	m->_31 = Axis[2].x;		
	m->_32 = Axis[2].y;
	m->_33 = Axis[2].z;
}

void _XOBBGetAxisFromMatrix(D3DXVECTOR3 *Axis,D3DXMATRIX *m)
{
	Axis[0].x = m->_11;
	Axis[0].y = m->_12;
	Axis[0].z = m->_13;

	Axis[1].x = m->_21;
	Axis[1].y = m->_22;
	Axis[1].z = m->_23;

	Axis[2].x = m->_31;
	Axis[2].y = m->_32;
	Axis[2].z = m->_33;		
}

CsuX3D_Box _XOBBMergeBoxes (CsuX3D_Box &Box0,CsuX3D_Box &Box1)
{

		CsuX3D_Box rBox;
		rBox.m_kCenter= 0.5*(Box0.m_kCenter + Box1.m_kCenter);
		D3DXQUATERNION  Q0,Q1;

		D3DXMATRIX M0,M1;
		_XOBBGetRotMatrix(&M0,Box0.m_akAxis);
		_XOBBGetRotMatrix(&M1,Box1.m_akAxis);

		D3DXQuaternionRotationMatrix(&Q0,&M0);
		D3DXQuaternionRotationMatrix(&Q1,&M1);

		float QDot = D3DXQuaternionDot(&Q0,&Q1);
		if(QDot < 0.0f)
			Q1 = -Q1;
		

		D3DXQUATERNION qResult = Q0 + Q1;
		float Q0Dot = D3DXQuaternionDot(&qResult,&qResult);

		float InvLenth = (float) (1/_XFC_sqrt(Q0Dot));
		qResult = InvLenth *qResult;
		D3DXMATRIX mResult;
		D3DXMatrixRotationQuaternion(&mResult,&qResult);
		_XOBBGetAxisFromMatrix(rBox.m_akAxis,&mResult);

		int i, j;
		float fDot;
		D3DXVECTOR3 akVertex[8], kDiff;
		float kMin[3],kMax[3];


		for (i = 0; i < 3; i++)
		{
			kMin[i] = 0.0f;
			kMax[i] = 0.0f;

		}

		
	    Box0.ComputeVertices(akVertex);
		for (i = 0; i < 8; i++)
		{
			kDiff = akVertex[i] - rBox.m_kCenter;
			for (j = 0; j < 3; j++)
			{
				
				fDot = D3DXVec3Dot(&kDiff,&rBox.m_akAxis[j]);
				if ( fDot > kMax[j] )
					kMax[j] = fDot;
				else if ( fDot < kMin[j] )
					kMin[j] = fDot;
			}
		}

		Box1.ComputeVertices(akVertex);
		for (i = 0; i < 8; i++)
		{
			kDiff = akVertex[i] - rBox.m_kCenter;
			for (j = 0; j < 3; j++)
			{
				
				fDot = D3DXVec3Dot(&kDiff,&rBox.m_akAxis[j]);
				if ( fDot > kMax[j] )
					kMax[j] = fDot;
				else if ( fDot < kMin[j] )
					kMin[j] = fDot;
			}
		}

		for (j = 0; j < 3; j++)
		{
			rBox.m_kCenter   += (((float)0.5)*(kMax[j]+kMin[j]))*rBox.m_akAxis[j];
			rBox.m_afExtent[j] = ((float)0.5)*(kMax[j]-kMin[j]);
		}


		return rBox;
}
//////////////////////////////////////////////////////////////////////
// OBB박스 만들기 
//////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void _XOBBRotate( float a[3][3], float s, float tau, int i, int j, int k, int l )
{
	float h, g;
	g = a[i][j];
	h = a[k][l];
	a[i][j] = g - s * ( h + g *tau );
	a[k][l] = h + s * ( g - h *tau );
}

bool  Jacobi( float a[3][3], float v[3][3], float d[3] )
{
	int n = 3;
	int i, j, iq, ip;
	float tresh, theta, tau, t, sm, s, h, g, c, b[3], z[3];

	for( ip = 0; ip < n; ip++ ){
		for( iq = 0; iq < n; iq++ ) v[ip][iq] = 0.0f;
		v[ip][ip] = 1.0f;
	}
	for( ip = 0; ip < n; ip++ ){
		b[ip] = d[ip] = a[ip][ip];
		z[ip] = 0.0f;
	}
	for( i = 0; i < 50; i++ ){
		sm = 0.0f;
		for( ip = 0; ip < n - 1; ip++ ){
			for( iq = ip + 1; iq < n; iq++ ) sm += (float)fabs(a[ip][iq]);
		}

		if( sm == 0.0f ) return true;
		if( i < 3 ) tresh = 0.2f * sm / ( n * n );
		else tresh = 0.0f;
		for( ip = 0; ip < n - 1; ip++ ){
			for( iq = ip + 1; iq < n; iq++ ){
				g = 100.0f * (float)fabs( a[ip][iq] );
				if( i > 3 && ( fabs( d[ip] ) + g ) == fabs( d[ip] )
					&& ( fabs( d[iq] ) + g ) == fabs( d[iq] ) ) a[ip][iq] = 0.0f;
				else if( fabs( a[ip][iq] ) > tresh ){
					h = d[iq] - d[ip];
					if( ( fabs( h ) + g ) == fabs( h ) ) t = a[ip][iq] / h;
					else{
						theta = 0.5f * h / a[ip][iq];
						t = 1.0f / ( (float)fabs( theta ) + (float)_XFC_sqrt( 1.0f + theta * theta ) );
						if( theta < 0.0f ) t = -t;
					}
					c = 1.0f / (float)_XFC_sqrt( 1 + t * t );
					s = t * c;
					tau = s / ( 1.0f + c );
					h = t * a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq] = 0.0f;

					for( j = 0; j < ip; j++ ) _XOBBRotate( a, s, tau, j, ip, j, iq );
					for( j = ip + 1; j < iq; j++ ) _XOBBRotate( a, s, tau, ip, j, j, iq );
					for( j = iq + 1; j < n; j++ ) _XOBBRotate( a, s, tau, ip, j, iq, j );
					for( j = 0; j < n; j++ ) _XOBBRotate( v, s, tau, j, ip, j, iq );
				}
			}
		}
		for( ip = 0; ip < n; ip++ ){
			b[ip] += z[ip];
			d[ip] = b[ip];
			z[ip] = 0.0f;
		}
	}

	return false;
}

void _XOBBMakeOBBBox (CsuX3D_Box *Box,int Size, D3DXVECTOR3* Vertex)    
{
    
   // compute mean of points
	 D3DXVECTOR3 m( 0, 0, 0 );
	int i = 0;for( ; i < Size; ++i ){
		m += Vertex[i];
	}
	m /= (float)Size;

	
    // compute covariances of points
 	float C11 = 0, C22 = 0, C33 = 0, C12 = 0, C13 = 0,  C23 = 0;
	for( i = 0; i < Size; ++i ){
		C11 += ( Vertex[i].x - m.x ) * ( Vertex[i].x - m.x );
		C22 += ( Vertex[i].y - m.y ) * ( Vertex[i].y - m.y );
		C33 += ( Vertex[i].z - m.z ) * ( Vertex[i].z - m.z );
		C12 += ( Vertex[i].x - m.x ) * ( Vertex[i].y - m.y );
		C13 += ( Vertex[i].x - m.x ) * ( Vertex[i].z - m.z );
		C23 += ( Vertex[i].y - m.y ) * ( Vertex[i].z - m.z );
	}
	C11 /= Size;
	C22 /= Size;
	C33 /= Size;
	C12 /= Size;
	C13 /= Size;
	C23 /= Size;

    // compute eigenvectors for covariance matrix
   	float Matrix[3][3] = {
		{ C11, C12, C13 },
		{ C12, C22, C23 },
		{ C13, C23, C33 },
	};

	float EigenVectors[3][3];
	float EigenValue[3];
	Jacobi( Matrix, EigenVectors, EigenValue );


	struct SORT{
		int ID;
		float Value;
	} Sort[3] = { { 0, EigenValue[0] }, { 1, EigenValue[1] }, { 2, EigenValue[2] } };


	// 축 구하기 
    for(i = 0; i < 3; ++i ){
		Box->m_akAxis[i].x = EigenVectors[0][Sort[i].ID];
		Box->m_akAxis[i].y = EigenVectors[1][Sort[i].ID];
		Box->m_akAxis[i].z = EigenVectors[2][Sort[i].ID];
	}

	float min[3] = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
	float max[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	int j = 0 ; for( j = 0; j < 3; ++j ){
		int i = 0 ; for(  i = 0; i < Size; ++i ){
			float a = D3DXVec3Dot( &Vertex[i], &Box->m_akAxis[j] );
			if( min[j] > a ) min[j] = a;
			if( max[j] < a ) max[j] = a;
		}
	}

	// 중심 구하기 
	Box->m_kCenter = Box->m_akAxis[0] * ( ( min[0] + max[0] ) * 0.5f )
				+ Box->m_akAxis[1] * ( ( min[1] + max[1] ) * 0.5f)
				+ Box->m_akAxis[2] * ( ( min[2] + max[2] ) * 0.5f );

	// 길이 구하기
	for(  i = 0; i < 3; ++i ) 
	Box->m_afExtent[i] = 0.5f*(max[i] - min[i]);

	
}
	
void _XCreateOBBFromVertexList( D3DXVECTOR3* pVertex, int vertexcount, _XOBBData* pOBBData )
{
	/*
	float fminmax[6] = { 99999.0f, -99999.0f, 99999.0f, -99999.0f, 99999.0f, -99999.0f };	////0=xmin, 1=xmax, 2=ymin, 3=ymax, ....
	
	int j = 0 ; for( j=0; j<vertexcount; j++ )
	{
		if( fminmax[1] < pVertex[j].x )
		{
			fminmax[1] = pVertex[j].x; //버텍스 x의 최대값
		}
		if( fminmax[0] > pVertex[j].x )
		{
			fminmax[0] = pVertex[j].x;	//버텍스 x의 최소값
		}
		
		if( fminmax[3] < pVertex[j].y )
		{
			fminmax[3] = pVertex[j].y; //버텍스 y의 최대값
		}
		if( fminmax[2] > pVertex[j].y )
		{
			fminmax[2] = pVertex[j].y; // 버텍스 y의 최소값
		}
		
		if( fminmax[5] < pVertex[j].z )
		{
			fminmax[5] = pVertex[j].z; //버텍스 Z의 최대값
		}
		if( fminmax[4] > pVertex[j].z )
		{
			fminmax[4] = pVertex[j].z; // 버텍스 Z의 최소값
		}
	}
	
	D3DXVECTOR3 _v[8];
	_v[0].x = fminmax[0]; _v[0].y = fminmax[2]; _v[0].z = fminmax[4] ;
	_v[1].x = fminmax[0]; _v[1].y = fminmax[3]; _v[1].z = fminmax[4] ;
	_v[2].x = fminmax[1]; _v[2].y = fminmax[3]; _v[2].z = fminmax[4] ;
	_v[3].x = fminmax[1]; _v[3].y = fminmax[2]; _v[3].z = fminmax[4] ;
	_v[4].x = fminmax[0]; _v[4].y = fminmax[2]; _v[4].z = fminmax[5] ;
	_v[5].x = fminmax[0]; _v[5].y = fminmax[3]; _v[5].z = fminmax[5] ;
	_v[6].x = fminmax[1]; _v[6].y = fminmax[3]; _v[6].z = fminmax[5] ;
	_v[7].x = fminmax[1]; _v[7].y = fminmax[2]; _v[7].z = fminmax[5] ;
	
	D3DXVECTOR3 vExtent;
	D3DXVECTOR3 vtemp;	
	vtemp = (_v[3] - _v[0])*0.5f ;
	vExtent.x = D3DXVec3Length(&vtemp);
	
	vtemp = (_v[1] - _v[0])*0.5f ;
	vExtent.y = D3DXVec3Length(&vtemp);
	
	vtemp = (_v[4] - _v[0])*0.5f ;
	vExtent.z = D3DXVec3Length(&vtemp);
	
	//공간 대각선의 중점, 즉 공간 상자의 중점을 구한다.
	//점 A와 B를 있는 선분의 좌표는[ (x1 + x2)/2 , (y1+y2)/2 , (z1+z2)/2] 
	
	CsuX3D_Box SuAABB;
	SuAABB.Setapex(_v);
	D3DXVECTOR3 vCenter;
	vCenter.x = (_v[5].x + _v[3].x) * 0.5f;
	vCenter.y = (_v[5].y + _v[3].y) * 0.5f;
	vCenter.z = (_v[5].z + _v[3].z) * 0.5f;
	
	D3DXVECTOR3 vAxis[3];
	vAxis[0].x = vCenter.x;
	vAxis[0].y = 0.0f;
	vAxis[0].z = 0.0f;
	D3DXVec3Normalize(&vAxis[0], &vAxis[0]);
	
	vAxis[1].x = 0.0f;
	vAxis[1].y = vCenter.y;
	vAxis[1].z = 0.0f;
	D3DXVec3Normalize(&vAxis[1], &vAxis[1]);
	
	vAxis[2].x = 0.0f;
	vAxis[2].y = 0.0f;
	vAxis[2].z = vCenter.z;
	D3DXVec3Normalize(&vAxis[2], &vAxis[2]);
	
	pOBBData->m_kCenter = vCenter;	
	pOBBData->m_afExtent[0] = vExtent.x;
	pOBBData->m_afExtent[1] = vExtent.y;
	pOBBData->m_afExtent[2] = vExtent.z;
	memcpy( pOBBData->m_akAxis, vAxis, sizeof(D3DXVECTOR3) * 3 );

	float fMagnitude  = sqrt( (pOBBData->m_kCenter.x  * pOBBData->m_kCenter.x) + 
								   (pOBBData->m_kCenter.y  * pOBBData->m_kCenter.y) + 
								   (pOBBData->m_kCenter.z  * pOBBData->m_kCenter.z) );
	float fMagnitude2 = sqrt( (pOBBData->m_afExtent[0] * pOBBData->m_afExtent[0]) + 
								   (pOBBData->m_afExtent[1] * pOBBData->m_afExtent[1]) + 
								   (pOBBData->m_afExtent[2] * pOBBData->m_afExtent[2]) );	
	pOBBData->m_Radius = fMagnitude + fMagnitude2;
	*/

	// Calculate obb center
	D3DXVECTOR3 m( 0, 0, 0 );
	
	int i = 0;for( ; i < vertexcount; ++i )
	{
		m.x += pVertex[i].x;
		m.y += pVertex[i].y;
		m.z += pVertex[i].z;
	}	
	
	m /= (float)vertexcount;
	
	float C11 = 0, C22 = 0, C33 = 0, C12 = 0, C13 = 0,  C23 = 0;
	
	for( i = 0; i < vertexcount; ++i )
	{
		C11 += ( pVertex[i].x - m.x ) * ( pVertex[i].x - m.x );
		C22 += ( pVertex[i].y - m.y ) * ( pVertex[i].y - m.y );
		C33 += ( pVertex[i].z - m.z ) * ( pVertex[i].z - m.z );
		C12 += ( pVertex[i].x - m.x ) * ( pVertex[i].y - m.y );
		C13 += ( pVertex[i].x - m.x ) * ( pVertex[i].z - m.z );
		C23 += ( pVertex[i].y - m.y ) * ( pVertex[i].z - m.z );
	}
	
	C11 /= vertexcount;
	C22 /= vertexcount;
	C33 /= vertexcount;
	C12 /= vertexcount;
	C13 /= vertexcount;
	C23 /= vertexcount;
	
	float Matrix[3][3] = 
	{
		{ C11, C12, C13 },
		{ C12, C22, C23 },
		{ C13, C23, C33 },
	};
	
	// jacobi
	float EigenVectors[3][3];
	float EigenValue[3];
	Jacobi( Matrix, EigenVectors, EigenValue );
	
	struct SORT{
		int ID;
		float Value;
	} Sort[3] = { { 0, EigenValue[0] }, { 1, EigenValue[1] }, { 2, EigenValue[2] } };
	
	int j = 0;for( ; j < 2; ++j ){
		int i = 0 ; for(  i = 2; i > j; --i ){
			if( Sort[i - 1].Value < Sort[i].Value ){
				SORT a = Sort[i];
				Sort[i] = Sort[i - 1];
				Sort[i - 1] = a;
			}
		}
	}
	
	D3DXVECTOR3 axis[3];
	for( i = 0; i < 3; ++i )
	{
		axis[i].x = EigenVectors[0][Sort[i].ID];
		axis[i].y = EigenVectors[1][Sort[i].ID];
		axis[i].z = EigenVectors[2][Sort[i].ID];
	}
	
	pOBBData->SetAxis( axis );
	
	float min[3] = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
	float max[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	
	for( j = 0; j < 3; ++j )
	{
		for( i = 0; i < vertexcount; ++i )
		{
			D3DXVECTOR3 vertex = D3DXVECTOR3( pVertex[i].x, pVertex[i].y, pVertex[i].z );
			
			float a = D3DXVec3Dot( &vertex, &pOBBData->Axis()[j] );
			if( min[j] > a ) min[j] = a;
			if( max[j] < a ) max[j] = a;
		}
	}
	
	pOBBData->SetCenter( pOBBData->Axis()[0] * ( ( min[0] + max[0] ) / 2.0f )
					   + pOBBData->Axis()[1] * ( ( min[1] + max[1] ) / 2.0f )
		               + pOBBData->Axis()[2] * ( ( min[2] + max[2] ) / 2.0f ) );
	
	D3DXVECTOR3 extentsvec = D3DXVECTOR3( (max[0] - min[0]) / 2.0f, (max[1] - min[1]) / 2.0f, (max[2] - min[2]) / 2.0f );
	pOBBData->SetExtents( extentsvec );
}