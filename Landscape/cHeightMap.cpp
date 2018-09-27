#include "stdafx.h"
#include "cHeightMap.h"
#include "TerrainBuffer.h"

Landscape::cHeightMap::cHeightMap(ExecuteValues* values)
	: values(values)
	, m_fFlattedValue(0.5f)
	, m_fRaisedValue(0.1f)
	, m_vAlphaSplatter(0, 0, 0, 0)
{
	m_pMaterial = new Material(Shaders + L"Terrain.hlsl");

	m_pWorld = new WorldBuffer();
	m_pGridBuffer = new GridBuffer();
}

Landscape::cHeightMap::~cHeightMap()
{
	SAFE_DELETE(m_pMaterial);

	SAFE_DELETE(m_pWorld);
	SAFE_DELETE(m_pGridBuffer);
}

void Landscape::cHeightMap::Update()
{
}

void Landscape::cHeightMap::Render(BrushBuffer* pBrush)
{
	m_pMaterial->SetBuffer();
	m_pWorld->SetVSBuffer(1);
	pBrush->SetVSBuffer(2);
	m_pGridBuffer->SetPSBuffer(2);

	cBasicMap::Render();
}

void Landscape::cHeightMap::PostRender()
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "GridOption");
	bool isGrid = m_pGridBuffer->Data.bGridView > 0 ? true : false;
	ImGui::Checkbox("OnGridView", &isGrid);
	m_pGridBuffer->Data.bGridView = isGrid ? 1 : 0;
	ImGui::ColorEdit4("GridColor", m_pGridBuffer->Data.GridColor);
	ImGui::SliderInt("GridSpacing", (int*)&m_pGridBuffer->Data.GridSpacing, 1, 10);

	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "InsertMode");
	ImGui::RadioButton("Raise", &m_iMode, 0); ImGui::SameLine(100);
	ImGui::RadioButton("Standardize", &m_iMode, 1);
	ImGui::RadioButton("Flatten", &m_iMode, 2); ImGui::SameLine(100);
	ImGui::RadioButton("BlendTextures", &m_iMode, 3);

	ImGui::Separator();
	switch (m_iMode)
	{
		case 0:
			ImGui::SliderFloat("Raised Value", &m_fRaisedValue, -2.0f, 2.0f);
			break;
		case 2:
			ImGui::SliderFloat("Flatted Value", &m_fFlattedValue, -2.0f, 2.0f);
			break;
		case 3:
			ImGui::ColorEdit4("Splatter", m_vAlphaSplatter);
			break;
	}
}

void Landscape::cHeightMap::BrushOnMap(BrushBuffer* pBrush)
{
	switch (m_iMode)
	{
		case 0:
			RaiseArea(pBrush);
			break;
		case 1:
			Standardize(pBrush);
			break;
		case 2:
			Flatten(pBrush);
			break;
		default:
			BlendTextures(pBrush);
	}
}

void Landscape::cHeightMap::RaiseArea(BrushBuffer* pBrush)
{
	//cellx, celly 기준 start, end 잡기
	auto start = pBrush->GetStart();
	auto end = pBrush->GetEnd();

	//범위 안의 높이 올리기
	Loop(pBrush, start, end, [this](UINT a, UINT b, UINT c, UINT d)
	{
		if (a <= vertexCount)
			vertexData[a].position.y += m_fRaisedValue;
		if (b <= vertexCount)
			vertexData[b].position.y += m_fRaisedValue;
		if (c <= vertexCount)
			vertexData[c].position.y += m_fRaisedValue;
		if (d <= vertexCount)
			vertexData[d].position.y += m_fRaisedValue;
	});
}

void Landscape::cHeightMap::Standardize(BrushBuffer* pBrush)
{
	//cellx, celly 기준 start, end 잡기
	auto start = pBrush->GetStart();
	auto end = pBrush->GetEnd();

	//범위 안의 높이 평균값 찾기
	Loop(pBrush, start, end, [this, start, end](UINT a, UINT b, UINT c, UINT d)
	{
		float avgHeight = 0.0f;
		if (a <= vertexCount)
			avgHeight += vertexData[a].position.y;
		if (b <= vertexCount)
			avgHeight += vertexData[b].position.y;
		if (c <= vertexCount)
			avgHeight += vertexData[c].position.y;
		if (d <= vertexCount)
			avgHeight += vertexData[d].position.y;
		avgHeight /= 4;

		vertexData[a].position.y = avgHeight;
		vertexData[b].position.y = avgHeight;
		vertexData[c].position.y = avgHeight;
		vertexData[d].position.y = avgHeight;
	});
}

void Landscape::cHeightMap::Flatten(BrushBuffer* pBrush)
{
	//cellx, celly 기준 start, end 잡기
	auto start = pBrush->GetStart();
	auto end = pBrush->GetEnd();

	//범위 안의 높이 같게 맞추기
	Loop(pBrush, start, end, [this](UINT a, UINT b, UINT c, UINT d)
	{
		if (a <= vertexCount)
			vertexData[a].position.y = m_fFlattedValue;
		if (b <= vertexCount)
			vertexData[b].position.y = m_fFlattedValue;
		if (c <= vertexCount)
			vertexData[c].position.y = m_fFlattedValue;
		if (d <= vertexCount)
			vertexData[d].position.y = m_fFlattedValue;
	});
}

void Landscape::cHeightMap::BlendTextures(BrushBuffer* pBrush)
{
	//cellx, celly 기준 start, end 잡기
	auto start = pBrush->GetStart();
	auto end = pBrush->GetEnd();

	Loop(pBrush, start, end, [this](UINT a, UINT b, UINT c, UINT d)
	{
		if (a <= vertexCount)
			vertexData[a].color = m_vAlphaSplatter;
		if (b <= vertexCount)
			vertexData[b].color = m_vAlphaSplatter;
		if (c <= vertexCount)
			vertexData[c].color = m_vAlphaSplatter;
		if (d <= vertexCount)
			vertexData[d].color = m_vAlphaSplatter;
	});
}

D3DXVECTOR2 Landscape::cHeightMap::GetCellNumber(const D3DXVECTOR3& start, const D3DXVECTOR3& direction)
{
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index0 = (width + 1) * z + x;
			UINT index1 = (width + 1) * (z + 1) + x;
			UINT index2 = (width + 1) * z + x + 1;
			UINT index3 = (width + 1) * (z + 1) + x + 1;

			D3DXVECTOR3 v0 = vertexData[index0].position;
			D3DXVECTOR3 v1 = vertexData[index1].position;
			D3DXVECTOR3 v2 = vertexData[index2].position;
			D3DXVECTOR3 v3 = vertexData[index3].position;

			float u, v, distance;
			if (D3DXIntersectTri(&v0, &v1, &v2, &start, &direction, &u, &v, &distance) == TRUE)
				return{ (float)x, (float)z };

			if (D3DXIntersectTri(&v3, &v1, &v2, &start, &direction, &u, &v, &distance) == TRUE)
				return{ (float)x, (float)z };
		}//for(x)
	}//for(z)

	return{ -1, -1 };
}

void Landscape::cHeightMap::Loop(BrushBuffer* pBrush, const D3DXVECTOR3 & start, const D3DXVECTOR3 & end, function<void(UINT, UINT, UINT, UINT)> func)
{
	for (size_t z = (UINT)start.z; z < (UINT)end.z; z++)
	{
		for (size_t x = (UINT)start.x; x < (UINT)end.x; x++)
		{
			if (pBrush->IsInArea(x, z))
			{
				UINT index0 = (width + 1) * z + x;
				UINT index1 = (width + 1) * (z + 1) + x;
				UINT index2 = (width + 1) * z + x + 1;
				UINT index3 = (width + 1) * (z + 1) + x + 1;

				func(index0, index1, index2, index3);
			}
		}
	}

	//정점 업데이트
	UpdateVertexData();
}
