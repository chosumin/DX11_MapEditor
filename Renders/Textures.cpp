#include "stdafx.h"
#include "Textures.h"

vector<TextureDesc> Textures::descs;

Texture::Texture(wstring file, D3DX11_IMAGE_LOAD_INFO* loadInfo)
	: file(file)
{
	Textures::Load(this, loadInfo);

	D3D11_SAMPLER_DESC desc;
	States::GetSamplerDesc(&desc);
	States::CreateSampler(&desc, &samplerState);
}

Texture::~Texture()
{
	SAFE_RELEASE(samplerState);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixels(DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels)
{
	HRESULT hr;

	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = srcDesc.Width;
	desc.Height = srcDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = readFormat;
	desc.SampleDesc = srcDesc.SampleDesc;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), srcTexture, NULL, texture);
	assert(SUCCEEDED(hr));


	D3D11_MAPPED_SUBRESOURCE map;
	UINT* colors = new UINT[desc.Width * desc.Height];
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, NULL, &map);
	{
		memcpy(colors, map.pData, sizeof(UINT) * desc.Width * desc.Height);
	}
	D3D::GetDC()->Unmap(texture, 0);


	pixels->reserve(desc.Width * desc.Height);
	for (UINT y = 0; y < desc.Height; y++)
	{
		for (UINT x = 0; x < desc.Width; x++)
		{
			UINT index = desc.Width * y + x;

			CONST FLOAT f = 1.0f / 255.0f;
			float r = f * (float)((0xFF000000 & colors[index]) >> 24);
			float g = f * (float)((0x00FF0000 & colors[index]) >> 16);
			float b = f * (float)((0x0000FF00 & colors[index]) >> 8);
			float a = f * (float)((0x000000FF & colors[index]) >> 0);

			pixels->push_back(D3DXCOLOR(a, b, g, r));
		}
	}

	SAFE_DELETE_ARRAY(colors);
	SAFE_RELEASE(srcTexture);
	SAFE_RELEASE(texture);

	return desc;
}

void Texture::SetSamplerState(D3D11_SAMPLER_DESC desc)
{
	SAFE_RELEASE(samplerState);

	States::CreateSampler(&desc, &samplerState);
}

void Texture::SetShaderResource(UINT slot)
{
	D3D::GetDC()->PSSetShaderResources(slot, 1, &view);
}

void Texture::SetShaderSampler(UINT slot)
{
	D3D::GetDC()->PSSetSamplers(slot, 1, &samplerState);
}

void Texture::SetShaderResources(UINT slot, UINT count, Texture ** textures)
{
	vector<ID3D11ShaderResourceView *> views;
	for (UINT i = 0; i < count; i++)
		views.push_back(textures[i]->GetView());

	D3D::GetDC()->PSSetShaderResources(slot, count, &views[0]);
}

void Texture::SetBlankShaderResource(UINT slot)
{
	ID3D11ShaderResourceView* null[1]{ NULL };

	D3D::GetDC()->PSSetShaderResources(slot, 1, null);
}

void Texture::SetBlankSamplerState(UINT slot)
{
	ID3D11SamplerState* null[1]{ NULL };

	D3D::GetDC()->PSSetSamplers(slot, 1, null);
}

void Textures::Create()
{

}

void Textures::Delete()
{
	for (TextureDesc desc : descs)
		SAFE_RELEASE(desc.view);
}

void Textures::Load(Texture * texture, D3DX11_IMAGE_LOAD_INFO * loadInfo)
{
	HRESULT hr;

	D3DX11_IMAGE_INFO info;
	hr = D3DX11GetImageInfoFromFile
	(
		texture->file.c_str(), NULL, &info, NULL
	);
	assert(SUCCEEDED(hr));

	UINT width = info.Width;
	UINT height = info.Height;

	if (loadInfo != NULL)
	{
		width = loadInfo->Width;
		height = loadInfo->Height;
	}

	TextureDesc desc;
	desc.file = texture->file;
	desc.width = width;
	desc.height = height;

	TextureDesc exist;
	bool bExist = false;
	for (TextureDesc temp : descs)
	{
		if (desc == temp)
		{
			bExist = true;
			exist = temp;

			break;
		}
	}

	if (bExist == true)
	{
		texture->info = exist.info;
		texture->view = exist.view;
	}
	else
	{
		ID3D11ShaderResourceView* view;

		D3DX11_IMAGE_LOAD_INFO tempLoadInfo;
		if (loadInfo != NULL)
			tempLoadInfo = *loadInfo;

		D3DX11_IMAGE_INFO imageInfo;
		tempLoadInfo.pSrcInfo = &imageInfo;
		hr = D3DX11CreateShaderResourceViewFromFile
		(
			D3D::GetDevice(), texture->file.c_str(), &tempLoadInfo, NULL, &view, NULL
		);
		assert(SUCCEEDED(hr));

		desc.file = texture->file;
		desc.width = imageInfo.Width;
		desc.height = imageInfo.Height;
		desc.info = imageInfo;
		desc.view = view;

		texture->view = view;
		texture->info = imageInfo;

		descs.push_back(desc);
	}
}