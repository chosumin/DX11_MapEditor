#include "stdafx.h"
#include "BinModel.h"
#include "MeshPart.h"
#include "BoundingBox.h"
#include "ConstBuffer.h"
#include "./Polygon/cWorld.h"
#include "./Utilities/String.h"

Model::BinModel::BinModel(const BinModel & model)
{
	name = model.name;
	path = model.path;
	m_pWorld = new cWorld();
	for (auto part : model.meshParts)
	{
		auto tempPart = new MeshPart(*part);
		meshParts.push_back(tempPart);
	}
	m_pColorBuffer = new ColorBuffer();

	//바운딩 박스 설정
	SetBoundingBox();
}

Model::BinModel::BinModel()
{
	m_pWorld = new cWorld();
	m_pColorBuffer = new ColorBuffer();
}

Model::BinModel::~BinModel()
{
	for (MeshPart* part : meshParts)
		SAFE_DELETE(part);

	SAFE_DELETE(m_pBounding);
	SAFE_DELETE(m_pWorld);
	SAFE_DELETE(m_pColorBuffer);
}

void Model::BinModel::Update()
{
	m_pWorld->Update();
	for (MeshPart* part : meshParts)
		part->Update(m_pWorld);
}

void Model::BinModel::Render()
{
	m_pColorBuffer->SetVSBuffer(2);
	for (MeshPart* part : meshParts)
		part->Render();

	if(m_pColorBuffer->Data.Picked)
		m_pBounding->Render();
}

void Model::BinModel::SaveJson(Json::Value * root)
{
	Json::Value fbxInfo;
	{
		//경로
		fbxInfo["FilePath"] = name;

		Json::Value world;
		{
			world["PosX"] = m_pWorld->m_vPosition.x;
			world["PosY"] = m_pWorld->m_vPosition.y;
			world["PosZ"] = m_pWorld->m_vPosition.z;
			world["ScaleX"] = m_pWorld->m_vScaling.x;
			world["ScaleY"] = m_pWorld->m_vScaling.x;
			world["ScaleZ"] = m_pWorld->m_vScaling.x;
			world["RotX"] = m_pWorld->m_vRotation.x;
			world["RotY"] = m_pWorld->m_vRotation.y;
			world["RotZ"] = m_pWorld->m_vRotation.z;
		}
		fbxInfo["World"] = world;

		Json::Value meshes;
		for (auto mesh : meshParts)
		{
			Json::Value jMesh;
			
			jMesh["PosX"] = mesh->GetVec3(0).x;
			jMesh["PosY"] = mesh->GetVec3(0).y;
			jMesh["PosZ"] = mesh->GetVec3(0).z;

			jMesh["ScaleX"] = mesh->GetVec3(1).x;
			jMesh["ScaleY"] = mesh->GetVec3(1).y;
			jMesh["ScaleZ"] = mesh->GetVec3(1).z;

			jMesh["RotX"] = mesh->GetVec3(2).x;
			jMesh["RotY"] = mesh->GetVec3(2).y;
			jMesh["RotZ"] = mesh->GetVec3(2).z;

			meshes.append(jMesh);
		}//for(meshParts)
		fbxInfo["Meshes"] = meshes;
	}//Value(fbxInfo)
	(*root)["FBXinfo"] = fbxInfo;
}

void Model::BinModel::LoadJson(Json::Value * root)
{
	auto world = (*root)["World"];
	{
		m_pWorld->m_vPosition.x = world["PosX"].asFloat();
		m_pWorld->m_vPosition.y = world["PosY"].asFloat();
		m_pWorld->m_vPosition.z = world["PosZ"].asFloat();

		m_pWorld->m_vScaling.x = world["ScaleX"].asFloat();
		m_pWorld->m_vScaling.y = world["ScaleY"].asFloat();
		m_pWorld->m_vScaling.z = world["ScaleZ"].asFloat();

		m_pWorld->m_vRotation.x = world["RotX"].asFloat();
		m_pWorld->m_vRotation.y = world["RotY"].asFloat();
		m_pWorld->m_vRotation.z = world["RotZ"].asFloat();
	}

	auto meshes = (*root)["Meshes"];
	
	int count = 0;
	for (auto mesh : meshes)
	{
		auto m = meshParts[count++];
		
		m->GetVec3(0).x = mesh["PosX"].asFloat();
		m->GetVec3(0).y = mesh["PosY"].asFloat();
		m->GetVec3(0).z = mesh["PosZ"].asFloat();

		m->GetVec3(1).x = mesh["ScaleX"].asFloat();
		m->GetVec3(1).y = mesh["ScaleY"].asFloat();
		m->GetVec3(1).z = mesh["ScaleZ"].asFloat();

		m->GetVec3(2).x = mesh["RotX"].asFloat();
		m->GetVec3(2).y = mesh["RotY"].asFloat();
		m->GetVec3(2).z = mesh["RotZ"].asFloat();
	}
}

Model::MeshPart * Model::BinModel::GetIntersectedMesh(D3DXVECTOR3 & vCamPos, D3DXVECTOR3 & vCamDir)
{
	for (auto mesh : meshParts)
	{
		if (mesh->IsIntersectTri(vCamPos, vCamDir))
			return mesh;
	}

	return nullptr;
}

string & Model::BinModel::GetPath()
{
	return path;
}

D3DXMATRIX & Model::BinModel::GetWorld()
{
	return m_pWorld->m_matWorld;
}

D3DXVECTOR3 & Model::BinModel::GetVec3(UINT type)
{
	switch (type)
	{
		case 0:
			return m_pWorld->m_vPosition;
		case 1:
			return m_pWorld->m_vScaling;
		case 2:
			return m_pWorld->m_vRotation;
	}

	return D3DXVECTOR3();
}

D3DXVECTOR4 & Model::BinModel::GetDiffuse()
{
	return m_pColorBuffer->Data.Color;
}

bool Model::BinModel::GetPickingState() const
{
	return m_pColorBuffer->Data.Picked;
}

void Model::BinModel::SetPickingState(bool bPicked)
{
	m_pColorBuffer->Data.Picked = bPicked;
}

void Model::BinModel::SetBoundingBox()
{
	D3DXVECTOR3 min{ 500,500,500 }, max{ -500,-500,-500 };
	for (auto part : meshParts)
	{
		part->SetBoundingBox();
		part->GetMinMax(min, max);
	}

	m_pBounding = new BoundingBox(min, max);
}
