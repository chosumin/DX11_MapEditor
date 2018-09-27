#include "stdafx.h"
#include "DrawEnv.h"
#include "./Environment/Sky.h"

DrawEnv::DrawEnv(ExecuteValues* values)
	: Execute(values)
{
	sky = new Environment::Sky(values);
}

DrawEnv::~DrawEnv()
{
	SAFE_DELETE(sky);
}

void DrawEnv::Update()
{
	sky->Update();
}

void DrawEnv::Render()
{
	sky->Render();
}

void DrawEnv::PostRender()
{
	sky->PostRender();
}