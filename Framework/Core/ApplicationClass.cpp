#include "Core/ApplicationClass.h"

#include "Check.h"
#include "Graphics/SpriteClass.h"
#include "Graphics/TimerClass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_TextureShader = nullptr;
	m_Sprite = nullptr;
	m_Timer = nullptr;
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
	m_Camera->SetPosition(0.f, 0.f, -10.f);
	m_Camera->Render();
	
	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}
	
	char spriteFilename[] = "assets/textures/sprite_data_01.txt";
	m_Sprite = new SpriteClass();
	result = m_Sprite->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),
		screenWidth, screenHeight, spriteFilename, 50, 50);
	
	m_Timer = new TimerClass;
	result = m_Timer->Initialize();
	
	
	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = nullptr;
	}
	
	if (m_Sprite)
	{
		m_Sprite->Shutdown();
		delete m_Sprite;
		m_Sprite = nullptr;
	}
	
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
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
	m_Timer->Frame();
	float frameTime = m_Timer->GetTime();
	m_Sprite->Update(frameTime);
	
	bool result = Render();

	if (result == false)
	{
		return false;
	}

	return true;
}

bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	
	bool result;
	m_Direct3D->BeginScene(0.f, 0.f, 0.f, 1.f);
	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);
	
	m_Direct3D->TurnZBufferOff();
	
	result = m_Sprite->Render(m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}
	
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(),
		m_Sprite->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Sprite->GetTexture());

	if (!result)
	{
		return false;
	}
	
	m_Direct3D->TurnZBufferOn();
	m_Direct3D->EndScene();

	return true;
}
