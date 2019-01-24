#pragma once
#include <d3dx9.h>
class Camera
{
public:
	Camera(IDirect3DDevice9* pDevice);
	Camera();
	~Camera();
	inline void SetCameraPosition(D3DXVECTOR3 pos) { m_vCameraPosition = pos; };
	inline void SetTargetPosition(D3DXVECTOR3 pos) { m_vTargetPosition = pos; };
	inline void SetUpVector(D3DXVECTOR3 vec) { m_vTempUp = vec; };
	D3DXMATRIX GetViewMatrix(); 
	D3DXMATRIX GetProjMatrix(); 
	inline void SetDevice(IDirect3DDevice9* device) { m_pDevice = device; };
	void SetProjParameter(FLOAT fovy,FLOAT aspect,FLOAT znear,FLOAT zfar);
	void RotateRight(FLOAT degree);
	void RotateUp(FLOAT degree);
	void RotateLookAt(FLOAT degree);
	void MoveRight(FLOAT dist);
	void MoveUp (FLOAT dist);
	void MoveFar(FLOAT dist);

private:
	IDirect3DDevice9* m_pDevice;
	D3DXVECTOR3 m_vRight;
	D3DXVECTOR3 m_vUp;
	D3DXVECTOR3 m_vLookAt;
	D3DXVECTOR3 m_vCameraPosition;
	D3DXVECTOR3 m_vTargetPosition;
	D3DXVECTOR3 m_vTempUp;

	FLOAT		m_Fov;
	FLOAT		m_Aspect;
	FLOAT		m_zNear;
	FLOAT		m_zFar;

	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;
private:
	void calcViewMatrix();
	void calcProjMatrix();
public:

};

