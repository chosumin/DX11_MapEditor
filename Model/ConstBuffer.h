#pragma once
#include "./Renders/ShaderBuffer.h"

namespace Model
{
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = { 1,1,1,1 };
			Data.Picked = 0;
		}

		struct Struct
		{
			D3DXVECTOR4 Color;
			int Picked;
			float Padding[3];
		}Data;
	};
}