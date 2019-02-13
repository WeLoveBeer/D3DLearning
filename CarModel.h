#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include "common.h"
class CarModel
{
public:
	CarModel(ID3D11DeviceContext* deviceContent,ID3D11Device* device);
	~CarModel();
	void Init();
	void PrepareRender();
	void Release();
private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ID3D11ShaderResourceView* m_Texture;
	float m_positionX, m_positionY, m_positionZ;

	ID3D11DeviceContext*	m_pDeviceContent;
	ID3D11Device*			m_pDevice;
};

