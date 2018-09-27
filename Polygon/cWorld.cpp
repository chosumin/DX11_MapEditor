#include "stdafx.h"
#include "cWorld.h"

cWorld::cWorld()
	:m_vPosition(0, 0, 0)
	, m_vScaling(1, 1, 1)
	, m_vRotation(0, 0, 0)
{
	m_pWorldBuffer = new WorldBuffer();
}

cWorld::cWorld(const cWorld & world)
{
	m_pWorldBuffer = new WorldBuffer();
}

cWorld::~cWorld()
{
	SAFE_DELETE(m_pWorldBuffer);
}

void cWorld::Multiply(cWorld * pWorld)
{
	m_matWorld = pWorld->m_matWorld;
	m_pWorldBuffer->SetMatrix(m_matWorld);
}

void cWorld::Update()
{
	D3DXMATRIX s, r, t;
	D3DXMatrixScaling(&s, m_vScaling.x, m_vScaling.y, m_vScaling.z);

	D3DXVECTOR3 radian{ 0,0,0 };
	radian.x = m_vRotation.x * (float)D3DX_PI / 180.0f;
	radian.y = m_vRotation.y * (float)D3DX_PI / 180.0f;
	radian.z = m_vRotation.z * (float)D3DX_PI / 180.0f;

	D3DXMatrixRotationYawPitchRoll(&r, radian.y, radian.x, radian.z);
	D3DXMatrixTranslation(&t, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	m_matWorld = s * r * t;
	m_pWorldBuffer->SetMatrix(m_matWorld);
}

void cWorld::Render(UINT slot)
{
	m_pWorldBuffer->SetVSBuffer(slot);
}
