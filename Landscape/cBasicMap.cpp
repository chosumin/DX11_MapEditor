#include "stdafx.h"
#include "cBasicMap.h"
#include "./Utilities/Path.h"
#include "./Utilities/String.h"

Landscape::cBasicMap::cBasicMap(wstring file, float heightRatio)
	: width(255)
	, height(255)
	, vertexCount(0)
	, indexCount(0)
	, vertexBuffer(nullptr)
	, indexBuffer(nullptr)
	, vertexData(nullptr)
	, indexData(nullptr)
	, heightMap(nullptr)
	, heightRatio(heightRatio)
{
	FillvertexData(heightRatio);
	FillNormalcBasicMap();
	CreateBuffer();
}

Landscape::cBasicMap::~cBasicMap()
{
	Clear();
}

bool Landscape::cBasicMap::WriteOnJson(OUT Json::Value * root)
{
	wstring heightStr, alphaStr;
	Path::SaveFileDialog(L"SaveHeightMap", Path::ImageFilter, Textures, [this, &heightStr](wstring path)
	{
		heightStr = path;
		SaveMap(0, path);
	});

	Path::SaveFileDialog(L"SaveAlphaMap", Path::ImageFilter, Textures, [this, &alphaStr](wstring path)
	{
		alphaStr = path;
		SaveMap(1, alphaStr);
	});

	if (root != nullptr)
	{
		(*root)["HeightMap"] = String::WStringToString(heightStr);
		(*root)["AlphaMap"] = String::WStringToString(alphaStr);
	}

	return true;
}

bool Landscape::cBasicMap::ReadJson(IN const Json::Value & root)
{
	auto heightStr = root["HeightMap"].asString();
	auto alphaStr = root["AlphaMap"].asString();

	SetHeightMapFile(String::StringToWString(heightStr));
	ReadSplattedMap(String::StringToWString(alphaStr));

	UpdateVertexData();

	return true;
}

float Landscape::cBasicMap::GetHeight(D3DXVECTOR3 position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width)
		return 0.0f;

	if (z < 0 || z >= height)
		return 0.0f;

	UINT index0 = (width + 1) * z + x;
	UINT index1 = (width + 1) * (z + 1) + x;
	UINT index2 = (width + 1) * z + x + 1;
	UINT index3 = (width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 v0 = vertexData[index0].position;
	D3DXVECTOR3 v1 = vertexData[index1].position;
	D3DXVECTOR3 v2 = vertexData[index2].position;
	D3DXVECTOR3 v3 = vertexData[index3].position;

	float deltaX = position.x - v0.x;
	float deltaZ = position.z - v0.z;

	D3DXVECTOR3 temp;
	if (deltaX + deltaZ <= 1)
		temp = v0 + (v2 - v0) * deltaX + (v1 - v0) * deltaZ;
	else
	{
		deltaX = 1 - deltaX;
		deltaZ = 1 - deltaZ;

		temp = v3 + (v2 - v3) * deltaX + (v1 - v3) * deltaZ;
	}

	return temp.y;
}

void Landscape::cBasicMap::SetHeightMapFile(wstring file)
{
	if (file.length() < 0) 
		return;
	Clear();
	heightMap = new Texture(file);

	FillvertexData(heightRatio);
	FillNormalcBasicMap();

	CreateBuffer();
}

void Landscape::cBasicMap::UpdateVertexData(D3D11_BOX * box)
{
	FillNormalcBasicMap();

	D3D::GetDC()->UpdateSubresource
	(
		vertexBuffer, 0, box, vertexData, sizeof(VertexType) * vertexCount, 0
	);
}

void Landscape::cBasicMap::Render()
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

bool Landscape::cBasicMap::ReadSplattedMap(wstring filePath)
{
	SAFE_DELETE(heightMap);
	heightMap = new Texture(filePath);

	if (heightMap)
	{
		vector<D3DXCOLOR> pixels;
		D3D11_TEXTURE2D_DESC desc = heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &pixels);

		UINT index = 0;
		for (UINT z = 0; z <= height; z++)
		{
			for (UINT x = 0; x <= width; x++)
			{
				vertexData[index].color = pixels[index];
				index++;
			}//for(x)
		}//for(y)	
	}

	return true;
}

void Landscape::cBasicMap::Clear()
{
	SAFE_DELETE(heightMap);

	SAFE_DELETE_ARRAY(vertexData);
	SAFE_DELETE_ARRAY(indexData);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

void Landscape::cBasicMap::FillvertexData(float heightRatio)
{
	vector<D3DXCOLOR> pixels;
	if (heightMap)
	{
		D3D11_TEXTURE2D_DESC desc = heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &pixels);

		width = desc.Width - 1;
		height = desc.Height - 1;
	}

	vertexCount = (width + 1) * (height + 1);
	vertexData = new VertexType[vertexCount];

	UINT index = 0;
	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			vertexData[index].position.x = (float)x;
			vertexData[index].position.y = heightMap != nullptr ? (float)(pixels[index].r * 255.0f) / heightRatio : 0;
			vertexData[index].position.z = (float)z;

			vertexData[index].uv.x = x / (float)width;
			vertexData[index].uv.y = z / (float)height;

			vertexData[index].normal = D3DXVECTOR3(0, 0, 0);

			index++;
		}//for(x)
	}//for(y)	


	indexCount = width * height * 6;
	indexData = new UINT[indexCount];

	index = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			indexData[index + 0] = (width + 1) * z + x;
			indexData[index + 1] = (width + 1) * (z + 1) + x;
			indexData[index + 2] = (width + 1) * z + x + 1;
			indexData[index + 3] = (width + 1) * z + x + 1;
			indexData[index + 4] = (width + 1) * (z + 1) + x;
			indexData[index + 5] = (width + 1) * (z + 1) + (x + 1);

			index += 6;
		}
	}
}

void Landscape::cBasicMap::FillNormalcBasicMap()
{
	for (UINT i = 0; i < (indexCount / 3); i++)
	{
		UINT index0 = indexData[i * 3 + 0];
		UINT index1 = indexData[i * 3 + 1];
		UINT index2 = indexData[i * 3 + 2];

		VertexType v0 = vertexData[index0];
		VertexType v1 = vertexData[index1];
		VertexType v2 = vertexData[index2];

		D3DXVECTOR3 d1 = v1.position - v0.position;
		D3DXVECTOR3 d2 = v2.position - v0.position;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &d1, &d2);

		vertexData[index0].normal += normal;
		vertexData[index1].normal += normal;
		vertexData[index2].normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertexData[i].normal, &vertexData[i].normal);
}

void Landscape::cBasicMap::CreateBuffer()
{
	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexType) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA cBasicMap = { 0 };
		cBasicMap.pSysMem = vertexData;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &cBasicMap, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA cBasicMap = { 0 };
		cBasicMap = { 0 };
		cBasicMap.pSysMem = indexData;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateBuffer(&desc, &cBasicMap, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Landscape::cBasicMap::SaveMap(int type, wstring fileName)
{
	UINT* color = new UINT[(width + 1) * (height + 1)];

	UINT count = 0;

	for (int y = 0; y < height + 1; y++)
	{
		for (int x = 0; x < width + 1; x++)
		{
			UINT r, g, b, a;
			int index = (width + 1) * y + x;

			//높이맵 저장
			if (type == 0)
			{
				float c = (vertexData[index].position.y * 7.5f) / 255.0f;
				r = (UINT)(c * 255.0f) << 24;
				g = (UINT)(c * 255.0f) << 16;
				b = (UINT)(c * 255.0f) << 8;
				a = (UINT)(c * 255.0f) << 0;
				color[index] = a + b + g + r;
			}
			//알파맵 저장
			else
			{
				auto c = vertexData[index].color;
				r = (UINT)(c.r * 255.0f) << 0;
				g = (UINT)(c.g * 255.0f) << 8;
				b = (UINT)(c.b * 255.0f) << 16;
				a = (UINT)(c.a * 255.0f) << 24;
				color[index] = (UINT)c;
			}
		}
	}

	D3D11_TEXTURE2D_DESC temp;
	ZeroMemory(&temp, sizeof(D3D11_TEXTURE2D_DESC));
	temp.Width = width + 1;
	temp.Height = height + 1;
	temp.MipLevels = 1; //밉맵 기본은 8장
	temp.ArraySize = 1; //나중에 설명
	temp.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	temp.SampleDesc.Count = 1;
	temp.SampleDesc.Quality = 0;
	//중요
	temp.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //cpu에서 읽을 수 있도록
	temp.Usage = D3D11_USAGE_STAGING; //cpu에서 읽기 접근만

	ID3D11Texture2D* texture = nullptr;
	HRESULT hr = D3D::GetDevice()->CreateTexture2D(&temp, NULL, &texture);
	assert(SUCCEEDED(hr));

	D3D11_MAPPED_SUBRESOURCE map;

	D3D::GetDC()->Map(texture, NULL, D3D11_MAP_WRITE, NULL, &map);
	{
		memcpy(map.pData, color, sizeof(UINT) * (width + 1) * (height + 1));
	}
	D3D::GetDC()->Unmap(texture, 0);

	hr = D3DX11SaveTextureToFile(D3D::GetDC(),
								 texture, D3DX11_IFF_PNG, (LPWSTR)fileName.c_str());
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(color);
}
