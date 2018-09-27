#pragma once

namespace Landscape
{
	class cHeightMap;
	class cTerrainTextures;
	class cFixedModels;
	class BrushBuffer;
	class Terrain
	{
	public:
		Terrain(ExecuteValues* values);
		~Terrain();

		void Update();
		void Render();
		void PostRender();
	public:
		void ReadJson(const Json::Value& root);
		void WriteOnJson(Json::Value* root);

		Landscape::cHeightMap* GetHeight();
	private:
		void MouseEvent();
	private:
		ExecuteValues*		values;
		BrushBuffer*		m_pBrushBuffer;
		cFixedModels*		m_pFixedModels;
		cHeightMap*			m_pHeightMap;
		cTerrainTextures*	m_pTextures;

		int					m_iMode;
	};
}