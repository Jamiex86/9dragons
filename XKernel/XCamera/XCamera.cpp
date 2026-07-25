#include "stdafx.h"
#include "XCamera.h"

_XCamera::_XCamera(_XCameraType p_Type, float fov, float aspect, float nearplane, float farplane)
{
	ZeroMembers();
	Set(p_Type, fov, aspect, nearplane, farplane);
}

void _XCamera::ZeroMembers(void)
{
	m_pos = m_move = mp_ypr = D3DXVECTOR3(0.0, 0.0, 0.0);

	m_look	= D3DXVECTOR3(0.0, 0.0, 1.0);
	m_up	= D3DXVECTOR3(0.0, 1.0, 0.0);
	m_right = D3DXVECTOR3(1.0, 0.0, 0.0);

	mp_Type = camFly;

	mp_fYaw =
	mp_fPitch = mp_fMinPitchLimit = mp_fMaxPitchLimit =
	mp_fRoll = mp_fMinRollLimit = mp_fMaxRollLimit = 0.0;
		
	D3DXMatrixIdentity(&mp_view_matrix);
	D3DXMatrixIdentity(&mp_proj_matrix);
	D3DXMatrixIdentity(&mp_viewproj_matrix);

	mp_fAspect = mp_fFov = mp_fNearPlane = mp_fFarPlane = 0.0;
}

//---------------------------------------------------------------------------
// Name: _XCamera::operator =
// Desc: 
//---------------------------------------------------------------------------
_XCamera& _XCamera::operator = (const _XCamera& inCam)
{
	mp_Type = inCam.mp_Type;

	mp_fFov = inCam.mp_fFov;
	mp_fAspect = inCam.mp_fAspect;
	mp_fNearPlane = inCam.mp_fNearPlane;
	mp_fFarPlane = inCam.mp_fFarPlane;

	mp_ypr = inCam.mp_ypr;
	m_pos = inCam.m_pos;
	m_move = inCam.m_move;

	mp_fYaw = inCam.mp_fYaw;
	mp_fPitch = inCam.mp_fPitch;
	mp_fRoll = inCam.mp_fRoll;
	mp_fMinRollLimit = inCam.mp_fMinRollLimit;
	mp_fMaxRollLimit = inCam.mp_fMaxRollLimit;
	mp_fMinPitchLimit = inCam.mp_fMinPitchLimit;
	mp_fMaxPitchLimit = inCam.mp_fMaxPitchLimit;

	m_look = inCam.m_look;
	m_up = inCam.m_up;
	m_right = inCam.m_right;

	mp_view_matrix = inCam.mp_view_matrix;
	mp_proj_matrix = inCam.mp_proj_matrix;
	mp_viewproj_matrix = inCam.mp_viewproj_matrix;

	return (*this);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::SetType(_XCameraType p_Type)
{
	mp_Type = p_Type;
	// just to check the limits
	AddYaw(0.0);
	AddPitch(0.0);
	AddRoll(0.0);
}

//---------------------------------------------------------------------------
// Name: _XCamera::Set()
// Desc: Set the parameters of a camera.
//---------------------------------------------------------------------------
void _XCamera::Set(_XCameraType p_Type, float fov, float aspect, float nearplane, float farplane)
{ 
	SetType(p_Type);
	mp_fFov = fov;
	mp_fAspect = aspect;
	mp_fNearPlane = nearplane;
	mp_fFarPlane = farplane;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::SetPitchLimits(float min, float max)
{
	mp_fMinPitchLimit = min;
	mp_fMaxPitchLimit = max;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::SetRollLimits(float min, float max)
{
	mp_fMinRollLimit = min;
	mp_fMaxRollLimit = max;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::AddYaw(float a)
{
	mp_ypr.x += a;
	mp_fYaw += a;
	mp_fYaw = fmod((double)mp_fYaw, (double)360.0);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::AddPitch(float a)
{
	mp_ypr.y += a;
	mp_fPitch += a;
	
	if(mp_Type == camFly)
	{
		mp_fPitch = fmod((double)mp_fPitch, (double)360.0);
	}
	else
	{
		if(mp_fPitch < mp_fMinPitchLimit)
			mp_fPitch = mp_fMinPitchLimit;
		else if(mp_fPitch > mp_fMaxPitchLimit)
			mp_fPitch = mp_fMaxPitchLimit;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::AddRoll(float a)
{
	mp_ypr.z += a;
	mp_fRoll += a;

	if(mp_Type == camFly)
	{
		mp_fRoll = fmod((double)mp_fRoll, (double)360.0);
	}
	else
	{
		if(mp_fRoll < mp_fMinRollLimit)
			mp_fRoll = mp_fMinRollLimit;
		else if(mp_fRoll > mp_fMaxRollLimit)
			mp_fRoll = mp_fMaxRollLimit;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
float _XCamera::GetYaw()
{
	return mp_fYaw;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
float _XCamera::GetPitch()
{
	return mp_fPitch;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
float _XCamera::GetRoll()
{
	return mp_fRoll;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::Set(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::MoveSidewards(float delta)
{
	m_move.x += delta;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::MoveUp(float delta)
{
	m_move.y += delta;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void _XCamera::MoveForward(float delta)
{
	m_move.z += delta;
}

//---------------------------------------------------------------------------
// Name: _XCamera::UpdateViewMatrix()
// Desc: Updates the view matrix.
//---------------------------------------------------------------------------
void _XCamera::UpdateViewMatrix()
{
	if(mp_Type == camFly)
	{
		// Base vector regeneration
		D3DXVec3Normalize(&m_look, &m_look);
		D3DXVec3Cross(&m_right, &m_up, &m_look);
		D3DXVec3Normalize(&m_right, &m_right);
		D3DXVec3Cross(&m_up, &m_look, &m_right);
		D3DXVec3Normalize(&m_up, &m_up);

		// Build the rotation matrices
		D3DXMATRIX YawMtx;
		D3DXMATRIX PitchMtx;
		D3DXMATRIX RollMtx;
	
		D3DXMatrixRotationAxis(&PitchMtx, &m_right, _X_RAD(mp_ypr.y));
		D3DXMatrixRotationAxis(&YawMtx, &m_up, _X_RAD(mp_ypr.x));
		D3DXMatrixRotationAxis(&RollMtx, &m_look, _X_RAD(mp_ypr.z));

		D3DXVec3TransformCoord(&m_look, &m_look, &YawMtx);
		D3DXVec3TransformCoord(&m_right, &m_right, &YawMtx);

		D3DXVec3TransformCoord(&m_look, &m_look, &PitchMtx);
		D3DXVec3TransformCoord(&m_up, &m_up, &PitchMtx);

		D3DXVec3TransformCoord(&m_right, &m_right, &RollMtx);
		D3DXVec3TransformCoord(&m_up, &m_up, &RollMtx);

		m_pos.y += m_right.y * m_move.x;
		m_pos.y += m_up.y * m_move.y;
		m_pos.y += m_look.y * m_move.z;
	}
	else
	{
		float theta, sinYaw, cosYaw, sinPitch, cosPitch, sinRoll, cosRoll;

		// Compute sines and cosines of the yaw, pitch, and roll angles
		theta  = _X_RAD(mp_fYaw);
		sinYaw = sinf(theta);
		cosYaw = cosf(theta);
	
		theta    = _X_RAD(mp_fPitch);
		sinPitch = sinf(theta);
		cosPitch = cosf(theta);
	
		theta   = _X_RAD(mp_fRoll);
		sinRoll = sinf(theta);
		cosRoll = cosf(theta);
	
		// Compute the orientation of the camera
		m_right.x = cosYaw * cosRoll + sinYaw * sinPitch * sinRoll;
		m_right.y = sinRoll * cosPitch;
		m_right.z = cosYaw * sinPitch * sinRoll - sinYaw * cosRoll;
	
		m_up.x = sinYaw * sinPitch * cosRoll - cosYaw * sinRoll;
		m_up.y = cosRoll * cosPitch;
		m_up.z = sinRoll * sinYaw + cosRoll * cosYaw * sinPitch;
	
		m_look.x = cosPitch * sinYaw;
		m_look.y = -sinPitch;
		m_look.z = cosPitch * cosYaw;
	}

	m_pos.x += m_right.x * m_move.x;
	m_pos.z += m_right.z * m_move.x;

	m_pos.x += m_up.x * m_move.y;
	m_pos.z += m_up.z * m_move.y;
	
	m_pos.x += m_look.x * m_move.z;
	m_pos.z += m_look.z * m_move.z;

	// Reset our rot/move vectors
	m_move = mp_ypr = D3DXVECTOR3(0.0, 0.0, 0.0);

	// Construct the view matrix
	mp_view_matrix._11 = m_right.x;
	mp_view_matrix._21 = m_right.y;
	mp_view_matrix._31 = m_right.z;
	mp_view_matrix._41 = -D3DXVec3Dot(&m_right, &m_pos);
	
	mp_view_matrix._12 = m_up.x;
	mp_view_matrix._22 = m_up.y;
	mp_view_matrix._32 = m_up.z;
	mp_view_matrix._42 = -D3DXVec3Dot(&m_up, &m_pos);
	
	mp_view_matrix._13 = m_look.x;
	mp_view_matrix._23 = m_look.y;
	mp_view_matrix._33 = m_look.z;
	mp_view_matrix._43 = -D3DXVec3Dot(&m_look, &m_pos);
	
	mp_view_matrix._14 = 0.0;
	mp_view_matrix._24 = 0.0;
	mp_view_matrix._34 = 0.0;
	mp_view_matrix._44 = 1.0;
}

//---------------------------------------------------------------------------
// Name: _XCamera::UpdateProjMatrix()
// Desc: Updates the proj matrix.
//---------------------------------------------------------------------------
void _XCamera::UpdateProjMatrix()
{
	D3DXMatrixPerspectiveFovLH(&mp_proj_matrix, _X_RAD(mp_fFov), mp_fAspect, mp_fNearPlane, mp_fFarPlane);
	//MatrixOrthoLH( &mp_proj_matrix, 1024.0f, 768.0f, mp_fNearPlane, mp_fFarPlane );	
}

//---------------------------------------------------------------------------
// Name: _XCamera::UpdateViewprojMatrix()
// Desc: Updates the viewproj matrix by multiply view by proj.
//---------------------------------------------------------------------------
void _XCamera::UpdateViewprojMatrix()
{
	mp_viewproj_matrix = mp_view_matrix * mp_proj_matrix;
}
