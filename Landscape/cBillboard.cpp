#include "stdafx.h"
#include "cBillboard.h"
#include "./Model/BinModel.h"
#include "./Model/MeshPart.h"

Landscape::cBillboard::cBillboard(Model::BinModel* model)
	:o_pModel(model)
{
	m_pWorld = new WorldBuffer();
}

Landscape::cBillboard::~cBillboard()
{
	SAFE_DELETE(m_pWorld);
}

void Landscape::cBillboard::Update()
{
	D3DXMatrixTranslation(&m_matTranslation, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	D3DXMatrixScaling(&m_matScaling, m_vScale.x, m_vScale.y, 1);
}

void Landscape::cBillboard::Render()
{
	for (auto radian : m_arrRotation)
	{
		D3DXMATRIX world;
		GetSRT(&world, radian, m_matTranslation, m_matScaling);
		o_pModel->GetMeshPart(0)->SetWorld(world);
		o_pModel->GetMeshPart(0)->Update();
		o_pModel->GetMeshPart(0)->SetMatrix(world);
		o_pModel->GetMeshPart(0)->Render();
	}
}

Model::MeshPart * Landscape::cBillboard::GetMeshPart(UINT slot)
{
	return o_pModel->GetMeshPart(slot);
}

string Landscape::cBillboard::GetPath()
{
	return o_pModel->GetPath();
}

void Landscape::cBillboard::SetPosition(function<float(D3DXVECTOR3)> funcGetHeight)
{
	m_vPosition.y = funcGetHeight(m_vPosition);
}

void Landscape::cBillboard::GetSRT(D3DXMATRIX * out, float radian, const D3DXMATRIX & t, const D3DXMATRIX & s) const
{
	D3DXMATRIX r;

	float rad = D3DX_PI / 180 * radian;
	D3DXMatrixRotationY(&r, rad);
	*out = s * r * t;
}
