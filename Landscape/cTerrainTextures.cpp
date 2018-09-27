#include "stdafx.h"
#include "cTerrainTextures.h"
#include "./Utilities/Path.h"
#include "./Utilities/String.h"

Landscape::cTerrainTextures::cTerrainTextures()
	:m_uMaxSize(5)
{
	D3D11_SAMPLER_DESC desc;
	States::GetSamplerDesc(&desc);
	States::CreateSampler(&desc, &m_pSamplerState);

	m_vecView.resize(m_uMaxSize, nullptr);
	m_vecPath.resize(m_uMaxSize, "");
}

Landscape::cTerrainTextures::~cTerrainTextures()
{
	for (auto& i : m_vecView)
		SAFE_RELEASE(i);

	SAFE_RELEASE(m_pSamplerState);
}

void Landscape::cTerrainTextures::SetShaderResources(UINT slot)
{
	if (m_vecView.empty() == false)
		D3D::GetDC()->PSSetShaderResources(slot, m_vecView.size(), &m_vecView[0]);
}

void Landscape::cTerrainTextures::SetSamplers(UINT slot)
{
	D3D::GetDC()->PSSetSamplers(slot, 1, &m_pSamplerState);
}

void Landscape::cTerrainTextures::PostRender()
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Splatting Texture");
	for (size_t i=0;i<m_vecView.size();i++)
	{
		string str = "Text0" + to_string(i);
		if (ImGui::Button(str.c_str()))
		{
			Path::OpenFileDialog(L"Open Texture", Path::ImageFilter
								 , Contents + L"./Landscape/", [this, i](wstring filePath)
			{
				SetTexture(filePath, i);
			});
		}
		ImGui::SameLine();
	}
}

void Landscape::cTerrainTextures::SetTexture(wstring fileName, UINT slot)
{
	string path = String::WStringToString(fileName);
	m_vecPath[slot] = path;

	auto tempView = m_vecView[slot];
	SAFE_RELEASE(tempView);

	ID3D11ShaderResourceView* view;
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), fileName.c_str(), NULL, NULL, &view, NULL
	);
	assert(SUCCEEDED(hr) && "텍스쳐 생성 오류!");

	m_vecView[slot] = view;
}

void Landscape::cTerrainTextures::ReadJson(const Json::Value& root)
{
	for (auto& i : m_vecView)
		SAFE_RELEASE(i);

	for (size_t i = 0; i < m_vecPath.size(); i++)
	{
		string text = "Texture" + to_string(i);
		auto str = root[text].asString();

		auto wStr = String::StringToWString(str);
		if (wStr == L"")
			continue;
		else
			SetTexture(wStr, i);
	}
}

void Landscape::cTerrainTextures::WriteOnJson(Json::Value* root)
{
	for (size_t i = 0; i < m_vecPath.size(); i++)
	{
		string text = "Texture" + to_string(i);
		(*root)[text] = m_vecPath[i];
	}
}
