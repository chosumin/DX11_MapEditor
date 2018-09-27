#include "stdafx.h"
#include "FbxData.h"
#include "MeshPart.h"
#include "FbxModel.h"

#include "../Utilities/String.h"
#include "../Utilities/BinaryFile.h"
#include "../Utilities/Path.h"

void Fbx::MaterialData::Write(BinaryWriter * w)
{
	w->UInt(Number);
	w->String(Name);

	w->Color4f(Ambient);
	w->Color4f(Diffuse);
	w->Color4f(Specular);
	w->Color4f(Emissive);

	w->Float(Shininess);
	w->Float(DetailFactor);

	w->String(ShaderFile);
	w->String(DiffuseFile);
	w->String(SpecularFile);
	w->String(EmissiveFile);
	w->String(NormalFile);
	w->String(DetailFile);
}

void Fbx::MaterialData::Write(BinaryWriter * w, Material * material)
{
	MaterialData data;

	data.Number = material->GetNumber();
	data.Name = String::WStringToString(material->GetName());

	data.Ambient = *(material->GetAmbient());
	data.Diffuse = *(material->GetDiffuse());
	data.Specular = *(material->GetSpecular());
	data.Emissive = *(material->GetEmissive());

	data.Shininess = *(material->GetShininess());
	data.DetailFactor = *(material->GetDetailFactor());


	wstring shader = material->GetShader()->GetFile();
	data.ShaderFile = String::WStringToString(shader);


	wstring texture = L"";

	texture = material->GetDiffuseMap()->GetFile();
	data.DiffuseFile = String::WStringToString(texture);

	texture = material->GetSpecularMap()->GetFile();
	data.SpecularFile = String::WStringToString(texture);

	texture = material->GetEmissiveMap()->GetFile();
	data.EmissiveFile = String::WStringToString(texture);

	texture = material->GetNormalMap()->GetFile();
	data.NormalFile = String::WStringToString(texture);

	texture = material->GetDetailMap()->GetFile();
	data.DetailFile = String::WStringToString(texture);

	data.Write(w);
}

void Fbx::MaterialData::Read(BinaryReader * r, Material ** material)
{
	MaterialData data;

	data.Number = r->UInt();
	data.Name = r->String();

	data.Ambient = r->Color4f();
	data.Diffuse = r->Color4f();
	data.Specular = r->Color4f();
	data.Emissive = r->Color4f();

	data.Shininess = r->Float();
	data.DetailFactor = r->Float();

	data.ShaderFile = r->String();
	data.DiffuseFile = r->String();
	data.SpecularFile = r->String();
	data.EmissiveFile = r->String();
	data.NormalFile = r->String();
	data.DetailFile = r->String();


	wstring file = L"";

	if (data.ShaderFile.length() < 1)
		file = FbxModel::DefaultShader;
	else
		file = String::StringToWString(data.ShaderFile);

	*material = new Material(file);

	(*material)->SetNumber(data.Number);
	(*material)->SetName(String::StringToWString(data.Name));

	(*material)->SetAmbient(data.Ambient);
	(*material)->SetDiffuse(data.Diffuse);
	(*material)->SetSpecular(data.Specular);
	(*material)->SetEmissive(data.Emissive);

	(*material)->SetShininess(data.Shininess);
	(*material)->SetDetailFactor(data.DetailFactor);

	
	if(Path::ExistFile(data.DiffuseFile) == true)
		(*material)->SetDiffuseMap(data.DiffuseFile);

	if (Path::ExistFile(data.ShaderFile) == true)
		(*material)->SetSpecularMap(data.SpecularFile);

	if (Path::ExistFile(data.EmissiveFile) == true)
		(*material)->SetEmissiveMap(data.EmissiveFile);

	if (Path::ExistFile(data.NormalFile) == true)
		(*material)->SetNormalMap(data.NormalFile);

	if (Path::ExistFile(data.DetailFile) == true)
		(*material)->SetDetailMap(data.DetailFile);
}

void Fbx::MeshData::Write(BinaryWriter * w)
{
	w->String(Name);
	w->Matrix(World);

	w->UInt(Vertices.size());
	w->Byte(&Vertices[0], sizeof(VertexTextureNormal) * Vertices.size());

	w->UInt(Indices.size());
	w->Byte(&Indices[0], sizeof(UINT) * Indices.size());
}

void Fbx::MeshData::Write(BinaryWriter * w, Model::MeshPart * meshPart)
{
	MeshData data;

	data.Name = meshPart->name;
	data.World = meshPart->GetWorld();

	data.Vertices.assign(meshPart->vertexData.begin(), meshPart->vertexData.end());
	data.Indices.assign(meshPart->indexData.begin(), meshPart->indexData.end());

	data.Write(w);
}

void Fbx::MeshData::Read(BinaryReader * r, Model::MeshPart ** meshPart)
{
	MeshData data;

	data.Name = r->String();
	data.World = r->Matrix();


	//VertexData 읽기
	{
		UINT count = r->UInt();
		data.Vertices.assign(count, VertexTextureNormal());

		void* ptr = (void *)&(data.Vertices[0]);
		r->Byte(&ptr, sizeof(VertexTextureNormal) * count);
	}

	//IndexData 읽기
	{
		UINT count = r->UInt();
		data.Indices.assign(count, UINT());

		void* ptr = (void *)&(data.Indices[0]);
		r->Byte(&ptr, sizeof(UINT) * count);
	}

	*meshPart = new Model::MeshPart(data);
}
