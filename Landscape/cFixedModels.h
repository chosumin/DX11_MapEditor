#pragma once

namespace Landscape
{
	class BrushBuffer;
	class cBillboard;
	class cFixedModels
	{
	public:
		cFixedModels(function<float(D3DXVECTOR3)> funcGetHeight);
		~cFixedModels();

		void Update();
		void Render();
		void PostRender();
	public:
		void InsertBillboardInArea(BrushBuffer* pBrush);
		void CreateBinModel(wstring filePath);

		void ReadJson(const Json::Value& root);
		void WriteOnJson(Json::Value* root);
	private:
		function<float(D3DXVECTOR3)> GetHeight;
		Model::BinModel* GetModel(wstring filePath);
	private:
		using Bmodel = Model::BinModel;
		vector<Bmodel*>		m_vecModel;
		Bmodel*				m_pCurModel;
		vector<cBillboard*>	m_vecBillboard;
		int					m_iCount;
	};
}