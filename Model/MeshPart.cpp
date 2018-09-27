#include "stdafx.h"
#include "MeshPart.h"
#include "FbxData.h"
#include "BoundingBox.h"
#include "ConstBuffer.h"
#include "./Polygon/cWorld.h"

Model::MeshPart::MeshPart(Fbx::MeshData & data)
{
	Init(data.Name, data.Vertices, data.Indices);
}

Model::MeshPart::MeshPart(const MeshPart & refPart)
{
	Init(refPart.name, refPart.vertexData, refPart.indexData
		 , refPart.material, refPart.world);
}

Model::MeshPart::~MeshPart()
{
	SAFE_DELETE(material);
	SAFE_DELETE(world);
	
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Model::MeshPart::Update(cWorld* pWorld)
{
	world->Update();
	
	if (pWorld != nullptr)
		world->Multiply(pWorld);
}

void Model::MeshPart::Render()
{
	UINT stride = sizeof(VertexTextureNormal);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	world->Render(1);
	material->SetBuffer();

	D3D::GetDC()->DrawIndexed(indexData.size(), 0, 0);
}

bool Model::MeshPart::IsIntersectTri(D3DXVECTOR3 & vCamPos, D3DXVECTOR3 & vCamDir)
{
	D3DXVECTOR3 transPos;
	D3DXVECTOR3 transDir;

	D3DXMATRIX invWorld;
	D3DXMatrixInverse(&invWorld, nullptr, &world->m_matWorld);
	D3DXVec3TransformCoord(&transPos, &vCamPos, &invWorld);
	D3DXVec3TransformNormal(&transDir, &vCamDir, &invWorld);
	
	UINT size = vertexData.size();
	for (UINT i = 0; i < size; i += 3)
	{
		D3DXVECTOR3 v0 = vertexData[i + 0].position;
		D3DXVECTOR3 v1 = vertexData[i + 1].position;
		D3DXVECTOR3 v2 = vertexData[i + 2].position;

		if (D3DXIntersectTri(&v0, &v1, &v2, &transPos, &transDir, nullptr, nullptr, nullptr) == TRUE)
			return true;
	}//for(i)

	return false;
}

D3DXMATRIX & Model::MeshPart::GetWorld()
{
	return world->m_matWorld;
}

D3DXVECTOR3 & Model::MeshPart::GetVec3(UINT type)
{
	switch (type)
	{
		case 0:
			return world->m_vPosition;
		case 1:
			return world->m_vScaling;
		case 2:
			return world->m_vRotation;
	}

	return D3DXVECTOR3();
}

Material * Model::MeshPart::GetMaterial()
{
	return material;
}

void Model::MeshPart::GetMinMax(D3DXVECTOR3 & vecMin, D3DXVECTOR3 & vecMax) const
{
	for (auto vertex : vertexData)
	{
		vecMax.x = max(vecMax.x, vertex.position.x);
		vecMax.y = max(vecMax.y, vertex.position.y);
		vecMax.z = max(vecMax.z, vertex.position.z);

		vecMin.x = min(vecMin.x, vertex.position.x);
		vecMin.y = min(vecMin.y, vertex.position.y);
		vecMin.z = min(vecMin.z, vertex.position.z);
	}
}

void Model::MeshPart::SetWorld(D3DXMATRIX & world)
{
	this->world->m_matWorld = world;
}

void Model::MeshPart::SetBoundingBox()
{
	D3DXVECTOR3 min{ 500,500,500 }, max{ -500,-500,-500 };
	GetMinMax(min, max);
	//boundingBox = new BoundingBox(min, max);
}

void Model::MeshPart::SetMatrix(D3DXMATRIX matrix)
{
	world->m_pWorldBuffer->SetMatrix(matrix);
}

void Model::MeshPart::Init(string sName
						   , const vector<VertexTextureNormal>& vecVertex
						   , const vector<UINT>& vecIndex
						   , Material* mat
						   , cWorld* world)
{
	name = sName;

	if(mat != nullptr)
		this->material = new Material(mat->GetShader()->GetFile());

	this->world = new cWorld();

	vertexData.assign(vecVertex.begin(), vecVertex.end());
	indexData.assign(vecIndex.begin(), vecIndex.end());

	CreateBuffer();
}

void Model::MeshPart::CreateBuffer()
{
	UINT vertexCount = vertexData.size();
	UINT indexCount = indexData.size();


	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormal) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertexData[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//2. Index Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indexData[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}