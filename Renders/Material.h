#pragma once
#include "./Interfaces/IClonable.h"

class MaterialBuffer : public ShaderBuffer
{
public:
	MaterialBuffer()
		: ShaderBuffer(&Data, sizeof(Data))
	{
		Data.Ambient = D3DXCOLOR(0, 0, 0, 1);
		Data.Diffuse = D3DXCOLOR(1, 1, 1, 1);
		Data.Specular = D3DXCOLOR(1, 1, 1, 1);
		Data.Emissive = D3DXCOLOR(0, 0, 0, 1);

		Data.Shininess = 1;
		Data.DetailFactor = 1;
	}

	struct Struct
	{
		D3DXCOLOR Ambient;
		D3DXCOLOR Diffuse;
		D3DXCOLOR Specular;
		D3DXCOLOR Emissive;

		float Shininess;
		float DetailFactor;

		float Padding[2];
	} Data;
};

class Material : public IClonable
{
public:
	void Clone(void** clone) override;
public:
	Material(wstring file);
	~Material();

	UINT GetNumber() { return number; }
	void SetNumber(UINT number) { this->number = number; }

	wstring GetName() { return name; }
	void SetName(wstring val) { name = val; }

	Shader* GetShader() { return shader; }
	void SetShader(wstring file);

	MaterialBuffer* GetBuffer() { return buffer; }

	D3DXCOLOR* GetAmbient() { return &buffer->Data.Ambient; }
	D3DXCOLOR* GetDiffuse() { return &buffer->Data.Diffuse; }
	D3DXCOLOR* GetSpecular() { return &buffer->Data.Specular; }
	D3DXCOLOR* GetEmissive() { return &buffer->Data.Emissive; }

	float* GetShininess() { return &buffer->Data.Shininess; }
	float* GetDetailFactor() { return &buffer->Data.DetailFactor; }

	void SetAmbient(D3DXCOLOR val)
	{
		SetAmbient(val.r, val.g, val.b);
	}

	void SetDiffuse(D3DXCOLOR val)
	{
		SetDiffuse(val.r, val.g, val.b);
	}

	void SetSpecular(D3DXCOLOR val)
	{
		SetSpecular(val.r, val.g, val.b);
	}

	void SetEmissive(D3DXCOLOR val)
	{
		SetEmissive(val.r, val.g, val.b);
	}


	void SetAmbient(float r, float g, float b)
	{
		buffer->Data.Ambient = D3DXCOLOR(r, g, b, 1);
	}

	void SetDiffuse(float r, float g, float b)
	{
		buffer->Data.Diffuse = D3DXCOLOR(r, g, b, 1);
	}

	void SetSpecular(float r, float g, float b)
	{
		buffer->Data.Specular = D3DXCOLOR(r, g, b, 1);
	}

	void SetEmissive(float r, float g, float b)
	{
		buffer->Data.Emissive = D3DXCOLOR(r, g, b, 1);
	}

	void SetShininess(float val)
	{
		buffer->Data.Shininess = val;
	}

	void SetDetailFactor(float val)
	{
		buffer->Data.DetailFactor = val;
	}

	Texture* GetDiffuseMap() { return diffuseMap; }
	Texture* GetSpecularMap() { return specularMap; }
	Texture* GetEmissiveMap() { return emissiveMap; }
	Texture* GetNormalMap() { return normalMap; }
	Texture* GetDetailMap() { return detailMap; }


	void SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetEmissiveMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetEmissiveMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetDetailMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDetailMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void SetBuffer();

private:
	UINT number;
	wstring name;
	
	Shader* shader;

	MaterialBuffer* buffer;

	Texture* diffuseMap;
	Texture* specularMap;
	Texture* emissiveMap;
	Texture* normalMap;
	Texture* detailMap;
};