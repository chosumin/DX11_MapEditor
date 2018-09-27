#include "stdafx.h"
#include "FbxUtility.h"
#include "../Utilities/Path.h"
#include "../Utilities/String.h"

D3DXVECTOR2 Fbx::FbxUtility::ToVector2(const FbxVector2 & vec)
{
	return D3DXVECTOR2((float)vec.mData[0], (float)vec.mData[1]);
}

D3DXVECTOR3 Fbx::FbxUtility::ToVector3(const FbxVector4 & vec)
{
	return D3DXVECTOR3((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2]);
}

D3DXCOLOR Fbx::FbxUtility::ToColor(const FbxVector4 & vec)
{
	return D3DXCOLOR((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2], 1);
}

D3DXVECTOR2 Fbx::FbxUtility::ToUv(const FbxVector2 & vec)
{
	D3DXVECTOR2 temp = ToVector2(vec);

	return D3DXVECTOR2(temp.x, 1.0f - temp.y);
}

D3DXMATRIX Fbx::FbxUtility::ToMatrix(const FbxMatrix & matrix)
{
	FbxVector4 r1 = matrix.GetRow(0);
	FbxVector4 r2 = matrix.GetRow(1);
	FbxVector4 r3 = matrix.GetRow(2);
	FbxVector4 r4 = matrix.GetRow(3);

	return D3DXMATRIX
	(
		(float)r1.mData[0], (float)r1.mData[1], (float)r1.mData[2], (float)r1.mData[3],
		(float)r2.mData[0], (float)r2.mData[1], (float)r2.mData[2], (float)r2.mData[3],
		(float)r3.mData[0], (float)r3.mData[1], (float)r3.mData[2], (float)r3.mData[3],
		(float)r4.mData[0], (float)r4.mData[1], (float)r4.mData[2], (float)r4.mData[3]
	);
}

D3DXVECTOR3 Fbx::FbxUtility::GetPosition(const FbxVector4 & position)
{
	return ToVector3(position);
}

D3DXVECTOR3 Fbx::FbxUtility::GetNormal(FbxMesh * mesh, int polygonIndex, int vertexIndex)
{
	FbxVector4 fbxNormal(0, 0, 0, 0);
	mesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, fbxNormal);
	fbxNormal.Normalize();

	return ToVector3(fbxNormal);
}

D3DXVECTOR2 Fbx::FbxUtility::GetUV(FbxMesh * mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	int layerCount = mesh->GetLayerCount();
	if (layerIndex >= layerCount) return D3DXVECTOR2();

	FbxLayer* layer = mesh->GetLayer(layerIndex);
	if (layer == NULL) return D3DXVECTOR2();

	FbxLayerElementUV* uv = layer->GetUVs(FbxLayerElement::eTextureDiffuse);
	if (uv == NULL) return D3DXVECTOR2();


	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	const FbxLayerElementArrayTemplate<FbxVector2>& uvArray = uv->GetDirectArray();
	const FbxLayerElementArrayTemplate<int>& uvIndexArray = uv->GetIndexArray();

	int mappingIndex = 0;
	if (mappingMode == FbxLayerElement::eByControlPoint)
	{
		mappingIndex = vertexIndex;

		if (refMode == FbxLayerElement::eDirect)
		{
			if (mappingIndex < uvArray.GetCount())
				return ToUv(uvArray.GetAt(mappingIndex));
		}
		else if (refMode == FbxLayerElement::eIndexToDirect)
		{
			if (mappingIndex < uvArray.GetCount())
			{
				int tempIndex = uvIndexArray.GetAt(mappingIndex);

				if (tempIndex < uvArray.GetCount())
					return ToUv(uvArray.GetAt(tempIndex));
			}
		}//if(refMode)
	}
	else if (mappingMode == FbxLayerElement::eByPolygonVertex)
	{
		mappingIndex = mesh->GetTextureUVIndex
		(
			polygonIndex
			, polygonVertexIndex
			, FbxLayerElement::eTextureDiffuse
		);

		switch (refMode)
		{
			case FbxLayerElement::eDirect:
			case FbxLayerElement::eIndexToDirect:
				if (mappingIndex < uvArray.GetCount())
					return ToUv(uvArray.GetAt(mappingIndex));
		}//switch(refMode)
	}//if(mappingMode)

	return D3DXVECTOR2();
}

D3DXMATRIX Fbx::FbxUtility::GetGeometricOffset(FbxNode * node)
{
	if (node == NULL)
	{
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);

		return matrix;
	}

	FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);

	FbxMatrix offset;
	offset.SetIdentity();
	offset.SetTRS(T, R, S);

	return ToMatrix(offset);
}

void Fbx::FbxUtility::GetMaterialProperty(D3DXCOLOR * outColor, wstring * outTextureFile, FbxSurfaceMaterial * fbxMaterial, string name, string factorName)
{
	if (outColor != NULL)
		*outColor = D3DXCOLOR(0, 0, 0, 1);

	*outTextureFile = L"";


	FbxProperty prop = fbxMaterial->FindProperty(name.c_str());
	FbxProperty factorProp = fbxMaterial->FindProperty(factorName.c_str());
	if (prop.IsValid() == true && factorProp.IsValid() == true)
	{
		FbxDouble3 color = prop.Get<FbxDouble3>();

		if (factorName.length() > 0)
		{
			double factor = factorProp.Get<FbxDouble>();

			color[0] *= factor;
			color[1] *= factor;
			color[2] *= factor;
		}

		if (outColor != NULL)
		{
			(*outColor).r = (float)color[0];
			(*outColor).g = (float)color[1];
			(*outColor).b = (float)color[2];
		}
	}

	if (prop.IsValid() == true)
	{
		UINT count = prop.GetSrcObjectCount<FbxFileTexture>();

		if (count > 0)
		{
			FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();

			if (texture != NULL)
			{
				const char* cFile = texture->GetFileName();
				string tempFile = Path::GetFileName(cFile);

				*outTextureFile = String::StringToWString(tempFile);
			}
		}//if(count)
	}//if(prop)
}