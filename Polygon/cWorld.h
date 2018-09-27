#pragma once

struct cWorld
{
public:
	cWorld();
	cWorld(const cWorld& world);
	~cWorld();

	void Multiply(cWorld* pWorld);
	void Update();
	void Render(UINT slot);
public:
	D3DXVECTOR3		m_vPosition;
	D3DXVECTOR3		m_vScaling;
	D3DXVECTOR3		m_vRotation;
	D3DXMATRIX		m_matWorld;
	WorldBuffer*	m_pWorldBuffer;
};

