#pragma once

class cRectangle;
namespace Model
{
	class BoundingBox
	{
	public:
		BoundingBox(D3DXVECTOR3 start, D3DXVECTOR3 end);
		~BoundingBox();
	public:
		void Render();
	private:
		void SetCube();
		void SetRasterizer();
	private:
		Shader*				m_pShader;
		vector<cRectangle*>	m_vecRect;

		//정점 정보
		D3DXVECTOR3			m_vStart;
		D3DXVECTOR3			m_vEnd;

		ID3D11RasterizerState* m_pRasterizer;
	};
}