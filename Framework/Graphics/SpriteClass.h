#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

class TextureClass;
using namespace DirectX;
using namespace std;

class SpriteClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
	
public:
	SpriteClass();
	SpriteClass(const SpriteClass&);
	~SpriteClass();
	
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, char*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*);
	
	void Update(float);
	
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	void SetRenderLocation(int, int);
	
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*);
	void RenderBuffers(ID3D11DeviceContext*);
	
	bool LoadTextures(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTextures();
	
private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
	int m_screenWidth;
	int m_screenHeight;
	int m_bitmapWidth;
	int m_bitmapHeight;
	int m_renderX;
	int m_renderY;
	int m_prevPosX;
	int m_prevPosY;
	TextureClass* m_Textures;
	float m_frameTime;
	float m_cycleTime;
	int m_currentTexture;
	int m_textureCount;
};
