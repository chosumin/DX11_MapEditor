#include "stdafx.h"
#include "ModelManager.h"
#include "./Model/FbxModels.h"
#include "./Model/BinModel.h"
#include "./Utilities/Path.h"
#include "./Utilities/String.h"
#include "./Command/Command.h"
#include "./JsonParser/JsonParser.h"

Model::ModelManager::ModelManager(CommandManager* pCommand)
	: selectedFbxFile(L""), o_Command(pCommand)
	, jsonFilePath("")
{
	//¾À ÀúÀå ÄÝ¹é
	auto func = bind(&Model::ModelManager::SaveScene
				, this, placeholders::_1);
	m_vecFunc.push_back(func);

	//¾À ºÒ·¯¿À±â ÄÝ¹é
	func = bind(&Model::ModelManager::LoadScene
				, this, placeholders::_1);
	m_vecFunc.push_back(func);
}


Model::ModelManager::~ModelManager()
{
	for (auto& model : vecModel)
		SAFE_DELETE(model);
}

void Model::ModelManager::Update()
{
	for (auto& model : vecModel)
		model->Update();
}

void Model::ModelManager::Render()
{
	for (auto& model : vecModel)
		model->Render();
}

void Model::ModelManager::PostRender()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Model"))
		{
			if (ImGui::MenuItem("Convert", ""))
				OpenConvertDialog();

			ImGui::Separator();

			if (ImGui::MenuItem("Model Open", ""))
				OpenModelDialog();

			ImGui::Separator();

			//json ÀúÀå
			if (ImGui::MenuItem("Save", ""))
				Path::SaveFileDialog(L"SceneName", L"*.json", Assets, m_vecFunc[0]);

			if (ImGui::MenuItem("Load", ""))
			{
				o_Command->Destroy();
				Path::OpenFileDialog(L"SceneName", L"*.json", Assets, m_vecFunc[1]);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void Model::ModelManager::RegisterObserver(iObserver * observer)
{
	o_pObserver = observer;
}

void Model::ModelManager::NotifyObservers()
{
	o_pObserver->UpdateBySubject();
}

void Model::ModelManager::LoadScene(wstring file)
{
	NotifyObservers();

	for (auto& model : vecModel)
	{
		SAFE_DELETE(model);
	}
	vecModel.clear();

	if (file == L"")
		return;

	Json::Value root;
	jsonFilePath = String::WStringToString(file);
	JsonParser::Get()->ParseValue(jsonFilePath, &root);

	Json::Value models = root["Models"];
	for (auto element : models)
	{
		Json::Value fbxInfo = element["FBXinfo"];

		//Bin¸ðµ¨ »ý¼º
		string path = fbxInfo["FilePath"].asString();
		wstring tempWstr = String::StringToWString(path);
		BinModel* model = nullptr;
		GenerateModel(&model, BinModels + tempWstr + L".model");

		//Bin¸ðµ¨ Á¤º¸ ºÒ·¯¿À±â
		model->LoadJson(&fbxInfo);

		model->Update();
	}//for(root)
}

void Model::ModelManager::SaveScene(wstring file)
{
	Json::Value root;

	Json::Value models;
	for (auto model : vecModel)
	{
		//¹è¿­
		Json::Value modelVal;

		//¸ðµ¨ Á¤º¸ ÀúÀå
		model->SaveJson(&modelVal);

		models.append(modelVal);
	}//for(m_vecModel)
	root["Models"] = models;

	jsonFilePath = String::WStringToString(file);
	JsonParser::Get()->WriteOnFile(jsonFilePath, &root);
}

void Model::ModelManager::GenerateModel(BinModel** model, wstring file)
{
	string strFile = String::WStringToString(Path::GetFileNameWithoutExtension(file));
	auto findIter = find_if(vecModel.begin(), vecModel.end(), [=](Model::BinModel* model)
	{
		return model->GetName() == strFile;
	});

	//Ã³À½ »ðÀÔµÇ´Â ¸ðµ¨
	if (findIter == vecModel.end())
		Fbx::FbxModels::ReadBinary(file, model);
	else
		*model = new Model::BinModel(**findIter);

	vecModel.push_back(*model);
}

void Model::ModelManager::OpenConvertDialog(wstring file)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() < 1)
	{
		function<void(wstring)> f;
		f = bind(&ModelManager::OpenConvertDialog, this, placeholders::_1);

		Path::OpenFileDialog(L"", Path::FbxModelFilter, Assets, f, desc.Handle);

		return;
	}

	selectedFbxFile = file;
	{
		wstring fileName = Path::GetFileNameWithoutExtension(file);

		function<void(wstring)> f;
		f = bind(&ModelManager::Convert, this, placeholders::_1);

		Path::SaveFileDialog(fileName, Path::BinModelFilter, BinModels, f, desc.Handle);
	}
	selectedFbxFile = L"";
}

void Model::ModelManager::Convert(wstring file)
{
	if (selectedFbxFile.length() < 1)
		return;

	wstring saveFile = file + L".model";
	Fbx::FbxModels::Convert(selectedFbxFile, saveFile);

	D3DDesc desc;
	D3D::GetDesc(&desc);

	MessageBox(desc.Handle, L"º¯È¯ ¿Ï·á!!", L"¼º°ø", MB_OK);
}

void Model::ModelManager::OpenModelDialog(wstring file)
{
	if (file.length() < 1)
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		function<void(wstring)> f;
		f = bind(&ModelManager::OpenModelDialog, this, placeholders::_1);

		Path::OpenFileDialog(L"", Path::BinModelFilter, BinModels, f, desc.Handle);

		return;
	}

	BinModel* model = nullptr;
	GenerateModel(&model, file);
}
