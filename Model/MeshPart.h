#pragma once

struct cWorld;
namespace Fbx 
{ 
	struct MeshData; 
	class FbxModel;
}

namespace Model
{
	class MeshPart
	{
	public:
		friend struct Fbx::MeshData;
		friend class Fbx::FbxModel;

	public:
		MeshPart(Fbx::MeshData& data);
		MeshPart(const MeshPart& refPart);
		~MeshPart();

		void Update(cWorld* pWorld = nullptr);
		void Render();

		bool IsIntersectTri(D3DXVECTOR3& vCamPos, D3DXVECTOR3& vCamDir);
	public:
		D3DXMATRIX& GetWorld();
		D3DXVECTOR3& GetVec3(UINT type);
		Material* GetMaterial();
		void GetMinMax(D3DXVECTOR3& vecMin, D3DXVECTOR3& vecMax) const;
		void SetWorld(D3DXMATRIX& world);
		void SetBoundingBox();
		void SetMatrix(D3DXMATRIX matrix);
	private:
		void Init(string sName
				  , const vector<VertexTextureNormal>& vecVertex
				  , const vector<UINT>& vecIndex, Material* mat = nullptr
				  , cWorld* world = nullptr);
		void CreateBuffer();
	private:
		string name;

		Material* material;

		cWorld*	world;

		vector<VertexTextureNormal> vertexData;
		vector<UINT>				indexData;

		ID3D11Buffer* vertexBuffer, *indexBuffer;
	};
}