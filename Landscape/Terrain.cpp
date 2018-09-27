#include "stdafx.h"
#include "Terrain.h"
#include "cHeightMap.h"
#include "cTerrainTextures.h"
#include "TerrainBuffer.h"
#include "cFixedModels.h"

Landscape::Terrain::Terrain(ExecuteValues *  values)
	:values(values)
	, m_iMode(0)
{
	m_pBrushBuffer = new BrushBuffer();
	m_pTextures = new cTerrainTextures();
	m_pHeightMap = new cHeightMap(values);
	m_pFixedModels = new cFixedModels([&](D3DXVECTOR3 pos) {
		return GetHeight()->GetHeight(pos); });
}

Landscape::Terrain::~Terrain()
{
	SAFE_DELETE(m_pBrushBuffer);
	SAFE_DELETE(m_pHeightMap);
	SAFE_DELETE(m_pTextures);
	SAFE_DELETE(m_pFixedModels);
}

void Landscape::Terrain::Update()
{
	D3DXVECTOR3 start;
	values->MainCamera->GetPosition(&start);

	D3DXVECTOR3 direction;
	direction =
		values->MainCamera->GetDirection(values->Viewport, values->Perspective);

	//매 프레임마다 피킹된 영역 검사
	m_pBrushBuffer->Data.BrushPos = m_pHeightMap->GetCellNumber(start, direction);

	m_pHeightMap->Update();
	m_pFixedModels->Update();

	if (Mouse::Get()->Press(0))
		MouseEvent();
}

void Landscape::Terrain::Render()
{
	m_pTextures->SetSamplers(0);
	m_pTextures->SetShaderResources(5);
	m_pHeightMap->Render(m_pBrushBuffer);
	m_pFixedModels->Render();
}

void Landscape::Terrain::PostRender()
{
	ImGui::Begin("Landscape");
	{
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "InsertionMode");
		ImGui::RadioButton("Terrain", &m_iMode, 0); 
		ImGui::SameLine(100);
		ImGui::RadioButton("Billboard", &m_iMode, 1);

		//////////////////////////////////////////
		ImGui::Separator();
		//////////////////////////////////////////

		ImGui::TextColored(ImVec4(1, 1, 0, 1), "BrushOption");
		ImGui::LabelText("Picked", "%.3f, %.3f"
						 , m_pBrushBuffer->Data.BrushPos.x
						 , m_pBrushBuffer->Data.BrushPos.y);
		ImGui::SliderInt("BrushSize", (int*)&m_pBrushBuffer->Data.BrushSize, 1, 40);

		//////////////////////////////////////////
		ImGui::Separator();
		//////////////////////////////////////////

		if (m_iMode == 0)
		{
			m_pHeightMap->PostRender();
			ImGui::Separator();
			m_pTextures->PostRender();
		}
		else
			m_pFixedModels->PostRender();
	}
	ImGui::End();
}

void Landscape::Terrain::ReadJson(const Json::Value& root)
{
	m_pHeightMap->ReadJson(root);
	m_pTextures->ReadJson(root);
	m_pFixedModels->ReadJson(root);
}

void Landscape::Terrain::WriteOnJson(Json::Value* root)
{
	m_pHeightMap->WriteOnJson(root);
	m_pTextures->WriteOnJson(root);
	m_pFixedModels->WriteOnJson(root);
}

Landscape::cHeightMap * Landscape::Terrain::GetHeight()
{
	return m_pHeightMap;
}

void Landscape::Terrain::MouseEvent()
{
	switch (m_iMode)
	{
		case 0:
			m_pHeightMap->BrushOnMap(m_pBrushBuffer);
			break;
		case 1:
			m_pFixedModels->InsertBillboardInArea(m_pBrushBuffer);
			break;
	}
}
