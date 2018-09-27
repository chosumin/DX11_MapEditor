#pragma once

class cRectangle
{
public:
	cRectangle(D3DXVECTOR3 start, D3DXVECTOR3 end);
	~cRectangle();
public:
	//바운딩 박스 정점 업데이트
	void UpdateData(D3DXVECTOR3 start, D3DXVECTOR3 end);
	void UpdateVertexBuffer();
	void Render();
private:
	void CreateBuffer();
private:
	ID3D11Buffer*	m_pVB;
	ID3D11Buffer*	m_pIB;
	Vertex*			m_arrVertexData;
	UINT*			m_arrIndexData;
};

