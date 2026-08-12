#pragma once

#include <Windows.h>
#include "D3DClass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.f;
const float SCREEEN_NEAR = 0.3f;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();
	
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	
private:
	bool Render();

private:
	D3DClass* m_Direct3D;
};
