#pragma once
#include <d3d11.h>
#include <xnamath.h>
class RenderShadowTexture
{
public:
	RenderShadowTexture();
	~RenderShadowTexture();

	bool Initialize(ID3D11Device*, int, int, float, float);
	void SetRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();
private:
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;

	XMMATRIX m_orthoMatrix;
};

