#include "SpriteClass.h"

#include "TextureClass.h"
#include "Core/Check.h"

SpriteClass::SpriteClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_Textures = nullptr;
}

SpriteClass::SpriteClass(const SpriteClass&)
{

}

SpriteClass::~SpriteClass()
{

}

bool SpriteClass::Initialize(ID3D11Device* device,
                             ID3D11DeviceContext* deviceContext,
                             int screenWidth,
                             int screenHeight,
                             char* spriteFilename,
                             int renderX,
                             int renderY)
{

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_renderX = renderX;
	m_renderY = renderY;

	m_frameTime = 0;

	bool result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	result = LoadTextures(device, deviceContext, spriteFilename);
	return result;
}

void SpriteClass::Shutdown()
{
	ReleaseTextures();
	ShutdownBuffers();
}

bool SpriteClass::Render(ID3D11DeviceContext* deviceContext)
{
	bool result = UpdateBuffers(deviceContext);
	if (!result)
	{
		return false;
	}

	RenderBuffers(deviceContext);
	return true;
}

void SpriteClass::Update(float frameTime)
{
	m_frameTime += frameTime;
	if (m_frameTime >= m_cycleTime)
	{
		m_frameTime -= m_cycleTime;
		m_currentTexture++;
		
		if (m_currentTexture == m_textureCount)
		{
			m_currentTexture = 0;
		}
	}
}

int SpriteClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* SpriteClass::GetTexture()
{
	return m_Textures[m_currentTexture].GetTexture();
}

bool SpriteClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_prevPosX = -1;
	m_prevPosY = -1;

	m_vertexCount = 6;

	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];
	indices = new unsigned long[m_indexCount];

	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));
	for (int i = 0; i < m_indexCount; i++)
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

void SpriteClass::ShutdownBuffers()
{
	SafeRelease(m_indexBuffer);
	SafeRelease(m_vertexBuffer);
}

bool SpriteClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	VertexType* dataPtr;
	HRESULT result;

	if ((m_prevPosX == m_renderX) && (m_prevPosY == m_renderY))
	{
		return true;
	}

	m_prevPosX = m_renderX;
	m_prevPosY = m_renderY;

	vertices = new VertexType[m_vertexCount];
	left = (float)m_screenWidth * -0.5f + (float)m_renderX;
	right = left + (float)m_bitmapWidth;
	top = (float)m_screenHeight * 0.5f - (float)m_renderY;
	bottom = top - (float)m_bitmapHeight;

	vertices[0].position = XMFLOAT3(left, top, 0.0f); // 좌상단.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f); // 우하단.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f); // 좌하단.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두 번째 삼각형.
	vertices[3].position = XMFLOAT3(left, top, 0.0f); // 좌상단.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f); // 우상단.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f); // 우하단.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (VertexType*)mappedResources.pData;
	memcpy(dataPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	deviceContext->Unmap(m_vertexBuffer, 0);
	dataPtr = nullptr;
	delete[] vertices;
	vertices = nullptr;
	return true;
}

void SpriteClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool SpriteClass::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	char textureFilename[128];
	ifstream fin;
	int i, j;
	char input;
	bool result;

	// 스프라이트 정보 데이터 파일을 연다.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// 텍스처 개수를 읽어 들인다.
	fin >> m_textureCount;

	// 파일에서 읽은 텍스처 개수로 텍스처 배열을 생성하고 초기화한다.
	m_Textures = new TextureClass[m_textureCount];

	// 다음 줄 시작까지 읽는다.
	fin.get(input);

	// 각 텍스처 파일 이름을 읽어 들인다.
	for (i = 0; i < m_textureCount; i++)
	{
		j = 0;
		fin.get(input);
		while (input != '\n')
		{
			textureFilename[j] = input;
			j++;
			fin.get(input);
		}
		textureFilename[j] = '\0';

		// 파일 이름을 얻었으면 그 텍스처를 텍스처 배열에 불러온다.
		result = m_Textures[i].Initialize(device, deviceContext, textureFilename);
		if (!result)
		{
			return false;
		}
	}

	// 사이클 시간을 읽어 들인다.
	fin >> m_cycleTime;

	// 정수 밀리초를 실수 표현으로 변환한다.
	m_cycleTime = m_cycleTime * 0.001f;

	// 파일을 닫는다.
	fin.close();

	// 첫 번째 텍스처의 크기를 얻어 2D 스프라이트 이미지의 크기로 사용한다.
	m_bitmapWidth = m_Textures[0].GetWidth();
	m_bitmapHeight = m_Textures[0].GetHeight();

	// 사이클의 시작 텍스처를 목록의 첫 번째로 설정한다.
	m_currentTexture = 0;

	return true;
}

void SpriteClass::ReleaseTextures()
{
	if (m_Textures)
	{
		for (int i = 0; i < m_textureCount; i++)
		{
			m_Textures[i].Shutdown();
		}

		delete[] m_Textures;
		m_Textures = nullptr;
	}
}

void SpriteClass::SetRenderLocation(int x, int y)
{
	m_renderX = x;
	m_renderY = y;
}
