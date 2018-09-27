#pragma once

class cRectangle;
struct cWorld;
namespace Fbx { class FbxModel; }
namespace Model
{
	class MeshPart;
	class BoundingBox;
	class ColorBuffer;
	class BinModel
	{
	public:
		friend class Fbx::FbxModel;	
	public:
		BinModel(const BinModel& model);
		~BinModel();

		void Update();
		void Render();

		void SaveJson(Json::Value* root);
		void LoadJson(Json::Value* root);

		class MeshPart* GetIntersectedMesh(D3DXVECTOR3& vCamPos, D3DXVECTOR3& vCamDir);
	public:
		string& GetPath();
		D3DXMATRIX& GetWorld();
		D3DXVECTOR3& GetVec3(UINT type);
		D3DXVECTOR4& GetDiffuse();
		bool GetPickingState() const;
		void SetPickingState(bool bPicked);

		const string GetName() const
		{
			return name;
		}

		UINT GetMeshPartSize()
		{
			return meshParts.size();
		}

		class MeshPart* GetMeshPart(UINT index)
		{
			return meshParts[index];
		}

		void SetBoundingBox();
	private:
		BinModel();
	private:
		string name;
		string path;
		cWorld*				m_pWorld;
		BoundingBox*		m_pBounding;
		vector<MeshPart *>	meshParts;
		ColorBuffer*		m_pColorBuffer;
	};
}