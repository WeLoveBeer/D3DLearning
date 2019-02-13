#include "Camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{
}
void Camera::Initialize(XMVECTOR eye, XMVECTOR at, XMVECTOR up)
{
	m_vCameraPosition = eye;
	m_vTargetPosition = at;
	m_vTempUp = up;

	m_vLookAt = at - eye;
	m_vLookAt = XMVector3Normalize(m_vLookAt);
	m_vRight = XMVector3Cross(up, m_vLookAt);
	m_vRight = XMVector3Normalize(m_vRight);
	m_vUp = XMVector3Cross(m_vLookAt, m_vRight);
	m_vUp = XMVector3Normalize(m_vUp);
}
void Camera::MakeTransform(XMMATRIX m)
{
	m_vCameraPosition = XMVector3Transform(m_vCameraPosition, m);
}

void Camera::calcViewMatrix()
{
	

	XMFLOAT4 right;
	XMFLOAT4 up;
	XMFLOAT4 lookat;
	XMStoreFloat4(&right, m_vRight);
	XMStoreFloat4(&up, m_vUp);
	XMStoreFloat4(&lookat, m_vLookAt);

	m_matView._11 = right.x;
	m_matView._12 = up.x;
	m_matView._13 = lookat.x;
	m_matView._14 = 0.0f;

	m_matView._21 = right.y;
	m_matView._22 = up.y;
	m_matView._23 = lookat.y;
	m_matView._24 = 0.0f;

	m_matView._31 = right.z;
	m_matView._32 = up.z;
	m_matView._33 = lookat.z;
	m_matView._34 = 0.0f;
	FLOAT val1 = XMVectorGetX(m_vRight)*XMVectorGetX(m_vCameraPosition) + 
		XMVectorGetY(m_vRight)*XMVectorGetX(m_vCameraPosition)+
		 XMVectorGetZ(m_vRight)*XMVectorGetZ(m_vCameraPosition) + 
		XMVectorGetW(m_vRight)*XMVectorGetW(m_vCameraPosition);
	FLOAT val2 = XMVectorGetX(m_vUp)*XMVectorGetX(m_vCameraPosition) +
		XMVectorGetY(m_vUp)*XMVectorGetY(m_vCameraPosition) +
		XMVectorGetZ(m_vUp)*XMVectorGetZ(m_vCameraPosition) +
		XMVectorGetW(m_vUp)*XMVectorGetW(m_vCameraPosition);
	FLOAT val3 = XMVectorGetX(m_vLookAt)*XMVectorGetX(m_vCameraPosition) +
		XMVectorGetY(m_vLookAt)*XMVectorGetY(m_vCameraPosition) +
		XMVectorGetZ(m_vLookAt)*XMVectorGetZ(m_vCameraPosition) +
		XMVectorGetW(m_vLookAt)*XMVectorGetW(m_vCameraPosition);
	
	
	m_matView._41 = -1*val1;
	m_matView._42 = -1 * val2;
	m_matView._43 = -1 * val3;
	m_matView._44 = 1.0f;
	//m_matView= XMMatrixLookAtLH(m_vCameraPosition,m_vTargetPosition,m_vTempUp);
}
void Camera::calcProjMatrix()
{
	m_matProj = XMMatrixPerspectiveFovLH(m_Fov,m_Aspect, m_zNear, m_zFar);
}
XMMATRIX Camera::GetViewMatrix()
{
	calcViewMatrix();
	return m_matView;
}
XMMATRIX Camera::GetProjMatrix()
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
	XMMATRIX rotation;
	rotation = XMMatrixRotationAxis(m_vRight, degree);
	m_vUp = XMVector3TransformCoord(m_vUp, rotation);
	m_vLookAt = XMVector3TransformCoord(m_vLookAt, rotation);
	m_vTargetPosition = m_vLookAt *XMVector3Length(m_vCameraPosition);
	
}
void Camera::RotateUp(FLOAT degree)
{
	XMMATRIX rotation;
	rotation = XMMatrixRotationAxis(m_vUp, degree);
	m_vRight = XMVector3TransformCoord(m_vRight, rotation);
	m_vLookAt = XMVector3TransformCoord(m_vLookAt, rotation);
	m_vTargetPosition = m_vLookAt * XMVector3Length(m_vCameraPosition);
}
void Camera::RotateLookAt(FLOAT degree)
{
	XMMATRIX rotation;
	rotation = XMMatrixRotationAxis(m_vLookAt, degree);
	m_vUp = XMVector3TransformCoord(m_vUp, rotation);
	m_vRight = XMVector3TransformCoord(m_vRight, rotation);
	m_vTargetPosition = m_vLookAt * XMVector3Length(m_vCameraPosition);
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