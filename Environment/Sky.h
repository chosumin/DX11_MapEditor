#pragma once

namespace Model { class BinModel; }
namespace Environment
{
	class Sky
	{
	public:
		class Buffer : public ShaderBuffer
		{
		public:
			Buffer() : ShaderBuffer(&Data, sizeof(Data))
			{
				Data.Apex = D3DXCOLOR(0xff9bcdff);
				Data.Center = D3DXCOLOR(0xff0080ff);
				Data.Height = 2.0f;
			}

			struct Struct
			{
				D3DXCOLOR Center;
				D3DXCOLOR Apex;
				float Height;
				float Padding[3];
			}Data;
		};

		Sky(ExecuteValues* values);
		~Sky();

		void Update();
		void Render();

		void PostRender();
	private:
		ExecuteValues* values;
		wstring shaderFile;
		wstring modelFile;

		Model::BinModel* model;
		Material* material;

		Buffer* buffer;

		ID3D11RasterizerState* rasterizer[2];
		ID3D11DepthStencilState* depthStencilState[2];
	};
}