#include "stdafx.h"
#include "cRectangle.h"

cRectangle::cRectangle(D3DXVECTOR3 start, D3DXVECTOR3 end)
{
	//정점 자료 입력
	m_arrVertexData = new Vertex[4];

	UpdateData(start, end);

	//색인 자료 입력
	m_arrIndexData = new UINT[6]{ 0,1,2,2,1,3 };

	CreateBuffer();
}


cRectangle::~cRectangle()
{
	SAFE_DELETE_ARRAY(m_arrVertexData);
	SAFE_DELETE_ARRAY(m_arrIndexData);

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}

void cRectangle::UpdateVertexBuffer()
{
	D3D::GetDC()->UpdateSubresource
	(
		m_pVB, 0, nullptr, m_arrVertexData, sizeof(Vertex) * 4, 0
	);
}

void cRectangle::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->DrawIndexed(6, 0, 0);
}

void cRectangle::UpdateData(D3DXVECTOR3 start, D3DXVECTOR3 end)
{
	auto gap = start - end;

	//좌, 우
	if (abs(gap.x) < D3DX_16F_EPSILON)
	{
		m_arrVertexData[0].position = { start.x, end.y, start.z };
		m_arrVertexData[1].position = { start.x, start.y, start.z };
		m_arrVertexData[2].position = { end.x, end.y, end.z };
		m_arrVertexData[3].position = { end.x, start.y, end.z };
	}
	//상, 하
	else if (abs(gap.y) < D3DX_16F_EPSILON)
	{
		m_arrVertexData[0].position = { start.x, end.y, end.z };
		m_arrVertexData[1].position = { start.x, start.y, start.z };
		m_arrVertexData[2].position = { end.x, end.y, end.z };
		m_arrVertexData[3].position = { end.x, start.y, start.z };
	}
	//전, 후
	else if (abs(gap.z) < D3DX_16F_EPSILON)
	{
		m_arrVertexData[0].position = { start.x, end.y, end.z };
		m_arrVertexData[1].position = { start.x, start.y, start.z };
		m_arrVertexData[2].position = { end.x, end.y, end.z };
		m_arrVertexData[3].position = { end.x, start.y, start.z };
	}
}

void cRectangle::CreateBuffer()
{
	//버텍스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT; //RAM과 VRAM접근 옵션
		desc.ByteWidth = sizeof(Vertex) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = m_arrVertexData;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &m_pVB);
		assert(SUCCEEDED(hr));
	}

	//인덱스 버퍼 생성
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = m_arrIndexData;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &m_pIB);
		assert(SUCCEEDED(hr));
	}
}