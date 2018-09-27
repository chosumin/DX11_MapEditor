#pragma once

class BinaryReader;
class BinaryWriter;

namespace Model { class MeshPart; }

namespace Fbx
{
	struct MaterialData
	{
		string Name = "";
		UINT Number = 0;

		D3DXCOLOR Ambient = D3DXCOLOR(0, 0, 0, 1);
		D3DXCOLOR Diffuse = D3DXCOLOR(0, 0, 0, 1);
		D3DXCOLOR Specular = D3DXCOLOR(0, 0, 0, 1);
		D3DXCOLOR Emissive = D3DXCOLOR(0, 0, 0, 1);

		float Shininess = 1.0f;
		float DetailFactor = 1.0f;

		string ShaderFile = "";
		string DiffuseFile = "";
		string SpecularFile = "";
		string EmissiveFile = "";
		string NormalFile = "";
		string DetailFile = "";

		void Write(BinaryWriter* w);
		static void Write(BinaryWriter* w, Material* material);

		static void Read(BinaryReader* r, Material** material);
	};

	struct MeshData
	{
		string Name;
		D3DXMATRIX World;

		vector<VertexTextureNormal> Vertices;
		vector<UINT> Indices;

		MeshData()
		{
			D3DXMatrixIdentity(&World);
		}

		void Write(BinaryWriter* w);
		static void Write(BinaryWriter* w, Model::MeshPart* meshPart);

		static void Read(BinaryReader* r, Model::MeshPart** meshPart);
	};
}