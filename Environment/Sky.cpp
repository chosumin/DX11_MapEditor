#include "stdafx.h"
#include "Sky.h"
#include "./Model/FbxModels.h"
#include "./Model/MeshPart.h"
#include "./Model/BinModel.h"

Environment::Sky::Sky(ExecuteValues* values)
	: values(values)
	, modelFile(BinModels + L"Sphere.model")
	, shaderFile(Shaders + L"024_Sky.hlsl")
{
	Fbx::FbxModels::ReadBinary(modelFile, &model);
	auto material = model->GetMeshPart(0)->GetMaterial();
	material->SetShader(shaderFile);

	buffer = new Buffer();

	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		States::CreateRasterizer(&desc, &rasterizer[0]);

		desc.FrontCounterClockwise = true;
		States::CreateRasterizer(&desc, &rasterizer[1]);
	}

	{
		D3D11_DEPTH_STENCIL_DESC desc;
		States::GetDepthStencilDesc(&desc);
		States::CreateDepthStencil(&desc, &depthStencilState[0]);

		desc.DepthEnable = false;
		States::CreateDepthStencil(&desc, &depthStencilState[1]);
	}
}

Environment::Sky::~Sky()
{
	SAFE_DELETE(buffer);
	SAFE_DELETE(model);

	SAFE_RELEASE(rasterizer[0]);
	SAFE_RELEASE(rasterizer[1]);

	SAFE_RELEASE(depthStencilState[0]);
	SAFE_RELEASE(depthStencilState[1]);
}

void Environment::Sky::Update()
{
	D3DXVECTOR3 position;
	values->MainCamera->GetPosition(&position);

	model->GetMeshPart(0)->GetVec3(0) = position;
	model->GetMeshPart(0)->Update();
}

void Environment::Sky::Render()
{
	D3D::GetDC()->OMSetDepthStencilState(depthStencilState[1], 1);
	D3D::GetDC()->RSSetState(rasterizer[1]);

	buffer->SetPSBuffer(2);
	model->GetMeshPart(0)->Render();

	D3D::GetDC()->RSSetState(rasterizer[0]);
	D3D::GetDC()->OMSetDepthStencilState(depthStencilState[0], 1);
}

void Environment::Sky::PostRender()
{
	ImGui::Begin("Environment");
	ImGui::ColorEdit4("Center", buffer->Data.Center);
	ImGui::ColorEdit4("Apex", buffer->Data.Apex);
	ImGui::SliderFloat("Height", &buffer->Data.Height, 0, 5);
	ImGui::End();
}
