#include "stdafx.h"
#include "FbxModels.h"
#include "FbxModel.h"
#include "FbxUtility.h"
#include "FbxData.h"
#include "../Utilities/String.h"

void Fbx::FbxModels::Convert(wstring fbxFile, wstring saveFile)
{
	FbxModel* fbxModel = NULL;
	FbxModels* models = new FbxModels();

	models->Open(fbxFile, &fbxModel);
	SAFE_DELETE(models);

	fbxModel->Write(saveFile);
	SAFE_DELETE(fbxModel);
}

void Fbx::FbxModels::ReadBinary(wstring binFile, Model::BinModel ** model)
{
	FbxModel::Read(binFile, model);
}

void Fbx::FbxModels::WriteBinary(wstring binFile, Model::BinModel * model)
{
	FbxModel::Write(binFile, model);
}

Fbx::FbxModels::FbxModels()
{
	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, "");
}

Fbx::FbxModels::~FbxModels()
{
	scene->Destroy();
	manager->Destroy();
}

void Fbx::FbxModels::Open(wstring file, FbxModel ** model)
{
	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	importer = FbxImporter::Create(manager, "");

	string sFile = String::WStringToString(file);
	bool bCheck = importer->Initialize(sFile.c_str(), -1, ios);
	assert(bCheck == true);


	importer->Import(scene);
	converter = new FbxGeometryConverter(manager);
	{
		*model = new FbxModel();
		this->model = *model;

		ProcessMaterial();
		ProcessMesh(scene->GetRootNode());

		ios->Destroy();
	}
	SAFE_DELETE(converter);
	importer->Destroy();
}

void Fbx::FbxModels::ProcessMaterial()
{
	for (int i = 0; i < scene->GetMaterialCount(); i++)
	{
		FbxSurfaceMaterial* material = scene->GetMaterial(i);
		fbxMaterials.push_back(material);

		model->PushMaterialData(material, i);
	}
}

void Fbx::FbxModels::ProcessMesh(FbxNode * node)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		if (nodeType == FbxNodeAttribute::eMesh)
		{
			converter->Triangulate(attribute, true, true);

			FbxMesh* mesh = node->GetMesh();
			int controlPointCount = mesh->GetControlPointsCount();


			MeshData* meshData = new MeshData();
			meshData->Name = node->GetName();

			FbxVector4* controlPoints = mesh->GetControlPoints();
			for (int i = 0; i < mesh->GetPolygonCount(); i++)
			{
				for (int vi = 0; vi < 3; vi++)
				{
					int vertexIndex = mesh->GetPolygonVertex(i, vi);
					if (vertexIndex < 0 || vertexIndex >= controlPointCount)
						continue;


					VertexTextureNormal vertex;
					vertex.position = FbxUtility::GetPosition(controlPoints[vertexIndex]);
					vertex.uv = FbxUtility::GetUV(mesh, 0, i, vi, vertexIndex);
					vertex.normal = FbxUtility::GetNormal(mesh, i, vi);

					meshData->Vertices.push_back(vertex);
					meshData->Indices.push_back(meshData->Indices.size());
				}//for(vi)
			}//for(i)

			meshData->World = FbxUtility::GetGeometricOffset(node);
			model->meshes.push_back(meshData);
		}//if(nodeType)
	}//if(attribute)

	for (int i = 0; i < node->GetChildCount(); i++)
		ProcessMesh(node->GetChild(i));
}