#pragma once

namespace Model { class BinModel; }

namespace Fbx
{
	class FbxModel
	{
	public:
		friend class FbxModels;

		static const wstring DefaultShader;

	private:
		FbxModel();
		~FbxModel();

		void PushMaterialData(FbxSurfaceMaterial* material, int number);

		void Write(wstring file);
		static void Write(wstring file, Model::BinModel* model);
		static void Read(wstring file, Model::BinModel** model);

	private:
		string name;
		D3DXMATRIX world;

		vector<struct MaterialData *> materials;
		vector<struct MeshData *> meshes;
	};
}