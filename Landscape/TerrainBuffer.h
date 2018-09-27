#pragma once
#include "stdafx.h"

namespace Landscape 
{
	class GridBuffer : public ShaderBuffer
	{
	public:
		GridBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.GridColor = { 1,1,1,1 };
			Data.bGridView = 1;
			Data.GridSpacing = 3;
		}

		struct Struct
		{
			D3DXCOLOR GridColor;
			UINT bGridView;
			UINT GridSpacing;
			float Padding[2];
		}Data;
	};

	/**************************************************************/

	class BrushBuffer : public ShaderBuffer
	{
	public:
		BrushBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.BrushSize = 1;
			Data.BrushPos = { -1,-1 };
		}

		struct Struct
		{
			UINT BrushSize;
			D3DXVECTOR2 BrushPos;
			float Padding;
		}Data;

		D3DXVECTOR3 GetStart()
		{
			return{ Data.BrushPos.x - Data.BrushSize, 0, Data.BrushPos.y - Data.BrushSize };
		}

		D3DXVECTOR3 GetEnd()
		{
			return{ Data.BrushPos.x + Data.BrushSize, 0, Data.BrushPos.y + Data.BrushSize };
		}

		bool IsInArea(int x, int z)
		{
			D3DXVECTOR2 temp{ x - Data.BrushPos.x, z - Data.BrushPos.y };
			float length = D3DXVec2Length(&temp);

			return length <= Data.BrushSize;
		}
	};
}