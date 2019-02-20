#include "Graphic.h"
Graphic::Graphic()
{
}

Graphic::~Graphic()
{
}
HRESULT Graphic::InitDevice(HWND hwnd)
{
	is_left_pressed = false;
	is_right_pressed = false;
	is_up_pressed = false;
	is_down_pressed = false;
	left_pressing_second = 0;
	right_pressing_second = 0;
	up_pressing_second = 0;

	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hwnd, &rc);
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
	sd.OutputWindow = hwnd;
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

	SetRenderTarget();
	//g_pImmediateContext->OMSetRenderTargets(1, &m_renderTargetView, g_pDepthStencilView);

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
	hr = CompileShaderFromFile(L"RenderCar.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX1 file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
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
	hr = CompileShaderFromFile(L"RenderCar.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX3 file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = g_pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;
	g_car = new CarModel(g_pImmediateContext, g_pd3dDevice);
	g_car->Init();
	g_car->PrepareRender();


	// Create the constant buffers
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
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
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_window.png", NULL, NULL, &g_pTextureCarWindow, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_window_back.png", NULL, NULL, &g_pTextureCarWindowBack, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/car_wheel.png", NULL, NULL, &g_pTextureCarWheel, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/black.bmp", NULL, NULL, &g_pTextureCarWheel1, NULL);
	D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/skybox.png", NULL, NULL, &g_pTextureSkyBox, NULL);
	//D3DX11CreateShaderResourceViewFromFile(g_pd3dDevice, L"Res/height1.png", NULL, NULL, &g_pTextureHeight, NULL);

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
	g_LMouseRotation = XMMatrixIdentity();
	g_RMouseRotation = XMMatrixIdentity();
	is_wheel_ture = false;
	// Initialize the view matrix

	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -8.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_Camera.Initialize(Eye, At, Up);

	XMVECTOR lightPos = XMVectorSet(-30.0f, 30.0f, 0.0f, 0.0f);
	XMVECTOR lightAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_LightView = XMMatrixLookAtLH(lightPos, lightAt, lightUp);

	CBChangesEveryFrame cbChangeFrame;
	cbChangeFrame.mWorld = XMMatrixTranspose(g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cbChangeFrame, 0, 0);

	// Initialize the projection matrix
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
	//平行光投影 用正交矩阵
	g_LightProj = XMMatrixOrthographicLH(70,50,0.01f,100.0f);
	CBNeverChanges cbNeverChange;
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, g_Projection);
	//see document https://docs.microsoft.com/en-us/windows/desktop/dxmath/pg-xnamath-getting-started#type_usage_guidelines_
	cbNeverChange.mLightProj = XMMatrixTranspose(g_LightProj);
	cbNeverChange.mProjection = XMMatrixTranspose(g_Projection);
	cbNeverChange.mLightView = XMMatrixTranspose(g_LightView);
	cbNeverChange.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbNeverChange.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbNeverChange.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbNeverChange.power = 5.0f;
	g_pImmediateContext->UpdateSubresource(g_pCBNeverChanges, 0, NULL, &cbNeverChange, 0, 0);

	CBRareChanges cbRareChange;
	cbRareChange.mEyePos = XMVector3Transform(g_Camera.GetEyePos(), g_World);
	cbRareChange.mView = XMMatrixTranspose(g_Camera.GetViewMatrix());
	cbRareChange.flag = 0;
	g_pImmediateContext->UpdateSubresource(g_pCBRareChanges, 0, NULL, &cbRareChange, 0, 0);

	Light dirLight;
	dirLight.pos = XMFLOAT4(-10.0f, 10.0f, 0.0f, 1.0f);
	dirLight.direction = XMFLOAT4(10.0f, -10.0f, 0.0f, 1.0f);
	dirLight._ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	dirLight._diffuse = XMFLOAT4(0.8f, 0.8f, 0.8, 1.0f);
	dirLight._specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_pImmediateContext->UpdateSubresource(g_Light, 0, NULL, &dirLight, 0, 0);

	g_renderSadowTexture = new RenderShadowTexture();
	g_renderSadowTexture->Initialize(g_pd3dDevice, 800, 600, 0, 0);

	g_shadowShader = new ShadowShader();
	g_shadowShader->Initialize(g_pd3dDevice, hwnd, L"depth.vs", L"depth.ps");
	return S_OK;

}
void Graphic::CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();

	if (g_pSamplerLinear) g_pSamplerLinear->Release();
	if (g_pTextureRV) g_pTextureRV->Release();
	if (g_pTextureGround) g_pTextureGround->Release();
	if (g_pTextureGround) g_pTextureCarFront->Release();

	if (g_pCBNeverChanges) g_pCBNeverChanges->Release();
	if (g_pCBChangesEveryFrame) g_pCBChangesEveryFrame->Release();
	
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (g_car) g_car->Release();
	delete g_car;
	//delete g_renderSadowTexture;
	//delete g_shadowShader;
}
void Graphic::Render()
{
	
	XMMATRIX translation = XMMatrixIdentity();
	XMMATRIX rotation = XMMatrixIdentity();
	XMMATRIX rotation_skybox = XMMatrixIdentity();
	FLOAT rotateDelta = 0.1;
	/******update world matrix******/
	//判断当前鼠标键盘状态，改变WORLD 矩阵
	//上键正在按下 
	if (is_up_pressed)
	{
		up_pressing_second += 1;
		FLOAT delta = GenSpeedDelta(up_pressing_second, true);
		FLOAT delta2 = GenRotateDelta(up_pressing_second, true);
		translation = XMMatrixTranslation(delta*g_World._31, delta*g_World._32, delta*g_World._33);
		g_WheelRotationY = XMMatrixIdentity();
		g_WheelRotationX *= XMMatrixRotationX(10*delta2*3.1415926 / 180);

		g_World = g_World * translation;
	}
	//下键正在按下
	else if (is_down_pressed)
	{
		up_pressing_second -= 1;
		FLOAT delta = GenSpeedDelta(up_pressing_second, true);
		FLOAT delta2 = GenRotateDelta(up_pressing_second, true);
		translation = XMMatrixTranslation(delta* g_World._31, delta*g_World._32, delta*g_World._33);

		g_WheelRotationY = XMMatrixIdentity();
		g_WheelRotationX *= XMMatrixRotationX(10*delta2* 3.1415926 / 180);
		g_World = g_World * translation;
	}
	else
	{
		if (up_pressing_second > 0)
		{
			up_pressing_second -= 1;
			FLOAT delta = GenSpeedDelta(up_pressing_second, false);
			FLOAT delta2 = GenRotateDelta(up_pressing_second, false);
			translation = XMMatrixTranslation(delta* g_World._31, delta*g_World._32, delta*g_World._33);
			g_WheelRotationY = XMMatrixIdentity();
			g_WheelRotationX *= XMMatrixRotationX(10*delta2*3.1415926 / 180);
		}
		else if (up_pressing_second < 0)
		{
			up_pressing_second += 1;
			FLOAT delta = GenSpeedDelta(up_pressing_second, false);
			FLOAT delta2 = GenRotateDelta(up_pressing_second, false);
			translation = XMMatrixTranslation(delta* g_World._31, delta*g_World._32, delta*g_World._33);

			g_WheelRotationY = XMMatrixIdentity();
			g_WheelRotationX *= XMMatrixRotationX(10*delta2*3.1415926 / 180);
		}

		g_World = g_World * translation;

	}
	//左键正在按下
	if (is_left_pressed)
	{
		left_pressing_second += 1;
		rotateDelta = 0.1*GenRotateDelta(up_pressing_second, false);
		rotation = XMMatrixRotationY(-5 * rotateDelta * 3.1415926 / 180);
		rotation_skybox = XMMatrixRotationY(3 * rotateDelta * 3.1415926 / 180); 
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
	//右键正在按下
	else if (is_right_pressed)
	{
		right_pressing_second += 1;
		rotateDelta = 0.1*GenRotateDelta(up_pressing_second, false);
		rotation = XMMatrixRotationY(5 * rotateDelta *3.1415926 / 180); //XMMatrixRotationY();
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

	//轮胎到车的model矩阵
	XMMATRIX mTranslate = XMMatrixTranslation(-1.3f, 0.35f, 1.1f);
	g_FrontLeftWheelToCar = mTranslate;
	mTranslate = XMMatrixTranslation(1.3f, 0.35f, 1.1f);
	g_FrontRightWheelToCar = mTranslate;
	mTranslate = XMMatrixTranslation(-1.3f, 0.35f, -1.1f);
	g_BackLeftWheelToCar = mTranslate;
	mTranslate = XMMatrixTranslation(1.3f, 0.35f, -1.1f);
	g_BackRightWheelToCar = mTranslate;
	/******update camera******/
	g_LMouseRotation *= XMMatrixRotationY(g_leftMouseXDelta/10 * 3.1415926 / 180);
	g_LMouseRotation *= XMMatrixRotationX(g_leftMouseYDelta / 10 * 3.1415926 / 180);
	g_RMouseRotation *= XMMatrixRotationY(g_rightMouseXDelta / 10 * 3.1415926 / 180);
	g_RMouseRotation *= XMMatrixRotationX(g_rightMouseYDelta / 10 * 3.1415926 / 180);
	XMVECTOR Eye,At,Up;
	// 更新第一人称视角
	Eye = XMVector3Transform(XMVectorSet(0.0f, 4.0f, -8.0f, 0.0f), g_LMouseRotation*g_World);//is 3,not 4
	At = XMVector3Transform(XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f),  g_World);//is 3,not 4
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_Camera.Initialize(Eye, At, Up);
	// 更新第三人称视角
	XMVECTOR lookat = At - Eye;
	lookat = XMVector3Transform(lookat, g_RMouseRotation);
	
	At =  Eye+ lookat;
	XMFLOAT4 v2F;    
	XMStoreFloat4(&v2F, At);
	At = XMVectorSet(v2F.x,v2F.y,v2F.z,1.0f);
	g_Camera.Initialize(Eye, At, Up);/**/

	g_car->PrepareRender();
	//首先，渲染阴影到纹理
	RenderShadow(g_World,g_LightView,g_LightProj);
	
	
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	CBChangesEveryFrame cbChangeFrame;
	cbChangeFrame.mWorld = XMMatrixTranspose(g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cbChangeFrame, 0, 0);

	CBRareChanges cbRareChange;
	cbRareChange.mEyePos = XMVector3Transform(g_Camera.GetEyePos(), g_World);
	cbRareChange.mView = XMMatrixTranspose(g_Camera.GetViewMatrix());
	cbRareChange.flag = 0;
	g_pImmediateContext->UpdateSubresource(g_pCBRareChanges, 0, NULL, &cbRareChange, 0, 0);
	//绑定shader
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pCBNeverChanges);
	g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pCBChangesEveryFrame);
	g_pImmediateContext->VSSetConstantBuffers(2, 1, &g_pCBRareChanges);
	g_pImmediateContext->VSSetConstantBuffers(3, 1, &g_Light);

	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pImmediateContext->PSSetConstantBuffers(1, 2, &g_pCBChangesEveryFrame);
	g_pImmediateContext->PSSetConstantBuffers(3, 1, &g_Light);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarHead);
	ID3D11ShaderResourceView* tex = g_renderSadowTexture->GetShaderResourceView();
	g_pImmediateContext->PSSetShaderResources(1, 1, &tex);//
	g_pImmediateContext->PSSetShaderResources(2, 1, &g_pTextureHeight);
	g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);

	//画天空盒
	CBChangesEveryFrame cb_sky;
	XMVECTOR v2 = g_Camera.GetEyePos();
	XMStoreFloat4(&v2F, v2);
	XMMATRIX temp = g_SkyBoxRotation*XMMatrixTranslation(v2F.x,v2F.y,v2F.z);
	cb_sky.mWorld = XMMatrixTranspose(temp);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb_sky, 0, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureSkyBox);

	g_pImmediateContext->DrawIndexed(36, 162, 0);
	//clear the depth buffer
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//draw skybox end

	//draw car begin
	CBChangesEveryFrame cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarHead);
	g_pImmediateContext->DrawIndexed(6, 0, 0);//162---------------------------------------

	//car back
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureCarHead);
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
	//draw car end

	//draw ground
	CBChangesEveryFrame cb5;
	cb5.mWorld = XMMatrixIdentity();//XMMatrixTranspose(XMMatrixTranslation(-32.0f, 0.0f, -32.0f));
	g_pImmediateContext->UpdateSubresource(g_pCBChangesEveryFrame, 0, NULL, &cb5, 0, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTextureGround);
	g_pImmediateContext->DrawIndexed(6, 198, 0);

	g_pSwapChain->Present(0, 0);
}
FLOAT Graphic::GenSpeedDelta(int second, int is_pressing)
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
FLOAT Graphic::GenRotateDelta(int second, int is_pressing)
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
		return data * 0.0001;
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
			return  data * 0.0001;
		}
		else if (data < -100)
		{
			if (data < -1000)
			{
				data = -1000;
				up_pressing_second = -1000;
			}
			return  data * 0.0001;
		}

		return 0;
	}
}

HRESULT Graphic::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
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

void Graphic::SetState(bool is_wheel_ture,
	bool is_left_pressed,
	bool is_right_pressed,
	bool is_up_pressed,
	bool is_down_pressed,
	float rightMouseDeltaX,
	float rightMouseDeltaY,
	float leftMouseDeltaX,
	float leftMouseDeltaY)
{
	this->is_wheel_ture = is_wheel_ture;
	this->is_left_pressed = is_left_pressed;
	this->is_right_pressed = is_right_pressed;
	this->is_up_pressed = is_up_pressed;
	this->is_down_pressed = is_down_pressed;

	this->g_leftMouseXDelta = leftMouseDeltaX;
	this->g_leftMouseYDelta = leftMouseDeltaY;
	this->g_rightMouseXDelta = rightMouseDeltaX;
	this->g_rightMouseYDelta = rightMouseDeltaY;
}

void Graphic::RenderShadow(XMMATRIX world,XMMATRIX view, XMMATRIX proj)
{
	g_renderSadowTexture->SetRenderTarget(g_pImmediateContext);
	g_renderSadowTexture->ClearRenderTarget(g_pImmediateContext, 0.0f, 0.0f, 0.0f, 1.0f);

	g_car->PrepareRender();
	g_shadowShader->Render(g_pImmediateContext,world,view,proj);
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
	//set the render target back
	SetRenderTarget();
}
void Graphic::SetRenderTarget()
{
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}