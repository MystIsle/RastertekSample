#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = nullptr;
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

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEEN_NEAR);
	if (result == false)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

void ApplicationClass::Shutdown()
{
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
	// 회색으로 클리어만 한다 (튜토리얼 3).
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	m_Direct3D->EndScene();

	return true;
}
