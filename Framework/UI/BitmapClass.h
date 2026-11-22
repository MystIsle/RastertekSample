#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#include "Graphics/TextureClass.h"

class BitmapClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
	
public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();
	
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*);
	
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	
	void SetRenderLocation(int, int);
	
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*);
	void RenderBuffers(ID3D11DeviceContext*);
	
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();
	
private:
	TextureClass* m_Texture = nullptr;
	
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	
	XMINT2 m_screenSize;
	XMINT2 m_bitmapSize;
	XMINT2 m_renderPos;
	XMINT2 m_prevPos;
};
