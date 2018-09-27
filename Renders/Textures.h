#pragma once

class Texture
{
public:
	friend class Textures;

public:
	static void SetShaderResources(UINT slot, UINT count, Texture** textures);
	static void SetBlankShaderResource(UINT slot);
	static void SetBlankSamplerState(UINT slot);

public:
	Texture(wstring file, D3DX11_IMAGE_LOAD_INFO* loadInfo = NULL);
	~Texture();

	
	wstring GetFile() { return file; }

	UINT GetWidth() { return info.Width; }
	UINT GetHeight() { return info.Height; }

	D3D11_TEXTURE2D_DESC ReadPixels(DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels);

	void GetImageInfo(D3DX11_IMAGE_INFO* info)
	{
		*info = this->info;
	}

	void SetSamplerState(D3D11_SAMPLER_DESC desc);

	ID3D11ShaderResourceView* GetView() { return view; }

	void SetShaderResource(UINT slot);
	void SetShaderSampler(UINT slot);

private:
	wstring file;

	D3DX11_IMAGE_INFO info;
	ID3D11ShaderResourceView* view;
	ID3D11SamplerState* samplerState;
};

struct TextureDesc
{
	wstring file;
	UINT width, height;
	D3DX11_IMAGE_INFO info;
	ID3D11ShaderResourceView* view;

	bool operator==(const TextureDesc& desc)
	{
		bool b = true;
		b &= file == desc.file;
		b &= width == desc.width;
		b &= height == desc.height;

		return b;
	}
};

class Textures
{
public:
	friend class Texture;

public:
	static void Create();
	static void Delete();

private:
	static void Load(Texture* texture, D3DX11_IMAGE_LOAD_INFO* loadInfo = NULL);

private:
	static vector<TextureDesc> descs;
};