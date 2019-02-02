//--------------------------------------------------------------------------------------
// File: Tutorial07.cpp
//
// This application demonstrates texturing
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "resource.h"
#include "common.h"
#include "Camera.h"
#include "Light.h"
//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = NULL;
HWND                                g_hWnd = NULL;
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
ID3D11Buffer*                       g_pVertexBuffer = NULL;
ID3D11Buffer*                       g_pIndexBuffer = NULL;
ID3D11Buffer*                       g_pCBNeverChanges = NULL;
ID3D11Buffer*                       g_pCBRareChanges = NULL;
ID3D11Buffer*                       g_pCBChangesEveryFrame = NULL;
ID3D11Buffer*                       g_Light = NULL;


ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11ShaderResourceView*           g_pTextureGround = NULL;
ID3D11ShaderResourceView*           g_pTextureCarFront = NULL;
ID3D11ShaderResourceView*           g_pTextureCarSide = NULL;
ID3D11ShaderResourceView*           g_pTextureCarHead = NULL;
ID3D11ShaderResourceView*           g_pTextureCarHead1 = NULL;
ID3D11ShaderResourceView*           g_pTextureCarWindow = NULL;
ID3D11ShaderResourceView*           g_pTextureCarWindowBack = NULL;

ID3D11ShaderResourceView*           g_pTextureCarWheel = NULL;
ID3D11ShaderResourceView*           g_pTextureCarWheel1 = NULL;
ID3D11ShaderResourceView*			g_pTextureSkyBox = NULL;


ID3D11SamplerState*                 g_pSamplerLinear = NULL;
XMMATRIX                            g_World;
//XMMATRIX                            g_View;
XMMATRIX                            g_Projection;
XMFLOAT4                            g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);
XMMATRIX							g_FrontLeftWheelToCar;
XMMATRIX							g_FrontRightWheelToCar;
XMMATRIX							g_BackLeftWheelToCar;
XMMATRIX							g_BackRightWheelToCar;
XMMATRIX                            g_SkyBoxRotation;


XMMATRIX							g_WheelRotationY;
XMMATRIX							g_WheelRotationX;

bool								is_wheel_ture;
bool								is_left_pressed;
bool								is_right_pressed;
bool								is_up_pressed;
bool								is_down_pressed;
bool								is_mouseleft_pressed;
int									left_pressing_second;
int									right_pressing_second;
int									up_pressing_second;
Camera								g_Camera;
FLOAT								g_mouseXDelta = 0.0f;
FLOAT								g_mouseYDelta = 0.0f;
FLOAT								lastMouseX = 0.0f;
FLOAT								lastMouseY = 0.0f;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Render();
FLOAT GenSpeedDelta(int second, int is_pressing);
FLOAT GenRotateDelta(int second, int is_pressing);

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitWindow(hInstance, nCmdShow)))
		return 0;

	if (FAILED(InitDevice()))
	{
		CleanupDevice();
		return 0;
	}

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}

	CleanupDevice();

	return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 7", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
	is_left_pressed = false;
	is_right_pressed = false;
	is_up_pressed = false;
	is_down_pressed = false;
	is_mouseleft_pressed = false;
	left_pressing_second = 0;
	right_pressing_second = 0;
	up_pressing_second = 0;

	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr))
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);

	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"Tutorial07.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = g_pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = g_pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &g_pVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"Tutorial07.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	XMFLOAT3 LeftFrontDown = XMFLOAT3(-1.0f, 0.65f, -2.0f);
	XMFLOAT3 RightFrontDown = XMFLOAT3(1.0f, 0.65f, -2.0f);
	XMFLOAT3 RightFrontUp = XMFLOAT3(1.0f, 1.15f, -2.0f);
	XMFLOAT3 LeftFrontUp = XMFLOAT3(-1.0f, 1.15f, -2.0f);

	XMFLOAT3 LeftBackDown = XMFLOAT3(-1.0f, 0.65f, 2.0f);
	XMFLOAT3 RightBackDown = XMFLOAT3(1.0f, 0.65f, 2.0f);
	XMFLOAT3 RightBackUp = XMFLOAT3(1.0f, 1.15f, 2.0f);
	XMFLOAT3 LeftBackUp = XMFLOAT3(-1.0f, 1.15f, 2.0f);

	XMFLOAT3 carLeftFrontDown = XMFLOAT3(-1.0f, 1.15f, -1.2f);
	XMFLOAT3 carRightFrontDown = XMFLOAT3(1.0f, 1.15f, -1.2f);
	XMFLOAT3 carRightFrontUp = XMFLOAT3(1.0f, 1.6f, -0.2f);
	XMFLOAT3 carLeftFrontUp = XMFLOAT3(-1.0f, 1.6f, -0.2f);
	XMFLOAT3 carLeftBackDown = XMFLOAT3(-1.0f, 1.15f, 1.2f);
	XMFLOAT3 carRightBackDown = XMFLOAT3(1.0f, 1.15f, 1.2f);
	XMFLOAT3 carRightBackUp = XMFLOAT3(1.0f, 1.6f, 0.2f);
	XMFLOAT3 carLeftBackUp = XMFLOAT3(-1.0f, 1.6f, 0.2f);

	// Create vertex buffer
	SimpleVertex vertices[] =
	{	//front
		{ LeftFrontDown, XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(-1,-1,-1)},
		{ RightFrontDown,   XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(1,-1,-1)},
		{ RightFrontUp,   XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(1,1,-1)},
		{ LeftFrontUp,  XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(-1,1,-1)},
		//back
		{ LeftBackDown, XMFLOAT2(0.0f, 1.0f),   XMFLOAT3(-1,-1,1)},
		{ RightBackDown, XMFLOAT2(1.0f, 1.0f),    XMFLOAT3(1,-1,1)},
		{ RightBackUp, XMFLOAT2(1.0f, 0.0f),    XMFLOAT3(1,1,1)},
		{ LeftBackUp, XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(-1,1,1)},
		//left
		{ LeftBackDown, XMFLOAT2(0.0f, 1.0f) ,XMFLOAT3(-1,-1,1)},
		{ LeftFrontDown, XMFLOAT2(1.0f, 1.0f) ,XMFLOAT3(-1,-1,-1)},
		{ LeftFrontUp, XMFLOAT2(1.0f, 0.0f) ,XMFLOAT3(-1,1,-1)},
		{ LeftBackUp, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,1,1)},
		//right
		{ RightBackDown, XMFLOAT2(0.0f, 1.0f) ,XMFLOAT3(1,-1,1)},
		{ RightFrontDown, XMFLOAT2(1.0f, 1.0f) ,XMFLOAT3(1,-1,-1)},
		{ RightFrontUp, XMFLOAT2(1.0f, 0.0f) ,XMFLOAT3(1,1,-1)},
		{ RightBackUp, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,1,1)},
		//up
		{ LeftFrontUp, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,1,-1)},
		{ RightFrontUp, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,1,-1)},
		{ RightBackUp, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,1,1)},
		{ LeftBackUp, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,1,1)},
		//down
		{ LeftFrontDown, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,-1,-1)},
		{ LeftBackDown, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,-1,1)},
		{ RightBackDown, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,-1,1)},
		{ RightFrontDown, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,-1,-1)},


		// car top begin with 24
		//front
		{ carLeftFrontDown, XMFLOAT2(0.0f, 1.0f) ,XMFLOAT3(-1,1,-0.45)},
		{ carRightFrontDown, XMFLOAT2(1.0f, 1.0f) ,XMFLOAT3(1,1,-0.45)},
		{ carRightFrontUp, XMFLOAT2(1.0f, 0.0f) ,XMFLOAT3(1,1,-0.45)},
		{ carLeftFrontUp, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,1,-0.45)},
		//back

		{ carRightBackDown,  XMFLOAT2(0.0f, 1.0f) ,XMFLOAT3(1,1,0.45)},
		{ carLeftBackDown,XMFLOAT2(1.0f, 1.0f) ,XMFLOAT3(-1,1,0.45)},
		{ carLeftBackUp,XMFLOAT2(1.0f, 0.0f) ,XMFLOAT3(-1,1,0.45)},
		{ carRightBackUp,  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,1,0.45)},

		//left
		{ carLeftBackDown,  XMFLOAT2(0.0f, 1.0f) ,XMFLOAT3(-1,1,0.45)},
		{ carLeftFrontDown,XMFLOAT2(1.0f, 1.0f) ,XMFLOAT3(-1,1,-0.45)},
		{ carLeftFrontUp,XMFLOAT2(1.0f, 0.0f) ,XMFLOAT3(-1,1,-0.45)},
		{ carLeftBackUp,  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,1,0.45)},
		//right
		{ carRightFrontDown, XMFLOAT2(0.0f, 1.0f) ,XMFLOAT3(1,1,-0.45)},
		{ carRightBackDown, XMFLOAT2(1.0f, 1.0f) ,XMFLOAT3(1,1,0.45)},
		{ carRightBackUp, XMFLOAT2(1.0f, 0.0f) ,XMFLOAT3(1,1,0.45)},
		{ carRightFrontUp, XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,1,-0.45)},
		//up
		{ carLeftFrontUp, XMFLOAT2(0.0f, 1.0f) ,XMFLOAT3(-1,1,-0.45)},
		{ carRightFrontUp, XMFLOAT2(1.0f, 1.0f) ,XMFLOAT3(1,1,-0.45)},
		{ carRightBackUp, XMFLOAT2(1.0f, 0.0f) ,XMFLOAT3(1,1,0.45)},
		{ carLeftBackUp,  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,1,0.45)},
		//wheel,leftface44
		{ XMFLOAT3(-0.1f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,0,0)},
		{ XMFLOAT3(-0.1f, -0.15f, 0.36f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,0,0)},
		{ XMFLOAT3(-0.1f, -0.36f, 0.15f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,0,0)},
		{ XMFLOAT3(-0.1f, -0.36f, -0.15f), XMFLOAT2(0.0f, 0.0f),XMFLOAT3(-1,0,0) },
		{ XMFLOAT3(-0.1f, -0.15f, -0.36f), XMFLOAT2(0.0f, 0.0f),XMFLOAT3(-1,0,0) },
		{ XMFLOAT3(-0.1f, 0.15f, -0.36f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,0,0)},
		{ XMFLOAT3(-0.1f, 0.36f, -0.15f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,0,0)},
		{ XMFLOAT3(-0.1f, 0.36f, 0.15f),  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,0,0)},
		{ XMFLOAT3(-0.1f, 0.15f,  0.36f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(-1,0,0)},


		//wheel,rightface53
		{ XMFLOAT3(0.1f, 0.0f, 0.0f),	  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,0,0)},
		{ XMFLOAT3(0.1f, -0.15f, -0.36f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,0,0)},
		{ XMFLOAT3(0.1f, -0.36f, -0.15f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,0,0)},
		{ XMFLOAT3(0.1f, -0.36f, 0.15f),  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,0,0)},
		{ XMFLOAT3(0.1f, -0.15f, 0.36f),  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,0,0)},
		{ XMFLOAT3(0.1f, 0.15f,  0.36f),  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,0,0)},
		{ XMFLOAT3(0.1f, 0.36f, 0.15f),   XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,0,0)},
		{ XMFLOAT3(0.1f, 0.36f, -0.15f),  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,0,0)},
		{ XMFLOAT3(0.1f, 0.15f, -0.36f),  XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(1,0,0)},
		//skybox begin with 62
		//front
		{ XMFLOAT3(-0.1f,3.9f,-6.1f), XMFLOAT2(0.26f,0.65f) },
		{ XMFLOAT3(0.1f,3.9f,-6.1f), XMFLOAT2(0.0f,0.65f) },
		{ XMFLOAT3(0.1f,4.1f,-6.1f), XMFLOAT2(0.0f,0.34f) },
		{ XMFLOAT3(-0.1f,4.1f,-6.1f), XMFLOAT2(0.26f,0.34f) },
		//back
		{ XMFLOAT3(0.1f,3.9f,-5.9f),  XMFLOAT2(0.75f,0.65f) },
		{ XMFLOAT3(-0.1f,3.9f,-5.9f),XMFLOAT2(0.49f, 0.65f) },
		{ XMFLOAT3(-0.1f,4.1f,-5.9f),XMFLOAT2(0.49f, 0.34f) },
		{ XMFLOAT3(0.1f,4.1f,-5.9f), XMFLOAT2(0.75f, 0.34f) },
		//left
		{ XMFLOAT3(-0.1f,3.9f,-5.9f),  XMFLOAT2(0.49f,0.65f) },
		{ XMFLOAT3(-0.1f,3.9f,-6.1f), XMFLOAT2(0.26f,0.65f) },
		{ XMFLOAT3(-0.1f,4.1f,-6.1f), XMFLOAT2(0.26f,0.34f) },
		{ XMFLOAT3(-0.1f,4.1f,-5.9f),  XMFLOAT2(0.49f,0.34f) },
		//right
		{ XMFLOAT3(0.1f,3.9f,-6.1f), XMFLOAT2(1.0f,0.65f) },
		{ XMFLOAT3(0.1f,3.9f,-5.9f), XMFLOAT2(0.75f,0.65f) },
		{ XMFLOAT3(0.1f,4.1f,-5.9f), XMFLOAT2(0.75f,0.34f) },
		{ XMFLOAT3(0.1f,4.1f,-6.1f), XMFLOAT2(1.0f,0.34f) },
		//up
		{ XMFLOAT3(-0.1f,4.1f,-6.1f),  XMFLOAT2(0.49f,0.0f)},
		{ XMFLOAT3(0.1f,4.1f,-6.1f),  XMFLOAT2(0.49f,0.34f)},
		{ XMFLOAT3(0.1f,4.1f,-5.9f),   XMFLOAT2(0.26f,0.34f)},
		{ XMFLOAT3(-0.1f,4.1f,-5.9f), XMFLOAT2(0.26f,0.0f)},
		//down
		{ XMFLOAT3(-0.1f,3.9f,-5.9f), XMFLOAT2(0.26f,0.65f) },
		{ XMFLOAT3(0.1f,3.9f,-5.9f), XMFLOAT2(0.26f,1.0f) },
		{ XMFLOAT3(0.1f,3.9f,-6.1f), XMFLOAT2(0.49f,1.0f) },
		{ XMFLOAT3(-0.1f,3.9f,-6.1f), XMFLOAT2(0.49f,0.65f) },




		//floor begin with 86
		{ XMFLOAT3(-50.0f, 0.0f, -50.0f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(0,1,0) },
		{ XMFLOAT3(50.0f, 0.0f, -50.0f), XMFLOAT2(100.0f, 0.0f) ,XMFLOAT3(0,1,0) },
		{ XMFLOAT3(50.0f, 0.0f, 50.0f), XMFLOAT2(100.0f, 100.0f) ,XMFLOAT3(0,1,0) },
		{ XMFLOAT3(-50.0f, 0.0f, 50.0f), XMFLOAT2(0.0f, 100.0f) ,XMFLOAT3(0,1,0) },



	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 90;//-----------------------------------------------
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// Create index buffer
	// Create vertex buffer
	WORD indices[] =
	{
		//box
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		20,23,22,
		20,22,21,
		//top
		24,27,26,
		24,26,25,

		28,31,30,
		28,30,29,

		32,35,34,
		32,34,33,

		36,39,38,
		36,38,37,

		40,43,42,
		40,42,41,
		//wheel leftface
		44,46,45,
		44,47,46,
		44,48,47,
		44,49,48,
		44,50,49,
		44,51,50,
		44,52,51,
		44,45,52,
		//wheel rightface
		53,55,54,
		53,56,55,
		53,57,56,
		53,58,57,
		53,59,58,
		53,60,59,
		53,61,60,
		53,54,61,
		//wheel rolls
		45,46,56,
		56,57,45,

		46,47,55,
		55,56,46,

		47,48,54,
		54,55,47,

		48,49,61,
		61,54,48,

		49,50,60,
		60,61,49,

		50,51,59,
		59,60,50,

		51,52,58,
		58,59,51,

		52,45,57,
		57,58,52,
		//skybox
		62,63,64,
		62,64,65,

		66,67,68,
		66,68,69,

		70,71,72,
		70,72,73,

		74,75,76,
		74,76,77,

		78,79,80,
		78,80,81,

		82,83,84,
		82,84,85,

		//floor
		86,88,87,
		86,89,88,


	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 204;//---------------------------------------------
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	if (FAILED(hr))
		return hr;

	// Set index buffer
	g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffers
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBNeverChanges);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBNeverChanges);
	if (FAILED(hr))
		return hr;


	bd.ByteWidth = sizeof(CBChangesEveryFrame);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBChangesEveryFrame);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(CBRareChanges);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pCBRareChanges);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(Light);
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_Light);
	if (FAILED(hr))
		return hr;

	// Load the Texture
	hr = D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/seafloor.dds", NULL, NULL, &g_pTextureRV, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/base1.png", NULL, NULL, &g_pTextureGround, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_back.png", NULL, NULL, &g_pTextureCarFront, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_side.png", NULL, NULL, &g_pTextureCarSide, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_head.png", NULL, NULL, &g_pTextureCarHead, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_head.png", NULL, NULL, &g_pTextureCarHead1, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_window.png", NULL, NULL, &g_pTextureCarWindow, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_window_back.png", NULL, NULL, &g_pTextureCarWindowBack, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_wheel.png", NULL, NULL, &g_pTextureCarWheel, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/black.bmp", NULL, NULL, &g_pTextureCarWheel1, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/skybox.png", NULL, NULL, &g_pTextureSkyBox, NULL);




	if (FAILED(hr))
		return hr;

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
	if (FAILED(hr))
		return hr;

	// Initialize the world matrices
	g_World = XMMatrixIdentity();
	g_FrontLeftWheelToCar = XMMatrixIdentity();
	g_FrontRightWheelToCar = XMMatrixIdentity();
	g_BackLeftWheelToCar = XMMatrixIdentity();
	g_BackRightWheelToCar = XMMatrixIdentity();
	g_WheelRotationY = XMMatrixIdentity();
	g_WheelRotationX = XMMatrixIdentity();
	g_SkyBoxRotation = XMMatrixIdentity();

	is_wheel_ture = false;
	// Initialize the view matrix

	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -6.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 4.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_Camera.SetCameraPosition(Eye);
	g_Camera.SetTargetPosition(At);
	g_Camera.SetUpVector(Up);

	CBChangesEveryFrame cbChangeFrame;
	cbChangeFrame.mWorld = XMMatrixTranspose(g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cbChangeFrame, 0, 0);

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
	CBNeverChanges cbNeverChange;
	cbNeverChange.mProjection = XMMatrixTranspose(g_Projection);
	cbNeverChange.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbNeverChange.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbNeverChange.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbNeverChange.power = 5.0f;
	g_pImmediateContext->UpdateSubresource(g_pCBNeverChanges, 0, NULL, &cbNeverChange, 0, 0);

	CBRareChanges cbRareChange;
	cbRareChange.mEyePos = XMVector3Transform(XMVectorSet(0.0f, 4.0f, -6.0f, 0.0f), g_World);
	cbRareChange.mView = XMMatrixTranspose(g_Camera.GetViewMatrix());
	g_pImmediateContext->UpdateSubresource(g_pCBRareChanges, 0, NULL, &cbRareChange, 0, 0);


	Material material;
	material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.power = 5.0f;
	//g_pImmediateContext->UpdateSubresource(g_Material, 0, NULL, &material, 0, 0);

	Light dirLight;
	//dirLight.light_type = 0;
	dirLight.pos = XMFLOAT4(-10.0f,10.0f,0.0f,1.0f);
	dirLight.direction = XMFLOAT4(10.0f, -10.0f, 0.0f, 1.0f);
	dirLight._ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight._diffuse = XMFLOAT4(0.8f, 0.8f, 0.8, 1.0f);
	dirLight._specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_pImmediateContext->UpdateSubresource(g_Light, 0, NULL, &dirLight, 0, 0);
	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();

	if (g_pSamplerLinear) g_pSamplerLinear->Release();
	if (g_pTextureRV) g_pTextureRV->Release();
	if (g_pTextureGround) g_pTextureGround->Release();
	if (g_pTextureGround) g_pTextureCarFront->Release();

	if (g_pCBNeverChanges) g_pCBNeverChanges->Release();
	if (g_pCBChangesEveryFrame) g_pCBChangesEveryFrame->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		lastMouseX = LOWORD(lParam);
		lastMouseY = HIWORD(lParam);
		is_mouseleft_pressed = true;
		break;
	case WM_LBUTTONUP:
		lastMouseX = 0.0f;
		lastMouseY = 0.0f;
		is_mouseleft_pressed = false;
		break;
	case WM_MOUSEMOVE:
		if (is_mouseleft_pressed)
		{
			g_mouseXDelta = LOWORD(lParam) - lastMouseX;
			g_mouseYDelta = HIWORD(lParam) - lastMouseY;
			lastMouseX = LOWORD(lParam);
			lastMouseY = HIWORD(lParam);
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_MOUSEWHEEL:
		break;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_UP:
		case 'W':
			is_up_pressed = true;
			is_down_pressed = false;


			break;
		case VK_DOWN:
		case 'S':
			is_down_pressed = true;
			is_up_pressed = false;
			break;
		case VK_LEFT:
		case 'A':
			is_left_pressed = true;
			is_right_pressed = false;
			break;
		case VK_RIGHT:
		case 'D':
			is_right_pressed = true;
			is_left_pressed = false;
			break;
			//speed up
		case VK_SHIFT:
			//break;
		case VK_SPACE:
			break;
		}
		break;
	}
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_UP:
		case 'W':
			is_up_pressed = false;
			break;
		case VK_DOWN:
		case 'S':
			is_down_pressed = false;
			break;
		case VK_LEFT:
		case 'A':
			is_left_pressed = false;
			is_wheel_ture = false;
			break;
		case VK_RIGHT:
		case 'D':
			is_right_pressed = false;
			is_wheel_ture = false;
			break;
			//speed up
		case VK_SHIFT:
			//break;
		case VK_SPACE:
			break;
		}
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
	XMMATRIX mTranslate = XMMatrixIdentity();
	XMMATRIX rotation = XMMatrixIdentity();
	XMMATRIX rotation_skybox = XMMatrixIdentity();
	FLOAT rotateDelta = 0.1;


	if (is_up_pressed)
	{
		up_pressing_second += 1;
		FLOAT delta = GenSpeedDelta(up_pressing_second, true);
		FLOAT delta2 = GenRotateDelta(up_pressing_second, true);
		mTranslate = XMMatrixTranslation(delta*g_World._31, delta*g_World._32, delta*g_World._33);
		g_WheelRotationY = XMMatrixIdentity();
		g_WheelRotationX *= XMMatrixRotationX(delta2*3.1415926 / 180);

		g_World = g_World * mTranslate;
	}

	else if (is_down_pressed)
	{
		up_pressing_second -= 1;
		FLOAT delta = GenSpeedDelta(up_pressing_second, true);
		FLOAT delta2 = GenRotateDelta(up_pressing_second, true);
		mTranslate = XMMatrixTranslation(delta* g_World._31, delta*g_World._32, delta*g_World._33);

		g_WheelRotationY = XMMatrixIdentity();
		g_WheelRotationX *= XMMatrixRotationX(delta2* 3.1415926 / 180);
		g_World = g_World * mTranslate;
	}
	else
	{
		if (up_pressing_second > 0)
		{
			up_pressing_second -= 1;
			FLOAT delta = GenSpeedDelta(up_pressing_second, false);
			FLOAT delta2 = GenRotateDelta(up_pressing_second, false);
			mTranslate = XMMatrixTranslation(delta* g_World._31, delta*g_World._32, delta*g_World._33);
			g_WheelRotationY = XMMatrixIdentity();
			g_WheelRotationX *= XMMatrixRotationX(delta2*3.1415926 / 180);
		}
		else if (up_pressing_second < 0)
		{
			up_pressing_second += 1;
			FLOAT delta = GenSpeedDelta(up_pressing_second, false);
			FLOAT delta2 = GenRotateDelta(up_pressing_second, false);
			mTranslate = XMMatrixTranslation(delta* g_World._31, delta*g_World._32, delta*g_World._33);

			g_WheelRotationY = XMMatrixIdentity();
			g_WheelRotationX *= XMMatrixRotationX(delta2*3.1415926 / 180);
		}

		g_World = g_World * mTranslate;

	}

	if (is_left_pressed)
	{
		left_pressing_second += 1;
		rotateDelta = 0.1*GenRotateDelta(up_pressing_second, false);
		rotation = XMMatrixRotationY(-3 * rotateDelta * 3.1415926 / 180); //XMMatrixRotationY();
		rotation_skybox = XMMatrixRotationY(3 * rotateDelta * 3.1415926 / 180); //XMMatrixRotationY();
		if (is_wheel_ture)
		{
			g_WheelRotationY = XMMatrixRotationY(-20 * 3.1415926 / 180);
		}

		else
		{
			g_WheelRotationY = XMMatrixRotationY(-10 * 3.1415926 / 180);
			is_wheel_ture = true;
		}
		g_World = rotation * g_World;
	}
	else if (is_right_pressed)
	{
		right_pressing_second += 1;
		rotateDelta = 0.1*GenRotateDelta(up_pressing_second, false);
		rotation = XMMatrixRotationY(3 * rotateDelta *3.1415926 / 180); //XMMatrixRotationY();
		rotation_skybox = XMMatrixRotationY(-3 * rotateDelta *3.1415926 / 180); //XMMatrixRotationY();
		if (is_wheel_ture)
			g_WheelRotationY = XMMatrixRotationY(20 * 3.1415926 / 180);
		else
		{
			g_WheelRotationY = XMMatrixRotationY(10 * 3.1415926 / 180);
			is_wheel_ture = true;
		}

		g_World = rotation * g_World;
	}
	else
		g_WheelRotationY = XMMatrixIdentity();
	g_SkyBoxRotation *= rotation_skybox;

	// Update our time
	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;
	}

	// Rotate cube around the origin
	//g_World = XMMatrixRotationY( t );

	mTranslate = XMMatrixTranslation(-1.0f, 0.35f, 1.1f);
	g_FrontLeftWheelToCar = mTranslate;
	mTranslate = XMMatrixTranslation(1.0f, 0.35f, 1.1f);
	g_FrontRightWheelToCar = mTranslate;
	mTranslate = XMMatrixTranslation(-1.0f, 0.35f, -1.1f);
	g_BackLeftWheelToCar = mTranslate;
	mTranslate = XMMatrixTranslation(1.0f, 0.35f, -1.1f);
	g_BackRightWheelToCar = mTranslate;



	// Modify the color
	g_vMeshColor.x = (sinf(t * 1.0f) + 1.0f) * 0.5f;
	g_vMeshColor.y = (cosf(t * 3.0f) + 1.0f) * 0.5f;
	g_vMeshColor.z = (sinf(t * 5.0f) + 1.0f) * 0.5f;

	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//change view matrix to let camera follow the car
	/*XMVECTOR Eye = XMVector3Transform(XMVectorSet(0.0f, 4.0f, -6.0f, 0.0f), g_World);//is 3,not 4
	XMVECTOR At = XMVector3Transform(XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f), g_World);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_Camera.SetCameraPosition(Eye);
	g_Camera.SetTargetPosition(At);
	g_Camera.SetUpVector(Up);*/

	//g_Camera.RotateUp(0.1*g_mouseXDelta * 3.1415926 / 180);
	//g_Camera.RotateRight(0.1*g_mouseYDelta * 3.1415926 / 180);
	/*if (is_mouseleft_pressed)
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(g_hWnd, &p);
			FLOAT deltaX = p.x - lastMouseX;
			FLOAT deltaY = p.y - lastMouseY;
			g_Camera.RotateUp(0.1*deltaX * 3.1415926 / 180);
			g_Camera.RotateRight(0.1*deltaY * 3.1415926 / 180);
		}
	}*/



	CBChangesEveryFrame cbChangeFrame;
	cbChangeFrame.mWorld = XMMatrixTranspose(g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cbChangeFrame, 0, 0);

	CBRareChanges cbRareChange;
	cbRareChange.mEyePos = XMVector3Transform(XMVectorSet(0.0f, 4.0f, -6.0f, 0.0f), g_World);
	cbRareChange.mView = XMMatrixTranspose(g_Camera.GetViewMatrix());
	g_pImmediateContext->UpdateSubresource(g_pCBRareChanges, 0, NULL, &cbRareChange, 0, 0);

	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBNeverChanges);
	g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBChangesEveryFrame);
	g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBRareChanges);
	g_pImmediateContext->VSSetConstantBuffers( 3, 1, &g_Light );

	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pImmediateContext->PSSetConstantBuffers(1, 2, &g_pCBChangesEveryFrame);
	//g_pImmediateContext->PSSetConstantBuffers(2, 4, &g_Material);
	g_pImmediateContext->PSSetConstantBuffers(3, 1, &g_Light);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarHead);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);


	CBChangesEveryFrame cb_sky;
	XMMATRIX temp = XMMatrixTranslation(0.0f, -4.0f, 6.0f)*g_SkyBoxRotation*XMMatrixTranslation(0.0f, 4.0f, -6.0f);
	cb_sky.mWorld = XMMatrixTranspose(temp*g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb_sky, 0, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureSkyBox);

	g_pImmediateContext->DrawIndexed(36, 162, 0);
	//clear the depth buffer
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	/*draw skybox end*/

	/*draw car begin*/
	CBChangesEveryFrame cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
	g_pImmediateContext->DrawIndexed(6, 0, 0);//162---------------------------------------

	//car back

	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarHead1);
	g_pImmediateContext->DrawIndexed(6, 6, 0);
	//carside
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarSide);
	g_pImmediateContext->DrawIndexed(24, 12, 0);

	//car front
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarFront);
	g_pImmediateContext->DrawIndexed(6, 36, 0);
	//car backwindow
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWindowBack);
	g_pImmediateContext->DrawIndexed(6, 42, 0);
	//car window
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWindow);
	g_pImmediateContext->DrawIndexed(18, 48, 0);

	//draw front left wheel
	CBChangesEveryFrame cb1;
	cb1.mWorld = XMMatrixTranspose(g_WheelRotationX* g_WheelRotationY*g_FrontLeftWheelToCar*g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb1, 0, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel);
	g_pImmediateContext->DrawIndexed(3, 66, 0);
	g_pImmediateContext->DrawIndexed(3, 72, 0);
	g_pImmediateContext->DrawIndexed(3, 78, 0);
	g_pImmediateContext->DrawIndexed(3, 84, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->DrawIndexed(3, 69, 0);
	g_pImmediateContext->DrawIndexed(3, 75, 0);
	g_pImmediateContext->DrawIndexed(3, 81, 0);
	g_pImmediateContext->DrawIndexed(3, 87, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel);
	g_pImmediateContext->DrawIndexed(3, 90, 0);
	g_pImmediateContext->DrawIndexed(3, 96, 0);
	g_pImmediateContext->DrawIndexed(3, 102, 0);
	g_pImmediateContext->DrawIndexed(3, 108, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->DrawIndexed(3, 93, 0);
	g_pImmediateContext->DrawIndexed(3, 99, 0);
	g_pImmediateContext->DrawIndexed(3, 105, 0);
	g_pImmediateContext->DrawIndexed(3, 111, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel1);
	g_pImmediateContext->DrawIndexed(48, 114, 0);

	//draw front right wheel;
	CBChangesEveryFrame cb2;
	cb2.mWorld = XMMatrixTranspose(g_WheelRotationX*g_WheelRotationY*g_FrontRightWheelToCar*g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb2, 0, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel);
	g_pImmediateContext->DrawIndexed(3, 66, 0);
	g_pImmediateContext->DrawIndexed(3, 72, 0);
	g_pImmediateContext->DrawIndexed(3, 78, 0);
	g_pImmediateContext->DrawIndexed(3, 84, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->DrawIndexed(3, 69, 0);
	g_pImmediateContext->DrawIndexed(3, 75, 0);
	g_pImmediateContext->DrawIndexed(3, 81, 0);
	g_pImmediateContext->DrawIndexed(3, 87, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel);
	g_pImmediateContext->DrawIndexed(3, 90, 0);
	g_pImmediateContext->DrawIndexed(3, 96, 0);
	g_pImmediateContext->DrawIndexed(3, 102, 0);
	g_pImmediateContext->DrawIndexed(3, 108, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->DrawIndexed(3, 93, 0);
	g_pImmediateContext->DrawIndexed(3, 99, 0);
	g_pImmediateContext->DrawIndexed(3, 105, 0);
	g_pImmediateContext->DrawIndexed(3, 111, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel1);
	g_pImmediateContext->DrawIndexed(48, 114, 0);
	//draw back left wheel
	CBChangesEveryFrame cb3;
	cb3.mWorld = XMMatrixTranspose(g_WheelRotationX*g_BackLeftWheelToCar*g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb3, 0, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel);
	g_pImmediateContext->DrawIndexed(3, 66, 0);
	g_pImmediateContext->DrawIndexed(3, 72, 0);
	g_pImmediateContext->DrawIndexed(3, 78, 0);
	g_pImmediateContext->DrawIndexed(3, 84, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->DrawIndexed(3, 69, 0);
	g_pImmediateContext->DrawIndexed(3, 75, 0);
	g_pImmediateContext->DrawIndexed(3, 81, 0);
	g_pImmediateContext->DrawIndexed(3, 87, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel);
	g_pImmediateContext->DrawIndexed(3, 90, 0);
	g_pImmediateContext->DrawIndexed(3, 96, 0);
	g_pImmediateContext->DrawIndexed(3, 102, 0);
	g_pImmediateContext->DrawIndexed(3, 108, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->DrawIndexed(3, 93, 0);
	g_pImmediateContext->DrawIndexed(3, 99, 0);
	g_pImmediateContext->DrawIndexed(3, 105, 0);
	g_pImmediateContext->DrawIndexed(3, 111, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel1);
	g_pImmediateContext->DrawIndexed(48, 114, 0);
	//draw back right wheel
	CBChangesEveryFrame cb4;
	cb4.mWorld = XMMatrixTranspose(g_WheelRotationX*g_BackRightWheelToCar*g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb4, 0, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel);
	g_pImmediateContext->DrawIndexed(3, 66, 0);
	g_pImmediateContext->DrawIndexed(3, 72, 0);
	g_pImmediateContext->DrawIndexed(3, 78, 0);
	g_pImmediateContext->DrawIndexed(3, 84, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->DrawIndexed(3, 69, 0);
	g_pImmediateContext->DrawIndexed(3, 75, 0);
	g_pImmediateContext->DrawIndexed(3, 81, 0);
	g_pImmediateContext->DrawIndexed(3, 87, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel);
	g_pImmediateContext->DrawIndexed(3, 90, 0);
	g_pImmediateContext->DrawIndexed(3, 96, 0);
	g_pImmediateContext->DrawIndexed(3, 102, 0);
	g_pImmediateContext->DrawIndexed(3, 108, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureRV);
	g_pImmediateContext->DrawIndexed(3, 93, 0);
	g_pImmediateContext->DrawIndexed(3, 99, 0);
	g_pImmediateContext->DrawIndexed(3, 105, 0);
	g_pImmediateContext->DrawIndexed(3, 111, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarWheel1);
	g_pImmediateContext->DrawIndexed(48, 114, 0);
	/*draw car end*/

	//draw ground
	CBChangesEveryFrame cb5;
	cb5.mWorld = XMMatrixIdentity();
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb5, 0, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureGround);

	g_pImmediateContext->DrawIndexed(6, 198, 0);

	//Present our back buffer to our front buffer

	g_pSwapChain->Present(0, 0);
}
FLOAT GenSpeedDelta(int second, int is_pressing)
{
	FLOAT data = second;
	if (is_pressing)
	{
		if (data > 1000)
		{
			data = 1000;
			up_pressing_second = 1000;
		}
		if (data < -1000)
		{
			data = -1000;
			up_pressing_second = -1000;
		}
		return data * 0.00001;
	}
	else
	{
		if (data > 100)
		{
			if (data > 1000)
				data = 1000;
			return  data * 0.00001;
		}
		else if (data < -100)
		{
			if (data < -1000)
				data = -1000;
			return  data * 0.00001;
		}

		return 0;
	}


}

FLOAT GenRotateDelta(int second, int is_pressing)
{
	FLOAT data = second;
	if (is_pressing)
	{
		if (data > 1000)
		{
			data = 1000;
			up_pressing_second = 1000;
		}
		if (data < -1000)
		{
			data = -1000;
			up_pressing_second = -1000;
		}
		return data * 0.001;
	}
	else
	{
		if (data > 100)
		{
			if (data > 1000)
			{
				data = 1000;
				up_pressing_second = 1000;
			}
			return  data * 0.001;
		}
		else if (data < -100)
		{
			if (data < -1000)
			{
				data = -1000;
				up_pressing_second = -1000;
			}
			return  data * 0.001;
		}

		return 0;
	}
}
