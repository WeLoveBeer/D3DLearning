#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "resource.h"
#include "common.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "CarModel.h"
#include "RenderShadowTexture.h"
#include "ShadowShader.h"
class Graphic
{
public:
	Graphic();
	~Graphic();
	HRESULT InitDevice(HWND);
	void CleanupDevice();
	void Render();
	FLOAT GenSpeedDelta(int second, int is_pressing);
	FLOAT GenRotateDelta(int second, int is_pressing);
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	void RenderShadow(XMMATRIX world, XMMATRIX view, XMMATRIX proj);
	void SetRenderTarget();
	void SetState(bool is_wheel_ture,
		bool is_left_pressed,
		bool is_right_pressed,
		bool is_up_pressed,
		bool is_down_pressed,
		float rightMouseDeltaX,
		float rightMouseDeltaY,
		float leftMouseDeltaX,
		float leftMouseDeltaY);

private:

	D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*                       g_pd3dDevice = NULL;
	ID3D11DeviceContext*                g_pImmediateContext = NULL;
	IDXGISwapChain*                     g_pSwapChain = NULL;
	ID3D11RenderTargetView*             g_pRenderTargetView = NULL;
	ID3D11Texture2D*                    g_pDepthStencil = NULL;
	ID3D11DepthStencilView*             g_pDepthStencilView = NULL;
	ID3D11VertexShader*                 g_pVertexShader = NULL;
	ID3D11PixelShader*                  g_pPixelShader = NULL;
	ID3D11InputLayout*                  g_pVertexLayout = NULL;
	ID3D11Buffer*                       g_pCBNeverChanges = NULL;
	ID3D11Buffer*                       g_pCBRareChanges = NULL;
	ID3D11Buffer*                       g_pCBChangesEveryFrame = NULL;
	ID3D11Buffer*                       g_Light = NULL;

	ID3D11Texture2D*					m_renderTargetTexture;
	ID3D11RenderTargetView*				m_renderTargetView;
	ID3D11DepthStencilView*				m_depthStencilView;
	ID3D11ShaderResourceView*			m_shaderResourceView;

	ID3D11ShaderResourceView*           g_pTextureRV = NULL;
	ID3D11ShaderResourceView*           g_pTextureGround = NULL;
	ID3D11ShaderResourceView*           g_pTextureCarFront = NULL;
	ID3D11ShaderResourceView*           g_pTextureCarSide = NULL;
	ID3D11ShaderResourceView*           g_pTextureCarHead = NULL;
	ID3D11ShaderResourceView*           g_pTextureCarHead1 = NULL;
	ID3D11ShaderResourceView*           g_pTextureCarWindow = NULL;
	ID3D11ShaderResourceView*           g_pTextureCarWindowBack = NULL;
	ID3D11ShaderResourceView*           g_pTextureHeight = NULL;

	ID3D11ShaderResourceView*           g_pTextureCarWheel = NULL;
	ID3D11ShaderResourceView*           g_pTextureCarWheel1 = NULL;
	ID3D11ShaderResourceView*			g_pTextureSkyBox = NULL;


	ID3D11SamplerState*                 g_pSamplerLinear = NULL;
	XMMATRIX                            g_World;
	//XMMATRIX                            g_View;
	XMMATRIX                            g_Projection;
	XMMATRIX							g_LightView;
	XMMATRIX							g_LightProj;
	//XMFLOAT4                            g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);
	XMMATRIX							g_FrontLeftWheelToCar;
	XMMATRIX							g_FrontRightWheelToCar;
	XMMATRIX							g_BackLeftWheelToCar;
	XMMATRIX							g_BackRightWheelToCar;
	XMMATRIX                            g_SkyBoxRotation;


	XMMATRIX							g_WheelRotationY;
	XMMATRIX							g_WheelRotationX;
	XMMATRIX							g_LMouseRotation;
	XMMATRIX							g_RMouseRotation;

	bool								is_wheel_ture;
	bool								is_left_pressed;
	bool								is_right_pressed;
	bool								is_up_pressed;
	bool								is_down_pressed;

	int									left_pressing_second;
	int									right_pressing_second;
	int									up_pressing_second;
	Camera								g_Camera;
	FLOAT								g_leftMouseXDelta = 0.0f;
	FLOAT								g_leftMouseYDelta = 0.0f;
	FLOAT								g_rightMouseXDelta = 0.0f;
	FLOAT								g_rightMouseYDelta = 0.0f;


	CarModel*							g_car;
	Model*								g_objModel;
	RenderShadowTexture*				g_renderSadowTexture;
	ShadowShader*						g_shadowShader;
};

