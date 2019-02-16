#pragma once
#include <d3dx11.h>
#include <xnamath.h>
class Camera
{
public:
	Camera();
	~Camera();
	inline void SetCameraPosition(XMVECTOR pos) { m_vCameraPosition = pos; };
	inline void SetTargetPosition(XMVECTOR pos) { m_vTargetPosition = pos; };
	inline void SetUpVector(XMVECTOR vec) { m_vTempUp = vec; };
	XMMATRIX GetViewMatrix(); 
	XMMATRIX GetProjMatrix();
	void SetProjParameter(FLOAT fovy,FLOAT aspect,FLOAT znear,FLOAT zfar);
	void RotateRight(FLOAT degree);
	void RotateUp(FLOAT degree);
	void RotateLookAt(FLOAT degree);
	void MoveRight(FLOAT dist);
	void MoveUp (FLOAT dist);
	void MoveFar(FLOAT dist);
	XMVECTOR GetEyePos() { return m_vCameraPosition; };
	XMVECTOR GetAtPos() { return m_vTargetPosition; };
	XMVECTOR GetUpPos() { return m_vTempUp; };
	void Initialize(XMVECTOR,XMVECTOR,XMVECTOR);
	void MakeTransform(XMMATRIX m);

	XMVECTOR m_vRight;
	XMVECTOR m_vUp;
	XMVECTOR m_vLookAt;
	XMVECTOR m_vCameraPosition;
	XMVECTOR m_vTargetPosition;
	XMVECTOR m_vTempUp;

	FLOAT		m_Fov;
	FLOAT		m_Aspect;
	FLOAT		m_zNear;
	FLOAT		m_zFar;

	XMMATRIX m_matView;
	XMMATRIX m_matProj;
private:
	//º∆À„viewæÿ’Û
	void calcViewMatrix();
	void calcProjMatrix();
public:

};

