#pragma once

namespace Landscape
{
	class cTerrainTextures
	{
	public:
		cTerrainTextures();
		~cTerrainTextures();

		void SetShaderResources(UINT slot);
		void SetSamplers(UINT slot);

		void PostRender();
		void SetTexture(wstring fileName, UINT slot);

		void ReadJson(const Json::Value& root);
		void WriteOnJson(Json::Value* root);
	private:
		vector<ID3D11ShaderResourceView*> m_vecView;
		vector<string>			m_vecPath;
		ID3D11SamplerState*		m_pSamplerState;
		const UINT	m_uMaxSize;
	};
}

