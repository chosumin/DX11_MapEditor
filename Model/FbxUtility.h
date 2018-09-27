#pragma once

namespace Fbx
{
	class FbxUtility
	{
	public:
		static D3DXVECTOR2 ToVector2(const FbxVector2& vec);
		static D3DXVECTOR3 ToVector3(const FbxVector4& vec);
		static D3DXCOLOR ToColor(const FbxVector4& vec);
		static D3DXVECTOR2 ToUv(const FbxVector2& vec);
		static D3DXMATRIX ToMatrix(const FbxMatrix& matrix);

		static D3DXVECTOR3 GetPosition(const class FbxVector4& position);
		static D3DXVECTOR3 GetNormal(class FbxMesh* mesh, int polygonIndex, int vertexIndex);
		static D3DXVECTOR2 GetUV(class FbxMesh* mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex);
		static D3DXMATRIX GetGeometricOffset(class FbxNode* node);

		static void GetMaterialProperty
		(
			D3DXCOLOR* outColor
			, wstring* outTextureFile
			, FbxSurfaceMaterial* fbxMaterial
			, string name
			, string factorName
		);
	};
}
