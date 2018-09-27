#pragma once

namespace Landscape
{
	class cBasicMap
	{
	public:
		using VertexType = VertexColorTextureNormal;

	public:
		cBasicMap(wstring file = L"", float heightRatio = 10.0f);
		virtual ~cBasicMap();

		void Render();
	public:
		bool ReadSplattedMap(wstring filePath);
		bool ReadJson(IN const Json::Value& root);
		bool WriteOnJson(OUT Json::Value* root);

		float GetHeight(D3DXVECTOR3 position);
		void SetHeightMapFile(wstring file);
		void UpdateVertexData(D3D11_BOX* box = NULL);

		void SaveMap(int type, wstring fileName);
	private:
		void Clear();

		void FillvertexData(float heightRatio);
		void FillNormalcBasicMap();
		
		void CreateBuffer();
	protected:
		UINT			width;
		UINT			height;
		VertexType*		vertexData;
		UINT			vertexCount;
	private:
		Texture*		heightMap;
		UINT			indexCount;
		ID3D11Buffer*	vertexBuffer;
		ID3D11Buffer*	indexBuffer;
		UINT*			indexData;
		float			heightRatio;
	};
}