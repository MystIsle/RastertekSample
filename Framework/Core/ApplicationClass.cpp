#include "Core/ApplicationClass.h"

#include "Check.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_TextureShader = nullptr;
}

ApplicationClass::ApplicationClass(const ApplicationClass&)
{

}

ApplicationClass::~ApplicationClass()
{

}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char textureFilename[128];
	bool result;

	m_Direct3D = new D3DClass();
	CHECK_RETURN(m_Direct3D, false);

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEEN_NEAR);
	if (result == false)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_Camera = new CameraClass();
	CHECK_RETURN(m_Camera, false);

	m_Camera->SetPosition(0.f, 0.f, -5.f);
	m_Model = new ModelClass();
	CHECK_RETURN(m_Model, false);

#ifdef _MSC_VER
	strcpy_s(textureFilename, "assets/textures/stone01.tga");
#else
	snprintf(textureFilename, sizeof(textureFilename), "assets/textures/stone01.tga");
#endif

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShaderClass();
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}
}

bool ApplicationClass::Frame()
{
	bool result;

	result = Render();
	if (result == false)
	{
		return false;
	}

	return true;
}

bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);
	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
	                                 m_Model->GetIndexCount(),
	                                 worldMatrix,
	                                 viewMatrix,
	                                 projectionMatrix,
	                                 m_Model->GetTexture());

	if (!result)
	{
		return false;
	}

	m_Direct3D->EndScene();

	return true;
}
