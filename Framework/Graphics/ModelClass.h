#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "TextureClass.h"

using namespace DirectX;


class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};
	
	// 버텍스 하나에 해당하는 모델 데이터
	struct ModelType
	{
		float x,y,z; // 모델 스페이스 좌표
		float tu, tv; // 텍스쳐 UV
		float nx, ny, nz; // 버텍스 스페이스 ? - 노말 방향
	};
	
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();
	
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, const char*);
	void ReleaseTexture();
	
	bool LoadModel(char*);
	void ReleaseModel();
	
private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
	TextureClass* m_Texture;
	ModelType* m_model;
};
