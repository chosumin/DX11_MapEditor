#pragma once

namespace Model { class BinModel; }
namespace Model { class MeshPart; }
namespace Landscape
{
	class cBillboard
	{
	public:
		cBillboard(Model::BinModel* model);
		~cBillboard();

		void Update();
		void Render();
	public:
		Model::MeshPart* GetMeshPart(UINT slot);
		string GetPath();
		D3DXVECTOR3 GetPosition()
		{
			return m_vPosition;
		}

		D3DXVECTOR2 GetScaling()
		{
			return m_vScale;
		}

		void SetPosition(function<float(D3DXVECTOR3)> funcGetHeight);
		void SetPosition(float x, float y, float z)
		{
			m_vPosition = { x,y,z };
		}

		void SetScale(float x, float y)
		{
			m_vScale = { x,y };
		}
	private:
		void GetSRT(D3DXMATRIX* out, float radian, const D3DXMATRIX& t, const D3DXMATRIX& s) const;
	private:
		Model::BinModel*	o_pModel;
		WorldBuffer*		m_pWorld;

		D3DXVECTOR3			m_vPosition;
		D3DXVECTOR2			m_vScale;

		float				m_arrRotation[8] 
			= { 0, 45, 90, 135, 180, 225, 270, 315 };

		D3DXMATRIX			m_matTranslation;
		D3DXMATRIX			m_matScaling;
	};
}