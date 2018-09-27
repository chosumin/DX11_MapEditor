#include "stdafx.h"
#include "BoundingBox.h"
#include "./Polygon/cRectangle.h"
using namespace Model;

BoundingBox::BoundingBox(D3DXVECTOR3 start, D3DXVECTOR3 end)
	: m_vStart(start)
	, m_vEnd(end)
{
	m_pShader = new Shader(Shaders + L"001_common.hlsl");

	SetCube();
	SetRasterizer();
}

BoundingBox::~BoundingBox()
{
	for (auto& rect : m_vecRect)
		SAFE_DELETE(rect);

	SAFE_DELETE(m_pShader);
	SAFE_RELEASE(m_pRasterizer);
}

void BoundingBox::Render()
{
	D3D::GetDC()->RSSetState(m_pRasterizer);
	m_pShader->Render();

	for (auto rect : m_vecRect)
		rect->Render();

	D3D::GetDC()->RSSetState(nullptr);
}

void Model::BoundingBox::SetCube()
{
	m_vecRect.push_back(new cRectangle{ { m_vStart.x,m_vEnd.y,m_vStart.z },{ m_vEnd.x,m_vStart.y,m_vStart.z } });
	m_vecRect.push_back(new cRectangle{ {m_vEnd.x,m_vEnd.y,m_vEnd.z }, { m_vStart.x,m_vStart.y, m_vEnd.z } });
	m_vecRect.push_back(new cRectangle{ { m_vStart.x,m_vEnd.y,m_vEnd.z },{ m_vEnd.x, m_vEnd.y,m_vStart.z } });
	m_vecRect.push_back(new cRectangle{ {m_vStart.x,m_vStart.y,m_vStart.z }, { m_vEnd.x,m_vStart.y, m_vEnd.z } });
	m_vecRect.push_back(new cRectangle{ { m_vEnd.x,m_vEnd.y,m_vStart.z },{ m_vEnd.x,m_vStart.y, m_vEnd.z } });
	m_vecRect.push_back(new cRectangle{ {m_vStart.x,m_vEnd.y,m_vEnd.z }, { m_vStart.x,m_vStart.y,m_vStart.z } });
}

void Model::BoundingBox::SetRasterizer()
{
	D3D11_RASTERIZER_DESC desc;
	States::GetRasterizerDesc(&desc);
	desc.FillMode = D3D11_FILL_WIREFRAME;
	States::CreateRasterizer(&desc, &m_pRasterizer);
}
