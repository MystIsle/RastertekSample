#include "BitmapClass.h"
#include "Core/Check.h"
#include "Utility/XMInt2Operators.h"

BitmapClass::BitmapClass()
{
}

BitmapClass::BitmapClass(const BitmapClass&)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, char* textureFilename, int renderX, int renderY)
{
	bool result;
	
	m_screenSize.x = screenWidth;
	m_screenSize.y = screenHeight;
	
	m_renderPos.x = renderX;
	m_renderPos.y = renderY;
	
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}
	
	return true;
}

void BitmapClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext)
{
	bool result = UpdateBuffers(deviceContext);
	if (!result)
	{
		return false;
	}
	
	RenderBuffers(deviceContext);
	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void BitmapClass::SetRenderLocation(int x, int y)
{
	m_renderPos.x = x;
	m_renderPos.y = y;	
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	
	m_prevPos.x = -1;
	m_prevPos.y = -1;
	
	m_vertexCount = 6;
	m_indexCount = m_vertexCount;
	
	VertexType* vertices = new VertexType[m_vertexCount];
	unsigned long* indices = new unsigned long[m_indexCount];
	memset(vertices, 0, sizeof(VertexType) * m_vertexCount);
	for (int i = 0 ; i < m_indexCount; i++)
	{
		indices[i] = i;
	}
	
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	
	delete [] vertices;
	vertices = nullptr;
	
	delete[] indices;
	indices = nullptr;
	
	return true;
}

void BitmapClass::ShutdownBuffers()
{
	SafeRelease(m_indexBuffer);
	SafeRelease(m_vertexBuffer);
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	VertexType* dataPtr;
	HRESULT result;
	
	if (m_prevPos == m_renderPos)
	{
		return true;
	}
	
	m_prevPos = m_renderPos;
	vertices = new VertexType[m_vertexCount];
	left = m_screenSize.x * -0.5f + m_renderPos.x;
	right = left + m_bitmapSize.x;
	top = m_screenSize.y *0.5f - m_renderPos.y;
	bottom = top - m_bitmapSize.y;
	
	// RECT 영역을 위한 정점 위치와 UV
	
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // 좌상단.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // 우하단.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // 좌하단.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두 번째 삼각형.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // 좌상단.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // 우상단.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // 우하단.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);
	
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if (FAILED(result))
	{
		return false;
	}
	
	dataPtr = (VertexType*)mappedResources.pData;
	memcpy(dataPtr,(void*)vertices, sizeof(VertexType) * m_vertexCount);
	deviceContext->Unmap(m_vertexBuffer, 0);
	
	dataPtr = nullptr;
	
	delete[] vertices;
	vertices = nullptr;
	
	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;
	
	stride = sizeof(VertexType);
	offset = 0;
	
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool BitmapClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	m_Texture = new TextureClass();
	bool result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}
	
	m_bitmapSize.x = m_Texture->GetWidth();
	m_bitmapSize.y = m_Texture->GetHeight();

	return true;
}

void BitmapClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}
}