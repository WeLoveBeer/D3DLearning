#include "CarModel.h"



CarModel::CarModel(ID3D11DeviceContext* deviceContent, ID3D11Device* device)
{
	m_pDeviceContent = deviceContent;
	m_pDevice = device;
}


CarModel::~CarModel()
{
}

void CarModel::Init()
{
	HRESULT hr;

	XMFLOAT3 LeftFrontDown = XMFLOAT3(-1.0f, 0.65f, -2.0f);
	XMFLOAT3 RightFrontDown = XMFLOAT3(1.0f, 0.65f, -2.0f);
	XMFLOAT3 RightFrontUp = XMFLOAT3(1.0f, 1.15f, -2.0f);
	XMFLOAT3 LeftFrontUp = XMFLOAT3(-1.0f, 1.15f, -2.0f);

	XMFLOAT3 LeftBackDown = XMFLOAT3(-1.0f, 0.65f, 2.0f);
	XMFLOAT3 RightBackDown = XMFLOAT3(1.0f, 0.65f, 2.0f);
	XMFLOAT3 RightBackUp = XMFLOAT3(1.0f, 1.15f, 2.0f);
	XMFLOAT3 LeftBackUp = XMFLOAT3(-1.0f, 1.15f, 2.0f);
	//cartop
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
		{ XMFLOAT3(-0.1f,-0.1f,-0.1f), XMFLOAT2(0.26f,0.65f) },
		{ XMFLOAT3(0.1f,-0.1f,-0.1f), XMFLOAT2(0.0f,0.65f) },
		{ XMFLOAT3(0.1f,0.1f,-0.1f), XMFLOAT2(0.0f,0.34f) },
		{ XMFLOAT3(-0.1f,0.1f,-0.1f), XMFLOAT2(0.26f,0.34f) },
		//back
		{ XMFLOAT3(0.1f,-0.1f,0.1f),  XMFLOAT2(0.75f,0.65f) },
		{ XMFLOAT3(-0.1f,-0.1f,0.1f),XMFLOAT2(0.49f, 0.65f) },
		{ XMFLOAT3(-0.1f,0.1f,0.1f),XMFLOAT2(0.49f, 0.34f) },
		{ XMFLOAT3(0.1f,0.1f,0.1f), XMFLOAT2(0.75f, 0.34f) },
		//left
		{ XMFLOAT3(-0.1f,-0.1f,0.1f),  XMFLOAT2(0.49f,0.65f) },
		{ XMFLOAT3(-0.1f,-0.1f,-0.1f), XMFLOAT2(0.26f,0.65f) },
		{ XMFLOAT3(-0.1f,0.1f,-0.1f), XMFLOAT2(0.26f,0.34f) },
		{ XMFLOAT3(-0.1f,0.1f,0.1f),  XMFLOAT2(0.49f,0.34f) },
		//right
		{ XMFLOAT3(0.1f,-0.1f,-0.1f), XMFLOAT2(1.0f,0.65f) },
		{ XMFLOAT3(0.1f,-0.1f,0.1f), XMFLOAT2(0.75f,0.65f) },
		{ XMFLOAT3(0.1f,0.1f,0.1f), XMFLOAT2(0.75f,0.34f) },
		{ XMFLOAT3(0.1f,0.1f,-0.1f), XMFLOAT2(1.0f,0.34f) },
		//up
		{ XMFLOAT3(-0.1f,0.1f,-0.1f),  XMFLOAT2(0.49f,0.0f)},
		{ XMFLOAT3(0.1f,0.1f,-0.1f),  XMFLOAT2(0.49f,0.34f)},
		{ XMFLOAT3(0.1f,0.1f,0.1f),   XMFLOAT2(0.26f,0.34f)},
		{ XMFLOAT3(-0.1f,0.1f,0.1f), XMFLOAT2(0.26f,0.0f)},
		//down
		{ XMFLOAT3(-0.1f,-0.1f,0.1f), XMFLOAT2(0.26f,0.65f) },
		{ XMFLOAT3(0.1f,-0.1f,0.1f), XMFLOAT2(0.26f,1.0f) },
		{ XMFLOAT3(0.1f,-0.1f,-0.1f), XMFLOAT2(0.49f,1.0f) },
		{ XMFLOAT3(-0.1f,-0.1f,-0.1f), XMFLOAT2(0.49f,0.65f) },




		//floor begin with 86
		{ XMFLOAT3(-32.0f, 0.0f, -32.0f), XMFLOAT2(0.0f, 0.0f) ,XMFLOAT3(0.0f,1.0f,0.0f) },
		{ XMFLOAT3(32.0f, 0.0f, -32.0f), XMFLOAT2(64.0f, 0.0f) ,XMFLOAT3(0.0f,1.0f,0.0f) },
		{ XMFLOAT3(32.0f, 0.0f, 32.0f), XMFLOAT2(64.0f, 64.0f) ,XMFLOAT3(0.0f,1.0f,0.0f) },
		{ XMFLOAT3(-32.0f, 0.0f, 32.0f), XMFLOAT2(0.0f, 64.0f) ,XMFLOAT3(0.0f,1.0f,0.0f) },



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
	hr = m_pDevice->CreateBuffer(&bd, &InitData, &m_vertexBuffer);
	//if (FAILED(hr))
	//	return hr;

	// Set vertex buffer
	
	

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
	hr = m_pDevice->CreateBuffer(&bd, &InitData, &m_indexBuffer);
	//if (FAILED(hr))
	//	return hr;

	
}

void CarModel::PrepareRender()
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pDeviceContent->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set index buffer
	m_pDeviceContent->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	m_pDeviceContent->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CarModel::Release()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
	
}