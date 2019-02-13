//--------------------------------------------------------------------------------------
// File: Tutorial07.cpp
//
// This application demonstrates texturing
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <windowsx.h>
#include "Graphic.h"
bool								is_wheel_ture;
bool								is_left_pressed;
bool								is_right_pressed;
bool								is_up_pressed;
bool								is_down_pressed;
bool								is_mouseleft_pressed;
bool								is_mouseright_pressed;


HINSTANCE                           g_hInst = NULL;
HWND                                g_hWnd = NULL;
Graphic*							g_graphic;

FLOAT								g_lmouseXDelta = 0.0f;
FLOAT								g_lmouseYDelta = 0.0f;
FLOAT								g_rmouseXDelta = 0.0f;
FLOAT								g_rmouseYDelta = 0.0f;
FLOAT								lastMouseX = 0.0f;
FLOAT								lastMouseY = 0.0f;
//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

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
	g_graphic = new Graphic();
	if (FAILED(g_graphic->InitDevice(g_hWnd)))
	{
		g_graphic->CleanupDevice();
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
			g_graphic->SetState(is_wheel_ture, is_left_pressed,	 is_right_pressed,
				 is_up_pressed,	 is_down_pressed, g_rmouseXDelta, g_rmouseYDelta,
				g_lmouseXDelta, g_lmouseYDelta);
			g_lmouseXDelta = 0;
			g_lmouseYDelta = 0;
			g_rmouseXDelta = 0;
			g_rmouseYDelta = 0;
			g_graphic->Render();
		}
	}

	g_graphic->CleanupDevice();

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
		lastMouseX = GET_X_LPARAM(lParam);
		lastMouseY = GET_Y_LPARAM(lParam);
		is_mouseleft_pressed = true;
		break;
	case WM_LBUTTONUP:
		lastMouseX = 0.0f;
		lastMouseY = 0.0f;
		is_mouseleft_pressed = false;
		break;
	case WM_RBUTTONDOWN:
		lastMouseX = GET_X_LPARAM(lParam);
		lastMouseY = GET_Y_LPARAM(lParam);
		is_mouseright_pressed = true;
		break;
	case WM_RBUTTONUP:
		lastMouseX = 0.0f;
		lastMouseY = 0.0f;
		is_mouseright_pressed = false;
		break;
	case WM_MOUSEMOVE:
		if (is_mouseleft_pressed) //|| is_mouseright_pressed)
		{
			g_lmouseXDelta = GET_X_LPARAM(lParam) - lastMouseX;
			g_lmouseYDelta = GET_Y_LPARAM(lParam) - lastMouseY;
			
		}
		else if (is_mouseright_pressed)
		{
			g_rmouseXDelta = GET_X_LPARAM(lParam) - lastMouseX;
			g_rmouseYDelta = GET_Y_LPARAM(lParam) - lastMouseY;
		}
		lastMouseX = GET_X_LPARAM(lParam);
		lastMouseY = GET_Y_LPARAM(lParam);
		break;
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


/*void RenderShadowTexture()
{
	g_pImmediateContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)800;
	vp.Height = (FLOAT)600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pImmediateContext->RSSetViewports(1, &vp);
	float color[4] = {0.0f,0.0f,0.0f,1.0f};
	g_pImmediateContext->ClearRenderTargetView(m_renderTargetView, color);
	// Clear the depth buffer.
	g_pImmediateContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//___________________________
	//_____________________________
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

}*/
