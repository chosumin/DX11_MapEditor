#pragma once

namespace Model { class BinModel; }

namespace Fbx
{
	class FbxModels
	{
	public:
		static void Convert(wstring fbxFile, wstring saveFile);
		static void ReadBinary(wstring binFile, Model::BinModel** model);
		static void WriteBinary(wstring binFile, Model::BinModel* model);

	private:
		FbxModels();
		~FbxModels();

		void Open(wstring file, class FbxModel** model);
		void ProcessMaterial();
		void ProcessMesh(FbxNode* node);

	private:
		class FbxModel* model;

		FbxManager* manager;
		FbxImporter* importer;

		FbxScene* scene;
		FbxGeometryConverter* converter;

		vector<FbxSurfaceMaterial *> fbxMaterials;
	};
}