/*
本文件负责：
初始化阴影着色器对象，载入用于渲染阴影的vs和ps，并绘制阴影
*/

#pragma once
#include <d3d11.h>
#include <d3dx11async.h>
#include <xnamath.h>
class ShadowShader
{
public:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	ShadowShader();
	~ShadowShader();
	bool Initialize(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void Render(ID3D11DeviceContext* deviceContext, XMMATRIX world, XMMATRIX view, XMMATRIX proj);
private:
	//vs shader pointer
	ID3D11VertexShader* m_vertexShader;
	//ps shader pointer
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;

	ID3D11Buffer* m_matrixBuffer;
};

