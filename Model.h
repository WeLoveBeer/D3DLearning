#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <fstream>
#include <xnamath.h>
#include <vector>
using namespace std;
class Model
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
public:
	Model();
	~Model();
	bool Initialize(ID3D11Device*, const char*, const WCHAR*);
	bool Initialize(ID3D11Device*, const WCHAR* textureFilename,int type);
	void Release();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	void SetPosition(float, float, float);
	void GetPosition(float&, float&, float&);
private:
	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

	bool LoadModel(const char*);
	bool GenModel(int type);
	void ReleaseModel();

	bool ReadRawFile(const char* filePath);
private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ID3D11ShaderResourceView* m_Texture;
	ModelType* m_model;
	float m_positionX, m_positionY, m_positionZ;

	std::vector<float> m_heightInfos;
};

