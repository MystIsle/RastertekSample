#include "Core/ApplicationClass.h"

#include "Check.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
}

ApplicationClass::ApplicationClass(const ApplicationClass&)
{

}

ApplicationClass::~ApplicationClass()
{

}

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
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

	char modelFilename[] = "assets/models/cube.txt";
	char textureFilename[] = "assets/textures/stone01.tga";

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_LightShader = new LightShaderClass();
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	m_Light = new LightClass();
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.f, 0.f, 1.f);
	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_Light)
	{
		delete m_Light;
		m_Light = nullptr;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = nullptr;
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
	static float rotation = 0.f;
	bool result;

	rotation -= 0.0174532925f * 0.25f;
	if (rotation < 0.f)
	{
		rotation += 360.f;
	}

	result = Render(rotation);

	if (result == false)
	{
		return false;
	}

	return true;
}

bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);
	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	
	worldMatrix = XMMatrixRotationY(rotation);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(),
	                               m_Model->GetIndexCount(),
	                               worldMatrix,
	                               viewMatrix,
	                               projectionMatrix,
	                               m_Model->GetTexture(),
	                               m_Light->GetDirection(),
	                               m_Light->GetDiffuseColor()
	                              );

	if (!result)
	{
		return false;
	}

	m_Direct3D->EndScene();

	return true;
}
