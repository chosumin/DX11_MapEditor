#include "stdafx.h"
#include "./Systems/Window.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 800;
	desc.Height = 600;
	D3D::SetDesc(desc);

	srand((unsigned int)time(nullptr));

	Window* window = new Window();
	WPARAM wParam = window->Run();
	SAFE_DELETE(window);

	return wParam;
}