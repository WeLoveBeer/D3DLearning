#include "Camera.h"
Camera::Camera(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
}
Camera::Camera()
{

}

Camera::~Camera()
{
}


void Camera::calcViewMatrix()
{
	m_vLookAt = m_vTargetPosition - m_vCameraPosition;
	D3DXVec3Normalize(&m_vLookAt, &m_vLookAt);
	D3DXVec3Cross(&m_vRight, &m_vTempUp, &m_vLookAt);
	D3DXVec3Normalize(&m_vRight, &m_vRight);
	D3DXVec3Cross(&m_vUp, &m_vLookAt, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);




	m_matView._11 = m_vRight.x;
	m_matView._12 = m_vUp.x;
	m_matView._13 = m_vLookAt.x;
	m_matView._14 = 0.0f;

	m_matView._21 = m_vRight.y;
	m_matView._22 = m_vUp.y;
	m_matView._23 = m_vLookAt.y;
	m_matView._24 = 0.0f;

	m_matView._31 = m_vRight.z;
	m_matView._32 = m_vUp.z;
	m_matView._33 = m_vLookAt.z;
	m_matView._34 = 0.0f;

	m_matView._41 = -D3DXVec3Dot(&m_vRight, &m_vCameraPosition);
	m_matView._42 = -D3DXVec3Dot(&m_vUp, &m_vCameraPosition);
	m_matView._43 = -D3DXVec3Dot(&m_vLookAt, &m_vCameraPosition);
	m_matView._44 = 1.0f;
}
void Camera::calcProjMatrix()
{
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_Fov,m_Aspect, m_zNear, m_zFar);
}
D3DXMATRIX Camera::GetViewMatrix()
{
	calcViewMatrix();
	return m_matView;
}
D3DXMATRIX Camera::GetProjMatrix()
{
	calcProjMatrix();
	return m_matProj;
}
void Camera::SetProjParameter(FLOAT fovy, FLOAT aspect, FLOAT znear, FLOAT zfar)
{
	m_Aspect = aspect;
	m_Fov = fovy;
	m_zFar = zfar;
	m_zNear = znear;
}
void Camera::RotateRight(FLOAT degree)
{
	D3DXMATRIX rotation;
	D3DXMatrixRotationAxis(&rotation, &m_vRight, degree);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &rotation);
	D3DXVec3TransformCoord(&m_vLookAt, &m_vLookAt, &rotation);
	m_vTargetPosition = m_vLookAt * D3DXVec3Length(&m_vCameraPosition);
	
}
void Camera::RotateUp(FLOAT degree)
{
	D3DXMATRIX rotation;
	D3DXMatrixRotationAxis(&rotation, &m_vUp, degree);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &rotation);
	D3DXVec3TransformCoord(&m_vLookAt, &m_vLookAt, &rotation);
	m_vTargetPosition = m_vLookAt * D3DXVec3Length(&m_vCameraPosition);
}
void Camera::RotateLookAt(FLOAT degree)
{
	D3DXMATRIX rotation;
	D3DXMatrixRotationAxis(&rotation, &m_vLookAt, degree);
	D3DXVec3TransformCoord(&m_vUp, &m_vUp, &rotation);
	D3DXVec3TransformCoord(&m_vRight, &m_vRight, &rotation);
	m_vTargetPosition = m_vLookAt * D3DXVec3Length(&m_vCameraPosition);
}
void Camera::MoveRight(FLOAT dist)
{
	dist = -1 * dist;
	m_vCameraPosition += m_vRight * dist;
	m_vTargetPosition += m_vRight * dist;
}
void Camera::MoveUp(FLOAT dist)
{
	dist = -1 * dist;
	m_vCameraPosition += m_vUp * dist;
	m_vTargetPosition += m_vUp * dist;
}
void Camera::MoveFar(FLOAT dist)
{
	dist = -1 * dist;
	m_vCameraPosition += m_vLookAt * dist;
	m_vTargetPosition += m_vLookAt * dist;
}