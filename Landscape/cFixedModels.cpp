#include "stdafx.h"
#include "cFixedModels.h"
#include "cBillboard.h"
#include "TerrainBuffer.h"
#include "./Model/BinModel.h"
#include "./Model/MeshPart.h"
#include "./Utilities/Math.h"
#include "./Utilities/String.h"
#include "./Utilities/Path.h"

Landscape::cFixedModels::cFixedModels(function<float(D3DXVECTOR3)> funcGetHeight)
	:GetHeight(funcGetHeight)
	, m_iCount(0)
	, m_pCurModel(nullptr)
{
	
}

Landscape::cFixedModels::~cFixedModels()
{
	for (auto& billboard : m_vecBillboard)
		SAFE_DELETE(billboard);
	for (auto& model : m_vecModel)
		SAFE_DELETE(model);
}

void Landscape::cFixedModels::Update()
{
	for (auto& billboard : m_vecBillboard)
		billboard->SetPosition(GetHeight);

	for (auto& billboard : m_vecBillboard)
		billboard->Update();
}

void Landscape::cFixedModels::Render()
{
	for (const auto& model : m_vecBillboard)
		model->Render();
}

void Landscape::cFixedModels::PostRender()
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Model");
	ImGui::SliderInt("InsertionCount", &m_iCount, 0, 20);

	//모델 생성 후 풀에 삽입
	if (ImGui::Button("Select Billboard"))
	{
		Path::OpenFileDialog(L"Billboard", Path::ImageFilter, Textures, [this](wstring path)
		{
			CreateBinModel(path);
		});
	}
}

void Landscape::cFixedModels::InsertBillboardInArea(BrushBuffer * pBrush)
{
	if (m_pCurModel == nullptr)
		return;

	//m_icount만큼 반복
	for (int i = 0; i < m_iCount; i++)
	{
		//랜덤 길이, 랜덤 라디안
		float radian = Math::ToRadian((float)Math::Random(0, 360));
		float length = Math::Random(0.0f, (float)pBrush->Data.BrushSize);

		//크기 설정
		D3DXVECTOR3 scale;
		scale.x = (float)Math::Random(5, 10);
		scale.y = (float)Math::Random(5, 10);
		
		//위치 설정
		D3DXVECTOR3 position;
		position.x = length * cosf(radian) 
			+ pBrush->Data.BrushPos.x;
		position.y = GetHeight(position) + (scale.y * 0.5f);
		position.z = length * sinf(radian)
			+ pBrush->Data.BrushPos.y;

		auto billboard = new Landscape::cBillboard(m_pCurModel);
		billboard->SetPosition(position.x, position.y, position.z);
		billboard->SetScale(scale.x, scale.y);

		m_vecBillboard.push_back(billboard);
	}
}

void Landscape::cFixedModels::CreateBinModel(wstring filePath)
{
	m_pCurModel = GetModel(filePath);

	if (!m_pCurModel)
	{
		Fbx::FbxModels::ReadBinary(BinModels + L"Quad.model", &m_pCurModel);

		Material* material = m_pCurModel->GetMeshPart(0)->GetMaterial();
		material->SetShader(Shaders + L"025_Billboard.hlsl");
		material->SetDiffuseMap(filePath);

		m_vecModel.push_back(m_pCurModel);
	}
}

void Landscape::cFixedModels::ReadJson(const Json::Value & root)
{
	auto jBillboard = root["Billboard"];
	for (auto& model : m_vecModel)
		SAFE_DELETE(model);
	m_vecModel.clear();

	for (auto& bill : m_vecBillboard)
		SAFE_DELETE(bill);
	m_vecBillboard.clear();

	for (auto model : jBillboard["BinModel"])
	{
		CreateBinModel(String::StringToWString(model.asString()));
	}

	for (auto model : jBillboard["BillboardInfo"])
	{
		string path = model["Path"].asString();
		D3DXVECTOR3 position;
		position.x = model["PosX"].asFloat();
		position.y = model["PosY"].asFloat();
		position.z = model["PosZ"].asFloat();

		D3DXVECTOR3 scale;
		scale.x = model["ScaleX"].asFloat();
		scale.y = model["ScaleY"].asFloat();

		auto iter = find_if(m_vecModel.begin(), m_vecModel.end(), [path](Bmodel* model)
		{
			return model->GetMeshPart(0)->GetMaterial()->GetDiffuseMap()->GetFile() == String::StringToWString(path);
		});

		auto billboard = new Landscape::cBillboard(*iter);
		billboard->SetPosition(position.x, position.y, position.z);
		billboard->SetScale(scale.x, scale.y);

		m_vecBillboard.push_back(billboard);
	}
}

void Landscape::cFixedModels::WriteOnJson(Json::Value * root)
{
	Json::Value billboard;

	Json::Value binModel;
	for (auto model : m_vecModel)
	{
		string str = String::WStringToString(model->GetMeshPart(0)->GetMaterial()->GetDiffuseMap()->GetFile());
		binModel.append(str);
	}
	billboard["BinModel"] = binModel;

	Json::Value billboardModel;
	for (auto jBillboard : m_vecBillboard)
	{
		Json::Value billModel;
		billModel["Path"] = String::WStringToString(jBillboard->GetMeshPart(0)->GetMaterial()->GetDiffuseMap()->GetFile());
		billModel["PosX"] = jBillboard->GetPosition().x;
		billModel["PosY"] = jBillboard->GetPosition().y;
		billModel["PosZ"] = jBillboard->GetPosition().z;
		billModel["ScaleX"] = jBillboard->GetScaling().x;
		billModel["ScaleY"] = jBillboard->GetScaling().y;

		billboardModel.append(billModel);
	}

	billboard["BillboardInfo"] = billboardModel;
	(*root)["Billboard"] = billboard;
}

Model::BinModel* Landscape::cFixedModels::GetModel(wstring filePath)
{
	auto iter = find_if(m_vecModel.begin(), m_vecModel.end(), [=](Bmodel* model)
	{
		return model->GetMeshPart(0)->GetMaterial()->GetDiffuseMap()->GetFile() == filePath;
	});

	if (iter != m_vecModel.end())
		return *iter;

	return nullptr;
}

void Landscape::cFixedModels::Function()
{
}
