#pragma once
#include "cBasicMap.h"

namespace Landscape
{
	class GridBuffer;
	class BrushBuffer;
	class cHeightMap : public cBasicMap
	{
	public:
		cHeightMap(ExecuteValues* values);
		~cHeightMap();

		void Update();
		void Render(BrushBuffer* pBrush);
		void PostRender();
	public:
		void BrushOnMap(BrushBuffer* pBrush);

		D3DXVECTOR2 GetCellNumber(const D3DXVECTOR3& start, const D3DXVECTOR3& direction);
	private:
		void RaiseArea(BrushBuffer* pBrush);
		void Standardize(BrushBuffer* pBrush);
		void Flatten(BrushBuffer* pBrush);
		void BlendTextures(BrushBuffer* pBrush);

		void Loop(BrushBuffer* pBrush, const D3DXVECTOR3& start, const D3DXVECTOR3& end
				  , function<void(UINT, UINT, UINT, UINT)> func);
	private:
		ExecuteValues*	values;

		WorldBuffer*	m_pWorld;
		GridBuffer*		m_pGridBuffer;
		Material*		m_pMaterial;
		float			m_fFlattedValue;
		float			m_fRaisedValue;
		D3DXCOLOR		m_vAlphaSplatter;
		int				m_iMode;
	};
}