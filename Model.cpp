#include "Model.h"
#include <vector>
#include <sstream>
#include <iostream>
using namespace std;

Model::Model()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
}


Model::~Model()
{

}
bool Model::LoadModel(const char* filename)
{
	ifstream fin;
	char input;
	std::string line;

	fin.open(filename);
	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}
	while (!fin.eof())
	{
		std::getline(fin, line);


	}
	

	// Close the model file.
	fin.close();

	return true;
}

bool Model::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	HRESULT result;
	// Load the texture in.
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_Texture, NULL);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

void Model::ReleaseTexture()
{
	if (m_Texture)
		m_Texture->Release();
}

void Model::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		//indices[i] = i;
	}
	int index = 0;
	for (i = 0; i < m_vertexCount - 256; i++)
	{
		indices[index] = i; index++;
		indices[index] = i+256; index++;
		indices[index] = i+256+1; index++;
		indices[index] = i; index++;
		indices[index] = i+ 256+1; index++;
		indices[index] = i  + 1; index++;

	}
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void Model::ReleaseBuffers()
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

int Model::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return m_Texture;
}

void Model::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}
void Model::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
}

void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void Model::Release()
{
	ReleaseBuffers();
	ReleaseModel();
	ReleaseTexture();
}

bool Model::Initialize(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename)
{
	bool result;


	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Model:: Initialize(ID3D11Device* device, const WCHAR* textureFilename,int type)
{
	bool result;
	// Load in the model data,
	//result = LoadModel(modelFilename);
	result = GenModel(1);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

bool Model::GenModel(int type)
{
	if (type == 1)
	{
		int size = 256;
		//ReadRawFile("Res/aa.raw");//?
		m_vertexCount = size * size;
		m_indexCount = m_vertexCount * 6;
		m_model = new ModelType[m_vertexCount];
		for (UINT i = 0; i < size; ++i)
		{
		    //float tempZ = oZ - dz * i;
		    for (UINT j = 0; j < size; ++j)
		       {
		           UINT index = size * i + j;
		           m_model[index].x = i;
		           m_model[index].y = 0;
		           m_model[index].z = j;
				   m_model[index].tu = i;
				   m_model[index].tv = j;
				   m_model[index].nx = 0.0f;
				   m_model[index].ny = 1.0f;
				   m_model[index].nx = 0.0f;
		       }
		}	     
	}
	return true;
}

//读取高度图信息
bool Model::ReadRawFile(const char* filePath)
{
	std::ifstream inFile;
	//二进制方式打开文件
	inFile.open(filePath, std::ios::binary);
	//文件指针移动到末尾
	inFile.seekg(0, std::ios::end);
	//大小为当前缓冲区大小
	std::vector<BYTE> inData(inFile.tellg());
	//文件指针移动到开头
	inFile.seekg(std::ios::beg);
	//读取高度信息
	inFile.read((char*)&inData[0], inData.size());
	inFile.close();

	m_heightInfos.resize(inData.size());
	for (int i = 0; i < inData.size(); ++i)
	{
		m_heightInfos[i] = inData[i];
	}

	return true;
}